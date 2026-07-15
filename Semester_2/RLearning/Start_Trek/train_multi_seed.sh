#!/bin/bash
# Script pour entraîner le agent Q-Learning avec plusieurs seeds
# Utile pour obtenir des résultats reproducibles et statistiquement robustes

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Configuration
NUM_SEEDS=${1:-3}           # Nombre de seeds à tester
EPISODES=${2:-1000}         # Épisodes par seed
LEARNING_RATE=${3:-0.001}   # Taux d'apprentissage

echo "╔════════════════════════════════════════════════════════════╗"
echo "║    Q-Learning Multi-Seed Training                          ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Configuration:"
echo "  Seeds: $NUM_SEEDS"
echo "  Episodes per seed: $EPISODES"
echo "  Learning rate: $LEARNING_RATE"
echo ""

# Vérifier l'activation du venv
if [[ -z "$VIRTUAL_ENV" ]]; then
    echo "❌ L'environnement virtuel n'est pas activé!"
    echo "Exécutez: source venv/bin/activate"
    exit 1
fi

echo "✓ Environnement virtuel: $VIRTUAL_ENV"
echo ""

# Créer répertoire des modèles
mkdir -p models
mkdir -p evaluation_results

# Variables pour collecte des résultats
declare -a EVAL_REWARDS

# Entraîner avec chaque seed
for seed in $(seq 0 $((NUM_SEEDS-1))); do
    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║ Seed $seed/$((NUM_SEEDS-1))                                   ║"
    echo "╚════════════════════════════════════════════════════════════╝"
    
    # Entraîner
    echo "Training..."
    python3 train.py \
        --episodes $EPISODES \
        --seed $seed \
        --lr $LEARNING_RATE
    
    echo ""
    
    # Évaluer
    echo "Evaluating..."
    MODEL="models/qlearning_agent_seed${seed}.pt"
    python3 eval.py \
        --model "$MODEL" \
        --episodes 100 \
        --seed $seed > /tmp/eval_${seed}.log 2>&1
    
    # Extraire la récompense moyenne
    MEAN_REWARD=$(grep "Moyenne:" /tmp/eval_${seed}.log | awk -F':' '{print $2}' | awk '{print $1}')
    EVAL_REWARDS+=($MEAN_REWARD)
    
    echo "✓ Seed $seed complete! Mean reward: $MEAN_REWARD"
    echo ""
done

# Résumé final
echo "╔════════════════════════════════════════════════════════════╗"
echo "║ RÉSUMÉ FINAL                                               ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

echo "Récompenses moyennes par seed:"
for i in $(seq 0 $((NUM_SEEDS-1))); do
    echo "  Seed $i: ${EVAL_REWARDS[$i]}"
done

# Calculer la moyenne et l'écart-type (si plus d'une seed)
if [ $NUM_SEEDS -gt 1 ]; then
    echo ""
    echo "Statistiques globales:"
    
    # Python one-liner pour calculer moyenne et std
    python3 << EOF
import numpy as np
rewards = [${EVAL_REWARDS[*]}]
print(f"  Moyenne: {np.mean(rewards):.2f}")
print(f"  Std Dev: {np.std(rewards):.2f}")
print(f"  Min: {np.min(rewards):.2f}")
print(f"  Max: {np.max(rewards):.2f}")
EOF
fi

echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║ ✓ Entraînement multi-seed terminé!                         ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Fichiers générés:"
echo "  - models/qlearning_agent_seed*.pt"
echo "  - models/stats_seed*.json"
echo "  - evaluation_results/eval_qlearning_agent_seed*.json"
echo ""
