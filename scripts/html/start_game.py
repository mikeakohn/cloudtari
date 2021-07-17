#!/usr/bin/env python3

import subprocess
import json
import os, sys
import time

max_clients = 8

def remove_completed(data):
  terminated = 0

  for item in data["items"]:

    container_status = item["status"]["containerStatuses"][0]

    if not "cloudtari" in container_status["image"]: continue

    state = container_status["state"]
    pod_name = item["metadata"]["name"]
    job_name = item["metadata"]["labels"]["job-name"]
    port = item["spec"]["containers"][0]["command"][3]

    print("port=" + port)

    if "terminated" in state:
      os.system("microk8s.kubectl delete pod " + pod_name)
      os.system("microk8s.kubectl delete job " + job_name)

      terminated += 1

  return terminated

def get_next_port(data):
  port = 8080

  ports = []

  for item in data["items"]:
    ports.append(int(item["spec"]["containers"][0]["command"][3]))

  while True:
    if port not in ports: break
    port += 1
    if port == 9000: return 0

  return port

def start_job(rom, port):
  yaml = """apiVersion: batch/v1
kind: Job
metadata:
 name: cloudtariMETA
spec:
  template:
    metadata:
      name: cloudtari
    spec:
      containers:
      - name: cloudtari
        image: localhost:32000/cloudtari:local
        imagePullPolicy: Always
        command: ["/root/cloudtari", "/root/ROM", "http", "PORT"]
      hostNetwork: true
      restartPolicy: Never
"""

  yaml = yaml.replace("META", str(port))
  yaml = yaml.replace("PORT", str(port))
  yaml = yaml.replace("ROM", rom)

  #print(yaml)

  fp = open("/tmp/cloudtari.yaml", "w")
  fp.write(yaml)
  fp.close()

  os.system("microk8s.kubectl apply -f /tmp/cloudtari.yaml")

def get_address(data, port):
  for item in data["items"]:
    if int(item["spec"]["containers"][0]["command"][3]) == port:
      host_ip = item["status"]["hostIP"]
      print(str(port) + " " + item["status"]["hostIP"])
      return host_ip + ":" + str(port)

  return "problem:0"

# ----------------------------- fold here ---------------------------------

if len(sys.argv) < 2:
  print("Usage: python start_game.py <romfile>")
  exit(0)

rom = sys.argv[1]

result = subprocess.run(['microk8s.kubectl', 'get', 'pods', '-o', 'json'], stdout=subprocess.PIPE)

data = json.loads(result.stdout)

terminated = remove_completed(data)

current_clients = len(data["items"]) - terminated

if current_clients >= max_clients:
  print("server full")
  sys.exit(1)

print("current_clients=" + str(current_clients))

# This part assumes the PHP script is locked so only one process can
# call this script.
port = get_next_port(data)

print("port=" + str(port))

start_job(rom, port)

# Need to pause so the job is done being created.
time.sleep(3)

result = subprocess.run(['microk8s.kubectl', 'get', 'pods', '-o', 'json'], stdout=subprocess.PIPE)
data = json.loads(result.stdout)

address = get_address(data, port)

print("address=" + address)

