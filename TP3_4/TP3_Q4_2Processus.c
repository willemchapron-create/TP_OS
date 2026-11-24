/**********************************************************************************
 * Fichier                  : TP3_Q4.c
 * Auteur                   : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 9 octobre 2026
 * Description              : Programme qui permet de synchroniser les 2 processus 
 *                            via semaphores afin
                              d’afficher les nombres par ordre ascendant ils s'executent 
                              a tour de role en fonction dela parité.
 **********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "./fcts_sem.c"

int s_impair, s_pair;

int main(int argc, char *argv[])
{
    int i, status;

    s_impair = creersem(argv, 1, 0);   /* valeur 1 : le fils commence */
    s_pair   = creersem(argv, 0, 1);   /* valeur 0 : le père attend   */

    if (fork() == 0)                   /* FILS : impairs */
    {
        for (i = 1; i <= 99; i += 2)
        {
            prendresem(s_impair);      /* P : attendre mon tour */
            printf("fils : %d\n", i);
            rendresem(s_pair);         /* V : donner la main au père */
        }
        exit(0);
    }

    for (i = 2; i <= 100; i += 2)      /* PÈRE : pairs */
    {
        prendresem(s_pair);
        printf("pere : %d\n", i);
        rendresem(s_impair);
    }

    wait(&status);
    detruiresem(s_impair);
    detruiresem(s_pair);
    return 0;
}
