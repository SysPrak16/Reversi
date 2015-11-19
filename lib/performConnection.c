#include "performConnection.h"

#include <unistd.h>
#include <stdio.h>
#include <strings.h>

int performConnection(int socket_fd)
{
	char buffer[2048];
	int n;
	//n is the return value for the read() and write() calls; i.e. it contains the number of characters read or written.
	
	// ToD: Schleife fuer aufeinanderfolgendes Empfangen und Senden
	bzero(buffer,2048);
	//buffer leeren
   	n = read(socket_fd, buffer, 100);
	// hier noch Fehler, da nicht bist FileEnde gewartet wird.
	// ToDo: Buffer bis letztes Zeichen lesen
   	if (n < 0) {
      		perror("ERROR reading from socket");
      		return(-1);
   	}
	printf("%s\n",buffer);	
	// ToDo: Fallabfrage mit Anwort
	return 0;
}
