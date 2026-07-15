# Tests unitaires Zappy

Ce dossier contient une base de tests unitaires pour le serveur Zappy.

## Structure

- `test_server.c`
  - Tests basiques et utilitaires :
    - `wrap_coord`
    - `resource_index`
    - `enqueue_write`
    - `queue_player_command`
    - `get_tile`
- `test_command.c`
  - Tests de commandes serveur :
    - `run_forward`
    - `run_right`
    - `run_left`
    - `run_connect_nbr`
    - `run_inventory`
    - `run_look`
- `Makefile`
  - Compile deux binaires de test :
    - `test_server`
    - `test_command`

## Comment exécuter les tests

Depuis la racine du projet :

```sh
cd test
make clean && make
./test_server
./test_command
```

Ou en une seule commande :

```sh
cd test && make clean && make && ./test_server && ./test_command
```

## Ajouter un nouveau test

1. Crée un nouveau fichier `test/test_<nom>.c`.
2. Inclue `../include/server/server_internal.h`.
3. Ajoute des prototypes des fonctions serveur que tu veux tester si elles ne sont pas exposées par un header public.
4. Rédige des fonctions de test avec `assert(...)`.
5. Ajoute le fichier source et la cible correspondante dans `test/Makefile`.

### Exemple de test

```c
#include <assert.h>
#include <stdio.h>
#include "../include/server/server_internal.h"

static void test_example(void)
{
    assert(wrap_coord(-1, 4) == 3);
}

int main(void)
{
    test_example();
    printf("Example test passed.\n");
    return 0;
}
```

## Comment étendre `test/Makefile`

1. Ajoute un nouveau bloc `TEST3_SRC` avec les fichiers nécessaires.
2. Ajoute `TEST3_TARGET` et sa règle de compilation.
3. Ajoute la nouvelle cible à la règle `all:`.

Par exemple :

```make
TEST3_SRC = ../src/server/server_xyz.c \
            test_xyz.c
TEST3_TARGET = test_xyz

all: $(TARGET) $(TEST2_TARGET) $(TEST3_TARGET)

$(TEST3_TARGET): $(TEST3_SRC)
	$(CC) $(CFLAGS) $(TEST3_SRC) -o $(TEST3_TARGET)
```

## Bonnes pratiques

- Ne lie que les fichiers nécessaires au test.
- Evite d inclure tout le serveur si un seul module suffit.
- Utilise des stubs pour les fonctions externes non testées.
- Appelle `free()` sur les ressources allouées dans les tests.

## Résultat attendu

Les deux tests actuels doivent afficher :

```sh
All tests passed.
Command tests passed.
```
