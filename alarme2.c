/******************************************************************************
 * Fichier           : alarme2.c
 * Auteur            : CHAPRON Willem
 * Date de création  : 9 octobre 2026
 * Description       : Chien de garde avec traitement asynchrone. Le fils 
 *                     exécute un calcul continu en tâche de fond. Le père lit 
 *                     une entrée chronométrée (5s), la place dans un tube, puis 
 *                     interrompt le fils avec SIGUSR1 pour déclencher la lecture.
 *****************************************************************************/
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define forever for (;;)

// Déclaration des prototypes des fonctions
void gerer_pere(void);
void gerer_fils(void);
void gerer_alarme(int sig);
void gerer_sigusr1(int sig);

// Déclaration des variables globales
int pidf = 0;
int tube[2];
volatile unsigned long compteur_fils = 0;

int main(void)
{
	pipe(tube);

	switch (pidf = fork()) {
	case -1:
	{
		printf("Création du fils impossible\n");
		exit(-1);
		break;
	}
	case 0:
	{
		gerer_fils();
		break;
	}
	default:
		gerer_pere();
		break;
	}

	return 0;
}

void gerer_alarme(int sig)
{
	printf("\ndélai expiré\n");
	kill(pidf, SIGKILL);
	close(tube[1]);
	exit(-1);
}

void gerer_pere(void)
{
	char chaine[128];
	close(tube[0]);

	signal(SIGALRM, gerer_alarme);

	forever
	{
		alarm(5);

		printf("Votre chaine SVP : \n");
		scanf("%s", chaine);

		alarm(0);

		// Le père transmet l'intégralité de la chaîne tapée
		write(tube[1], chaine, strlen(chaine));

		// Notification au fils que la donnée est prête dans le tube
		kill(pidf, SIGUSR1);

		// Laisse le temps au fils de traiter et d'afficher avant le prochain prompt
		usleep(100000);
	}
}

void gerer_sigusr1(int sig)
{
    signal(SIGUSR1, gerer_sigusr1);

    char buff[128];
    int nb_lus = read(tube[0], buff, sizeof(buff) - 1);
    if (nb_lus > 0) {
        if (nb_lus > 5) nb_lus = 5;
        buff[nb_lus] = '\0';
        printf("\t[FILS] Message lu : %s | Calculs effectués : %lu\n", buff, compteur_fils);
    }
}

void gerer_fils(void)
{
    close(tube[1]);
    signal(SIGUSR1, gerer_sigusr1);

    // Le fils travaille en permanence sans jamais bloquer
    forever {
        compteur_fils++;
    }
}
