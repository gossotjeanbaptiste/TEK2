# Formats de donnees (JSON/JSONL)

Ce document explique les fichiers de donnees utilises dans le projet.

## 1) Configuration agents Unity

Fichier: [configs/agents_config.json](configs/agents_config.json)

Role:
- Definir les agents crees par le simulateur.
- Chaque agent declare ses parametres de perception (fov, nbRay).

Exemple:

```json
{
  "agents": [
    { "fov": 120, "nbRay": 20 }
  ]
}
```

Contraintes:
- fov: 1 a 180
- nbRay: 1 a 50

Pourquoi ce fichier est separe:
- On peut tester rapidement plusieurs configurations capteurs sans modifier les scripts Python.

## 2) Dataset principal (.jsonl)

Fichiers: [data/datasets](data/datasets)

Format:
- 1 ligne = 1 sample JSON.
- Extension .jsonl (JSON Lines).

Exemple d'une ligne:

```json
{
  "env_step": 42,
  "behavior": "Agent0?team=0",
  "agent_id": 17,
  "reward": 0.03,
  "observation": [0.12, -0.87, 0.44],
  "observation_shapes": [[3]],
  "action_type": "continuous",
  "action": [0.0, 1.0]
}
```

Signification des champs:
- env_step: index de pas de simulation.
- behavior: nom du behavior Unity ML-Agents.
- agent_id: id unique de l'agent dans le pas courant.
- reward: recompense courante (utile pour analyse, meme en supervise).
- observation: vecteur flatten des observations.
- observation_shapes: forme originale de chaque bloc observation.
- action_type: type d'action (continuous/discrete).
- action: action envoyee au simulateur.

Pourquoi JSONL:
- Ecriture en streaming (simple et robuste).
- Facile a concatener, filtrer, parser.
- Pratique pour gros volumes.

## 3) Metadata de session (.meta.json)

Fichier associe a chaque dataset:
- exemple: manual_trackA_s03.jsonl.meta.json

Contenu typique:
- timestamp de creation
- chemin de config
- mode de collecte
- description des behaviors

Pourquoi ce fichier existe:
- Trace du contexte de collecte.
- Diagnostic des incompatibilites obs/action entre datasets.

## 4) Artefact modele (.joblib)

Dossier: [models](models)

Contenu sauvegarde par [src/train_supervised.py](src/train_supervised.py):
- modele scikit-learn (pipeline scaler + MLP)
- dimensions d'entree/sortie
- metadonnees de run (seed, hidden layers)

Pourquoi joblib:
- Format standard scikit-learn, rapide et pratique.

## 5) Regles de gestion des fichiers dataset

Dans [src/agent_forward.py](src/agent_forward.py):
- Par defaut: echec si le fichier existe deja (protection anti-ecrasement).
- --dataset-overwrite: remplace le fichier.
- --dataset-append: ajoute de nouvelles lignes.

Recommendation:
- Preferer un fichier par session, puis entrainer sur plusieurs fichiers via glob.
