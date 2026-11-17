#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## get_ffmpeg.sh
## File description:
## Télécharge un ffmpeg autonome dans .tools/ (sans droits root) pour
## l'enregistrement automatique du mode vidéo
##

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEST="$ROOT/.tools"

if [ -x "$DEST/ffmpeg" ]; then
    echo "ffmpeg déjà présent : $DEST/ffmpeg"
    exit 0
fi
command -v python3 >/dev/null || { echo "python3 requis" >&2; exit 1; }

echo "Téléchargement de imageio-ffmpeg (binaire ffmpeg statique, ~30 Mo)..."
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
python3 -m pip install --quiet --target "$TMP" imageio-ffmpeg \
    || { echo "échec du téléchargement (pip)" >&2; exit 1; }
BIN=$(ls "$TMP"/imageio_ffmpeg/binaries/ffmpeg-* 2>/dev/null | head -n 1)
[ -n "$BIN" ] || { echo "binaire ffmpeg introuvable dans le paquet" >&2; exit 1; }
mkdir -p "$DEST"
cp "$BIN" "$DEST/ffmpeg"
chmod +x "$DEST/ffmpeg"
echo "OK : $DEST/ffmpeg"
