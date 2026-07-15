"""
Deep Q-Network (DQN) Agent for LunarLander-v3
Q-Learning basé sur un réseau de neurones MLP
"""

import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from collections import deque
import random


class QNetwork(nn.Module):
    """
    Multi-layer perceptron pour approximer la fonction Q-value
    Architecture: Input -> ReLU -> ReLU -> Output
    """
    def __init__(self, state_dim, action_dim, hidden_dim=128):
        super(QNetwork, self).__init__()
        self.fc1 = nn.Linear(state_dim, hidden_dim)
        self.fc2 = nn.Linear(hidden_dim, hidden_dim)
        self.fc3 = nn.Linear(hidden_dim, action_dim)
        self.relu = nn.ReLU()
        
    def forward(self, state):
        """Forward pass: state -> Q-values for each action"""
        x = self.relu(self.fc1(state))
        x = self.relu(self.fc2(x))
        q_values = self.fc3(x)
        return q_values


class ReplayBuffer:
    """
    Experience replay buffer pour stocker et sampler les transitions
    """
    def __init__(self, capacity=10000):
        self.buffer = deque(maxlen=capacity)
    
    def push(self, state, action, reward, next_state, done):
        """Ajoute une transition au buffer"""
        self.buffer.append((state, action, reward, next_state, done))
    
    def sample(self, batch_size):
        """Sample un batch aléatoire du buffer"""
        transitions = random.sample(self.buffer, batch_size)
        states, actions, rewards, next_states, dones = zip(*transitions)
        
        states = torch.FloatTensor(np.array(states))
        actions = torch.LongTensor(np.array(actions))
        rewards = torch.FloatTensor(np.array(rewards))
        next_states = torch.FloatTensor(np.array(next_states))
        dones = torch.FloatTensor(np.array(dones))
        
        return states, actions, rewards, next_states, dones
    
    def __len__(self):
        return len(self.buffer)


class QLearningAgent:
    """
    Agent Q-Learning avec réseau de neurones et target network
    Implémente DQN classique avec:
    - Experience replay
    - Target network (mises à jour dures)
    - Epsilon-greedy exploration
    """
    
    def __init__(self, 
                 state_dim=8,
                 action_dim=4,
                 learning_rate=1e-3,
                 gamma=0.99,
                 epsilon=1.0,
                 epsilon_min=0.01,
                 epsilon_decay=0.995,
                 buffer_capacity=10000,
                 batch_size=32,
                 target_update_freq=1000,
                 device="cpu"):
        """
        Args:
            state_dim: Dimension de l'espace d'état (8 pour LunarLander)
            action_dim: Dimension de l'espace d'action (4 pour LunarLander)
            learning_rate: Taux d'apprentissage
            gamma: Facteur de discount
            epsilon: Probabilité d'exploration initiale
            epsilon_min: Probabilité minimale d'exploration
            epsilon_decay: Decay factor pour epsilon
            buffer_capacity: Taille du replay buffer
            batch_size: Taille du batch pour l'entraînement
            target_update_freq: Fréquence de mise à jour du target network
            device: cpu ou cuda
        """
        self.state_dim = state_dim
        self.action_dim = action_dim
        self.learning_rate = learning_rate
        self.gamma = gamma
        self.epsilon = epsilon
        self.epsilon_min = epsilon_min
        self.epsilon_decay = epsilon_decay
        self.batch_size = batch_size
        self.target_update_freq = target_update_freq
        self.device = device
        
        # Réseaux: Q-network et Target Q-network
        self.q_network = QNetwork(state_dim, action_dim, hidden_dim=128).to(device)
        self.target_network = QNetwork(state_dim, action_dim, hidden_dim=128).to(device)
        self.target_network.load_state_dict(self.q_network.state_dict())
        self.target_network.eval()  # Target network en mode évaluation
        
        # Optimiseur
        self.optimizer = optim.Adam(self.q_network.parameters(), lr=learning_rate)
        
        # Replay buffer
        self.replay_buffer = ReplayBuffer(capacity=buffer_capacity)
        
        # Compteurs
        self.total_steps = 0
        self.episode_count = 0
        self.training_losses = []
        
    def select_action(self, state, training=True):
        """
        Sélectionne une action avec epsilon-greedy
        
        Args:
            state: État actuel
            training: Si True, utilise epsilon-greedy. Si False, greedy uniquement
            
        Returns:
            Action (entier)
        """
        if training and np.random.random() < self.epsilon:
            # Exploration: action aléatoire
            return np.random.randint(0, self.action_dim)
        else:
            # Exploitation: meilleure action connue
            with torch.no_grad():
                state_tensor = torch.FloatTensor(state).unsqueeze(0).to(self.device)
                q_values = self.q_network(state_tensor)
                return q_values.argmax(dim=1).item()
    
    def store_transition(self, state, action, reward, next_state, done):
        """Stocke une transition dans le replay buffer"""
        self.replay_buffer.push(state, action, reward, next_state, done)
        self.total_steps += 1
    
    def train_step(self):
        """
        Effectue une étape d'entraînement (mise à jour du Q-network)
        """
        if len(self.replay_buffer) < self.batch_size:
            return None
        
        # Sample un batch du replay buffer
        states, actions, rewards, next_states, dones = self.replay_buffer.sample(self.batch_size)
        
        states = states.to(self.device)
        actions = actions.to(self.device)
        rewards = rewards.to(self.device)
        next_states = next_states.to(self.device)
        dones = dones.to(self.device)
        
        # Calcul des Q-values cibles
        with torch.no_grad():
            # Utilise le target network pour stabilité
            next_q_values = self.target_network(next_states).max(dim=1)[0]
            target_q_values = rewards + (1 - dones) * self.gamma * next_q_values
        
        # Calcul des Q-values prédites
        q_values = self.q_network(states)
        predicted_q_values = q_values.gather(1, actions.unsqueeze(1)).squeeze(1)
        
        # Perte (MSE)
        loss = nn.MSELoss()(predicted_q_values, target_q_values)
        
        # Rétropropagation
        self.optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(self.q_network.parameters(), 1.0)
        self.optimizer.step()
        
        self.training_losses.append(loss.item())
        
        # Mise à jour du target network (tous les target_update_freq steps)
        if self.total_steps % self.target_update_freq == 0:
            self.update_target_network()
        
        return loss.item()
    
    def update_target_network(self):
        """Copie les poids du Q-network vers le target network"""
        self.target_network.load_state_dict(self.q_network.state_dict())
    
    def update_epsilon(self):
        """Décroit epsilon pour réduire progressivement l'exploration"""
        self.epsilon = max(self.epsilon_min, self.epsilon * self.epsilon_decay)
        self.episode_count += 1
    
    def save(self, filepath):
        """Sauvegarde l'agent"""
        checkpoint = {
            'q_network': self.q_network.state_dict(),
            'target_network': self.target_network.state_dict(),
            'optimizer': self.optimizer.state_dict(),
            'epsilon': self.epsilon,
            'total_steps': self.total_steps,
            'episode_count': self.episode_count,
        }
        torch.save(checkpoint, filepath)
        print(f"Agent sauvegardé: {filepath}")
    
    def load(self, filepath):
        """Charge un agent sauvegardé"""
        checkpoint = torch.load(filepath, map_location=self.device)
        self.q_network.load_state_dict(checkpoint['q_network'])
        self.target_network.load_state_dict(checkpoint['target_network'])
        self.optimizer.load_state_dict(checkpoint['optimizer'])
        self.epsilon = checkpoint['epsilon']
        self.total_steps = checkpoint['total_steps']
        self.episode_count = checkpoint['episode_count']
        print(f"Agent chargé: {filepath}")
    
    def get_stats(self):
        """Retourne les statistiques d'entraînement"""
        return {
            'epsilon': self.epsilon,
            'total_steps': self.total_steps,
            'episode_count': self.episode_count,
            'buffer_size': len(self.replay_buffer),
            'avg_loss': np.mean(self.training_losses[-100:]) if self.training_losses else 0.0,
        }
