/******************************************************************************
 * Fichier           : bitache1.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 2 octobre 2026
 * Description       : Programme illustrant la création d'un processus fils via 
 *                     fork() et l'attente de sa fin par le père avec wait(). 
 *                     Le père analyse ensuite le mode de terminaison du fils 
 *                     (normale ou anormale) via les macros WIFEXITED/WIFSIGNALED.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void gerer_pere(pid_t pid_fils);
void gerer_fils(void);

int main(void)
{
    pid_t pidf = fork();

    switch (pidf) {
    case -1:
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);

    case 0:
        /* Processus fils */
        gerer_fils();


    default:
        /* Processus père */
        gerer_pere(pidf);

    }

    return 0;
}

void gerer_fils(void)
{
    printf("\t[FILS] Je suis le fils. Mon PID = %d, PID de mon pere = %d\n",getpid(), getppid());
    sleep(10); // délai durant lequel on peux kill le fils pour simuler une terminaise anormal de ce dernier
    exit(42); 
}

void gerer_pere(pid_t pid_fils)
{
    int status = 0;

    printf("[PERE] Je suis le pere. Mon PID = %d, PID de mon fils = %d\n",getpid(), pid_fils);

    wait(&status); // Bloque jusqu'à la terminaison du fils

    /* Mode de terminaison */
    if (WIFEXITED(status)) {
        printf("Mode de terminaison : NORMALE\n");
        printf("Valeur retournee : %d\n", WEXITSTATUS(status));
    } 
    else if (WIFSIGNALED(status)) {
        printf("Mode de terminaison : ANORMALE\n");
    } 
    else {
        printf("Mode de terminaison : AUTRE\n");
    }
}
