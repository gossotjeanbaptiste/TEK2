# 🏦 Thuun - Application Bancaire

## 📋 Structure du projet

```
web/
├── front/                     # Frontend HTML/CSS/JS
│   ├── landing.html          # Page d'accueil (redirection)
│   ├── index.html            # Page de connexion
│   ├── home.html             # Page d'accueil après connexion
│   ├── config.js             # Configuration API
│   ├── script.js             # Logique JavaScript
│   └── style.css             # Styles
│
└── back/                      # Backend Node.js/Express
    ├── src/
    │   ├── config/
    │   │   └── database.js         # Config MySQL
    │   ├── controllers/
    │   │   └── authController.js   # Logique auth
    │   ├── middleware/
    │   │   └── auth.js             # Middleware JWT
    │   └── routes/
    │       └── auth.js             # Routes API
    ├── server.js                   # Serveur Express
    ├── init-db.js                  # Script d'initialisation BD
    ├── package.json                # Dépendances
    ├── .env                        # Variables d'env
    └── database.sql                # Schema SQL (ancien)
```

## 🚀 Installation et Démarrage

### Backend - Configuration

1. **Aller dans le dossier backend:**
```bash
cd back
```

2. **Installer les dépendances:**
```bash
npm install
```

3. **Configurer le `.env`:**
Le fichier `.env` contient déjà vos paramètres. Vérifiez que le serveur MySQL est accessible avec les identifiants fournis.

4. **Initialiser la base de données:**
```bash
npm run init-db
```

Cette commande va:
- Créer la base de données `thuun_db`
- Créer la table `users`
- Créer un utilisateur de test

5. **Démarrer le serveur:**
```bash
npm start
```

ou avec nodemon (reload auto):
```bash
npm run dev
```

Le serveur sera accessible sur `http://localhost:5000`

### Frontend - Configuration

1. **Aller dans le dossier frontend:**
```bash
cd front
```

2. **Ouvrir directement:**
Ouvrir `landing.html` dans votre navigateur (il redirige automatiquement)

Ou utiliser un serveur local Python:
```bash
python -m http.server 3000
```

Accéder à: `http://localhost:3000/front/landing.html`

## 🔐 Identifiants de test

```
Email: test@thuun.com
Mot de passe: test123
```

## 🔌 API Endpoints

### POST /api/auth/login
Connexion utilisateur

**Requête:**
```json
{
  "email": "test@thuun.com",
  "password": "test123"
}
```

**Réponse (200):**
```json
{
  "success": true,
  "message": "Connexion réussie",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "user": {
    "id": 1,
    "email": "test@thuun.com",
    "nom": "Dupont",
    "prenom": "Jean"
  }
}
```

### POST /api/auth/register
Créer un nouveau compte

**Requête:**
```json
{
  "email": "nouveau@thuun.com",
  "password": "securepassword",
  "nom": "Martin",
  "prenom": "Sophie"
}
```

**Réponse (201):**
```json
{
  "success": true,
  "message": "Utilisateur créé avec succès"
}
```

### GET /api/auth/me
Récupérer les infos utilisateur (protégé par JWT)

**Headers:**
```
Authorization: Bearer <TOKEN>
```

### GET /api/health
Vérifier l'état du serveur

**Réponse (200):**
```json
{
  "status": "actif",
  "message": "Le serveur Thuun est en ligne",
  "timestamp": "2026-01-13T13:31:25.000Z"
}
```

## 🔄 Flux d'authentification

1. **landing.html** → Vérification du token en localStorage
   - Si token → Redirection vers **home.html**
   - Si pas token → Redirection vers **index.html**

2. **index.html** → Formulaire de connexion
   - POST /api/auth/login
   - Sauvegarde du token en localStorage
   - Redirection vers **home.html**

3. **home.html** → Page principal
   - Affiche les 3 boutons (Cours, Compte, Conseiller)
   - Bouton déconnexion → Supprime le token et revient à **index.html**

## 🛠️ Technologie utilisée

**Frontend:**
- HTML5
- CSS3 (Gradient, Flexbox, Animation)
- JavaScript Vanilla (ES6+)
- Fetch API pour les requêtes HTTP
- LocalStorage pour les tokens

**Backend:**
- Node.js
- Express.js
- MySQL2
- bcryptjs (hachage mots de passe)
- JWT (JSON Web Tokens)
- CORS (Cross-Origin Resource Sharing)
- dotenv (variables d'environnement)

## 🔒 Sécurité

- ✅ Les mots de passe sont hashés avec bcryptjs (10 rounds)
- ✅ Authentification par JWT tokens
- ✅ CORS configuré pour le frontend
- ✅ Validation des emails et mots de passe
- ✅ Gestion d'erreurs appropriée
- ⚠️ À changer en production: `JWT_SECRET` dans `.env`

## 📝 Notes importantes

- Le serveur doit être lancé AVANT d'accéder au frontend
- Les tokens JWT expirent après 7 jours (configurable)
- La base de données MySQL doit être accessible sur localhost:3306
- En cas d'erreur de connexion, vérifier les logs du serveur

## 📞 Troubleshooting

**Erreur: "Erreur de connexion. Vérifiez que le serveur est actif"**
- Vérifier que le serveur Node.js est lancé (`npm start`)
- Vérifier que le PORT 5000 n'est pas occupé
- Vérifier la connexion MySQL

**Erreur: "Cet email existe déjà"**
- L'email est déjà enregistré dans la base de données
- Utiliser un autre email ou supprimer l'utilisateur de la BD

**Erreur: "Email ou mot de passe incorrect"**
- Vérifier l'email et le mot de passe
- S'assurer que l'utilisateur existe dans la BD

## ✨ Améliorations futures

- [ ] Page de création de compte (register)
- [ ] Réinitialisation de mot de passe
- [ ] Authentification à deux facteurs
- [ ] Pages Cours, Compte, Conseiller
- [ ] Tableau de bord avec données bancaires
- [ ] Notifications email
- [ ] Refresh tokens automatiques
