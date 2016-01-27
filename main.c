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

void initDefault(){
    config.port=DEF_PORTNUMBER;
    strcpy(config.hostname,DEF_HOSTNAME);
}

int main( int argc, char* argv[] )
{
    opterr=0;
    int initFlag=-1;
	char *gameID;
	gameID="ERROR";
	initconfig();
    char *tmpCfgFile=malloc(sizeof(char)*256);
    char configFile[256];
    strcpy(configFile, DEFAULT_CONFIG);
	int ret;
	while ((ret=getopt(argc, argv, "g:c:d:")) != -1) {
		switch (ret) {
			case 'g':
                gameID=readGameID(optarg);
                strcpy(config.gameID, gameID);
				break;
			case 'c':
                printf("%s", tmpCfgFile);
                tmpCfgFile=optarg;
                if (readCfg(tmpCfgFile)>-1){
                    printf("Config File initialised\n");
                }
                else{
                    printf("ERROR: Reading from provided file (%s) failed!\nInitialising from default.\n", tmpCfgFile);
                    initDefault();
                    initFlag=1;
                }
				break;
            case 'd':;
                //char *AI=optarg;
                config.aiType=atoi(optarg);
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
    }
    printf("You are playing Reversi.\n");
    printf("\tYour Game-ID: %s\n", config.gameID);
    if(initFlag<0){
        printf("\tNo external configuration provided, initialising from default...\n");
        initDefault();
    }
    return think();
}