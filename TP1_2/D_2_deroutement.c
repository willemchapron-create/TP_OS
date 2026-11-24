/******************************************************************************
 * Fichier           : D_2_deroutement.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 24 novembre 2026
 * Description       : Déroutement des signaux SIGUSR1 et SIGINT vers un même
 *                     handler dans le processus père. Le fils envoie 2000
 *                     signaux SIGUSR1 au père afin d'observer la gestion des
 *                     signaux pendants. Ctrl+Z (SIGTSTP) permet de quitter.
 *****************************************************************************/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define forever for (;;)

// Déclaration des prototypes des fonctions
void gerer_pere(void);
void gerer_fils(void);
void gerer_sig(int sig);

// Déclaration des variables globales
int pidf = 0;
int compteur = 0;

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

// Handler unique pour SIGUSR1, SIGINT et SIGTSTP
void gerer_sig(int sig)
{
	switch (sig) {
	case SIGUSR1:
		compteur++;                 // Incrémente le compteur à chaque SIGUSR1
		signal(SIGUSR1, gerer_sig); // Réarmement du signal
		break;
	case SIGINT:
		printf("\n[PERE] Nombre de signaux SIGUSR1 recus : %d\n", compteur);
		signal(SIGINT, gerer_sig);  // Réarmement du signal
		break;
	case SIGTSTP:
		printf("\n[PERE] Arret du programme.\n");
		exit(0);                    // Quitte proprement sur Ctrl+Z
		break;
	}
}

// PÈRE : Déroute les signaux et attend en boucle
void gerer_pere(void)
{
	signal(SIGUSR1, gerer_sig);
	signal(SIGINT, gerer_sig);
	signal(SIGTSTP, gerer_sig);     // Associe Ctrl+Z à l'arrêt du programme

	printf("[PERE] Pret (PID=%d). Attente des signaux...\n", getpid());

	forever {
		pause(); // Endort le père jusqu'à réception d'un signal
	}
}

// FILS : Envoie 2000 exemplaires de SIGUSR1 au père
void gerer_fils(void)
{
	int i;

	// Laisse le temps au père d'armer ses signaux avant l'envoi
	sleep(1);

	for (i = 0; i < 2000; i++) {
		kill(getppid(), SIGUSR1);
	}

	printf("[FILS] 2000 signaux SIGUSR1 envoyes.\n");
	printf("[FILS] Appuyez sur Ctrl+C pour afficher le compteur (Ctrl+Z pour quitter).\n");
	exit(0);
}