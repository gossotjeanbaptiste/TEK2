// Configuration API
const config = {
    API_URL: `${window.location.protocol}//${window.location.hostname}:5000/api`,
    TOKEN_KEY: 'thuun_token',
    USER_KEY: 'thuun_user'
};

// Exporter pour utilisation dans d'autres fichiers
if (typeof module !== 'undefined' && module.exports) {
    module.exports = config;
}
