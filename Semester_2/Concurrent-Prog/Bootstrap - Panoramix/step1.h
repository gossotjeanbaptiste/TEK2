/*
** EPITECH PROJECT, 2026
** Panoramix [WSL : Ubuntu-24.04]
** File description:
** step1
*/

#ifndef INCLUDED_STEP1_H
    #define INCLUDED_STEP1_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include <unistd.h>
    #include <string.h>

    #define NB_TRAINS 3
    #define BRIDGE_START 5
    #define BRIDGE_LENGTH 4
    #define TRACK_LENGTH 15

    // Macro pour représenter la position
    #define TRAIN_CHAR '#'
    #define TRACK_CHAR '='
    #define BRIDGE_LEFT '|'
    #define BRIDGE_RIGHT '|'
    #define BRIDGE_CHAR '-'

// Données des trains
typedef struct {
    int train_id;
    int position;
} train_t;

// Données partagées entre threads
typedef struct {
    train_t trains[NB_TRAINS];
    pthread_mutex_t bridge_mutex;
} shared_state_t;

/**
 * Structure passée aux threads
 */
typedef struct {
    int train_id;
    shared_state_t *state;
} thread_args_t;
#endif
