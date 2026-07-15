#!/bin/bash
# Script de démarrage rapide pour l'entraînement Q-Learning

echo "================================"
echo "Q-Learning Agent - Démarrage"
echo "================================"
echo ""

# Vérifier que l'environnement virtuel est activé
if [[ -z "$VIRTUAL_ENV" ]]; then
    echo "❌ L'environnement virtuel n'est pas activé!"
    echo "Exécutez: source venv/bin/activate"
    exit 1
fi

echo "✓ Environnement virtuel: $VIRTUAL_ENV"
echo ""

# Paramètres par défaut
EPISODES=${1:-1000}
SEED=${2:-42}
LR=${3:-0.001}

echo "Paramètres d'entraînement:"
echo "  - Episodes: $EPISODES"
echo "  - Seed: $SEED"
echo "  - Learning rate: $LR"
echo ""

# Créer le répertoire de modèles
mkdir -p models

# Entraîner
echo "Démarrage de l'entraînement..."
echo "================================"
python3 train.py \
    --episodes $EPISODES \
    --seed $SEED \
    --lr $LR

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Entraînement terminé!"
    echo ""
    
    # Évaluer automatiquement
    MODEL="models/qlearning_agent_seed${SEED}.pt"
    
    if [ -f "$MODEL" ]; then
        echo "Évaluation du modèle..."
        echo "================================"
        python3 eval.py --model "$MODEL" --episodes 100
    fi
else
    echo ""
    echo "❌ Erreur lors de l'entraînement!"
    exit 1
fi

echo ""
echo "================================"
echo "✓ Terminé!"
echo "================================"
