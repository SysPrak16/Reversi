#ifndef APPPARAMHANDLER_H
#define APPPARAMHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include "global.h"



/*
 * This is a library which exclusively is responsible for console command parameters
 * which can be called from within the project.
*/

/*
 * Reads the specified argument and returns them accordingly.
 */
char* getParam(int argc, char *argv[]);

/*
 * Checks, whether arguments are valid
 */
char* checkParam(int argc, char *argv[]);

/*
 * reads game ID and determines whether it actually IS a game ID.
 */
char* readGameID(char *argv);

char* getIDFromTerminal();


#endif
