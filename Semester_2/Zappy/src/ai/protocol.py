# protocol.py
"""Décodage/encodage du protocole Zappy côté IA.

Tolérant aux deux dialectes rencontrés :
- serveur du repo : Look -> "[food 0, linemate 1, ...]" (compteurs de la case
  courante uniquement), Inventory -> "food 10 linemate 0 ..." (sans crochets)
- serveur du sujet : Look -> "[player food, food, ...]" (liste d'objets par
  case, case 0 = case du joueur), Inventory -> "[food 10, linemate 0, ...]"
"""

import zlib

# Préfixe des messages d'équipe, propre au nom d'équipe : deux équipes qui
# utilisent cette même IA ne doivent pas s'écouter mutuellement.
PREFIX = "Z"


def set_team(team):
    global PREFIX
    PREFIX = "Z%04x" % (zlib.crc32(team.encode()) & 0xFFFF)


RESOURCES = ("food", "linemate", "deraumere", "sibur",
             "mendiane", "phiras", "thystame")
STONES = RESOURCES[1:]

# Pierres et joueurs requis pour l'élévation niveau N -> N+1
ELEVATION = {
    1: {"players": 1, "stones": {"linemate": 1}},
    2: {"players": 2, "stones": {"linemate": 1, "deraumere": 1, "sibur": 1}},
    3: {"players": 2, "stones": {"linemate": 2, "sibur": 1, "phiras": 2}},
    4: {"players": 4, "stones": {"linemate": 1, "deraumere": 1, "sibur": 2,
                                 "phiras": 1}},
    5: {"players": 4, "stones": {"linemate": 1, "deraumere": 2, "sibur": 1,
                                 "mendiane": 3}},
    6: {"players": 6, "stones": {"linemate": 1, "deraumere": 2, "sibur": 3,
                                 "phiras": 1}},
    7: {"players": 6, "stones": {"linemate": 2, "deraumere": 2, "sibur": 2,
                                 "mendiane": 2, "phiras": 2, "thystame": 1}},
}


def _strip_brackets(line):
    line = line.strip()
    if line.startswith("[") and line.endswith("]"):
        return line[1:-1]
    return line


def parse_inventory(line):
    """Retourne {ressource: quantité} ou None si la ligne n'y ressemble pas.

    Gère "[food 10, linemate 0, ...]" (sujet) et
    "food 10 linemate 0 ..." sans virgules (serveur du repo).
    """
    inner = _strip_brackets(line)
    counts = {}
    if "," in inner:
        pairs = [entry.split() for entry in inner.split(",")]
    else:
        tokens = inner.split()
        if len(tokens) % 2 != 0:
            return None
        pairs = [[tokens[i], tokens[i + 1]]
                 for i in range(0, len(tokens), 2)]
    for parts in pairs:
        if len(parts) != 2:
            continue
        name, qty = parts
        if name in RESOURCES and qty.lstrip("-").isdigit():
            counts[name] = int(qty)
    return counts if counts else None


def parse_look(line):
    """Retourne les compteurs de ressources de la case courante."""
    counts = parse_inventory(line)
    if counts is not None and len(counts) >= 2:
        return counts
    # Format sujet : la case 0 liste les objets, répétés si multiples
    entries = _strip_brackets(line).split(",")
    tile0 = entries[0].split() if entries else []
    counts = {}
    for obj in tile0:
        if obj in RESOURCES:
            counts[obj] = counts.get(obj, 0) + 1
    return counts


def parse_message(line):
    """Décode 'message K, texte' -> (K, texte) ou None."""
    body = line[len("message"):].strip()
    if "," not in body:
        return None
    k_str, text = body.split(",", 1)
    try:
        k = int(k_str.strip())
    except ValueError:
        k = -1
    return k, text.strip()


def parse_current_level(line):
    """Décode 'Current level: N' -> N ou None."""
    if ":" not in line:
        return None
    try:
        return int(line.split(":", 1)[1].strip())
    except ValueError:
        return None


def zap_encode(*fields):
    """Encode un message d'équipe : Zxxxx:TAG:champ:... (< 50 chars)."""
    return ":".join([PREFIX] + [str(f) for f in fields])


def zap_decode(text):
    """Décode un message d'équipe -> (tag, [champs]) ou None si étranger."""
    if not text.startswith(PREFIX + ":"):
        return None
    parts = text.split(":")
    if len(parts) < 2:
        return None
    return parts[1], parts[2:]
