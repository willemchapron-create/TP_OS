//sem

/*
	Programme dans lequel un processus père et son fils se passent tour à tour la main à trois reprises.
	Ici la ressource dont l'accés est gardé est le processeur : pendant qu'un processus attend, l'autre peut faire les instructions qu'il souhaite (une pause de 5 secondes)
	Illustre l'initialisation et l'utilisation du sémaphore.

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "./fcts_sem.c"
#define forever for(;;)

// Déclaration des prototypes des fonctions
void gerer_pere(void);
void gerer_fils(void);

// Déclaration des variables globales
int pidf;
int semid;

int main(int argc,char *argv[])
{
int status;

semid=creersem_partage(argv);		// initialisation du sémaphore

switch(pidf=fork())
	{
	case -1:			// fork a renvoyé -1, c'est au père, il n'y a pas de fils car c'est un code d'échec
		{
		printf("Creation du fils impossible\n");
		exit(-1);
		break;
		}
	case 0:			// fork a renvoyé 0 : on est donc dans le processus fils processus fils
		{
		gerer_fils();
		break;
		}
	default:
		{
		gerer_pere(); 	// fork a renvoyé une autre valeur : c'est le PID du fils pour le processus du père
		wait(&status);	// Le père attend la fin du fils pour pouvoir ...
		detruiresem(semid);	// ... Détruire le sémaphore
		}
	}
}

void gerer_pere(void)
{
int i = 0,j;

while (i< 3)	// On va faire 3 boucles
	{
	printf("Le pere attend\n");
	prendresem(semid);	// Opération P sur le sémaphore
	printf("Le pere a pris\n");
	sleep(5); 			// Ne rien faire pendant 5 secondes
	rendresem(semid);	// Opération V sur le sémaphore
	printf("Le pere a libere\n");

	sleep(1); 			// Pour laisser le temps au père de reprendre le contrôle
	++i;
	}
}

void gerer_fils(void)
{
int i = 0,j;

while (i< 3)	// On va faire 3 boucles
	{
	printf("Le fils attend\n");
	prendresem(semid);	// Opération P sur le sémaphore
	printf("Le fils a pris\n");
	sleep(5); 			// Ne rien faire pendant 5 secondes
	rendresem(semid);	// Opération V sur le sémaphore
	printf("Le fils a libere\n");

	sleep(1); 			// Pour laisser le temps au père de reprendre le contrôle
	++i;
	}
exit(2);
}

