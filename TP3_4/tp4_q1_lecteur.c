/******************************************************************************
 * Fichier           : lecteur.c
 * Auteur            : CHAPRON Willem & CAZALS Gauthier
 * Date de création  : 26 novembre 2026
 * Description       : Lecteur d'un tube nommé (FIFO). Crée le nœud "tubefifo"
 *                     s'il n'existe pas, l'ouvre en lecture (bloquant jusqu'à
 *                     la connexion d'un écrivain), puis affiche chaque message
 *                     reçu. S'arrête à la réception de "fin" ou à la fermeture
 *                     du tube par l'écrivain, puis supprime le nœud.
 *                     À lancer dans un terminal, avec ecrivain dans un autre.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // strcmp
#include <unistd.h>     // read, close, unlink
#include <fcntl.h>      // open, O_RDONLY
#include <errno.h>      // errno, EEXIST
#include <sys/stat.h>   // mkfifo

// Nom du tube nommé : même nom dans le lecteur et dans l'écrivain
char nom_noeud[] = "tubefifo";

int main(void)
{
	char lecture[128];  // tampon de réception
	int fd_rd;          // descripteur du tube ouvert en lecture
	int nb_car;         // nombre d'octets lus par read

	// 1. Création du nœud FIFO dans le système de fichiers (droits rw-r--r--)
	//    Si le tube existe déjà (EEXIST), ce n'est pas une erreur : on le réutilise
	if (mkfifo(nom_noeud, 0644) != 0 && errno != EEXIST)
	{
		perror("mkfifo");
		exit(1);
	}

	// 2. Ouverture en lecture seule
	//    open est BLOQUANT tant qu'aucun écrivain n'a ouvert le tube
	printf("[LECTEUR] en attente d'un ecrivain...\n");
	fd_rd = open(nom_noeud, O_RDONLY);
	printf("[LECTEUR] ecrivain connecte\n");

	// 3. Boucle de lecture
	//    read bloque tant que le tube est vide
	//    read renvoie 0 quand l'écrivain a fermé le tube (fin de fichier)
	//    On lit 127 octets au maximum pour garder une place pour le '\0'
	while ((nb_car = read(fd_rd, lecture, 127)) > 0)
	{
		lecture[nb_car] = '\0';                  // transforme les octets lus en chaîne C
		printf("[LECTEUR] recu = %s\n", lecture);

		if (strcmp(lecture, "fin") == 0)         // message de fin envoyé par l'écrivain
			break;
	}

	// 4. Fermeture du descripteur et suppression du nœud du système de fichiers
	close(fd_rd);
	unlink(nom_noeud);
	return 0;
}