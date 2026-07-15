const bcrypt = require('bcryptjs');
const pool = require('../config/database');

// ===== USERS =====
exports.getAllUsers = async (req, res) => {
    try {
        const connection = await pool.getConnection();
        const [users] = await connection.query(
            'SELECT id, email, nom, prenom, solde, solde_max, created_at FROM users ORDER BY created_at DESC'
        );
        connection.release();

        res.status(200).json({
            success: true,
            data: users,
            count: users.length
        });
    } catch (error) {
        console.error('❌ Erreur getAllUsers:', error);
        res.status(500).json({ success: false, message: 'Erreur serveur' });
    }
};

exports.createUser = async (req, res) => {
    try {
        const { email, password, nom, prenom, solde } = req.body;

        if (!email || !password || !nom || !prenom) {
            return res.status(400).json({
                success: false,
                message: 'Tous les champs sont requis'
            });
        }

        const connection = await pool.getConnection();
        
        // Vérifier si email existe
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
        const soldeValue = parseFloat(solde) || 0;

        // Créer l'utilisateur - solde_max = solde au départ
        await connection.query(
            'INSERT INTO users (email, password, nom, prenom, solde, solde_max) VALUES (?, ?, ?, ?, ?, ?)',
            [email, hashedPassword, nom, prenom, soldeValue, soldeValue]
        );
        connection.release();

        console.log(`✅ Nouvel utilisateur créé par admin: ${email}`);

        res.status(201).json({
            success: true,
            message: 'Utilisateur créé avec succès'
        });
    } catch (error) {
        console.error('❌ Erreur createUser:', error);
        res.status(500).json({ success: false, message: 'Erreur serveur' });
    }
};

exports.updateUser = async (req, res) => {
    try {
        const { id } = req.params;
        const { email, nom, prenom, solde, solde_max, age } = req.body;

        const connection = await pool.getConnection();

        const updates = [];
        const values = [];

        if (email) {
            updates.push('email = ?');
            values.push(email);
        }
        if (nom) {
            updates.push('nom = ?');
            values.push(nom);
        }
        if (prenom) {
            updates.push('prenom = ?');
            values.push(prenom);
        }
        if (age !== undefined && age !== null) {
            updates.push('userAge = ?');
            values.push(parseInt(age));
        }

        // Gérer le solde et solde_max
        let finalSoldeMax = solde_max;

        if (solde !== undefined && solde !== null) {
            updates.push('solde = ?');
            values.push(parseFloat(solde));

            // Si solde_max n'est pas fourni, le calculer automatiquement
            if (solde_max === undefined || solde_max === null) {
                const [users] = await connection.query(
                    'SELECT solde_max FROM users WHERE id = ?',
                    [id]
                );
                
                if (users.length > 0) {
                    const currentSoldeMax = parseFloat(users[0].solde_max) || 0;
                    const newSolde = parseFloat(solde);
                    finalSoldeMax = Math.max(newSolde, currentSoldeMax);
                } else {
                    finalSoldeMax = parseFloat(solde);
                }
            }
        }

        // Mettre à jour solde_max si nous l'avons déterminé
        if (finalSoldeMax !== undefined) {
            updates.push('solde_max = ?');
            values.push(parseFloat(finalSoldeMax));
        }

        if (updates.length > 0) {
            values.push(id);
            await connection.query(
                `UPDATE users SET ${updates.join(', ')} WHERE id = ?`,
                values
            );
            const logParts = [];
            if (solde !== undefined) logParts.push(`solde: ${solde}`);
            if (finalSoldeMax !== undefined) logParts.push(`solde_max: ${finalSoldeMax}`);
            if (age !== undefined) logParts.push(`age: ${age}`);
            console.log(`✅ Utilisateur ${id} modifié - ${logParts.join(', ')}`);
        }

        connection.release();

        res.status(200).json({
            success: true,
            message: 'Utilisateur mis à jour avec succès'
        });
    } catch (error) {
        console.error('❌ Erreur updateUser:', error);
        res.status(500).json({ success: false, message: 'Erreur serveur' });
    }
};

exports.deleteUser = async (req, res) => {
    try {
        const { id } = req.params;

        const connection = await pool.getConnection();
        await connection.query('DELETE FROM users WHERE id = ?', [id]);
        connection.release();

        console.log(`✅ Utilisateur ${id} supprimé par admin`);

        res.status(200).json({
            success: true,
            message: 'Utilisateur supprimé avec succès'
        });
    } catch (error) {
        console.error('❌ Erreur deleteUser:', error);
        res.status(500).json({ success: false, message: 'Erreur serveur' });
    }
};

// ===== COURSES =====
exports.getAllCourses = async (req, res) => {
    try {
        const fs = require('fs');
        const path = require('path');
        const coursesPath = path.join(__dirname, '../../data/courses.json');
        const coursesData = fs.readFileSync(coursesPath, 'utf8');
        const courses = JSON.parse(coursesData);

        res.status(200).json({
            success: true,
            data: courses,
            count: courses.length
        });
    } catch (error) {
        console.error('❌ Erreur getAllCourses:', error);
        res.status(500).json({ success: false, message: 'Erreur serveur' });
    }
};

exports.createCourse = async (req, res) => {
    try {
        const { nom, description, ageCategory, category, duree, niveau, instructor, quiz } = req.body;

        if (!nom || !description) {
            return res.status(400).json({
                success: false,
                message: 'Le nom et la description sont requis'
            });
        }

        const fs = require('fs');
        const path = require('path');
        const coursesPath = path.join(__dirname, '../../data/courses.json');
        const coursesData = fs.readFileSync(coursesPath, 'utf8');
        const courses = JSON.parse(coursesData);

        const newCourse = {
            id: Math.max(...courses.map(c => c.id), 0) + 1,
            nom,
            description,
            ageCategory: ageCategory || '12-14',
            category: category || 'Tous nos cours',
            duree: duree || '0:00',
            niveau: niveau || 'Débutant',
            instructor: instructor || 'Non assigné',
            video: '/videos/default.mp4',
            quiz: quiz || []
        };

        courses.push(newCourse);
        fs.writeFileSync(coursesPath, JSON.stringify(courses, null, 2));

        console.log(`✅ Cours créé par admin: ${nom}`);

        res.status(201).json({
            success: true,
            message: 'Cours créé avec succès',
            course: newCourse
        });
    } catch (error) {
        console.error('❌ Erreur createCourse:', error);
        res.status(500).json({ success: false, message: 'Erreur serveur' });
    }
};

exports.deleteCourse = async (req, res) => {
    try {
        const { id } = req.params;

        const fs = require('fs');
        const path = require('path');
        const coursesPath = path.join(__dirname, '../../data/courses.json');
        const coursesData = fs.readFileSync(coursesPath, 'utf8');
        const courses = JSON.parse(coursesData);

        const filteredCourses = courses.filter(c => c.id !== parseInt(id));

        if (filteredCourses.length === courses.length) {
            return res.status(404).json({
                success: false,
                message: 'Cours non trouvé'
            });
        }

        fs.writeFileSync(coursesPath, JSON.stringify(filteredCourses, null, 2));

        console.log(`✅ Cours ${id} supprimé par admin`);

        res.status(200).json({
            success: true,
            message: 'Cours supprimé avec succès'
        });
    } catch (error) {
        console.error('❌ Erreur deleteCourse:', error);
        res.status(500).json({ success: false, message: 'Erreur serveur' });
    }
};
