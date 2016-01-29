//
// Created by Philipp on 13.11.15.
//

#include <stdio.h>
#include <string.h>
/* damit getopt() deklariert wird */
#define _POSIX_C_SOURCE 2
#include <getopt.h>
#include <stdlib.h>
#include "fileIOHandler.h"
#include "thinker.h"
#include "global.h"

//global variable
extern config_t config;
extern int opterr;

void initDefault(){
    config.port=DEF_PORTNUMBER;
    strcpy(config.hostname,DEF_HOSTNAME);
}

int main( int argc, char* argv[])
{
    opterr=0;
    int diffFlag,initFlag;
    diffFlag=initFlag=-1;
	char *gameID;
    //char gameID[11];
    gameID="ERROR";
	initconfig();
    char tmpCfgFile[256];//=malloc(sizeof(char)*256);
    char configFile[256];
    strcpy(configFile, DEFAULT_CONFIG);
	int ret;
	while ((ret=getopt(argc, argv, "g:c:d:")) != -1) {
		switch (ret) {
			case 'g':
                gameID=optarg;
                //strcpy(config.gameID, gameID);
				break;
			case 'c':
                //DEBUG: printf("%s", tmpCfgFile)
                strcpy(tmpCfgFile,optarg);
                if (readCfg(tmpCfgFile)>-1){
                    initFlag=1;
                }
                else{
                    printf("\tERROR: Reading from provided file (%s) failed!\nInitialising from default.\n", tmpCfgFile);
                    initDefault();
                }
                break;
            case 'd':;
                config.aiType=atoi(optarg);
                diffFlag=1;
                break;
            default:
                printf(HOW_TO_USE);
                break;
		}
	}
	if (strcmp(gameID,"ERROR")==0){
        printf(NO_ID_ERROR);
        char buf[256];
        do{
            printf(ENTER_GAME_ID);
            scanf("%s",buf);
        }while(strlen(buf)!=11);
        gameID=buf;
        strcpy(config.gameID, gameID);
    }else{
        strcpy(config.gameID, gameID);
    }
    printf("You are playing REVERSI..\n");
    printf("\tYour Game-ID: %s\n", config.gameID);
    if(initFlag<0){
        printf("\tNo external configuration provided, initialising from default...\n");
        printf(HOW_TO_CONFIG);
        initDefault();
    }else{
	printf("\tReading from provided file (%s) succeded.\n\tInitialising...\n", tmpCfgFile);
    }
    if (diffFlag!=1){
        printf(HOW_TO_AI);
    }
    return think();
}
