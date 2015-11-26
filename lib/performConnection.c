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
    char line[BUF_SIZE];
    int n,i,cancel;
    bzero(buffer, BUF_SIZE);
    bzero(line, BUF_SIZE);
    //TODO: Actual question and answer game!
    /*
     * for now there is only one "right" answer included.
     * the gameID breaks the server if not correct, so we probably should handle that in the client.
     * have tested this "feature" extensively, that server is awfully annoying
     * reminds me: TODO: Ask for debugging server setting with prolonged timeout!!!
     */
    //for(i=0;i<2;i++){
    for(i=0;i<2;i++){
    //for (;;){
        cancel=1;
        bzero(buffer,BUF_SIZE);
        bzero(line, BUF_SIZE);
        //TODO: For some reason the second answer is not received completely every time.
        while(cancel>0 && (n = recv(socket_fd, buffer, 256, 0)) > 0){
            //printf("Entered While%s\n");
            if (n<0){
                perror("ERROR reading from socket");
                return -1;
            }
            if (buffer[n - 1] == '\n') {
                cancel = 0;
            }
            strcat(line, buffer);
            //printf("BUFFER: %s\n",buffer);
        }

        printf("SERVER: %s\n", line);
        /*
         * CLeaning out buffer, just to be sure.
         * for some reason cleaning out line before buffer works better than the other way around
        */
        bzero(line, BUF_SIZE);
        bzero(buffer, BUF_SIZE);
        //TODO: replace with actual statements and remove placeholders
        switch(i) {
            case 0:
                buffer[0]='V';
                buffer[1]='E';
                buffer[2]='R';
                buffer[3]='S';
                buffer[4]='I';
                buffer[5]='O';
                buffer[6]='N';
                buffer[7]=' ';
                buffer[8]='1';
                buffer[9]='.';
                buffer[10]='0';
                buffer[11]='\n';
                printf("Client: %s\n", buffer);
                send(socket_fd, buffer, BUF_SIZE,0);
                break;
            case 1:
                buffer[0]='I';
                buffer[1]='D';
                buffer[2]=' ';
                buffer[3]='1';
                buffer[4]='2';
                buffer[5]='3';
                buffer[6]='4';
                buffer[7]='5';
                buffer[8]='6';
                buffer[9]='7';
                buffer[10]='8';
                buffer[11]='9';
                buffer[12]='1';
                buffer[13]='2';
                buffer[14]='\n';
                printf("Client: %s\n", buffer);
                send(socket_fd, buffer, BUF_SIZE,0);
                break;
            default:
                return -1;
        }
    }
    return 0;
}
