# brain.py
"""Cerveau de l'IA Zappy : stratégie « colonie fourrageuse ».

Le serveur ne transmet ni position ni orientation, et Look ne montre que la
case courante. En revanche :
- Fork pond un oeuf sur la case du joueur ;
- un client qui se connecte éclot depuis un oeuf de son équipe (même case) ;
- une incantation élève TOUS les joueurs de même équipe/niveau de la case ;
- nos propres déplacements sont déterministes : chaque agent suit sa
  position relative (« navigation à l'estime ») depuis son point d'origine.

Le premier client s'élit « reine » ; sa case devient le NID (origine 0,0).
Elle pond des oeufs au nid et les autres (« drones ») se reconnectent pour y
éclore : ils connaissent alors exactement la position du nid.

La nourriture est rare (~0,5 par case, réapparition aléatoire sur toute la
carte) : rester au nid = mourir de faim. Chacun part donc en expédition
(aller-retour en « pétale » de longueur aléatoire), ramasse nourriture et
pierres, revient au nid et y dépose les pierres. Quand le nid contient les
pierres du prochain rituel, la reine rappelle la colonie (Z:QC), attend les
drones au nid, gèle (Z:QF) et incante.

Rôles des broadcasts (préfixe Z, ignorés s'ils ne viennent pas de nous) :
  Z:H:<id>[:E|:L]   drone -> bonjour (E = j'ai éclos au nid, L = je suis perdu)
  Z:D:<id>:<lvl>:<f>:<food> drone -> rapport (f: S=fourrage, H=au nid sur
                    rappel, R=élevé par rituel, C=rattrapage solo)
  Z:C:<id>          drone niveau 1 -> j'incante seul au nid (silence nid)
  Z:QA:<qid>:<lvl>  reine -> annonce périodique (élection + niveau)
  Z:QS:<qid>:<id>   reine -> tu as éclos au nid, tu es des nôtres
  Z:QR:<qid>:<id>   reine -> tu n'es pas au nid, attends un oeuf
  Z:QE:<qid>:<n>    reine -> n oeufs prêts, les exilés se reconnectent
  Z:QC:<qid>:<lvl>  reine -> rappel au nid des drones de niveau <lvl>
  Z:QF:<qid>:<lvl>  reine -> gel : incantation imminente, silence radio
  Z:QU:<qid>        reine -> rituel annulé, reprise du fourrage
  Z:QD:<qid>:<lvl>  reine -> rituel réussi, je suis niveau <lvl>
"""

import os
import random
import subprocess
import sys
import time

import protocol as proto
from network import LineSocket, ConnectionClosed

# Délais en secondes réelles, calibrés pour la fréquence serveur par
# défaut (-f 100). ZAPPY_FREQ=<f> les met à l'échelle (100/f) quand le
# serveur tourne à une autre fréquence ; les drones héritent de la variable.
try:
    TIME_SCALE = 100.0 / max(1.0, float(os.environ.get("ZAPPY_FREQ", 100)))
except ValueError:
    TIME_SCALE = 1.0
ELECTION_WINDOW = 2.5 * TIME_SCALE
HELLO_PERIOD = 1.5 * TIME_SCALE
QUEEN_ANNOUNCE_PERIOD = 2.5 * TIME_SCALE
LOOK_PERIOD = 0.4 * TIME_SCALE
INVENTORY_PERIOD = 2.0 * TIME_SCALE
DRONE_REPORT_PERIOD = 3.0 * TIME_SCALE
DRONE_PRUNE_AFTER = 25.0 * TIME_SCALE
FOOD_LIFE_PERIOD = 1.26 * TIME_SCALE  # 126/f : 1 food convertie en vie
FREEZE_SETTLE = 0.6 * TIME_SCALE
RITUAL_RETRY_DELAY = 1.5 * TIME_SCALE
RITUAL_RESULT_TIMEOUT = 12.0 * TIME_SCALE
FROZEN_TIMEOUT = 15.0 * TIME_SCALE
CLAIM_FREEZE_TIMEOUT = 6.0 * TIME_SCALE
QUEEN_SILENCE_TIMEOUT = 30.0 * TIME_SCALE
PENDING_TIMEOUT = 4.0 * TIME_SCALE
GATHER_PATIENCE = 4.0 * TIME_SCALE  # attente des retardataires, quota atteint
GATHER_TIMEOUT = 12.0 * TIME_SCALE  # rappel abandonné si quota non atteint
GATHER_DRONE_TIMEOUT = 20.0 * TIME_SCALE
FOOD_TARGET = 30
FOOD_CRITICAL = 4
RITUAL_MIN_FOOD = 8         # personne ne rituelle le ventre vide
TAKE_BURST = 4
# 5 drones + la reine = 6 joueurs pour gagner, plus 2 réservistes : un drone
# largué au-delà du niveau 2 ne peut plus jamais rattraper la colonie
DRONES_TARGET = 7
LEVEL_WAIT_RELAXED = 30.0 * TIME_SCALE  # ensuite : incante dès le quota minimal
# Rattrapage en groupe : des retardataires de même niveau s'élèvent entre
# eux au nid (2 joueurs suffisent pour 2->3 et 3->4, 4 pour 4->5 et 5->6).
PEER_TIMEOUT = 15.0 * TIME_SCALE        # le meneur abandonne s'il reste seul
PEER_CALL_PERIOD = 3.0 * TIME_SCALE     # rappel du meneur
PEER_HOME_PERIOD = 1.5 * TIME_SCALE     # « je suis au nid » des participants
PEER_PRESENT = 4.0 * TIME_SCALE         # un participant compte s'il s'est
                                        # signalé depuis moins que ça
MAX_PENDING = 8             # le serveur refuse au-delà de 10 en file
MAX_SPAWNS = 10             # garde-fou anti fork-bomb de processus

HOME = (0, 0)
# Repère propre à chaque agent (son orientation initiale = « nord »)
DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))

START_TIME = time.monotonic()


def stones_needed_from(level):
    """Pierres nécessaires pour tous les rituels de `level` jusqu'au 8."""
    need = {s: 0 for s in proto.STONES}
    for lvl in range(max(1, level), 8):
        for stone, n in proto.ELEVATION[lvl]["stones"].items():
            need[stone] += n
    need["linemate"] += 2       # marge pour les rattrapages solo 1->2
    return need


class Dead(Exception):
    """Le serveur nous a annoncé 'dead'."""


class Respawn(Exception):
    """Reconnexion volontaire pour éclore d'un oeuf de la reine."""


class Agent:
    def __init__(self, host, port, team):
        self.host = host
        self.port = port
        self.team = team
        proto.set_team(team)
        self.ident = "%04x" % random.randrange(16 ** 4)
        self.net = None
        self.victory = False
        self.egg_respawn = False    # cette session vient d'une éclosion
        self.qid = None
        self.queen_level = 1
        self.map_w = 20
        self.map_h = 20
        self.reset_player_state()

    # ------------------------------------------------------------------
    # Cycle de vie
    # ------------------------------------------------------------------

    def reset_player_state(self):
        self.role = "?"
        self.mode = "WAIT_ASSIGN"
        self.level = 1
        self.inventory = {r: 0 for r in proto.RESOURCES}
        self.inv_synced_at = time.monotonic()
        self.tile = {}
        self.tile_at = 0.0
        self.tile_pos = None
        self.pending = []
        self.timers = {}
        self.election_started = 0.0
        self.peer_ids = set()
        self.last_queen_seen = time.monotonic()
        # navigation à l'estime
        self.pos = HOME
        self.heading = 0
        self.home_known = False
        self.home_tile = {}
        self.route = []
        # état reine
        self.drones = {}            # id -> {level, seen, sat, food, home}
        self.egg_credit = 0
        self.spawned = 0
        self.ritual_phase = "IDLE"
        self.phase_at = 0.0
        self.ritual_retry_at = 0.0
        self.level_since = time.monotonic()
        # état drone
        self.assigned = False       # la reine a confirmé : éclos au nid
        self.frozen_at = 0.0
        self.frozen_timeout = FROZEN_TIMEOUT
        self.mode_before_freeze = None
        self.mode_at = 0.0
        self.claimed = False        # avertissement Z:C envoyé pour rattrapage
        self.catchup_wait_until = 0.0
        # rattrapage en groupe (retardataires de niveau >= 2)
        self.peer_leader = None     # meneur suivi (mode PEER)
        self.peer_joiners = {}      # meneur : participant -> dernier « PH »
        self.peer_phase = "IDLE"    # meneur : IDLE, FREEZING, CASTING
        self.peer_at = 0.0

    def log(self, msg):
        tag = self.role if self.role != "?" else "···"
        print("%7.2f [%s %s lvl%d] %s" % (time.monotonic() - START_TIME,
              tag, self.ident, self.level, msg), file=sys.stderr, flush=True)

    def run(self):
        """Point d'entrée : sessions successives (reconnexions volontaires)."""
        while True:
            try:
                self.run_session()
            except Dead:
                self.log("mort de faim, fin du programme")
                return 0
            except Respawn:
                self.net.close()
                self.egg_respawn = True
                self.reset_player_state()
                self.role = "DRONE"
                time.sleep(random.uniform(0.1, 0.7))
                self.log("reconnexion pour éclore au nid")
                continue
            except ConnectionClosed:
                if self.victory or self.level >= 8:
                    self.log("VICTOIRE : partie terminée au niveau %d"
                             % self.level)
                    return 0
                if self.role == "QUEEN" and self.ritual_phase == "CASTING" \
                        and self.level >= 7:
                    self.log("serveur fermé pendant le rituel final : "
                             "victoire présumée")
                    return 0
                self.log("connexion perdue (niveau %d)" % self.level)
                return 1
            except KeyboardInterrupt:
                self.log("interruption utilisateur")
                return 0

    def run_session(self):
        for attempt in range(5):
            self.net = LineSocket(self.host, self.port)
            try:
                self.net.connect()
                self.handshake()
                break
            except (OSError, ConnectionClosed) as exc:
                self.net.close()
                if attempt == 4:
                    print("[-] connexion impossible à %s:%d : %s"
                          % (self.host, self.port, exc), file=sys.stderr)
                    sys.exit(84)
                self.log("handshake raté (%s), nouvel essai" % exc)
                time.sleep(0.5 + attempt * 0.5)
        self.send("Inventory")
        if self.egg_respawn:
            self.role = "DRONE"
            self.mode = "WAIT_ASSIGN"
            self.home_known = True      # on vient d'éclore au nid
            self.bc("H", self.ident, "E")
        else:
            self.election_started = time.monotonic()
            self.bc("H", self.ident)
        while True:
            line = self.net.poll_line(0.02)
            drained = 0
            while line is not None and drained < 64:
                self.handle_line(line)
                drained += 1
                line = self.net.poll_line(0)
            self.tick()

    def handshake(self):
        welcome = self.net.read_line(5.0)
        if welcome != "WELCOME":
            raise ConnectionClosed("pas de WELCOME (%r)" % welcome)
        self.net.send_line(self.team)
        first = self.net.read_line(2.0)
        if first is None:
            # vieux handshake du serveur : "CLIENT-NUM 0" sans \n final
            if "CLIENT-NUM" in self.net.pending_raw():
                self.net.clear_raw()
                self.log("handshake dégradé accepté (CLIENT-NUM sans \\n)")
                return
            raise ConnectionClosed("pas de réponse au handshake")
        if first == "ko":
            print("[-] équipe refusée par le serveur", file=sys.stderr)
            sys.exit(84)
        if "CLIENT-NUM" in first:
            return
        try:
            slots = int(first)
        except ValueError:
            print("[-] handshake inattendu : %r" % first, file=sys.stderr)
            sys.exit(84)
        dims = self.net.read_line(1.5)
        try:
            w, h = (int(v) for v in (dims or "").split())
            self.map_w, self.map_h = max(1, w), max(1, h)
        except ValueError:
            pass
        self.log("connecté (slots restants: %d, monde: %s)"
                 % (slots, dims or "?"))

    # ------------------------------------------------------------------
    # Émission
    # ------------------------------------------------------------------

    def send(self, cmd):
        if len(self.pending) >= MAX_PENDING:
            return False
        self.net.send_line(cmd)
        self.pending.append((cmd, time.monotonic()))
        return True

    def bc(self, *fields):
        return self.send("Broadcast " + proto.zap_encode(*fields))

    def report_level(self, flag):
        self.bc("D", self.ident, self.level, flag, self.food_estimate())

    def in_flight(self, prefix):
        return any(cmd.startswith(prefix) for cmd, _ in self.pending)

    def busy(self):
        """Une action de terrain est en vol (les broadcasts ne comptent pas :
        leur réponse n'influe pas sur la position ni la vision)."""
        return any(not cmd.startswith("Broadcast") and cmd != "Inventory"
                   for cmd, _ in self.pending)

    def food_estimate(self):
        """Nourriture en stock, corrigée de la conversion continue en vie
        (1 unité toutes les 126/f) depuis la dernière synchro Inventory."""
        burned = int((time.monotonic() - self.inv_synced_at)
                     / FOOD_LIFE_PERIOD)
        return max(0, self.inventory.get("food", 0) - burned)

    def every(self, key, period):
        now = time.monotonic()
        if now - self.timers.get(key, 0.0) >= period:
            self.timers[key] = now
            return True
        return False

    def set_mode(self, mode):
        self.mode = mode
        self.mode_at = time.monotonic()

    # ------------------------------------------------------------------
    # Réception
    # ------------------------------------------------------------------

    def handle_line(self, line):
        if line == "dead":
            raise Dead()
        if line.startswith("message"):
            return self.on_broadcast(line)
        if line.startswith("eject"):
            return self.on_ejected()
        if line.startswith("seg"):
            self.victory = True
            return
        if line.startswith("Elevation underway"):
            if self.role == "DRONE":
                self.freeze(time.monotonic())
            return
        if line.startswith("Current level"):
            lvl = proto.parse_current_level(line)
            if lvl is None:
                return
            idx = next((i for i, (cmd, _) in enumerate(self.pending)
                        if cmd == "Incantation"), None)
            self_cast = idx is not None
            if self_cast:
                self.pending.pop(idx)
            self.on_level_change(lvl, self_cast)
            return
        self.match_response(line)

    def match_response(self, line):
        """Aligne la réponse sur la file en tolérant les décalages.

        Le serveur peut injecter des lignes en trop (ko immédiat quand on
        parle pendant un gel d'incantation, ko passif de rituel raté) : un
        appariement strictement FIFO partirait en vrille définitivement.
        """
        guard = 0
        while self.pending and guard <= len(self.pending):
            cmd, _ = self.pending[0]
            if self.response_fits(cmd, line):
                self.pending.pop(0)
                return self.on_response(cmd, line)
            if line in ("ok", "ko"):
                # ligne parasite : on ne consomme pas la file
                return self.on_stray(line)
            # la réponse de cmd s'est perdue : on la déclare ko et on
            # tente d'apparier la ligne à la commande suivante
            self.pending.pop(0)
            self.log("réponse de %r perdue (reçu %r)" % (cmd, line[:40]))
            self.on_response(cmd, "ko")
            guard += 1
        if not self.pending:
            self.on_stray(line)

    @staticmethod
    def response_fits(cmd, line):
        if cmd == "Look":
            return line.startswith("[")
        if cmd == "Inventory":
            return line.startswith("[") \
                or proto.parse_inventory(line) is not None
        if cmd == "Connect_nbr":
            return line.lstrip("-").isdigit()
        return line in ("ok", "ko")

    def on_response(self, cmd, line):
        now = time.monotonic()
        if cmd in ("Forward", "Left", "Right"):
            if line == "ok":
                self.apply_move(cmd)
            return
        if cmd == "Look":
            self.tile = proto.parse_look(line)
            self.tile_at = now
            self.tile_pos = self.pos
            if self.at_home():
                self.home_tile = dict(self.tile)
            if self.role == "QUEEN" and self.ritual_phase == "VERIFY":
                if self.level < 8 and self.at_home() and self.stones_ok():
                    self.send("Incantation")
                    self.ritual_phase = "CASTING"
                    self.phase_at = now
                else:
                    self.log("pierres manquantes au dernier contrôle")
                    self.ritual_fail(now)
            return
        if cmd == "Inventory":
            inv = proto.parse_inventory(line)
            if inv:
                self.inventory.update(inv)
                self.inv_synced_at = now
            return
        if cmd == "Connect_nbr":
            self.log("slots libres d'après le serveur : %s" % line)
            return
        if cmd.startswith("Take "):
            item = cmd.split()[1]
            if line == "ok":
                self.inventory[item] = self.inventory.get(item, 0) + 1
                self.tile[item] = max(0, self.tile.get(item, 0) - 1)
            else:
                self.tile[item] = 0
            if self.at_home():
                self.home_tile[item] = self.tile.get(item, 0)
            return
        if cmd.startswith("Set "):
            item = cmd.split()[1]
            if line == "ok":
                self.inventory[item] = max(0, self.inventory.get(item, 0) - 1)
                self.tile[item] = self.tile.get(item, 0) + 1
                if self.at_home():
                    self.home_tile[item] = self.tile[item]
            else:
                self.inventory[item] = 0
            return
        if cmd == "Fork":
            if line == "ok":
                self.egg_credit += 1
                self.log("oeuf pondu (crédit: %d)" % self.egg_credit)
            return
        if cmd == "Incantation" and line == "ko":
            if self.role == "QUEEN":
                self.log("incantation refusée (ko)")
                self.ritual_fail(now)
            else:
                if self.peer_phase == "CASTING":
                    self.log("rattrapage en groupe : incantation refusée")
                    self.peer_abort()
                self.set_mode("SIT" if self.assigned else "WAIT_ASSIGN")
                self.mode_before_freeze = None
                self.claimed = False
            return

    def on_stray(self, line):
        now = time.monotonic()
        if line == "ko":
            if self.role == "DRONE" and self.mode == "FROZEN":
                self.log("rituel raté (ko passif)")
                self.unfreeze()
            elif self.role == "QUEEN" and self.ritual_phase == "CASTING":
                self.ritual_fail(now)

    def on_level_change(self, lvl, self_cast):
        self.level = lvl
        self.level_since = time.monotonic()
        self.queen_level = max(self.queen_level, lvl)
        self.log("NIVEAU %d atteint (%s)" % (lvl, "rituel lancé par nous"
                 if self_cast else "élévation collective"))
        if self.role == "QUEEN":
            self.ritual_phase = "IDLE"
            self.ritual_retry_at = time.monotonic() + 1.0
            self.bc("QD", self.ident, lvl)
        elif self.role == "DRONE":
            if self.peer_phase != "IDLE" or self.peer_leader is not None:
                self.log("rattrapage en groupe réussi : niveau %d" % lvl)
            self.set_mode("SIT" if self.assigned else "WAIT_ASSIGN")
            self.mode_before_freeze = None
            self.claimed = False
            self.peer_reset()
            self.route = []
            self.report_level("C" if self_cast else "R")
        if lvl >= 8:
            self.log("*** NIVEAU 8 ATTEINT ***")

    def on_ejected(self):
        """Poussé par un adversaire : notre estime ne vaut plus rien."""
        self.tile_pos = None
        if self.role == "QUEEN":
            self.log("éjectée du nid ! le nid est perdu, nouveau nid ici")
            self.pos = HOME
            self.home_tile = {}
            self.route = []
            for entry in self.drones.values():
                entry["sat"] = False
            self.egg_credit = 0
            self.bc("QL", self.ident)
            return
        if self.home_known:
            self.log("éjecté : position du nid perdue")
        self.home_known = False
        self.route = []
        if self.role == "DRONE" and self.mode != "FROZEN":
            self.assigned = False
            self.set_mode("LIMBO")
            self.bc("H", self.ident, "L")

    # ------------------------------------------------------------------
    # Broadcasts d'équipe
    # ------------------------------------------------------------------

    def on_broadcast(self, line):
        parsed = proto.parse_message(line)
        if parsed is None:
            return
        _, text = parsed
        decoded = proto.zap_decode(text)
        if decoded is None:
            return              # message d'une équipe adverse : on ignore
        tag, fields = decoded
        if not fields or fields[0] == self.ident:
            return              # notre propre écho
        self.on_zap(tag, fields)

    def on_zap(self, tag, fields):
        now = time.monotonic()
        if tag.startswith("Q"):
            self.last_queen_seen = now
            if self.role == "QUEEN":
                return self.on_rival_queen(fields[0])
            self.qid = fields[0]
            if self.role == "?":
                self.role = "DRONE"
                self.home_known = False
                self.route = []
                self.log("reine détectée (%s), je suis un drone" % self.qid)
        lvl_field = None
        if len(fields) > 1 and fields[1].isdigit():
            lvl_field = int(fields[1])
        if tag == "H":
            self.peer_ids.add(fields[0])
            if self.role == "QUEEN":
                self.on_drone_hello(fields[0], fields[1:], now)
        elif tag == "D" and self.role == "QUEEN":
            self.on_drone_report(fields, now)
        elif tag == "C":
            # un niveau 1 va incanter au nid : silence pour les niveaux 1
            # présents (ils seront élevés avec lui)
            if self.role == "DRONE" and self.level == 1 \
                    and self.at_home() and not self.claimed:
                self.freeze(now, CLAIM_FREEZE_TIMEOUT)
        elif tag == "P":
            self.on_peer_call(fields[0], lvl_field, now)
        elif tag == "PH":
            # un participant est au nid : fields = [id, niveau, meneur]
            if self.mode == "PEERLEAD" and lvl_field == self.level \
                    and len(fields) > 2 and fields[2] == self.ident:
                self.peer_joiners[fields[0]] = now
        elif tag == "PF":
            if self.mode == "PEER" and fields[0] == self.peer_leader \
                    and lvl_field == self.level and self.at_home():
                self.freeze(now)
        elif tag == "PU":
            if self.peer_leader == fields[0] \
                    and self.mode in ("PEER", "FROZEN"):
                self.unfreeze()
                self.set_mode("SIT")
                self.peer_reset()
        elif tag == "QA":
            if lvl_field is not None:
                self.queen_level = lvl_field
        elif tag == "QS":
            if len(fields) > 1 and fields[1] == self.ident \
                    and self.mode == "WAIT_ASSIGN" and self.home_known:
                self.log("assignation : membre de la colonie, au fourrage")
                self.set_mode("SIT")
                self.assigned = True
                self.egg_respawn = False
                self.report_level("S")
        elif tag == "QR":
            if len(fields) > 1 and fields[1] == self.ident \
                    and self.mode in ("WAIT_ASSIGN", "SIT"):
                self.log("hors nid : en attente d'un oeuf de la reine")
                self.set_mode("LIMBO")
                self.assigned = False
                self.home_known = False
                self.route = []
        elif tag == "QE":
            if self.role == "DRONE" and self.mode == "LIMBO":
                raise Respawn()
        elif tag == "QL":
            if self.role == "DRONE" and self.mode != "FROZEN":
                self.log("le nid a été perdu : retour aux limbes")
                self.assigned = False
                self.home_known = False
                self.route = []
                self.set_mode("LIMBO")
        elif tag == "QC":
            if lvl_field is not None:
                self.queen_level = max(self.queen_level, lvl_field)
            if self.role == "DRONE" and self.assigned \
                    and lvl_field == self.level \
                    and self.mode in ("SIT", "CATCHUP"):
                self.set_mode("GATHER")
                self.route = [HOME]
                self.timers["home_report"] = 0.0
        elif tag == "QF":
            if lvl_field is not None:
                self.queen_level = max(self.queen_level, lvl_field)
            if os.environ.get("ZDEBUG"):
                self.log("DBG QF pos=%s home=%s mode=%s pending=%s"
                         % (self.pos, self.home_known, self.mode,
                            [c for c, _ in self.pending]))
            if self.role == "DRONE" and self.at_home() \
                    and lvl_field == self.level \
                    and self.mode not in ("FROZEN", "LIMBO"):
                self.freeze(now)
                self.mode_before_freeze = "SIT" if self.assigned \
                    else self.mode_before_freeze
        elif tag == "QU":
            if self.role == "DRONE":
                if not self.in_peer_ritual():
                    self.unfreeze()
                if self.mode == "GATHER":
                    self.set_mode("SIT")
        elif tag == "QD":
            if lvl_field is not None:
                self.queen_level = max(self.queen_level, lvl_field)
            if self.role == "DRONE":
                if not self.in_peer_ritual():
                    self.unfreeze()
                if self.mode == "GATHER":
                    self.set_mode("SIT")
                if self.level < self.queen_level:
                    self.on_missed_ritual()

    def on_missed_ritual(self):
        if self.mode == "LIMBO" or not self.assigned:
            return      # un oeuf nous remettra de toute façon à zéro
        if self.level == 1:
            self.log("rituel manqué : rattrapage solo 1->2 au nid")
        else:
            self.log("ATTENTION : largué au niveau %d (reine: %d)"
                     % (self.level, self.queen_level))
            self.report_level("S")

    # --- Rattrapage en groupe ------------------------------------------

    def is_straggler(self):
        """Drone de la colonie resté sous le niveau de la reine (>= 2 : le
        niveau 1 a déjà son rattrapage solo)."""
        return self.role == "DRONE" and self.assigned \
            and 2 <= self.level < min(self.queen_level, 7)

    def on_peer_call(self, leader, lvl, now):
        """Un retardataire propose un rituel entre retardataires."""
        if lvl != self.level or not self.is_straggler():
            return
        if self.mode == "PEERLEAD":
            if leader < self.ident:     # deux meneurs : le plus petit id
                self.log("rattrapage en groupe : je suis %s" % leader)
                self.bc("PU", self.ident)
                self.peer_reset()
                self.join_peer(leader, now)
            return
        if self.mode == "PEER":
            if leader < self.peer_leader:
                self.peer_leader = leader
            return
        if self.mode == "SIT" \
                and self.food_estimate() >= RITUAL_MIN_FOOD:
            self.join_peer(leader, now)

    def join_peer(self, leader, now):
        self.log("rattrapage en groupe : je rejoins %s au nid (%d->%d)"
                 % (leader, self.level, self.level + 1))
        self.set_mode("PEER")
        self.peer_leader = leader
        self.route = [HOME]
        self.timers["peer_home"] = 0.0

    def start_peer_lead(self, now):
        self.log("rattrapage en groupe : je propose un rituel %d->%d entre "
                 "retardataires" % (self.level, self.level + 1))
        self.set_mode("PEERLEAD")
        self.peer_joiners = {}
        self.peer_phase = "IDLE"
        self.timers["peer_call"] = now
        self.bc("P", self.ident, self.level)

    def in_peer_ritual(self):
        """Gelé pour un rituel entre retardataires (pas celui de la reine)."""
        return self.mode == "FROZEN" and (self.peer_leader is not None
                                          or self.peer_phase != "IDLE")

    def peer_reset(self):
        self.peer_leader = None
        self.peer_joiners = {}
        self.peer_phase = "IDLE"

    def peer_abort(self):
        """Le meneur renonce : il libère ses participants."""
        if self.mode == "PEERLEAD" or self.peer_phase != "IDLE":
            self.bc("PU", self.ident)
        self.peer_reset()
        if self.mode in ("PEERLEAD", "PEER"):
            self.set_mode("SIT")

    def tick_peer_lead(self, now):
        if self.level >= self.queen_level \
                or self.food_estimate() < FOOD_CRITICAL \
                or now - self.mode_at > PEER_TIMEOUT:
            self.log("rattrapage en groupe abandonné (%d participant(s))"
                     % len(self.peer_joiners))
            return self.peer_abort()
        if not self.at_home():
            self.route = [HOME]
            self.forage_step(rush=True)
            return
        if self.peer_phase == "FREEZING":
            if now - self.peer_at < FREEZE_SETTLE or self.busy():
                return
            if not self.tile_fresh(0.5 * TIME_SCALE):
                self.send("Look")
                return
            if not self.stones_ok():
                self.log("rattrapage en groupe : pierres manquantes")
                return self.peer_abort()
            present = self.peer_present(now)
            self.log("rattrapage en groupe : incantation %d->%d à %d"
                     % (self.level, self.level + 1, len(present) + 1))
            self.peer_phase = "CASTING"
            self.send("Incantation")
            self.freeze(now)
            return
        if self.every("peer_call", PEER_CALL_PERIOD):
            self.bc("P", self.ident, self.level)
        self.idle_step()
        required = proto.ELEVATION[self.level]["players"]
        if len(self.peer_present(now)) + 1 >= required \
                and not self.busy() and self.tile_fresh() \
                and self.stones_ok():
            self.bc("PF", self.ident, self.level)
            self.peer_phase = "FREEZING"
            self.peer_at = now

    def peer_present(self, now):
        return [i for i, t in self.peer_joiners.items()
                if now - t < PEER_PRESENT]

    def tick_peer(self, now):
        if not self.is_straggler() \
                or self.food_estimate() < FOOD_CRITICAL \
                or now - self.mode_at > PEER_TIMEOUT + 5.0 * TIME_SCALE:
            self.set_mode("SIT")
            self.peer_reset()
            return
        if not self.at_home():
            self.route = [HOME]
            self.forage_step(rush=True)
            return
        # « au nid » seulement sans déplacement en vol : la position estimée
        # ne compte que les pas acquittés, un Forward encore en file nous
        # ferait repartir après l'annonce (et le serveur refuserait le rituel)
        if not self.busy() and self.every("peer_home", PEER_HOME_PERIOD):
            self.bc("PH", self.ident, self.level, self.peer_leader)
        if self.peer_leader is not None and self.every("peer_look",
                                                       LOOK_PERIOD * 4):
            self.idle_step()

    def on_rival_queen(self, other_id):
        # deux reines (élection simultanée) : la plus petite id l'emporte
        if other_id < self.ident:
            self.log("abdication au profit de la reine %s" % other_id)
            self.role = "DRONE"
            self.set_mode("WAIT_ASSIGN")
            self.home_known = False
            self.route = []
            self.qid = other_id
        else:
            self.timers["qa"] = 0.0     # on ré-annonce immédiatement

    # Reine : un drone se présente
    def on_drone_hello(self, ident, flags, now):
        entry = self.drones.setdefault(
            ident, {"level": 1, "seen": now, "sat": False})
        entry["seen"] = now
        if self.is_frozen():
            return      # on répondra à son prochain hello
        if "L" in flags:
            entry["sat"] = False
            self.bc("QR", self.ident, ident)
            return
        if entry["sat"]:
            self.bc("QS", self.ident, ident)
            return
        # strict : seul un drone qui vient d'éclore d'un oeuf (flag E) est
        # au nid ; les autres doivent se reconnecter au signal QE
        if "E" in flags and self.egg_credit > 0:
            self.egg_credit -= 1
            entry["sat"] = True
            self.bc("QS", self.ident, ident)
            self.log("drone %s intégré (crédit oeufs: %d)"
                     % (ident, self.egg_credit))
        else:
            self.bc("QR", self.ident, ident)

    def on_drone_report(self, fields, now):
        if len(fields) < 2 or not fields[1].isdigit():
            return
        ident, lvl = fields[0], int(fields[1])
        entry = self.drones.setdefault(
            ident, {"level": lvl, "seen": now, "sat": False})
        entry["level"] = lvl
        entry["seen"] = now
        if len(fields) > 2:
            entry["home"] = fields[2] == "H" \
                and self.ritual_phase == "CALLING"
        if len(fields) > 3 and fields[3].isdigit():
            entry["food"] = int(fields[3])

    # ------------------------------------------------------------------
    # Navigation à l'estime
    # ------------------------------------------------------------------

    def apply_move(self, cmd):
        if cmd == "Forward":
            dx, dy = DIRS[self.heading]
            self.pos = (self.pos[0] + dx, self.pos[1] + dy)
        elif cmd == "Right":
            self.heading = (self.heading + 1) % 4
        else:
            self.heading = (self.heading + 3) % 4

    def at_home(self):
        return self.home_known and self.pos == HOME

    def tile_fresh(self, max_age=1.0 * TIME_SCALE):
        return self.tile_pos == self.pos \
            and time.monotonic() - self.tile_at <= max_age

    def nav_command(self, target):
        dx = target[0] - self.pos[0]
        dy = target[1] - self.pos[1]
        if dx == 0 and dy == 0:
            return None

        def gain(h):
            return DIRS[h][0] * dx + DIRS[h][1] * dy

        if gain(self.heading) > 0:
            return "Forward"
        if gain((self.heading + 1) % 4) > 0:
            return "Right"
        return "Left"

    def max_reach(self):
        return max(2, min(self.map_w, self.map_h) // 2 - 1)

    def plan_trip(self, max_len):
        """Pétale : décalage latéral k, aller sur L cases, pas de côté,
        retour par la voie parallèle, puis rentrée au nid."""
        max_len = max(2, max_len)
        length = random.randint(max(2, max_len // 2), max_len)
        ux, uy = random.choice(DIRS)
        side = random.choice((1, -1))
        vx, vy = -uy * side, ux * side
        k = random.randint(-length, length)

        def pt(a, b):
            return (a * vx + b * ux, a * vy + b * uy)

        self.route = [pt(k, 0), pt(k, length), pt(k + 1, length),
                      pt(k + 1, 0), HOME]

    def plan_wander(self):
        reach = self.max_reach()
        self.route = [(self.pos[0] + random.randint(-reach, reach),
                       self.pos[1] + random.randint(-reach, reach))]

    def next_target(self):
        while self.route and self.route[0] == self.pos:
            self.route.pop(0)
        return self.route[0] if self.route else None

    # ------------------------------------------------------------------
    # Ramassage / dépôt
    # ------------------------------------------------------------------

    def stone_wants(self):
        # un retardataire ramasse aussi les pierres de son propre niveau
        base = self.level if self.role == "DRONE" \
            else max(self.level, self.queen_level)
        need = stones_needed_from(base)
        wants = {}
        for stone, n in need.items():
            have = self.inventory.get(stone, 0)
            if self.home_known:
                have += self.home_tile.get(stone, 0)
            if n > have:
                wants[stone] = n - have
        return wants

    def take_here(self, stones=True):
        """Envoie les Take utiles sur la case (vision fraîche requise)."""
        sent = 0
        food = self.tile.get("food", 0)
        if food > 0 and self.food_estimate() < FOOD_TARGET:
            for _ in range(min(food, TAKE_BURST)):
                sent += self.send("Take food")
        if stones and not self.at_home():
            for stone, want in self.stone_wants().items():
                for _ in range(min(want, self.tile.get(stone, 0))):
                    sent += self.send("Take " + stone)
        return sent > 0

    def deposit(self):
        """Au nid : on dépose toutes nos pierres pour les rituels."""
        sent = 0
        for stone in proto.STONES:
            for _ in range(self.inventory.get(stone, 0)):
                sent += self.send("Set " + stone)
        return sent > 0

    def forage_step(self, rush=False):
        """Un pas d'expédition : voir, ramasser, déposer, avancer."""
        if self.busy():
            return
        hungry = self.food_estimate() < FOOD_CRITICAL
        if not rush or hungry:
            if not self.tile_fresh():
                self.send("Look")
                return
            if self.take_here(stones=not rush):
                return
        if self.at_home() and self.deposit():
            return
        if self.at_home() and self.on_home_visit():
            return
        target = self.next_target()
        if target is None:
            if rush:
                return
            if self.home_known:
                # les abords du nid sont ratissés par tout le monde : la
                # reine ne fait de courtes sorties que si elle est repue
                reach = self.max_reach()
                if self.role == "QUEEN" and self.food_estimate() >= 15:
                    reach = max(3, reach // 2)
                self.plan_trip(reach)
            else:
                self.plan_wander()
            target = self.next_target()
        cmd = self.nav_command(target) if target else None
        if cmd:
            self.send(cmd)

    def idle_step(self):
        """Sur place : voir, manger, déposer."""
        if self.busy():
            return
        if not self.tile_fresh(LOOK_PERIOD):
            self.send("Look")
            return
        if self.take_here(stones=False):
            return
        if self.at_home():
            self.deposit()

    def on_home_visit(self):
        """Passage au nid en fin d'expédition. True = on reste au nid."""
        if self.role == "QUEEN":
            return self.queen_at_home(time.monotonic())
        if self.role == "DRONE" and self.mode == "SIT" and self.level == 1 \
                and self.queen_level >= 2 \
                and self.food_estimate() >= RITUAL_MIN_FOOD \
                and self.home_tile.get("linemate", 0) >= 1:
            self.set_mode("CATCHUP")
            self.claimed = False
            self.catchup_wait_until = time.monotonic() \
                + random.uniform(0.1, 0.8)
            return True
        # retardataire de niveau >= 2 : rituel entre retardataires si le nid
        # a les pierres de notre niveau
        if self.mode == "SIT" and self.is_straggler() \
                and self.food_estimate() >= RITUAL_MIN_FOOD \
                and all(self.home_tile.get(s, 0) >= n for s, n in
                        proto.ELEVATION[self.level]["stones"].items()) \
                and self.every("peer_propose", PEER_TIMEOUT):
            self.start_peer_lead(time.monotonic())
            return True
        return False

    # ------------------------------------------------------------------
    # Décisions
    # ------------------------------------------------------------------

    def tick(self):
        now = time.monotonic()
        if self.pending and self.pending[0][0] != "Incantation" \
                and now - self.pending[0][1] > PENDING_TIMEOUT:
            cmd, _ = self.pending.pop(0)
            self.log("réponse perdue pour %r, on désynchronise" % cmd)
        if os.environ.get("ZDEBUG") and self.every("dbg", 5.0):
            self.log("DBG mode=%s phase=%s pos=%s food=%d tile=%s home=%s "
                     "inv=%s route=%s pending=%s"
                     % (self.mode, self.ritual_phase, self.pos,
                        self.food_estimate(), self.tile, self.home_tile,
                        self.inventory, self.route[:3],
                        [c for c, _ in self.pending]))
        if self.role == "?":
            return self.tick_election(now)
        if self.is_frozen():
            if self.role == "DRONE":
                if now - self.frozen_at > self.frozen_timeout:
                    self.unfreeze()
            elif self.role == "QUEEN":
                self.queen_ritual_watchdog(now)
            return
        if self.every("inv", INVENTORY_PERIOD) \
                and not self.in_flight("Inventory"):
            self.send("Inventory")
        if self.role == "QUEEN":
            self.tick_queen(now)
        else:
            self.tick_drone(now)

    def is_frozen(self):
        if self.role == "DRONE":
            return self.mode == "FROZEN"
        return self.ritual_phase in ("FREEZING", "VERIFY", "CASTING")

    def freeze(self, now, timeout=FROZEN_TIMEOUT):
        """Silence radio : le serveur répond ko à toute commande envoyée
        pendant un gel d'incantation, ce qui désynchronise la file."""
        if self.mode != "FROZEN":
            self.mode_before_freeze = self.mode
            self.mode = "FROZEN"
            self.frozen_at = now
            self.frozen_timeout = timeout

    def unfreeze(self):
        if self.mode == "FROZEN":
            self.set_mode(self.mode_before_freeze or "SIT")
            self.mode_before_freeze = None

    def tick_election(self, now):
        if self.every("hello", HELLO_PERIOD):
            self.bc("H", self.ident)
        self.forage_step()          # on mange pendant le scrutin
        waited = now - self.election_started
        if waited < ELECTION_WINDOW:
            return
        rivals = [p for p in self.peer_ids if p < self.ident]
        if rivals and waited < ELECTION_WINDOW * 2.5:
            return              # un candidat prioritaire va se couronner
        self.become_queen()

    def become_queen(self):
        self.role = "QUEEN"
        self.level_since = time.monotonic()
        self.pos = HOME             # le nid est ici
        self.home_known = True
        self.home_tile = {}
        self.tile_pos = None
        self.route = []
        # délai de grâce : les clients déjà lancés se présentent d'abord,
        # on ne spawne des remplaçants que pour les places vraiment vides
        self.timers["spawn"] = time.monotonic() + 4.0
        self.log("aucune reine détectée : je prends la couronne, nid ici")
        self.bc("QA", self.ident, self.level)
        self.send("Connect_nbr")

    # --- Reine ---------------------------------------------------------

    def tick_queen(self, now):
        if self.every("qa", QUEEN_ANNOUNCE_PERIOD):
            self.bc("QA", self.ident, self.level)
        self.prune_drones(now)
        sat = self.sat_count()
        limbo = max(0, len(self.drones) - sat)
        if sat < DRONES_TARGET and limbo > 0 \
                and self.egg_credit >= min(limbo, DRONES_TARGET - sat) \
                and self.every("qe", 2.0 * TIME_SCALE):
            self.bc("QE", self.ident, self.egg_credit)
        if len(self.drones) < DRONES_TARGET and limbo == 0 \
                and self.egg_credit > 0 \
                and self.spawned < MAX_SPAWNS \
                and self.every("spawn", 3.0 * TIME_SCALE):
            missing = min(self.egg_credit, DRONES_TARGET - len(self.drones),
                          MAX_SPAWNS - self.spawned)
            for _ in range(missing):
                self.spawn_replacement()
        if self.ritual_phase == "CALLING":
            return self.tick_calling(now)
        self.forage_step()

    def queen_at_home(self, now):
        """Au nid avec une vision fraîche : pondre, puis rituel ou départ."""
        need = DRONES_TARGET - self.sat_count() - self.egg_credit
        if need > 0 and self.every("fork", 0.3 * TIME_SCALE):
            self.send("Fork")
            return True
        if self.can_call(now):
            self.start_call(now)
            return True
        return False

    def can_call(self, now):
        if self.level >= 8 or now < self.ritual_retry_at:
            return False
        if self.food_estimate() < RITUAL_MIN_FOOD:
            return False            # on mange d'abord, on rituelle ensuite
        if not self.tile_fresh() or not self.stones_ok():
            return False
        return self.ritual_bodies_ok(now)

    def start_call(self, now):
        for entry in self.drones.values():
            entry["home"] = False
        self.ritual_phase = "CALLING"
        self.phase_at = now
        self.route = [HOME]
        self.log("rappel de la colonie au nid pour le rituel %d->%d"
                 % (self.level, self.level + 1))
        self.bc("QC", self.ident, self.level)

    def tick_calling(self, now):
        if not self.at_home():
            self.forage_step(rush=True)
            return
        self.idle_step()
        if self.every("qc", 3.0) and now - self.phase_at > 1.0:
            self.bc("QC", self.ident, self.level)   # rappel répété
        required = proto.ELEVATION[self.level]["players"]
        expected = [d for d in self.drones.values()
                    if d["sat"] and d["level"] == self.level]
        home = [d for d in expected if d.get("home")]
        waited = now - self.phase_at
        if len(home) + 1 >= required \
                and (len(home) >= len(expected) or waited > GATHER_PATIENCE):
            if self.busy() or not self.tile_fresh() or not self.stones_ok():
                return
            roster = ", ".join("%s lvl%d f%d%s" % (
                i, d["level"], d.get("food", -1),
                "@" if d.get("home") else "-")
                for i, d in sorted(self.drones.items()))
            self.log("gel de la colonie pour le rituel %d->%d [%s]"
                     % (self.level, self.level + 1, roster))
            self.bc("QF", self.ident, self.level)
            self.ritual_phase = "FREEZING"
            self.phase_at = now
        elif waited > GATHER_TIMEOUT:
            self.log("rappel infructueux (%d/%d au nid)"
                     % (len(home) + 1, required))
            self.ritual_fail(now)

    def spawn_replacement(self):
        """Lance un nouveau processus client pour occuper un oeuf pondu
        (drone mort ou jamais lancé). --hatched lui dit qu'il éclot."""
        self.spawned += 1
        script = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                              "main.py")
        self.log("spawn d'un drone de remplacement (%d/%d)"
                 % (self.spawned, MAX_SPAWNS))
        subprocess.Popen(
            [sys.executable, script, "-p", str(self.port), "-n", self.team,
             "-h", self.host, "--hatched"],
            stdout=subprocess.DEVNULL, stderr=sys.stderr,
            start_new_session=True)

    def queen_ritual_watchdog(self, now):
        if self.ritual_phase == "CASTING" \
                and now - self.phase_at > RITUAL_RESULT_TIMEOUT:
            self.log("rituel sans réponse, abandon")
            self.ritual_fail(now)
        elif self.ritual_phase == "FREEZING":
            if now - self.phase_at >= FREEZE_SETTLE and not self.pending:
                self.send("Look")
                self.ritual_phase = "VERIFY"
                self.phase_at = now
            elif now - self.phase_at > 5.0:
                self.ritual_fail(now)
        elif self.ritual_phase == "VERIFY" and now - self.phase_at > 5.0:
            self.ritual_fail(now)

    def ritual_fail(self, now):
        self.ritual_phase = "IDLE"
        self.ritual_retry_at = now + RITUAL_RETRY_DELAY
        self.bc("QU", self.ident)

    def sat_count(self):
        return sum(1 for d in self.drones.values() if d["sat"])

    def prune_drones(self, now):
        for ident, entry in list(self.drones.items()):
            if now - entry["seen"] > DRONE_PRUNE_AFTER:
                self.log("drone %s silencieux, présumé mort" % ident)
                del self.drones[ident]

    def stones_ok(self):
        stones = proto.ELEVATION.get(self.level, {}).get("stones", {})
        return all(self.tile.get(s, 0) >= n for s, n in stones.items())

    def ritual_bodies_ok(self, now):
        sat = [d for d in self.drones.values() if d["sat"]]
        if self.level == 1:
            return True         # 1->2 : on emmène les niveaux 1 présents
        required = proto.ELEVATION[self.level]["players"]
        ready = [d for d in sat if d["level"] >= self.level]
        if len(ready) + 1 < required:
            return False        # le serveur dirait ko de toute façon
        fed = [d for d in ready if d.get("food", 0) >= RITUAL_MIN_FOOD]
        if now - self.level_since > LEVEL_WAIT_RELAXED:
            # tant pis pour les retardataires, mais pas de rituel à jeun
            return len(fed) + 1 >= required
        # régime normal : on attend les drones encore rattrapables
        # (au-delà du niveau 2, un retardataire ne peut plus nous rejoindre)
        waitable = [d for d in sat
                    if d["level"] >= self.level or self.level <= 2]
        if len(waitable) > len(ready):
            return False
        return len(fed) == len(ready)

    # --- Drone ---------------------------------------------------------

    def tick_drone(self, now):
        if now - self.last_queen_seen > QUEEN_SILENCE_TIMEOUT:
            self.log("reine silencieuse : nouvelle élection")
            self.role = "?"
            self.qid = None
            self.election_started = now
            self.peer_ids = set()
            self.home_known = False
            self.route = []
            return
        if self.mode == "WAIT_ASSIGN":
            if self.every("hello", HELLO_PERIOD):
                if self.egg_respawn:
                    self.bc("H", self.ident, "E")
                else:
                    self.bc("H", self.ident)
            self.idle_step()
            return
        if self.mode == "LIMBO":
            if self.every("hello", HELLO_PERIOD * 4):
                self.bc("H", self.ident, "L")
            self.forage_step()
            return
        if self.every("report", DRONE_REPORT_PERIOD):
            self.report_level("H" if self.mode == "GATHER"
                              and self.at_home() else "S")
        if self.mode == "SIT":
            self.forage_step()
        elif self.mode == "GATHER":
            self.tick_gather(now)
        elif self.mode == "CATCHUP":
            self.tick_catchup(now)
        elif self.mode == "PEERLEAD":
            self.tick_peer_lead(now)
        elif self.mode == "PEER":
            self.tick_peer(now)

    def tick_gather(self, now):
        if now - self.mode_at > GATHER_DRONE_TIMEOUT \
                or self.food_estimate() < FOOD_CRITICAL:
            self.set_mode("SIT")        # mieux vaut largué que mort
            self.report_level("S")
            return
        if not self.at_home():
            self.route = [HOME]
            self.forage_step(rush=True)
            return
        if self.every("home_report", 1.5):
            self.report_level("H")
        self.idle_step()

    def tick_catchup(self, now):
        """Rattrapage 1->2 au nid : on prévient par Z:C (les autres niveaux
        1 présents se taisent et seront élevés avec nous), puis on incante
        dès que la case a un linemate."""
        if self.level != 1:
            self.set_mode("SIT")
            self.claimed = False
            return
        if not self.at_home():
            self.route = [HOME]
            self.forage_step(rush=True)
            return
        if now < self.catchup_wait_until or self.in_flight("Incantation"):
            return
        if not self.claimed:
            self.claimed = True
            self.catchup_wait_until = now + 0.5
            self.bc("C", self.ident)
            return
        if self.busy() or not self.tile_fresh(0.5 * TIME_SCALE):
            if not self.busy():
                self.send("Look")
            return
        if self.tile.get("linemate", 0) >= 1:
            self.log("rattrapage : incantation solo 1->2")
            self.send("Incantation")
            self.freeze(now)    # silence pendant notre propre rituel
        else:
            self.claimed = False
            self.set_mode("SIT")    # on repart chercher du linemate
