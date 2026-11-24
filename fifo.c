
/* lecture d'un message dans un tube nommé */
/* fifo.c  */

#include <fcntl.h> // pour utiliser open
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> // necessaire pour utiliser mkfifo
#include <string.h>

char nom_noeud[]="tubefifo";


int main (void){

  mode_t mode;
  int fd_rd;
  int nb_car;
  char lecture[128]="";
  char chaine[128];

  // creation du tube nomme a partir d un noeud dans le syst de fichier
  mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  // S_IRUSR autorisation de lecture pour le proprietaire du fichier
  // S_IWUSR autorisation d ecriture pour le proprietaire du fichier
  // S_IRGRP autorisation de lecture pour le groupe  du fichier
  // S_IRUSR autorisation de lecture pour tout le monde

  if (mkfifo(nom_noeud,mode) != 0){
    printf("\nimpossible de creer le noeud fifo \n");
    exit(1);
  }
  // ouverture du tube nomme en lecture uniquement
  fd_rd = open(nom_noeud,O_RDONLY);
 
  while (strcmp("fin",lecture) != 0){
    // pour read descripteur, buffer, taille buffer
    if((nb_car = read(fd_rd,lecture,128))<0){   
      printf("\nimpossible de lire le tube \n");
      exit(1);
    }
    lecture[nb_car]='\0';
       printf("\nrecu = %s\n",lecture);
       fflush(stdout);
  }
  close(fd_rd);
    unlink(nom_noeud);
    return 0;
  }





