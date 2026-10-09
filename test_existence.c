/******************************************************************************
 * Fichier           : test_existence.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 9 octobre 2026
 * Description       : Utilisation de la primitive kill(pid, 0) pour tester 
 *                     l'existence d'un processus. Utilisation de SIGCHLD 
 *                     ignoré pour empêcher l'état zombie et valider le test.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Prototypes
void gerer_pere(void);
void gerer_fils(void);

// Variable globale pour le PID
int pidf = 0;

int main(void)
{
	// Demande au système de supprimer automatiquement le fils dès sa mort
	signal(SIGCHLD, SIG_IGN);

	switch (pidf = fork()) {
	case -1:
		perror("Erreur fork");
		exit(-1);
	case 0:
		gerer_fils();
		break;
	default:
		gerer_pere();
		break;
	}

	return 0;
}


// PÈRE : Vérifie si le fils existe toujours
void gerer_pere(void)
{
	// Le signal 0 teste l'existence dans la table des processus.
	// Dès que le fils meurt (et disparait grâce à SIG_IGN), kill renvoie -1.
	while (kill(pidf, 0) == 0) {
		printf("[PERE] Le processus fils (PID %d) est present...\n", pidf);
		sleep(1); 
	}

	printf("\n[PERE] Fin du test : le processus n'est plus present.\n");
}

// FILS : Simule un temps de traitement
void gerer_fils(void)
{
	// Le processus s'endort 3 secondes (plus pratique pour du test)
    // à remplacer par 30 pour convenir au cahier des charges.
	sleep(3);
	exit(0);
}