#include "performConnection.h"
#include "global.h"
#include "connector.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>

int strbeg(const char *str1, const char *str2)
{
    return strncmp(str1, str2, strlen(str2)) == 0;
}

int performConnection(int socket_fd)
{
    char buffer[BUF_SIZE];
    int n,i;
    //TODO: Actual question and answer game!
    i=0;
    //while((n = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0)
    while((n = receiveMessage(socket_fd, buffer, sizeof(buffer))) > 0)
    {
        char sbuf2[]="ID jvmyltF0k3c\n";
        char sbuf3[]="PLAYER 1\n";
        char sbuf4[]="THINKING\n";
        char sbuf5[]="PLAY F2\n";
        char sbuf6[]="OKWAIT\n";
        /*
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

        if (strbeg(buffer, "-")){
            printf("Server Connection error\n");
            return -1;
        } else if (strbeg(buffer, "+ MNM Gameserver")){
            if(send(socket_fd , CVERSION , strlen(CVERSION) , 0) < 0)
            {
                puts("Send failed");
                return -1;
            }else{
                printf("Client: %.*s", n, CVERSION);
            }
            //printf("Client sending Client Version\n");
        } else if (strbeg(buffer, "+ Client version accepted - please send Game-ID to join")){
            if(send(socket_fd , sbuf2, strlen(sbuf2) , 0) < 0)
            {
                puts("Send failed");
                return -1;
            }else{
                printf("Client: %.*s", n, sbuf2);
            }
            //printf("Client sending Game-ID\n");
        } else if (strbeg(buffer, "+ PLAYING")){
            printf("%c",buffer[10]);//R
            printf("%c",buffer[11]);//E
            printf("%c",buffer[12]);//V
            printf("%c",buffer[13]);//E
            printf("%c",buffer[14]);//R
            printf("%c",buffer[15]);//S
            printf("%c\n",buffer[16]);//I
            // Auslesen von GameKind-Name
            /*if(!strbeg(buffer[11],"Reversi"))
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
            }*/
        } else {
            printf("Fehlerhafte Nachricht vom Server\n");
            printf("%s\n", buffer);
            return -1;
        }
        i++;

    }
    return 0;
}
