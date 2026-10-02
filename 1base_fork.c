//base_fork
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define forever for (;;)

// D�claration des prototypes des fonctions
void gerer_pere(void);
void gerer_fils(void);

// D�claration des variables globales
int pidf = 0;
int var_global = 50;

int main(void)
{
	printf("PID de la fonction main=%d\n", getpid());

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
}

void gerer_pere(void)
{
	int var_locale=0;
	forever {
		var_locale++;
		var_global++;
		printf ("Pere actif: PID= %d, var_global=%d, var_locale=%d\n",getpid(),var_global, var_locale);

		sleep(2);
	}
}

void gerer_fils(void)
{
	int var_locale=0;
	forever {
		var_locale += 2;
		var_global--;
		printf ("Fils actif: PID= %d, Fils PPID = %d, var_global=%d, var_locale=%d\n",getpid(),getppid(),var_global, var_locale);
		
		sleep(2);
	}
}
