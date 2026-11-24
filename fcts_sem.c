//fcts_sem
/*
	Fonctions pour exploiter les fonctions de base des sémaphores
*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

// Déclaration des prototypes des fonctions
int creersem_partage(char *argv[]);
int creersem_synchro(char *argv[]);
int creersem(char *argv[],int valinit, int indice);
void detruiresem(int semid);
void prendresem(int semid);
void rendresem(int semid);
void attendresemnul(int semid);
void incrementersem(int semid);
void decrementersem(int semid);

union semun
	{
	int val;
	struct semid_ds *buf;
	ushort *array;
	};

/*
	Fonctions spécifiques à la gestion des sémaphores
*/

// Fonctions liées à la gestion d'une ressource
void prendresem(int semid)			// Pour réserver une ressource
{
static struct sembuf att_prise={0,-1,0};
if (semop(semid,&att_prise,1)==-1)		// Attente de libération du sémaphore & prise
	perror("Echec semop prendresem ");		
}

void rendresem(int semid)			// Pour libérer la ressource
{
static struct sembuf liberer={0,1,0};
if (semop(semid,&liberer,1)==-1)
	perror("Echec semop rendresem ");		// Libération de la ressource
}

// Fonctions liées à la synchronisation de processus

void attendresemnul(int semid)		// Attente de la nullité du sémaphore
{
static struct sembuf liberer={0,0,0};
if (semop(semid,&liberer,1)==-1)		
	perror("Echec semop attendresemnul ");		
}

void incrementersem(int semid)
{
static struct sembuf liberer={0,1,0};
if (semop(semid,&liberer,1)==-1)
	perror("Echec semop incrementer ");
}

void decrementersem(int semid)
{
static struct sembuf att_prise={0,-1,0};
if (semop(semid,&att_prise,1)==-1)
	perror("Echec semop decrementer ");
}

// Fonctions liées à l'initialisation, la création des sémaphores et la destruction des sémaphores

int creersem(char *argv[],int valinit, int indice)
{
int semid;					// Numéro d'identification du jeu de sémaphores
key_t cle;					// Clé unique du syteme IPC
int semflg = IPC_CREAT | 0666;	// option (création) et permissions
			
int nsems = 1;				// Nombre de sémaphores
union semun arg;

if ((cle=ftok(argv[0],indice))==-1)	// Création de la clé
	{
	perror("Probleme sur ftok ");
	exit(-1);
	}
else printf("creersem: Clé obtenue\n");
printf("creersem: création du sémaphore\n");	

if ((semid = semget(cle, nsems, semflg)) == -1) 
	{
	perror("semget: Echec lors de la création du sémaphore ");
	exit(-1);
	}
else printf("creersem: sémaphore créé, semid=%d\n",semid);

arg.val=valinit;					// Valeur initiale du sémaphore
if (semctl(semid,0,SETVAL,arg)==-1)
	{
	perror("Erreur à l'initialisation ");
	semctl(semid,IPC_RMID,NULL);		// Effacement du sémaphore
	exit(-1);
	}

return(semid);
}

int creersem_partage(char *argv[])
{
int semid=creersem(argv,1,0);
return(semid);
}

int creersem_synchro(char *argv[])
{
int semid=creersem(argv,0,1);
return(semid);
}

void detruiresem(int semid)
{
semctl(semid,IPC_RMID,NULL);		// Effacement du sémaphore
}

