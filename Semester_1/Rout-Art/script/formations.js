document.addEventListener('DOMContentLoaded', function() {
    const select = document.getElementById('formation-select');
    const tables = document.querySelectorAll('#formation-details table');
    const heroImg = document.querySelector('.hero-image');

    // Read 'formation' from query string or from hash (supports both approaches)
    function getFormationFromUrl() {
        try {
            const params = new URLSearchParams(window.location.search);
            const q = params.get('formation');
            if (q) return q;
        } catch (e) {
            // ignore
        }
        // fallback to hash like #b-traditionnel
        if (window.location.hash) {
            return window.location.hash.replace('#', '');
        }
        return null;
    }

    function showTable(selected) {
        tables.forEach(table => {
            table.style.display = 'none';
        });
        const table = document.getElementById('table-' + selected);
        if (table) {
            table.style.display = 'table';
        }
    }

    function updateHeroImage(selected) {
        if (selected === 'b-traditionnel') {
            heroImg.src = '../images/voiture/permis-b-traditionnel.png';
        } else if (selected === 'b-accompagnee' || selected === 'b-supervisee') {
            heroImg.src = '../images/voiture/permis-b-accompagnee.png';
        } else if (selected === 'b-automatique') {
            heroImg.src = '../images/voiture/permis-b-automatique.png';
        } else if (selected === 'moto-a2' || selected === 'passerelle-a2-a') {
            heroImg.src = '../images/moto/permis-a2.png';
        } else if (selected === 'moto-a1' || selected === 'formation-125cc') {
            heroImg.src = '../images/moto/permis-a1-125.png';
        } else if (selected === 'am-moto') {
            heroImg.src = '../images/moto/am-moto.png';
        } else if (selected === 'am-voiture') {
            heroImg.src = '../images/voiture/am-voiture.png';
        } else {
            heroImg.src = '../images/hero-car-road.png';
        }
    }

    // If a formation is specified in the URL, pre-select it
    const pre = getFormationFromUrl();
    if (pre) {
        // Ensure the option exists before setting it
        const opt = Array.from(select.options).find(o => o.value === pre);
        if (opt) {
            select.value = pre;
        }
    }

    // Ensure we have a selected value
    const selectedValue = select.value || 'b-traditionnel';
    
    showTable(selectedValue);
    updateHeroImage(selectedValue);

    select.addEventListener('change', function() {
        showTable(this.value);
        updateHeroImage(this.value);
        });
});