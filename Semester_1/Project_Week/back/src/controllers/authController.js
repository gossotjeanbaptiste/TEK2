const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const pool = require('../config/database');

exports.login = async (req, res) => {
    try {
        const { email, password } = req.body;

        // Validation
        if (!email || !password) {
            return res.status(400).json({ 
                success: false,
                message: 'Email et mot de passe requis' 
            });
        }

        // Récupérer l'utilisateur
        const connection = await pool.getConnection();
        const [users] = await connection.query(
            'SELECT id, email, password, nom, prenom, solde FROM users WHERE email = ?',
            [email]
        );
        connection.release();

        if (users.length === 0) {
            return res.status(401).json({ 
                success: false,
                message: 'Email ou mot de passe incorrect' 
            });
        }

        const user = users[0];

        // Vérifier le mot de passe
        const isPasswordValid = await bcrypt.compare(password, user.password);
        if (!isPasswordValid) {
            return res.status(401).json({ 
                success: false,
                message: 'Email ou mot de passe incorrect' 
            });
        }

        // Créer le JWT
        const token = jwt.sign(
            { id: user.id, email: user.email },
            process.env.JWT_SECRET,
            { expiresIn: process.env.JWT_EXPIRE }
        );

        console.log(`✅ Connexion réussie pour ${email}`);

        // Retourner la réponse
        res.status(200).json({
            success: true,
            message: 'Connexion réussie',
            token,
            user: {
                id: user.id,
                email: user.email,
                nom: user.nom,
                prenom: user.prenom,
                solde: user.solde,
                solde_max: user.solde_max,
                userAge: user.userAge
            }
        });
    } catch (error) {
        console.error('❌ Erreur login:', error);
        res.status(500).json({ 
            success: false,
            message: 'Erreur serveur' 
        });
    }
};

exports.register = async (req, res) => {
    try {
        const { email, password, nom, prenom } = req.body;

        // Validation
        if (!email || !password || !nom || !prenom) {
            return res.status(400).json({ 
                success: false,
                message: 'Tous les champs sont requis' 
            });
        }

        // Vérifier si l'utilisateur existe
        const connection = await pool.getConnection();
        const [existingUsers] = await connection.query(
            'SELECT id FROM users WHERE email = ?',
            [email]
        );

        if (existingUsers.length > 0) {
            connection.release();
            return res.status(409).json({ 
                success: false,
                message: 'Cet email existe déjà' 
            });
        }

        // Hasher le mot de passe
        const hashedPassword = await bcrypt.hash(password, 10);

        // Créer l'utilisateur
        await connection.query(
            'INSERT INTO users (email, password, nom, prenom) VALUES (?, ?, ?, ?)',
            [email, hashedPassword, nom, prenom]
        );
        connection.release();

        console.log(`✅ Nouvel utilisateur créé: ${email}`);

        res.status(201).json({ 
            success: true,
            message: 'Utilisateur créé avec succès' 
        });
    } catch (error) {
        console.error('❌ Erreur register:', error);
        res.status(500).json({ 
            success: false,
            message: 'Erreur serveur' 
        });
    }
};

exports.getProfile = async (req, res) => {
    try {
        const userId = req.user.id;

        const connection = await pool.getConnection();
        const [users] = await connection.query(
            'SELECT id, email, nom, prenom, solde, solde_max, created_at FROM users WHERE id = ?',
            [userId]
        );
        connection.release();

        if (users.length === 0) {
            return res.status(404).json({
                success: false,
                message: 'Utilisateur non trouvé'
            });
        }

        const user = users[0];
        res.status(200).json({
            success: true,
            user: {
                id: user.id,
                email: user.email,
                nom: user.nom,
                prenom: user.prenom,
                solde: user.solde,
                solde_max: user.solde_max,
                created_at: user.created_at
            }
        });
    } catch (error) {
        console.error('❌ Erreur getProfile:', error);
        res.status(500).json({
            success: false,
            message: 'Erreur serveur'
        });
    }
};

exports.updateSolde = async (req, res) => {
    try {
        const userId = req.user.id;
        const { solde } = req.body;

        if (solde === undefined || solde === null) {
            return res.status(400).json({
                success: false,
                message: 'Le solde est requis'
            });
        }

        const newSolde = parseFloat(solde);
        const connection = await pool.getConnection();

        // Récupérer le solde_max actuel
        const [users] = await connection.query(
            'SELECT solde_max FROM users WHERE id = ?',
            [userId]
        );

        if (users.length === 0) {
            connection.release();
            return res.status(404).json({
                success: false,
                message: 'Utilisateur non trouvé'
            });
        }

        const currentSoldeMax = parseFloat(users[0].solde_max);
        const newSoldeMax = Math.max(newSolde, currentSoldeMax);

        // Mettre à jour solde et solde_max si nécessaire
        await connection.query(
            'UPDATE users SET solde = ?, solde_max = ? WHERE id = ?',
            [newSolde, newSoldeMax, userId]
        );
        connection.release();

        console.log(`✅ Solde mis à jour pour utilisateur ${userId}: ${newSolde} (max: ${newSoldeMax})`);

        res.status(200).json({
            success: true,
            message: 'Solde mis à jour',
            user: {
                solde: newSolde,
                solde_max: newSoldeMax
            }
        });
    } catch (error) {
        console.error('❌ Erreur updateSolde:', error);
        res.status(500).json({
            success: false,
            message: 'Erreur serveur'
        });
    }
};
