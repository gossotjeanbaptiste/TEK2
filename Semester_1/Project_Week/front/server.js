const http = require('http');
const fs = require('fs');
const path = require('path');

const PORT = 3000;
const frontDir = __dirname;

const mimeTypes = {
    '.html': 'text/html',
    '.css': 'text/css',
    '.js': 'application/javascript',
    '.json': 'application/json',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.gif': 'image/gif',
    '.svg': 'image/svg+xml'
};

const server = http.createServer((req, res) => {
    // Logger
    console.log(`📨 ${req.method} ${req.url}`);

    // Redirection automatique vers index.html
    let filePath = req.url === '/' ? '/index.html' : req.url;
    filePath = path.join(frontDir, filePath);

    // Éviter les remontées de répertoire
    if (!filePath.startsWith(frontDir)) {
        res.statusCode = 403;
        res.end('Accès refusé');
        return;
    }

    fs.readFile(filePath, (err, content) => {
        if (err) {
            if (err.code === 'ENOENT') {
                res.statusCode = 404;
                res.end('Fichier non trouvé');
            } else {
                res.statusCode = 500;
                res.end('Erreur serveur');
            }
        } else {
            const ext = path.extname(filePath);
            res.statusCode = 200;
            res.setHeader('Content-Type', mimeTypes[ext] || 'application/octet-stream');
            res.setHeader('Access-Control-Allow-Origin', '*');
            res.end(content);
        }
    });
});

server.listen(PORT, () => {
    console.log(`\n🚀 ========================================`);
    console.log(`🚀 Frontend Thuun ACTIF`);
    console.log(`🚀 http://localhost:${PORT}`);
    console.log(`🚀 ========================================\n`);
});
