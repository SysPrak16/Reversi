//
// Created by kad on 26.11.15.
//

#include <sys/socket.h>
#include <stdio.h>
#include "connector.h"
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
//#include "prologue.h"
#include "performConnection.h"

extern config_t config;

void initgameData()
{
    memset(&gameData[0], 0, sizeof(gameData_t));
    gameData->shmid_gameData = -1;
    gameData->shmid_field= -1;
    gameData->shmid_players= -1;
    gameData->thinkerMakeMove=-1;
    gameData->sig_exit=1;
}

int connectToServer()
{
    // create socket:
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        perror("ERROR opening socket");
    struct sockaddr_in serv_addr;
    struct hostent *server;
    //connecting to provided host in config
    server = gethostbyname(config.hostname);
    if (server <=0 ) {
        printf("ERROR, no such host.\n");
        return 0;
    }
    //server address is clean:
    bzero((char *) &serv_addr, sizeof(serv_addr));
    printf("\tConnecting to %s \n",config.hostname);
    bcopy((char *)server->h_addr_list[0],(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_port = htons(config.port);
    if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        perror("ERROR connecting");
    }
    //calls the server listener:
    performConnection(sock);
    return 0;
}

int receiveMessage(int sfd, char* buffer, int size)
{
    int n = 0;
    while (n < size) {
        n += recv(sfd, &buffer[n], size - n, 0);
        if (n == -1) {
            perror("Fehler: recv()");
            break;
        }
        buffer[n] = 0;
        if (n != 0 && buffer[n-1] == '\n') {
            break;
        }
    }
    return n;
}
