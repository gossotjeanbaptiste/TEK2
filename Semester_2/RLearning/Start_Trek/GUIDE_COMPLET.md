# 📖 Guide Complet - Agent Q-Learning pour Star Trek LunarLander

## 🎯 Objectif du projet (selon le PDF)

Développer un agent de contrôle autonome capable d'atterrir un module lunaire sur la Lune en utilisant le **Q-Learning avec apprentissage profond (DQN)**.

- **Environnement**: LunarLander-v3 (Gymnasium)
- **Critère de succès**: Moyenne de récompense ≥ 200 sur 100 épisodes

## ✅ Ce qui a été créé

### 1. **Agent Q-Learning complet** (`agents/qlearning_agent.py`)

Implémentation professionnelle avec:

**Architecture réseau:**
- MLP 2 couches cachées (128 neurones chacune)
- Activation ReLU
- Output: Q-values pour 4 actions

**Techniques de stabilisation:**
- ✅ Experience Replay Buffer (10,000 transitions max)
- ✅ Target Network (mise à jour dure toutes les 1000 étapes)
- ✅ Epsilon-Greedy Exploration (initial: 1.0 → min: 0.01)
- ✅ Gradient clipping
- ✅ Optimiseur Adam

### 2. **Scripts d'entraînement et évaluation**

**train.py**
- Entraîne l'agent sur N épisodes
- Logging automatique tous les 50 épisodes
- Évaluation périodique (tous les 200 épisodes)
- Sauvegarde modèle et statistiques

**eval.py**
- Évalue un modèle sur N épisodes
- Calcule statistiques complètes:
  - Moyenne, std dev, IC 95%
  - Taux de succès/crash/timeout
  - Longueur moyenne d'épisode

### 3. **Documentation et guides**

- 📖 **README.md** - Documentation complète
- ⚡ **QUICKSTART.txt** - Instructions rapides
- 📊 **IMPLEMENTATION_SUMMARY.md** - Résumé de l'implémentation

### 4. **Scripts automatisés**

- 🚀 **quickstart.sh** - Lance train + eval en une commande
- 🔄 **train_multi_seed.sh** - Entraîne avec N seeds, collecte stats

## 🔧 Architecture détaillée

### Classes principales

```python
QNetwork(nn.Module)
├── Entrée: État (8D)
├── FC(128) + ReLU
├── FC(128) + ReLU
└── Sortie: Q-values (4D)

ReplayBuffer
├── Stocke transitions
└── Sample batches aléatoires

QLearningAgent
├── Q-network + Target network
├── Replay buffer
├── Epsilon-greedy selection
├── Training loop
└── Model save/load
```

## 📊 Hyperparamètres par défaut

```
learning_rate:      1e-3
gamma:              0.99
epsilon_initial:    1.0
epsilon_min:        0.01
epsilon_decay:      0.995
buffer_capacity:    10000
batch_size:         32
target_update_freq: 1000
```

## 🚀 Guide d'utilisation

### Installation complète

```bash
cd /home/antoine/devas/PGE2/IA/star_trek
source venv/bin/activate
pip install -r requirements.txt
```

### Entraînement basique

```bash
# 50 épisodes (test rapide, ~2 min)
python3 train.py --episodes 50 --seed 42

# 1000 épisodes (résultats sérieux, ~30-50 min)
python3 train.py --episodes 1000 --seed 42

# 2000 épisodes (pour convergence complète)
python3 train.py --episodes 2000 --seed 42
```

### Évaluation

```bash
# Évaluer modèle
python3 eval.py --model models/qlearning_agent_seed42.pt --episodes 100

# Évaluer tous les modèles
for seed in 0 1 2 3 4; do
    python3 eval.py --model models/qlearning_agent_seed$seed.pt --episodes 100
done
```

### Multi-seed (reproducibilité)

```bash
# Entraîner 5 modèles avec graines différentes
bash train_multi_seed.sh 5 1000 0.001
```

### Utiliser GPU (si disponible)

```bash
python3 train.py --episodes 1000 --device cuda
```

## 📈 Performance attendue

### Selon le nombre d'épisodes:

| Episodes | Reward moyen | Success rate | Status |
|----------|--------------|--------------|--------|
| 50       | -100 à -200  | 0-10%        | 🟡 Apprentissage initial |
| 200      | -50 à 50     | 10-30%       | 🟡 Progrès |
| 500      | 50 à 150     | 30-50%       | 🟠 Bon apprentissage |
| 1000     | 100 à 250    | 50-80%       | 🟢 **Résolu!** |
| 2000     | 150 à 300+   | 70-95%       | 🟢 **Surappris!** |

**Critère de succès: Moyenne ≥ 200 sur 100 épisodes**

## 🎓 Concepts d'apprentissage clés

### Q-Learning basique

Pour chaque transition (s, a, r, s', done):
```
Q(s,a) ← Q(s,a) + α[r + γ·max_a' Q(s',a') - Q(s,a)]
```

### Avec Deep Q-Network:

1. **Experience Replay**: Stocke transitions, sample batches aléatoires
   - ✅ Réduit la corrélation temporelle
   - ✅ Améliore l'efficacité des données

2. **Target Network**: Réseau séparé pour les cibles
   - ✅ Stabilise l'apprentissage
   - ✅ Réduit les oscillations

3. **Epsilon-Greedy**: Exploration + Exploitation
   - ✅ Haut epsilon au début (exploration)
   - ✅ Bas epsilon à la fin (exploitation)

## 🔬 Suggestions d'ablation

Comparer les configurations:

```bash
# 1. Taille du buffer
python3 train.py --episodes 1000 --buffer_size 5000
python3 train.py --episodes 1000 --buffer_size 20000

# 2. Decay d'epsilon
python3 train.py --episodes 1000 --epsilon_decay 0.99
python3 train.py --episodes 1000 --epsilon_decay 0.999

# 3. Taux d'apprentissage
python3 train.py --episodes 1000 --lr 0.0001
python3 train.py --episodes 1000 --lr 0.01

# 4. Batch size (modifier dans le code)
# Editer train.py et changer --batch_size
```

## 📁 Structure finale du projet

```
star_trek/
├── agents/
│   ├── __init__.py
│   ├── qlearning_agent.py         ← Agent Q-Learning
│   ├── random_agent.py
│   ├── heuristic_agent.py
│   └── policy_gradient_agent.py
├── models/
│   ├── qlearning_agent_seed0.pt   ← Modèles entraînés
│   ├── qlearning_agent_seed1.pt
│   └── stats_seed0.json           ← Statistiques
├── evaluation_results/
│   └── eval_qlearning_agent_seed0.json
├── bootstrap/                      ← Fichiers d'exemple
├── projet/
│   └── G-AIA-401_start_trek.pdf   ← Spécifications
├── train.py                        ← Script d'entraînement
├── eval.py                         ← Script d'évaluation
├── requirements.txt                ← Dépendances
├── README.md                       ← Documentation
├── QUICKSTART.txt                  ← Démarrage rapide
├── quickstart.sh                   ← Script d'automatisation
├── train_multi_seed.sh             ← Entraînement multi-seed
└── IMPLEMENTATION_SUMMARY.md       ← Ce fichier
```

## 🎯 Prochaines étapes

1. **Court terme (30 min)**:
   ```bash
   python3 train.py --episodes 200 --seed 42
   python3 eval.py --model models/qlearning_agent_seed42.pt --episodes 50
   ```

2. **Moyen terme (2-3 heures)**:
   ```bash
   bash train_multi_seed.sh 5 1000 0.001
   # Puis analyser les résultats JSON
   ```

3. **Long terme (recherche)**:
   - Étude d'ablation systématique
   - Comparaison avec autres algorithmes (Policy Gradient, A3C, etc.)
   - Optimisation des hyperparamètres
   - Documentation détaillée

## ⚙️ Paramètres CPU/GPU

| Hardware | Episodes/min | Recommandation |
|----------|-------------|-----------------|
| CPU      | ~0.5        | Pour tests rapides |
| GPU      | ~3-5        | Pour sessions longues |

## 📞 Dépannage

**Problème**: `ModuleNotFoundError: No module named 'gymnasium'`
```bash
source venv/bin/activate
pip install gymnasium[box2d]
```

**Problème**: `Box2D is not installed`
```bash
pip install swig
pip install "gymnasium[box2d]"
```

**Problème**: Apprentissage lent
- Réduire learning_rate: `--lr 0.0005`
- Augmenter buffer_size: `--buffer_size 20000`
- Utiliser GPU: `--device cuda`

## ✨ Points forts de cette implémentation

✅ Code propre, commenté, et modulaire
✅ Support CPU/GPU transparent
✅ Reproducible avec seeds
✅ Logging détaillé et automatique
✅ Évaluation statistiquement robuste
✅ Facilement extensible pour ablations
✅ Documentation complète
✅ Scripts d'automatisation inclus

## 📚 Ressources

- [Gymnasium](https://gymnasium.farama.org/)
- [PyTorch Documentation](https://pytorch.org/docs/)
- [Human-level control through deep reinforcement learning](https://www.nature.com/articles/nature14236)
- [Deep Q-Networks Explained](https://towardsdatascience.com/deep-q-networks-explained-15c938e77f8)

---

**Créé avec ❤️ pour le projet Star Trek LunarLander**

Pour démarrer maintenant:
```bash
cd /home/antoine/devas/PGE2/IA/star_trek
source venv/bin/activate
python3 train.py --episodes 100
```

Bon entraînement! 🚀🌕
