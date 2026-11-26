/******************************************************************************
 * Fichier           : ecrivain_shm.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 26 novembre 2026
 * Description       : Écrivain par mémoire partagée. Se rattache au segment
 *                     partagé et au sémaphore créés par lecteur_shm (mêmes
 *                     clés ftok), écrit dans la mémoire un caractère, un entier
 *                     et un réel saisis au clavier, puis signale au lecteur
 *                     que la donnée est disponible (V sur le sémaphore).
 *                     À lancer après lecteur_shm, dans le même dossier.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>      // ftok
#include <sys/shm.h>      // shmget, shmat, shmdt
#include <sys/sem.h>      // semget
#include "./fcts_sem.c"   // rendresem

// Structure des données échangées : identique dans le lecteur
typedef struct
{
	char  c;
	int   i;
	float f;
} SNOMBRE;

int main(void)
{
	int shmid;        // identificateur du segment
	int semid;        // identificateur du sémaphore
	SNOMBRE *ptr;     // pointeur sur la mémoire partagée

	// 1. Rattachement au segment et au sémaphore EXISTANTS (pas d'IPC_CREAT)
	//    Mêmes clés que le lecteur : ftok(".", 'M') et ftok(".", 'S')
	//    Échec si le lecteur n'est pas lancé, ou s'il tourne dans un autre dossier
	//    Le sémaphore n'est PAS réinitialisé : on risquerait d'effacer un V
	if ((shmid = shmget(ftok(".", 'M'), sizeof(SNOMBRE), 0666)) < 0 ||
	    (semid = semget(ftok(".", 'S'), 1, 0666)) < 0)
	{
		printf("Lancez d'abord ./lecteur_shm\n");
		exit(1);
	}

	// 2. Attachement au segment
	ptr = (SNOMBRE *) shmat(shmid, 0, 0);

	// 3. Saisie au clavier écrite DIRECTEMENT dans la mémoire partagée
	//    L'espace avant %c ignore les blancs et '\n' restés dans le tampon
	printf("[ECRIVAIN] caractere entier reel : ");
	scanf(" %c %d %f", &ptr->c, &ptr->i, &ptr->f);

	// 4. V : signale au lecteur que les données sont prêtes (sémaphore 0 -> 1)
	rendresem(semid);

	// 5. Détachement ; c'est le lecteur qui supprimera le segment et le sémaphore
	shmdt(ptr);
	return 0;
}