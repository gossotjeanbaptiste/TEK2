/*
** EPITECH PROJECT, 2026
** Panoramix [WSL : Ubuntu-24.04]
** File description:
** villager
*/

#include "../include/pano.h"


static int villager_condition(panoramix_data_t *data, villager_t *villager)
{
    if (data->portions_left == 0) {
        if (data->nb_refills == 0) {
            pthread_mutex_unlock(&data->cauldron_mutex);
            return 84;
        }
        printf("Villager %d: Hey Pano wake up! We need more potion.\n",
            villager->id);
        sem_post(&data->sem_druid_wakeup);
        sem_wait(&data->sem_villager_wait);
    }
    return 0;
}

static void villager_loop(panoramix_data_t *data, villager_t *villager)
{
    int fights_left = data->nb_fights;

    while (fights_left > 0) {
        pthread_mutex_lock(&data->cauldron_mutex);
        printf("Villager %d: I need a drink... I see %d servings left.\n",
            villager->id, data->portions_left);
        if (villager_condition(data, villager) == 84)
            break;
        data->portions_left--;
        pthread_mutex_unlock(&data->cauldron_mutex);
        fights_left--;
        printf("Villager %d: Take that roman scum! Only %d left.\n",
            villager->id, fights_left);
    }
}

void *villager_routine(void *arg)
{
    villager_t *villager = (villager_t *)arg;
    panoramix_data_t *data = villager->data;

    printf("Villager %d: Going into battle!\n", villager->id);
    villager_loop(data, villager);
    printf("Villager %d: I'm going to sleep now.\n", villager->id);
    return NULL;
}
