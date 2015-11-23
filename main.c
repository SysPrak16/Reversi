/* damit getopt() deklariert wird */
#define _POSIX_C_SOURCE 2

#include <stdio.h>
#include <getopt.h>
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
	char *configFile;
    char *tempID;
	int ret;
	/*while ((ret=getopt(argc, argv, "g:c")) != -1) {
		switch (ret) {
			case 'g':
				//TODO: gameID lesen
                tempID=malloc(11*sizeof(char));
                tempID=optarg;
				gameID=readGameID(tempID);
				break;
			case 'c':
				//ToDO: configurationsfile lesen
				break;
			default:
				//TODO: Fehler
				break;
		}
	}*/
	gameID = checkParam(argc,argv);
	printf("Your Game-ID: %s\n",gameID);
	connectToServer(PORTNUMBER, HOSTNAME); //TODO: PORTNUMMER und HOSTNAME momentan überflüssig, da in prologue.c definiert
	return 0;
}
