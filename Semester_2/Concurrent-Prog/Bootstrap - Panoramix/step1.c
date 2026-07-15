/*
** EPITECH PROJECT, 2026
** Panoramix [WSL : Ubuntu-24.04]
** File description:
** step1
*/

#include "step1.h"

/**
 * Affiche l'état actuel de la simulation
 */

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

static void display_trains(shared_state_t *state)
{
    printf("\033[2J\033[H");
    printf("0123456789012345\n");
    printf("-------------------\n");
    for (int i = 0; i < NB_TRAINS; i++) {
        print_char(i, state);
        printf("\n");
    }
}

static void move_train_before_bridge(int train_id,
    int *position, shared_state_t *state)
{
    while (*position < BRIDGE_START) {
        state->trains[train_id].position = *position;
        display_trains(state);
        (*position)++;
        usleep(100000);
    }
}

static void move_train_on_bridge(int train_id,
    int *position, shared_state_t *state)
{
    while (*position < TRACK_LENGTH) {
        state->trains[train_id].position = *position;
        display_trains(state);
        (*position)++;
        usleep(100000);
    }
}

/**
 * Fonction exécutée par chaque thread de train
 */
void *train_thread(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;
    int train_id = args->train_id;
    shared_state_t *state = args->state;
    int position = 0;

    printf("Train %d started\n", train_id);
    move_train_before_bridge(train_id, &position, state);
    pthread_mutex_lock(&state->bridge_mutex);
    printf("Train %d is crossing the bridge\n", train_id);
    move_train_on_bridge(train_id, &position, state);
    printf("Train %d has crossed the bridge\n", train_id);
    pthread_mutex_unlock(&state->bridge_mutex);
    free(args);
    return NULL;
}

int main(void)
{
    pthread_t threads[NB_TRAINS];
    shared_state_t state;
    thread_args_t *args;

    pthread_mutex_init(&state.bridge_mutex, NULL);
    for (int i = 0; i < NB_TRAINS; i++) {
        state.trains[i].train_id = i;
        state.trains[i].position = 0;
    }
    for (int i = 0; i < NB_TRAINS; i++) {
        args = malloc(sizeof(thread_args_t));
        args->train_id = i;
        args->state = &state;
        pthread_create(&threads[i], NULL, train_thread, args);
    }
    for (int i = 0; i < NB_TRAINS; i++)
        pthread_join(threads[i], NULL);
    pthread_mutex_destroy(&state.bridge_mutex);
    printf("All trains have crossed the bridge!\n");
    return 0;
}
