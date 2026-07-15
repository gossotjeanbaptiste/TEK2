/*
** EPITECH PROJECT, 2026
** Panoramix [WSL : Ubuntu-24.04]
** File description:
** main
*/

#include "include/pano.h"

static int parse_arguments(int argc, char **argv, panoramix_data_t *data,
    int *nb_villagers)
{
    if (argc != 5) {
        fprintf(stderr, "USAGE: ./panoramix <nb_villagers>"
            " <pot_size> <nb_fights> <nb_refills>\n");
        return 84;
    }
    *nb_villagers = atoi(argv[1]);
    data->pot_size = atoi(argv[2]);
    data->nb_fights = atoi(argv[3]);
    data->nb_refills = atoi(argv[4]);
    if (*nb_villagers <= 0 || data->pot_size <= 0 ||
        data->nb_fights <= 0 || data->nb_refills <= 0) {
        fprintf(stderr, "USAGE: ./panoramix <nb_villagers> "
            "<pot_size> <nb_fights> <nb_refills>\n");
        fprintf(stderr, "Values must be >0.\n");
        return 84;
    }
    data->portions_left = data->pot_size;
    return 0;
}

static int init_sync_tools(panoramix_data_t *data)
{
    data->simulation_over = 0;
    if (pthread_mutex_init(&data->cauldron_mutex, NULL) != 0)
        return 84;
    if (sem_init(&data->sem_druid_wakeup, 0, 0) != 0)
        return 84;
    if (sem_init(&data->sem_villager_wait, 0, 0) != 0)
        return 84;
    return 0;
}

static int launch_simulation(panoramix_data_t *data,
    int nb_villagers, thread_manager_t *tm)
{
    tm->villager_threads = malloc(sizeof(pthread_t) * nb_villagers);
    tm->villagers_data = malloc(sizeof(villager_t) * nb_villagers);
    if (!tm->villager_threads || !tm->villagers_data)
        return 84;
    if (pthread_create(&tm->druid_thread, NULL, druid_routine, data) != 0)
        return 84;
    for (int i = 0; i < nb_villagers; i++) {
        tm->villagers_data[i].id = i;
        tm->villagers_data[i].data = data;
        if (pthread_create(&tm->villager_threads[i], NULL,
                villager_routine, &tm->villagers_data[i]) != 0)
            return 84;
    }
    return 0;
}
// * C'est quoi &(*v_threads)[i]
// * v_threads -> l'adresse du pointeur de pointeur
// * (*v_threads) -> dereferencage du pointeur pour acceder a
// * l'element directement + parantheses pour une question de priorite
// * [i] accede au i-eme element
// * & recupere l'adresse memoire de l'element en question
// ! Meme logique pour &(*v_data)[i] avec le seul element distinguant
// ! etant que threads manipulait pthread_t et data manipule villager_t

static void end_simulation(panoramix_data_t *data,
    int nb_villagers, thread_manager_t *tm)
{
    for (int i = 0; i < nb_villagers; i++)
        pthread_join(tm->villager_threads[i], NULL);
    data->simulation_over = 1;
    sem_post(&data->sem_druid_wakeup);
    pthread_join(tm->druid_thread, NULL);
    free(tm->villager_threads);
    free(tm->villagers_data);
    pthread_mutex_destroy(&data->cauldron_mutex);
    sem_destroy(&data->sem_druid_wakeup);
    sem_destroy(&data->sem_villager_wait);
}

int main(int argc, char **argv)
{
    panoramix_data_t data;
    int nb_villagers = 0;
    thread_manager_t tm;

    tm.villager_threads = NULL;
    tm.villagers_data = NULL;
    if (parse_arguments(argc, argv, &data, &nb_villagers) == 84)
        return 84;
    if (init_sync_tools(&data) == 84)
        return 84;
    if (launch_simulation(&data, nb_villagers, &tm) == 84)
        return 84;
    end_simulation(&data, nb_villagers, &tm);
    return 0;
}
