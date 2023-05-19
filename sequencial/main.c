#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gol.h"

typedef struct {
    int size;
    int steps;
    cell_t **prev;
    cell_t **next;
    stats_t stats_step;
} thread_data_t;

pthread_mutex_t mutex; // Mutex global

int main(int argc, char **argv)
{
    int size, steps;
    cell_t **prev, **next, **tmp;
    FILE *f;
    stats_t stats_step = {0, 0, 0, 0};
    stats_t stats_total = {0, 0, 0, 0};
    int num_threads;

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
    num_threads = atoi(argv[2]); // Converter o argumento de string para inteiro

    prev = allocate_board(size);
    next = allocate_board(size);

    read_file(f, prev, size);

    fclose(f);

#ifdef DEBUG
    printf("Initial:\n");
    print_board(prev, size);
    print_stats(stats_step);
#endif

    pthread_t threads[num_threads];
    thread_data_t thread_data[num_threads];

    // Inicializar o mutex
    pthread_mutex_init(&mutex, NULL);

    // Dividir o número de passos igualmente entre as threads
    int steps_per_thread = steps / num_threads;
    int remaining_steps = steps % num_threads;

    // Criar as threads
    for (int i = 0; i < num_threads; i++)
    {
        thread_data[i].size = size;
        thread_data[i].steps = steps_per_thread;

        if (i == 0)
            thread_data[i].steps += remaining_steps;

        thread_data[i].prev = prev;
        thread_data[i].next = next;
        thread_data[i].stats_step = stats_step;

        pthread_create(&threads[i], NULL, play, &thread_data[i]);
    }


    for (int i = 0; i < steps; i++)
    {
        stats_step = play(prev, next, size);
        
        stats_total.borns += stats_step.borns;
        stats_total.survivals += stats_step.survivals;
        stats_total.loneliness += stats_step.loneliness;
        stats_total.overcrowding += stats_step.overcrowding;

#ifdef DEBUG
        printf("Step %d ----------\n", i + 1);
        print_board(next, size);
        print_stats(stats_step);
#endif
        tmp = next;
        next = prev;
        prev = tmp;
    }

    // Esperar todas as threads terminarem
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);

        // Adquirir o mutex antes de atualizar as estatísticas totais
        pthread_mutex_lock(&mutex);

        // Atualizar as estatísticas totais
        stats_total.borns += thread_data[i].stats_step.borns;
        stats_total.survivals += thread_data[i].stats_step.survivals;
        stats_total.loneliness += thread_data[i].stats_step.loneliness;
        stats_total.overcrowding += thread_data[i].stats_step.overcrowding;

        // Liberar o mutex após atualizar as estatísticas totais
        pthread_mutex_unlock(&mutex);
    }

#ifdef RESULT
    printf("Final:\n");
    print_board(prev, size);
    print_stats(stats_total);
#endif

    // Destruir o mutex
    pthread_mutex_destroy(&mutex);

    free_board(prev, size);
    free_board(next, size);
}
