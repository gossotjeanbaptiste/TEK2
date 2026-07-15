# Installer le manager de vm

```bash
sudo snap install multipass
```
# Lancez les vm 

```bash
multipass launch --name web-1 24.04
multipass launch --name web-2 24.04
multipass launch --name cache-1 24.04
```
# Liste des VM

```bash
multipass list
```

#Mettre les cles ssh
```bash
ssh-copy-id ubuntu@<IP_VM_1>
ssh-copy-id ubuntu@<IP_VM_2>
ssh-copy-id ubuntu@<IP_VM_3>
```

```bash
ansible-playbook -i hosts.ini setup_vm.yml --ask-pass --ask-become-pass
```

## Test de connexion

```bash
ansible debian_vms -i hosts.ini -m ping
```
