#include "performConnection.h"
#include "global.h"

#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>


int performConnection(int socket_fd)
{
    char buffer[BUF_SIZE];
    int n,i;
    //TODO: Actual question and answer game!
    i=0;
    while((n = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0)
    {
        printf("Server: %.*s", n, buffer);

        char sbuf[]="VERSION 1.0\n";
        char sbuf2[]="ID GXe1oEV1gGs\n";
        char sbuf3[]="PLAYER 1\n";
        char sbuf4[]="THINKING\n";
        char sbuf5[]="PLAY F2\n";
        char sbuf6[]="OKWAIT\n";
        switch(i){
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
            }
        i++;

    }
    return 0;
}
