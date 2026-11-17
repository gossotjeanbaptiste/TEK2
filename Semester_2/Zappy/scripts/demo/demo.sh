#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## demo.sh
## File description:
## Menu de lancement des démos de soutenance
##

DIR="$(cd "$(dirname "$0")" && pwd)"

if [ -n "$2" ]; then
    export SPEED="$2"
fi
if [ -n "$1" ]; then
    exec "$DIR/0$1_"*.sh
fi

cat <<'EOF'

  ███████╗ █████╗ ██████╗ ██████╗ ██╗   ██╗
  ╚══███╔╝██╔══██╗██╔══██╗██╔══██╗╚██╗ ██╔╝
    ███╔╝ ███████║██████╔╝██████╔╝ ╚████╔╝
   ███╔╝  ██╔══██║██╔═══╝ ██╔═══╝   ╚██╔╝
  ███████╗██║  ██║██║     ██║        ██║
  ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝        ╚═╝   — démos

  1) Express    une colonie atteint le niveau 8 (10x10)
  2) Duel       deux équipes IA s'affrontent (20x20)
  3) Bataille   quatre équipes sur une grande carte (42x42)
  4) Vitrine    chaque commande jouée au ralenti, commentée
  5) Manuel     piloter un joueur au clavier
  6) Tutoriel   visite guidée de la fenêtre, bulles explicatives
  7) Vidéo      caméra automatique + titres, enregistrable en MP4

  En anglais : GUI_LANG=en ./demo.sh  (ou touche L dans la fenêtre)

  Vitesse demandée au lancement (slow / medium / fast),
  ou directement : ./demo.sh <n°> <slow|medium|fast>

  Variables : SPEED=medium PORT=4242  DEMO_PAUSE=1  DEMO_BUILD=1  NO_GUI=1  ZDEBUG=1
              W/H/FREQ (2, 3)  STEP (4)  BOTS=1 (5)  TUTORIAL_AUTO=6 (6)
              GUI_MODE=demo|tutorial|video|normal  GUI_SIZE=1920x1080
              SHOT_EVERY=5 (captures)  RECORD=fichier.mp4|0  END_HOLD=10

EOF
read -r -p "Choix [1-7] : " choice
case "$choice" in
    [1-7]) exec "$DIR/0${choice}_"*.sh ;;
    *) echo "Choix invalide." >&2; exit 1 ;;
esac
