#!/usr/bin/env python3
# main.py
"""Point d'entrée du client zappy_ai."""

import argparse
import sys

from brain import Agent


def main():
    parser = argparse.ArgumentParser(
        description="Zappy AI Client", add_help=False,
        usage="./zappy_ai -p port -n name -h machine")
    parser.add_argument("--help", action="help", help="Affiche l'aide")
    parser.add_argument("-p", dest="port", type=int, required=True,
                        help="Port du serveur")
    parser.add_argument("-n", dest="team", type=str, required=True,
                        help="Nom de l'équipe")
    parser.add_argument("-h", dest="host", type=str, default="localhost",
                        help="Machine hôte (défaut: localhost)")
    parser.add_argument("--hatched", action="store_true",
                        help=argparse.SUPPRESS)
    args = parser.parse_args()

    agent = Agent(args.host, args.port, args.team)
    agent.egg_respawn = args.hatched
    try:
        sys.exit(agent.run())
    except KeyboardInterrupt:
        print("\n[*] Arrêt de l'IA (interruption utilisateur).",
              file=sys.stderr)
        sys.exit(0)


if __name__ == "__main__":
    main()
