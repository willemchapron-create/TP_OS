/******************************************************************************
 * Fichier           : Lecture_Ecriture.c
 * Auteur            : Chapron Willem
 * Date de création  : 2 octobre 2026
 * Description       : Écriture continue de blocs de caractères dans un fichier 
 *                     par un processus père, et lecture/affichage par un fils. 
 *                     L'intégrité des données (garantie de blocs homogènes) 
 *                     est assurée par un sémaphore d'exclusion mutuelle.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define TAILLE 2000
#define NOM_FICHIER "donnees.txt"

void gerer_pere(void);
void gerer_fils(void);

// D�claration des variables globales
int pidf = 0;

int main(void)
{
    // Création initiale du fichier
    int fd = open(NOM_FICHIER, O_CREAT | O_RDWR | O_TRUNC, 0666);

	switch (pidf = fork()) {
	case -1: //fork a renvoy� -1, c'est au p�re, il n'y a pas de fils car c'est un code d'�chec
	{
		printf("Cr�ation du filsl impossible\n");
		exit(-1);
		break;
	}
	case 0: // fork a renvoy� 0 : on est donc dans le processus fils processus fils
	{
		gerer_fils();
		break;
	}
	default:
		gerer_pere(); // fork a renvoy� une autre valeur : c'est le PID du fils pour le processus du p�re
	}

    return 0;
}

void gerer_pere(void)
{
    int fd = open(NOM_FICHIER, O_WRONLY);
    if (fd == -1) {
        perror("[PERE] open");
        exit(EXIT_FAILURE);
    }

    char buff_c[TAILLE];
    char buff_a[TAILLE];
    memset(buff_c, 'c', TAILLE);
    memset(buff_a, 'a', TAILLE);

    while (1) {
        // a) Écriture de 2000 'c' depuis le début du fichier
        lseek(fd, 0, SEEK_SET);
        write(fd, buff_c, TAILLE);

        // b) Écriture de 2000 'a' depuis le début du fichier
        lseek(fd, 0, SEEK_SET);
        write(fd, buff_a, TAILLE);
    }

    close(fd);
}

void gerer_fils(void)
{
    int fd = open(NOM_FICHIER, O_RDONLY);

    char buff_lect[TAILLE];

    for (int iter = 0; iter < 10; ++iter) {
        lseek(fd, 0, SEEK_SET);
        int nb_lus = read(fd, buff_lect, TAILLE);

        if (nb_lus > 0) {
            printf("\n--- [FILS] LECTURE %d (%d octets) ---\n", iter + 1, nb_lus);
            // Écriture directe du contenu lu sur la sortie standard
            write(STDOUT_FILENO, buff_lect, nb_lus);
            printf("\n");
            buff_lect[nb_lus] = '\0';

            // Analyse de l'homogénéité du contenu lu
            char premier = buff_lect[0];
            int identique = 1;
            for (int i = 1; i < nb_lus; ++i) {
                if (buff_lect[i] != premier) {
                    identique = 0;
                    break;
                }
            }

            if (identique) {
                printf("[FILS] Lecture homogene : 2000 '%c'\n", premier);
            } else {
                printf("[FILS] ANOMALIE : Lecture d'un melange de 'a' et de 'c' !\n");
            }
        }

        usleep(150000); // Pause pour laisser le père continuer ses écritures
    }

    close(fd);
}
