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

// Déclaration des variables globales
int pidf = 0;
int tube[2];

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
	kill(pidf, SIGKILL); // Tue le fils
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

		// Limiter la chaîne à 5 caractères max
		if (strlen(chaine) > 5) {
			chaine[5] = '\0';
		}

		// Envoi des 5 caractères au maximum
		write(tube[1], chaine, strlen(chaine));

		// Micro-pause (50 ms) pour laisser le temps au fils d'afficher
		usleep(50000);
	}
}

void gerer_fils(void)
{
	char buff[6]; // Chaîne de 5 caractères max + '\0'
	int nb_lus;
	close(tube[1]);

	forever
	{
		nb_lus = read(tube[0], buff, 5);
		if (nb_lus > 0) {
			buff[nb_lus] = '\0';
			printf("\t[FILS] Message lu : %s\n", buff);
		}
	}

	close(tube[0]);
	exit(0);
}