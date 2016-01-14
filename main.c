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
#include "lib/thinker.h"
//#include "lib/global.h"

//global variable
extern config_t config;
extern int opterr;

int main( int argc, char* argv[] )
{
    opterr=0;
	char *gameID;
    char *test="ERROR";
	gameID="ERROR";
	initconfig();
    //gameID=malloc(sizeof(char)*11);
	char tmpCfgFile[256]="ERROR";
    char configFile[256];
    strcpy(configFile, DEFAULT_CONFIG);
    //char *tempID;
	int ret;
	while ((ret=getopt(argc, argv, "g:f")) != -1) {
		switch (ret) {
			case 'g':
                gameID=readGameID(optarg);
                strcpy(config.gameID, gameID);
                //gameID=checkParam(argc, argv);

				break;
			case 'f':
                openFile(optarg);
                /*strncpy(tmpCfgFile, optarg, sizeof(tmpCfgFile));
                printf("%s", tmpCfgFile);
                //tmpCfgFile=optarg;
                if (readCfg(tmpCfgFile)>0){
                    printf("Config File initialised\n");
                }
                else{
                    printf("ERROR: Reading from provided file (%s) failed!\nInitialising from default.\n", tmpCfgFile);
                }*/
				break;
			//default:
		}
	}
    config.port=1357;
    strcpy(config.hostname,DEF_HOSTNAME);
	if (strcmp(gameID,"ERROR")==0){
        printf(NO_ID_ERROR);
        char buf[256];
        do{
            printf(ENTER_GAME_ID);
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