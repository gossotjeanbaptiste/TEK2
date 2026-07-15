On utilise des `make_shared` parce que : 
- Une seule allocation (contrairement a faire `std::shared_ptr<Primitive>(new Primitive(...))`)
- Gere bien l'heritage
- Pas de fuite de memoire possible contrairement a new + delete si on oublie le delete

On utilise une class `Math::Vector3D` a la place de `std::vector<T>` parce que : 
- `std::vector<T>` c'est un conteneur dynamique pour stocker une collection d'element
- `std::vector<T>` a une taille variable
alors que
- `Math::Vector3D` C'est reellement un objet mathematique qui represente un vecteur tri-dimensionnel
- On a toujours un set de 3 coordonnees, pas 4 pas 2 au bon vouloir du programme ou de ce que l'on donne
- On a refait les operations mathematiques de base des vecteurs (Produits scalaire, etc...) pour que ca s'applique correcement
- On appelle notre vecteur de maniere mathematiquement correcte

C'est la meme logique a l'utilisation de `Point3D` et ca nous permet de na pas confondre entre les points les vecteurs. On peut egalement creer un vecteur depuis la difference de deux `Point3D` (ce serait la meme logique si c'etait des `Point2D` ou a `n dimensions`). Ca nous permet une semantique correcte et une meilleur lisibilite quand on demande la position de la camera par exemple, on s'attend a avoir un `Point3D` qui nous est donnee pas un `Vector3D`

Pourquoi `std::vector<Math::Vector3D>` Dans le multithreading :
- Couleur = Vecteur 3D - Chaque pixel a (R, G, B) → parfait pour Vector3D
- Accès rapide par index - O(1) per thread-safe
- Contiguous en mémoire - Bon pour la cache (performance)
- Taille connue à l'avance
Contigu/Contiguous def : Qui ont une bordure en commune/qui se touche

