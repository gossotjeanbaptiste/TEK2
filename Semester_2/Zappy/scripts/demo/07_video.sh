#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## 07_video.sh
## File description:
## Mode vidéo : caméra automatique, titres et sous-titres, prêt à être
## enregistré (MP4 via ffmpeg si disponible) et posté
##

source "$(dirname "$0")/common.sh"
ensure_built
SPEED=${SPEED:-medium}   # ~2 min 30 de vidéo par défaut
choose_speed

W=${W:-12}; H=${H:-12}
FREQ=${FREQ:-$(speed_pick 25 40 100)}
GUI_MODE=video
GUI_SIZE=${GUI_SIZE:-1280x720}
if [ "$SPEED" = "slow" ]; then
    warn "Vitesse lente : la partie n'ira pas jusqu'à la victoire en quelques"
    warn "minutes ; fermez la fenêtre pour arrêter l'enregistrement."
fi

# Cherche ffmpeg : variable FFMPEG, PATH, .tools/ffmpeg (get_ffmpeg.sh),
# puis le binaire d'un paquet pip imageio-ffmpeg déjà installé.
find_ffmpeg() {
    [ -n "${FFMPEG:-}" ] && [ -x "$FFMPEG" ] && { echo "$FFMPEG"; return; }
    command -v ffmpeg 2>/dev/null && return
    [ -x "$ROOT/.tools/ffmpeg" ] && { echo "$ROOT/.tools/ffmpeg"; return; }
    python3 -c "import imageio_ffmpeg; print(imageio_ffmpeg.get_ffmpeg_exe())" \
        2>/dev/null
}

title "Démo 7 — Mode vidéo"
if [ "${RECORD:-}" != "0" ]; then
    FFMPEG=$(find_ffmpeg)
    if [ -n "$FFMPEG" ]; then
        mkdir -p "$ROOT/videos"
        RECORD=${RECORD:-$ROOT/videos/zappy_$(date +%Y%m%d_%H%M%S).mp4}
        info "Enregistrement automatique → $RECORD"
    else
        RECORD=""
        warn "ffmpeg introuvable : pas d'enregistrement automatique."
        warn "  sans root : ./scripts/demo/get_ffmpeg.sh   (puis relancer)"
        warn "  ou        : sudo apt install ffmpeg"
        warn "  ou enregistrez la fenêtre vous-même (Windows : Win+Alt+R, OBS)."
    fi
else
    RECORD=""
fi
echo "Carton titre, plans automatiques sur les rituels, sous-titres, puis"
echo "carton de victoire : la fenêtre se ferme seule ${END_HOLD:-10} s après."
pause

# GUI_LANG=en : vidéo en anglais (titres, sous-titres, noms d'équipes).
if [ "${GUI_LANG:-fr}" = "en" ]; then
    TEAM_A=red; TEAM_B=blue
else
    TEAM_A=rouge; TEAM_B=bleu
fi
start_server "$W" "$H" 2 "$FREQ" "$TEAM_A" "$TEAM_B"
start_gui
start_ai "$TEAM_A" 1
start_ai "$TEAM_B" 1
wait_end
sleep 1   # laisse ffmpeg finaliser le fichier
if [ -n "$RECORD" ] && [ -s "$RECORD" ]; then
    title "Vidéo prête"
    info "$RECORD ($(du -h "$RECORD" | cut -f1))"
fi
