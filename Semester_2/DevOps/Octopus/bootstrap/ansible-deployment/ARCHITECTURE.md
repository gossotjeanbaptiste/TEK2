# Architecture Déploiement Step 2 - Octopus Bootstrap

## Vue d'ensemble globale

Ce document décrit l'architecture complète du déploiement Ansible pour le Step 2 du projet G-DOP-400.

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Playbook Principal                                │
│                    (playbook.yml)                                    │
└──────────────────────────────┬──────────────────────────────────────┘
                               │
                 ┌─────────────┼─────────────┐
                 │             │             │
                 ▼             ▼             ▼
        ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
        │ Base Role    │ │ Redis Role   │ │ Deploy Role  │
        │ (all hosts)  │ │ (redis)      │ │ (web)        │
        └──────────────┘ └──────────────┘ └──────────────┘
                 │             │             │
    ┌────────────┴─────────────┴─────────────┴────────────┐
    │                                                      │
    ▼                                                      ▼
┌─────────────────────────────┐         ┌─────────────────────────────┐
│   Node.js Role              │         │   Ansible Config            │
│   (web - après deploy)      │         │   - ansible.cfg             │
└─────────────────────────────┘         │   - group_vars.yml          │
                                        └─────────────────────────────┘
```

## Infrastructure cible

```
                   ┌─────────────────────────────────────┐
                   │   Contrôleur Ansible (votre PC)     │
                   │   - Playbook                        │
                   │   - Rôles                           │
                   │   - Inventaire                      │
                   └──────────────┬──────────────────────┘
                                  │ SSH
                   ┌──────────────┴──────────────┐
                   │                             │
                   ▼                             ▼
        ┌──────────────────────┐     ┌──────────────────────┐
        │  Cache Server        │     │  Web Servers x2      │
        │  (192.168.56.13)     │     │  (web-1, web-2)     │
        │                      │     │  (192.168.56.11,12) │
        │ - Redis :6379        │     │                      │
        │ - Base config        │     │ - Node.js 18.x       │
        │ - zsh                │     │ - npm                │
        │ - Europe/Paris       │     │ - my-awesome-app :3000│
        │ - Packages essentiels│     │ - systemd service    │
        └──────────────────────┘     │ - Base config        │
                                     │ - zsh                │
                                     └──────────────────────┘
```

## Flux de déploiement

### Phase 1: Configuration de base (tous les hôtes)

```
Role: base
  ├─ Update apt cache
  ├─ Upgrade all packages
  ├─ Install essential packages
  │   ├─ apt-transport-https
  │   ├─ ca-certificates
  │   ├─ git
  │   ├─ curl
  │   ├─ unzip
  │   ├─ zsh
  │   └─ autres...
  ├─ Set timezone (Europe/Paris)
  ├─ Set zsh as root shell
  └─ Configure SSH keys (optionnel)
```

### Phase 2: Installation Redis (serveur cache)

```
Role: redis
  ├─ Update apt cache
  ├─ Install redis-server
  ├─ Start redis-server service
  ├─ Enable redis-server (boot)
  ├─ Wait for port 6379
  └─ Verify Redis version
```

### Phase 3: Installation Node.js (serveurs web)

```
Role: nodejs
  ├─ Update apt cache
  ├─ Install dependencies (curl, gnupg, ca-certificates)
  ├─ Add NodeSource GPG key
  ├─ Add NodeSource repository
  ├─ Install nodejs + npm
  ├─ Verify node --version
  └─ Verify npm --version
```

### Phase 4: Déploiement application (serveurs web)

```
Role: deploy
  ├─ Create group 'node'
  ├─ Create user 'node'
  ├─ Create /opt/my-awesome-app directory
  ├─ Copy application files
  ├─ Fix permissions (owner: node:node)
  ├─ npm ci (install dependencies)
  ├─ Create /var/log/my-awesome-app directory
  ├─ Deploy systemd service file
  ├─ Enable my-awesome-app service
  ├─ Start my-awesome-app service
  ├─ Wait for port 3000
  └─ Display deployment status
```

## Structure des fichiers

```
ansible-deployment/
│
├── playbook.yml                      # Playbook principal
├── ansible.cfg                       # Configuration Ansible
├── group_vars.yml                    # Variables par groupe
├── variables.example.yml             # Exemple de variables avancées
│
├── roles/
│   ├── base/
│   │   ├── tasks/
│   │   │   └── main.yml              # Tâches configuration de base
│   │   ├── defaults/
│   │   │   └── main.yml              # Variables par défaut
│   │   └── handlers/
│   │       └── main.yml              # Handlers (vide pour base)
│   │
│   ├── redis/
│   │   ├── tasks/
│   │   │   └── main.yml              # Installation Redis
│   │   ├── defaults/
│   │   │   └── main.yml              # Config Redis (port, bind)
│   │   └── handlers/
│   │       └── main.yml              # Redémarrage Redis
│   │
│   ├── nodejs/
│   │   ├── tasks/
│   │   │   └── main.yml              # Installation Node.js
│   │   ├── defaults/
│   │   │   └── main.yml              # Version Node.js (18.x)
│   │   └── handlers/
│   │       └── main.yml              # Redémarrage app Node
│   │
│   └── deploy/
│       ├── tasks/
│       │   └── main.yml              # Déploiement application
│       ├── defaults/
│       │   └── main.yml              # Chemin app, utilisateur
│       ├── files/
│       │   └── my-awesome-app.service # Service systemd
│       └── handlers/
│           └── main.yml              # Redémarrage service
│
├── README.md                         # Documentation complète
├── EXECUTION_GUIDE.md                # Guide d'exécution (FR)
├── ARCHITECTURE.md                   # Ce fichier
│
├── deploy.sh                         # Script de déploiement
├── validate.sh                       # Script de validation
└── checklist.sh                      # Pré-déploiement checklist
```

## Service SystemD - Détails

### Fichier: `/etc/systemd/system/my-awesome-app.service`

```ini
[Unit]
Description=My Awesome Node.js Application
After=network.target redis-server.service        # Dépendances
Wants=redis-server.service

[Service]
Type=simple                                      # Service simple
User=node                                        # Utilisateur d'exécution
WorkingDirectory=/opt/my-awesome-app             # Répertoire de travail
ExecStart=/usr/bin/node /opt/my-awesome-app/index.js
Restart=always                                   # Redémarrage automatique
RestartSec=10                                    # Délai entre redémarrages
StandardOutput=append:/var/log/my-awesome-app/app.log
StandardError=append:/var/log/my-awesome-app/error.log
Environment="NODE_ENV=production"                # Variables d'env

[Install]
WantedBy=multi-user.target                      # Activation au démarrage
```

### Implications

- Service démarre **après** le réseau et Redis
- Redémarrage automatique en cas d'erreur (utile pour crashes)
- Logs sauvegardés dans `/var/log/my-awesome-app/`
- S'exécute en tant qu'utilisateur `node` (sécurité)
- Activé au démarrage du système

## Idempotence garantie

Tous les modules utilisés sont **déclaratifs** (pas d'imperatif):

| Module | Idempotent | Raison |
|--------|-----------|--------|
| `apt` | ✅ | Vérifie l'état actuel |
| `service`/`systemd` | ✅ | Idempotent intégré |
| `user`/`group` | ✅ | Pas de changement si exist |
| `copy`/`synchronize` | ✅ | Compare checksums |
| `timezone` | ✅ | Vérifie avant de changer |
| `file` | ✅ | Permissions uniquement si nécessaire |

❌ **NON utilisés (comme demandé):**
- `command`
- `shell`
- `raw`

## Variables et personnalisation

### Par défaut dans les rôles

```yaml
# Base
timezone: Europe/Paris
root_shell: /bin/zsh

# Redis
redis_port: 6379
redis_bind: 127.0.0.1

# Node.js
nodejs_version: 18.x

# Deploy
app_deploy_path: /opt/my-awesome-app
app_port: 3000
app_user: node
app_group: node
```

### Personnalisation possible

1. **Modifier les défauts** dans `roles/*/defaults/main.yml`
2. **Override au run** avec `-e "variable=valeur"`
3. **Variables par groupe** dans `group_vars.yml`
4. **Variables par hôte** dans l'inventaire

### Exemple: Changer le timezone

```bash
# Option 1: Modifier le fichier
vi roles/base/defaults/main.yml
# Changer: timezone: Europe/London

# Option 2: Override au runtime
ansible-playbook playbook.yml -e "timezone=Europe/London"
```

## Sécurité

### Points importants

1. **Utilisateur node**: L'app s'exécute avec un utilisateur non-privilégié
2. **SSH keys**: Authentification par clé (pas de mot de passe)
3. **Sudo sans mot de passe**: Configuré dans les VMs
4. **Logs**: Sauvegardés avec permissions restrictives
5. **Service isolation**: Chaque service tourne indépendamment

### Recommandations de sécurité (futur)

- Utiliser des certificats SSL pour Redis
- Configurer un firewall
- Mettre en place un monitoring
- Rotations de logs automatiques
- Secrets management (passwords, tokens)

## Performance

### Optimisations possibles

1. **Node.js**: Max old space size configuré
2. **Redis**: Politique de mémoire définie
3. **Systemd**: Redémarrage avec délai (évite le storm)
4. **NPM**: Utilisation de `npm ci` pour reproductibilité

## Dépannage

### Logs utiles

```bash
# Application Node
tail -f /var/log/my-awesome-app/app.log
tail -f /var/log/my-awesome-app/error.log

# Systemd
journalctl -u my-awesome-app -n 100 -f

# Redis
journalctl -u redis-server -n 100 -f

# Ansible
ansible-playbook playbook.yml -vvv  # Triple verbose
```

### Vérifications

```bash
# Port écoute
lsof -i :3000

# Processus Node
ps aux | grep node

# Service status
systemctl status my-awesome-app

# Redis cli
redis-cli ping
```

## Évolution future

### Étapes suivantes recommandées

1. **Monitoring**: Prometheus + Grafana
2. **Logging centralisé**: ELK Stack
3. **Load balancing**: Nginx reverse proxy
4. **SSL/TLS**: Certificats Let's Encrypt
5. **CI/CD**: Pipeline de déploiement
6. **Infrastructure as Code**: Terraform pour les VMs
7. **Backup**: Snapshots Redis, backups BD
8. **Disaster recovery**: Réplication, failover

## Conclusion

Cette architecture fournit:

✅ **Reproductibilité**: Déploiement identique chaque fois
✅ **Scalabilité**: Facilement ajouter des serveurs
✅ **Maintenabilité**: Code clair et documenté
✅ **Idempotence**: Exécution sûre et répétée
✅ **Automatisation**: Zéro intervention manuelle
