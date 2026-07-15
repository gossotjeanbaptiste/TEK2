# Racing Simulator

The objective of this project is to initiate the **Robocar** project by focusing on the artificial intelligence (AI) aspect.
You will have access to a simulation similar to the one you will work with during the **Robocar** project.
Using this simulation, your goal is to train your AI to navigate the track as quickly and efficiently as possible.

# Robocar Client

Client Python pour piloter le simulateur Unity, collecter des donnees, entrainer un modele supervise, puis executer la policy.

## Documentation detaillee

- Architecture: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
- Formats JSON/JSONL: [docs/DATA_FORMATS.md](docs/DATA_FORMATS.md)
- Workflow complet: [docs/WORKFLOW.md](docs/WORKFLOW.md)

## Arborescence

```text
robocar_client/
  .venv/
  configs/
    agents_config.json
  data/
    datasets/
      *.jsonl
      *.jsonl.meta.json
  models/
    *.joblib
  src/
    agent_forward.py
    train_supervised.py
    run_policy.py
  requirements.txt
  README.md
```

## 1) Installation

### Prerequis systeme (Linux)

Installer Python, `venv` et `pip` (Debian/Ubuntu):

```bash
sudo apt update
sudo apt install -y python3 python3-venv python3-pip
```

Verifier:

```bash
python3 --version
```

### Environnement virtuel + dependances Python

```bash
cd /home/victortv57/Downloads/RacingSimulatorLinux/BuildLinux/robocar_client
python3 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
```

## 2) Collecte de donnees

### Manuel (recommande)

```bash
python src/agent_forward.py \
  --mode manual-collect \
  --manual-capture-backend pynput \
  --steps 30000 \
  --dataset-path ./data/datasets/manual_trackA_s04.jsonl

Exemple avec bridage de vitesse:

```bash
python src/agent_forward.py \
  --mode manual-collect \
  --manual-capture-backend pynput \
  --steps 30000 \
  --dataset-path ./data/datasets/manual_trackA_s04.jsonl \
  --speed-limit 0.3
```
```

### Random (complement de diversite)

```bash
python src/agent_forward.py \
  --mode collect-random \
  --steps 15000 \
  --dataset-path ./data/datasets/random_trackA_s01.jsonl
```

### Securite ecriture

- Par defaut: erreur si le fichier existe deja.
- `--dataset-overwrite`: remplace le fichier.
- `--dataset-append`: ajoute a la fin du fichier.
- `--speed-limit`: bride le throttle en valeur absolue.

Comportement de `--speed-limit`:
- sans flag: pas de bridage
- flag sans valeur (`--speed-limit`): pas de bridage
- flag avec valeur (`--speed-limit 0.6`): bridage actif

Exemple append:

```bash
python src/agent_forward.py \
  --mode manual-collect \
  --manual-capture-backend pynput \
  --steps 10000 \
  --dataset-path ./data/datasets/manual_trackA_s04.jsonl \
  --dataset-append
```

## 3) Entrainement supervise

### Un seul fichier

```bash
python src/train_supervised.py \
  --dataset-path ./data/datasets/manual_trackA_s04.jsonl \
  --output-model ./models/policy_mlp.joblib \
  --max-iter 250
```

### Plusieurs fichiers (recommande)

```bash
python src/train_supervised.py \
  --dataset-glob './data/datasets/manual_*.jsonl' \
  --dataset-glob './data/datasets/forward_*.jsonl' \
  --output-model ./models/policy_mlp.joblib \
  --max-iter 250
```

Le script affiche des metriques de validation:
- MSE
- MAE
- R2

## 4) Prise en main par l'IA (inference)

```bash
python src/run_policy.py --model-path ./models/policy_mlp.joblib --steps 5000
```

Si le simulateur est deja lance:

```bash
python src/run_policy.py --connect-only --model-path ./models/policy_mlp.joblib --steps 5000
```

Brider la vitesse (canal throttle):

```bash
python src/run_policy.py --model-path ./models/policy_mlp.joblib --speed-limit 0.6
```

Comportement du flag `--speed-limit`:
- sans flag: pas de bridage
- flag sans valeur (`--speed-limit`): pas de bridage
- flag avec valeur (`--speed-limit 0.6`): bridage actif

## Notes

- Le mapping continu par defaut est calibre pour ce simulateur:
  - throttle/brake -> index 0
  - steering -> index 1
- Le fichier `configs/agents_config.json` est la config agent transmise au simulateur.
- Le projet privilegie un petit modele + gros dataset propre et diversifie.
