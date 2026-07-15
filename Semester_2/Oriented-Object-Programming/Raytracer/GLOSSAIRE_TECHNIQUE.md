# Glossaire des Termes Techniques - Raytracer

## A

**Ambient Occlusion (AO)**
- Technique qui assombrit les zones où la lumière est "piégée"
- Exemple : coins d'une pièce, creux entre objets
- Simule l'ombre sans calculer l'illumination globale complexe
- Dans raytracer : utilise random sampling (16-32 rayons par pixel)

**Atomic (std::atomic)**
- Variable qui peut être accédée/modifiée de manière thread-safe
- Opération atomique = pas de race condition
- Exemple : `std::atomic<int> counter;` peut être incrémentée par plusieurs threads sans lock
- Dans raytracer : `nextTask` compte les pixels traités par tous les threads

**Axe (X, Y, Z)**
- Directions principales dans l'espace 3D
- X = gauche/droite, Y = haut/bas, Z = avant/arrière
- Chaque point/vecteur a une coordonnée pour chaque axe

---

## B

**BVH (Bounding Volume Hierarchy)**
- Structure d'accélération pour intersection rayon-géométrie
- Enveloppe les objets dans des boîtes imbriquées
- Permet sauter tests inutiles : si rayon ne touche pas la boîte, aucun objet dedans n'est touché
- Alternative à O(n) naïf : réduit à O(log n) en moyenne
- Raytracer n'utilise PAS car 150 objets = pas besoin (overhead > bénéfice)

---

## C

**Cache (Mémoire Cache)**
- Mémoire très rapide entre CPU et RAM (L1, L2, L3)
- Si données sont contiguës en mémoire = plus rapide à accéder
- Exemple : vector continu vs liste chaînée scattered
- Dans raytracer : `renderedPixels` vector est continu = cache-friendly

**Clamp (Écrêtage)**
- Limiter une valeur entre min et max
- Exemple : `clamp(255.5) = 255`, `clamp(-10) = 0`
- Dans raytracer : limitation couleurs à [0, 255]

**Conteneur (Container)**
- Structure qui stocke plusieurs éléments
- Exemples : `std::vector`, `std::array`, `std::list`
- Dans raytracer : `vector<shared_ptr<IPrimitive>>` = conteneur de primitives

**Contiguous Memory (Mémoire Contiguë)**
- Mémoire arrangée de manière consécutive sans "trous"
- `vector` = continu (rapide accès séquentiels)
- `list` = non-continu (slow random access)
- Dans raytracer : important pour multithreading, chaque thread accède index différent du même vector

**Cross Product (Produit Vectoriel)**
- Opération mathématique entre deux vecteurs → résultat vecteur perpendiculaire
- Exemple : `cross(forward, right) = up`
- Utilisé pour calculer normales de surfaces (Dans quel direction la surface est orientée)
- `a × b = (a.y*b.z - a.z*b.y, ...)`

---

## D

**Déterministe**
- Résultat toujours identique avec même input
- Contraire : probabiliste/aléatoire
- Raytracer utilise récursion avec max_depth = déterministe (vs Russian roulette = probabiliste)

**Dot Product (Produit Scalaire)**
- Opération mathématique entre deux vecteurs → résultat nombre
- Formule : `a·b = a.x*b.x + a.y*b.y + a.z*b.z`
- Mesure "similitude" direction (0 = perpendiculaires, >0 = même direction)
- Utilisé pour calcul ombrage Phong

---

## E

**Eigen (Bibliothèque)**
- Bibliothèque C++ pour algèbre linéaire (matrices, vecteurs)
- Alternative à Vector3D personnalisé
- Raytracer n'utilise PAS : "réinventer la roue" intentionnel pour apprentissage car interdit par le sujet

**Extensibilité**
- Capacité à ajouter de nouvelles fonctionnalités sans casser existant
- Pattern polymorphe = extensible (ajouter Torus sans toucher Sphere)
- Switch/case = non-extensible (ajouter type = modifier tous les switch)

---

## F

**Fetch_add (Opération Atomique)**
- Récupère valeur et l'incrémente = une opération atomique indivisible
- Thread-safe : deux threads ne lisent jamais la même valeur
- Dans raytracer : `nextTask.fetch_add(1)` = prendre prochain pixel à calculer

**Flag (Drapeau)**
- Variable booléenne pour indiquer état
- Exemple : `limited=true/false` pour Cone limité
- Raytracer utilise classes séparées (LimitedCone) au lieu de flag

---

## G

**Garbage Collection**
- Mécanisme de récupération mémoire automatique
- C++ n'a PAS de GC → utilise `delete` manuel ou smart pointers
- `shared_ptr` simule GC via reference counting

**Global Illumination**
- Calcul complet de la lumière qui rebondit dans la scène
- Très complexe, besoin Monte Carlo sampling
- Raytracer utilise Ambient Occlusion comme approximation simple

**GPU (Graphics Processing Unit)**
- Processeur spécialisé pour calculs parallèles massifs
- 1000s de cores vs CPU ~8 cores
- Raytracer utilise CPU (pas GPU) → multithreading classique

---

## H

**Hardware Concurrency**
- Nombre de threads parallèles que CPU peut exécuter
- Exemple : 8-core CPU = 8 threads
- `std::thread::hardware_concurrency()` le retourne
- Raytracer l'utilise pour adapter nombre workers

**Hit Record**
- Structure contenant résultat d'intersection rayon-primitive
- Champs : t (distance), point (position), normal (direction surface), color, hit (booléen)
- Créé à chaque rayon, passé au système d'ombrage

---

## I

**Indirection**
- Accès mémoire via pointeur (double dereference)
- Pointeur → adresse en mémoire (lent)
- Variable directe → valeur immédiate (rapide)
- Dans raytracer : pointeur intelligent = petite indirection acceptable

**Interface (Classe Abstraite)**
- Définit quelles méthodes doivent exister
- Exemple : `IPrimitive` force `hit()` method
- Permet polymorphisme

**Intersection Rayon-Géométrie**
- Calcul : le rayon touche-t-il l'objet ?
- Pour chaque type (Sphere, Cone, etc.), équation mathématique différente
- Sphère : résoudre équation quadratique
- Plan : diviser par normal.dot(direction)

---

## K

**KD-Tree (Kd-dimensional tree)**
- Structure acceleration spatiale (alternative BVH)
- Divise espace 3D en sous-cubes recursifs
- Plus compact que BVH mais moins flexible
- Raytracer n'utilise PAS

---

## L

**Lambda Expression**
- Fonction anonyme inline
- Syntaxe : `[capture](args) { body }`
- Exemple : `auto doubled = [](int x) { return x * 2; };`
- Raytracer l'utilise peu (pas de lambdas dans code principal)

**Load Balancing**
- Distribution équitable du travail entre threads
- Mauvais : 1 thread complète 1000 pixels, autres 100 chacun → inégal
- Bon : tous threads finissent presque au même moment
- Raytracer l'utilise : work-stealing = auto load balancing

---

## M

**Make_shared**
- Fonction qui crée `shared_ptr` en UNE allocation (plus rapide)
- Vs `shared_ptr(new Type())` = DEUX allocations
- Exemple : `auto sphere = std::make_shared<Sphere>(...);`
- Dans raytracer : utilisé partout pour créer primitives/lumières

**Memory Order**
- Niveau de synchronisation pour atomics
- `memory_order_relaxed` = pas de synchronisation (rapide, risqué)
- `memory_order_acquire/release` = synchronisation (sûr, lent)
- Raytracer utilise `relaxed` car `nextTask` a ordre faible

**Method Overriding (Surcharge Méthode)**
- Classe enfant redéfinit méthode de classe parent
- Exemple : `Sphere::hit()` override `IPrimitive::hit()`
- Permet polymorphisme runtime

**Monte Carlo**
- Méthode numérique utilisant random sampling
- Plus samples = meilleur résultat (mais lent)
- Utilisé en path tracing pour Global Illumination
- Raytracer n'utilise PAS (simple AO avec fixed samples)

---

## N

**Normal (Vecteur Normal)**
- Vecteur perpendiculaire à une surface
- Pointe "dehors" de l'objet
- Utilisé pour calcul ombrage (lumière reçue)
- Sphère : normal = (point - centre) / radius

**Normalisation**
- Convertir vecteur à longueur 1 (direction pure)
- Formule : `v_normalized = v / v.length()`
- Important pour calculs ombrage (intensité lumière)
- Raytracer normalise vecteurs "lazy" (quand nécessaire)

---

## O

**Object-Oriented Programming (OOP)**
- Paradigme programmation basé sur classes/objets
- Principes : encapsulation, héritage, polymorphisme
- Raytracer l'utilise fortement (classes, héritage)

**Open/Closed Principle**
- Principe SOLID : ouvert extension, fermé modification
- Exemple : ajouter ToriusShape sans éditer code existant
- Polymorphisme = open/closed respecté

**Operator Overloading (Surcharge Opérateur)**
- Redéfinir sens d'opérateurs (+, -, *, etc.) pour classes
- Exemple : `Vector3D a + Vector3D b` utilise `operator+` custom
- Rend code mathématiquement lisible
- Raytracer l'utilise pour Vector3D

---

## P

**Pattern (Design Pattern)**
- Solution réutilisable à problème courant
- Exemples : Polymorphisme, Décorateur, Factory
- Raytracer utilise : Polymorphe (IPrimitive), Décorateur (TransformedPrimitive)

**Phong Shading**
- Modèle d'ombrage simple et rapide
- Combine : Ambient + Diffuse + Specular
- Raytracer utilise version simplifiée (pas Specular)
- Formule : `color = ambient + (lumière · normal) * couleur_matériel`

**Pointeur (Pointer)**
- Variable qui stocke adresse mémoire d'une autre variable
- Syntaxe : `int* ptr = &var;` (& = adresse de)
- Déréférence : `*ptr` (valeur à cette adresse)
- Raytracer utilise `shared_ptr` (pointeurs intelligents)

**Polymorphisme**
- Capacité objets différents à répondre même message différemment
- Exemple : `sphere.hit(ray)` vs `plane.hit(ray)` → code différent
- Réalisé via héritage + virtual methods
- Permet traiter Sphère/Cone/Disk uniformément

**Primitive (Forme Géométrique)**
- Objet géométrique basique (Sphère, Cube, Cylindre, etc.)
- Raytracer a 11 primitives : Sphere, Cone, Cylinder, Plane, Torus, etc.
- Chaque primitive = classe qui hérite IPrimitive

---

## Q

**Quadratic Equation (Équation Quadratique)**
- Équation forme : `ax² + bx + c = 0`
- Solutions : x = (-b ± √(b²-4ac)) / 2a
- Raytracer l'utilise pour Sphere : `(ray - center)² = radius²`

---

## R

**Ray (Rayon)**
- Demi-ligne : point + direction
- Formule : `ray(t) = origin + direction * t` (t ≥ 0)
- Utilisé pour traçage (pour chaque pixel = 1 rayon)
- Structure : Point3D origin, Vector3D direction

**Ray Casting / Raytracing**
- Technique rendu : tracer rayon par pixel, calculer intersection, ombrer
- Raytracer implémente cela
- Vs rasterization (approche jeux vidéo)

**Recursion / Récursion**
- Fonction qui s'appelle elle-même
- Condition arrêt = évite infini
- Raytracer l'utilise pour transparence : `traceRay()` appelle `traceRay()` récursivement

**Reference Counting**
- Mécanisme `shared_ptr` : compte combien références pointent objet
- Quand count = 0, objet deleted automatiquement
- Exemple : 2 shared_ptr pointent Sphere → count = 2, delete après dernier disappear

**Refraction**
- Déviation lumière en traversant matériel transparent
- Rayon change direction en verre/eau
- Raytracer implémente avec transparency + récursion

**Runtime**
- Temps d'exécution (pendant que programme tourne)
- Vs compile-time (lors compilation)
- Polymorphisme = résolution runtime (quel `hit()` appeler ?)

---

## S

**Sampling**
- Prendre plusieurs "exemples" pour approximer résultat
- AO sampling : 16 rayons random par pixel → moyenne
- Plus samples = meilleur mais lent

**Scene**
- Conteneur pour toutes primitives + lumières + caméra
- Responsable raycast, shading, tracing complet
- Centre du raytracer

**Shared_ptr (Pointeur Intelligent Partagé)**
- Pointeur qui se détruit automatiquement
- Compte références : delete quand dernier ptr disparet
- Syntaxe : `shared_ptr<Sphere> s(new Sphere(...));`
- Plus sûr que `new`/`delete` manuel

**Shading / Ombrage**
- Calcul couleur finale d'un pixel basé sur :
  - Matériel (couleur intrinsèque)
  - Lumières (ambient, directional, AO)
  - Normale surface
- Raytracer implémente dans `Scene::shade()`

**Slicing (Tranchage)**
- Problème : stocker objet enfant dans conteneur parent
- Exemple : `vector<IPrimitive>` contient Sphere → données Sphere perdues
- Solution raytracer : utiliser `vector<shared_ptr<IPrimitive>>`

**SOLID Principles**
- S = Single Responsibility
- O = Open/Closed
- L = Liskov Substitution
- I = Interface Segregation
- D = Dependency Inversion
- Raytracer respecte plusieurs

**Stack Allocation (Allocation Stack)**
- Mémoire locale automatiquement libérée
- Rapide (simple pointeur)
- Taille connue à compile-time
- Exemple : `Vector3D v(1, 2, 3);` = stack

**STL (Standard Template Library)**
- Bibliothèque standard C++ : containers, algorithms
- Exemples : vector, array, string
- Raytracer l'utilise intensivement

---

## T

**Template (Modèle)**
- Code générique paramétré par type
- Exemple : `vector<T>` fonctionne pour int, double, Sphere, etc.
- Utilisé en raytracer indirectement (STL)

**Thread (Fil d'Exécution)**
- Exécution parallèle du code
- CPU multi-core peut exécuter plusieurs threads simultanément
- Raytracer crée N threads (N = hardware_concurrency)
- Chaque thread = calcule 1 pixel indépendamment

**Tile (Tuile)**
- Subdivision image (exemple : 64×64 pixels par tile)
- Alternative à pixel-by-pixel rendering
- Raytracer n'utilise PAS (work-stealing plus efficace)

---

## U

**Unit Vector (Vecteur Unitaire)**
- Vecteur de longueur 1
- Utilisé pour direction pures (normale, direction rayon)
- Créé par normalisation

---

## V

**Vector3D / Vector4D**
- Vector3D = (x, y, z) mathématique
- Vector4D = (x, y, z, w) = RGB + Alpha (transparence)
- Raytracer utilise pour direction, couleur, normal

**Virtual Method (Méthode Virtuelle)**
- Méthode qui peut être overridée par enfants
- Syntaxe : `virtual ReturnType method() = 0;` (pur abstrait)
- Permet polymorphisme runtime
- Sphère/Cone/Plane overridée `hit()` différemment

---

## W

**Work-Stealing**
- Stratégie multithreading : thread prend "work" quand libre
- Vs tile-based : chaque thread a région fixe (peut être inégal)
- Raytracer l'utilise : atomic counter = queue distribuée


---

## Indices Complexité

**O(1)** - Constant
- Accès element vector via index : `v[5]`

**O(n)** - Linéaire
- Boucle sur n éléments : `for (auto p : primitives)`

**O(log n)** - Logarithmique
- Recherche binaire, BVH tree

**O(n²)** - Quadratique
- Boucle imbriquée

Raytracer utilise O(n) intersection check (suffisant < 200 objets)
