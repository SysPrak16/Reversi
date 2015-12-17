//
// Created by kad on 26.11.15.
//

#ifndef REVERSI_CONNECTOR_H
#define REVERSI_CONNECTOR_H

#include "global.h"

/*
 * Creates all necessary stuff to open and establish connection to the server.
 */
int connectToServer();

int receiveMessage(int sfd, char* buffer, int size);

#endif //REVERSI_CONNECTOR_H
