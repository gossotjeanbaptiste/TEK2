#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## common.sh
## File description:
## Fonctions partagées par les scripts de démo (sourcé, pas exécuté)
##

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
LOG_DIR="${LOG_DIR:-/tmp/zappy_demo}"
PORT="${PORT:-4242}"
HOST="${HOST:-localhost}"
PIDS=()

C_BOLD=$'\e[1m'; C_CYAN=$'\e[36m'; C_GREEN=$'\e[32m'
C_YELLOW=$'\e[33m'; C_RED=$'\e[31m'; C_RESET=$'\e[0m'

title()   { printf '\n%s%s== %s ==%s\n' "$C_BOLD" "$C_CYAN" "$*" "$C_RESET"; }
info()    { printf '%s[*]%s %s\n' "$C_GREEN" "$C_RESET" "$*"; }
warn()    { printf '%s[!]%s %s\n' "$C_YELLOW" "$C_RESET" "$*"; }
die()     { printf '%s[x]%s %s\n' "$C_RED" "$C_RESET" "$*" >&2; exit 1; }

# Attend Entrée si DEMO_PAUSE=1 (pour rythmer la soutenance).
pause() {
    if [ "${DEMO_PAUSE:-0}" = "1" ]; then
        read -r -p "${C_YELLOW}↵ ${*:-Entrée pour continuer}${C_RESET} " _
    fi
}

# Vitesse de la démo : SPEED=slow|medium|fast, demandée si absente
# (les anciens noms lent|moyen|rapide restent acceptés).
#   slow   : pour regarder : durées du sujet, chaque pas dure ~0,3 s. La
#            partie ne vise pas la fin (niveaux 1 à 4 en ~15 min).
#   medium : partie complète en quelques minutes
#   fast   : vérifier vite que tout marche (fréquence 100)
normalize_speed() {
    case "$1" in
        slow|lent|s|l|1)          echo slow ;;
        medium|moyen|med|m|2)     echo medium ;;
        fast|rapide|f|r|3)        echo fast ;;
        *)                        echo "" ;;
    esac
}

choose_speed() {
    SPEED=$(normalize_speed "${SPEED:-}")
    if [ -z "$SPEED" ]; then
        if [ -t 0 ]; then
            echo "Vitesse :"
            echo "  1) slow    on voit marcher chaque joueur (sans viser la fin)"
            echo "  2) medium  partie complète en quelques minutes"
            echo "  3) fast    vérifier que tout fonctionne"
            read -r -p "Choix [1-3, défaut 2] : " s
            SPEED=$(normalize_speed "$s")
            [ -z "$SPEED" ] && SPEED=medium
        else
            SPEED=fast
        fi
    fi
    export SPEED
    info "Vitesse : $SPEED"
}

# speed_pick <valeur slow> <valeur medium> <valeur fast>
speed_pick() {
    case "$SPEED" in slow) echo "$1" ;; medium) echo "$2" ;; *) echo "$3" ;; esac
}

# Compile ce qui manque (ou tout si DEMO_BUILD=1).
ensure_built() {
    cd "$ROOT" || die "racine du repo introuvable"
    if [ "${DEMO_BUILD:-0}" = "1" ] || [ ! -x zappy_server ] \
        || [ ! -x zappy_gui ] || [ ! -x zappy_ai ]; then
        info "Compilation (make)..."
        make -s >/dev/null || die "make a échoué"
    fi
    mkdir -p "$LOG_DIR"
}

# Tue tout ce qu'on a lancé, y compris les drones forkés par l'IA
# (lancés dans leur propre session, donc rattrapés par le port/équipe).
cleanup() {
    trap - EXIT INT TERM
    echo
    info "Arrêt de la démo..."
    for pid in "${PIDS[@]}"; do
        kill "$pid" 2>/dev/null
    done
    pkill -f "^python3? \S*src/ai/main\.py -p $PORT " 2>/dev/null
    pkill -f "^\./zappy_server -p $PORT " 2>/dev/null
    wait 2>/dev/null
    info "Logs disponibles dans $LOG_DIR"
    exit 0
}
trap cleanup EXIT INT TERM

port_busy() { (exec 3<>"/dev/tcp/$HOST/$PORT") 2>/dev/null; }

wait_port() {
    for _ in $(seq 1 50); do
        port_busy && return 0
        sleep 0.1
    done
    die "le serveur n'écoute pas sur $HOST:$PORT (voir $LOG_DIR/server.log)"
}

# start_server <width> <height> <clients/équipe> <freq> <équipes...>
start_server() {
    local w=$1 h=$2 c=$3 f=$4
    shift 4
    SERVER_FREQ=$f
    # Durées du sujet (Forward = 7/f...) en slow : les joueurs
    # marchent au lieu de bondir. SUBJECT_TIMES=0/1 pour forcer.
    local subject=${SUBJECT_TIMES:-$(speed_pick 1 0 0)}
    port_busy && die "le port $PORT est déjà utilisé (PORT=xxxx pour changer)"
    info "Serveur : ${w}x${h}, $c clients/équipe, f=$f, équipes : $*$( \
        [ "$subject" = "1" ] && echo " (durées du sujet)")"
    ZAPPY_SUBJECT_TIMES="$subject" \
        ./zappy_server -p "$PORT" -x "$w" -y "$h" -c "$c" -f "$f" -n "$@" \
        >"$LOG_DIR/server.log" 2>&1 &
    PIDS+=($!)
    wait_port
}

# start_gui : la fenêtre affiche elle-même les explications (ZAPPY_MODE).
#   GUI_MODE=demo (défaut) | tutorial | video | normal
#   SHOTS_DIR + SHOT_EVERY=<s> : captures d'écran automatiques
#   RECORD=<fichier.mp4>       : enregistrement vidéo (ffmpeg requis)
#   GUI_SIZE=1920x1080         : taille de fenêtre
#   TUTORIAL_AUTO=<s>          : le tutoriel avance seul toutes les s secondes
#   END_HOLD=<s>               : durée du carton de fin du mode vidéo
#   GUI_LANG=en                : GUI en anglais (tous les modes ; L bascule)
GUI_PID=""
start_gui() {
    [ "${NO_GUI:-0}" = "1" ] && return
    local mode=${GUI_MODE:-demo}
    [ "$mode" = "normal" ] && mode=""
    info "GUI ($( [ -n "$mode" ] && echo "mode $mode" || echo "classique")) sur $HOST:$PORT"
    ZAPPY_MODE="$mode" \
    ZAPPY_SIZE="${GUI_SIZE:-}" \
    ZAPPY_SHOTS="${SHOTS_DIR:-$LOG_DIR/screenshots}" \
    ZAPPY_SHOT_EVERY="${SHOT_EVERY:-0}" \
    ZAPPY_RECORD="${RECORD:-}" \
    ZAPPY_FFMPEG="${FFMPEG:-ffmpeg}" \
    ZAPPY_END_HOLD="${END_HOLD:-10}" \
    ZAPPY_TUTORIAL_AUTO="${TUTORIAL_AUTO:-${TUTO_AUTO:-0}}" \
    ZAPPY_LANG="${GUI_LANG:-fr}" \
        ./zappy_gui -p "$PORT" -h "$HOST" >"$LOG_DIR/gui.log" 2>&1 &
    GUI_PID=$!
    PIDS+=($GUI_PID)
    sleep "${GUI_DELAY:-1.5}"
}

# start_ai <équipe> [nombre]  — l'IA reine fork elle-même ses drones.
# ZAPPY_FREQ adapte les délais internes de l'IA à la fréquence du serveur.
start_ai() {
    local team=$1 n=${2:-1}
    for i in $(seq 1 "$n"); do
        info "IA « $team » #$i"
        ZDEBUG="${ZDEBUG:-}" ZAPPY_FREQ="${SERVER_FREQ:-100}" \
            ./zappy_ai -p "$PORT" -n "$team" -h "$HOST" \
            >>"$LOG_DIR/ai_$team.log" 2>&1 &
        PIDS+=($!)
        sleep 0.3
    done
}

# Attend la fin : avec GUI, les explications sont DANS la fenêtre et on
# attend sa fermeture ; sans GUI (NO_GUI=1), tableau de bord terminal.
wait_end() {
    if [ -n "$GUI_PID" ]; then
        info "Les explications sont dans la fenêtre du GUI (touche H)."
        info "Fermez la fenêtre (ou Ctrl+C ici) pour arrêter la démo."
        # Le serveur s'arrête à la victoire mais la fenêtre reste ouverte
        # sur l'écran de fin : on attend que l'utilisateur la ferme.
        while kill -0 "$GUI_PID" 2>/dev/null; do
            sleep 0.5
        done
        return
    fi
    info "Partie en cours — Ctrl+C pour arrêter."
    sleep 1
    python3 "$ROOT/scripts/demo/observer.py" "$HOST" "$PORT" \
        "$(speed_pick 14 10 6)"
    [ -t 0 ] && read -r -p "Entrée pour quitter " _
}
