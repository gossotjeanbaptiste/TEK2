# BOOTSTRAP - Panoramix

## Objectif
Étudier la programmation multi-thread et les mutex en C à travers une simulation de trains traversant un pont fragile.

## Contexte
- **n**: numéro du train
- **=**: la voie ferrée
- **|-|**: le pont (peut supporter un nombre limité de trains)
- **#**: les trains

### Position initiale
```
0123456789012345
-------------------
0: #====|----|=====
1: #====|----|=====
2: #====|----|=====
```

### Position pendant la traversée
```
0123456789012345
-------------------
0: =====|-#--|=====
1: ====#|----|=====
2: ====#|----|=====
```

## Étapes

### Étape 1: Un à la fois
- Écrire un programme qui simule les trains traversant le pont
- Un seul train peut traverser le pont à la fois
- `NB_TRAINS`: nombre total de trains
- Chaque train est exécuté par un thread
- La traversée du pont est contrôlée par un **mutex**

### Étape 2: Plusieurs trains à la fois
- Modifier le programme pour permettre plusieurs trains simultanément
- `NB_MAX`: nombre maximum de trains sur le pont en même temps
- Exemple avec `NB_MAX = 2`:
```
0123456789012345
------------------
0: =====|----|==#==
1: =====|--#-|=====
2: =====|-#--|=====
3: ====#|----|=====
```

## Concepts clés
- **Mutex**: synchronisation pour l'accès exclusif
- **Semaphore**: pour contrôler le nombre de threads simultanés
- **Threads**: chaque train dans son propre thread
- **Condition variables**: coordination entre threads

