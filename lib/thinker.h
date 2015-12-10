//
// Created by kad on 26.11.15.
//

#ifndef REVERSI_THINKER_H
#define REVERSI_THINKER_H

/*
	Converts position-ID into string
*/
char* convertMove(int position, int groesse);

/*
	returns a valid move with included random AI
*/
int gueltigeZuege(int feld[], int groesse);

/*
 * Think() method.
 * this is doing a lot of work as initialising the shared memory and thinking
 */
int think();

#endif //REVERSI_THINKER_H
