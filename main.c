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
    checkParam(argc,argv);
	connectToServer(PORTNUMBER, HOSTNAME);
	return 0;
}
