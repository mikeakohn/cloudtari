
default:
	microk8s.kubectl create -f cloudtari.yaml
	sleep 5
	microk8s.kubectl get pods

import:
	docker save cloudtari > cloudtari.tar
	microk8s.ctr --namespace k8s.io image import cloudtari.tar
	rm cloudtari.tar
	microk8s ctr images ls

query:
	microk8s.kubectl get pods

describe:
	microk8s.kubectl describe pod cloudtari

delete:
	#sudo microk8s.kubectl delete pod cloudtari
	microk8s.kubectl delete -f cloudtari.yaml

