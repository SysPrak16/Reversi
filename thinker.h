//
// Created by kad on 26.11.15.
//

#ifndef REVERSI_THINKER_H
#define REVERSI_THINKER_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "thinker.h"
#include "connector.h"
#include "global.h"
#include <signal.h>

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


#endif //REVERSI_THINKER_H
