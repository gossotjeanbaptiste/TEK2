# Workflow complet (collecte -> entrainement -> inference)

Ce document donne une procedure standard pour travailler proprement.

## 1) Collecte de donnees

Objectif:
- Produire un dataset grand, propre et diversifie.

Commande type (manuel):

```bash
python src/agent_forward.py \
  --mode manual-collect \
  --manual-capture-backend pynput \
  --steps 30000 \
  --dataset-path ./data/datasets/manual_trackA_s04.jsonl
```

Complement (random faible proportion):

```bash
python src/agent_forward.py \
  --mode collect-random \
  --steps 10000 \
  --dataset-path ./data/datasets/random_trackA_s01.jsonl
```

Bonnes pratiques:
- Faire plusieurs sessions courtes au lieu d'un seul gros fichier.
- Varier track, vitesse, trajectoires.
- Eviter d'avoir 95% de ligne droite.

## 2) Entrainement

Objectif:
- Entrainer un modele simple mais robuste sur beaucoup de donnees.

Commande multi-fichiers:

```bash
python src/train_supervised.py \
  --dataset-glob './data/datasets/manual_*.jsonl' \
  --dataset-glob './data/datasets/forward_*.jsonl' \
  --output-model ./models/policy_mlp.joblib \
  --max-iter 250
```

Metriques a surveiller (obligatoire sujet):
- MSE
- MAE
- R2

Interpretation rapide:
- MSE/MAE plus bas = meilleure precision moyenne.
- R2 plus haut = meilleur pouvoir explicatif du modele.

## 3) Inference (IA au volant)

Commande:

```bash
python src/run_policy.py --model-path ./models/policy_mlp.joblib --steps 5000
```

Si le simulateur est deja lance:

```bash
python src/run_policy.py --connect-only --model-path ./models/policy_mlp.joblib --steps 5000
```

## 4) Boucle d'amelioration

1. Collecter de nouvelles sessions la ou le modele echoue.
2. Re-entrainer sur l'ensemble des fichiers.
3. Comparer metriques run N vs run N+1.
4. Re-tester en simulation.

## 5) Convention de nommage conseillee

Datasets:
- manual_trackA_s01.jsonl
- manual_trackA_s02.jsonl
- manual_trackB_s01.jsonl
- random_trackA_s01.jsonl

Modeles:
- policy_mlp_v1.joblib
- policy_mlp_v2.joblib

Cette convention facilite le suivi des experiences.
