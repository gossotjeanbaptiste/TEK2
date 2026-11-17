#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## 02_duel.sh
## File description:
## Duel : deux équipes IA s'affrontent sur une carte moyenne
##

source "$(dirname "$0")/common.sh"
ensure_built
choose_speed

W=${W:-20}; H=${H:-20}; FREQ=${FREQ:-$(speed_pick 25 40 100)}

title "Démo 2 — Duel rouge contre bleu"
echo "Carte ${W}x${H}, deux équipes avec la même IA : les broadcasts sont"
echo "préfixés par un hash du nom d'équipe, elles ne se parasitent pas."
pause

start_server "$W" "$H" 2 "$FREQ" rouge bleu
start_gui
pause "Entrée pour lancer les deux équipes"
start_ai rouge 1
start_ai bleu 1
wait_end
