1.  **Interface utilisateur simple et rapide** (avec Tkinter ou CustomTkinter).
2.  **Manipulation HTML fiable** (avec BeautifulSoup).
3.  **Gestion Git aisée** (avec GitPython ou des appels systèmes).

Voici un plan détaillé des étapes clés à implémenter en Python pour garantir le succès de votre MVP en 4 semaines.

## 🐍 1. Prérequis Python et Librairies

Vous devrez installer ces trois librairies principales pour votre projet :

  * **`beautifulsoup4` (ou `bs4`) :** Pour lire, naviguer et modifier le contenu de votre fichier HTML (`page/tarifs.html`).
  * **`gitpython` :** Pour interagir directement avec le dépôt Git local (tirer, ajouter, *commiter*, et *pusher*).
  * **`tkinter` (ou `customtkinter`) :** Pour construire l'interface utilisateur de l'application de bureau.

<!-- end list -->

```bash
pip install beautifulsoup4 gitpython
# Tkinter est souvent inclus avec Python.
```

-----

## ⚙️ 2. Le "Cœur" de la Logique (Semaine 2)

Ceci est la fonction centrale qui sera déclenchée par le bouton **"Mettre le Site à Jour"**.

### A. La Fonction de Mise à Jour HTML (BeautifulSoup)

L'idée est de créer une fonction qui prend en entrée le chemin du fichier, l'ID de la table et le nouveau prix, puis qui met à jour le fichier local.

**Action à faire (Modification Manuelle du HTML) :**

Dans `page/tarifs.html`, ajoutez une classe unique (ex: `data-price`) aux prix que l'on doit pouvoir modifier :

```html
<tr><td>Inscription dans l'Auto-École</td><td>70€</td></tr>

<tr><td>Inscription dans l'Auto-École</td><td class="data-price-inscription">70€</td></tr>
```

L'application Python pourra alors cibler : `soup.find(class_="data-price-inscription")`.

### B. La Fonction de Gestion Git (GitPython)

Vous devez encapsuler les étapes Git dans une fonction sécurisée.

```python
from git import Repo

def push_to_github(repo_path, commit_message):
    try:
        # Initialise le dépôt
        repo = Repo(repo_path)
        
        # 1. Pull (pour éviter les conflits)
        print("Tirage des dernières modifications...")
        repo.remotes.origin.pull()
        
        # 2. Add et Commit (Ajouter tous les changements, mais vous pouvez être plus sélectif)
        print("Commit des changements...")
        repo.index.add(['page/tarifs.html']) # Seulement le fichier modifié
        repo.index.commit(commit_message)
        
        # 3. Push
        print("Push vers GitHub...")
        origin = repo.remote(name='origin')
        origin.push()
        
        return "SUCCÈS : Le site est en cours de mise à jour sur GitHub Pages !"
        
    except Exception as e:
        return f"ERREUR GIT : {e}"
```

-----

## 🖥️ 3. L'Interface de Bureau (Semaines 3-4)

L'UI doit être simple et directe. Voici un schéma minimaliste (avec des exemples Tkinter) :

### Éléments Clés de l'UI

1.  **Champ de Répertoire :** Pour que l'utilisateur indique où se trouve le dossier local.
2.  **Champs de Formulaire des Prix :** Pour afficher et modifier les valeurs.
3.  **Champ de Message de Commit :** Pour la traçabilité.
4.  **Bouton d'Action :** Le déclencheur unique.

### Exemple de Structure

```python
import tkinter as tk
from tkinter import messagebox
# ... (imports des fonctions HTML et Git)

class AutoEcoleApp:
    def __init__(self, master):
        self.master = master
        master.title("Rout'Art - Outil de Mise à Jour (MVP)")
        self.repo_path = tk.StringVar(value="./") # Chemin par défaut

        # --- Section 1: Répertoire Local ---
        tk.Label(master, text="Dossier Local du Site :").grid(row=0, column=0, sticky="w")
        tk.Entry(master, textvariable=self.repo_path, width=40).grid(row=0, column=1)

        # --- Section 2: Champs de Modification des Prix (Exemple) ---
        
        # Champ pour Inscription
        tk.Label(master, text="Prix Inscription (€) :").grid(row=2, column=0, sticky="w", pady=5)
        self.price_inscription = tk.StringVar()
        tk.Entry(master, textvariable=self.price_inscription).grid(row=2, column=1)
        
        # Champ pour Heure de Conduite
        tk.Label(master, text="Prix Heure Conduite (€/h) :").grid(row=3, column=0, sticky="w", pady=5)
        self.price_hour = tk.StringVar()
        tk.Entry(master, textvariable=self.price_hour).grid(row=3, column=1)

        # --- Section 3: Action ---
        tk.Label(master, text="Message de Commit :").grid(row=5, column=0, sticky="w", pady=10)
        self.commit_msg = tk.StringVar(value="Mise à jour des tarifs")
        tk.Entry(master, textvariable=self.commit_msg, width=40).grid(row=5, column=1)
        
        # Le bouton magique
        tk.Button(master, text="Mettre le Site à Jour et Publier", 
                  command=self.publish_action, bg="green", fg="white", font=('Arial', 12, 'bold')).grid(row=6, column=0, columnspan=2, pady=20)
        
    def publish_action(self):
        # 1. Mise à jour du HTML (fonction à coder)
        # update_html_price(self.repo_path.get(), "data-price-inscription", self.price_inscription.get())
        
        # 2. Push Git
        result = push_to_github(self.repo_path.get(), self.commit_msg.get())
        
        # 3. Affichage du résultat
        messagebox.showinfo("Résultat de la publication", result)

# root = tk.Tk()
# app = AutoEcoleApp(root)
# root.mainloop()
```