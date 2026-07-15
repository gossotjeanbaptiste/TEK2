# Comment faire fonctionner le projet

## Prérequis
- Docker

## Lancement du projet
1) Lancer N8N en local avec la commande suivante :
```bash
docker run -it --rm --name n8n -p 5678:5678 --add-host=host.docker.internal:host-gateway -v ~/.n8n:/home/node/.n8n docker.n8n.io/n8nio/n8n```

2) Lancer la DB en local avec la commande suivante :
```bash
docker run -d --name mysql-persona -p 3307:3306 -e MYSQL_ROOT_PASSWORD=persona mysql:latest
```

3) Se connecter à la DB avec un client MySQL (ex: MySQL Workbench) en utilisant les informations suivantes :
```bash
docker exec -it mysql-persona mysql -u root -ppersona
```

4) Créer la base de données et la table avec les commandes suivantes :
```sql
CREATE DATABASE IF NOT EXISTS persona;
USE persona;

CREATE TABLE IF NOT EXISTS users (
    chat_id VARCHAR(255) PRIMARY KEY,
    email VARCHAR(255) NOT NULL,
    interests TEXT,
    schedule_time VARCHAR(5) DEFAULT '08:00',
    rgpd_consent TINYINT(1) DEFAULT 0
);
```

5) Importer le workflow N8N en utilisant le fichier `ia_workflow.json` fourni dans ce projet.