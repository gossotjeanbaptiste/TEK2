# Procédure de bootstrap — Réseau et accès SSH pour 3 VM Debian 13

Ce README décrit, pas-à-pas, la procédure que nous avons suivie pour préparer 3 machines virtuelles Debian 13 utilisables par Ansible (SSH sans mot de passe, réseau privé host-only, IP statique).

Tous les exemples sont en français et supposent que vous utilisez VirtualBox en local. Adaptez les IP et noms d'interfaces si besoin.

## Résumé rapide
- 3 VM Debian 13
- Adapter 1 : NAT (accès Internet, port-forward si nécessaire)
- Adapter 2 : Host-only (`vboxnet0`) pour le réseau privé (192.168.56.0/24)
- IP statiques sur la 2e interface (ex. 192.168.56.11/12/13)
- SSH sans mot de passe : `ssh-keygen` + `ssh-copy-id`

## 1 — Préparer ta machine hôte
1. Installer VirtualBox (ou utiliser un cloud provider).
2. Installer Ansible sur ta machine hôte (si tu veux piloter depuis ta machine) :

```bash
sudo apt update
sudo apt install ansible
ansible --version
ansible-playbook --version
```

## 2 — Créer les 3 VM
- Crée 3 VM Debian 13 nommées par exemple `deb1`, `deb2`, `deb3`.
- Pour chaque VM, configure :
  - Adapter 1 : Attached to `NAT` (cocher "Cable connected").
  - Adapter 2 : Attached to `Host-only Adapter` et choisis `vboxnet0` (si vide, créer le host-only network via Tools > Host Network Manager).

Si tu veux un accès SSH depuis l'hôte sans utiliser l'IP host-only, tu peux ajouter un port forwarding dans Adapter 1 (NAT): Host Port -> Guest Port 22.

## 3 — Installer OpenSSH sur chaque VM
Sur chaque VM :
```bash
sudo apt update
sudo apt install openssh-server
sudo systemctl enable --now ssh
sudo systemctl status ssh
```

## 4 — Générer et copier ta clé SSH
Sur ta machine hôte (ou poste de contrôle) :
```bash
ssh-keygen -t ed25519 -f ~/.ssh/id_ed25519 -C "ansible"
```
Puis pour chaque VM :
```bash
ssh-copy-id -i ~/.ssh/id_ed25519.pub user@IP_VM
```
Test : `ssh user@IP_VM` doit ouvrir une session sans mot de passe.

## 5 — Configurer l'interface host-only dans VirtualBox
Si `Name` est vide dans Adapter 2, crée un host-only network : VirtualBox → File/Tools → Host Network Manager → Create. Tu auras une interface `vboxnet0` sur l'hôte.

## 6 — Attribuer une IP statique permanente (Debian 13, sans NetworkManager)
Modifier `/etc/network/interfaces` sur chaque VM.

Exemple pour `deb1` (`/etc/network/interfaces`):
```ini
source /etc/network/interfaces.d/*

auto lo
iface lo inet loopback

# NAT (carte principale)
allow-hotplug enp0s3
iface enp0s3 inet dhcp

# Host-only (carte 2) — IP statique
allow-hotplug enp0s8
iface enp0s8 inet static
    address 192.168.56.11
    netmask 255.255.255.0

# Ne pas mettre de gateway sur l'interface host-only
```

Pour `deb2` : `address 192.168.56.12` ; pour `deb3` : `address 192.168.56.13`.

Appliquer sans redémarrer :
```bash
sudo ifdown enp0s8 2>/dev/null || true
sudo ifup enp0s8
ip a show enp0s8
```
Si `ifup` se plaint, vérifie les fautes de frappe (ex. `address` pas `adress`).

Si nécessaire, redémarre la VM :
```bash
sudo reboot
```

## 7 — Vérifications et debug
- `ip a` : confirmer que l'IP est bien sur `enp0s8`.
- `ping 192.168.56.12` depuis `deb1` pour tester la connectivité.
- Si tu obtiens des `DUP!` ou des réponses étranges, vérifie :
  - que deux VM n'ont pas la même IP (conflit d'IP),
  - que l'IP est bien sur la bonne interface (`enp0s8`),
  - que `vboxnet0` existe et que l'Adapter 2 est connecté.
- Pour les logs du service réseau :
```bash
systemctl status networking
journalctl -xeu networking
```

## 8 — Exemple d'inventaire Ansible simple
Crée un fichier `hosts` pour Ansible :
```ini
[octopus]
192.168.56.11 ansible_user=vboxuser
192.168.56.12 ansible_user=vboxuser
192.168.56.13 ansible_user=vboxuser

[octopus:vars]
ansible_ssh_private_key_file=~/.ssh/id_ed25519
```

Utilisation :
```bash
ansible -i hosts -m ping all
ansible-playbook -i hosts playbook.yml
```

## 9 — Conseils pratiques
- Utilise un cloud provider si tu veux éviter la configuration host-only locale.
- Préfère `ed25519` pour les clés SSH.
- N'ajoute pas de gateway sur l'interface host-only.

---
Fichier créé automatiquement. Si tu veux, je peux générer aussi le fichier `hosts` Ansible dans le repo.
