/*
** EPITECH PROJECT, 2026
** Panoramix [WSL : Ubuntu-24.04]
** File description:
** step2
*/

#ifndef INCLUDED_STEP2_H
    #define INCLUDED_STEP2_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <unistd.h>
    #include <string.h>

    #define NB_TRAINS 4
    #define NB_MAX 2
    #define BRIDGE_START 5
    #define BRIDGE_LENGTH 4
    #define TRACK_LENGTH 15

    // Macro pour représenter la position
    #define TRAIN_CHAR '#'
    #define TRACK_CHAR '='
    #define BRIDGE_LEFT '|'
    #define BRIDGE_RIGHT '|'
    #define BRIDGE_CHAR '-'

// Données d'un train
typedef struct {
    int train_id;
    int position;
} train_t;

// Données partagées entre threads
typedef struct {
    train_t trains[NB_TRAINS];
    pthread_mutex_t display_mutex;
    sem_t bridge_semaphore;
} shared_state_t;

typedef struct {
    int train_id;
    shared_state_t *state;
} thread_args_t;
#endif
