# network.py
"""Socket TCP avec lecture ligne par ligne, sans jamais bloquer (select)."""

import select
import socket
import time


class ConnectionClosed(Exception):
    """Le serveur a fermé la connexion."""


class LineSocket:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.sock = None
        self.buffer = b""

    def connect(self, timeout=5.0):
        self.sock = socket.create_connection((self.host, self.port),
                                             timeout=timeout)
        self.sock.setblocking(False)
        self.buffer = b""

    def close(self):
        if self.sock is not None:
            try:
                self.sock.close()
            except OSError:
                pass
        self.sock = None
        self.buffer = b""

    def _fill(self, timeout):
        """Attend au plus `timeout` s des données. False si rien reçu."""
        ready, _, _ = select.select([self.sock], [], [], max(0.0, timeout))
        if not ready:
            return False
        try:
            data = self.sock.recv(4096)
        except (ConnectionResetError, OSError) as exc:
            raise ConnectionClosed() from exc
        if not data:
            raise ConnectionClosed()
        self.buffer += data
        return True

    def _pop_line(self):
        if b"\n" not in self.buffer:
            return None
        line, self.buffer = self.buffer.split(b"\n", 1)
        return line.decode(errors="replace").strip()

    def poll_line(self, timeout=0.0):
        """Retourne une ligne (sans \\n) ou None si aucune dans le délai."""
        line = self._pop_line()
        if line is not None:
            return line
        if self._fill(timeout):
            return self._pop_line()
        return None

    def read_line(self, timeout):
        """Attend une ligne complète jusqu'à `timeout` secondes au total."""
        deadline = time.monotonic() + timeout
        while True:
            remain = deadline - time.monotonic()
            if remain <= 0:
                return None
            line = self.poll_line(remain)
            if line is not None:
                return line

    def pending_raw(self):
        """Contenu du tampon sans \\n final (handshake dégradé)."""
        return self.buffer.decode(errors="replace")

    def clear_raw(self):
        self.buffer = b""

    def send_line(self, text):
        data = (text + "\n").encode()
        while data:
            _, ready, _ = select.select([], [self.sock], [], 5.0)
            if not ready:
                raise ConnectionClosed()
            try:
                sent = self.sock.send(data)
            except (BrokenPipeError, ConnectionResetError, OSError) as exc:
                raise ConnectionClosed() from exc
            data = data[sent:]
