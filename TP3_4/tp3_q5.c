/******************************************************************************
 * Fichier           : filtre_sem.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Description       : Sur base de filtre.c, le père lit la chaîne entière, envoie lettres et chiffres
 *                     aux fils par tubes, puis leur DEMANDE les résultats
 *                     (SIGUSR1). Un sémaphore garantit que les fils ont fini
 *                     de traiter les données avant la demande.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "./fcts_filt.h"
#include "./fcts_sem.c"

// Prototypes
void gerer_pere(void);
void gerer_fils1(void);
void gerer_fils2(void);
void envoyer_lettres(int sig);
void envoyer_somme(int sig);

// PID, tubes et sémaphore
int pidf1, pidf2;
int t_p_f1[2], t_f1_p[2];
int t_p_f2[2], t_f2_p[2];
int semid;          // sémaphore de synchro : nombre de fils ayant fini

// Résultats des fils (globaux pour être accessibles dans les gestionnaires)
char uniques[128];
int  nb_uniques = 0;
int  somme = 0;

int main(int argc, char *argv[])
{
	semid = creersem_synchro(argv);     // valeur initiale 0

	pipe(t_p_f1); pipe(t_f1_p);
	pipe(t_p_f2); pipe(t_f2_p);

	switch (pidf1 = fork()) {
	case 0:
		gerer_fils1();
		break;
	default:
		switch (pidf2 = fork()) {
		case 0:
			gerer_fils2();
			break;
		default:
			gerer_pere();
			break;
		}
		break;
	}
	return 0;
}

// PÈRE
void gerer_pere(void)
{
	int c;
	char buf_lettres[128] = {0};
	char buf_chiffres[128] = {0};
	int idx_l = 0, idx_c = 0;
	char res_lettres[128] = {0};
	int res_somme = 0;

	close(t_p_f1[0]); close(t_p_f2[0]);
	close(t_f1_p[1]); close(t_f2_p[1]);

	printf("[PERE] Entrez une chaine puis validez par Entree :\n");

	// 1. Lecture de la chaîne entière et tri
	while ((c = getchar()) != '\n' && c != EOF) {
		if (est_une_lettre(c))       buf_lettres[idx_l++] = c;
		else if (est_un_chiffre(c))  buf_chiffres[idx_c++] = c;
	}

	// 2. Envoi aux fils
	write(t_p_f1[1], buf_lettres, idx_l + 1);
	write(t_p_f2[1], buf_chiffres, idx_c + 1);

	// 3. Attente de la fin du traitement des DEUX fils
	prendresem(semid);    // P : bloque tant qu'aucun fils n'a fini
	prendresem(semid);    // P : bloque tant que le 2e fils n'a pas fini

	// 4. Demande des résultats
	kill(pidf1, SIGUSR1);
	kill(pidf2, SIGUSR1);

	// 5. Récupération et affichage
	read(t_f1_p[0], res_lettres, sizeof(res_lettres));
	read(t_f2_p[0], &res_somme, sizeof(int));

	printf("\n[PERE] Lettres uniques : %s\n", res_lettres);
	printf("[PERE] Somme des chiffres : %d\n", res_somme);

	wait(NULL); wait(NULL);
	detruiresem(semid);
}

// FILS 1 : lettres uniques
void gerer_fils1(void)
{
	char recu[128] = {0};

	signal(SIGUSR1, envoyer_lettres);   // armé AVANT le V

	close(t_p_f1[1]); close(t_f1_p[0]);
	close(t_p_f2[0]); close(t_p_f2[1]);
	close(t_f2_p[0]); close(t_f2_p[1]);

	read(t_p_f1[0], recu, sizeof(recu));
	sleep(1);                           // simule un traitement long

	for (int i = 0; recu[i] != '\0'; i++) {
		int existe = 0;
		for (int j = 0; j < nb_uniques; j++)
			if (uniques[j] == recu[i]) { existe = 1; break; }
		if (!existe) uniques[nb_uniques++] = recu[i];
	}
	uniques[nb_uniques] = '\0';

	rendresem(semid);                   // V : traitement terminé
	while (1) pause();                  // attente de la demande du père
}

void envoyer_lettres(int sig)
{
	write(t_f1_p[1], uniques, nb_uniques + 1);
	exit(0);
}

// FILS 2 : somme des chiffres
void gerer_fils2(void)
{
	char recu[128] = {0};

	signal(SIGUSR1, envoyer_somme);

	close(t_p_f2[1]); close(t_f2_p[0]);
	close(t_p_f1[0]); close(t_p_f1[1]);
	close(t_f1_p[0]); close(t_f1_p[1]);

	read(t_p_f2[0], recu, sizeof(recu));
	sleep(1);                           // simule un traitement long

	for (int i = 0; recu[i] != '\0'; i++)
		somme += recu[i] - '0';

	rendresem(semid);                   // V : traitement terminé
	while (1) pause();
}

void envoyer_somme(int sig)
{
	write(t_f2_p[1], &somme, sizeof(int));
	exit(0);
}