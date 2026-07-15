# Script SQL pour créer la base de données Thuun

# Créer la base de données
CREATE DATABASE IF NOT EXISTS thuun_db;
USE thuun_db;

# Créer la table users
CREATE TABLE IF NOT EXISTS users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    email VARCHAR(255) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    nom VARCHAR(100) NOT NULL,
    prenom VARCHAR(100) NOT NULL,
    solde DECIMAL(15, 2) DEFAULT 0.00,
    solde_max DECIMAL(15, 2) DEFAULT 0.00,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

# Créer un utilisateur de test (mot de passe: test123)
INSERT INTO users (email, password, nom, prenom, solde, solde_max) VALUES 
('test@thuun.com', '$2a$10$8Z8Z8Z8Z8Z8Z8Z8Z8Z8Z8O8Z8Z8Z8Z8Z8Z8Z8Z8Z8Z8Z8Z8Z', 'Dupont', 'Jean', 50000.00, 50000.00);

# Index pour optimiser les requêtes
CREATE INDEX idx_email ON users(email);
