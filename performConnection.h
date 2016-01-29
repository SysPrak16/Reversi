#ifndef PERFORMCONNECTION_H
#define PERFORMCONNECTION_H
#include "global.h"
#include "connector.h"

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * \param sfd   open socket for communication
 * \return -1 on failure otherwise 0
 */
int performConnection(int sfd);

#endif // PERFORMCONNECTION_H
