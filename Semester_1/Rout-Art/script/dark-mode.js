/* ==========================================================================
   DARK MODE FUNCTIONALITY - Gestion du thème clair/sombre
   ========================================================================== */

class DarkModeToggle {
    constructor() {
        this.init();
    }

    init() {
        // Attacher l'événement au bouton de navigation
        this.attachToNavButton();
        
        // Charger le thème sauvegardé ou détecter les préférences système
        this.loadSavedTheme();
        
        // Écouter les changements de préférences système
        this.listenToSystemChanges();
    }

    attachToNavButton() {
        const navToggleButton = document.getElementById('theme-toggle-nav');
        if (navToggleButton) {
            navToggleButton.addEventListener('click', () => this.toggleTheme());
            this.toggleButton = navToggleButton;
        }
    }

    toggleTheme() {
        const currentTheme = document.documentElement.getAttribute('data-theme');
        const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
        
        this.setTheme(newTheme);
    }

    setTheme(theme) {
        // Appliquer le thème
        if (theme === 'dark') {
            document.documentElement.setAttribute('data-theme', 'dark');
            this.updateToggleText('Thème Clair');
        } else {
            document.documentElement.removeAttribute('data-theme');
            this.updateToggleText('Thème Sombre');
        }

        // Mettre à jour les images selon le thème
        this.updateThemeImages(theme);

        // Sauvegarder le choix
        localStorage.setItem('preferred-theme', theme);
        
        // Déclencher un événement personnalisé
        document.dispatchEvent(new CustomEvent('themeChanged', {
            detail: { theme: theme }
        }));
    }

    updateToggleText(text) {
        if (this.toggleButton) {
            this.toggleButton.textContent = text;
            this.toggleButton.setAttribute('aria-label', `Passer au ${text.toLowerCase()}`);
            this.toggleButton.setAttribute('title', `Passer au ${text.toLowerCase()}`);
        }
    }

    updateThemeImages(theme) {
        // Définir les correspondances d'images pour le mode sombre
        const imageMapping = {
            'france-travail.png': 'france-travail-sombre.png'
            // Ajouter d'autres correspondances d'images ici si nécessaire
            // 'autre-image.png': 'autre-image-sombre.png'
        };

        // Trouver toutes les images dans la page
        const images = document.querySelectorAll('img');
        
        images.forEach(img => {
            let currentSrc = img.src;
            let filename = currentSrc.split('/').pop(); // Obtenir le nom du fichier
            
            if (theme === 'dark') {
                // Passer au mode sombre
                for (const [lightImage, darkImage] of Object.entries(imageMapping)) {
                    if (filename === lightImage) {
                        // Remplacer par la version sombre
                        img.src = currentSrc.replace(lightImage, darkImage);
                        // Optionnel: mettre à jour l'alt text
                        if (img.alt && img.alt.includes('France Travail')) {
                            img.alt = img.alt + ' (mode sombre)';
                        }
                        break;
                    }
                }
            } else {
                // Passer au mode clair
                for (const [lightImage, darkImage] of Object.entries(imageMapping)) {
                    if (filename === darkImage) {
                        // Remplacer par la version claire
                        img.src = currentSrc.replace(darkImage, lightImage);
                        // Restaurer l'alt text original
                        if (img.alt && img.alt.includes('(mode sombre)')) {
                            img.alt = img.alt.replace(' (mode sombre)', '');
                        }
                        break;
                    }
                }
            }
        });
    }

    loadSavedTheme() {
        // Vérifier le thème sauvegardé
        const savedTheme = localStorage.getItem('preferred-theme');
        
        if (savedTheme) {
            this.setTheme(savedTheme);
        } else {
            // Détecter les préférences système
            const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
            this.setTheme(prefersDark ? 'dark' : 'light');
        }
    }

    listenToSystemChanges() {
        const mediaQuery = window.matchMedia('(prefers-color-scheme: dark)');
        
        mediaQuery.addEventListener('change', (e) => {
            // Ne changer automatiquement que si l'utilisateur n'a pas de préférence sauvegardée
            const savedTheme = localStorage.getItem('preferred-theme');
            if (!savedTheme) {
                this.setTheme(e.matches ? 'dark' : 'light');
            }
        });
    }

    // Méthode pour obtenir le thème actuel
    getCurrentTheme() {
        return document.documentElement.getAttribute('data-theme') || 'light';
    }

    // Méthode pour forcer un thème (utile pour debugging)
    forceTheme(theme) {
        this.setTheme(theme);
    }
}

// Initialiser le dark mode quand le DOM est chargé
document.addEventListener('DOMContentLoaded', () => {
    window.darkModeToggle = new DarkModeToggle();
    
    // Ajouter un raccourci clavier (Ctrl/Cmd + Shift + D)
    document.addEventListener('keydown', (e) => {
        if ((e.ctrlKey || e.metaKey) && e.shiftKey && e.key === 'D') {
            e.preventDefault();
            window.darkModeToggle.toggleTheme();
        }
    });
});

// Exporter pour utilisation globale
window.DarkModeToggle = DarkModeToggle;