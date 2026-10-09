/******************************************************************************
 * Fichier           : filtre.c
 * Auteur            : CHAPRON WIllem & CAZALS Gauthier
 * Date de création  : 9 octobre 2026
 * Description       : Système de filtrage concurrent utilisant 4 tubes (pipes). 
 *                     Le processus père lit une saisie, sépare les lettres et 
 *                     les chiffres, puis les transmet en bloc à deux processus fils. 
 *                     Le fils 1 isole les lettres uniques, le fils 2 calcule la 
 *                     somme des chiffres, puis renvoient leurs résultats au père.
 *****************************************************************************/
#include "./fcts_filt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Prototypes
void gerer_pere(void);
void gerer_fils1(void);
void gerer_fils2(void);

// PID et tubes
int pidf1, pidf2;
int t_p_f1[2], t_f1_p[2];
int t_p_f2[2], t_f2_p[2];

int main(void)
{
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
	char c;
	char buf_lettres[128] = {0};
	char buf_chiffres[128] = {0};
	int idx_l = 0, idx_c = 0;
	
	char res_lettres[128] = {0};
	int res_somme = 0;

	close(t_p_f1[0]); close(t_p_f2[0]);
	close(t_f1_p[1]); close(t_f2_p[1]);

	printf("[PERE] Entrez une chaine puis validez par Entree :\n");

	// 1. Le père lit tout et sépare dans deux tableaux distincts
	while ((c = getchar()) != '\n' && c != EOF) {
		if (est_une_lettre(c)) {
			buf_lettres[idx_l++] = c;
		} else if (est_un_chiffre(c)) {
			buf_chiffres[idx_c++] = c;
		}
	}

	// 2. Envoi global en une seule fois aux deux fils (+1 pour le \0 final)
	write(t_p_f1[1], buf_lettres, idx_l + 1);
	write(t_p_f2[1], buf_chiffres, idx_c + 1);

	// 3. Récupération globale des résultats
	read(t_f1_p[0], res_lettres, sizeof(res_lettres));
	read(t_f2_p[0], &res_somme, sizeof(int));

	printf("\n[PERE] Lettres uniques : %s\n", res_lettres);
	printf("[PERE] Somme des chiffres : %d\n", res_somme);

	wait(NULL); wait(NULL);
}


// FILS 1 : Extrait les lettres uniques 
void gerer_fils1(void)
{
	char recu[128] = {0};
	char uniques[128] = {0};
	int nb_uniques = 0;

	close(t_p_f1[1]); close(t_f1_p[0]);
	close(t_p_f2[0]); close(t_p_f2[1]);
	close(t_f2_p[0]); close(t_f2_p[1]);

	// Lecture en une seule fois du tableau envoyé par le père
	read(t_p_f1[0], recu, sizeof(recu));

	// Parcours classique pour stocker les lettres sans doublon (contourne les bugs du TP)
	for (int i = 0; recu[i] != '\0'; i++) {
		int existe = 0;
		for (int j = 0; j < nb_uniques; j++) {
			if (uniques[j] == recu[i]) {
				existe = 1; 
				break;
			}
		}
		if (!existe) {
			uniques[nb_uniques++] = recu[i];
		}
	}

	// Renvoi direct du résultat final
	write(t_f1_p[1], uniques, nb_uniques + 1);
	exit(0);
}
S
// FILS 2 : Somme des chiffres
void gerer_fils2(void)
{
	char recu[128] = {0};
	int somme = 0;

	close(t_p_f2[1]); close(t_f2_p[0]);
	close(t_p_f1[0]); close(t_p_f1[1]);
	close(t_f1_p[0]); close(t_f1_p[1]);

	// Lecture globale
	read(t_p_f2[0], recu, sizeof(recu));

	// Addition de chaque caractère
	for (int i = 0; recu[i] != '\0'; i++) {
		somme += (recu[i] - '0');
	}

	write(t_f2_p[1], &somme, sizeof(int));
	exit(0);
}
