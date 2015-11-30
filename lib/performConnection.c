#include "performConnection.h"
#include "global.h"
#include "connector.h"

#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

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
        //printf("Server: %.*s", n, buffer);

        char sbuf[]="VERSION 1.0\n";
        char sbuf2[]="ID GXe1oEV1gGs\n";
        char sbuf3[]="PLAYER 1\n";
        char sbuf4[]="THINKING\n";
        char sbuf5[]="PLAY F2\n";
        char sbuf6[]="OKWAIT\n";

        if (strbeg(buffer, "-")){
            printf("Server Connection error\n");
            return -1;
        } else if (strbeg(buffer, "+ MNM Gameserver")){
            if(send(socket_fd , sbuf , strlen(sbuf) , 0) < 0)
            {
                puts("Send failed");
                return -1;
            }else{
                printf("Client: %.*s", n, sbuf);
            }
            //printf("Client sending Client Version\n");
        } else if (strbeg(buffer, "+ Client version accepted - please send Game-ID to join")){
            if(send(socket_fd , sbuf2 , strlen(sbuf2) , 0) < 0)
            {
                puts("Send failed");
                return -1;
            }else{
                printf("Client: %.*s", n, sbuf2);
            }
            //printf("Client sending Game-ID\n");
        } else if (strbeg(buffer, "+ PLAYING")){
        } else {
            printf("Fehlerhafte Nachricht vom Server\n");
            printf("%s\n", buffer);
            return -1;
        }

        /*switch(i){
            case 0:
                if(send(socket_fd , sbuf , strlen(sbuf) , 0) < 0)
                {
                    puts("Send failed");
                    return -1;
                }else{
                    printf("Client: %.*s", n, sbuf);
                }
                bzero(buffer, BUF_SIZE);
                break;
            case 1:
                if(send(socket_fd , sbuf2 , strlen(sbuf2) , 0) < 0)
                {
                    puts("Send failed");
                    return -1;
                }else{
                    printf("Client: %.*s", n, sbuf2);
                }
                bzero(buffer, BUF_SIZE);
                break;
            case 2:
                if(send(socket_fd , sbuf3 , strlen(sbuf3) , 0) < 0)
                {
                    puts("Send failed");
                    return -1;
                }else{
                    printf("Client: %.*s", n, sbuf3);
                }
                bzero(buffer, BUF_SIZE);
                break;
            case 3:
                if(send(socket_fd , sbuf4 , strlen(sbuf4) , 0) < 0)
                {
                    puts("Send failed");
                    return -1;
                }else{
                    printf("Client: %.*s", n, sbuf4);
                }
                bzero(buffer, BUF_SIZE);
                break;
            case 4:
                if(send(socket_fd , sbuf5 , strlen(sbuf5) , 0) < 0)
                {
                    puts("Send failed");
                    return -1;
                }else{
                    printf("Client: %.*s", n, sbuf5);
                }
                bzero(buffer, BUF_SIZE);
                break;
            case 5:
                if(send(socket_fd , sbuf6 , strlen(sbuf6) , 0) < 0)
                {
                    puts("Send failed");
                    return -1;
                }else{
                    printf("Client: %.*s", n, sbuf6);
                }
                bzero(buffer, BUF_SIZE);
                break;
            }*/
        i++;

    }
    return 0;

    /*if (strbeg(line, "-")){	printf("Server Connection error\n");
      return -1;
    	} else if (strbeg(line, "+ MNM Gameserver")){
            send(socket_fd, "VERSION 1.0", strlen("VERSION 1.0")+1,0);
		    printf("Client sending Client Version\n");
	    } else if (strbeg(line, "+ Client version accepted - please send Game-ID tp join")){
		    printf("Client sending Game-ID\n");
    	} else if (strbeg(line, "+ PLAYING")){
    	} else {
	    	printf("Fehlerhafte Nachricht vom Server\n");
	    	return -1;
	    }*/
}
