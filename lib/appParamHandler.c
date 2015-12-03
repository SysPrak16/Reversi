//
// Created by Philipp on 19.11.15.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define errorString "\0"
/* @param: int argc, char *argv[]: Kommandozeilenparrameter aus main()
 * @ret: -1 bei Fehler (non valid Input numer) wrongParamError
 * @ret:
 *
 * */

char* checkParam(int argc, char *argv[]){

    char *GameID;
    GameID = malloc(11 * sizeof(char));
    if(argc < 2 || argc > 2) {
        printf("Sie haben keine Game-ID angegeben! ");
        /*char buf[256];
        do{
            printf("Bitte geben sie die 11 stellige Game-ID an:");
            scanf("%s",buf);
        }
        while(strlen(buf)!=11);
        strncpy(GameID,buf,11);
        return GameID;*/
        //return readGameID(-1);
        //return errorString; //wrongParamError
    }
    else{
        int count = 0;
        while(count<11){
            if(argv[1][count]=='\0'){
                printf("Die von ihnen angegebene Game-ID hat nur %d Stellen! ",count);
                char buf[256];
                do{
                    printf("Bitte geben sie die 11 stellige Game-ID an: \n");
                    scanf("%s",buf);
                }
                while(strlen(buf)!=11);
                strncpy(GameID,buf,11);
                return GameID;
                //return readGameID(count);
                //return errorString; //wrongParamError
            }
                GameID[count]=argv[1][count];
                count++;
        }
        if(argv[1][count]!='\0'){
            printf("Die von ihnen angegebene Game-ID ist zu lang! ");
            char buf[256];
            do{
                printf("Bitte geben sie die 11 stellige Game-ID an:");
                scanf("%s",buf);
            }
            while(strlen(buf)!=11);
            strncpy(GameID,buf,11);
            return GameID;
        }
        return GameID;
    }
}

char* readGameID(char *argv){
    char *GameID;
    GameID = malloc(11 * sizeof(char));

    int count = 0;
    while(count<11){
        if(argv[count]=='\0'){
            printf("Die von ihnen angegebene Game-ID hat nur %d Stellen! ",count);
            //GameID=getIDFromTerminal();
            char buf[256];
            do{
                printf("Bitte geben sie die 11 stellige Game-ID an: ");
                scanf("%s",buf);
            }
            while(strlen(buf)!=11);
            strncpy(GameID,buf,11);
            return GameID;
            //return readGameID(count);
            //return errorString; //wrongParamError
        }
        GameID[count]=argv[count];
        count++;
    }
    if(argv[count]!='\0'){
        printf("Die von ihnen angegebene Game-ID ist zu lang! ");
        //GameID=getIDFromTerminal();
        char buf[256];
        do{
            printf("Bitte geben sie die 11 stellige Game-ID an:");
            scanf("%s",buf);
        }
        while(strlen(buf)!=11);
        strncpy(GameID,buf,11);
        return GameID;
    }
    return GameID;
}