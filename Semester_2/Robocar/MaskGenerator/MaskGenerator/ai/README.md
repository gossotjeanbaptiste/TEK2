# MaskGenerator AI

Cette version repart sur une base proche de `RacingSimulator`.
Le modèle n'est plus un segmenteur PyTorch: c'est un MLP supervisé, entraîné sur des features visuelles compactes, qui prédit la présence de deux lignes blanches continues sur circuit: gauche et droite.

## Principe

1. `generate_synthetic_dataset.py` fabrique des circuits synthétiques avec deux lignes blanches.
2. `train_unity.py` extrait des features visuelles, entraîne un `StandardScaler + MLPRegressor`, puis enregistre un artefact `joblib`.
3. `infer_unity.py` recharge le modèle et renvoie les probabilités de présence des deux lignes.
4. `run_test.py` exécute tout le pipeline sur un petit dataset de smoke test.

## Dépendances

Installer les dépendances Python avec:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r MaskGenerator/ai/requirements.txt
```

## Commandes utiles

Générer un dataset synthétique:

```bash
python3 MaskGenerator/ai/generate_synthetic_dataset.py --out dataset --n 1000 --w 320 --h 240
```

Entraîner le modèle:

```bash
python3 MaskGenerator/ai/train_unity.py --in dataset/images --mask dataset/masks --out MaskGenerator/ai/lane_presence.joblib --source synthetic
```

Lancer l'inférence:

```bash
python3 MaskGenerator/ai/infer_unity.py --model MaskGenerator/ai/lane_presence.joblib --in dataset/images --mask dataset/masks --out preds --source synthetic
```

Smoke test complet:

```bash
python3 MaskGenerator/ai/run_test.py
```

## Fichiers principaux

- `model.py`: extraction de features, labelisation et helpers joblib.
- `unity_dataset.py`: appariement images/masques.
- `train_unity.py`: entraînement du MLP.
- `infer_unity.py`: inférence et export JSONL.
- `generate_synthetic_dataset.py`: génération d'un circuit synthétique avec deux bords blancs.
- `simple_pixel_learner.py`: baseline simple sur les mêmes features.
- `raycast.py`: utilitaire bas niveau pour mesurer les distances sur une image binaire.


