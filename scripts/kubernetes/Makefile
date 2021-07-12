
default:
	sudo microk8s.kubectl create -f cloudtari.yaml
	#sudo microk8s.kubectl apply -f cloudtari.yaml
	sleep 5
	sudo microk8s.kubectl get pods

import:
	sudo docker save cloudtari > cloudtari.tar
	sudo microk8s.ctr --namespace k8s.io image import cloudtari.tar
	sudo rm cloudtari.tar
	sudo microk8s ctr images ls

query:
	sudo microk8s.kubectl get pods

describe:
	sudo microk8s.kubectl describe pod cloudtari

delete:
	#sudo microk8s.kubectl delete pod cloudtari
	sudo microk8s.kubectl delete -f cloudtari.yaml

