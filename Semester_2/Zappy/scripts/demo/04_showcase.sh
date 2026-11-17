#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## 04_showcase.sh
## File description:
## Vitrine : chaque commande du protocole jouée au ralenti, avec la GUI
##

source "$(dirname "$0")/common.sh"
ensure_built
choose_speed

STEP=${STEP:-$(speed_pick 3 1.5 0.2)}    # pause entre étapes commentées
FREQ=${FREQ:-$(speed_pick 10 15 100)}    # basse = animations lisibles

title "Démo 4 — Visite guidée du protocole"
echo "Un client scénarisé joue Forward, Look, Take, Set, Broadcast, Fork,"
echo "Eject et Incantation, un par un. Regardez la GUI réagir à chaque étape."
pause

start_server 8 8 4 "$FREQ" demo
start_gui
pause "Entrée pour démarrer la visite"
python3 "$ROOT/scripts/demo/showcase.py" "$HOST" "$PORT" demo "$STEP"
