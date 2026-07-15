# Agent Q-Learning pour LunarLander-v3

Implémentation d'un agent **Deep Q-Network (DQN)** pour résoudre l'environnement LunarLander-v3 de Gymnasium.

## 📋 Caractéristiques

- **Architecture**: MLP 2 couches cachées avec ReLU
- **Apprentissage**: Q-Learning avec réseau de neurones
- **Stabilisation**: 
  - Experience replay buffer
  - Target network (mise à jour dure)
  - Epsilon-greedy exploration avec decay
- **Évaluation**: Support du multi-seed pour reproducibilité

## 🚀 Utilisation rapide

### 1. Installation des dépendances

```bash
cd /home/antoine/devas/PGE2/IA/G-AIA-401-NCY-4-1-starttrek-5
source .venv/bin/activate
pip install -r requirements.txt
```

### 2. Entraîner l'agent

```bash
python3 train.py --episodes 1000 --seed 42
```

Options disponibles:
- `--episodes`: Nombre d'épisodes d'entraînement (défaut: 1000)
- `--lr`: Taux d'apprentissage (défaut: 1e-3)
- `--gamma`: Facteur de discount (défaut: 0.99)
- `--epsilon_decay`: Decay d'epsilon (défaut: 0.995)
- `--buffer_size`: Taille du replay buffer (défaut: 10000)
- `--batch_size`: Taille des batches (défaut: 32)
- `--seed`: Graine aléatoire (défaut: 42)
- `--device`: cpu ou cuda (défaut: cpu)

Exemple avec hyperparamètres personnalisés:
```bash
python3 train.py --episodes 2000 --lr 5e-4 --epsilon_decay 0.998 --seed 0
```

### 3. Évaluer l'agent

```bash
python3 eval.py --model models/qlearning_agent_seed42.pt --episodes 100
```

Options disponibles:
- `--model`: Chemin du modèle à charger
- `--episodes`: Nombre d'épisodes d'évaluation (défaut: 100)
- `--seed`: Graine aléatoire (défaut: 42)

## 📊 Structure du code

```
agents/
├── __init__.py
└── qlearning_agent.py    # Implémentation de l'agent DQN
train.py                   # Script d'entraînement
eval.py                    # Script d'évaluation
models/                    # Répertoire de sauvegarde des modèles
```

## 🧠 Architecture du modèle

### QNetwork (MLP)
```
Input (8) → FC(128) → ReLU → FC(128) → ReLU → FC(4)
```

### Agent QLearning
- **Exploration-Exploitation**: Epsilon-greedy
- **Stabilisation**:
  - Target network mise à jour toutes les 1000 étapes
  - Experience replay avec batch size 32
- **Fonction de coût**: Mean Squared Error (MSE)
- **Optimiseur**: Adam

## 📈 Critères de succès

L'environnement est considéré **résolu** quand:
- **Moyenne de récompense ≥ 200** sur 100 épisodes consécutifs

Récompenses:
- `+100` pour un atterrissage réussi (crash landing)
- `-100` pour un crash
- `+50` pour chaque jambe qui touche le sol
- `-0.3` par degré d'inclinaison
- `-1` par unité de vitesse négative

## 🔧 Fichiers générés

Après chaque entraînement, les fichiers suivants sont sauvegardés:

1. **models/qlearning_agent_seedX.pt** - Modèle entraîné
2. **models/stats_seedX.json** - Statistiques d'entraînement
   - Récompenses par épisode
   - Longueur des épisodes
   - Pertes d'entraînement

## 📝 Exemple complet

```bash
# Activer l'environnement virtuel
source venv/bin/activate

# Entraîner sur 2000 épisodes
python3 train.py --episodes 2000 --seed 42

# Évaluer le modèle entraîné
python3 eval.py --model models/qlearning_agent_seed42.pt --episodes 100

# Résultats
# - Moyenne: 150.5 ± 25.3
# - Success rate: 60%
# - Crash rate: 25%
```

## 🔬 Ablation possible

Pour des études d'ablation, vous pouvez modifier:

1. **Taille du buffer**: `--buffer_size 5000` vs `--buffer_size 20000`
2. **Decay d'epsilon**: `--epsilon_decay 0.99` vs `--epsilon_decay 0.999`
3. **Taux d'apprentissage**: `--lr 1e-4` vs `--lr 1e-2`
4. **Architecture**: Modifier `hidden_dim` dans `qlearning_agent.py`

## 📚 Références

- [Gymnasium](https://gymnasium.farama.org/)
- [PyTorch](https://pytorch.org/)
- [DQN Paper (Mnih et al., 2015)](https://www.nature.com/articles/nature14236)

## ⚠️ Notes

- GPU est optionnel (cpu par défaut)
- Les résultats varient selon la graine aléatoire
- Pour la reproducibilité, utilisez les mêmes graines et hyperparamètres
