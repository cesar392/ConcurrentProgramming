#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include "helper.c"

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// | 
// |         tamanho do vetor (usado <-----+
// |         como 2o retorno)              |
// v                                       v
double* load_vector(const char* filename, int* out_size);

// Avalia o resultado no vetor c. Assume-se que todos os ponteiros (a, b, e c)
// tenham tamanho size.
void avaliar(double* a, double* b, double* c, int size);

typedef struct {
    int start;
    int end;
    double* a;
    double* b;
    double* result;
} data_to_sum_t;

void* sum_vector(void* arg) {
    data_to_sum_t *data = (data_to_sum_t*) arg;
    for (int i = data->start; i < data->end; i++) {
        data->result[i] = data->a[i] + data->b[i];
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // Gera um resultado diferente a cada execução do programa
    // Se **para fins de teste** quiser gerar sempre o mesmo valor
    // descomente o srand(0)
    // srand(time(NULL)); //valores diferentes
    srand(0);        //sempre mesmo valor
    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n",
               argv[0]);
        return 1;
    }
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }
    //Cria vetor do resultado vazio
    double* c = malloc(a_size*sizeof(double));

    // Divide o trabalho entre as threads
    if (n_threads > a_size) {
        n_threads = a_size;
    }
    int chunk_size = (a_size + n_threads - 1) / n_threads;
    int remainder = a_size % n_threads;
    int current = 0;

    // Cria lista de threads e estrutura de dados para somar
    pthread_t threads[n_threads];
    data_to_sum_t data[n_threads];

    for (int i = 0; i < n_threads; ++i) {
        data[i].a = a;
        data[i].b = b;
        data[i].result = c;

        data[i].start = current;
        current += chunk_size;
        data[i].end = current;

        // Adiciona o restante aos últimos threads
        if (remainder && i >= n_threads - remainder) {
            data[i].end++;
            remainder--;
        }

        pthread_create(&threads[i], NULL, sum_vector, &data[i]);
    }

    for (int i = 0; i < n_threads; ++i)
        pthread_join(threads[i], NULL);

    for (int i = 0; i < n_threads; i++) {
        for (int j = data[i].start; j < data[i].end; j++) {
            c[j] = data[i].result[j];
        }
    }

    // IMPORTANTE: avalia o resultado e liberar memória!
    avaliar(a, b, c, a_size);
    free(a);
    free(b);
    free(c);
    return 0;
}
