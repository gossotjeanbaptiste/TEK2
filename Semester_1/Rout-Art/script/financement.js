document.addEventListener('DOMContentLoaded', function () {
    // Fonctionnalité pour les boutons "Voir plus"
    document.querySelectorAll('.toggle-more').forEach(function(btn) {
        btn.addEventListener('click', function(e) {
            var container = e.target.closest('.collapsible');
            var more = container.querySelector('.more-content');
            var expanded = e.target.getAttribute('aria-expanded') === 'true';

            if (!container || !more)
                return;
            if (expanded) {
                more.hidden = true;
                e.target.textContent = 'Voir plus';
                e.target.setAttribute('aria-expanded', 'false');
                container.scrollIntoView({behavior: 'smooth', block: 'start'});
            } else {
                more.hidden = false;
                e.target.textContent = 'Voir moins';
                e.target.setAttribute('aria-expanded', 'true');
            }
        });
    });

    // Smooth scrolling pour les liens d'ancrage du sommaire
    document.querySelectorAll('.quick-nav-list a[href^="#"]').forEach(function(link) {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            
            const targetId = this.getAttribute('href').substring(1);
            const targetElement = document.getElementById(targetId);
            
            if (targetElement) {
                targetElement.scrollIntoView({
                    behavior: 'smooth',
                    block: 'start'
                });
            }
        });
    });
});
