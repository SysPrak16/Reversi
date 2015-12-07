#ifndef SPIELFELD_H
#define SPIELFELD_H

#include "spielfeld.h"
/*
	Converts position-ID into string
*/
char* convertMove(int position, int groesse);

/*
	returns a valid move with included random AI
*/
int gueltigeZuege(int feld[], int groesse);

#endif
