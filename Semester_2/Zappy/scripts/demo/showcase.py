#!/usr/bin/env python3
"""Client scénarisé : joue chaque commande du protocole IA, lentement et
commentée, pour montrer l'animation correspondante dans la GUI.

Usage : showcase.py <host> <port> <équipe> [délai_s]"""

import socket
import sys
import time

RESOURCES = ["food", "linemate", "deraumere", "sibur",
             "mendiane", "phiras", "thystame"]


class Client:
    def __init__(self, host, port, team, name):
        self.name = name
        self.sock = socket.create_connection((host, port))
        self.buf = b""
        self.expect_line()                      # WELCOME
        self.sock.sendall(team.encode() + b"\n")
        slots = self.expect_line()
        if slots == "ko":
            raise SystemExit(f"{name}: équipe pleine ou inconnue")
        self.size = self.expect_line()
        say(name, f"connecté (slots restants {slots}, carte {self.size})")

    def expect_line(self):
        while b"\n" not in self.buf:
            data = self.sock.recv(4096)
            if not data:
                raise SystemExit(f"{self.name}: serveur déconnecté")
            self.buf += data
        line, self.buf = self.buf.split(b"\n", 1)
        return line.decode(errors="replace").strip()

    def cmd(self, command, until=None):
        """Envoie une commande et renvoie sa réponse en affichant au passage
        les messages asynchrones. `until` : préfixes attendus (sinon la
        première ligne non asynchrone)."""
        print(f"  \x1b[36m{self.name} →\x1b[0m {command}")
        self.sock.sendall(command.encode() + b"\n")
        while True:
            line = self.expect_line()
            if line == "dead":
                raise SystemExit(f"{self.name} est mort de faim")
            if line.startswith(("message", "eject:")) or \
                    (until and not line.startswith(until)):
                print(f"  \x1b[35m{self.name} ← (async)\x1b[0m {line}")
                continue
            print(f"  \x1b[32m{self.name} ←\x1b[0m {line}")
            return line


def say(who, text):
    print(f"\x1b[1m[{who}]\x1b[0m {text}")


def step(title, delay):
    print(f"\n\x1b[1;33m▶ {title}\x1b[0m")
    time.sleep(delay)


def parse_inv(line):
    words = line.strip("[] ").replace(",", " ").split()
    return {words[i]: int(words[i + 1]) for i in range(0, len(words) - 1, 2)
            if words[i + 1].lstrip("-").isdigit()}


def tile_items(look):
    """Case courante : accepte « food 1, linemate 0 » (ce serveur) comme
    « player food linemate » (format du sujet)."""
    words = look.strip("[] ").split(",")[0].split() \
        if not any(c.isdigit() for c in look) \
        else look.strip("[] ").replace(",", " ").split()
    if any(w.isdigit() for w in words):
        inv = parse_inv(" ".join(words))
        return [r for r, n in inv.items() if n > 0]
    return words


def main():
    host, port, team = sys.argv[1], int(sys.argv[2]), sys.argv[3]
    delay = float(sys.argv[4]) if len(sys.argv) > 4 else 1.5

    step("Connexion d'un joueur (pnw) — il apparaît dans la GUI", delay)
    a = Client(host, port, team, "Alice")

    step("Inventory (pin) — panneau d'info du joueur", delay)
    a.cmd("Inventory")

    step("Déplacements : Forward / Right / Left (ppo)", delay)
    for c in ["Forward", "Forward", "Right", "Forward", "Left", "Forward"]:
        a.cmd(c)
        time.sleep(delay / 3)

    step("Look — ce que voit le joueur", delay)
    look = a.cmd("Look")

    step("Recherche d'un linemate (pgt) pour le rituel niveau 2", delay)
    got = False
    for _ in range(40):
        items = tile_items(look)
        for res in ("linemate", "food"):
            if res in items and a.cmd(f"Take {res}") == "ok" \
                    and res == "linemate":
                got = True
        if got:
            break
        a.cmd("Forward")
        look = a.cmd("Look")
    if not got:
        say("Alice", "pas de linemate trouvé, on continue sans rituel")

    step("Set food (pdr) — dépôt d'une ressource", delay)
    a.cmd("Set food")

    step("Broadcast (pbc) — onde sonore dans la GUI", delay)
    a.cmd("Broadcast Bonjour la soutenance !")

    step("Fork (pfk + enw) — Alice pond un oeuf", delay)
    a.cmd("Fork")

    step("Connexion d'un 2e joueur : il éclot sur l'oeuf (ebo)", delay)
    b = Client(host, port, team, "Bob")
    b.cmd("Look")

    step("Eject (pex) — Alice pousse Bob hors de sa case", delay)
    a.cmd("Eject")
    time.sleep(0.2)

    if got:
        step("Set linemate + Incantation (pic → pie → plv) niveau 2", delay)
        a.cmd("Set linemate")
        a.cmd("Incantation", until=("Current level", "ko"))
        inv = parse_inv(a.cmd("Inventory"))
        say("Alice", f"inventaire après rituel : {inv}")

    step("Fin de la vitrine — les joueurs restent connectés", delay)
    say("demo", "Ctrl+C pour terminer (ils mourront de faim sinon 😉)")
    try:
        while True:
            a.cmd("Inventory")
            b.cmd("Inventory")
            time.sleep(3)
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    except OSError as err:
        print(f"showcase: {err}", file=sys.stderr)
        sys.exit(1)
