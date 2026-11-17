#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## 05_manual.sh
## File description:
## Mode manuel : on pilote un joueur à la main (jury / questions)
##

source "$(dirname "$0")/common.sh"
ensure_built
choose_speed

FREQ=${FREQ:-$(speed_pick 10 10 100)}

title "Démo 5 — Pilotage manuel"
cat <<EOF
Vous êtes un joueur de l'équipe « main ». Tapez les commandes du protocole :
  Forward  Right  Left  Look  Inventory  Take food  Set food
  Broadcast <texte>  Fork  Eject  Connect_nbr  Incantation
Ctrl+D ou Ctrl+C pour quitter. Des IA « bots » jouent en fond si BOTS=1.
EOF
pause

start_server 10 10 4 "$FREQ" main bots
start_gui
[ "${BOTS:-0}" = "1" ] && start_ai bots 1

if command -v nc >/dev/null; then
    { echo main; cat; } | nc "$HOST" "$PORT"
else
    warn "nc absent : connexion via bash /dev/tcp"
    exec 3<>"/dev/tcp/$HOST/$PORT"
    cat <&3 &
    PIDS+=($!)
    { echo main; cat; } >&3
fi
