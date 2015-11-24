#include "performConnection.h"

#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <string.h>

int performConnection(int socket_fd)
{
	char buffer[256];
	char line[256];
	int n,i;
	//n is the return value for the read() and write() calls; i.e. it contains the number of characters read or written.
	
	// ToDo: Schleife fuer aufeinanderfolgendes Empfangen und Senden
	for(i = 0;i<4;i++) {
		bzero(buffer, 256);    //buffer leeren
		bzero(line, 256);    //buffer leeren
		//n = read(socket_fd, buffer, 100);
		int cancel = 1;// Wird in while Schleife auf 0 gesetzt wenn '\n' empfangen wurde. -> Abbruchbedingung

		while ((n = recv(socket_fd, buffer, 256, 0)) > 0){ //&& cancel==1) {
			// hier noch Fehler, da nicht bist FileEnde gewartet wird.
			// ToDo: Buffer bis letztes Zeichen lesen
			if (n < 0) {
				perror("ERROR reading from socket");
				return (-1);
			}
			if (buffer[n - 1] == '\n') {
				cancel = 0;
			}
			strcat(line, buffer);
			bzero(buffer, 256);
		}

		printf("LINE: %s\n", line);
        printf("BUFFER: %s\n",buffer);
		// ToDo: Fallabfrage mit Anwort
        //bzero(buffer, 256);
        buffer[0]='a';
        send(socket_fd, buffer, strlen(buffer),0);
        //printf("Was bist du? %s\n", buffer);
	}
	return 0;
}
