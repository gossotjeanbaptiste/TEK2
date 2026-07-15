# Convention des branches

Afin de garder un dépôt propre et facile à suivre, nous utilisons une organisation simple des branches selon leur rôle.

## Branche principale

### `main`
Cette branche contient la version stable du projet.

Règles :
- Uniquement du code fonctionnel ;
- Pas de développement en cours ;
- Utilisée pour les versions prêtes à être livrées.

---
# Branch naming convention

To keep the repository clean and easy to follow, we use a simple branch organization based on role.

## Main branch

### `main`
This branch contains the stable project version.

Rules:
- Only functional code;
- No ongoing development;
- Used for release-ready versions.

---

## Development branch

### `dev`
This branch holds the project's main integration.

It is used to:
- Merge finished features;
- Test the project as a whole;
- Prepare upcoming releases before merging into `main`.

---

## Feature branches

### `feature/...`
These branches are for developing a new feature without impacting the rest of the project.

Examples:
- `feature/gui-map-render`
- `feature/server-network`
- `feature/ai-exploration`
- `feature/protocol-parser`

Rules:
- One branch = one clear feature;
- Short and descriptive name;
- Merge into `dev` when finished.

---

## Fix branches

### `fix/...`
These branches are for fixing non-urgent bugs.

Examples:
- `fix/player-movement`
- `fix/gui-refresh`
- `fix/command-parser`

### `hotfix/...`
These branches are for fixing critical or blocking issues.

Examples:
- `hotfix/server-crash`
- `hotfix/login-bug`
- `hotfix/gui-freeze`

Rules:
- Quick fix;
- High priority;
- Merge into `dev`, and then into `main` if needed.

---

## Test / experiment branches

### `test/...`
These branches are used to experiment with an idea, behavior, or temporary implementation.

Examples:
- `test/gui-library`
- `test/ai-strategy`
- `test/network-performance`

These branches can be deleted if the experiment is not kept.

---

## Release preparation branches

### `release/...`
Use these branches when preparing a stable release.

Examples:
- `release/v1.0`
- `release/gui-beta`

They are used to:
- Finalize last adjustments;
- Fix small issues before publishing;
- Stabilize the release before merging into `main`.

---

## Naming convention

Branch names should be:
- lowercase;
- readable;
- without spaces;
- separated by dashes `-`.

Good examples:
- `feature/ai-exploration`
- `fix/gui-refresh`
- `hotfix/server-crash`

Bad examples:
- `FeatureAI`
- `fix bug`
- `branch1`

---

## Recommended project structure

For this project, a simple structure can be:

- `main`: stable version
- `dev`: integration branch
- `feature/server-*`: server development
- `feature/gui-*`: GUI development
- `feature/ai-*`: AI development
- `fix/*`: bug fixes
- `hotfix/*`: urgent fixes

---

## Workflow example

1. Create a branch from `dev`
2. Develop the feature
3. Test the code
4. Commit using the agreed convention
5. Merge into `dev`
6. When stable, merge into `main`

---

## Best practices

- Avoid working directly on `main`;
- Keep one task per branch;
- Delete branches once finished;
- Use explicit branch names;
- Prefer short, focused branches.