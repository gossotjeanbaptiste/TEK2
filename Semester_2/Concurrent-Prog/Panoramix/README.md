# Architecture & Choix Techniques : Projet Panoramix

Voici un petit tour de l'architecture choisie pour faire tourner Panoramix. L'objectif principal ici était de gérer la concurrence proprement, d'éviter de faire tourner le CPU dans le vide pour rien, et surtout, d'esquiver les *deadlocks* (interblocages).

## 1. La Mutex : Protéger le chaudron avant tout

Dans ce projet, notre ressource critique, c'est le contenu du chaudron (`portions_left`). Avec plusieurs threads de villageois qui tournent en même temps, il fallait éviter les accidents.

* **Le problème (Race Condition) :** Si deux villageois regardent le chaudron au même instant, voient qu'il reste 1 portion, et décident de se servir en même temps, notre compteur passe à -1. Le crash de la logique est donc assurer.
* **La solution :** On utilise une unique `cauldron_mutex`. Un villageois la verrouille *juste avant* de vérifier s'il y a de la potion, et la relâche *dès qu'il s'est servi*.
* **La règle d'or :** Le villageois ne part **jamais** au combat en gardant la mutex verrouillée. S'il le faisait, tous les autres villageois resteraient bloqués à attendre indéfiniment devant le chaudron.

## 2. Les Sémaphores : On se réveille !

Le sujet nous impose d'utiliser des sémaphores pour la communication. Là où la mutex sert de "bouclier", les sémaphores servent de "bipeur".

* **Pourquoi pas une simple boucle `while` ?** On aurait pu faire en sorte que le druide tourne en boucle pour vérifier si le chaudron est vide (ce qu'on appelle du *busy waiting*). Mais ça fait exploser l'utilisation du CPU. En initialisant nos deux sémaphores (`sem_druid_wakeup` et `sem_villager_wait`) à 0, on force les threads à s'endormir profondément tant qu'on ne les a pas signalés.
* **Le choix de design qui change tout :** Quand un villageois trouve le chaudron vide, l'algorithme fait le choix fort de **ne pas relâcher la mutex** avant de réveiller le druide.
* *Pourquoi ?* Imagine si le villageois relâchait la mutex : le villageois suivant arriverait, verrait le chaudron vide, et spammerait le druide d'un nouveau signal de réveil. Le druide serait complètement désynchronisé.
* En gardant la mutex verrouillée, le premier villageois agit comme un videur à l'entrée d'une boîte de nuit : il crée une file d'attente propre. Les autres attendent sagement, et une fois que le druide a rempli la marmite, tout le monde peut se servir à tour de rôle.



## 3. L'Arrêt du programme : Quitter proprement (Graceful Shutdown)

Le programme doit s'arrêter proprement une fois tous les combats terminés. Mais il y a un piège classique ici avec le druide.

* On utilise `pthread_join` sur chaque thread de villageois. Cela garantit que le processus principal attend la fin de la baston.


* Mais que se passe-t-il si les villageois finissent tous leurs combats sans vider le chaudron une dernière fois ? Le druide reste endormi sur son sémaphore à attendre un réveil qui n'arrivera jamais.
* **L'astuce :** Une fois que le `main` a validé le retour de tous les villageois, il procède au nettoyage de la mémoire (`free`, `sem_destroy`, etc.) et fait son `return 0`. En C, la mort du processus principal entraîne la mort de tous les threads enfants. Le druide est donc arrêté proprement "en silence", sans causer de fuite mémoire ou de deadlock.

## 4. Un code modulaire et robuste

Pour que le code soit coding-style compliant, le `main` a été découpé en petites briques logiques (`parse_arguments`, `init_sync_tools`, etc.).

* **Tolérance zéro sur les erreurs :** Chaque étape critique (un argument manquant, une allocation qui échoue, un sémaphore qui refuse de s'initialiser) stoppe tout et retourne `84`.


* La simulation ne se lance que si l'environnement est parfaitement sain, ce qui évite de se retrouver avec un *Segfault* en plein milieu de l'exécution.