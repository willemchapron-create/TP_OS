/******************************************************************************
 * Fichier           : D_3_synchro_signaux.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 24 novembre 2026
 * Description       : Synchronisation d'un processus père (entiers pairs) et
 *                     d'un processus fils (entiers impairs) entre 1 et 100
 *                     à l'aide des signaux SIGUSR1 et SIGUSR2.
 *****************************************************************************/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// Déclaration des prototypes des fonctions
void gerer_pere(void);
void gerer_fils(void);
void gerer_sig(int sig);

// Variable globale pour le PID du fils
int pidf = 0;

int main(void)
{
	switch (pidf = fork()) {
	case -1:
		printf("Creation du fils impossible\n");
		exit(-1);
		break;
	case 0:
		gerer_fils();
		break;
	default:
		gerer_pere();
		break;
	}

	return 0;
}

// Gestionnaire commun : réarme simplement le signal pour réveiller pause()
void gerer_sig(int sig)
{
	signal(sig, gerer_sig);
}

// FILS : Affiche les entiers impairs (1, 3, ..., 99)
void gerer_fils(void)
{
	int i;

	signal(SIGUSR1, gerer_sig); // Armement du signal envoyé par le père
	sleep(1);                   // Laisse le temps au père d'armer SIGUSR2 et d'entrer en pause()

	for (i = 1; i <= 99; i += 2) {
		printf("%d ", i);
		fflush(stdout);         // Force l'affichage immédiat (évite la mise en tampon)

		usleep(1000);           // Laisse le temps au père d'atteindre pause()
		kill(getppid(), SIGUSR2); // Passe la main au père

		if (i < 99) {
			pause();            // Attend le signal SIGUSR1 du père pour le tour suivant
		}
	}

	exit(0);
}

// PÈRE : Affiche les entiers pairs (2, 4, ..., 100)
void gerer_pere(void)
{
	int i;

	signal(SIGUSR2, gerer_sig); // Armement du signal envoyé par le fils

	for (i = 2; i <= 100; i += 2) {
		pause();                // Attend le signal SIGUSR2 du fils

		printf("%d ", i);
		fflush(stdout);         // Force l'affichage immédiat

		if (i < 100) {
			usleep(1000);       // Laisse le temps au fils d'atteindre pause()
			kill(pidf, SIGUSR1); // Passe la main au fils
		}
	}

	printf("\n");
	wait(NULL);                 // Attend la fin propre du fils
}