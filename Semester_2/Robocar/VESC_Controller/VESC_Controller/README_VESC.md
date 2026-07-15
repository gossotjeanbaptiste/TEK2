# Contrôle du VESC avec Python

Ce dépôt contient un script simple pour piloter un contrôleur VESC depuis Python via le protocole officiel VESC.

Le script est disponible ici: [vesc_controller.py](vesc_controller.py)

## Prérequis

- Python 3.12 ou compatible
- Un VESC accessible en USB ou UART
- Les droits d'accès au port série sur Linux

Sur Linux, le port est souvent `/dev/ttyACM0` ou `/dev/ttyUSB0`.

## Installation

Installe les dépendances Python:

```bash
pip install git+https://github.com/LiamBindle/PyVESC.git
pip install pyserial
```

Si le port série est refusé sur Linux, ajoute ton utilisateur au groupe `dialout`:

```bash
sudo usermod -aG dialout $USER
```

Puis déconnecte-toi et reconnecte-toi.

## Utilisation

Le script propose plusieurs actions via la ligne de commande.

### Vérifier la connexion

```bash
python3 vesc_controller.py --port /dev/ttyACM0 --firmware
```

### Appliquer un duty cycle moteur

```bash
python3 vesc_controller.py --port /dev/ttyACM0 --duty 0.1
```

Pour envoyer la commande pendant une durée limitée puis arrêter automatiquement:

```bash
python3 vesc_controller.py --port /dev/ttyACM0 --duty 0.1 --duration 2
```

### Commander le servo

```bash
python3 vesc_controller.py --port /dev/ttyACM0 --servo 0.0
python3 vesc_controller.py --port /dev/ttyACM0 --servo 0.5
python3 vesc_controller.py --port /dev/ttyACM0 --servo 1.0
```

### Arrêt complet

```bash
python3 vesc_controller.py --port /dev/ttyACM0 --stop
```

## Options disponibles

- `--port`: port série du VESC, par défaut `/dev/ttyACM0`
- `--baudrate`: vitesse série, par défaut `115200`
- `--timeout`: délai de lecture série, par défaut `0.1`
- `--firmware`: affiche la version du firmware
- `--duty`: définit le duty cycle moteur
- `--servo`: définit la position servo entre `0.0` et `1.0`
- `--duration`: durée d'exécution avant arrêt automatique
- `--stop`: envoie un arrêt propre

## Sécurité

- Lève toujours le véhicule avant d'envoyer une commande moteur.
- Vérifie que la roue ou la transmission ne peut pas toucher d'obstacle.
- Garde une main près de l'arrêt d'urgence pendant les premiers tests.
- En cas de doute, commence avec une valeur de duty cycle très faible.

## Dépannage

- Si le script affiche une erreur de port série, vérifie le nom du périphérique.
- Si `pyvesc` ou `pyserial` manque, réinstalle les dépendances.
- Si le servo ne réagit pas, vérifie que l'option d'activation du servo est bien configurée dans VESC Tool.

## Référence

Documentation du projet PyVESC: https://github.com/LiamBindle/PyVESC