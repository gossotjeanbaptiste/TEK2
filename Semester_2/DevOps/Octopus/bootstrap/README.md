# G-DOP-400-NCY-4-1-OCTOPUS-3


- What is Ansible?
    - Outil d’automatisation et de gestion de configuration, sans agent, souvent via SSH.
- What is an Ansible playbook?
    - Playbook Ansible : fichier YAML qui décrit les tâches à exécuter sur des machines.
- Why using Ansible instead of just bare shell scripts?
    - Pourquoi Ansible plutôt que des scripts shell : plus lisible, idempotent, scalable, et plus facile à maintenir.
- In what way is Ansible different than Jenkins?
    - Différence avec Jenkins : Ansible automatise la config et le déploiement, Jenkins orchestre surtout des pipelines CI/CD.
- Can octopuses help you in your DevOps journey? (octocats do not count)
    - Octopodes : oui, surtout pour l’image. Beaucoup de bras, donc bon symbole pour faire plein de choses en même temps.

## Step 1 - Taking inventory

### 1) Créer l'inventaire
On crée un fichier d'inventaire Ansible nommé `my-inventory` avec 2 groupes :
- `web` : `web-1`, `web-2`
- `redis` : `cache-1`

Contenu déjà ajouté dans le repo :
```ini
[web]
web-1 ansible_host=192.168.56.11
web-2 ansible_host=192.168.56.12

[redis]
cache-1 ansible_host=192.168.56.13

[all:vars]
ansible_user=vboxuser
ansible_ssh_private_key_file=~/.ssh/id_ed25519
ansible_python_interpreter=/usr/bin/python3
```

### 2) Vérifier que les hôtes répondent
```bash
ansible -i my-inventory web -m ping
ansible -i my-inventory redis -m ping
ansible -i my-inventory all -m ping
```

### 3) Exécuter `uptime` sur les 3 machines en même temps
```bash
ansible -i my-inventory all -a "uptime"
```

### 4) Redémarrer toutes les machines simultanément
Le module `reboot` est prévu pour ça :
```bash
ansible -i my-inventory all -b -m reboot
```

### 5) Installer `htop` partout puis `nginx` seulement sur `web`
```bash
ansible -i my-inventory all -b -m apt -a "name=htop state=present update_cache=yes"
ansible -i my-inventory web -b -m apt -a "name=nginx state=present update_cache=yes"
```

### 6) Désinstaller `htop`
```bash
ansible -i my-inventory all -b -m apt -a "name=htop state=absent autoremove=yes"
```

### 7) Démarrer Nginx au boot avec le module `service`
```bash
ansible -i my-inventory web -b -m service -a "name=nginx state=started enabled=yes"
```

### Option recommandée : version playbook (idempotente)
Si tu veux éviter les commandes à la main, crée un playbook `step1.yml` :

```yaml
- name: Install htop everywhere
    hosts: all
    become: true
    tasks:
        - name: Install htop
            apt:
                name: htop
                state: present
                update_cache: true

- name: Install and enable nginx on web servers
    hosts: web
    become: true
    tasks:
        - name: Install nginx
            apt:
                name: nginx
                state: present

        - name: Ensure nginx is enabled and running
            service:
                name: nginx
                state: started
                enabled: true
```

Exécution :
```bash
ansible-playbook -i my-inventory step1.yml
```

