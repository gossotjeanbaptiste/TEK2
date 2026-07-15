# network.py
import socket
import sys

from logger import Logger


class NetworkClient:
    # Garde-fou : aucun serveur sain ne reste muet aussi longtemps envers
    # un drone vivant (la plus longue action, Incantation, dure 300/f).
    # Évite un processus fantôme si le signal de mort se perd.
    WATCHDOG_SECONDS = 10

    def __init__(self, host, port, logger=None):
        self.host = host
        self.port = port
        self.socket = None
        self.buffer = ""
        self.log = logger if logger else Logger()

    def connect(self):
        """Établit la connexion TCP."""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            self.socket.settimeout(self.WATCHDOG_SECONDS)
            self.log(f"[*] Connecté au serveur {self.host}:{self.port}")
        except Exception as e:
            self.log(f"[-] Erreur de connexion : {e}")
            sys.exit(1)

    def read_line(self):
        """Lit la socket de manière sécurisée et découpe strictement au retour à la ligne."""
        while "\n" not in self.buffer:
            try:
                data = self.socket.recv(4096).decode("utf-8")
            except socket.timeout:
                self.log(f"\n[-] Aucune nouvelle du serveur depuis {self.WATCHDOG_SECONDS}s : "
                         "le drone est probablement mort, arrêt du programme.")
                sys.exit(0)
            except (ConnectionResetError, BrokenPipeError):
                self.log("\n[-] Le serveur a coupé la connexion brutalement.")
                sys.exit(0)

            if not data:
                self.log("[-] Connexion interrompue par le serveur.")
                sys.exit(1)
            self.buffer += data

        line, self.buffer = self.buffer.split("\n", 1)

        # ⚠️ SÉCURITÉ MORT : On intercepte le signal du serveur ici
        if "dead" in line:
            self.log(
                "\n[-] Signal de mort reçu du serveur. Fin de la partie, vous êtes mort de faim !")
            sys.exit(0)

        return line.strip()

    def send_command(self, cmd):
        """Envoie une chaîne brute terminée par un saut de ligne."""
        try:
            self.socket.sendall(f"{cmd}\n".encode("utf-8"))
        except (ConnectionResetError, BrokenPipeError):
            self.log("\n[-] Le serveur a fermé la connexion.")
            sys.exit(0)

    def close(self):
        """Ferme proprement la connexion et purge le tampon de lecture."""
        if self.socket:
            try:
                self.socket.close()
            except OSError:
                pass
            self.socket = None
        self.buffer = ""
