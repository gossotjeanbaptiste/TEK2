const express = require('express');
const coursesController = require('../controllers/coursesController');
const authMiddleware = require('../middleware/auth');

const router = express.Router();

// Route publique - Récupérer tous les cours
router.get('/', coursesController.getCourses);

// Route publique - Récupérer les cours par catégorie d'âge
router.get('/age/:ageCategory', coursesController.getCoursesByAge);

// Route publique - Récupérer un cours spécifique
router.get('/:id', coursesController.getCourseById);

// Route protégée - Ajouter un cours (admin only)
router.post('/', authMiddleware, coursesController.addCourse);

module.exports = router;
