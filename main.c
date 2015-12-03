/* damit getopt() deklariert wird */
#define _POSIX_C_SOURCE 2

#include <stdio.h>
#include <getopt.h>
#include "lib/prologue.h"
#include "lib/appParamHandler.h"
#include "lib/global.h"
/*#include <stdlib.h>
*#include <unistd.h>
*#include <string.h>
*#include <sys/types.h>
*#include <sys/socket.h>
*#include <netdb.h>
*#include <netinet/in.h>
*#include "./lib/performConnection.h"
*/
#include "lib/thinker.h"


int main( int argc, char* argv[] )
{

	field[0][0]='*';	field[0][1]='*';	field[0][2]='*';	field[0][3]='*';
	field[1][0]='*';	field[1][1]='*';	field[1][2]='*';	field[1][3]='*';
	field[2][0]='*';	field[2][1]='*';	field[2][2]='*';	field[2][3]='*';
	field[3][0]='*';	field[3][1]='*';	field[3][2]='*';	field[3][3]='*';


	moveOk();

	char *gameID;
	char *configFile;
    //char *tempID;
	int ret;
	while ((ret=getopt(argc, argv, "g:c")) != -1) {
		switch (ret) {
			case 'g':
                gameID=readGameID(optarg);
				break;
			case 'c':
				//ToDO: configurationsfile lesen
				break;
			default:
				//TODO: Fehler
				break;
		}
	}
	//gameID = checkParam(argc,argv);
	printf("Your Game-ID: %s\n",gameID);
	connectToServer(DEF_PORTNUMBER, DEF_HOSTNAME); //TODO: PORTNUMMER und DEF_HOSTNAME momentan überflüssig, da in prologue.c definiert
	return 0;
}
