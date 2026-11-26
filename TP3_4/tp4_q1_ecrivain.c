/******************************************************************************
 * Fichier           : ecrivain.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 26 novembre 2026
 * Description       : Écrivain d'un tube nommé (FIFO). Crée le nœud "tubefifo"
 *                     s'il n'existe pas, l'ouvre en écriture (bloquant jusqu'à
 *                     la connexion d'un lecteur), puis envoie dans le tube
 *                     chaque ligne saisie au clavier. La saisie de "fin"
 *                     termine l'échange.
 *                     À lancer dans un terminal, avec lecteur dans un autre.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // strlen, strcmp, strcspn
#include <unistd.h>     // write, close
#include <fcntl.h>      // open, O_WRONLY
#include <errno.h>      // errno, EEXIST
#include <sys/stat.h>   // mkfifo

// Nom du tube nommé : doit être identique à celui du lecteur
char nom_noeud[] = "tubefifo";

int main(void)
{
	char chaine[128];   // message saisi au clavier
	int fd_wr;          // descripteur du tube ouvert en écriture

	// 1. Création du nœud FIFO s'il n'existe pas encore
	//    Permet de lancer lecteur et écrivain dans n'importe quel ordre
	if (mkfifo(nom_noeud, 0644) != 0 && errno != EEXIST)
	{
		perror("mkfifo");
		exit(1);
	}

	// 2. Ouverture en écriture seule
	//    open est BLOQUANT tant qu'aucun lecteur n'a ouvert le tube
	printf("[ECRIVAIN] en attente d'un lecteur...\n");
	fd_wr = open(nom_noeud, O_WRONLY);

	// 3. Boucle d'envoi des messages saisis au clavier
	do {
		printf("[ECRIVAIN] message (fin pour quitter) : ");
		fgets(chaine, sizeof(chaine), stdin);   // lecture d'une ligne (avec le '\n')
		chaine[strcspn(chaine, "\n")] = '\0';   // suppression du '\n' final

		// Envoi du message dans le tube (sans le '\0', le lecteur le rajoute)
		// Si le lecteur a disparu, write provoque le signal SIGPIPE
		write(fd_wr, chaine, strlen(chaine));

	} while (strcmp(chaine, "fin") != 0);        // "fin" termine l'échange

	// 4. Fermeture du tube : le read du lecteur renverra alors 0 (fin de fichier)
	//    C'est le lecteur qui supprime le nœud avec unlink
	close(fd_wr);
	return 0;
}