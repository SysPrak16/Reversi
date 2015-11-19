#include <stdio.h>
#include "lib/prologue.h"
#include "lib/appParamHandler.h"
/*#include <stdlib.h>
*#include <unistd.h>
*#include <string.h>
*#include <sys/types.h>
*#include <sys/socket.h>
*#include <netdb.h>
*#include <netinet/in.h>
*#include "./lib/performConnection.h"
*/

#define GAMEKINDNAME "Reversi"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"


int main( int argc, char* argv[] )
{
	char *gameID;
	gameID = checkParam(argc,argv);
	printf("Your Game-ID: %s\n",gameID);
	connectToServer(PORTNUMBER, HOSTNAME); //TODO: PORTNUMMER und HOSTNAME momentan überflüssig, da in prologue.c definiert
	return 0;
}
