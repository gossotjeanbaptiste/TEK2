const API_URL = `${window.location.protocol}//${window.location.hostname}:5000/api`;
const TOKEN_KEY = 'thuun_token';
const USER_KEY = 'thuun_user';

// Fonction pour créer et jouer un son de clic
function playClickSound() {
    try {
        const audioContext = new (window.AudioContext || window.webkitAudioContext)();
        const oscillator = audioContext.createOscillator();
        const gainNode = audioContext.createGain();
        
        oscillator.connect(gainNode);
        gainNode.connect(audioContext.destination);
        
        oscillator.frequency.setValueAtTime(800, audioContext.currentTime);
        oscillator.frequency.exponentialRampToValueAtTime(400, audioContext.currentTime + 0.1);
        
        gainNode.gain.setValueAtTime(0.3, audioContext.currentTime);
        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.1);
        
        oscillator.start(audioContext.currentTime);
        oscillator.stop(audioContext.currentTime + 0.1);
    } catch (e) {
        console.log('Son non disponible:', e);
    }
}

// Fonction pour gérer la connexion
async function handleLogin(event) {
    event.preventDefault();
    
    playClickSound();
    
    const email = document.getElementById('email').value;
    const password = document.getElementById('password').value;
    const errorDiv = document.getElementById('error-message');
    const loginBtn = event.target.querySelector('button[type="submit"]');
    
    // Réinitialiser le message d'erreur
    errorDiv.textContent = '';
    loginBtn.disabled = true;
    loginBtn.textContent = 'Connexion...';
    
    try {
        console.log('Envoi de la requête de connexion...');
        const response = await fetch(`${API_URL}/auth/login`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ email, password })
        });
        
        const data = await response.json();
        console.log('Réponse du serveur:', data);
        
        if (response.ok) {
            // Sauvegarder le token et les infos utilisateur
            localStorage.setItem(TOKEN_KEY, data.token);
            localStorage.setItem(USER_KEY, JSON.stringify(data.user));
            
            console.log('Connexion réussie, redirection...');
            
            // Redirection après un délai pour entendre le son
            setTimeout(() => {
                window.location.href = 'account.html';
            }, 100);
        } else {
            errorDiv.textContent = data.message || 'Erreur de connexion';
            loginBtn.disabled = false;
            loginBtn.textContent = 'Se connecter';
        }
    } catch (error) {
        console.error('Erreur:', error);
        errorDiv.textContent = 'Erreur de connexion. Vérifiez que le serveur est actif';
        loginBtn.disabled = false;
        loginBtn.textContent = 'Se connecter';
    }
}
// Charger les infos de l'utilisateur sur la page d'accueil
document.addEventListener('DOMContentLoaded', function() {
    // Vérifier si nous sommes sur la page home
    if (document.getElementById('user-name')) {
        const token = localStorage.getItem(TOKEN_KEY);
        const user = JSON.parse(localStorage.getItem(USER_KEY) || '{}');
        
        if (!token || !user.email) {
            // Rediriger vers la connexion si pas connecté
            console.log('Pas de token, redirection vers index.html');
            window.location.href = 'index.html';
            return;
        }
        
        document.getElementById('user-name').textContent = `Connecté en tant que ${user.email}`;
    }
    
    // Ajouter l'effet ripple aux boutons
    document.querySelectorAll('.nav-button, .login-btn').forEach(button => {
        button.addEventListener('click', function(e) {
            if (e.target.classList.contains('logout-btn')) return;
            
            let ripple = document.createElement('span');
            let rect = this.getBoundingClientRect();
            let size = Math.max(rect.width, rect.height);
            let x = e.clientX - rect.left - size / 2;
            let y = e.clientY - rect.top - size / 2;
            
            ripple.style.width = ripple.style.height = size + 'px';
            ripple.style.left = x + 'px';
            ripple.style.top = y + 'px';
            ripple.classList.add('ripple');
            
            this.appendChild(ripple);
            
            setTimeout(() => ripple.remove(), 600);
        });
    });
});

// Fonction de navigation
function navigateTo(page) {
    playClickSound();
    
    setTimeout(() => {
        switch(page) {
            case 'courses':
                console.log('Navigation vers Cours');
                window.location.href = 'courses.html';
                break;
            case 'account':
                console.log('Navigation vers Compte');
                window.location.href = 'account.html';
                break;
            case 'advisor':
                console.log('Navigation vers Conseiller');
                // window.location.href = 'advisor.html';
                break;
        }
    }, 100);
}

// Fonction pour gérer la déconnexion
function handleLogout() {
    playClickSound();
    
    setTimeout(() => {
        // Effacer les données
        localStorage.removeItem(TOKEN_KEY);
        localStorage.removeItem(USER_KEY);
        
        window.location.href = 'index.html';
    }, 100);
}

// Fonction pour revenir à l'accueil
function goBack() {
    playClickSound();
    
    setTimeout(() => {
        window.location.href = 'home.html';
    }, 100);
}
