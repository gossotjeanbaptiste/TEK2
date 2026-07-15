document.addEventListener('DOMContentLoaded', function() {
    const select = document.getElementById('tarif-select');
    const tables = document.querySelectorAll('#tarif-details table');
    const heroImg = document.querySelector('.hero-image');

    function showTable(selected) {
        tables.forEach(table => {
            table.style.display = 'none';
        });
        const table = document.getElementById('table-' + selected);
        if (table) {
            table.style.display = '';
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
    showTable(select.value);
    updateHeroImage(select.value);
    select.addEventListener('change', function() {
        showTable(this.value);
        updateHeroImage(this.value);
        });
});