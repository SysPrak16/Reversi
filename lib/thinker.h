//
// Created by kad on 26.11.15.
//

#ifndef REVERSI_THINKER_H
#define REVERSI_THINKER_H

/*
	Converts position-ID into string, as spielzug use an empty char[3]
*/
char* convertMove(char * spielzug, int position, int groesse);

/*
	returns a valid move with included random AI
*/
int gueltigerZug(int feld[], int groesse);

/*
 * Think() method.
 * this is doing a lot of work as initialising the shared memory and thinking
 */
int think();
int think1();

#endif //REVERSI_THINKER_H
