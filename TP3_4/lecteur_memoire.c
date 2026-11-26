/******************************************************************************
 * Fichier           : lecteur_shm.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 26 novembre 2026
 * Description       : Lecteur par mémoire partagée. Crée le segment partagé
 *                     (structure char/int/float) et un sémaphore de
 *                     synchronisation initialisé à 0, à partir de clés ftok
 *                     communes. Attend (P) que l'écrivain ait rempli la
 *                     mémoire, affiche son contenu, puis supprime le segment
 *                     et le sémaphore.
 *                     À lancer EN PREMIER, avant ecrivain_shm.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>      // ftok, IPC_CREAT, IPC_RMID
#include <sys/shm.h>      // shmget, shmat, shmdt, shmctl
#include "./fcts_sem.c"   // creersem, prendresem, detruiresem

// Structure des données échangées : identique dans l'écrivain
typedef struct
{
	char  c;
	int   i;
	float f;
} SNOMBRE;

int main(void)
{
	// creersem calcule sa clé avec ftok(argv[0], indice) : on lui passe "."
	// (répertoire courant) pour obtenir la même clé que dans l'écrivain
	char *fichier_cle[] = { "." };
	int shmid;        // identificateur du segment
	int semid;        // identificateur du sémaphore
	SNOMBRE *ptr;     // pointeur sur la mémoire partagée

	// 1. Création du segment avec une clé commune : ftok(".", 'M')
	//    IPC_PRIVATE est impossible ici : les 2 programmes n'ont pas de lien de parenté
	shmid = shmget(ftok(".", 'M'), sizeof(SNOMBRE), IPC_CREAT | 0666);
	if (shmid < 0)
	{
		perror("shmget");
		exit(1);
	}

	// 2. Création du sémaphore de synchronisation, clé ftok(".", 'S'), valeur 0
	//    C'est le lecteur qui l'initialise : il doit donc être lancé en premier
	semid = creersem(fichier_cle, 0, 'S');

	// 3. Attachement au segment
	ptr = (SNOMBRE *) shmat(shmid, 0, 0);

	// 4. P : bloque jusqu'à ce que l'écrivain ait rempli la mémoire
	printf("[LECTEUR] en attente de l'ecrivain...\n");
	prendresem(semid);

	// 5. Lecture des données écrites par l'écrivain
	printf("[LECTEUR] lu : c = %c, i = %d, f = %f\n", ptr->c, ptr->i, ptr->f);

	// 6. Nettoyage : détachement, puis suppression du segment et du sémaphore
	//    (sinon ils restent dans le système, visibles avec ipcs)
	shmdt(ptr);
	shmctl(shmid, IPC_RMID, NULL);
	detruiresem(semid);
	return 0;
}