# TEK 2

## Epitech

### Version Française

Dans ce repo github vous trouverez tous mes projets de Tek 2 à Epitech

#### Prérequis

Récapitulatif des outils et librairies à installer pour compiler/lancer l'ensemble des projets (noms de paquets pour Debian/Ubuntu, à adapter selon votre distribution) :

- **Compilateurs** : `clang` / `clang++` (la plupart des projets C/C++), `ghc` (Haskell, Piscine Rush2)
- **Librairies C/C++** :
  ```bash
  sudo apt install libncurses-dev libsfml-dev libsdl2-dev libsdl2-ttf-dev libcriterion-dev libgl-dev libx11-dev
  ```
  - `libncurses-dev` : Rush4, Arcade (affichage terminal)
  - `libsfml-dev` : Rush4 (affichage graphique)
  - `libsdl2-dev` + `libsdl2-ttf-dev` : Arcade (lib graphique SDL)
  - `libcriterion-dev` : NanoTekSpice, Raytracer (cible `unit_tests`)
  - `libgl-dev` + `libx11-dev` : Zappy (GUI raylib)
  - `raylib` : Arcade, Zappy (GUI) — non disponible dans les dépôts Ubuntu, à compiler depuis les sources ([raysan5/raylib](https://github.com/raysan5/raylib))
- **Python** (`pip install -r requirements.txt` dans chaque dossier) : 
  - [MyCMS](Semester_1/MyCMS/cms/requirements.txt)
  - [NotLinkedin](Semester_1/NotLinkedin/requirements.txt)
  - [Start Trek](Semester_2/RLearning/Start_Trek/requirements.txt)
  - [Racing Simulator](Semester_2/Robocar/RacingSimulator/RacingSimulator/requirements.txt)
  - [VESC Controller](Semester_2/Robocar/VESC_Controller/VESC_Controller/requirements.txt)
  - [Mask Generator](Semester_2/Robocar/MaskGenerator/MaskGenerator/ai/requirements.txt)
- **Node.js / npm** 
  - (`npm install` dans chaque dossier) : [Project Week](Semester_1/Project_Week) (front + back)
  - [Codewash](Semester_2/Legacy-Code/Codewash)
  - [Octopus Bootstrap app](Semester_2/DevOps/Octopus_Boostrap/B-DOP-400_bootstrap_octopus_applications)
- **Bases de données & Infra** : 
  - MySQL/MariaDB (NotLinkedin, Project Week back)
  - Ansible (DevOps/Octopus)
  - Docker (DevOps/Marvin)

- [Semestre 1](Semester_1) :
  - [MyCMS](Semester_1/MyCMS)
  - [NotLinkedin](Semester_1/NotLinkedin)
  - [Piscine C++](Semester_1/Piscine)
    - [Day01 - string](Semester_1/Piscine/Day01/string)
    - [Day01 - zorglub](Semester_1/Piscine/Day01/zorglub)
    - [Day02](Semester_1/Piscine/Day02)
    - [Day03](Semester_1/Piscine/Day03)
    - [Day04 - ex00](Semester_1/Piscine/Day04/ex00)
    - [Day04 - ex01](Semester_1/Piscine/Day04/ex01)
    - [Day04 - ex02](Semester_1/Piscine/Day04/ex02)
    - [Day05](Semester_1/Piscine/Day05)
    - [Day06](Semester_1/Piscine/Day06)
    - [Day07](Semester_1/Piscine/Day07)
    - [Day08](Semester_1/Piscine/Day08)
    - [Day09](Semester_1/Piscine/Day09)
    - [Day10](Semester_1/Piscine/Day10)
    - [Day11](Semester_1/Piscine/Day11)
    - [Day12](Semester_1/Piscine/Day12)
    - [Day13](Semester_1/Piscine/Day13)
    - [Rush1](Semester_1/Piscine/Rush1)
    - [Rush2](Semester_1/Piscine/Rush2)
    - [Rush3](Semester_1/Piscine/Rush3)
    - [Rush4](Semester_1/Piscine/Rush4)
  - [Project Week](Semester_1/Project_Week)
  - [Rout-Art](Semester_1/Rout-Art)
  - [Stage](Semester_1/Stage)
- [Semestre 2](Semester_2) :
  - [Communication Professionnelle](Semester_2/Com_Pro)
    - [Back in line](Semester_2/Com_Pro/Back_in_line)
    - [Report](Semester_2/Com_Pro/Report)
    - [Slideshow](Semester_2/Com_Pro/Slideshow)
  - [Concurrent Programming](Semester_2/Concurrent-Prog)
    - [Panoramix](Semester_2/Concurrent-Prog/Panoramix)
    - [Plazza](Semester_2/Concurrent-Prog/Plazza)
  - [DevOps](Semester_2/DevOps)
    - [Marvin](Semester_2/DevOps/Marvin)
    - [Octopus](Semester_2/DevOps/Octopus)
    - [Octopus Bootstrap](Semester_2/DevOps/Octopus_Boostrap)
  - [Legacy Code](Semester_2/Legacy-Code)
    - [Codewash](Semester_2/Legacy-Code/Codewash)
  - [Oriented Object Programming](Semester_2/Oriented-Object-Programming)
    - [Arcade](Semester_2/Oriented-Object-Programming/Arcade)
    - [NanoTekSpice](Semester_2/Oriented-Object-Programming/NanoTekSpice)
    - [Raytracer](Semester_2/Oriented-Object-Programming/Raytracer)
  - [Prompt Engineering](Semester_2/Prompt-Ing)
    - [Codename](Semester_2/Prompt-Ing/Codename)
    - [Persona](Semester_2/Prompt-Ing/Persona)
  - [Reinforcement Learning](Semester_2/RLearning)
    - [Start Trek](Semester_2/RLearning/Start_Trek)
  - [Robocar](Semester_2/Robocar)
    - [Mask Generator](Semester_2/Robocar/MaskGenerator)
    - [Racing Simulator](Semester_2/Robocar/RacingSimulator)
    - [VESC Controller](Semester_2/Robocar/VESC_Controller)
  - [Zappy](Semester_2/Zappy)

---

### English Version

In this github repo you will find all my projects from Tek 2 at Epitech (except for projects requiring a Virtual Machine)

#### Prerequisites

Summary of the tools and libraries needed to build/run all the projects (package names for Debian/Ubuntu, adapt for your distribution) :

- **Compilers**: `clang` / `clang++` (most C/C++ projects), `ghc` (Haskell, Piscine Rush2)
- **C/C++ libraries**:
  ```bash
  sudo apt install libncurses-dev libsfml-dev libsdl2-dev libsdl2-ttf-dev libcriterion-dev libgl-dev libx11-dev
  ```
  - `libncurses-dev`: Rush4, Arcade (terminal display)
  - `libsfml-dev`: Rush4 (graphical display)
  - `libsdl2-dev` + `libsdl2-ttf-dev`: Arcade (SDL graphics lib)
  - `libcriterion-dev`: NanoTekSpice, Raytracer (`unit_tests` target)
  - `libgl-dev` + `libx11-dev`: Zappy (raylib GUI)
  - `raylib`: Arcade, Zappy (GUI) — not available in Ubuntu repos, build from source ([raysan5/raylib](https://github.com/raysan5/raylib))
- **Python** (`pip install -r requirements.txt` in each folder): 
  - [MyCMS](Semester_1/MyCMS/cms/requirements.txt)
  - [NotLinkedin](Semester_1/NotLinkedin/requirements.txt)
  - [Start Trek](Semester_2/RLearning/Start_Trek/requirements.txt)
  - [Racing Simulator](Semester_2/Robocar/RacingSimulator/RacingSimulator/requirements.txt)
  - [VESC Controller](Semester_2/Robocar/VESC_Controller/VESC_Controller/requirements.txt)
  - [Mask Generator](Semester_2/Robocar/MaskGenerator/MaskGenerator/ai/requirements.txt)
- **Node.js / npm** (`npm install` in each folder):
  - [Project Week](Semester_1/Project_Week) (front + back)
  - [Codewash](Semester_2/Legacy-Code/Codewash)
  - [Octopus Bootstrap app](Semester_2/DevOps/Octopus_Boostrap/B-DOP-400_bootstrap_octopus_applications)
- **Databases & Infra**:
  - MySQL/MariaDB (NotLinkedin, Project Week back)
  - Ansible (DevOps/Octopus)
  - Docker (DevOps/Marvin)

- [Semester 1](Semester_1) :
  - [MyCMS](Semester_1/MyCMS)
  - [NotLinkedin](Semester_1/NotLinkedin)
  - [C++ Piscine](Semester_1/Piscine)
    - [Day01 - string](Semester_1/Piscine/Day01/string)
    - [Day01 - zorglub](Semester_1/Piscine/Day01/zorglub)
    - [Day02](Semester_1/Piscine/Day02)
    - [Day03](Semester_1/Piscine/Day03)
    - [Day04 - ex00](Semester_1/Piscine/Day04/ex00)
    - [Day04 - ex01](Semester_1/Piscine/Day04/ex01)
    - [Day04 - ex02](Semester_1/Piscine/Day04/ex02)
    - [Day05](Semester_1/Piscine/Day05)
    - [Day06](Semester_1/Piscine/Day06)
    - [Day07](Semester_1/Piscine/Day07)
    - [Day08](Semester_1/Piscine/Day08)
    - [Day09](Semester_1/Piscine/Day09)
    - [Day10](Semester_1/Piscine/Day10)
    - [Day11](Semester_1/Piscine/Day11)
    - [Day12](Semester_1/Piscine/Day12)
    - [Day13](Semester_1/Piscine/Day13)
    - [Rush1](Semester_1/Piscine/Rush1)
    - [Rush2](Semester_1/Piscine/Rush2)
    - [Rush3](Semester_1/Piscine/Rush3)
    - [Rush4](Semester_1/Piscine/Rush4)
  - [Project Week](Semester_1/Project_Week)
  - [Rout-Art](Semester_1/Rout-Art)
  - [Internship (Stage)](Semester_1/Stage)
- [Semester 2](Semester_2) :
  - [Professional Communication](Semester_2/Com_Pro)
    - [Back in line](Semester_2/Com_Pro/Back_in_line)
    - [Report](Semester_2/Com_Pro/Report)
    - [Slideshow](Semester_2/Com_Pro/Slideshow)
  - [Concurrent Programming](Semester_2/Concurrent-Prog)
    - [Panoramix](Semester_2/Concurrent-Prog/Panoramix)
    - [Plazza](Semester_2/Concurrent-Prog/Plazza)
  - [DevOps](Semester_2/DevOps)
    - [Marvin](Semester_2/DevOps/Marvin)
    - [Octopus](Semester_2/DevOps/Octopus)
    - [Octopus Bootstrap](Semester_2/DevOps/Octopus_Boostrap)
  - [Legacy Code](Semester_2/Legacy-Code)
    - [Codewash](Semester_2/Legacy-Code/Codewash)
  - [Oriented Object Programming](Semester_2/Oriented-Object-Programming)
    - [Arcade](Semester_2/Oriented-Object-Programming/Arcade)
    - [NanoTekSpice](Semester_2/Oriented-Object-Programming/NanoTekSpice)
    - [Raytracer](Semester_2/Oriented-Object-Programming/Raytracer)
  - [Prompt Engineering](Semester_2/Prompt-Ing)
    - [Codename](Semester_2/Prompt-Ing/Codename)
    - [Persona](Semester_2/Prompt-Ing/Persona)
  - [Reinforcement Learning](Semester_2/RLearning)
    - [Start Trek](Semester_2/RLearning/Start_Trek)
  - [Robocar](Semester_2/Robocar)
    - [Mask Generator](Semester_2/Robocar/MaskGenerator)
    - [Racing Simulator](Semester_2/Robocar/RacingSimulator)
    - [VESC Controller](Semester_2/Robocar/VESC_Controller)
  - [Zappy](Semester_2/Zappy)
