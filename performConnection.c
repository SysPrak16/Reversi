#include "performConnection.h"
#include "global.h"
#include "connector.h"

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

extern config_t config;

int strbeg(const char *str1, const char *str2)
{
    return strncmp(str1, str2, strlen(str2)) == 0;
}

int sendSignalToThinker(){
    if (kill(gameData->processIDParent, SIGUSR1) == -1){
        printf("Error sending signal!! SIGUSR1\n");
        return -1;
    }
    return 0;
}

int sendExitToThinker(){
    gameData->sig_exit=0;
    if (kill(gameData->processIDParent, SIGUSR1) == -1){
        printf("Error sending signal!! SIGUSR1\n");
        return -1;
    }
    return 0;
}

int cleanupSharedMemories(){
    if (gameData->shmid_players!=-1){
        if(shmctl (gameData->shmid_players, IPC_RMID, NULL)<0){
            perror(DETATCH_ERROR);
        }
    }
    if (gameData->shmid_field!=-1) {
        if (shmctl(gameData->shmid_field, IPC_RMID, NULL) < 0) {
            perror(DETATCH_ERROR);
        }
    }
    if(sendExitToThinker()==-1){
        perror(DETATCH_ERROR);
    }
    return 0;
}


int performConnection(int socket_fd)
{
    player_t *players;
    gameField_t *gfield;
    char clientName[BUF_SIZE];

    int opi=0;      //@var Opponent Info Flag
    int ggN=0;      //@var Get Game Name Flag
    int getField=0; //@var Helps to check if we are between +FIELD and +ENDFIELD to know when to read out the mom Field

    char buffer[BUF_SIZE];
    int n,i,j;
    char *move;
    char gid[16];
    strcpy(gid, "ID ");
    strcat(gid, config.gameID);
    strcat(gid, "\n");
    const char splitToken[2] = "\n";

    i=0;
    while((n = receiveMessage(socket_fd, buffer, sizeof(buffer))) > 0)
    {
        char *lineBuf;
        lineBuf = strtok(buffer, splitToken);

        while( lineBuf != NULL ) {
            if (strbeg(lineBuf, CON_TIMEOUT)) {
                perror(CON_TIMEOUT_ERR_MSG);
                cleanupSharedMemories();
                return -1;
            } else if (strbeg(lineBuf, VERSION_ERROR)) {
                perror(VERSION_ERROR_MSG);
                cleanupSharedMemories();
                return -1;
            } else if (strbeg(lineBuf, NO_FREE_PLAYER)) {
                perror(NO_FREE_PLAYER_MSG);
                cleanupSharedMemories();
                return -1;
            } else if (strbeg(lineBuf, NO_GAME_ERROR)) {
                perror(NO_GAME_ERROR_MSG);
                cleanupSharedMemories();
                return -1;
            } else if (strbeg(lineBuf, INVALID_MOVE)) {
                perror(INVALID_MOVE_MSG);
                cleanupSharedMemories();
                return -1;
            } else if (strbeg(lineBuf, MNM_SERVER)) {
                sscanf(lineBuf, "+ MNM Gameserver v%d.%d", &gameData->server_major, &gameData->server_minor);
                printf("\tServer V%d.%d matches client version.\n\tConnection established.\n", gameData->server_major, gameData->server_minor);

                if (send(socket_fd, CVERSION, strlen(CVERSION), 0) < 0) {
                    perror(SEND_FAILED);
                    cleanupSharedMemories();
                    return -1;
                } else {
                }
            } else if (strbeg(lineBuf, VERSION_ACCEPTED)) {
                if (send(socket_fd, gid, strlen(gid), 0) < 0) {
                    perror(SEND_FAILED);
                    cleanupSharedMemories();
                    return -1;
                } else {
                }
            } else if (strbeg(lineBuf, "+ PLAYING")) {
                ggN=1;
                if (!strbeg(lineBuf, "+ PLAYING Reversi")) {
                    printf(GAMEKIND_ERROR_MSG);
                    cleanupSharedMemories();
                    return -1;
                } else {
                    if (strbeg(lineBuf, "+ PLAYING Reversi")) {
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            perror(SEND_FAILED);
                            cleanupSharedMemories();
                            return -1;
                        } else {
                        }
                    } else {
                        n = receiveMessage(socket_fd, lineBuf, sizeof(lineBuf));
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            perror(SEND_FAILED);
                            cleanupSharedMemories();
                            return -1;
                        } else {
                        }
                    }
                }
            } else if (ggN && strbeg(lineBuf, "+ ")) {//Getting Gamename
                sscanf(lineBuf,"+ %s",gameData->gamename);
                ggN=0;
            } else if (strbeg(lineBuf, "+ YOU")) {
                sscanf(lineBuf,"+ YOU %i %[^\n]",&gameData->playerID, clientName);
                printf("\tYou are player %i, named %s, playing the %s tokens\n",gameData->playerID, clientName,(gameData->playerID==0)?("black"):("white"));
            } else if (strbeg(lineBuf, "+ TOTAL")) {
                sscanf(lineBuf,"+ TOTAL %i",&gameData->playerCount);
                gameData->shmid_players=shmget(IPC_PRIVATE, sizeof(player_t) * gameData->playerCount, IPC_CREAT | 0666);
                if (gameData->shmid_players == -1) {
                    perror(SHM_ERROR);
                    cleanupSharedMemories();
                    return -1;
                }
                players=shmat(gameData->shmid_players, 0, 0);
                if (players == (player_t *) -1) {
                    perror(SHM_ERROR);
                    cleanupSharedMemories();
                    return -1;
                }
                strcpy(players[gameData->playerID].name, clientName);
                players[gameData->playerID].number=gameData->playerID;
                players[gameData->playerID].flag=1;
                opi=1;
            }else if(opi&&strbeg(lineBuf, "+ ")){
                int pnumber=-1;
                char pname[BUF_SIZE];
                sscanf(lineBuf,"+ %i %[^\n]", &pnumber, pname);
                int c=-1;
                do{
                    c++;
                }while(pname[c]!='\0');
                players[pnumber].flag = atoi(&pname[c-1]);
                players[pnumber].number=pnumber;
                strcpy(players[pnumber].name, pname);
                printf("\tYou are playing against Player %i, named %s, who is %s\n",players[pnumber].number,players[pnumber].name,(players[pnumber].flag==1)?("Ready"):("Not Ready yet"));
                opi=0;
            } else if (strbeg(lineBuf, "+ ENDPLAYERS")) {
                if(config.aiType==AI_RAND) {
                    printf(AGAINST_RAND);
                }else if(config.aiType==AI_ENHANCED_RAND){
                    printf(AGAINST_ENH_RAND);
                }else if(config.aiType==AI_MAX_GAIN){
                    printf(AGAINST_MAX_GAIN);
                }else{
                    printf(AGAINST_RAND);
                }
            } else if (strbeg(lineBuf, "+ WAIT")) {
                if (send(socket_fd, CWAIT, strlen(CWAIT), 0) < 0) {
                    puts("Send failed");
                    cleanupSharedMemories();
                    return -1;
                } else {
                }
            }else if (strbeg(lineBuf, "+ MOVE")){
            }else if (strbeg(lineBuf, "+ FIELD")){
                getField=1;
                gameData->shmid_field=shmget(FIELD_ID+gameData->playerID*5, sizeof(gfield), IPC_CREAT | 0666);
                if (gameData->shmid_field == -1) {
                    printf("This");
                    perror(SHM_ERROR);
                    cleanupSharedMemories();
                    return -1;
                }
                gfield=shmat(gameData->shmid_field, 0, 0);
                if (gfield == (void *) -1) {
                    perror(SHM_ERROR);
                    cleanupSharedMemories();
                    return -1;
                }
                sscanf(lineBuf, "+ FIELD %d,%d", &gfield->width, &gfield->height);
            }
            else if (strbeg(lineBuf, "+ ") && getField) {
                int line;
                sscanf(lineBuf, "+ %i", &line);
                if(line>=10){
                    for(j = 0; j < gfield->width; j++){
                        if (lineBuf[5 + j * 2] == 'W'){
                            switch (gameData->playerID){
                                case 0:
                                    gfield->field[j+(gfield->width*(gfield->height-line))]=2;
                                    break;
                                case 1:
                                    gfield->field[j+(gfield->width*(gfield->height-line))]=1;
                                    break;
                            }
                        }else if(lineBuf[5 + j * 2] == 'B'){
                            switch (gameData->playerID){
                                case 0:
                                    gfield->field[j+(gfield->width*(gfield->height-line))]=1;
                                    break;
                                case 1:
                                    gfield->field[j+(gfield->width*(gfield->height-line))]=2;
                                    break;
                            }
                        } else if(lineBuf[5 + j * 2] == '*'){
                            gfield->field[j+(gfield->width*(gfield->height-line))]=0;
                        }
                    }
                }else{
                    for(j = 0; j < gfield->width; j++){
                        if (lineBuf[4 + j * 2] == 'W'){
                            switch (gameData->playerID){
                                case 0:
                                    gfield->field[j+(gfield->width*(gfield->height-line))]=2;
                                    break;
                                case 1:
                                    gfield->field[j+(gfield->width*(gfield->height-line))]=1;
                                    break;
                            }
                        }else if(lineBuf[4 + j * 2] == 'B'){
                            switch (gameData->playerID){
                                case 0:
                                    gfield->field[j+(gfield->width*(gfield->height-line))]=1;
                                    break;
                                case 1:
                                    gfield->field[j+(gfield->width*(gfield->height-line))]=2;
                                    break;
                            }
                        } else if(lineBuf[4 + j * 2] == '*'){
                            gfield->field[j+(gfield->width*(gfield->height-line))]=0;
                        }
                    }
                }
                if(line==1) {
                    getField = 0;
                }
            } else if (strbeg(lineBuf, "+ ENDFIELD")) {
                if (send(socket_fd, CTHINK, strlen(CTHINK), 0) < 0) {
                    puts("Send failed");
                    cleanupSharedMemories();
                    return -1;
                } else {
                }
            } else if (strbeg(lineBuf, "+ OKTHINK")) {
                gameData->thinkerMakeMove=1;
                if(sendSignalToThinker()==-1){
                    perror(SIG_ERROR);
                    cleanupSharedMemories();
                    return -1;
                }else{
                    do{
                        wait(0);
                    } while (gameData->moveOK!=1);
                    gameData->moveOK=0;
                    move=malloc(gameData->movesize*sizeof(char));
                    read(gameData->pipe.read, move, gameData->movesize*sizeof(char));
                    char *final_move=malloc(gameData->movesize*sizeof(char)+6*sizeof(char));
                    bzero(final_move,sizeof(final_move));
                    strcpy(final_move, "PLAY ");
                    strcat(final_move, move);
                    printf("\n\tAI Moved to: %s\n",move);
                    if (send(socket_fd, final_move, strlen(final_move), 0) < 0) {
                        puts("Send failed");
                        free(move);
                        free(final_move);
                        cleanupSharedMemories();
                        return -1;
                    } else {
                    }
                    free(move);
                    free(final_move);
                }

            } else if (strbeg(lineBuf, "+ GAMEOVER")){
                int winner_nbr;
                sscanf(lineBuf,"+ GAMEOVER %i", &winner_nbr);
                if(winner_nbr==gameData->playerID){
                    printf("CONGRATULATIONS, %s!\n\nYOU WIN!\n\n\n", players[gameData->playerID].name);
                }else{
                    printf("YOU LOOSE!\n\nPLAYER %s BEATS YOU!\n\n\n", players[winner_nbr].name);
                }
            }
            else if (strbeg(lineBuf, "+ QUIT")){
                cleanupSharedMemories();
                return 0;

            } else {
                printf("Corrupt server message!\n");
                cleanupSharedMemories();
                return 1;
            }
            i++;
            lineBuf = strtok(NULL, splitToken);
        }
    }
    return 0;
}