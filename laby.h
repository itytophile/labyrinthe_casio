#ifndef _LABY_H
#define _LABY_H

struct Laby {
	unsigned char *grille; //On a pas besoin de beaucoup de place 4 bits suffisent
	unsigned int tx; //Largeur du labyrinthe
	unsigned int ty; //Hauteur du labyrinthe
};

int murGauche(unsigned char c);
int murDroite(unsigned char c);
int murHaut(unsigned char c);
int murBas(unsigned char c);
void initLaby(struct Laby *l, unsigned int tx, unsigned int ty);
void sentier(struct Laby *l, int entree, int sortie, int m);
void diversion(struct Laby *l, int entree, int sortie1, int sortie2);
void nettoyage(struct Laby *l, int entree);
void dedale(struct Laby *l);
void pause();
void renduLaby(struct Laby *l);

#endif