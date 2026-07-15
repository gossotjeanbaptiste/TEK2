# Step 2 - Développement avec Rôles Ansible

## 📋 Index de documentation

Bienvenue dans le répertoire de déploiement Ansible pour le Step 2 du projet G-DOP-400 Octopus Bootstrap.

### 🚀 Démarrage rapide

1. **Vérifier la configuration**: `bash checklist.sh`
2. **Tester la syntaxe**: `ansible-playbook playbook.yml --syntax-check`
3. **Aperçu des changements**: `ansible-playbook -i ../my-inventory playbook.yml --check -v`
4. **Déployer**: `bash deploy.sh`

### 📚 Documentation

| Document | Description |
|----------|-------------|
| [README.md](README.md) | Vue d'ensemble complète des rôles et configuration |
| [EXECUTION_GUIDE.md](EXECUTION_GUIDE.md) | Guide détaillé d'exécution et résolution de problèmes |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Architecture technique et détails de déploiement |

### 🛠️ Scripts utiles

| Script | Utilité |
|--------|---------|
| `deploy.sh` | Déploie l'infrastructure (avec options --dry-run, --syntax-check) |
| `validate.sh` | Valide toute la configuration avant déploiement |
| `checklist.sh` | Pré-déploiement: vérifie prérequis |

### 📁 Structure des rôles

```
roles/
├── base/        → Configuration commune à tous les hôtes
├── redis/       → Installation et configuration Redis
├── nodejs/      → Installation Node.js et npm
└── deploy/      → Déploiement application + service systemd
```

### 🎯 Objectifs du Step 2

#### 1. Rôle `base` - Tous les hôtes ✅
- Mise à jour du système
- Installation des packages essentiels
- Configuration du fuseau horaire (Europe/Paris)
- Configuration de zsh comme shell root
- Configuration SSH

#### 2. Rôle `redis` - Serveurs cache ✅
- Installation du serveur Redis
- Activation du service
- Vérification du bon fonctionnement

#### 3. Rôle `nodejs` - Serveurs web ✅
- Installation de Node.js 18.x
- Installation de npm
- Outils de développement

#### 4. Rôle `deploy` - Serveurs web ✅
- Copie des fichiers d'application
- Installation des dépendances
- Configuration du service systemd
- Démarrage automatique

#### 5. Service SystemD ✅
- Fichier `/etc/systemd/system/my-awesome-app.service`
- Démarrage automatique au boot
- Redémarrage en cas de crash
- Logs centralisés

### 📊 Architecture infrastructure

```
┌──────────────────────────────────────┐
│      Contrôleur Ansible (Vous)       │
└──────────────────────────────────────┘
           SSH sur 3 VMs
    ┌─────────────────────────┐
    │                         │
    ▼                         ▼
┌──────────────────┐  ┌──────────────────┐
│ Cache Server     │  │ Web Servers (x2) │
│ redis:6379       │  │ node:3000 (x2)   │
│ 192.168.56.13    │  │ 192.168.56.11/12 │
└──────────────────┘  └──────────────────┘
```

### ⚙️ Configuration

| Fichier | Utilité |
|---------|---------|
| `playbook.yml` | Playbook principal orchestrant tous les rôles |
| `ansible.cfg` | Configuration Ansible |
| `group_vars.yml` | Variables par groupe d'hôtes |
| `variables.example.yml` | Exemple de personnalisation avancée |

### 📖 Variables importantes

```yaml
# Base
timezone: Europe/Paris
root_shell: /bin/zsh

# Redis
redis_port: 6379

# Node.js
nodejs_version: 18.x

# Application
app_deploy_path: /opt/my-awesome-app
app_port: 3000
app_user: node
app_group: node
```

### 🔍 Vérifications après déploiement

```bash
# Tous les hôtes
ansible -i ../my-inventory all -m ping

# Redis
ansible -i ../my-inventory redis -m command -a "redis-cli ping"

# Node.js
ansible -i ../my-inventory web -m command -a "node --version"

# Service application
ansible -i ../my-inventory web -m systemd -a "name=my-awesome-app state=started"

# Logs
ansible -i ../my-inventory web -m command -a "tail /var/log/my-awesome-app/app.log"
```

### 🐛 Dépannage

**Erreur de connexion SSH?**
```bash
ssh -i ~/.ssh/id_ed25519 vboxuser@192.168.56.11
```

**Erreur de privilèges?**
```bash
ssh -i ~/.ssh/id_ed25519 vboxuser@192.168.56.11 sudo whoami
```

**Service ne démarre pas?**
```bash
ansible -i ../my-inventory web -m command -a "journalctl -u my-awesome-app -n 50"
```

Voir [EXECUTION_GUIDE.md](EXECUTION_GUIDE.md) pour plus de détails.

### 🎓 Points clés à retenir

1. **Idempotence**: Exécuter le playbook plusieurs fois est sûr
2. **Modules déclaratifs**: Pas de `command`, `shell`, ou `raw`
3. **Service systemd**: Redémarrage automatique en cas d'erreur
4. **Logs**: Sauvegardés dans `/var/log/my-awesome-app/`
5. **User application**: L'app s'exécute sous l'utilisateur `node` (sécurité)

### 📝 Commandes courantes

```bash
# Vérification pré-déploiement
bash checklist.sh

# Validation de syntaxe
ansible-playbook playbook.yml --syntax-check

# Aperçu sans modification (dry-run)
ansible-playbook -i ../my-inventory playbook.yml --check -v

# Déploiement complet
bash deploy.sh
# ou
ansible-playbook -i ../my-inventory playbook.yml -v

# Déploiement d'un rôle spécifique
ansible-playbook -i ../my-inventory playbook.yml --tags base -v

# Exécution avec logs détaillés
ansible-playbook -i ../my-inventory playbook.yml -vvv
```

### 🔗 Fichiers connexes

- Inventaire: `../my-inventory`
- Application: `../G-DOP-400_bootstrap_octopus_applications/`
- Documentation project: `../README.md`

### 📞 Support

Pour les problèmes:

1. Consultez [EXECUTION_GUIDE.md](EXECUTION_GUIDE.md) - Section Résolution des problèmes
2. Vérifiez les logs: `journalctl -u <service>`
3. Exécutez: `bash checklist.sh`
4. Mode verbose: `ansible-playbook -vvv ...`

### ✅ Checklist de déploiement

- [ ] Vérifier les prérequis: `bash checklist.sh`
- [ ] Tester la connexion SSH à chaque hôte
- [ ] Valider la syntaxe: `ansible-playbook playbook.yml --syntax-check`
- [ ] Exécuter dry-run: `ansible-playbook ... --check -v`
- [ ] Déployer: `bash deploy.sh`
- [ ] Vérifier les services: `ansible -i ../my-inventory all -m ping`
- [ ] Consulter les logs si besoin

---

**Bon déploiement ! 🚀**

Pour plus d'informations, consultez la documentation complète dans les fichiers listés ci-dessus.
