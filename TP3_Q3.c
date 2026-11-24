/******************************************************************************
 * Fichier                  : TP3_Q3.c
 * Auteur                  : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 2 octobre 2026
 * Description           : Programme qui permet de synchroniser les 2 processus 
 *                         pere et fils avec des semaphores pour ne pas bloquer
 *                         les affichages.
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "./fcts_sem.c"

int semid;   /* sémaphore protégeant l'écran */

void gerer_pere(void){
    prendresem(semid);
    for(int i=0;i<=5;i++){
        printf("Je suis le pere (%d/5)\n", i);
        sleep(1);
    }
    rendresem(semid);
}

void gerer_fils(void){
    prendresem(semid);
    for(int i=0;i<=7;i++){
        printf("Je suis le fils (%d/7)\n", i);
        sleep(1);
    }
    rendresem(semid);
    exit(0);
}


int main(int argc, char *argv[]){

    int status;

    semid = creersem_partage(argv);    /* valeur initiale 1 : écran libre */

    switch (fork())
    {
        case -1: perror("fork"); exit(-1);
        case 0:  gerer_fils(); break;
        default: gerer_pere();
                 wait(&status);        /* attendre le fils ... */
                 detruiresem(semid);   /* ... avant de détruire le sémaphore */
    }
    return 0;
}
