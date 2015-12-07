#include "performConnection.h"
#include "global.h"
#include "connector.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

MTS myMTS;  //global Var for sharing in SHM later on

int strbeg(const char *str1, const char *str2)
{
    return strncmp(str1, str2, strlen(str2)) == 0;
}


int performConnection(int socket_fd)
{
    int getField=0;     //@var Helps to check if we are between +FIELD and +ENDFIELD to know when to read out the mom Field

    char buffer[BUF_SIZE];
    int n,i,j;
    //TODO: Actual question and answer game!
    i=0;
    while((n = receiveMessage(socket_fd, buffer, sizeof(buffer))) > 0)
    {
        //Placeholder ID: WAIT
        char sbuf2[]="ID OOxUYtkaiFE\n";

        const char splitToken[2] = "\n";
        char *lineBuf;

        //get the first token
        lineBuf = strtok(buffer, splitToken);

        // walk through other tokens
        while( lineBuf != NULL ) {
            fprintf(stdout, "S: %s\n", lineBuf);
            //strncpy(lineBuf, lineBuf, BUF_SIZE);

            if (strbeg(lineBuf, "-")) {
                printf("Server Connection error\n");
                return -1;
            } else if (strbeg(lineBuf, "+ MNM Gameserver")) {
                if (send(socket_fd, CVERSION, strlen(CVERSION), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, CVERSION);
                }
                //printf("Client sending Client Version\n");
            } else if (strbeg(lineBuf, "+ Client version accepted - please send Game-ID to join")) {
                if (send(socket_fd, sbuf2, strlen(sbuf2), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, sbuf2);
                }
                //printf("Client sending Game-ID\n");
            } else if (strbeg(lineBuf, "+ PLAYING")) {
                // Auslesen von GameKind-Name
                if (!strbeg(lineBuf, "+ PLAYING Reversi")) {
                    printf("Falsche Spielart\n");
                    return -1;
                } else {
                    if (strbeg(lineBuf, "+ PLAYING Reversi")) {
                        // Senden von Spielernummer
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            puts("Send failed");
                            return -1;
                        } else {
                            printf("C: %.*s", n, PLAYER1);
                        }
                    } else {
                        n = receiveMessage(socket_fd, lineBuf, sizeof(lineBuf));
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            puts("Send failed");
                            return -1;
                        } else {
                            printf("C: %.*s", n, PLAYER1);
                        }
                    }
                }
            } else if (strbeg(lineBuf, "+ YOU")) {

            } else if (strbeg(lineBuf, "+ TOTAL")) {

            } else if (strbeg(lineBuf, "+ ENDPLAYERS")) {

            } else if (strbeg(lineBuf, "+ WAIT")) {
                if (send(socket_fd, CWAIT, strlen(CWAIT), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, CWAIT);
                }
            }else if (strbeg(lineBuf, "+ MOVE")){
                getField=1;
            }else if (strbeg(lineBuf, "+ FIELD")){
                sscanf(lineBuf, "+ FIELD %d,%d", &myMTS.width, &myMTS.height);
                printf("   Deine Höhe: %d\n   Deine Breite: %d\n",myMTS.height,myMTS.width);      /*Just for Testing*/
                myMTS.field=malloc(myMTS.height*myMTS.width*sizeof(int));
            }
            else if (strbeg(lineBuf, "+ ") && getField) {
                int line;
                sscanf(lineBuf, "+ %d", &line);
                //printf("            %d   ",line);                                                 /*Just for Testing*/
                for (j = 0; j < myMTS.width; j++) {
                    if(lineBuf[4 + j * 2] == 'W') {
                        myMTS.field[line-1*myMTS.width+j] = 1;
                    }else if(lineBuf[4 + j * 2] == 'B') {
                        myMTS.field[line-1*myMTS.width+j] = 2;
                    }else if(lineBuf[4 + j * 2] == '*') {
                        myMTS.field[line-1*myMTS.width+j] = 0;
                    }
                //printf("%d ",myMTS.field[line-1*myMTS.width+j]);                                  /*Just for Testing*/
                }
                //printf("\n");                                                                     /*Just for Testing*/
                if(line==1)
                    getField=0;
            } else if (strbeg(lineBuf, "+ ENDFIELD")) {
                if (send(socket_fd, CTHINK, strlen(CTHINK), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, CTHINK);
                }
            } else if (strbeg(lineBuf, "+ ")) {
                //TODO Glimmertintling
            }
            else {
                printf("Fehlerhafte Nachricht vom Server\n");
                //printf("%s\n", lineBuf);
                return -1;
            }
            i++;
            lineBuf = strtok(NULL, splitToken);
        }
    }
    return 0;
}