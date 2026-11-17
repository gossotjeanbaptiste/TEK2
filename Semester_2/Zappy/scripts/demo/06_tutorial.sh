#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## 06_tutorial.sh
## File description:
## Tutoriel : visite guidée de la fenêtre avec bulles explicatives, puis
## les moments clés de la partie cadrés par la caméra
##

source "$(dirname "$0")/common.sh"
ensure_built
SPEED=${SPEED:-slow}     # le tutoriel a besoin de temps : slow par défaut
choose_speed

FREQ=${FREQ:-$(speed_pick 25 30 100)}
GUI_MODE=tutorial

title "Démo 6 — Tutoriel guidé"
echo "Des bulles présentent chaque partie de la fenêtre, puis la caméra"
echo "vous emmène voir un ramassage, une ponte, un rituel, une montée de"
echo "niveau... dès qu'ils se produisent. Entrée = suivant, Échap = passer."
pause

start_server 10 10 2 "$FREQ" alpha beta
start_gui
start_ai alpha 1
wait_end
