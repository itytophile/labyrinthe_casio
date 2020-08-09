#include <fxlib.h>
#include "simpleTime.h"
#include "laby.h"
#include "MonochromeLib.h"

#define HAUTEUR 32
#define LARGEUR 16

int main() {
	srand(GetTicks());
	unsigned int key;
	struct Laby test;
	while(1) {
		ML_clear_vram();
		initLaby(&test, HAUTEUR, LARGEUR);
		dedale(&test);
		renduLaby(&test);
		GetKey(&key);
		free(test.grille);
	}
	return 1;
}