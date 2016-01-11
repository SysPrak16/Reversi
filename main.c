//
// Created by Philipp on 13.11.15.
//

#include <stdio.h>
#include <string.h>
/* damit getopt() deklariert wird */
#define _POSIX_C_SOURCE 2
#include <getopt.h>
#include "lib/appParamHandler.h"
#include "lib/fileIOHandler.h"
#include "lib/connector.h"
#include "lib/thinker.h"
#include "lib/global.h"
//#include "lib/global.h"

//global variable
extern config_t config;

int main( int argc, char* argv[] )
{
	char *gameID;
	gameID="ERROR";
	initconfig();
    //gameID=malloc(sizeof(char)*11);
	const char *configFile, *tmpCfgFile;
    configFile="client.conf";
    //char *tempID;
	int ret;
	while ((ret=getopt(argc, argv, "g:c")) != -1) {
		switch (ret) {
			case 'g':
                gameID=readGameID(optarg);
                strcpy(config.gameID, gameID);
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
    config.port=1357;
    strcpy(config.hostname,DEF_HOSTNAME);
	if (strcmp(gameID,"ERROR")==0){
        printf("ERROR: Sie haben keine GameID angegeben!\n");
        char buf[256];
        do{
            printf("Bitte geben sie die 11 stellige Game-ID an: \n");
            scanf("%s",buf);
        }while(strlen(buf)!=11);
        gameID=buf;
        strcpy(config.gameID, gameID);
    }
	printf("Your Game-ID: %s\n", config.gameID);
    return think();
	/*connectToServer(DEF_PORTNUMBER, DEF_HOSTNAME); //TODO: PORTNUMMER und DEF_HOSTNAME momentan überflüssig, da in prologue.c definiert
	return 0;*/
}