#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_FILHOS 2
#define NUM_NETOS 3
#define TEMPO_ESPERA 5

//                          (principal)
//                               |
//              +----------------+--------------+
//              |                               |
//           filho_1                         filho_2
//              |                               |
//    +---------+-----------+          +--------+--------+
//    |         |           |          |        |        |
// neto_1_1  neto_1_2  neto_1_3     neto_2_1 neto_2_2 neto_2_3

// ~~~ printfs  ~~~
//      principal (ao finalizar): "Processo principal %d finalizado\n"
// filhos e netos (ao finalizar): "Processo %d finalizado\n"
//    filhos e netos (ao inciar): "Processo %d, filho de %d\n"

// Obs:
// - netos devem esperar 5 segundos antes de imprmir a mensagem de finalizado (e terminar)
// - pais devem esperar pelos seu descendentes diretos antes de terminar

void cria_netos() {
    for (int n = 0; n < NUM_NETOS; n++) {
        pid_t pid = fork();
        if (pid < 0) { // Erro ao criar proceso neto
            perror("Erro ao criar neto\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Processo neto criado com sucesso
            printf("Processo %d, filho de %d\n", getpid(), getppid());
            sleep(TEMPO_ESPERA);
            printf("Processo %d finalizado\n", getpid());
            exit(EXIT_SUCCESS);
        }
    }
    while(wait(NULL) >= 0);
}

void cria_filhos_e_netos() {
    for (int f = 0; f < NUM_FILHOS; f++) {
        pid_t pid = fork();
        if (pid < 0) { // Erro ao criar processo filho
            perror("Erro ao criar filho\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Processo filho criado com sucesso
            printf("Processo %d, filho de %d\n", getpid(), getppid());
            cria_netos();
            sleep(TEMPO_ESPERA);
            printf("Processo %d finalizado\n", getpid());
            exit(EXIT_SUCCESS);
        }
    }
    while(wait(NULL) >= 0);
}

int main() {
    int pid = getpid();
    cria_filhos_e_netos();
    printf("Processo principal %d finalizado\n", pid);
    return EXIT_SUCCESS;
}
