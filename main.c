#include <stdio.h>
#include <getopt.h>
//#include <string.h>
//#include "lib/prologue.h"
#include "lib/appParamHandler.h"
//#include "lib/global.h"
#include "lib/fileIOHandler.h"
#include "lib/connector.h"



int main( int argc, char* argv[] )
{
	char *gameID;
	gameID="ERROR";
    //gameID=malloc(sizeof(char)*11);
	const char *configFile, *tmpCfgFile;
    configFile="client.conf";
    //char *tempID;
	int ret;
	while ((ret=getopt(argc, argv, "g:c")) != -1) {
		switch (ret) {
			case 'g':
                gameID=readGameID(optarg);
                //gameID=checkParam(argc, argv);

				break;
			case 'c':
                tmpCfgFile=optarg;
                if (readCfg(tmpCfgFile)>0){
                    configFile=tmpCfgFile;
                    printf("Config File initialised\n");
                }
                else{
                    printf("Initialising from default.\n");
                }
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
