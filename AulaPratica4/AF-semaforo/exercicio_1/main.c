#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

FILE* out;
sem_t semaphore_a, semaphore_b;
int a_count = 0, b_count = 0;

void *thread_a(void *args) {
    for (int i = 0; i < *(int*)args; ++i) {
        if (a_count <= b_count) {
            sem_wait(&semaphore_a);
            fprintf(out, "A");
            fflush(stdout);
            a_count++;
        } else {
            sem_wait(&semaphore_a);
            sem_post(&semaphore_b);
            i--;
        }
        sem_post(&semaphore_a);
    }
    return NULL;
}

void *thread_b(void *args) {
    for (int i = 0; i < *(int*)args; ++i) {
        if (b_count <= a_count) {
            sem_wait(&semaphore_b);
            fprintf(out, "B");
            fflush(stdout);
            b_count++;
        } else {
            sem_wait(&semaphore_b);
            sem_post(&semaphore_a);
            i--;
        }
        sem_post(&semaphore_b);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: %s [ITERAÇÕES]\n", argv[0]);
        return 1;
    }
    int iters = atoi(argv[1]);
    srand(time(NULL));
    out = fopen("result.txt", "w");

    pthread_t ta, tb;
    sem_init(&semaphore_a, 0, 1);
    sem_init(&semaphore_b, 0, 1);

    // Cria threads
    pthread_create(&ta, NULL, thread_a, &iters);
    pthread_create(&tb, NULL, thread_b, &iters);

    // Espera pelas threads
    pthread_join(ta, NULL);
    pthread_join(tb, NULL);
    sem_destroy(&semaphore_a);
    sem_destroy(&semaphore_b);

    //Imprime quebra de linha e fecha arquivo
    fprintf(out, "\n");
    fclose(out);
    return 0;
}
