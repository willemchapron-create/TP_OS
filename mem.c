/*Essai de Creation, attachement a une memoire
partagee avec un seul processus
mem.c*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

struct snombre
{
char c;
int i;
float f;
};
typedef struct snombre SNOMBRE;
#define MODE_MEMOIRE (SHM_R|SHM_W) /* utilisation en lecture et ecriture*/
int main ( )
{
int shmid; /* identificateur de memoire partagee*/
char *shmptr; /* pointeur sur la memoire partagee */
SNOMBRE *ptr;

/* --- Creation de la Memoire Partagee ---*/

if((shmid = shmget (IPC_PRIVATE, sizeof(SNOMBRE),MODE_MEMOIRE )) < 0)
{
printf("Erreur de Creation de la memoire partagee \n");
exit (0);
}
/* --- Attachement a la Memoire Partagee ---*/

if((shmptr = shmat (shmid, 0,0))== (void *)-1)
{
printf("Erreur d'Attachement a la memoire partagee \n");
exit (0);
}
/* --- Ecriture dans la memoire partagee ---*/
ptr = (SNOMBRE *)shmptr;
ptr->c = 'a';
ptr->i = 2;
ptr->f = 3.5;
/* --- Lecture dans la memoire partagee ---*/
printf("Lecture dans la Memoire Partagee	\n");
printf("Caractere a : %c : Entier 2 : %d : Flottant 3.5	: %f \n",ptr->c, ptr->i, ptr->f);
}
