# 📊 Diagramme de Flux: CMS → GitHub Pages via GitHub Actions

## Vue d'ensemble du processus

```mermaid
graph TD
    A["👤 Utilisateur<br/>Lance le CMS"] --> B["🎨 Interface GUI<br/>rout_art_cms.py"]
    
    B --> C{"Action de l'utilisateur"}
    
    C -->|Éditer contenu| D["✏️ html_manager.py<br/>Modifie fichier HTML"]
    C -->|Voir aperçu| E["👁️ server_manager.py<br/>Lance serveur local"]
    C -->|Synchroniser| F["📤 Git Push"]
    
    D --> G["💾 Fichier local<br/>modifié<br/>pages/*.html"]
    E --> H["🌐 Prévisualisation<br/>localhost:port"]
    
    G --> I{"Valider et pousser?"}
    
    I -->|Oui| F
    I -->|Non| B
    
    F --> J["🔧 git_manager.py<br/>Opérations Git"]
    
    J --> K["1️⃣ Pull<br/>Récupère dernières<br/>modifications"]
    J --> L["2️⃣ Add<br/>Prépare fichiers<br/>modifiés"]
    J --> M["3️⃣ Commit<br/>Crée message<br/>de commit"]
    J --> N["4️⃣ Push<br/>Envoie vers<br/>GitHub"]
    
    K --> O["📡 Synchronisation<br/>locale"]
    L --> P["📦 Staging area"]
    M --> Q["📝 Historique<br/>commit créé"]
    
    O --> R["🔗 GitHub Repository<br/>main/master branch"]
    P --> R
    Q --> R
    N --> R
    
    R --> S["⚡ GitHub Actions<br/>Workflow Trigger"]
    
    S --> T["🔄 GitHub Actions<br/>Pipeline"]
    
    T --> U["1. Build<br/>Valide contenu"]
    T --> V["2. Test<br/>Vérifie HTML/CSS"]
    T --> W["3. Deploy<br/>Génère site statique"]
    
    U --> X["✅ Contrôle qualité"]
    V --> X
    W --> X
    
    X --> Y{"Build <br/>réussi?"}
    
    Y -->|Non| Z["❌ Notification erreur<br/>Logs disponibles"]
    Y -->|Oui| AA["📋 Sitemap update<br/>sitemap_generator.py"]
    
    Z --> AB["🔔 Notification<br/>utilisateur"]
    AA --> AC["🌍 GitHub Pages<br/>Build & Deploy"]
    
    AC --> AD["🚀 Site en ligne<br/>routart.github.io"]
    
    AD --> AE["👥 Site accessible<br/>aux utilisateurs"]
    
    style A fill:#e1f5ff
    style B fill:#fff3e0
    style D fill:#f3e5f5
    style E fill:#f3e5f5
    style F fill:#fff3e0
    style R fill:#c8e6c9
    style S fill:#ffccbc
    style T fill:#ffccbc
    style AC fill:#c8e6c9
    style AD fill:#c8e6c9
    style AE fill:#b3e5fc
```

---

## 📊 Flux détaillé étape par étape

### **Phase 1: Édition dans le CMS (Local)**

```mermaid
graph LR
    A["👤 Utilisateur ouvre le CMS"] --> B["Interface CustomTkinter<br/>rout_art_cms.py"]
    B --> C["📁 Sélection fichier<br/>page/*.html"]
    C --> D["✏️ html_manager.py<br/>edit_html()"]
    D --> E["🖊️ Zone d'édition<br/>avec recherche/remplace"]
    E --> F["💾 Sauvegarde locale <br/>lors du clique sur bouton sauvegarder"]
    F --> G["📁 Fichier modifié<br/>pages/tarifs.html<br/>pages/contact.html<br/>etc."]
    
    style A fill:#e1f5ff
    style B fill:#fff3e0
    style D fill:#f3e5f5
    style G fill:#ffebee
```

### **Phase 2: Synchronisation Git (Local → GitHub)**

```mermaid
graph TD
    A["📁 Fichiers modifiés<br/>en local"] --> B["⚙️ git_manager.py<br/>push_to_github()"]
    
    B --> C["1️⃣ PULL<br/>repo.remotes.origin.pull()"]
    C --> D["Fusionner les derniers<br/>changements de GitHub"]
    
    D --> E["2️⃣ ADD<br/>repo.index.add()"]
    E --> F["Préparer fichiers<br/>tarifs.html, contact.html"]
    
    F --> G["3️⃣ COMMIT<br/>repo.index.commit()"]
    G --> H["Message de commit:<br/>'Update tarifs prices'<br/>'Fix contact form'"]
    
    H --> I["4️⃣ PUSH<br/>origin.push()"]
    I --> J["📡 Envoi vers GitHub"]
    
    J --> K["🔗 GitHub Repository<br/>Branche main/master"]
    K --> L["✅ Commits enregistrés<br/>Historique mis à jour"]
    
    style A fill:#ffebee
    style B fill:#fff3e0
    style K fill:#c8e6c9
    style L fill:#a5d6a7
```

### **Phase 3: Déclenchement GitHub Actions**

```mermaid
graph TD
    A["📡 Push reçu<br/>par GitHub"] --> B["⚡ GitHub Actions<br/>Workflow déclenché"]
    
    B --> C["trigger: on push<br/>to main branch"]
    
    C --> D["🔄 Pipeline démarre"]
    
    D --> E["Job 1: Build & Test"]
    D --> F["Job 2: Generate Sitemap"]
    D --> G["Job 3: Deploy Pages"]
    
    E --> E1["✓ Valider HTML"]
    E --> E2["✓ Vérifier CSS"]
    E --> E3["✓ Tester liens"]
    
    F --> F1["🗺️ sitemap.xml<br/>généré/mis à jour"]
    
    G --> G1["📦 Build static site"]
    G --> G2["📤 Deploy to Pages"]
    
    E1 --> H{"Tous les<br/>tests OK?"}
    E2 --> H
    E3 --> H
    
    H -->|Non| I["❌ Workflow échoue<br/>Logs disponibles"]
    H -->|Oui| J["✅ Workflow réussit"]
    
    I --> K["🔔 Notification GitHub<br/>Commit marqué comme échoué"]
    J --> L["📋 Sitemap mis à jour"]
    L --> M["🌍 Pages déployées"]
    
    style B fill:#ffccbc
    style D fill:#ffccbc
    style J fill:#a5d6a7
    style M fill:#c8e6c9
```

### **Phase 4: Déploiement GitHub Pages**

```mermaid
graph LR
    A["✅ GitHub Actions<br/>réussi"] --> B["📦 Artefacts générés"]
    
    B --> C["Index.html<br/>pages/tarifs.html<br/>pages/contact.html<br/>..."]
    
    C --> D["🌍 GitHub Pages<br/>Service"]
    
    D --> E["Build et compilation<br/>du site statique"]
    
    E --> F["🚀 Déploiement sur<br/>routart.github.io"]
    
    F --> G["🌐 Site en ligne<br/>accessible à tous"]
    
    G --> H["👥 Utilisateurs"]
    H --> I["Voient le site<br/>à jour!"]
    
    style A fill:#a5d6a7
    style F fill:#c8e6c9
    style G fill:#b3e5fc
    style I fill:#81c784
```

---

## 📋 Tableau Récapitulatif

| Phase | Composant            | Action                  | Entrée                  | Sortie                  |
| ----- | -------------------- | ----------------------- | ----------------------- | ----------------------- |
| **1** | CMS GUI              | Éditer fichiers HTML    | Fichiers locaux         | Fichiers modifiés       |
| **2** | html_manager.py      | Lire/écrire HTML        | Changements utilisateur | Fichiers .html modifiés |
| **3** | server_manager.py    | Serveur local optionnel | Port configuré          | Prévisualisation locale |
| **4** | git_manager.py       | Pull/Add/Commit/Push    | Fichiers modifiés       | Commits sur GitHub      |
| **5** | GitHub Actions       | Build & Test            | Commits reçus           | Logs + Rapports         |
| **6** | sitemap_generator.py | Générer sitemap.xml     | Structure HTML          | sitemap.xml mis à jour  |
| **7** | GitHub Pages         | Déploiement             | Fichiers générés        | Site en ligne           |

---

## 🔄 Flux Complet: Exemple Concret

### Scénario: Mise à jour des tarifs

```
1. Utilisateur ouvre CMS
   ↓
2. Sélectionne "page/tarifs.html"
   ↓
3. Modifie prix: "70€" → "75€"
   ↓
4. Clique "Synchroniser avec GitHub"
   ↓
5. CMS exécute:
   - git pull (obtient derniers changements)
   - git add pages/tarifs.html (prépare fichier)
   - git commit -m "Update tarif inscription: 70€ → 75€" (crée commit)
   - git push (envoie vers GitHub)
   ↓
6. GitHub reçoit le push
   ↓
7. Actions déclenchées:
   - Valide HTML/CSS
   - Teste liens
   - Génère sitemap
   - Déploie sur Pages
   ↓
8. routart.github.io mis à jour ✅
   ↓
9. Utilisateurs voient les nouveaux tarifs!
```

---

## 🔐 Points de Sécurité & Validation

```mermaid
graph TD
    A["👤 Utilisateur<br/>modifie données"] --> B["🛡️ Validations"]
    
    B --> C["1. Fichier existe?"]
    B --> D["2. HTML valide?"]
    B --> E["3. Chemin sûr?"]
    
    C --> F{"Tous les<br/>checks OK?"}
    D --> F
    E --> F
    
    F -->|Non| G["🚨 Erreur<br/>Affichage message"]
    F -->|Oui| H["✅ Fichier sauvegardé"]
    
    H --> I["🔄 Git Pull<br/>Évite conflits"]
    
    I --> J["🔑 Authentification<br/>Utilise credentials"]
    
    J --> K["✅ Push réussi"]
    
    style B fill:#fff9c4
    style K fill:#a5d6a7
```

---

## 🎯 Architecture Résumée

```
┌─────────────────────────────────────────────────────────┐
│                    LOCAL MACHINE                         │
│                                                           │
│  ┌──────────────────────────────────────────────────┐   │
│  │        ROUT'ART CMS (CustomTkinter GUI)          │   │
│  │                                                   │   │
│  │  ┌─────────────────────────────────────────┐    │   │
│  │  │   rout_art_cms.py (Main Application)   │    │   │
│  │  └─────────────────────────────────────────┘    │   │
│  │           ↓                    ↓                  │   │
│  │    ┌──────────────┐    ┌─────────────────┐     │   │
│  │    │html_manager  │    │server_manager   │     │   │
│  │    │(Edit HTML)   │    │(Local Preview)  │     │   │
│  │    └──────────────┘    └─────────────────┘     │   │
│  │           ↓                                      │   │
│  │    ┌──────────────────────────────────────┐    │   │
│  │    │    git_manager.py (Git Sync)        │    │   │
│  │    │  Pull → Add → Commit → Push         │    │   │
│  │    └──────────────────────────────────────┘    │   │
│  └──────────────────────────────────────────────────┘   │
│                      ↓                                    │
│            Local Git Repository                          │
│     (pages/tarifs.html, pages/contact.html...)          │
└─────────────────────────────────────────────────────────┘
          ↓ git push via HTTPS/SSH
┌─────────────────────────────────────────────────────────┐
│              GITHUB REPOSITORY                          │
│          (github.com/user/repo)                         │
│                                                           │
│  - Fichiers source (pages/*.html)                       │
│  - Commit history                                        │
│  - GitHub Actions Workflows                             │
└─────────────────────────────────────────────────────────┘
          ↓ Webhook: on push to main
┌─────────────────────────────────────────────────────────┐
│              GITHUB ACTIONS                             │
│        (Automated CI/CD Pipeline)                       │
│                                                           │
│  1. Build & Test                                         │
│  2. Generate sitemap_generator.py                       │
│  3. Deploy to Pages                                      │
└─────────────────────────────────────────────────────────┘
          ↓ Auto-deploy
┌─────────────────────────────────────────────────────────┐
│           GITHUB PAGES (Hosting)                        │
│        (routart.github.io - Public Site)                │
│                                                           │
│  - Site statique en ligne                              │
│  - Contenu à jour                                       │
│  - sitemap.xml généré                                  │
└─────────────────────────────────────────────────────────┘
          ↓
┌─────────────────────────────────────────────────────────┐
│              PUBLIC USERS                               │
│        Visitent et voient le site à jour ✅              │
└─────────────────────────────────────────────────────────┘
```

---

## ⚙️ Configuration GitHub Actions (Exemple)

```yaml
name: CI/CD Pipeline

on:
  push:
    branches: [ main, master ]

jobs:
  build-and-deploy:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Validate HTML
      run: |
        # Validation des fichiers HTML
        for file in page/*.html; do
          echo "Validating $file"
        done
    
    - name: Generate Sitemap
      run: |
        python3 -m cms.source.sitemap_generator
    
    - name: Deploy to GitHub Pages
      uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./
```

---

## 🎓 Résumé du Flux Complet

```
CMS USER EDIT
    ↓
LOCAL FILE MODIFIED (HTML)
    ↓
GIT COMMIT PUSHED
    ↓
GITHUB RECEIVES PUSH
    ↓
GITHUB ACTIONS TRIGGERED
    ↓
BUILD & TEST & SITEMAP
    ↓
GITHUB PAGES DEPLOY
    ↓
LIVE SITE UPDATED ✅
    ↓
USERS SEE NEW CONTENT 🎉
```
