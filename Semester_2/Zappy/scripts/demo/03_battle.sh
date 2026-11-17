#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## 03_battle.sh
## File description:
## Bataille : 4 équipes sur une grande carte (stress test GUI + serveur)
##

source "$(dirname "$0")/common.sh"
ensure_built
choose_speed

W=${W:-42}; H=${H:-42}; FREQ=${FREQ:-$(speed_pick 25 50 100)}
if [ "${GUI_LANG:-fr}" = "en" ]; then
    TEAMS=(fire water earth air)
else
    TEAMS=(feu eau terre air)
fi

title "Démo 3 — Bataille à ${#TEAMS[@]} équipes"
echo "Grande carte ${W}x${H} : des dizaines de joueurs, d'oeufs et de"
echo "rituels simultanés. Montre la tenue en charge du serveur et de la GUI."
pause

start_server "$W" "$H" 2 "$FREQ" "${TEAMS[@]}"
start_gui
pause "Entrée pour lâcher les ${#TEAMS[@]} colonies"
for t in "${TEAMS[@]}"; do
    start_ai "$t" 1
done
wait_end
