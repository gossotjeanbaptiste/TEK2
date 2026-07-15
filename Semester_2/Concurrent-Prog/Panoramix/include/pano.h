/*
** EPITECH PROJECT, 2026
** Panoramix [WSL : Ubuntu-24.04]
** File description:
** pano
*/

#ifndef PANO_H
    #define PANO_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include <semaphore.h>

typedef struct {
    int pot_size;
    int nb_fights;
    int nb_refills;
    int portions_left;
    int simulation_over;
    pthread_mutex_t cauldron_mutex;
    sem_t sem_druid_wakeup;
    sem_t sem_villager_wait;
} panoramix_data_t;

typedef struct {
    int id;
    panoramix_data_t *data;
} villager_t;

typedef struct {
    pthread_t druid_thread;
    pthread_t *villager_threads;
    villager_t *villagers_data;
} thread_manager_t;

void *druid_routine(void *arg);
void *villager_routine(void *arg);
#endif
