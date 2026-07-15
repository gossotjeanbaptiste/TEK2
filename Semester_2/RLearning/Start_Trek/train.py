"""
Script d'entraînement pour l'agent Q-Learning sur LunarLander-v3
"""

import gymnasium as gym
import numpy as np
import torch
import argparse
from agents.qlearning_agent import QLearningAgent
import json
from pathlib import Path
from gymnasium.wrappers import RecordVideo
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


def train_agent(env_name="LunarLander-v3",
                num_episodes=1000,
                learning_rate=1e-3,
                gamma=0.99,
                epsilon_decay=0.995,
                buffer_capacity=10000,
                batch_size=32,
                target_update_freq=1000,
                seed=42,
                device="cpu",
                save_dir="models",
                render_mode=None):
    """
    Entraîne l'agent Q-Learning

    Args:
        env_name: Nom de l'environnement Gymnasium
        num_episodes: Nombre d'épisodes d'entraînement
        learning_rate: Taux d'apprentissage
        gamma: Facteur de discount
        epsilon_decay: Decay factor pour l'exploration
        buffer_capacity: Taille du replay buffer
        batch_size: Taille des batches
        target_update_freq: Fréquence de mise à jour du target network
        seed: Graine aléatoire
        device: cpu ou cuda
        save_dir: Répertoire pour sauvegarder les modèles
        render_mode: "human" pour visualiser, None sinon
    """

    # Créer le répertoire de sauvegarde
    Path(save_dir).mkdir(parents=True, exist_ok=True)

    # Seeder
    np.random.seed(seed)
    torch.manual_seed(seed)

    # Créer l'environnement
    print(f"Création de l'environnement: {env_name}")
    if render_mode:
        env = gym.make(env_name, render_mode=render_mode)
    else:
        env = gym.make(env_name)
    env.reset(seed=seed)

    # Créer l'agent
    print("Création de l'agent Q-Learning...")
    agent = QLearningAgent(
        state_dim=env.observation_space.shape[0],
        action_dim=env.action_space.n,
        learning_rate=learning_rate,
        gamma=gamma,
        epsilon=1.0,
        epsilon_min=0.01,
        epsilon_decay=epsilon_decay,
        buffer_capacity=buffer_capacity,
        batch_size=batch_size,
        target_update_freq=target_update_freq,
        device=device
    )

    # Stocker l'historique
    episode_rewards = []
    episode_lengths = []
    episode_losses = []

    print(f"\nDémarrage de l'entraînement ({num_episodes} épisodes)...")
    print("=" * 60)

    for episode in range(num_episodes):
        # Réinitialiser l'environnement
        state, info = env.reset()
        episode_reward = 0
        episode_length = 0
        episode_loss = []

        done = False
        while not done:
            # Sélectionner une action (epsilon-greedy)
            action = agent.select_action(state, training=True)

            # Effectuer l'action
            next_state, reward, terminated, truncated, info = env.step(action)
            done = terminated or truncated

            # Stocker la transition
            agent.store_transition(state, action, reward, next_state, done)

            # Entraîner l'agent
            loss = agent.train_step()
            if loss is not None:
                episode_loss.append(loss)

            episode_reward += reward
            episode_length += 1
            state = next_state

        # Mettre à jour epsilon
        agent.update_epsilon()

        # Stocker les statistiques
        episode_rewards.append(episode_reward)
        episode_lengths.append(episode_length)
        if episode_loss:
            episode_losses.append(np.mean(episode_loss))

        # Afficher les progressions
        if (episode + 1) % 50 == 0:
            avg_reward = np.mean(episode_rewards[-50:])
            avg_length = np.mean(episode_lengths[-50:])
            avg_loss = np.mean(episode_losses[-50:]) if episode_losses else 0

            print(f"Episode {episode+1}/{num_episodes} | "
                  f"Avg Reward (50): {avg_reward:7.2f} | "
                  f"Avg Length: {avg_length:6.1f} | "
                  f"Epsilon: {agent.epsilon:.4f} | "
                  f"Loss: {avg_loss:.4f}")

        # Évaluer périodiquement
        if (episode + 1) % 200 == 0:
            eval_reward = evaluate_agent(env, agent, num_eval_episodes=10)
            print(f"  → Évaluation (10 épisodes): {eval_reward:.2f}")

    print("=" * 60)
    print("Entraînement terminé!")

    # Sauvegarder le modèle
    model_path = f"{save_dir}/qlearning_agent_seed{seed}.pt"
    agent.save(model_path)

    # Sauvegarder les statistiques
    stats = {
        'episode_rewards': episode_rewards,
        'episode_lengths': episode_lengths,
        'episode_losses': episode_losses,
        'hyperparameters': {
            'learning_rate': learning_rate,
            'gamma': gamma,
            'epsilon_decay': epsilon_decay,
            'buffer_capacity': buffer_capacity,
            'batch_size': batch_size,
            'target_update_freq': target_update_freq,
            'seed': seed,
        }
    }

    stats_path = f"{save_dir}/stats_seed{seed}.json"
    with open(stats_path, 'w') as f:
        json.dump(stats, f, indent=2)
    print(f"Statistiques sauvegardées: {stats_path}")

    # Résumé final
    print("\nRésumé final:")
    print(
        f"  - Récompense moyenne (derniers 100 épisodes): {np.mean(episode_rewards[-100:]):.2f}")
    print(f"  - Récompense max: {np.max(episode_rewards):.2f}")
    print(f"  - Récompense min: {np.min(episode_rewards):.2f}")

    env.close()

    return agent, episode_rewards, episode_lengths


def plot_training_results(episode_rewards, episode_lengths, episode_losses, seed, save_dir="models"):
    """
    Crée et affiche des graphiques de synthèse d'entraînement

    Args:
        episode_rewards: Liste des récompenses par épisode
        episode_lengths: Liste des longueurs par épisode
        episode_losses: Liste des losses par épisode
        seed: Graine utilisée
        save_dir: Répertoire de sauvegarde
    """
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle(
        f'Résumé d\'entraînement - Seed {seed}', fontsize=16, fontweight='bold')

    # ========== Graphique 1: Récompenses par episode ==========
    ax = axes[0, 0]
    ax.plot(episode_rewards, alpha=0.7, linewidth=1,
            label='Récompense par épisode')

    # Moyenne mobile sur 50 épisodes
    if len(episode_rewards) >= 50:
        moving_avg = np.convolve(episode_rewards, np.ones(50)/50, mode='valid')
        ax.plot(range(49, len(episode_rewards)), moving_avg,
                'r-', linewidth=2, label='Moyenne mobile (50)')

    ax.set_xlabel('Épisode')
    ax.set_ylabel('Récompense')
    ax.set_title('Récompenses par épisode')
    ax.legend()
    ax.grid(True, alpha=0.3)

    # ========== Graphique 2: Longueurs par episode ==========
    ax = axes[0, 1]
    ax.plot(episode_lengths, alpha=0.7, linewidth=1, color='orange')
    ax.set_xlabel('Épisode')
    ax.set_ylabel('Longueur')
    ax.set_title('Longueur des épisodes')
    ax.grid(True, alpha=0.3)

    # ========== Graphique 3: Statistiques en barres ==========
    ax = axes[1, 0]

    # Calculer les statistiques
    mean_reward_all = np.mean(episode_rewards)
    mean_reward_last_100 = np.mean(
        episode_rewards[-100:]) if len(episode_rewards) >= 100 else mean_reward_all
    max_reward = np.max(episode_rewards)
    min_reward = np.min(episode_rewards)

    stats_labels = ['Moyenne\ntotale', 'Moyenne\nlast 100', 'Max', 'Min']
    stats_values = [mean_reward_all,
                    mean_reward_last_100, max_reward, min_reward]
    colors = ['blue', 'green', 'orange', 'red']

    bars = ax.bar(stats_labels, stats_values, color=colors,
                  alpha=0.7, edgecolor='black', linewidth=1.5)

    # Ajouter les valeurs sur les barres
    for bar, val in zip(bars, stats_values):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height,
                f'{val:.2f}',
                ha='center', va='bottom' if height > 0 else 'top', fontweight='bold')

    ax.set_ylabel('Récompense')
    ax.set_title('Statistiques de récompense')
    ax.grid(True, alpha=0.3, axis='y')
    ax.axhline(y=0, color='black', linestyle='-', linewidth=0.5)
    ax.axhline(y=100, color='green', linestyle='--', linewidth=1,
               alpha=0.5, label='Seuil de succès (100)')
    ax.legend()

    # ========== Graphique 4: Résumé texte ==========
    ax = axes[1, 1]
    ax.axis('off')

    # Calculer d'autres stats utiles
    num_success = sum(1 for r in episode_rewards if r >= 100)
    success_rate = (num_success / len(episode_rewards)) * 100

    # Texte de résumé
    summary_text = f"""
    📊 RÉSUMÉ D'ENTRAÎNEMENT
    
    📈 Statistiques Générales:
    • Total d'épisodes: {len(episode_rewards)}
    • Récompense moyenne: {mean_reward_all:.2f}
    • Récompense (derniers 100): {mean_reward_last_100:.2f}
    • Max récompense: {max_reward:.2f}
    • Min récompense: {min_reward:.2f}
    
    ✅ Succès (score ≥ 100):
    • Nombre de succès: {num_success}
    • Taux de succès: {success_rate:.1f}%
    
    ⏱️ Longueurs d'épisodes:
    • Moyenne: {np.mean(episode_lengths):.1f}
    • Max: {np.max(episode_lengths)}
    • Min: {np.min(episode_lengths)}
    """

    ax.text(0.05, 0.95, summary_text, transform=ax.transAxes,
            fontsize=10, verticalalignment='top', fontfamily='monospace',
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.3))

    plt.tight_layout()

    # Sauvegarder la figure
    figure_path = f"{save_dir}/training_results_seed{seed}.png"
    plt.savefig(figure_path, dpi=150, bbox_inches='tight')
    print(f"\n📊 Graphique sauvegardé: {figure_path}")

    # Afficher
    plt.show()


def evaluate_agent(env, agent, num_eval_episodes=10):
    """
    Évalue l'agent en mode exploitation (pas d'exploration)

    Args:
        env: Environnement Gymnasium
        agent: Agent à évaluer
        num_eval_episodes: Nombre d'épisodes d'évaluation

    Returns:
        Récompense moyenne
    """
    eval_rewards = []

    for _ in range(num_eval_episodes):
        state, _ = env.reset()
        episode_reward = 0
        done = False

        while not done:
            action = agent.select_action(state, training=False)
            state, reward, terminated, truncated, _ = env.step(action)
            done = terminated or truncated
            episode_reward += reward

        eval_rewards.append(episode_reward)

    return np.mean(eval_rewards)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Entraîner un agent Q-Learning pour LunarLander")
    parser.add_argument("--episodes", type=int,
                        default=1000, help="Nombre d'épisodes")
    parser.add_argument("--lr", type=float, default=1e-3,
                        help="Taux d'apprentissage")
    parser.add_argument("--gamma", type=float, default=0.99,
                        help="Facteur de discount")
    parser.add_argument("--epsilon_decay", type=float,
                        default=0.995, help="Decay d'epsilon")
    parser.add_argument("--buffer_size", type=int,
                        default=10000, help="Taille du replay buffer")
    parser.add_argument("--batch_size", type=int,
                        default=32, help="Taille des batches")
    parser.add_argument("--seed", type=int, default=42,
                        help="Graine aléatoire")
    parser.add_argument("--device", type=str,
                        default="cpu", help="cpu ou cuda")
    parser.add_argument("--save_dir", type=str,
                        default="models", help="Répertoire de sauvegarde")
    parser.add_argument(
        "--env", type=str, default="LunarLander-v3", help="Nom de l'environnement")
    parser.add_argument("--record", action="store_true",
                        help="Enregistrer des vidéos de démonstration après entraînement")
    parser.add_argument("--video_dir", type=str,
                        default="videos", help="Répertoire pour les vidéos")
    parser.add_argument("--human", action="store_true",
                        help="Visualiser l'entraînement en temps réel (pygame)")

    args = parser.parse_args()

    agent, episode_rewards, episode_lengths = train_agent(
        env_name=args.env,
        num_episodes=args.episodes,
        learning_rate=args.lr,
        gamma=args.gamma,
        epsilon_decay=args.epsilon_decay,
        buffer_capacity=args.buffer_size,
        batch_size=args.batch_size,
        seed=args.seed,
        device=args.device,
        save_dir=args.save_dir,
        render_mode="human" if args.human else None
    )

    # Générer les graphiques de résumé
    print("\n📊 Génération des graphiques de résumé...")
    # Créer une liste vide pour episode_losses (on n'a pas accès dedans)
    plot_training_results(episode_rewards, episode_lengths,
                          [], args.seed, args.save_dir)

    if args.record:
        # Enregistrer quelques épisodes en greedy pour inspection
        Path(args.video_dir).mkdir(parents=True, exist_ok=True)
        try:
            env = gym.make(args.env, render_mode="rgb_array")
        except TypeError:
            env = gym.make(args.env)
        env.reset(seed=args.seed)
        try:
            env = RecordVideo(env, args.video_dir,
                              episode_trigger=lambda ep: True)
        except Exception:
            env = RecordVideo(env, args.video_dir)

        print("\nEnregistrement de 3 épisodes de démonstration...\n")
        for ep in range(3):
            state, info = env.reset()
            done = False
            while not done:
                action = agent.select_action(state, training=False)
                state, reward, terminated, truncated, info = env.step(action)
                done = terminated or truncated
        env.close()
