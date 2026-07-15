#!/usr/bin/env python3
# main.py
import argparse
from zappy_ai import ZappyAI

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Zappy AI Client", add_help=False)
    parser.add_argument("--help", action="help", help="Affiche l'aide")
    parser.add_argument("-p", type=int, required=True, help="Port du serveur")
    parser.add_argument("-n", type=str, required=True, help="Nom de l'équipe")
    parser.add_argument("-h", dest="host", type=str, default="localhost", help="Machine hôte")
    args = parser.parse_args()
    ai = ZappyAI(host=args.host, port=args.p, team_name=args.n)
    ai.authenticate()
    try:
        ai.run()
    except KeyboardInterrupt:
        print("\n[*] Arrêt de l'IA (Interruption utilisateur).")
