# logger.py


class Logger:
    """Trace préfixée par l'id de lignée de l'IA (1, 1.1, 1.2...)."""

    def __init__(self, ai_id="1"):
        self.ai_id = ai_id

    def __call__(self, msg):
        print(f"[IA {self.ai_id}] {msg}", flush=True)
