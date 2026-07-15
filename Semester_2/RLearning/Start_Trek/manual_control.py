"""
Script de contrôle manuel pour LunarLander-v3
Permet de piloter le lander au clavier

Contrôles:
  - Flèches (←/→/↓) ou AZERTY (Z/Q/D) ou QWERTY (W/A/D) : rotation et moteur
  - Space : moteur principal
  - R : redémarrer l'épisode
  - Esc : quitter
"""

import gymnasium as gym
import numpy as np
import argparse
from gymnasium.wrappers import RecordVideo
import pygame

# Initialiser pygame pour la gestion du clavier
pygame.init()


class ManualController:
    """Contrôleur manuel pour LunarLander avec affichage pygame"""

    def __init__(self, env_name="LunarLander-v3", render_mode="human", record_video=False):
        """
        Args:
            env_name: Nom de l'environnement
            render_mode: "human" pour affichage pygame
            record_video: Si True, enregistre les vidéos
        """
        self.env = gym.make(env_name, render_mode=render_mode)
        self.record_video = record_video

        if record_video:
            self.env = RecordVideo(
                self.env, "manual_videos", episode_trigger=lambda ep: True)

        self.running = True
        self.episode_reward = 0
        self.episode_length = 0
        self.episode_count = 0
        self.restart_needed = False
        self.keys_pressed = set()  # Ensemble des touches actuellement pressées

    def get_action_from_keys(self):
        """
        Récupère l'action à partir des touches actuellement pressées
        Returns: Action (0-3)
        """
        # Vérifier les flèches en priorité (moins ambigues)
        if pygame.K_DOWN in self.keys_pressed:
            return 2  # Moteur principal
        if pygame.K_LEFT in self.keys_pressed:
            return 1  # Rotation gauche
        if pygame.K_RIGHT in self.keys_pressed:
            return 3  # Rotation droite

        # Puis vérifier les lettres
        if pygame.K_SPACE in self.keys_pressed:
            return 1
        if pygame.K_w in self.keys_pressed or pygame.K_z in self.keys_pressed:
            return 2  # Moteur principal
        if pygame.K_a in self.keys_pressed or pygame.K_q in self.keys_pressed:
            return 1  # Rotation gauche
        if pygame.K_d in self.keys_pressed:
            return 3  # Rotation droite

        return 0  # Aucune action

    def handle_events(self):
        """
        Gère les événements pygame
        Returns: False si l'utilisateur veut quitter, True sinon
        """
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return False
            elif event.type == pygame.KEYDOWN:
                self.keys_pressed.add(event.key)
                if event.key == pygame.K_ESCAPE:
                    return False
                elif event.key == pygame.K_r:
                    # Redémarrer l'épisode
                    self.restart_needed = True
            elif event.type == pygame.KEYUP:
                self.keys_pressed.discard(event.key)

        return True

    def reset_episode(self):
        """Réinitialise l'épisode"""
        self.state, self.info = self.env.reset()
        self.episode_reward = 0
        self.episode_length = 0
        self.episode_count += 1
        print(f"\n🎮 Épisode {self.episode_count} redémarré")

    def run(self):
        """Boucle principale du contrôle manuel"""
        print("=" * 70)
        print("🚀 LUNAR LANDER - CONTRÔLE MANUEL")
        print("=" * 70)
        print("\n📖 CONTRÔLES:")
        print("  AZERTY:")
        print("    Z            : Moteur principal (descendre)")
        print("    Q            : Rotation gauche")
        print("    D            : Rotation droite")
        print("  QWERTY:")
        print("    W            : Moteur principal (descendre)")
        print("    A            : Rotation gauche")
        print("    D            : Rotation droite")
        print("  Flèches:")
        print("    ↓            : Moteur principal (descendre)")
        print("    ←            : Rotation gauche")
        print("    →            : Rotation droite")
        print("\n  Autres:")
        print("    Space        : Moteur principal (alternative)")
        print("    R            : Redémarrer l'épisode")
        print("    Esc          : Quitter")
        print("\n" + "=" * 70)
        print("Clic sur la fenêtre pygame pour que les contrôles fonctionnent!")
        print("=" * 70 + "\n")

        # Premier reset
        self.reset_episode()

        while self.running:
            # Gérer les événements
            if not self.handle_events():
                break

            # Récupérer l'action du clavier
            action = self.get_action_from_keys()

            # Exécuter l'action dans l'environnement
            next_state, reward, terminated, truncated, info = self.env.step(
                action)
            done = terminated or truncated

            self.episode_reward += reward
            self.episode_length += 1

            # Afficher les informations
            if self.episode_length % 20 == 0 or done:
                status = "🔥" if action > 0 else "  "
                print(
                    f"{status} Step {self.episode_length:4d} | Reward: {self.episode_reward:8.2f} | Action: {action}")

            # Si l'épisode est terminé
            if done:
                reason = ""
                # Déterminer la raison de fin
                if terminated:
                    # L'environnement a arrêté (succès ou crash)
                    if self.episode_reward >= 100:  # Seuil de succès: >= 100
                        reason = "✅ SUCCÈS! Lander posé correctement!"
                    else:
                        reason = "💥 CRASH! Le lander s'est écrasé..."
                else:
                    # truncated = True = timeout
                    reason = "⏱️  TIMEOUT! Temps limite dépassé..."

                print(f"\n{reason}")
                print(f"Récompense finale: {self.episode_reward:.2f}")
                print(f"Nombre d'étapes: {self.episode_length}")
                print("Appuie sur R pour recommencer ou Esc pour quitter...\n")

                # Attendre que l'utilisateur redemarre ou quitte
                while not self.restart_needed and self.running:
                    if not self.handle_events():
                        self.running = False
                    pygame.time.delay(50)

                # Si redémarrage demandé
                if self.restart_needed:
                    self.restart_needed = False
                    self.reset_episode()
                    continue

            # Petit délai pour pas surcharger
            pygame.time.delay(50)

        # Fermeture
        self.env.close()
        pygame.quit()
        print("\n👋 Au revoir!")


def main():
    parser = argparse.ArgumentParser(
        description="Contrôle manuel de LunarLander-v3")
    parser.add_argument("--record", action="store_true",
                        help="Enregistrer les vidéos")
    parser.add_argument("--render-mode", default="human",
                        help="Mode de rendu (human par défaut)")
    args = parser.parse_args()

    controller = ManualController(
        render_mode=args.render_mode,
        record_video=args.record
    )
    controller.run()


if __name__ == "__main__":
    main()
