# 🚗 Auto-École Rout'Art

> **Votre Permis, Votre Liberté** - Site web moderne et professionnel pour l'auto-école Rout'Art

[![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white)](https://developer.mozilla.org/docs/Web/HTML)
[![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=for-the-badge&logo=css3&logoColor=white)](https://developer.mozilla.org/docs/Web/CSS)
[![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)](https://developer.mozilla.org/docs/Web/JavaScript)
[![Responsive](https://img.shields.io/badge/Responsive-Design-04AA6D?style=for-the-badge)](https://web.dev/responsive-web-design-basics/)

## 📋 Aperçu du Projet

Site web vitrine moderne et entièrement responsive pour l'auto-école Rout'Art, située à Lunéville (54300). Ce projet offre une expérience utilisateur optimale pour présenter l'ensemble des services de l'auto-école : formations de conduite, tarifs transparents, équipe professionnelle et informations pratiques.

### 🎯 Objectifs

- Présenter l'offre de formation de l'auto-école
- Faciliter la prise de contact et les inscriptions
- Offrir une expérience utilisateur moderne et intuitive
- Assurer une accessibilité optimale sur tous les appareils

## ✨ Fonctionnalités Principales

### 🎨 Design & Expérience Utilisateur

- **Interface moderne** avec système de design glassmorphism
- **Architecture CSS modulaire** avec variables centralisées
- **Navigation responsive** avec menu hamburger adaptatif
- **Animations fluides** et transitions optimisées pour l'engagement
- **Optimisation UX** avec bouton retour en haut et navigation intuitive

### 📱 Pages & Contenu Complet

| Page                   | Description                             | Fonctionnalités                                 |
| ---------------------- | --------------------------------------- | ----------------------------------------------- |
| **🏠 Accueil**          | Présentation générale avec hero section | Cartes interactives, informations de contact    |
| **🎓 Formations**       | Détail des permis (B, A1, A2, AM/BSR)   | Tableaux comparatifs, documents téléchargeables |
| **💰 Tarifs**           | Grilles tarifaires transparentes        | Tables responsives, options de financement      |
| **💳 Financement**      | Solutions de paiement                   | CPF, aides disponibles, simulateurs             |
| **📚 Code de la Route** | Apprentissage théorique                 | Méthodes pédagogiques, outils numériques        |
| **👥 Équipe**           | Présentation des moniteurs              | Photos, spécialités, expériences                |
| **📞 Contact**          | Informations pratiques                  | Formulaire, géolocalisation, horaires           |

### 🔧 Fonctionnalités Techniques Avancées

- **Système de grilles adaptatives** pour tous les contenus
- **Boutons "Voir plus" dynamiques** avec expansion progressive
- **Tables responsives intelligentes** avec transformation mobile
- **Navigation avec états actifs** et indicateurs visuels
- **Architecture CSS optimisée** avec import modulaire
- **JavaScript vanilla optimisé** pour les performances

## 🏗️ Architecture du Projet

```text
Rout-Art/
├── 📄 index.html                     # Page d'accueil principale 🏠
├── 📖 README.md                      # Documentation complète du projet 📋
├── 📝 app.py                         # Application serveur Flask
├── 🤖 robots.txt                     # Fichier robots pour le SEO
├── 🗺️ sitemap.xml                   # Plan du site pour les moteurs de recherche
├── 📋 Comment regler les soucis du CMS.docx  # Guide de dépannage CMS
│
├── 🚀 Fichiers Batch (Windows)
│   ├── ⚙️ compile.bat                # Compilation PyInstaller
│   ├── 📦 dependecies.bat            # Installation dépendances
│   ├── 🔐 setup_git_safe.bat         # Configuration Git safe directory
│   └── ▶️ run.bat                    # Lanceur d'application
│
├── 📁 page/                          # Pages HTML du site 🌐
│   ├── 📚 code_de_la_route.html      # Formation théorique - Code
│   ├── 📞 contact.html               # Formulaire contact & localisation
│   ├── 👥 equipe.html                # Présentation équipe
│   ├── 💳 financement.html           # Solutions paiement & aide financière
│   ├── 🎓 formations.html            # Détail des formations
│   ├── 🏦 garantie_financiere.html   # Garanties légales
│   ├── 📋 positionnement.html        # Évaluation initiale
│   ├── 📰 presse.html                # Page presse/média
│   ├── 📝 reclamation.html           # Gestion réclamations
│   ├── 📜 reglement.html             # Règlement intérieur
│   ├── 📊 resultat.html              # Consultation résultats
│   ├── 😊 satisfaction.html          # Enquêtes de qualité
│   ├── 🙏 remerciement.html          # Page remerciements
│   ├── 💰 tarifs.html                # Grilles tarifaires
│   └── 📄 template.html              # Modèle de développement
│
├── 📁 style/                         # Architecture CSS modulaire 🎨
│   ├── 📚 README.md                  # Documentation CSS
│   ├── 📚 code_de_la_route.css       # Styles - Code
│   ├── 📞 contact.css                # Styles - Contact
│   ├── 🌑 dark-mode.css              # Styles mode sombre
│   ├── 👥 equipe.css                 # Styles - Équipe
│   ├── 💳 financement.css            # Styles - Financement
│   ├── 🎓 formations.css             # Styles - Formations
│   ├── 🏦 garantie_financiere.css    # Styles - Garantie
│   ├── 🏗️ general.css                # Styles globaux
│   ├── 🏠 index.css                  # Styles - Accueil
│   ├── 📋 positionnement.css         # Styles - Positionnement
│   ├── 📰 presse.css                 # Styles - Presse
│   ├── 📝 reclamation.css            # Styles - Réclamation
│   ├── 📜 reglement.css              # Styles - Règlement
│   ├── 🙏 remerciement.css           # Styles - Remerciements
│   ├── 📊 resultat.css               # Styles - Résultats
│   ├── 😊 satisfaction.css           # Styles - Satisfaction
│   ├── 💰 tarifs.css                 # Styles - Tarifs
│   └── 🎨 variables.css              # Variables de design système
│
├── 📁 script/                        # Scripts JavaScript ⚙️
│   ├── 🌑 dark-mode.js               # Mode sombre
│   ├── 💳 financement.js             # Logique financement
│   ├── 🎓 formations.js              # Logique formations
│   ├── 🙏 remerciement.js            # Logique remerciements
│   ├── ⚙️ script.js                  # Fonctionnalités principales
│   └── 💰 tarifs.js                  # Logique tarifs
│
├── 📁 images/                        # Ressources visuelles 🖼️
│   ├── 👤 avatar/                    # Photos équipe
│   │   ├── avatar-emma.png
│   │   └── avatar-paul.png
│   ├── 🇫🇷 france-travail.png        # Logo France Travail
│   ├── 🇫🇷 france-travail-sombre.png # Logo France Travail (dark)
│   ├── 🏦 garantie-financiere.png    # Image garantie
│   ├── 🌅 hero-car-road.png          # Hero section
│   ├── 🚗 icon-car.png               # Icône voiture
│   ├── 📘 icon-facebook.ico          # Icône Facebook
│   ├── 📸 icon-instagram.png         # Icône Instagram
│   ├── 🏍️ icon-moto.png              # Icône moto
│   ├── 🎨 logo_routart_modern.png    # Logo principal
│   ├── 🚗 permis-1e.png              # Visuel permis 1€
│   ├── 📚 prepa_code.png             # Visuel préparation code
│   ├── 🏍️ moto/                      # Visuels formations moto
│   │   ├── am-bsr.png
│   │   ├── permis-a1-125.png
│   │   └── permis-a2.png
│   ├── 🚗 voiture/                   # Visuels formations voiture
│   │   ├── permis-b-accompagnee.png
│   │   ├── permis-b-automatique.png
│   │   └── permis-b-traditionnel.png
│   └── 🙏 remerciement/              # Page remerciements
│       ├── avatar_jean-baptiste.png
│       ├── github_ico.svg
│       ├── github_ico_darkmode.svg
│       ├── linkedin_ico.png
│       ├── portfolio_ico.png
│       ├── portfolio_ico_darkmode.png
│       └── techno/                   # Logos technologies
│           ├── github_ico.svg
│           ├── github_ico_dark.svg
│           ├── html.png
│           ├── html_dark.png
│           └── python.png
│
├── 📁 files/                         # Documents PDF officiels 📝
│   ├── 🏍️ moto/                      # Documents formations moto
│   │   ├── 125cm3/
│   │   │   └── programme_formation_125cm3.pdf
│   │   ├── a1/
│   │   │   ├── parcours_formation_a1.pdf
│   │   │   └── programme_formation_a1.pdf
│   │   ├── a2/
│   │   │   ├── enjeux_formation_a2.pdf
│   │   │   ├── parcours_formation_a2.pdf
│   │   │   └── programme_formation_a2.pdf
│   │   ├── a2_a/
│   │   │   └── parcours_formation_a.pdf
│   │   └── am/
│   │       ├── enjeux_formation_permis_am.pdf
│   │       └── programme_formation_permis_am.pdf
│   ├── 📜 reglement_interieur.docx   # Règlement intérieur
│   └── 🚗 voiture/                   # Documents formations voiture
│       ├── auto/
│       │   └── programme_auto_manuel.pdf
│       ├── enjeux_formation_permis_b.pdf
│       ├── parcours_formation_b.pdf
│       └── programme_formation_b.pdf
│
├── 📁 icon/                          # Favicon et icônes 🎯
│   └── 🎯 logo_routart_modern.png
│
├── 📁 cms/                           # CMS Desktop - Gestion du contenu 🖥️
│   ├── 📋 README_CMS.md              # Documentation CMS
│   ├── 📋 DOCUMENTATION.md           # Documentation détaillée
│   ├── 📋 CMS.md                     # Notes du CMS
│   ├── 📦 requirements.txt           # Dépendances Python
│   ├── 📁 icon/                      # Icônes du CMS
│   │   ├── logo-routart.png
│   │   └── logo_routart.ico
│   ├── 📁 source/                    # Code source Python
│   │   ├── __init__.py
│   │   ├── 🐍 rout_art_cms.py        # Point d'entrée principal
│   │   ├── 🛠️ config_manager.py      # Gestion configuration
│   │   ├── 🎨 html_manager.py        # Gestion contenu HTML
│   │   ├── 📝 logger.py              # Système logging
│   │   ├── 🔨 build_exe.py           # Compilation PyInstaller
│   │   ├── 🌳 git_manager.py         # Gestion Git/GitHub
│   │   ├── 🔧 server_manager.py      # Serveur web
│   │   └── 📦 rout_art_cms.spec      # Config PyInstaller
│   └── __pycache__/                  # Cache Python
│
├── 📁 dist/                          # Builds compilés 🖥️
│   └── Rout'Art CMS.exe              # Application compilée
│
└── 🔊 sounds/                        # Ressources sonores 🎵
    └── change_page.mp3               # Son changement page
```

## 🎨 Système de Design

### 🎨 Palette de Couleurs

```css
/* Couleurs Principales */
--primary-color: #1d3557;     /* Bleu marine professionnel */
--secondary-color: #22223b;    /* Gris anthracite */
--accent-color: #457b9d;       /* Bleu clair dynamique */
--white-color: #ffffff;        /* Blanc pur */
--light-gray: #f8f9fa;        /* Gris clair fond */
--text-color: #22223b;         /* Texte principal */
--success-color: #28a745;      /* Vert validation */
--warning-color: #ffc107;      /* Orange attention */
--error-color: #dc3545;        /* Rouge erreur */
```

### 📏 Système d'Espacement

```css
/* Espacements Cohérents */
--spacing-xs: 0.25rem;    /* 4px */
--spacing-sm: 0.5rem;     /* 8px */
--spacing-md: 1rem;       /* 16px */
--spacing-lg: 1.5rem;     /* 24px */
--spacing-xl: 2rem;       /* 32px */
--spacing-2xl: 3rem;      /* 48px */
--spacing-3xl: 4rem;      /* 64px */
```

### 🎭 Effets Visuels

- **Glassmorphism** : `backdrop-filter: blur(10px)`
- **Ombres progressives** : 4 niveaux d'intensité
- **Border radius** : Système unifié (4px, 8px, 12px)
- **Transitions** : Durées optimisées (0.3s, 0.5s)

## 📱 Responsive Design Avancé

### 📐 Breakpoints Stratégiques

| Appareil           | Largeur       | Optimisations                      |
| ------------------ | ------------- | ---------------------------------- |
| 📱 **Mobile**       | < 480px       | Menu hamburger, navigation tactile |
| 📱 **Large Mobile** | 480px - 768px | Grilles 1-2 colonnes               |
| 💻 **Tablette**     | 768px - 992px | Grilles 2-3 colonnes               |
| 🖥️ **Desktop**      | > 992px       | Grilles complètes, hover effects   |

### 🔄 Transformations Adaptatives

- **Tables → Cartes empilées** sur mobile
- **Grilles multi-colonnes → Colonne unique** responsive
- **Navigation horizontale → Menu hamburger** tactile
- **Images → Format adaptatif** selon l'écran

## Technologies & Performance

### 💻 Stack Technique

- **HTML5** : Structure sémantique moderne
- **CSS3** : Flexbox, Grid, Variables, Media Queries
- **JavaScript ES6+** : Modules, Arrow Functions, DOM moderne
- **Google Fonts** : Montserrat (Display), Open Sans (Body)

### ⚡ Optimisations Performance

- **CSS modulaire** : Chargement ciblé par page
- **Images optimisées** : Formats WebP/PNG selon usage
- **JavaScript léger** : < 15KB total, fonctionnalités essentielles
- **Architecture scalable** : Variables CSS, composants réutilisables

### 🔧 Fonctionnalités JavaScript

```javascript
// Navigation responsive
toggleMobileMenu()

// Expansion de contenu
expandContent(trigger, content)

// Tables adaptatives
transformTable(breakpoint)

// Navigation fluide
smoothScroll(target)

// Bouton retour haut
showScrollToTop()
```

## 🛠️ Installation & Développement

### 📋 Prérequis

- Navigateur moderne (Chrome, Firefox, Safari, Edge)
- Serveur HTTP local (Live Server, XAMPP, WAMP)
- Éditeur de code (VS Code recommandé)

### 🚀 Démarrage Rapide

```bash
# Cloner le projet
git clone https://github.com/gossotjeanbaptiste/Rout-Art.git

# Naviguer dans le dossier
cd Rout-Art

# Ouvrir avec un serveur local
# Avec Live Server (VS Code)
# Ou via Python
python -m http.server 8000

# Accéder au site
http://localhost:8000
```

### 🔧 Script de Développement

```bash
# Pour Chrome sans sécurité (développement)
./chrome_no_secu.sh
```

## 📊 Structure des Données

### 👥 Équipe

```javascript
const team = [
  {
    name: "Cédric",
    role: "Exploitant & Formateur",
    specialties: ["Permis B", "A1", "A2", "AM"],
    experience: "Depuis 2002"
  },
  // ...autres membres
];
```

### 🎓 Formations

```javascript
const formations = {
  "permis-b": {
    title: "Permis B",
    age: "Dès 17 ans",
    duration: "Variable selon profil",
    price: "À partir de 1200€"
  }
  // ...autres formations
};
```

## 🔐 Sécurité & Bonnes Pratiques

- **Validation côté client** pour les formulaires
- **Sanitisation des entrées** utilisateur
- **Accessibilité WCAG** niveau AA
- **SEO optimisé** avec meta tags appropriés
- **Structure sémantique** HTML5

## 📞 Contact & Support

### 🏢 Auto-École Rout'Art

- **📧 Email** : [autoecoleroutart@hotmail.com](mailto:autoecoleroutart@hotmail.com)
- **📱 Cédric (Gérant)** : [06 24 07 18 39](tel:+33624071839)
- **📍 Adresse** : 79 Rue d'Alsace, 54300 Lunéville
- **🌐 Réseaux sociaux** : Facebook, Instagram

### 🗓️ Horaires d'Ouverture

- **Lundi - Vendredi** : 10h00 - 19h00
- **Samedi** : 9h00 - 17h00
- **Dimanche** : Fermé

## 📈 Roadmap & Évolutions

### 🎯 Version Actuelle (v2.0)

- ✅ Design responsive complet
- ✅ Architecture CSS modulaire
- ✅ Pages optimisées mobile
- ✅ Système de navigation avancé

### 🚀 Prochaines Améliorations

- 🔄 Système de réservation en ligne
- 📱 Application mobile PWA
- 🤖 Chatbot d'assistance
- 📊 Dashboard élève personnalisé

## 📄 Licence & Droits

Ce projet est la propriété exclusive de **l'Auto-École Rout'Art**. Tous droits réservés © 2025.

**Développement** : Réalisé avec ❤️ pour offrir la meilleure expérience aux futurs conducteurs de Lunéville et ses environs.

---

> *"Apprendre à conduire, c'est gagner en liberté"* - Auto-École Rout'Art
