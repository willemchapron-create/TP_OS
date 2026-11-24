//fcts_filt

/*
fichier de bibliotheque pour le filtrage des caracteres
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int est_un_chiffre(int c);
int est_une_lettre(int c);
void init_tlettres(void);
int gerer_tlettres(int c);
int taille_tlettres(void);

char tlettres[53];

int est_un_chiffre(int c)
{
	if ('0' <= c && c <= '9') // Est-ce un chiffre ?
		return (1);
	else
		return (0);
}

int est_une_lettre(int c)
{
	if (('a' <= c && c <= 'z') ||
	    ('A' <= c && c <= 'Z')) // Est-ce une lettre ?
		return (1);
	else
		return (0);
}

void init_tlettres(void)
{
	int i;
	for (i = 0; i < 53; i++)
		tlettres[i] = '\0';
}

int gerer_tlettres(int c)
{
	int dlettre = 0;
	char lettre;
	dlettre = taille_tlettres();
	if (strchr(tlettres, c) == NULL) {
		//printf("La lettre n'est  pas dans le tableau\n");
		sprintf(&lettre, "%c", c);
		tlettres[dlettre] = lettre;
		return (1);
	} else {
		return (0);
	}
}

int taille_tlettres(void)
{
	return (strlen(tlettres));
}
