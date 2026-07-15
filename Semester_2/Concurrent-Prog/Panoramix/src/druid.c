/*
** EPITECH PROJECT, 2026
** Panoramix [WSL : Ubuntu-24.04]
** File description:
** druid
*/

#include "../include/pano.h"

void *druid_routine(void *arg)
{
    panoramix_data_t *data = (panoramix_data_t *)arg;

    printf("Druid: I'm ready... but sleepy...\n");
    while (data->nb_refills > 0) {
        sem_wait(&data->sem_druid_wakeup);
        if (data->simulation_over == 1)
            break;
        data->portions_left = data->pot_size;
        data->nb_refills--;
        printf("Druid: Ah! Yes, yes, I'm awake! Working on it! "
            "Beware I can only make %d more refills after this one.\n",
            data->nb_refills);
        sem_post(&data->sem_villager_wait);
    }
    printf("Druid: I'm out of viscum. I'm going back to... zZz\n");
    return NULL;
}
