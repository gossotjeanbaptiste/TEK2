/*
** EPITECH PROJECT, 2026
** Panoramix [WSL : Ubuntu-24.04]
** File description:
** step2
*/

#include "step2.h"

static void print_char(int i, shared_state_t *state)
{
    for (int j = 0; j < TRACK_LENGTH; j++) {
        if (j == state->trains[i].position)
            printf("%c", TRAIN_CHAR);
        if (j >= BRIDGE_START && j < BRIDGE_START + BRIDGE_LENGTH)
            printf("%c", BRIDGE_CHAR);
        else
            printf("%c", TRACK_CHAR);
        printf("\n");
    }
}

/**
 * Affiche l'état actuel de la simulation
 */
static void display_trains(shared_state_t *state)
{
    printf("\033[2J\033[H");
    printf("0123456789012345\n");
    printf("-------------------\n");
    for (int i = 0; i < NB_TRAINS; i++)
        print_char(i, state);
    printf("\n");
}

/**
 * Déplace le train avant le pont
 */
static void move_train_before_bridge(int train_id, int *position,
    shared_state_t *state)
{
    while (*position < BRIDGE_START) {
        pthread_mutex_lock(&state->display_mutex);
        state->trains[train_id].position = *position;
        display_trains(state);
        pthread_mutex_unlock(&state->display_mutex);
        (*position)++;
        usleep(100000);
    }
}

/**
 * Déplace le train sur le pont
 */
static void move_train_on_bridge(int train_id, int *position,
    shared_state_t *state)
{
    while (*position < TRACK_LENGTH) {
        pthread_mutex_lock(&state->display_mutex);
        state->trains[train_id].position = *position;
        display_trains(state);
        pthread_mutex_unlock(&state->display_mutex);
        (*position)++;
        usleep(100000);
    }
}

/**
 * Train attend et traverse le pont avec sémaphore
 */
static void cross_bridge(int train_id, int *position, shared_state_t *state)
{
    sem_wait(&state->bridge_semaphore);
    printf("Train %d is crossing the bridge\n", train_id);
    move_train_on_bridge(train_id, position, state);
    printf("Train %d has crossed the bridge\n", train_id);
    sem_post(&state->bridge_semaphore);
}

/**
 * Fonction exécutée par chaque thread de train
 */
static void *train_thread(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;
    int train_id = args->train_id;
    shared_state_t *state = args->state;
    int position = 0;

    printf("Train %d started\n", train_id);
    move_train_before_bridge(train_id, &position, state);
    cross_bridge(train_id, &position, state);
    free(args);
    return NULL;
}

/**
 * Initialise les structures partagées (mutex et sémaphore)
 */
static void init_synchronization(shared_state_t *state)
{
    pthread_mutex_init(&state->display_mutex, NULL);
    sem_init(&state->bridge_semaphore, 0, NB_MAX);
}

/**
 * Initialise tous les trains
 */
static void init_trains(shared_state_t *state)
{
    for (int i = 0; i < NB_TRAINS; i++) {
        state->trains[i].train_id = i;
        state->trains[i].position = 0;
    }
}

/**
 * Crée les threads pour les trains
 */
static void create_threads(pthread_t *threads, shared_state_t *state)
{
    thread_args_t *args;

    for (int i = 0; i < NB_TRAINS; i++) {
        args = malloc(sizeof(thread_args_t));
        args->train_id = i;
        args->state = state;
        pthread_create(&threads[i], NULL, train_thread, args);
    }
}

/**
 * Attend la fin de tous les threads
 */
static void wait_threads(pthread_t *threads)
{
    for (int i = 0; i < NB_TRAINS; i++)
        pthread_join(threads[i], NULL);
}

/**
 * Détruit les ressources partagées
 */
static void destroy_resources(shared_state_t *state)
{
    pthread_mutex_destroy(&state->display_mutex);
    sem_destroy(&state->bridge_semaphore);
}

int main(void)
{
    pthread_t threads[NB_TRAINS];
    shared_state_t state;

    init_synchronization(&state);
    init_trains(&state);
    create_threads(threads, &state);
    wait_threads(threads);
    destroy_resources(&state);
    printf("All trains have crossed the bridge!\n");
    return 0;
}
