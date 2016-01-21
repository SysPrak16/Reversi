#include "performConnection.h"
#include "global.h"
#include "connector.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <signal.h>

typedef struct {
    int *field;
    int height;
    int width;
    int flag;
    int fielda[8*8];
}SHM;

MTS myMTS;  //global Var for sharing in SHM later on
extern config_t config;

int strbeg(const char *str1, const char *str2)
{
    return strncmp(str1, str2, strlen(str2)) == 0;
}

int sendSignalToThinker(){
    printf("Debug Message: Sending Signal to think.\n");
    if (kill(getppid(), SIGUSR1) != 0)	{	//Sendet das Signal SIGUSR1 an den Elternprozess
        printf("Error sending signal!! SIGUSR1\n");
        return -1;
    }
    return 0;
}

int sendExitToThinker(){
    if (kill(getppid(), SIGUSR2) != 0)	{	//Sendet das Signal SIGUSR2 an den Elternprozess
        printf("Error sending signal!! SIGUSR2\n");
        return -1;
    }
    return 0;
}


int performConnection(int socket_fd)
{
    /*
     * New shared Memory variables and addresses
     */
    player_t *players;
    gameField_t *gfield;
    char clientName[BUF_SIZE];

    fd_set set;
    FD_ZERO(&set);

    int opi=0;      //@var Opponent Info Flag
    int ggN=0;      //@var Get Game Name Flag
    int getField=0; //@var Helps to check if we are between +FIELD and +ENDFIELD to know when to read out the mom Field

    char buffer[BUF_SIZE];
    printf("Your Game ID: %s\n",config.gameID);
    int n,i,j;
    //TODO: Actual question and answer game!
    //Placeholder ID: WAIT
    //TODO: resolve test variables!
    //TEST--Start
    char play[]="PLAY C5\n";
    //TEST--End

    char gid[16];
    strcpy(gid, "ID ");
    strcat(gid, config.gameID);
    strcat(gid, "\n");
    //Split by \n
    const char splitToken[2] = "\n";

    i=0;
    while((n = receiveMessage(socket_fd, buffer, sizeof(buffer))) > 0)
    {
        FD_SET(socket_fd, &set);
        FD_SET(gameData->pipe.in, &set);
        char *lineBuf;
        //get the first token
        lineBuf = strtok(buffer, splitToken);
        // walk through other tokens
        while( lineBuf != NULL ) {
            //DEBUG:
            fprintf(stdout, "S: %s\n", lineBuf);
            if (strbeg(lineBuf, CON_TIMEOUT)) {
                printf(CON_TIMEOUT_ERR_MSG);
                return -1;
            } else if (strbeg(lineBuf, VERSION_ERROR)) {
                printf(VERSION_ERROR_MSG);
                return -1;
            } else if (strbeg(lineBuf, NO_FREE_PLAYER)) {
                printf(NO_FREE_PLAYER_MSG);
                return -1;
            } else if (strbeg(lineBuf, NO_GAME_ERROR)) {
                printf(NO_GAME_ERROR_MSG);
                return -1;
            } else if (strbeg(lineBuf, INVALID_MOVE)) {
                printf(INVALID_MOVE_MSG);
                return -1;
            } else if (strbeg(lineBuf, MNM_SERVER)) {
                int versionMajor, versionMinor;
                sscanf(lineBuf, "%*s %*s %*c%d%*c%d", &versionMajor, &versionMinor);
                //fprintf(stdout, "Server V%d.%d is ready for connection.\n", versionMajor, versionMinor);
                if (send(socket_fd, CVERSION, strlen(CVERSION), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    //DEBUG: printf("C: %.*s", n, CVERSION);
                }
            } else if (strbeg(lineBuf, VERSION_ACCEPTED)) {
                if (send(socket_fd, gid, strlen(gid), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    //DEBUG: printf("C: %.*s", n, gid);
                }
            } else if (strbeg(lineBuf, "+ PLAYING")) {
                ggN=1;
                // Auslesen von GameKind-Name
                if (!strbeg(lineBuf, "+ PLAYING Reversi")) {
                    printf(GAMEKIND_ERROR_MSG);
                    return -1;
                } else {
                    if (strbeg(lineBuf, "+ PLAYING Reversi")) {
                        // Senden von Spielernummer
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            puts("Send failed");
                            return -1;
                        } else {
                            //DEBUG: printf("C: %.*s", n, PLAYER1);
                        }
                    } else {
                        n = receiveMessage(socket_fd, lineBuf, sizeof(lineBuf));
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            puts("Send failed");
                            return -1;
                        } else {
                            //DEBUG: printf("C: %.*s", n, PLAYER1);
                        }
                    }
                }
            } else if (ggN && strbeg(lineBuf, "+ ")) {//Getting Gamename
                sscanf(lineBuf,"+ %s",gameData->gamename);
                ggN=0;
            } else if (strbeg(lineBuf, "+ YOU")) {
                sscanf(lineBuf,"+ YOU %i %s",&gameData->playerID, clientName);
                printf("You are player %i, named %s\n",gameData->playerID, clientName);
            } else if (strbeg(lineBuf, "+ TOTAL")) {
                sscanf(lineBuf,"+ TOTAL %i",&gameData->playerCount);
                //fprintf(stdout, "Active players: %d\n", gameData->playerCount);

                gameData->shmid_players=shmget(IPC_PRIVATE, sizeof(player_t) * gameData->playerCount, IPC_CREAT | 0666);
                if (gameData->shmid_players == -1) {
                    perror(SHM_ERROR);
                    return -1;
                }
                players=shmat(gameData->shmid_players, NULL, 0);
                if (players == (player_t *) -1) {
                    perror(SHM_ERROR);
                    return -1;
                }
                strcpy(players[gameData->playerID].name, clientName);
                players[gameData->playerID].number=gameData->playerID;
                players[gameData->playerID].flag=1;
                opi=1;
            }else if(opi&&strbeg(lineBuf, "+ ")){
                //TODO: für mehr als 2 spieler
                //Nope
                int pnumber=-1;
                char pname[BUF_SIZE];
                int pactive=-1;
                sscanf(lineBuf,"+ %i %s %i", &pnumber, pname, &pactive);
                //DEBUG: printf("\tNUMBER: %i\n",pnumber);
                players[pnumber].number=pnumber;
                strcpy(players[pnumber].name, pname);
                players[pnumber].flag=pactive;
                opi=0;
            } else if (strbeg(lineBuf, "+ ENDPLAYERS")) {

            } else if (strbeg(lineBuf, "+ WAIT")) {
                if (send(socket_fd, CWAIT, strlen(CWAIT), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    //DEBUG: printf("C: %.*s", n, CWAIT);
                }
            }else if (strbeg(lineBuf, "+ MOVE")){
                sscanf(lineBuf,"+ MOVE %i",&gameData->movetime);
                getField=1;
            }else if (strbeg(lineBuf, "+ FIELD")){
                getField=1;
                gameData->shmid_field=shmget(IPC_PRIVATE, sizeof(gfield), IPC_CREAT | 0666);
                if (gameData->shmid_field == -1) {
                    perror(SHM_ERROR);
                    return -1;
                }
                gfield=shmat(gameData->shmid_field, NULL, 0);
                if (gfield == (void *) -1) {
                    perror(SHM_ERROR);
                    return -1;
                }
                sscanf(lineBuf, "+ FIELD %d,%d", &gfield->width, &gfield->height);
                gfield->field=malloc(gfield->height*gfield->width*sizeof(int));
            }
            else if (strbeg(lineBuf, "+ ") && getField) {
                int line;
                sscanf(lineBuf, "+ %d", &line);
                for (j = 0; j < gfield->width; j++){
                    if(lineBuf[4 + j * 2] == 'W') {
                        if(gameData->playerID==0) {
                            gfield->field[line - 1 * gfield->width + j] = 2;
                        }else {
                            gfield->field[line - 1 * gfield->width + j] = 1;
                        }
                    }else if(lineBuf[4 + j * 2] == 'B') {
                        if(gameData->playerID==0) {
                            gfield->field[line-1 * gfield->width + j] = 1;
                        }else{
                            gfield->field[line-1*gfield->width+j] = 2;
                        }
                    }else if(lineBuf[4 + j * 2] == '*') {
                        gfield->field[line-1*gfield->width+j] = 0;
                    }
                    printf("%d ",gfield->field[line-1*gfield->width+j]);                            //Just for Testing
                }
                if(line==1) {
                    getField = 0;
                }
            } else if (strbeg(lineBuf, "+ ENDFIELD")) {
                if (send(socket_fd, CTHINK, strlen(CTHINK), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    //DEBUG: printf("C: %.*s", n, CTHINK);
                }
                //TODO signal
                gameData->thinkerMakeMove=1;
                if(sendSignalToThinker()==-1){
                    perror(SIG_ERROR);
                }
            } else if (strbeg(lineBuf, "+ OKTHINK")) {
                //TODO PLAY -- MOVE
                if (FD_ISSET(gameData->pipe.in, &set)) {
                    char move[4];
                    read(gameData->pipe.in, move, sizeof(move));

                    if (send(socket_fd, move, strlen(move), 0) < 0) {
                        puts("Send failed");
                        return -1;
                    } else {
                        //DEBUG: printf("C: %.*s", n, play);
                    }
                }

            } else if (strbeg(lineBuf, "+ QUIT")){
                //TODO:Free memories + signal thinker exit

            }
            else {
                printf("Fehlerhafte Nachricht vom Server\n");
                //printf("%s\n", lineBuf);
                    //myuSHYc->flag=42;       //Bricht SHM in Eleternprozess ab -> Beenden des Programmes
                return -1;
            }
            i++;
            lineBuf = strtok(NULL, splitToken);
        }
    }
    return 0;       //fallback
}