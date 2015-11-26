#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include "global.h"

/*
 * This is a library which exclusively is responsible for server related issues which can be called from within the project.
 */

/*
 * Reads server answer
 */
char* recieveAnswer();

/*
 * Sends answer -- kinda obvious
 */
void sendAnswer(char answer[BUF_SIZE]);


#endif
