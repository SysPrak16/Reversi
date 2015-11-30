//
// Created by kad on 26.11.15.
//

#include <sys/socket.h>
#include <stdio.h>
#include "connector.h"

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
        if (n != 0 && buffer[n - 1] == '\n') {
            fprintf(stdout, "SERVER: %s", buffer);
            break;
        }
    }

    return n;
}
