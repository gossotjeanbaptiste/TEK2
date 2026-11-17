# Scripts de démo

Lanceurs prêts à l'emploi pour la soutenance, un recruteur ou une vidéo
(Linux / WSL). Chaque script compile ce qui manque, démarre le serveur, le
GUI et les clients, puis nettoie tous les processus (drones forkés compris)
à la fermeture de la fenêtre ou sur Ctrl+C.

```bash
./scripts/demo/demo.sh            # menu interactif (demande aussi la vitesse)
./scripts/demo/demo.sh 6          # tutoriel guidé
./scripts/demo/demo.sh 7 medium    # vidéo, sans question
```

| # | Script | Ce qu'on montre | Carte | f slow / medium / fast |
|---|--------|-----------------|-------|-------------------------|
| 1 | `01_quick.sh` | une colonie IA atteint le niveau 8 | 10x10 | 25 / 40 / 100 |
| 2 | `02_duel.sh` | deux équipes IA en compétition | 20x20 | 25 / 40 / 100 |
| 3 | `03_battle.sh` | quatre équipes, stress test serveur + GUI | 42x42 | 25 / 50 / 100 |
| 4 | `04_showcase.sh` | chaque commande jouée et commentée (déplacements, Take/Set, Broadcast, Fork + éclosion, Eject, Incantation) | 8x8 | 10 / 15 / 100 |
| 5 | `05_manual.sh` | piloter un joueur au clavier (questions du jury) | 10x10 | 10 / 10 / 100 |
| 6 | `06_tutorial.sh` | **tutoriel** : bulles qui présentent la fenêtre, puis les moments clés | 10x10 | 25 / 30 / 100 |
| 7 | `07_video.sh` | **vidéo** : caméra automatique, titres, enregistrement MP4 | 12x12 | 25 / 40 / 100 |

## Les explications sont dans la fenêtre

Les démos lancent le GUI en mode `demo` : un panneau à droite de la fenêtre
(touche **H** pour le masquer) explique ce que font les joueurs :
- le but du jeu et la stratégie de l'IA (reine, œufs, drones, rituels) ;
- par équipe : joueurs, œufs, et un histogramme des niveaux 1 → 8 ;
- des compteurs : objets ramassés, messages radio, rituels, morts… ;
- un journal en français : « « alpha » lance un rituel niveau 3 → 4 en
  (5, 8) », « rituel réussi », « un œuf éclot : un joueur « alpha » naît »… ;
- une bannière de victoire à la fin (la fenêtre reste ouverte : le serveur,
  lui, s'arrête).

**En anglais** : `GUI_LANG=en` (ou la touche **L** pendant la partie) traduit
le panneau, le tutoriel, les panneaux Carte/Équipes, les textes de la carte
et le mode vidéo. Les messages du terminal restent en français.

Sans GUI (`NO_GUI=1`), le même tableau de bord s'affiche dans le terminal
(`observer.py`).

## Tutoriel (démo 6)

Une visite guidée en 13 étapes, en bulles :
1. présentation de Zappy, du monde, de la caméra (« essayez ! ») ;
2. les panneaux : chacun est mis en surbrillance, le reste de l'écran
   s'assombrit ;
3. suivre un joueur (ouvrir le menu avec M, cliquer un joueur) ;
4. puis le tutoriel **attend les vrais moments de la partie** : un joueur,
   un ramassage, une ponte, un rituel, une montée de niveau. La caméra va
   cadrer l'endroit et une flèche le désigne.

Entrée / Espace : suivant · Retour arrière : précédent · Échap : quitter le
tutoriel. `TUTORIAL_AUTO=6` le fait défiler seul (écran de salon, stand).

## Vidéo (démo 7)

Le GUI se met en mode « réalisateur » :
- carton titre (Zappy, technologies, équipes en présence) ;
- vue d'ensemble en lente rotation, coupes en gros plan sur les rituels,
  plans rapprochés sur un joueur ;
- sous-titres en bas d'écran (« Rituel d'élévation · « rouge » · niveau
  2 → 3 », « Nouveau record »…) et mini tableau des niveaux ;
- carton de victoire, puis la fenêtre se ferme seule (`END_HOLD`, 10 s).

**Enregistrement automatique** en MP4 (H.264, 30 i/s) dans `videos/` si
ffmpeg est disponible. Sans root : `./scripts/demo/get_ffmpeg.sh` télécharge
un ffmpeg autonome dans `.tools/`. Sinon, enregistrez la fenêtre vous-même
(Windows : Win+Alt+R, ou OBS). Pour une vidéo en 1080p :
`GUI_SIZE=1920x1080 ./scripts/demo/demo.sh 7`.

**Vidéo en anglais** (titres, sous-titres, textes de la carte, équipes
« red » et « blue ») : `GUI_LANG=en ./scripts/demo/demo.sh 7`.

En 1080p, la lecture de l'écran ralentit le rendu sous WSL (~10 images
uniques/s). Pour une vidéo fluide, enregistrer en 720p (~19 images/s) puis
agrandir : `ffmpeg -i in.mp4 -vf scale=1920:1080:flags=lanczos -c:v libx264
-preset slow -crf 20 -pix_fmt yuv420p -movflags +faststart out.mp4`.

## Captures d'écran

Dans tous les modes, **F12** enregistre une capture (dossier `SHOTS_DIR`,
par défaut `/tmp/zappy_demo/screenshots`). `SHOT_EVERY=5` en prend une
toutes les 5 s.

## Vitesses

Chaque démo demande une vitesse au lancement (ou `SPEED=slow|medium|fast`) :

| Vitesse | Pour qui | Démo 1 (10x10) |
|---------|----------|----------------|
| **slow** | un recruteur, un public : on voit marcher chaque joueur (~0,3 s par pas) | pas de victoire rapide : niveaux 1 à 4 en ~15 min (f=25) |
| **medium** | une présentation de quelques minutes, partie complète | ~2 min 20 (f=40) |
| **fast** | vérifier en une minute que tout fonctionne | ~50 s (f=100) |

En `slow`, le serveur applique les **durées du sujet** (Forward, Look, Take…
= 7/f, Fork = 42/f, Inventory = 1/f) au lieu d'une unité de temps pour toutes
les commandes (variable `ZAPPY_SUBJECT_TIMES=1`, forcée par `SUBJECT_TIMES=0/1`).
Les joueurs marchent au lieu de bondir, mais une partie complète demande
alors 30 à 45 minutes : on regarde le temps qu'on veut, puis on ferme.

Le tutoriel est en `slow` par défaut, la vidéo en `medium`. Sans terminal
interactif, c'est `fast`.
Les anciens noms français (`lent`, `moyen`, `rapide`) restent acceptés. Les délais internes de l'IA sont réglés pour
f=100 ; les scripts lui passent la fréquence (`ZAPPY_FREQ`) pour qu'elle
reste fonctionnelle à basse vitesse.

## Variables d'environnement

| Variable | Défaut | Effet |
|----------|--------|-------|
| `PORT` | 4242 | port du serveur |
| `SPEED` | demandé | `slow`, `medium` ou `fast` |
| `GUI_MODE` | `demo` | `demo`, `tutorial`, `video` ou `normal` (GUI d'origine) |
| `GUI_SIZE` | 1280x720 | taille de la fenêtre |
| `SHOT_EVERY` / `SHOTS_DIR` | 0 / logs | captures automatiques toutes les n secondes |
| `RECORD` | auto (démo 7) | fichier MP4 ; `0` = ne pas enregistrer |
| `FFMPEG` | auto | chemin de ffmpeg |
| `END_HOLD` | 10 | secondes de carton de fin avant fermeture (vidéo) |
| `TUTORIAL_AUTO` | 0 | le tutoriel avance seul toutes les n secondes |
| `GUI_LANG` | fr | `en` = GUI en anglais, dans tous les modes (touche **L** pour basculer) |
| `DEMO_PAUSE` | 0 | `1` = attend Entrée entre les étapes (pratique pour parler) |
| `DEMO_BUILD` | 0 | `1` = force un `make` avant de lancer |
| `NO_GUI` | 0 | `1` = pas de GUI (tableau de bord dans le terminal) |
| `GUI_DELAY` | 1.5 | délai (s) laissé au GUI pour charger ses modèles |
| `ZDEBUG` | — | `1` = traces d'état de l'IA dans les logs |
| `W`, `H`, `FREQ` | voir tableau | taille de carte / fréquence (écrase la vitesse) |
| `SUBJECT_TIMES` | 1 en slow | `1` = durées du sujet (pas visibles), `0` = 1/f pour tout |
| `STEP` | selon vitesse | secondes entre les étapes de la vitrine (3 / 1.5 / 0.2) |
| `BOTS` | 0 | `1` = une colonie IA joue en fond pendant le mode manuel |

Le binaire `zappy_gui` garde la ligne de commande du sujet (`-p port -h
machine`) : ces modes passent par des variables `ZAPPY_*` (`ZAPPY_MODE`,
`ZAPPY_SIZE`, `ZAPPY_RECORD`, `ZAPPY_SHOTS`, `ZAPPY_SHOT_EVERY`,
`ZAPPY_TUTORIAL_AUTO`, `ZAPPY_END_HOLD`, `ZAPPY_FFMPEG`). Sans elles, le GUI
se comporte comme avant (H affiche quand même le panneau d'explications).

Les logs (`server.log`, `gui.log`, `ai_<équipe>.log`) sont dans
`/tmp/zappy_demo` (modifiable via `LOG_DIR`).

## Déroulé conseillé

- **Soutenance** : Vitrine (`DEMO_PAUSE=1 ./scripts/demo/demo.sh 4 slow`),
  puis Express (`demo.sh 1 medium`), puis Duel ou Bataille, et Manuel si le
  jury veut tester une commande.
- **Recruteur / public** : Tutoriel (`demo.sh 6`), qui laisse ensuite la
  partie tourner avec le panneau d'explications.
- **Réseaux sociaux** : Vidéo (`demo.sh 7 medium`), MP4 prêt dans `videos/`.
