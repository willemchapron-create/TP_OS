/******************************************************************************
 * Fichier           : custom_shell.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 2 octobre 2026
 * Description       : Mini-interpréteur de commandes (shell personnalisé). 
 *                     Il lit des commandes francisées en boucle (dir, quisuisje, 
 *                     ousuisje) et exécute les utilitaires Linux correspondants 
 *                     grâce à la primitive de recouvrement execvp() dans un fils.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TAILLE_MAX 256

int main(void)
{
    char commande_lue[TAILLE_MAX];
    pid_t pid;
    int status;

    while (1) {
        //Affichage du prompt
        printf("New_sh:> ");

        // Saisie de la commande au clavier
        if (fgets(commande_lue, sizeof(commande_lue), stdin) == NULL) {
            printf("\n");
            break;
        }

        // Suppression du caractère saut de ligne '\n' ajouté par fgets
        commande_lue[strcspn(commande_lue, "\n")] = '\0';

        // Tableaux d'arguments pour execvp (se terminant obligatoirement par NULL)
        char *chaine[5] = {NULL};

        // Comparaison avec les commandes francisées demandées
        if (strcmp(commande_lue, "dir") == 0) {
            // "dir" correspond à "ls -l -s"
            chaine[0] = "ls";
            chaine[1] = "-l";
            chaine[2] = "-s";
            chaine[3] = NULL;
        }
        else if (strcmp(commande_lue, "quisuisje") == 0) {
            // "quisuisje" correspond à "whoami"
            chaine[0] = "whoami";
            chaine[1] = NULL;
        }
        else if (strcmp(commande_lue, "ousuisje") == 0) {
            // "ousuisje" correspond à "pwd"
            chaine[0] = "pwd";
            chaine[1] = NULL;
        }
        else {
            printf("Commande '%s' inconnue\n", commande_lue);
            continue;
        }

        //Création du processus fils pour le recouvrement
        pid = fork();

        switch (pid) {
        case -1:
            perror("Erreur fork");
            break;

        case 0:
            execvp(chaine[0], chaine);
        default:
            /* Le père attend la fin du fils avant de réafficher le prompt */
            wait(&status);
            break;
        }
    }

    return 0;
}
