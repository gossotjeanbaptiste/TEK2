# Architecture du projet robocar_client

Ce document explique le role de chaque dossier et de chaque script Python, ainsi que les choix techniques.

## Vue d'ensemble

Structure actuelle:

- [README.md](README.md): point d'entree rapide (commandes essentielles)
- [requirements.txt](requirements.txt): dependances Python
- [configs/agents_config.json](configs/agents_config.json): configuration des agents Unity
- [data/datasets](data/datasets): donnees de collecte (.jsonl + .meta.json)
- [models](models): modeles entraines (.joblib)
- [src/agent_forward.py](src/agent_forward.py): conduite manuelle/auto + collecte
- [src/train_supervised.py](src/train_supervised.py): entrainement supervise
- [src/run_policy.py](src/run_policy.py): inference du modele dans le simulateur

## Pourquoi cette separation

1. Separation des responsabilites
- Les scripts de collecte, d'entrainement et d'inference ont des objectifs differents.
- Cette separation limite les regressions: modifier la collecte n'impacte pas directement l'inference.

2. Reproductibilite
- Les donnees sont conservees dans [data/datasets](data/datasets).
- Les modeles sont versionnables dans [models](models).
- La configuration Unity est centralisee dans [configs/agents_config.json](configs/agents_config.json).

3. Evolutivite
- Ajouter de nouveaux modeles ou de nouvelles strategies de collecte est simple sans casser le flux existant.

## Details des scripts Python

### [src/agent_forward.py](src/agent_forward.py)

Role:
- Se connecter au simulateur Unity ML-Agents.
- Piloter en mode forward, random ou manuel.
- Collecter des paires observation/action pour l'apprentissage supervise.

Pourquoi ce script existe:
- C'est la base de creation du dataset. Sans donnees de qualite, un modele supervise ne peut pas apprendre correctement.

Capacites principales:
- Modes: forward, collect-forward, collect-random, manual, manual-collect.
- Entree manuelle clavier (pynput/pygame) et manette (pygame).
- Sauvegarde dataset en JSONL + metadata JSON.
- Protection d'ecriture: blocage par defaut, append explicite, overwrite explicite.

### [src/train_supervised.py](src/train_supervised.py)

Role:
- Charger un ou plusieurs datasets JSONL.
- Filtrer/assembler les samples valides.
- Entrainer un modele MLP supervise.
- Evaluer avec des metriques de validation.
- Sauvegarder un artefact joblib (modele + metadonnees).

Pourquoi ce script existe:
- Le sujet exige des metriques, pas seulement une observation visuelle.
- Le script fournit une base de benchmark rapide et repetable.

Choix techniques:
- Pipeline scikit-learn avec normalisation (StandardScaler) + MLPRegressor.
- Metriques: MSE, MAE, R2.
- Support multi-fichiers via --dataset-path repete ou --dataset-glob.

### [src/run_policy.py](src/run_policy.py)

Role:
- Charger le modele entraine.
- Lire les observations en temps reel.
- Predire l'action et l'envoyer au simulateur.

Pourquoi ce script existe:
- Separer l'execution d'un modele (inference) de la collecte et de l'entrainement.
- Permet de tester rapidement differents modeles sans modifier la logique de collecte.

## Chemins par defaut robustes

Les scripts calculent des chemins par rapport au dossier projet, ce qui permet d'executer:
- depuis [robocar_client](.)
- sans casser les defaults de config, dataset et modeles

## Bonnes pratiques recommandees

1. Garder un fichier dataset par session
- Exemple: manual_trackA_s01.jsonl, manual_trackA_s02.jsonl.

2. Favoriser la qualite des donnees manuelles
- Le random est utile pour la diversite, mais doit rester minoritaire.

3. Suivre les metriques a chaque entrainement
- Comparer MSE/MAE/R2 entre runs.
- Eviter de juger uniquement a l'oeil.
