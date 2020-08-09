#include <fxlib.h>
#include "laby.h"
#include "simpleTime.h"
#include "MonochromeLib.h"

#define CHEMIN 15
#define GAUCHE 8
#define DROITE 4
#define HAUT 2
#define BAS 1
#define TAILLECASE 4

/*
1000 = mur gauche
0100 = 100 = mur droite
0010 = 10 = mur haut
0001 = 1 = mur bas
exemple :
1001 = mur gauche + mur bas

1111 = case reservee pour le chemin entree-sortie (c'est d'abord des cases cloisonnees)
donc pour les define on a converti en base 10 on aura plus qu'a additionner pour avoir les configurations murales
*/

int murGauche(unsigned char c) { //Pour savoir si un des murs est present
	return c&GAUCHE;
}
int murDroite(unsigned char c) {
	return c&DROITE;
}
int murHaut(unsigned char c) {
	return c&HAUT;
}
int murBas(unsigned char c) {
	return c&BAS;
}

void initLaby(struct Laby *l, unsigned int tx, unsigned int ty) { //initialise un labyrinthe vide dans *l
	l->tx = tx;
	l->ty = ty;
	l->grille = (unsigned char *)calloc(tx*ty, sizeof(char));
}

void sentier(struct Laby *l, int entree, int sortie, int m) { //les arguments sont des coordonnees sous forme c = x+tx*y donc pour obtenir x on fait c%tx et pour y on fait c/tx
	int tx = l->tx;
	int ty = l->ty/2;
	int x, y;
	unsigned char *grille = l->grille;
	if(!m) grille+=ty*tx;
	int loto;
	grille[entree] = CHEMIN - GAUCHE; //la premiere case chemin est l'entree mais on peu deja lui casser le mur gauche
	while(entree != sortie) {
		x = entree%tx;
		y = entree/tx;
		if(x == tx-1) { //si on est au bord droite
			if(!y || grille[entree-tx] || sortie/tx > entree/tx) grille[entree+=tx] = CHEMIN; //on affecte directement pour economiser des lignes
			else if(y == ty-1 || grille[entree+tx] || sortie/tx < entree/tx) grille[entree-=tx] = CHEMIN;
			else grille[entree+=(2*(rand()%2)-1)*tx] = CHEMIN; //2*1-1 = 1 et 2*0-1 = -1
		}
		else if(!y || grille[entree-tx]) { //si on peut pas aller en haut
			if(y == ty-1 || grille[entree+tx]) grille[++entree] = CHEMIN; //condition rencontree que si on a un CHEMIN au dessus de nous OU un CHEMIN en dessous de nous
			else { loto = rand()%2; grille[entree+= loto+(!loto)*tx] = CHEMIN; } //si loto=0 on va en bas sinon on va a droite
		}
		else if(y == ty-1 || grille[entree+tx]) { //si on peut pas aller en bas
			loto = rand()%2; //les autres cas limitants ont ete rencontres au dessus
			grille[entree+= loto-(!loto)*tx] = CHEMIN; //si loto=0 on va en haut sinon on va a droite
		}
		else { //si tout va bien
			loto = rand()%3; //soit on va a droite, en bas, en haut
			if(!loto) grille[++entree] = CHEMIN;
			else if(loto == 1) grille[entree+=tx] = CHEMIN;
			else grille[entree-=tx] = CHEMIN;
		}
		//on casse les murs pour rapprocher les hommes
		if(entree%tx > x) {
			grille[x + y*tx] -= DROITE;
			grille[entree] -= GAUCHE;
		}
		else if(entree/tx > y) {
			grille[x + y*tx] -= BAS;
			grille[entree] -= HAUT;
		}
		else {
			grille[x + y*tx] -= HAUT;
			grille[entree] -= BAS;
		}
	}
	grille[sortie] -= DROITE; //on retire le mur pour que la sortie soit une vraie sortie
}

void diversion(struct Laby *l, int entree, int sortie1, int sortie2) {
	int tx = l->tx, ty = l->ty;
	unsigned char *grille = l->grille;
	//On cree des faux chemins
	int i = 0, loto, loto2, loto3, mur;
	while(i < tx*ty) {
		//on cherche une case vide
		if(grille[i]) ++i;
		else {
			//on va toujours faire des cases a 3 ou 2 murs
			mur = rand()%6; //si mur est vrai alors il y aura un 3eme mur (faut pas laisser beaucoup de chances pour deux murs sinon le laby est trop ouvert)
			loto = rand()%4;
			grille[i] = GAUCHE*(!loto)+DROITE*(loto == 1)+BAS*(loto == 2)+HAUT*(loto == 3);
			while((loto2 = rand()%4) == loto); //on refait un choix aleatoire mais pour un autre mur
			grille[i] += GAUCHE*(!loto2)+DROITE*(loto2 == 1)+BAS*(loto2 == 2)+HAUT*(loto2 == 3);
			if(mur) {
				do {
					loto3 = rand()%4;
				} while(loto3 == loto2 || loto3 == loto);
				grille[i] += GAUCHE*(!loto3)+DROITE*(loto3 == 1)+BAS*(loto3 == 2)+HAUT*(loto3 == 3);
			}
		}
	}
	//On entoure le labyrinthe de mur
	for(int i = 0; i < ty; ++i) {
		if(i*tx!=entree) grille[i*tx] |= GAUCHE; //on verifie que ce n'est pas l'entree et que le mur est absent
		if(tx*(i+1)-1!=sortie1 && tx*(i+1)-1!=sortie2) grille[tx*(i+1)-1] |= DROITE; //(j'ai factorise)
	}
	for(int i = 0; i < tx; ++i) {
		grille[i] |= HAUT;
		grille[i+tx*(ty-1)] |= BAS; 
	}
}

void nettoyage(struct Laby *l, int entree) { //supprimer les murs uniques crees par diversion()
	int tx = l->tx, ty = l->ty;
	unsigned char *grille = l->grille;
	for(int i = entree; i < ty/2; ++i) if(murBas(grille[i*tx])) grille[i*tx] -= BAS;
	for(int i = 0; i < tx*ty; ++i) {
		//suppression mur gauche
		if(i%tx && murGauche(grille[i]) && !murDroite(grille[i-1])) grille[i] -= GAUCHE; //si on a un mur gauche mais que le voisin n'a pas de mur droite
		//suppression mur droite
		if(i%tx != tx-1 && murDroite(grille[i]) && !murGauche(grille[i+1])) grille[i] -= DROITE;
		//suppression mur haut
		if(i/tx && murHaut(grille[i]) && !murBas(grille[i-tx])) grille[i] -= HAUT;
		//suppression mur bas
		if(i/tx != ty-1 && murBas(grille[i]) && !murHaut(grille[i+tx])) grille[i] -= BAS;
	}
}

void dedale(struct Laby *l) { //apparition des murs
	int tx = l->tx, ty = l->ty;
	int entree = rand()%(ty/2); //l'entree sera a gauche du labyrinthe seule sa hauteur est aleatoire
	int sortie1 = rand()%(ty/2); //la sortie sera a droite
	int sortie2 = rand()%(ty/2);
	sentier(l, entree*tx, tx-1 + sortie1*tx, 1);
	sentier(l, 0, tx-1 + sortie2*tx, 0); //cree le chemin
	diversion(l, entree*tx, tx-1 + sortie1*tx, tx-1 + (ty/2+sortie2)*tx);
	nettoyage(l, entree);
}

void renduLaby(struct Laby *l) {
	int x, y;
	int tx = l->tx, ty = l->ty;
	unsigned char *grille = l->grille;
	for(int i = 0; i < tx*ty; ++i) {
		if(murBas(grille[i])) { //Si il y a un mur bas 0001
			x = (i%tx)*TAILLECASE;
			y = (i/tx+1)*TAILLECASE-1;
			ML_line(x,y,x+TAILLECASE,y,ML_BLACK);
		}
		if(murHaut(grille[i])) { //Si il y a un mur haut 0010
			x = (i%tx)*TAILLECASE;
			y = (i/tx)*TAILLECASE;
			ML_line(x,y,x+TAILLECASE,y,ML_BLACK);
		}
		if(murDroite(grille[i])) { //Si il y a un mur a droite 0100
			x = (i%tx + 1)*TAILLECASE - 1;
			y = (i/tx)*TAILLECASE;
			ML_line(x,y,x,y+TAILLECASE,ML_BLACK);
		}
		if(murGauche(grille[i])) { //Si il y a un mur a gauche 1000
			x = (i%tx)*TAILLECASE;
			y = (i/tx)*TAILLECASE;
			ML_line(x,y,x,y+TAILLECASE,ML_BLACK);
		}
	}
}