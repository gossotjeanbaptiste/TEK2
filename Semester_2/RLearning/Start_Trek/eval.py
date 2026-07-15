"""
Script d'évaluation pour l'agent Q-Learning sur LunarLander-v3
"""

import gymnasium as gym
import numpy as np
import torch
import argparse
from agents.qlearning_agent import QLearningAgent
import json
from pathlib import Path
from gymnasium.wrappers import RecordVideo


def evaluate_agent(agent, env, num_episodes=100, record_video=False, video_dir="videos"):
    """
    Évalue l'agent sur plusieurs épisodes
    
    Args:
        agent: Agent Q-Learning à évaluer
        env: Environnement Gymnasium
        num_episodes: Nombre d'épisodes d'évaluation
        record_video: Si True, enregistre les vidéos
        video_dir: Répertoire pour les vidéos
        
    Returns:
        Dictionnaire avec les statistiques
    """
    
    episode_rewards = []
    episode_lengths = []
    episode_reasons = []  # Raison de la fin (crash, out_of_bounds, success)
    
    if record_video:
        Path(video_dir).mkdir(parents=True, exist_ok=True)
        # Wrap env to record every episode
        try:
            env = RecordVideo(env, video_dir, episode_trigger=lambda ep: True)
        except Exception:
            # Fallback: try without trigger (older gym versions)
            env = RecordVideo(env, video_dir)
    
    print(f"Évaluation sur {num_episodes} épisodes...")
    
    for episode in range(num_episodes):
        state, info = env.reset()
        episode_reward = 0
        episode_length = 0
        done = False
        
        while not done:
            # Pas d'exploration en évaluation
            action = agent.select_action(state, training=False)
            state, reward, terminated, truncated, info = env.step(action)
            done = terminated or truncated
            episode_reward += reward
            episode_length += 1
        
        episode_rewards.append(episode_reward)
        episode_lengths.append(episode_length)
        
        # Déterminer la raison de la fin
        reason = "unknown"
        if info.get("success", False):
            reason = "success"
        elif info.get("crash", False):
            reason = "crash"
        else:
            reason = "truncated"
        
        episode_reasons.append(reason)
        
        if (episode + 1) % 20 == 0:
            print(f"  Episode {episode+1}/{num_episodes} - Reward: {episode_reward:7.2f}")
    
    # Calculer les statistiques
    rewards_array = np.array(episode_rewards)
    mean_reward = np.mean(rewards_array)
    std_reward = np.std(rewards_array)
    min_reward = np.min(rewards_array)
    max_reward = np.max(rewards_array)
    
    # Intervalle de confiance 95%
    ci_95 = 1.96 * std_reward / np.sqrt(len(episode_rewards))
    
    # Raisons de fin
    success_count = episode_reasons.count("success")
    crash_count = episode_reasons.count("crash")
    truncated_count = episode_reasons.count("truncated")
    
    stats = {
        'num_episodes': num_episodes,
        'mean_reward': float(mean_reward),
        'std_reward': float(std_reward),
        'min_reward': float(min_reward),
        'max_reward': float(max_reward),
        'ci_95': float(ci_95),
        'success_rate': float(success_count / num_episodes),
        'crash_rate': float(crash_count / num_episodes),
        'truncated_rate': float(truncated_count / num_episodes),
        'mean_episode_length': float(np.mean(episode_lengths)),
        'episode_rewards': episode_rewards,
        'episode_lengths': episode_lengths,
        'episode_reasons': episode_reasons,
    }
    
    return stats


def main(model_path, num_episodes=100, seed=42, device="cpu", env_name="LunarLander-v3"):
    """
    Charge un modèle et l'évalue
    
    Args:
        model_path: Chemin vers le modèle sauvegardé
        num_episodes: Nombre d'épisodes d'évaluation
        seed: Graine aléatoire
        device: cpu ou cuda
        env_name: Nom de l'environnement
    """
    
    # Seeder
    np.random.seed(seed)
    torch.manual_seed(seed)
    
    # Créer l'environnement
    print(f"Création de l'environnement: {env_name}")
    env = gym.make(env_name)
    env.reset(seed=seed)
    
    # Créer et charger l'agent
    print(f"Création de l'agent...")
    agent = QLearningAgent(
        state_dim=env.observation_space.shape[0],
        action_dim=env.action_space.n,
        device=device
    )
    
    print(f"Chargement du modèle: {model_path}")
    agent.load(model_path)
    
    # Évaluer
    print("\nDémarrage de l'évaluation...\n")
    stats = evaluate_agent(agent, env, num_episodes=num_episodes)
    
    # Afficher les résultats
    print("\n" + "="*60)
    print("RÉSULTATS D'ÉVALUATION")
    print("="*60)
    print(f"Nombre d'épisodes: {stats['num_episodes']}")
    print(f"\nRécompense:")
    print(f"  - Moyenne: {stats['mean_reward']:.2f}")
    print(f"  - Std Dev: {stats['std_reward']:.2f}")
    print(f"  - IC 95%:  ±{stats['ci_95']:.2f}")
    print(f"  - Min:     {stats['min_reward']:.2f}")
    print(f"  - Max:     {stats['max_reward']:.2f}")
    print(f"\nRésultats:")
    print(f"  - Success: {stats['success_rate']*100:.1f}%")
    print(f"  - Crash:   {stats['crash_rate']*100:.1f}%")
    print(f"  - Timeout: {stats['truncated_rate']*100:.1f}%")
    print(f"\nLongueur moyenne d'épisode: {stats['mean_episode_length']:.1f}")
    print("="*60)
    
    # Sauvegarder les résultats
    output_dir = Path("evaluation_results")
    output_dir.mkdir(parents=True, exist_ok=True)
    
    output_file = output_dir / f"eval_{Path(model_path).stem}.json"
    with open(output_file, 'w') as f:
        # Enlever les listes pour la sérialisation
        stats_save = stats.copy()
        stats_save.pop('episode_rewards')
        stats_save.pop('episode_lengths')
        stats_save.pop('episode_reasons')
        json.dump(stats_save, f, indent=2)
    
    print(f"\nRésultats sauvegardés: {output_file}")
    
    env.close()
    
    return stats


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Évaluer un agent Q-Learning")
    parser.add_argument("--model", type=str, default="models/qlearning_agent_seed42.pt",
                        help="Chemin du modèle à charger")
    parser.add_argument("--episodes", type=int, default=100, help="Nombre d'épisodes d'évaluation")
    parser.add_argument("--seed", type=int, default=42, help="Graine aléatoire")
    parser.add_argument("--device", type=str, default="cpu", help="cpu ou cuda")
    parser.add_argument("--record", action="store_true", help="Enregistrer des vidéos d'évaluation")
    parser.add_argument("--video_dir", type=str, default="videos", help="Répertoire pour les vidéos")
    
    args = parser.parse_args()
    
    if not Path(args.model).exists():
        print(f"Erreur: Le modèle '{args.model}' n'existe pas!")
        print("Veuillez d'abord entraîner le modèle avec: python3 train.py")
        exit(1)
    
    main(
        model_path=args.model,
        num_episodes=args.episodes,
        seed=args.seed,
        device=args.device,
    )

    # Si l'utilisateur a demandé l'enregistrement, relancer l'évaluation avec enregistrement
    if args.record:
        # Recreate env and call evaluate to ensure RecordVideo wrapping
        env = gym.make("LunarLander-v3", render_mode="rgb_array")
        env.reset(seed=args.seed)
        agent = QLearningAgent(
            state_dim=env.observation_space.shape[0],
            action_dim=env.action_space.n,
            device=args.device
        )
        agent.load(args.model)
        print("\nEnregistrement des vidéos d'évaluation...\n")
        _ = evaluate_agent(agent, env, num_episodes=args.episodes, record_video=True, video_dir=args.video_dir)
