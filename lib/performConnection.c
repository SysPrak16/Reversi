#include "performConnection.h"
#include "global.h"

#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <string.h>


int performConnection(int socket_fd)
{
    /*
     * Re-Written Method from scratch: simple connect, sending one message, receiving the answer
     */
    char buffer[BUF_SIZE];
    char line[BUF_SIZE];
    int n,i,cancel, doneonce;
    bzero(buffer, BUF_SIZE);
    bzero(line, BUF_SIZE);
    //cancel=1;
    doneonce=1;
    //TODO: Actual question and answer game!
    for(i=0;i<2;i++){
        cancel=1;
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
         * TODO: Actual loop
         * First connection performed, now for the send:
         * Trying to answer:
        */
        bzero(buffer, BUF_SIZE);
        bzero(line, BUF_SIZE);
        if (doneonce>0) {
            doneonce=0;
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
        }
    }
    return 0;
}
