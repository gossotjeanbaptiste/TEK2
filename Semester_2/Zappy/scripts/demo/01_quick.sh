#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## 01_quick.sh
## File description:
## Démo express : une équipe, petite carte, niveau 8 (medium ~2 min, fast ~1 min ; slow : sans viser la fin)
##

source "$(dirname "$0")/common.sh"
ensure_built
choose_speed

title "Démo 1 — Une colonie atteint le niveau 8"
echo "Petite carte 10x10, une seule IA lancée : la reine pond des oeufs,"
echo "les drones éclosent au nid, fourragent et reviennent pour les rituels."
pause

FREQ=${FREQ:-$(speed_pick 25 40 100)}
start_server 10 10 2 "$FREQ" alpha beta
start_gui
pause "Le GUI est prête ? Entrée pour lancer l'IA"
start_ai alpha 1
wait_end
