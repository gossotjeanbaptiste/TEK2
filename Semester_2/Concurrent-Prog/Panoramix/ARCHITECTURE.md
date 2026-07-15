# Document de Choix Techniques pour le projet Panoramix

## 1. Introduction

Ce document détaille les choix d'implémentation et d'architecture réalisés pour le projet **Panoramix**. L'objectif principal de ce projet est de modéliser un problème classique de concurrence et de synchronisation (producteur/consommateur) en utilisant l'API POSIX Threads (`pthread`), tout en évitant les problèmes inhérents à la programmation concurrente : *data races*, *deadlocks*, *starvation* et fuites mémoire.

---

## 2. Architecture Globale et Gestion des Données

### 2.1. Refus des variables globales

**Le choix :** Aucune variable globale n'est utilisée pour la synchronisation ou le stockage de l'état (à l'exception des variables de contexte inhérentes au C, si applicables).

**La justification :** L'utilisation de variables globales en programmation multi-threadée rend le code imprévisible, difficile à tester et augmente le risque d'effets de bord. J'ai donc opté pour une structure unique `panoramix_data_t` allouée dans le `main` (sur la pile) et passée par pointeur à tous les threads. Cette structure encapsule les ressources partagées (le chaudron, la mutex, les sémaphores, et le flag d'arrêt).

### 2.2. Modélisation par Threads

**Le choix :** Chaque entité (1 Druide, N Villageois) est modélisée par son propre thread natif (`pthread_create`).

**La justification :** Cela permet au système d'exploitation (l'ordonnanceur) de gérer lui-même l'exécution concurrente de nos entités, exploitant ainsi les architectures multi-cœurs.

---

## 3. Synchronisation et Accès aux Ressources Critiques

### 3.1. La ressource critique : `portions_left`

**Le choix :** L'accès à la variable représentant le contenu du chaudron est strictement protégé par une unique **Mutex** (`cauldron_mutex`).

**La justification :** Pour éviter les *Race Conditions*. Si la mutex n'était pas là, deux villageois pourraient lire simultanément qu'il reste 1 portion, et la décrémenter en même temps, amenant le chaudron à une valeur aberrante (-1).

**Règle d'acquisition :** La mutex est verrouillée *juste avant* l'inspection du chaudron et déverrouillée *immédiatement après* le service. Aucun villageois ne part au combat en gardant la mutex verrouillée, ce qui provoquerait un *deadlock* général (famine des autres villageois).

### 3.2. Communication inter-threads : Les Sémaphores

**Le choix :** Utilisation de deux sémaphores initialisés à 0 (`sem_druid_wakeup` et `sem_villager_wait`) pour la communication entre les villageois et le druide.

**La justification :** L'initialisation à 0 force les threads à s'endormir (`sem_wait`) en attendant un signal (`sem_post`).

* **Évitement du "Busy Waiting" :** Plutôt que de faire boucler le druide sur un `while(portions_left > 0)` qui consommerait 100% d'un cœur CPU pour rien, le sémaphore permet au druide de dormir profondément tant qu'aucun villageois n'a besoin de lui. Le même principe s'applique au villageois qui attend le remplissage.

---

## 4. Prévention de la Désynchronisation (Choix de Design Majeur)

**Le problème :** Que se passe-t-il lorsque plusieurs villageois trouvent le chaudron vide presque en même temps ?

**Le choix technique :** Le premier villageois qui trouve le chaudron vide réveille le druide, **mais ne relâche pas la mutex du chaudron** pendant qu'il s'endort pour attendre la potion.

**La justification :** C'est le point de conception le plus critique. Si le villageois relâchait la mutex avant le remplissage :

1. Le villageois A voit le chaudron vide, réveille le druide, relâche la mutex, s'endort.
2. Le villageois B arrive, prend la mutex, voit le chaudron vide, réveille *à nouveau* le druide, relâche la mutex.
*Conséquence :* Le druide recevrait de multiples signaux `sem_post` non désirés, corrompant la logique de décrémentation de ses ingrédients (`nb_refills`).
En gardant la mutex verrouillée, le villageois A crée une "file d'attente" naturelle au niveau du système (les autres villageois bloquent sur `pthread_mutex_lock`). La synchronisation Druide-Villageois reste ainsi strictement en 1 pour 1.

---

## 5. Arrêt Gracieux (Graceful Shutdown) et Gestion Mémoire

**Le problème :** Une fois que tous les villageois ont terminé leurs combats, la simulation doit s'arrêter. Cependant, si le druide a encore des ingrédients (`nb_refills > 0`), il est bloqué à l'infini dans son `sem_wait`. Quitter brutalement le programme provoque des avertissements "possibly lost" dans Valgrind (mémoire interne allouée par la *glibc* pour le thread du druide non libérée).

**Le choix technique :** L'implémentation d'un flag d'arrêt asynchrone (`simulation_over`).
**La justification (Le Workflow de fin) :**

1. Le `main` attend la fin de tous les combats via `pthread_join` sur les villageois.
2. Une fois les villageois terminés, le `main` passe `simulation_over = 1`.
3. Le `main` envoie un signal artificiel au druide via `sem_post(&sem_druid_wakeup)` pour le forcer à sortir de son sommeil.
4. Le druide se réveille, vérifie le flag, constate que la simulation est finie, et sort de sa boucle avec un `break`.
5. Le `main` effectue un `pthread_join` sur le druide.
*Conséquence :* Le cycle de vie de **tous** les threads est géré de A à Z. Toute la mémoire est proprement libérée (`free`, `sem_destroy`, `pthread_mutex_destroy`), assurant un diagnostic mémoire Valgrind immaculé (0 fuites, 0 erreurs).

---

## 6. Modularité et Robustesse

### 6.1. Découpage Fonctionnel

**Le choix :** Le code est organisé en modules logiques (`parse_arguments`, `init_sync_tools`, `launch_simulation`, `end_simulation`).

**La justification :** Respect de la norme de codage (limite de taille des fonctions, limite de paramètres) et séparation des responsabilités (Single Responsibility Principle).

### 6.2. Tolérance zéro sur l'initialisation

**Le choix :** Vérification systématique des retours de fonctions de l'API standard (`malloc`, `pthread_mutex_init`, `sem_init`, `pthread_create`).

**La justification :** Le sujet impose le code de retour `84` en cas d'erreur. Si une seule allocation ou initialisation échoue, le programme retourne immédiatement cette valeur sans tenter de lancer une simulation bancale, garantissant ainsi l'absence de *Segmentation Fault* dus à un environnement système instable.