#include "connector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netdb.h>
#include <stdarg.h>

#include "config.h"
#include "performConnection.h"

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
        if (n != 0 && buffer[n - 1] == '\n') {
            fprintf(stdout, "S: %s", buffer);
            break;
        }
    }

    return n;
}

int sendMessage(int sfd, char* format, ...)
{
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    int n;

    va_list ap;
    va_start(ap, format);
    n = vsnprintf(buffer, 255, format, ap);
    va_end(ap);

    if (n < 0)
        return -1;

    buffer[n] = '\n';
    n = send(sfd, buffer, n + 1, 0);

    if (n == -1) {
        perror("Fehler: send()");
        return -1;
    }

    fprintf(stdout, "C: %s", buffer);

    return n;
}

int connectToHost(const char *host, unsigned short port)
{
    struct addrinfo hints, *result, *address;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = PF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_flags      = 0;
    hints.ai_protocol   = 0;
    hints.ai_canonname  = NULL;
    hints.ai_addr       = NULL;
    hints.ai_next       = NULL;

    char portnumber[6];
    snprintf(portnumber, 5, "%u", port);

    int error;
    if ((error = getaddrinfo(host, portnumber, &hints, &result)) != 0) {
        fprintf(stderr, "Fehler: getaddrinfo(): %s\n", gai_strerror(error));
        return -1;
    }

    int sfd;

    for (address = result; address != NULL; address = address->ai_next) {
        if ((sfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol)) == -1) {
            perror("Fehler: socket()");
            continue;
        }

        if (connect(sfd, address->ai_addr, address->ai_addrlen) != -1)
            break;

        perror("Fehler: connect()");
        close(sfd);
        sfd = -1;
    }

    freeaddrinfo(result);

    return sfd;
}

int connector()
{
    int sfd, result;

    if ((sfd = connectToHost(config.hostname, config.port)) == -1) {
        fprintf(stderr, "Fehler: Verbindung zum Host konnte nicht hergestellt werden.\n");
        return -1;
    }

    result = performConnection(sfd);
    close(sfd);

    return result;
}


void initgame()
{
    memset(&game[0], 0, sizeof(game_t));
    game->shmidGame      = -1;
    game->shmidPlayers   = -1;
    game->shmidPieceList = -1;
    game->shmidPieces    = -1;
}
