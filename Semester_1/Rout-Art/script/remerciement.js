/**
 * Script pour gérer le changement des images selon le thème
 * Change les icônes des réseaux sociaux en fonction du mode clair/sombre
 */

// Fonction pour mettre à jour les images selon le thème
function updateSocialIconsForTheme() {
    const isDarkMode = document.documentElement.getAttribute('data-theme') === 'dark';

    // GitHub icon (section réseaux sociaux)
    const githubImg = document.querySelector('img[alt="Logo GitHub"]');
    if (githubImg) {
        githubImg.src = isDarkMode
            ? '../images/remerciement/github_ico_darkmode.svg'
            : '../images/remerciement/github_ico.svg';
    }

    // Portfolio icon
    const portfolioImg = document.querySelector('img[alt="Logo Portfolio"]');
    if (portfolioImg) {
        portfolioImg.src = isDarkMode
            ? '../images/remerciement/portfolio_ico_darkmode.png'
            : '../images/remerciement/portfolio_ico.png';
    }

    // HTML icon (section technologies)
    const htmlImg = document.querySelector('img[alt="logo html"]');
    if (htmlImg) {
        htmlImg.src = isDarkMode
            ? '../images/remerciement/techno/html_dark.png'
            : '../images/remerciement/techno/html.png';
    }

    // GitHub icon (section technologies)
    const githubTechImg = document.querySelector('img[alt="logo github"]');
    if (githubTechImg) {
        githubTechImg.src = isDarkMode
            ? '../images/remerciement/techno/github_ico_dark.svg'
            : '../images/remerciement/techno/github_ico.svg';
    }
}

// Mettre à jour les images au chargement de la page
document.addEventListener('DOMContentLoaded', updateSocialIconsForTheme);

// Observer pour les changements de thème
const observer = new MutationObserver((mutations) => {
    mutations.forEach((mutation) => {
        if (mutation.attributeName === 'data-theme') {
            updateSocialIconsForTheme();
        }
    });
});

// Configurer l'observateur pour surveiller les changements d'attribut
observer.observe(document.documentElement, {
    attributes: true,
    attributeFilter: ['data-theme']
});
