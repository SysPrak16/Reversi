//
// Created by kad on 26.11.15.
//

#ifndef REVERSI_CONNECTOR_H
#define REVERSI_CONNECTOR_H

#include "global.h"
#include <sys/socket.h>
#include <stdio.h>
#include "connector.h"
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include "performConnection.h"

/*
 * initialising the values of the main shared segment
 * mainly used for dependencies and storage
 * @void function
 */
void initgameData();

/*
 * connects the client to the server
 * @void
 */
int connectToServer();

/*
 * reads the server message line by line
 * @param: socket
 * @param: line buffer
 * @param: size of line buffer
 *
 * @return: number of read lines
 */
int receiveMessage(int sfd, char* buffer, int size);

#endif //REVERSI_CONNECTOR_H
