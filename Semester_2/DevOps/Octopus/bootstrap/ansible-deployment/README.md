# Step 2 - Role-playing development with Ansible

Ce répertoire contient l'implémentation complète du Step 2 du projet G-DOP-400 avec Ansible.

## Structure

```
ansible-deployment/
├── playbook.yml                 # Playbook principal orchestrant tous les rôles
├── ansible.cfg                  # Configuration Ansible
├── roles/
│   ├── base/                    # Rôle de configuration de base
│   │   ├── tasks/main.yml       # Tâches de base
│   │   ├── defaults/main.yml    # Variables par défaut
│   │   └── handlers/main.yml    # Gestionnaires d'événements
│   ├── redis/                   # Rôle Redis
│   │   ├── tasks/main.yml       # Installation Redis
│   │   ├── defaults/main.yml    # Configuration Redis
│   │   └── handlers/main.yml    # Redémarrage du service
│   ├── nodejs/                  # Rôle Node.js
│   │   ├── tasks/main.yml       # Installation Node.js et npm
│   │   ├── defaults/main.yml    # Configuration Node.js
│   │   └── handlers/main.yml    # Gestionnaires d'événements
│   └── deploy/                  # Rôle de déploiement
│       ├── tasks/main.yml       # Déploiement de l'application
│       ├── defaults/main.yml    # Configuration du déploiement
│       ├── files/               # Fichiers à copier (service systemd)
│       └── handlers/main.yml    # Gestionnaires d'événements
```

## Rôles Détaillés

### 1. Rôle `base`
**Exécuté sur:** Tous les hôtes (`all`)

**Tâches:**
- ✅ Mise à jour du cache apt
- ✅ Upgrade de tous les packages
- ✅ Installation des packages essentiels (apt-transport-https, ca-certificates, git, curl, unzip, zsh, etc.)
- ✅ Configuration du fuseau horaire (Europe/Paris)
- ✅ Configuration de zsh comme shell root
- ✅ Configuration SSH si nécessaire

**Variables:**
- `timezone`: Fuseau horaire (default: Europe/Paris)
- `essential_packages`: Liste des packages à installer
- `root_shell`: Shell root (default: /bin/zsh)

### 2. Rôle `redis`
**Exécuté sur:** Groupe `redis`

**Tâches:**
- ✅ Mise à jour du cache apt
- ✅ Installation du serveur Redis
- ✅ Démarrage et activation du service Redis
- ✅ Vérification que Redis est actif sur le port 6379
- ✅ Affichage de la version Redis

**Variables:**
- `redis_port`: Port Redis (default: 6379)
- `redis_bind`: Adresse de liaison (default: 127.0.0.1)
- `redis_version`: Version (default: dernière)

### 3. Rôle `nodejs`
**Exécuté sur:** Groupe `web`

**Tâches:**
- ✅ Installation des dépendances (curl, gnupg, ca-certificates)
- ✅ Ajout de la clé GPG NodeSource
- ✅ Ajout du référentiel NodeSource
- ✅ Installation de Node.js et npm
- ✅ Vérification des installations
- ✅ Affichage des versions installées

**Variables:**
- `nodejs_version`: Version Node.js (default: 18.x)
- `nodejs_npm_install`: Installation npm (default: true)
- `nodejs_apt_key_url`: URL de la clé GPG
- `nodejs_apt_repo_url`: URL du référentiel

### 4. Rôle `deploy`
**Exécuté sur:** Groupe `web`

**Tâches:**
- ✅ Création du groupe utilisateur `node`
- ✅ Création de l'utilisateur `node`
- ✅ Création du répertoire de déploiement (`/opt/my-awesome-app`)
- ✅ Copie des fichiers d'application
- ✅ Installation des dépendances npm
- ✅ Création du répertoire de logs
- ✅ Upload du fichier de service systemd
- ✅ Activation et démarrage du service
- ✅ Vérification que l'application s'exécute sur le port 3000

**Variables:**
- `app_name`: Nom de l'application (default: my-awesome-app)
- `app_deploy_path`: Chemin de déploiement (default: /opt/my-awesome-app)
- `app_user`: Utilisateur de l'application (default: node)
- `app_group`: Groupe de l'application (default: node)
- `app_port`: Port d'écoute (default: 3000)

## Service SystemD

### Fichier: `roles/deploy/files/my-awesome-app.service`

```ini
[Unit]
Description=My Awesome Node.js Application
After=network.target redis-server.service
Wants=redis-server.service

[Service]
Type=simple
User=node
WorkingDirectory=/opt/my-awesome-app
ExecStart=/usr/bin/node /opt/my-awesome-app/index.js
Restart=always
RestartSec=10
StandardOutput=append:/var/log/my-awesome-app/app.log
StandardError=append:/var/log/my-awesome-app/error.log
Environment="NODE_ENV=production"

[Install]
WantedBy=multi-user.target
```

**Caractéristiques:**
- Démarrage après la mise en place du réseau et de Redis
- Exécution sous l'utilisateur `node`
- Redémarrage automatique en cas d'erreur (délai: 10s)
- Logging vers `/var/log/my-awesome-app/`
- Mode production activé

## Utilisation

### Exécuter le playbook complet
```bash
cd ansible-deployment
ansible-playbook -i ../my-inventory playbook.yml -v
```

### Exécuter le playbook avec tags spécifiques
```bash
# Uniquement la configuration de base
ansible-playbook -i ../my-inventory playbook.yml --tags base -v

# Uniquement Redis
ansible-playbook -i ../my-inventory playbook.yml --tags redis -v

# Uniquement Node.js
ansible-playbook -i ../my-inventory playbook.yml --tags nodejs -v

# Uniquement le déploiement
ansible-playbook -i ../my-inventory playbook.yml --tags deploy -v
```

### Vérifier les hôtes
```bash
ansible -i ../my-inventory all -m ping
```

### Exécuter ad-hoc des commandes
```bash
# Obtenir les faits des hôtes
ansible -i ../my-inventory all -m setup

# Vérifier l'état du service Node.js
ansible -i ../my-inventory web -m systemd -a "name=my-awesome-app state=started"

# Vérifier la connexion Redis
ansible -i ../my-inventory redis -m command -a "redis-cli ping"
```

## Modifications Possibles

### Changer le fuseau horaire
Éditer `roles/base/defaults/main.yml` et modifier:
```yaml
timezone: Europe/London  # ou votre fuseau horaire
```

### Changer la version de Node.js
Éditer `roles/nodejs/defaults/main.yml` et modifier:
```yaml
nodejs_version: "20.x"  # pour Node.js 20
```

### Changer les packages essentiels
Éditer `roles/base/defaults/main.yml` et ajouter/retirer des packages:
```yaml
essential_packages:
  - apt-transport-https
  - ca-certificates
  - git
  # Ajouter vos packages ici
```

### Changer le port de l'application
Éditer `roles/deploy/defaults/main.yml` et modifier:
```yaml
app_port: 8080  # ou votre port
```

Puis mettez à jour aussi le fichier de service systemd pour refléter ce changement dans la vérification.

## Idempotence

Tous les rôles sont conçus pour être **idempotents**. Cela signifie que vous pouvez exécuter le playbook plusieurs fois sans risque - les tâches ne s'exécuteront que si nécessaire.

## Notes Important

⚠️ **Sans les modules `command`, `shell` ou `raw`**
Tous les rôles utilisent des modules Ansible natifs pour assurer l'idempotence:
- `apt`: Gestion des packages
- `service`/`systemd`: Gestion des services
- `copy`, `synchronize`: Transfert de fichiers
- `user`, `group`: Gestion des utilisateurs
- `timezone`: Configuration du fuseau horaire
- Et autres modules Ansible déclaratifs

✅ **Avantages:**
- Idempotence garantie
- Meilleure traçabilité
- Plus facile à déboguer
- Plus portable entre systèmes
