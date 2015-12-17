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
//#include "prologue.h"
#include "performConnection.h"

int connectToServer(int portnumber, char hostname[BUF_SIZE])
{
    // Socket anlegen
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    //  int socket(int domain, int type, int protocol);

    if (sock < 0)
        perror("ERROR opening socket");
    struct sockaddr_in serv_addr;
    struct hostent *server;
    //hostent defines a host computer on the Internet

    server = gethostbyname(DEF_HOSTNAME);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

    /*
     * The function bzero() sets all values in a buffer to zero.
     * It takes two arguments, the first is a pointer to the buffer and the second is the size of the buffer.
     * Thus, this line initializes serv_addr to zeros.
     */

    bcopy((char *)server->h_addr_list[0],
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_family = PF_INET;
    serv_addr.sin_port = htons(DEF_PORTNUMBER);
    if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        perror("ERROR connecting");}

    performConnection(sock);
    return 0;
}

int receiveMessage(int sfd, char* buffer, int size)
{
    int n = 0;

    while (n < size) {
        //Liest n Zeichen. Merkt sich  Stelle n und schreibt ab n im Array weiter.
        n += recv(sfd, &buffer[n], size - n, 0);
        if (n == -1) {
            perror("Fehler: recv()");
            break;
        }
        buffer[n] = 0;
        //Abbruch bei vorletzter Stelle \n
        if (n != 0 && buffer[n-1] == '\n') {
            //fprintf(stdout, "SERVER: %s\n", buffer);      Auskommentiert, da aus performConnection geprintet wird ()
            break;
        }
    }

    return n;
}
