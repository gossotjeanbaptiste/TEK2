# Choix Techniques Importants du Projet Raytracer

## 1. Architecture générale : Pattern Polymorphe avec Interfaces Abstraites

### Choix : `IPrimitive` et `ILight` comme classes abstraites
**Implémentation:**
- Toutes les primitives (Sphère, Cylindre, Cône, etc.) héritent de `IPrimitive`
- Tous les éclairages (Ambient, Directional, AmbientOcclusion) héritent de `ILight`
- Utilisation de méthodes virtuelles pures

**Pourquoi ce choix ?**
- **Extensibilité** : Ajouter une nouvelle primitive = créer une classe qui hérite de `IPrimitive` sans modifier le code existant (principe Open/Closed)
- **Flexibilité** : La scène stocke des `shared_ptr<IPrimitive>` et `shared_ptr<ILight>`, pas des objets concrets. Permet de traiter tous les objets uniformément
- **Maintenance** : Changer l'implémentation d'une sphère ne casse pas le reste du code
- **Alternative rejetée** : Utiliser une seule classe `Primitive` avec un switch/case sur le type → code rigide et non scalable

---

## 2. Gestion Mémoire : `std::shared_ptr` + `std::make_shared`

### Choix : Pointeurs intelligents au lieu de `new`/`delete`
**Implémentation:**
```cpp
std::shared_ptr<IPrimitive> sphere = std::make_shared<Sphere>(pos, radius, color);
scene.addPrimitive(sphere);
```

**Pourquoi ce choix ?**
- **Pas de fuite mémoire** : Destruction automatique quand refcount = 0
- **Une seule allocation** : `std::make_shared` alloue primitive ET compteur en une seule opération (vs `new Sphere + shared_ptr()` = 2 allocations)
- **Sécurité exception** : Même si une exception est levée, la mémoire est correctement libérée
- **Gestion d'héritage** : `shared_ptr<IPrimitive>` peut pointer vers une Sphere, Cone, etc. sans problème
- **Alternative rejetée** : Utiliser `delete` manuel → erreurs humaines, code fragile, maintenabilité cauchemardesque

---

## 3. Types Mathématiques Spécialisés : `Vector3D` vs `std::vector`

### Choix : Classe `Math::Vector3D` personnalisée au lieu de `std::vector<double>`
**Implémentation:**
```cpp
class Vector3D {
    double x, y, z;  // Toujours 3 coordonnées
    double length();
    double dot(const Vector3D& other);
    // ... opérateurs +, -, *, etc.
};
```

**Pourquoi ce choix ?**
- **Sémantique correcte** : Un vecteur mathématique ≠ conteneur dynamique
  - `std::vector<double>` a taille variable (0, 1, 4, N éléments possibles)
  - `Vector3D` a **toujours** exactement 3 coordonnées
- **Performance** : 
  - `Vector3D` est petit (24 bytes) → stack allocation → pas de pointeur indirection
  - `std::vector` ajoute surcharge de pointeur/capacité/taille
- **Opérations mathématiques** : `dot()`, `cross()` sont explicites et optimisées
- **Lisibilité** : Code plus compréhensible : `camera.position` retourne `Point3D` (pas `std::vector`)
- **Alternative rejetée** : Utiliser `Eigen::Vector3d` → ajout dépendance externe inutile pour du simple 3D + pas autorisé par le sujet

---

## 4. Distinction `Point3D` vs `Vector3D`

### Choix : Deux types séparés pour position et direction
**Implémentation:**
- `Point3D` : position absolue (x, y, z)
- `Vector3D` : direction/déplacement (dx, dy, dz)
- `Ray` utilise `Point3D origin` et `Vector3D direction`

**Pourquoi ce choix ?**
- **Prévention d'erreurs** : Empêche de faire `camera_position + light_position` (incohérent mathématiquement)
- **Clarté sémantique** : Quand on demande "position de la caméra", on s'attend à `Point3D`, pas `Vector3D`
- **Opérations valides** : 
  - `Point3D + Vector3D = Point3D` ✓ (translation)
  - `Point3D - Point3D = Vector3D` ✓ (direction entre deux points)
  - `Vector3D + Vector3D = Vector3D` ✓ (composition de déplacements)
- **Type Safety** : Le compilateur empêche les opérations invalides

---

## 5. Rendering : Multithreading avec Work-Stealing

### Choix : Architecture travail/queue atomique plutôt que thread pool classique
**Implémentation:**
```cpp
std::atomic<std::size_t> nextTask(0);
// Chaque thread prend le prochain pixel indépendamment
std::size_t currentTask = nextTask.fetch_add(1, std::memory_order_relaxed);
```

**Pourquoi ce choix ?**
- **Load balancing automatique** : Les pixels complexes ne ralentissent pas les autres threads
  - Exemple : Pixel avec beaucoup de géométrie vs pixel vide
- **Pas de synchronisation** : Utilisation de `fetch_add` atomique (non-blocking)
- **Cache friendly** : Accès séquentiel à `renderedPixels` vector (contiguous memory)
- **Scalabilité** : Nombre de threads = `hardware_concurrency` (adapte automatiquement)
- **Alternative rejetée** : Diviser l'image en tiles → workload imbalancé, synchronisation supplémentaire

---

## 6. Stockage des Couleurs : `std::vector<Vector3D>` pour Pixels

### Choix : Vector3D comme conteneur pour (R, G, B)
**Implémentation:**
```cpp
std::vector<Math::Vector3D> renderedPixels;  // Chaque pixel = (R, G, B)
```

**Pourquoi ce choix ?**
- **Perfection sémantique** : Couleur RGB = 3 valeurs, exactement comme `Vector3D`
- **Cache efficiency** : Accès séquentiel à un vector continu en mémoire
- **Thread-safe indexing** : Chaque thread écrit dans un index différent de `renderedPixels`
- **Réutilisabilité** : Opérations vectorielles (multiplication, addition) fonctionnent naturellement
- **Alternative rejetée** : Utiliser `struct Color { uint8_t r, g, b; }` → casting nécessaire, moins flexible

---

## 7. Stockage Primitives/Lumières : `shared_ptr` dans `std::vector`

### Choix : Conteneur de pointeurs intelligents plutôt que d'objets directs
**Implémentation:**
```cpp
class Scene {
    std::vector<std::shared_ptr<IPrimitive>> primitives;
    std::vector<std::shared_ptr<ILight>> lights;
};
```

**Pourquoi ce choix ?**
- **Taille variable** : Scene peut avoir 1 ou 1000 objets
- **Polymorphisme** : Un vector d'interfaces pointant vers différents types concrets
- **Performance accès** : Pointeur = 8 bytes = indirection unique O(1)
- **Pas de slicing** : `shared_ptr<IPrimitive>` ne "slice" pas une Sphere au lieu d'un vector d'objets
- **Flexibilité ajout/suppression** : Facile d'ajouter/retirer des primitives sans invalidation d'adresses

---

## 8. Transformations : Pattern Décorateur (`TransformedPrimitive`)

### Choix : Envelopper la primitive plutôt que la modifier
**Implémentation:**
```cpp
class TransformedPrimitive : public IPrimitive {
    std::shared_ptr<IPrimitive> primitive;
    Transform transform;
    HitRecord hit(const Ray &ray) const override;  // Transform ray, appelle primitive, untransform
};
```

**Pourquoi ce choix ?**
- **Immutabilité** : Primitives de base restent inchangées
- **Composabilité** : On peut transformer une sphère, un cône, etc. avec le même code
- **Mathématique correct** : 
  1. Transformer le rayon dans l'espace local
  2. Calculer intersection avec primitive untransformed
  3. Transformer les résultats back
- **Pas de duplication** : Pas besoin de `TransformedSphere`, `TransformedCone`, etc.
- **Alternative rejetée** : Ajouter transformation à chaque classe primitive → code dupliqué, maintenance difficile

---

## 9. Fichiers Configuration : Format Texte Parsé

### Choix : Format `.cfg` texte au lieu de format binaire ou XML
**Implémentation:**
```
sphere: -2 0 -3 0.4 255 0 0
cone: 0 0 -2.5 0.4 0.15 0 1 0 255 100 200
plane: 0 2 -5 0 -1 0 200 200 200
```

**Pourquoi ce choix ?**
- **Humanement lisible** : Facile de déboguer, modifier, comprendre
- **Pas dépendance binaire** : Pas besoin de parseur spécialisé
- **Git-friendly** : Diffs visibles et compréhensibles
- **Flexible** : Ajouter nouvelles primitives/paramètres facile
- **Alternative rejetée** : 
  - Format binaire → pas editable à la main, moins flexible
  - XML → verbeux, overhead parsing
  - JSON → overkill pour cette utilisation

---

## 10. Intersection Rayons : Approche Naive (O(n)) vs Acceleration Structures

### Choix : Boucle O(n) sur toutes les primitives (pas de BVH/KD-tree)
**Implémentation:**
```cpp
HitRecord Scene::raycast(const Ray &ray) const {
    HitRecord closest; closest.t = INFINITY;
    for (auto &prim : primitives) {
        HitRecord h = prim->hit(ray);
        if (h.hit && h.t < closest.t) closest = h;
    }
    return closest;
}
```

**Pourquoi ce choix ?**
- **Simplicité** : Implémentation triviale, facile à debuguer
- **Suffisant pour la démo** : 150-200 objets max, pas millions
- **Overhead de BVH** : Pour petit nombre d'objets, structures complexes ralentissent plus qu'elles n'aident
- **Maintenance** : Moins de code = moins de bugs
- **Amélioration future** : Si nécessaire, BVH/KD-tree plug-in behind interface
- **Alternative rejetée** : BVH prématurément → complex, overhead pas justifié pour cette taille

---

## 11. Éclairage : Modèle Phong Simplifié (Ambient + Directional + AO)

### Choix : Pas de Specular, Pas de Global Illumination
**Implémentation:**
- Lumière ambiante : illumination globale simplifiée
- Lumière directionnelle : ombrage basique
- Ambient Occlusion : shadow approximation

**Pourquoi ce choix ?**
- **Perf/Qualité tradeoff** : Phong = bon résultat rapide
- **Pas besoin specular** : Matériaux non-brillants suffisent pour démo
- **AO comme approximation** : Remplace Global Illumination complexe de manière acceptable
- **Raytracing global** : Trop complex, besoin Monte Carlo sampling → exponential temps
- **Alternative rejetée** : Path tracing → temps rendu 1000x plus long, hors scope

---

## 12. Parser : Approche Stateful avec Transform Pending

### Choix : État "transformation en attente" plutôt que parsing post-hoc
**Implémentation:**
```cpp
Transform pendingTransform;
// Si on voit "transform: rotate 45 45 45", on l'accumule
if (line == "transform: rotate...") parseTransform(..., pendingTransform);
// Au prochain primitive, on l'applique
if (line == "sphere...") {
    parseSphere(..., pendingTransform);
    pendingTransform.reset();  // Clear
}
```

**Pourquoi ce choix ?**
- **Ordre d'application préservé** : Transform appliqué au primitive suivant
- **Simple** : Une passe de parsing, pas de graph AST complexe
- **Format naturel** : Format `.cfg` a cet ordre séquentiellement
- **Alternative rejetée** : 
  - Parser à deux passes → complexe
  - Graph AST → overkill pour format simple
  - Regex matching → fragile

---

## 13. Transparency : Recursive Raytracing avec Max Depth

### Choix : Récursion avec profondeur max (10) plutôt que probabiliste
**Implémentation:**
```cpp
Math::Vector3D Scene::traceRayWithTransparency(const Ray &ray, int maxDepth = 10) const {
    if (maxDepth == 0) return backgroundColor;
    // ... intersection
    // Si transparent (alpha < 1.0), continuer récursivement
    return shade(...) + traceRayWithTransparency(refractedRay, maxDepth - 1);
}
```

**Pourquoi ce choix ?**
- **Déterministe** : Même résultat à chaque run
- **Contrôle** : Depth limit = control qualité/perf tradeoff
- **Implémentation simple** : 10 lignes, pas Monte Carlo sampling complexe
- **Alternative rejetée** : 
  - Probabilistic russian roulette → denoise nécessaire
  - Pas de transparency → limitation majeure

---

## 14. Parser Erreurs : Line Numbers + Messages Clairs

### Choix : Logging détaillé avec numéro de ligne plutôt que silent failures
**Implémentation:**
```cpp
if (!parseCamera(..., lineNum)) {
    std::cerr << "Line " << lineNum << ": Invalid camera format\n";
    return false;
}
```

**Pourquoi ce choix ?**
- **Debuggable** : Utilisateur sait exactement où est le problème
- **User experience** : Pas de silence radio "parsing failed"
- **Alternative rejetée** : Silent fail → impossible debugger fichiers `.cfg` cassés

---

## 15. Output : Format PPM au lieu de PNG/JPG

### Choix : PPM (Portable PixMap) simple vs format compressé
**Implémentation:**
```cpp
file << "P6\n" << width << " " << height << "\n255\n";
for (auto &pixel : pixels) {
    file.put(static_cast<unsigned char>(clamp(pixel.x)));
    file.put(static_cast<unsigned char>(clamp(pixel.y)));
    file.put(static_cast<unsigned char>(clamp(pixel.z)));
}
```

**Pourquoi ce choix ?**
- **Implémentation triviale** : Zéro dépendance, ~10 lignes
- **Non-compressed** : Pas de artefacts de compression
- **Universalité** : Tous les viewers supportent PPM
- **Debug friendly** : Texture brute = inspection facile
- **Alternative rejetée** : PNG/JPG → dépendances libpng/libjpeg, overhead injection

---

## 16. Hit Record : Structure Centralisée au lieu de Retours Multiples

### Choix : `HitRecord` contient `(t, point, normal, color, hit)` au lieu de retourner tuple/array
**Implémentation:**
```cpp
struct HitRecord {
    double t;
    Math::Point3D point;
    Math::Vector3D normal;
    Math::Vector4D color;  // RGBA
    bool hit;
};
```

**Pourquoi ce choix ?**
- **Nommage** : Champs nommés vs indices magiques (tuple[0], tuple[1])
- **Extensibilité** : Ajouter `uv coordinates`, `material ID` facile
- **Lisibilité** : `record.normal` vs `std::get<2>(ray_hit)`
- **Type safety** : Compilateur vérifie tous les champs
- **Alternative rejetée** : `tuple<bool, double, Point3D, ...>` → code illisible

---

## 17. Normalisation Vecteurs : Lazy vs Eager

### Choix : Normaliser à la demande (lazy) plutôt que systématiquement (eager)
**Implémentation:**
```cpp
Vector3D direction = ...;  // Pas normalisé
double invLen = 1.0 / direction.length();
Vector3D normalized = direction * invLen;  // Normalisé when needed
```

**Pourquoi ce choix ?**
- **Performance** : Pas de sqrt inutile si vecteur pas utilisé
- **Flexibilité** : Magnitudes significatives préservées temporairement
- **Cache** : Moins de calculs = mieux
- **Alternative rejetée** : Normaliser toujours → calculs inutiles 30% du temps

---

## 18. Primeur sur Cones/Cylinders Limités : Séparation Classes

### Choix : `LimitedCone` et `LimitedCylinder` comme classes séparées vs booléen
**Implémentation:**
```cpp
class Cone : public IPrimitive { ... };
class LimitedCone : public IPrimitive { ... };  // Pas Cone(limited=true)
```

**Pourquoi ce choix ?**
- **Simplicité math** : Intersection cone infini vs fini = code différent
- **Performance** : Pas de conditions "if limited" inside hot path
- **Clarté** : Code explicite au parsing
- **Maintenance** : Chaque classe focus sur son cas
- **Alternative rejetée** : Flag `limited` → mélange concerns, plus complex

---

## 19. Ambient Occlusion : Sampling Aléatoire au lieu de Systématique

### Choix : Random ray sampling vs grid sampling
**Implémentation:**
```cpp
for (int i = 0; i < sampleCount; ++i) {
    // Random directions on hemisphere around normal
    Vector3D randomDir = getRandomHemisphereDirection(normal);
    if (scene.raycast(Ray(point, randomDir)).hit)
        occlusionCount++;
}
```

**Pourquoi ce choix ?**
- **Anti-aliasing gratuit** : Random sampling = less banding
- **Flexibilité** : Augmenter sampleCount = qualité progressive
- **Implémentation simple** : Random > grid pattern complex
- **Alternative rejetée** : Grid pattern → artefacts réguliers visibles

---

## Résumé : Principes Transversaux

1. **KISS** (Keep It Simple Stupid) : Pas de sur-engineering
2. **Polymorphisme** : Extensibilité via interfaces
3. **Mémoire sûre** : `shared_ptr` partout
4. **Type safety** : Distinctions Point3D/Vector3D
5. **Performance** : Multithreading, cache-friendly
6. **Maintenabilité** : Code lisible > optimisé prématurément
