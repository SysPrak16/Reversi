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
    int n,i;
    int *test;
    test=malloc(sizeof(int));
    *test=1234;
    //TODO: Actual question and answer game!
    i=0;
    //while((n = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0)
    while((n = receiveMessage(socket_fd, buffer, sizeof(buffer))) > 0)
    {
        //Placeholder ID: Glimmertintling
        char sbuf2[]="ID jvmyltF0k3c\n";
        char sbuf3[]="PLAYER 1\n";
        char sbuf4[]="THINKING\n";
        char sbuf5[]="PLAY F2\n";
        char sbuf6[]="OKWAIT\n";

        char backbuf[BUF_SIZE];
        strncpy(backbuf,buffer,BUF_SIZE);

        const char splittingTool[2] = "\n";
        char *lineBuf;

        //get the first token
        lineBuf = strtok(backbuf, splittingTool);

        // walk through other tokens
        while( lineBuf != NULL ) {
            fprintf(stdout, "S: %s\n", lineBuf);
            strncpy(buffer, lineBuf, BUF_SIZE);

            if (strbeg(buffer, "-")) {
                printf("Server Connection error\n");
                return -1;
            } else if (strbeg(buffer, "+ MNM Gameserver")) {
                if (send(socket_fd, CVERSION, strlen(CVERSION), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, CVERSION);
                }
                //printf("Client sending Client Version\n");
            } else if (strbeg(buffer, "+ Client version accepted - please send Game-ID to join")) {
                if (send(socket_fd, sbuf2, strlen(sbuf2), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, sbuf2);
                }
                //printf("Client sending Game-ID\n");
            } else if (strbeg(buffer, "+ PLAYING")) {
                // Auslesen von GameKind-Name
                if (!strbeg(buffer, "+ PLAYING Reversi")) {
                    printf("Falsche Spielart\n");
                    return -1;
                } else {
                    if (strbeg(buffer, "+ PLAYING Reversi")) {
                        // Senden von Spielernummer
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            puts("Send failed");
                            return -1;
                        } else {
                            printf("C: %.*s", n, PLAYER1);
                        }
                    } else {
                        n = receiveMessage(socket_fd, buffer, sizeof(buffer));
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            puts("Send failed");
                            return -1;
                        } else {
                            printf("C: %.*s", n, PLAYER1);
                        }
                    }
                }
            } else if (strbeg(buffer, "+ YOU")) {

            } else if (strbeg(buffer, "+ TOTAL")) {

            } else if (strbeg(buffer, "+ ENDPLAYERS")) {

            } else if (strbeg(buffer, "+ MOVE")){
                getField=1;
            }else if (strbeg(buffer, "+ FIELD")){
                sscanf(buffer,"+ FIELD %d,%d",&myMTS.width, &myMTS.height);
                //printf("   Deine Höhe: %d\n   Deine Breite: %d\n",myMTS.height,myMTS.width);      /*Just for Testing*/
                myMTS.field=malloc(myMTS.height*myMTS.width*sizeof(int));
            }
            else if (strbeg(buffer, "+ ")&&getField) {
                int line;
                sscanf(buffer,"+ %d",&line);
                //printf("            %d   ",line);                                                 /*Just for Testing*/
                for (int j = 0; j < myMTS.width; j++) {
                    if(buffer[4+j*2]=='W') {
                        myMTS.field[line-1*myMTS.width+j] = 1;
                    }else if(buffer[4+j*2]=='B') {
                        myMTS.field[line-1*myMTS.width+j] = 2;
                    }else if(buffer[4+j*2]=='*') {
                        myMTS.field[line-1*myMTS.width+j] = 0;
                    }
                        //printf("%d ",myMTS.field[line-1*myMTS.width+j]);                          /*Just for Testing*/
                }
                //printf("\n");                                                                     /*Just for Testing*/
                if(line==1)
                    getField=0;
            }else if (strbeg(buffer, "+ ENDFIELD")) {
            }
            else if (strbeg(buffer, "+ ")) {
                //TODO Glimmertintling
            }
            else {
                printf("Fehlerhafte Nachricht vom Server\n");
                //printf("%s\n", buffer);
                return -1;
            }
            i++;
            lineBuf = strtok(NULL, splittingTool);
        }
    }
    return 0;
}


/*
 * else if (strbeg(buffer, "+ YOU ")){
            printf("Counter bei: %i\n",i);
        } else if (strbeg(buffer, "+ TOTAL")){
            //TODO:NOTHING
        }

        }else if (strbeg(buffer, "+ PLAYING")){
            // Auslesen von GameKind-Name
            if(!strbeg(buffer[11],"Reversi"))
            {
                printf("Falsche Spielart\n");
                return -1;
            }
            // Senden von Spielernummer
            if(send(socket_fd , sbuf3 , strlen(sbuf3) , 0) < 0)
            {
                puts("Send failed");
                return -1;
            }else{
                printf("Client: %.*s", n, sbuf3);
            }
        } else if(strbeg(buffer, "+") && i==3){
            // Auslesen von GameName
        } else if (strbeg(buffer, "+ MOVE") ){
            // TODO:Der MOVE-Befehl fordert zum Zug auf
            // TODO:Ausgabe von Spielfeld
            // TODO:Senden von Thinking
            if(send(socket_fd , sbuf4 , strlen(sbuf4) , 0) < 0)
            {
                puts("Send failed");
                return -1;
            }else{
                printf("Client: %.*s", n, sbuf4);
            }}else if(strbeg(buffer, "+ OKTHINK") ){
            // TODO: Senden des Spielzuges
        }else if(strbeg(buffer, "+ MOVEOK") ){
            // Gültiger Spielzug gesendet
        }else if(strbeg(buffer, "+ WAIT")){
            // Senden des Wartesignals
            if(send(socket_fd , sbuf6 , strlen(sbuf6) , 0) < 0)
            {
                puts("Send failed");
                return -1;
            }else{
                printf("Client: %.*s", n, sbuf6);
            }

        }else {
            printf("Fehlerhafte Nachricht vom Server\n");
            printf("%s\n", buffer);
            return -1;
        }*/