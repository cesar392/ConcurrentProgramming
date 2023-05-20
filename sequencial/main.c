#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "gol.h"

typedef struct {
    int size;
    int steps;
    cell_t **prev;
    cell_t **next;
    stats_t stats_step;
} thread_data_t;

pthread_mutex_t stats_mutex;
sem_t step_semaphore;
int current_step = 0;
stats_t stats_total = {0, 0, 0, 0};  // Definir stats_total no escopo global

void *play_parallel(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;

    while (1) {
        sem_wait(&step_semaphore);

        if (current_step >= data->steps) {
            sem_post(&step_semaphore);
            break;
        }

        int step = current_step++;
        data->stats_step = play(data->prev, data->next, data->size);

        pthread_mutex_lock(&stats_mutex);
        stats_total.borns += data->stats_step.borns;
        stats_total.survivals += data->stats_step.survivals;
        stats_total.loneliness += data->stats_step.loneliness;
        stats_total.overcrowding += data->stats_step.overcrowding;
        pthread_mutex_unlock(&stats_mutex);

        printf("Step %d ----------\n", step + 1);
        print_board(data->next, data->size);
        print_stats(data->stats_step);

        cell_t **tmp = data->next;
        data->next = data->prev;
        data->prev = tmp;

        sem_post(&step_semaphore);
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    int size, steps;
    cell_t **prev, **next;
    FILE *f;

    if (argc != 3)
    {
        printf("ERRO! Você deve digitar %s <nome do arquivo do tabuleiro> <número de threads>!\n\n", argv[0]);
        return 0;
    }

    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("ERRO! O arquivo de tabuleiro '%s' não existe!\n\n", argv[1]);
        return 0;
    }

    fscanf(f, "%d %d", &size, &steps);

    prev = allocate_board(size);
    next = allocate_board(size);

    read_file(f, prev, size);

    fclose(f);

    int num_threads = atoi(argv[2]);
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
    thread_data_t *thread_data = (thread_data_t *)malloc(sizeof(thread_data_t) * num_threads);

    pthread_mutex_init(&stats_mutex, NULL);
    sem_init(&step_semaphore, 0, 1);

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].size = size;
        thread_data[i].steps = steps;
        thread_data[i].prev = prev;
        thread_data[i].next = next;
        thread_data[i].stats_step = (stats_t){0, 0, 0, 0};
        pthread_create(&threads[i], NULL, play_parallel, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

#ifdef RESULT
    printf("Final:\n");
    print_board(thread_data->prev, thread_data->size);
    print_stats(thread_data->stats_step);
#endif

    free(threads);
    free(thread_data);
    free_board(prev, size);
    free_board(next, size);

    pthread_mutex_destroy(&stats_mutex);
    sem_destroy(&step_semaphore);

    return 0;
}
