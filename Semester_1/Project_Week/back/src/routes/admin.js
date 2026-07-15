const express = require('express');
const adminController = require('../controllers/adminController');
const authMiddleware = require('../middleware/auth');

const router = express.Router();

// Routes protégées - Admin only
router.get('/users', authMiddleware, adminController.getAllUsers);
router.post('/users', authMiddleware, adminController.createUser);
router.put('/users/:id', authMiddleware, adminController.updateUser);
router.delete('/users/:id', authMiddleware, adminController.deleteUser);

router.get('/courses', authMiddleware, adminController.getAllCourses);
router.post('/courses', authMiddleware, adminController.createCourse);
router.delete('/courses/:id', authMiddleware, adminController.deleteCourse);

module.exports = router;
