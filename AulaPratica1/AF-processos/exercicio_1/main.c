#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#define NUM_FILHOS 2

//       (pai)      
//         |        
//    +----+----+
//    |         |   
// filho_1   filho_2


// ~~~ printfs  ~~~
// pai (ao criar filho): "Processo pai criou %d\n"
//    pai (ao terminar): "Processo pai finalizado!\n"
//  filhos (ao iniciar): "Processo filho %d criado\n"

// Obs:
// - pai deve esperar pelos filhos antes de terminar!

int main(int argc, char** argv) {
    for (int i = 0; i < NUM_FILHOS; i++) {
        pid_t pid = fork();
        if (pid < 0) { // Erro na criação do processo filho
            printf("Erro ao criar processo filho!\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Processo filho criado com sucesso
            printf("Processo filho %d criado\n", getpid());
            exit(EXIT_SUCCESS);
        } else { // Processo pai criou um processo filho com sucesso
            printf("Processo pai criou %d\n", pid);
        }
    }

    // Espera os filhos terminarem de executar
    while(wait(NULL) >= 0);
    printf("Processo pai finalizado!\n");
    return 0;
}
