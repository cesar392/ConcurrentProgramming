#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

//                 (main)      
//                    |
//    +----------+----+------------+
//    |          |                 |   
// worker_1   worker_2   ....   worker_n


// ~~~ argumentos (argc, argv) ~~~
// ./program n_threads

// ~~~ printfs  ~~~
// main thread (após término das threads filhas): "Contador: %d\n"
// main thread (após término das threads filhas): "Esperado: %d\n"

// Obs:
// - pai deve criar n_threds (argv[1]) worker threads 
// - cada thread deve incrementar contador_global (operador ++) n_loops vezes
// - pai deve esperar pelas worker threads  antes de imprimir!

int contador_global = 0;

void* incrementa_contador(void* arg) {
    int n_loops = *((int*) arg);
    for (int i = 0; i < n_loops; i++) {
        ++contador_global;
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("n_threads é obrigatório!\n");
        printf("Uso: %s n_threads n_loops\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);
    int n_loops = atoi(argv[2]);
    pthread_t threads[n_threads];

    for (int i = 0; i < n_threads; ++i)
        pthread_create(&threads[i], NULL, incrementa_contador, &n_loops);

    for (int i = 0; i < n_threads; ++i)
        pthread_join(threads[i], NULL);

    printf("Contador: %d\n", contador_global);
    printf("Esperado: %d\n", n_threads*n_loops);

    return 0;
}

/* O que acontece com o valor do contador_global conforme o número de threads aumenta?

Quanto mais threads são criadas, mais paralelismo é introduzido no programa. 
Isso pode levar a um aumento do desempenho, mas também pode introduzir problemas de concorrência. 
Neste caso, como várias threads estão incrementando a mesma variável global simultaneamente, 
pode ocorrer um problema conhecido como condição de corrida.
Isso ocorre quando duas ou mais threads tentam acessar a mesma variável ao mesmo tempo 
e acabam sobrescrevendo o valor umas das outras, resultando em um resultado final imprevisível.

*/

/* O que acontece quando o valor de n_loops aumenta?

Quanto maior o valor de n_loops, mais trabalho cada thread terá que realizar e, portanto, 
o tempo de execução total do programa aumenta. Além disso,  o risco de condições de corrida também aumenta.
É possível corrigir isso com mecanismos de sincronização.
*/