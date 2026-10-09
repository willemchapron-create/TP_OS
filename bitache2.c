/******************************************************************************
 * Fichier           : bitache2.c
 * Auteur            : CHAPRON Willem
 * Date de création  : 2 octobre 2026
 * Description       : Communication unidirectionnelle de base via un tube 
 *                     anonyme (pipe). Le processus père acquiert une chaîne 
 *                     au clavier et la transmet au fils, qui effectue une 
 *                     lecture bloquante limitée aux 5 premiers caractères.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Déclaration des prototypes des fonctions
void gerer_pere(void);
void gerer_fils(void);

// Déclaration des variables globales
int pidf = 0;
int tube[2]; // tube[0] = lecture, tube[1] = écriture

int main(void)
{
    pipe(tube); // création du pipe

	switch (pidf = fork()) {
	case -1: // Échec du fork
	{
		printf("Création du fils impossible\n");
		exit(-1);
		break;
	}
	case 0: // Processus fils
	{
		gerer_fils();
		break;
	}
	default: // Processus père
		gerer_pere();
		break;
	}

	return 0;
}

void gerer_pere(void)
{
	char chaine[128];

	// Fermeture du pipe de lecture inutile au père
	close(tube[0]);

	printf("[PERE] Saisissez une chaine de caracteres : ");
	if (fgets(chaine, sizeof(chaine), stdin) != NULL) {
		// Suppression du retour '\n'
		chaine[strcspn(chaine, "\n")] = '\0';

		// Écriture de la chaîne dans le tube
		write(tube[1], chaine, strlen(chaine));
	}

	// Fermeture de l'écriture (notifie la fin des données au lecteur)
	close(tube[1]);

	// Attente de la terminaison du fils
	wait(NULL);
}

void gerer_fils(void)
{
	char buff[6]; // 5 caractères max + octet nul '\0'
	int nb_lus;

	// Fermeture du pipe d'écriture inutile au fils
	close(tube[1]);

	// Lecture de 5 caractères maximum
	nb_lus = read(tube[0], buff, 5);
	if (nb_lus > 0) {
		buff[nb_lus] = '\0';
		printf("\t[FILS] Chaine lue dans le tube (%d carac.) : %s\n", nb_lus, buff);
	}

	close(tube[0]);
	exit(0);
}
