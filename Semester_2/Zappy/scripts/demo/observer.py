#!/usr/bin/env python3
"""Spectateur terminal : se connecte en GRAPHIC et affiche en direct le
tableau des niveaux par équipe, un fil d'événements en français (pour
comprendre ce que font les joueurs à l'écran), puis le vainqueur (seg).

Usage : observer.py [host] [port] [lignes_d_événements]"""

import socket
import sys
import time
from collections import Counter, deque

ESC = "\x1b["
B, DIM, RST = "\x1b[1m", "\x1b[2m", "\x1b[0m"
YEL, GRN, RED, CYA, MAG = ("\x1b[33m", "\x1b[32m", "\x1b[31m",
                           "\x1b[36m", "\x1b[35m")

LEGEND = (
    f"{DIM} But : la 1re équipe avec 6 joueurs au niveau 8 gagne.\n"
    " Chaque IA « reine » pond des oeufs ; les drones qui en éclosent partent\n"
    " chercher nourriture et pierres, les ramènent au nid, puis tous se\n"
    f" rassemblent pour un rituel d'élévation (incantation).{RST}\n")


class State:
    def __init__(self, n_events):
        self.teams, self.players = [], {}
        self.eggs, self.egg_team = Counter(), {}
        self.events = deque(maxlen=n_events)
        self.stats = Counter()
        self.start = time.time()

    def team_of(self, pid):
        p = self.players.get(pid)
        return p[0] if p else "?"

    def log(self, text):
        t = time.time() - self.start
        self.events.append(f"{DIM}{t:6.1f}s{RST} {text}")

    def handle(self, f):
        cmd = f[0]
        if cmd == "tna":
            self.teams += [t for t in f[1:] if t not in self.teams]
        elif cmd == "pnw" and len(f) >= 7:
            new = f[1] not in self.players
            self.players[f[1]] = [f[6], int(f[5])]
            if new:
                self.log(f"{CYA}👤 un joueur « {f[6]} » entre en jeu "
                         f"en ({f[2]},{f[3]}){RST}")
        elif cmd == "plv" and len(f) >= 3 and f[1] in self.players:
            self.players[f[1]][1] = int(f[2])
        elif cmd == "pdi" and len(f) >= 2:
            team = self.team_of(f[1])
            self.players.pop(f[1], None)
            self.stats["morts"] += 1
            self.log(f"{RED}💀 un joueur « {team} » meurt de faim{RST}")
        elif cmd == "enw" and len(f) >= 3:
            team = self.team_of(f[2])
            self.egg_team[f[1]] = team
            self.eggs[team] += 1
            self.stats["oeufs"] += 1
            self.log(f"🥚 « {team} » pond un oeuf (futur coéquipier)")
        elif cmd in ("ebo", "edi") and len(f) >= 2:
            team = self.egg_team.pop(f[1], None)
            if team:
                self.eggs[team] -= 1
            if cmd == "ebo":
                self.log(f"🐣 un oeuf « {team} » éclot")
        elif cmd == "pic" and len(f) >= 5:
            lvl, n = int(f[3]), len(f) - 4
            team = self.team_of(f[4])
            self.stats["rituels"] += 1
            self.log(f"{YEL}⚡ « {team} » lance un rituel niveau {lvl}→"
                     f"{lvl + 1} en ({f[1]},{f[2]}) à {n} joueur(s){RST}")
        elif cmd == "pie" and len(f) >= 4:
            if f[3] == "1":
                self.log(f"{GRN}✨ rituel réussi en ({f[1]},{f[2]}) : "
                         f"les participants montent de niveau{RST}")
            else:
                self.log(f"{RED}✖ rituel raté en ({f[1]},{f[2]}) "
                         f"(pierres ou joueurs manquants){RST}")
        elif cmd == "pex" and len(f) >= 2:
            self.log(f"💥 un joueur « {self.team_of(f[1])} » éjecte "
                     "ses voisins")
        elif cmd == "pbc" and len(f) >= 2:
            self.stats["messages"] += 1
        elif cmd == "pgt":
            self.stats["ramassés"] += 1

    def render(self):
        elapsed = time.time() - self.start
        out = [f"{ESC}H{ESC}J{B} Zappy — partie en direct  "
               f"t+{elapsed:5.1f}s{RST}\n", LEGEND, "\n"]
        out.append(f" {B}{'équipe':<10} {'vivants':>7} {'oeufs':>6}  "
                   f"niveaux  1  2  3  4  5  6  7  8{RST}\n")
        for team in self.teams:
            lv = Counter(p[1] for p in self.players.values()
                         if p[0] == team)
            bar = " ".join(f"{lv.get(i, 0):>2}" for i in range(1, 9))
            n8 = lv.get(8, 0)
            star = f"  {YEL}★ {n8}/6{RST}" if n8 else ""
            out.append(f" {team:<10} {sum(lv.values()):>7} "
                       f"{self.eggs[team]:>6}          {bar}{star}\n")
        s = self.stats
        out.append(f"\n {DIM}objets ramassés {s['ramassés']}  ·  "
                   f"messages radio {s['messages']}  ·  rituels "
                   f"{s['rituels']}  ·  oeufs {s['oeufs']}  ·  morts "
                   f"{s['morts']}{RST}\n")
        if self.events.maxlen:
            out.append(f"\n {B}Derniers événements{RST}\n")
            out += [f" {e}\n" for e in self.events]
        sys.stdout.write("".join(out))
        sys.stdout.flush()


def main():
    host = sys.argv[1] if len(sys.argv) > 1 else "localhost"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 4242
    n_events = int(sys.argv[3]) if len(sys.argv) > 3 else 10
    sock = socket.create_connection((host, port))
    sock.sendall(b"GRAPHIC\n")
    st = State(n_events)
    buf, last = b"", 0.0
    sock.settimeout(0.25)
    while True:
        try:
            data = sock.recv(65536)
            if not data:
                print("\nServeur déconnecté.")
                return 1
            buf += data
        except socket.timeout:
            pass
        while b"\n" in buf:
            raw, buf = buf.split(b"\n", 1)
            f = raw.decode(errors="replace").split()
            if not f:
                continue
            if f[0] == "seg" and len(f) >= 2:
                st.render()
                print(f"\n {B}{GRN}🏆 Victoire de « {f[1]} » en "
                      f"{time.time() - st.start:.1f}s{RST}\n")
                return 0
            st.handle(f)
        if time.time() - last > 0.25:
            st.render()
            last = time.time()


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        sys.exit(0)
    except OSError as err:
        print(f"observer: {err}", file=sys.stderr)
        sys.exit(1)
