const mysql = require('mysql2/promise');
const bcrypt = require('bcryptjs');
require('dotenv').config();

async function initDatabase() {
    try {
        console.log('🔧 Initialisation de la base de données...\n');

        // Connexion sans base de données pour créer la BD
        const connection = await mysql.createConnection({
            host: process.env.DB_HOST,
            user: process.env.DB_USER,
            password: process.env.DB_PASSWORD,
            port: process.env.DB_PORT
        });

        console.log('✅ Connecté au serveur MySQL');

        // Créer la base de données
        await connection.query(`CREATE DATABASE IF NOT EXISTS ${process.env.DB_DATABASE}`);
        console.log(`✅ Base de données '${process.env.DB_DATABASE}' créée/vérifiée`);

        // Se connecter à la base de données
        await connection.query(`USE ${process.env.DB_DATABASE}`);

        // Supprimer la table si elle existe
        await connection.query(`DROP TABLE IF EXISTS users`);
        console.log('✅ Table "users" supprimée');

        // Créer la table users
        await connection.query(`
            CREATE TABLE IF NOT EXISTS users (
                id INT PRIMARY KEY AUTO_INCREMENT,
                email VARCHAR(255) NOT NULL UNIQUE,
                password VARCHAR(255) NOT NULL,
                nom VARCHAR(100) NOT NULL,
                prenom VARCHAR(100) NOT NULL,
                userAge INT DEFAULT 0,
                solde DECIMAL(15, 2) DEFAULT 0.00,
                solde_max DECIMAL(15, 2) DEFAULT 0.00,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
            )
        `);
        console.log('✅ Table "users" créée/vérifiée');

        // Vérifier si l'utilisateur test existe
        const [users] = await connection.query(
            'SELECT id FROM users WHERE email = ?',
            ['test@tuun.com']
        );

        if (users.length === 0) {
            // Créer l'utilisateur de test
            const hashedPassword = await bcrypt.hash('test123', 10);
            await connection.query(
                'INSERT INTO users (email, password, nom, prenom, solde, solde_max) VALUES (?, ?, ?, ?, ?, ?)',
                ['test@tuun.com', hashedPassword, 'Dupont', 'Jean', 5000.00, 5000.00]
            );
            console.log('✅ Utilisateur test créé (test@tuun.com / test123)');
        } else {
            console.log('ℹ️  Utilisateur test existe déjà');
        }

        // Créer un index
        await connection.query('CREATE INDEX IF NOT EXISTS idx_email ON users(email)');
        console.log('✅ Index créé');

        await connection.end();
        console.log('\n🎉 Base de données initialisée avec succès!\n');
    } catch (error) {
        console.error('❌ Erreur:', error.message);
        process.exit(1);
    }
}

initDatabase();
