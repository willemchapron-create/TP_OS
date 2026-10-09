/******************************************************************************
 * Fichier           : tache.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 2 octobre 2026
 * Description       : Programme de base affichant les identifiants système 
 *                     du processus (PID, PPID, UID, GID) avant d'entrer dans 
 *                     une boucle infinie. Idéal pour observer son exécution 
 *                     en arrière-plan ou tester l'envoi de signaux.
 *****************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void main(void)
{
    printf("PID  : %d\n", getpid());
    printf("PPID : %d\n", getppid());
    printf("UID  : %d\n", getuid());
    printf("GID  : %d\n", getgid());

    /* Boucle infinie */
    while (1) {
        sleep(1);
    }
}
