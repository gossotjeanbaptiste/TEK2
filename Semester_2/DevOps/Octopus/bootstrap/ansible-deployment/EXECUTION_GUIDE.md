# Step 2 - Développement avec Rôles (Guide d'Exécution)

## Vue d'ensemble

Ce guide vous aide à exécuter le déploiement complet de l'infrastructure Ansible créée pour le Step 2.

## Prérequis

1. ✅ Ansible installé (`ansible --version` doit fonctionner)
2. ✅ Accès SSH aux 3 machines virtuelles
3. ✅ Fichier d'inventaire `my-inventory` à la racine du projet
4. ✅ Clé SSH privée `~/.ssh/id_ed25519` configurée

## Structure des rôles

```
roles/
├── base/           → Configuration commune à tous les hôtes
├── redis/          → Installation et configuration Redis
├── nodejs/         → Installation Node.js et npm
└── deploy/         → Déploiement de l'application
```

## Étapes d'exécution

### Étape 1: Validation avant déploiement

```bash
cd ansible-deployment

# Vérifier la syntaxe du playbook
ansible-playbook -i ../my-inventory playbook.yml --syntax-check

# Vérifier la connectivité aux hôtes
ansible -i ../my-inventory all -m ping

# Exécuter un dry-run (aperçu des changements)
ansible-playbook -i ../my-inventory playbook.yml --check -v
```

**✅ Résultat attendu:** Tous les hôtes répondent au ping et la syntaxe est valide.

### Étape 2: Exécuter le déploiement complet

```bash
cd ansible-deployment

# Exécuter le playbook complet
ansible-playbook -i ../my-inventory playbook.yml -v

# Ou utiliser le script fourni:
bash deploy.sh
```

**⏱️ Durée estimée:** 5-10 minutes selon la vitesse du réseau

**✅ Résultat attendu:** Tous les rôles s'exécutent avec succès (OK ou changed).

### Étape 3: Vérifier le déploiement

```bash
# Vérifier que la base est configurée sur tous les hôtes
ansible -i ../my-inventory all -m command -a "whoami; date"

# Vérifier Redis sur le serveur cache
ansible -i ../my-inventory redis -m command -a "redis-cli ping"

# Vérifier Node.js sur les serveurs web
ansible -i ../my-inventory web -m command -a "node --version; npm --version"

# Vérifier que le service my-awesome-app fonctionne
ansible -i ../my-inventory web -m systemd -a "name=my-awesome-app state=started"

# Vérifier les logs de l'application
ansible -i ../my-inventory web -m command -a "tail -20 /var/log/my-awesome-app/app.log"
```

## Déploiement par étapes (optionnel)

Si vous préférez déployer chaque rôle séparément:

### Déployer uniquement la base

```bash
ansible-playbook -i ../my-inventory playbook.yml --tags "base" -v
```

Effectue:
- Mise à jour du système
- Installation des packages essentiels
- Configuration du fuseau horaire
- Configuration de zsh

### Déployer Redis

```bash
ansible-playbook -i ../my-inventory playbook.yml --tags "redis" -v
```

Effectue:
- Installation du serveur Redis
- Activation du service
- Vérification du port 6379

### Déployer Node.js

```bash
ansible-playbook -i ../my-inventory playbook.yml --tags "nodejs" -v
```

Effectue:
- Ajout du référentiel NodeSource
- Installation de Node.js 18.x
- Installation de npm

### Déployer l'application

```bash
ansible-playbook -i ../my-inventory playbook.yml --tags "deploy" -v
```

Effectue:
- Création de l'utilisateur `node`
- Copie des fichiers d'application
- Installation des dépendances npm
- Configuration du service systemd
- Démarrage de l'application

## Résolution des problèmes

### Erreur de connexion SSH

```bash
# Vérifier que les hôtes sont accessibles
ping 192.168.56.11
ping 192.168.56.12
ping 192.168.56.13

# Tester la connexion SSH manuellement
ssh -i ~/.ssh/id_ed25519 vboxuser@192.168.56.11
```

**Solution:** Vérifiez que les VMs sont bien en cours d'exécution et que votre clé SSH est correcte.

### Erreur de privilèges (become)

```bash
# Vérifier que l'utilisateur peut utiliser sudo sans mot de passe
ssh -i ~/.ssh/id_ed25519 vboxuser@192.168.56.11 sudo whoami
```

**Solution:** Assurez-vous que `vboxuser` est configuré pour sudo sans mot de passe.

### Port déjà utilisé

```bash
# Vérifier quel processus utilise le port 3000
ansible -i ../my-inventory web -m command -a "lsof -i :3000"

# Arrêter l'application existante
ansible -i ../my-inventory web -b -m service -a "name=my-awesome-app state=stopped"
```

### Redis ne démarre pas

```bash
# Vérifier le statut de Redis
ansible -i ../my-inventory redis -m service -a "name=redis-server"

# Vérifier les logs
ansible -i ../my-inventory redis -m command -a "journalctl -u redis-server -n 50"
```

## Scripts d'aide

### Script de validation

```bash
bash validate.sh
```

Exécute tous les tests de validation pour s'assurer que tout est en place avant le déploiement.

### Script de déploiement rapide

```bash
# Exécution normale
bash deploy.sh

# Dry-run (aperçu des changements)
bash deploy.sh --dry-run

# Vérification de syntaxe
bash deploy.sh --syntax-check
```

## Vérification finale

Après un déploiement réussi, vous devriez avoir:

```bash
# ✅ Sur tous les hôtes (all)
- zsh configuré comme shell root
- Timezone: Europe/Paris
- Packages essentiels installés

# ✅ Sur les serveurs cache (redis)
- Redis actif et écoute sur le port 6379
- Service redis-server activé au démarrage

# ✅ Sur les serveurs web (web)
- Node.js 18.x installé
- npm installé
- Service my-awesome-app actif et écoute sur le port 3000
- Application lancée automatiquement au démarrage
```

## Récapitulatif des modifications

| Rôle | Host Group | Actions |
|------|-----------|---------|
| base | all | Upgrade système, packages essentiels, timezone, zsh |
| redis | redis | Installation + activation du service Redis |
| nodejs | web | Installation Node.js + npm |
| deploy | web | Copie app, npm install, service systemd |

## Points importants

⚠️ **Idempotence:** Vous pouvez exécuter le playbook plusieurs fois - il ne fera les changements que si nécessaire.

⚠️ **Logs:** Les logs sont disponibles à `/var/log/my-awesome-app/app.log` sur les serveurs web.

⚠️ **Service systemd:** Le fichier service utilise `Restart=always` pour redémarrer automatiquement l'application en cas d'erreur.

⚠️ **Dépendances:** Le service application dépend de Redis (`After=redis-server.service`).

## Prochaines étapes (Step 3)

Une fois le Step 2 complété, vous pouvez:
- Configurer un reverse proxy (Nginx)
- Ajouter du monitoring (Prometheus, Grafana)
- Mettre en place du logging centralisé (ELK Stack)
- Configurer des backups automatiques

