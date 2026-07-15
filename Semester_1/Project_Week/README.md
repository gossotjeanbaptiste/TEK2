# Thuun - Application Bancaire

## Structure du projet

```
web/
├── front/                 # Frontend React/HTML/CSS/JS
│   ├── index.html        # Page de connexion
│   ├── home.html         # Page d'accueil
│   ├── style.css         # Styles
│   └── script.js         # Logique JavaScript
│
└── back/                 # Backend Node.js/Express
    ├── src/
    │   ├── config/
    │   │   └── database.js       # Configuration MySQL
    │   ├── controllers/
    │   │   └── authController.js # Logique authentification
    │   ├── middleware/
    │   │   └── auth.js           # Middleware JWT
    │   └── routes/
    │       └── auth.js           # Routes authentification
    ├── server.js                 # Serveur principal
    ├── package.json              # Dépendances
    ├── .env                      # Variables d'environnement
    └── database.sql              # Script création BD
```

## Installation

### Backend

1. Aller dans le dossier backend:
```bash
cd back
```

2. Installer les dépendances:
```bash
npm install
```

3. Configurer le `.env` avec vos identifiants MySQL:
```env
DB_HOST=localhost
DB_USER=root
DB_PASSWORD=votre_mot_de_passe
DB_DATABASE=thuun_db
```

4. Créer la base de données:
```bash
mysql -u root -p < database.sql
```

5. Démarrer le serveur:
```bash
npm start
```

Le serveur sera accessible sur `http://localhost:5000`

### Frontend

1. Aller dans le dossier frontend:
```bash
cd front
```

2. Ouvrir `index.html` dans votre navigateur

Ou utiliser un serveur local (ex: Python):
```bash
python -m http.server 3000
```

## API Endpoints

### POST /api/auth/login
Connecter un utilisateur
```json
{
  "email": "test@thuun.com",
  "password": "test123"
}
```

### POST /api/auth/register
Créer un nouveau compte
```json
{
  "email": "nouveau@thuun.com",
  "password": "securepassword",
  "nom": "Dupont",
  "prenom": "Jean"
}
```

### GET /api/auth/me
Récupérer les infos utilisateur (protégé par JWT)

## Credentials de test

- Email: test@thuun.com
- Mot de passe: test123

## Notes

- Les mots de passe sont hashés avec bcryptjs
- L'authentification utilise JWT
- CORS est configuré pour communiquer front/back
- À modifier en production: JWT_SECRET, DB_PASSWORD
