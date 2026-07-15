const fs = require('fs');
const path = require('path');

const coursesPath = path.join(__dirname, '../../data/courses.json');

exports.getCourses = async (req, res) => {
    try {
        const coursesData = fs.readFileSync(coursesPath, 'utf8');
        const courses = JSON.parse(coursesData);
        
        res.status(200).json({
            success: true,
            data: courses,
            count: courses.length
        });
    } catch (error) {
        console.error('❌ Erreur getCourses:', error);
        res.status(500).json({ 
            success: false,
            message: 'Erreur serveur' 
        });
    }
};

exports.getCourseById = async (req, res) => {
    try {
        const { id } = req.params;
        const coursesData = fs.readFileSync(coursesPath, 'utf8');
        const courses = JSON.parse(coursesData);
        
        const course = courses.find(c => c.id === parseInt(id));
        
        if (!course) {
            return res.status(404).json({
                success: false,
                message: 'Cours non trouvé'
            });
        }
        
        res.status(200).json({
            success: true,
            data: course
        });
    } catch (error) {
        console.error('❌ Erreur getCourseById:', error);
        res.status(500).json({ 
            success: false,
            message: 'Erreur serveur' 
        });
    }
};

exports.getCoursesByAge = async (req, res) => {
    try {
        const { ageCategory } = req.params;
        const coursesData = fs.readFileSync(coursesPath, 'utf8');
        let courses = JSON.parse(coursesData);
        
        // Filtrer par catégorie d'âge
        const filteredCourses = courses.filter(c => c.ageCategory === ageCategory);
        
        res.status(200).json({
            success: true,
            data: filteredCourses,
            count: filteredCourses.length,
            ageCategory: ageCategory
        });
    } catch (error) {
        console.error('❌ Erreur getCoursesByAge:', error);
        res.status(500).json({ 
            success: false,
            message: 'Erreur serveur' 
        });
    }
};

exports.addCourse = async (req, res) => {
    try {
        const { nom, description, video, duree, niveau, instructor } = req.body;
        
        if (!nom || !description || !video) {
            return res.status(400).json({
                success: false,
                message: 'Champs requis: nom, description, video'
            });
        }
        
        const coursesData = fs.readFileSync(coursesPath, 'utf8');
        let courses = JSON.parse(coursesData);
        
        const newCourse = {
            id: Math.max(...courses.map(c => c.id), 0) + 1,
            nom,
            description,
            video,
            duree: duree || 'N/A',
            niveau: niveau || 'Débutant',
            instructor: instructor || 'Admin',
            date: new Date().toISOString().split('T')[0]
        };
        
        courses.push(newCourse);
        fs.writeFileSync(coursesPath, JSON.stringify(courses, null, 2));
        
        console.log(`✅ Nouveau cours créé: ${nom}`);
        
        res.status(201).json({
            success: true,
            message: 'Cours créé avec succès',
            data: newCourse
        });
    } catch (error) {
        console.error('❌ Erreur addCourse:', error);
        res.status(500).json({ 
            success: false,
            message: 'Erreur serveur' 
        });
    }
};
