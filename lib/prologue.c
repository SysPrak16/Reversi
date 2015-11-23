//
// Created by kad on 19.11.15.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include "prologue.h"
#include "performConnection.h"

#define GAMEKINDNAME "Reversi"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"

int connectToServer(int portnumber, char hostname)
{
    // Socket anlegen
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    //  int socket(int domain, int type, int protocol);

    if (sock < 0)
        perror("ERROR opening socket");
    struct sockaddr_in serv_addr;
    struct hostent *server;
    //hostent defines a host computer on the Internet

    server = gethostbyname(HOSTNAME);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //The function bzero() sets all values in a buffer to zero. It takes two arguments, the first is a pointer to the buffer and the second is the size of the buffer. Thus, this line initializes serv_addr to zeros.
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    //
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_port = htons(PORTNUMBER);
    if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        perror("ERROR connecting");}

    performConnection(sock);
    return 0;
}
