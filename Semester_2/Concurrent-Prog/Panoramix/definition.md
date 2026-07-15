### 1. Le Thread (Fil d'exécution)

Un thread est la plus petite séquence d'instructions pouvant être gérée de façon indépendante par l'ordonnanceur (scheduler) du système d'exploitation.

Contrairement à un processus classique (qui est créé via `fork` sous Linux et possède son propre espace mémoire totalement isolé), un thread vit **à l'intérieur** d'un processus.
Plusieurs threads créés au sein d'un même processus partagent le même espace d'adressage mémoire (le tas/heap, la section des données, le code compilé). Chaque thread possède en revanche sa propre pile d'exécution (stack) et ses propres registres CPU.

* **L'avantage :** Ce partage de mémoire permet aux threads de communiquer entre eux de manière extrêmement rapide, sans avoir recours à des mécanismes lourds de communication inter-processus (IPC).
* **Le danger :** Si plusieurs threads modifient la même zone mémoire simultanément, cela provoque des comportements indéfinis appelés *race conditions*.

### 2. La Mutex (Mutual Exclusion)

Une mutex est un objet de synchronisation strictement conçu pour **protéger** une ressource critique (comme une variable partagée ou l'accès à un fichier).

Son fonctionnement est binaire (verrouillé/déverrouillé) et repose sur un principe fondamental : **la propriété** (ownership).
Lorsqu'un thread appelle la fonction de verrouillage (ex: `pthread_mutex_lock`), il devient le propriétaire exclusif de la mutex. Si n'importe quel autre thread tente de verrouiller cette même mutex, le système d'exploitation va le bloquer (le suspendre) jusqu'à ce qu'elle soit libérée.

* **Règle d'or :** Seul le thread qui a verrouillé une mutex a le droit de la déverrouiller. Il est impossible architecturalement qu'un thread "A" verrouille une ressource et qu'un thread "B" vienne la déverrouiller pour lui.

### 3. Le Sémaphore

Un sémaphore est un outil de **signalisation** et de gestion de capacité. Contrairement à la mutex, un sémaphore n'est pas un simple verrou binaire, mais un compteur entier géré par le système d'exploitation.

Il est manipulé via deux opérations atomiques (indivisibles) :

1. **Wait (ou P)** : Si le compteur est supérieur à 0, le thread décrémente le compteur de 1 et continue son exécution. Si le compteur est à 0, le thread est immédiatement mis en sommeil par l'OS jusqu'à ce que le compteur repasse au-dessus de 0.
2. **Post (ou V)** : Le thread incrémente le compteur de 1. Si d'autres threads étaient endormis en train d'attendre (sur un `wait`), l'un d'eux est réveillé par le système.

* **Différence cruciale avec la mutex :** Le sémaphore n'a aucune notion de "propriétaire". Un thread peut tout à fait s'endormir en appelant `wait`, et c'est un **autre** thread qui viendra faire le `post` pour le réveiller. C'est donc le mécanisme parfait pour synchroniser des événements distincts entre plusieurs acteurs (ex: un producteur qui avertit un consommateur qu'une donnée est prête).