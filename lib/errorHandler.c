//
// Created by kad on 26.11.15.
//
#include <stdio.h>
#include "errorHandler.h"

//int wrongParamError()

/*
 * throws an error to stderror
 * could be changed to an ret int for further error handling
 */
int fileNotFoundError()
{
    perror("ERROR during: fopen");
    return -1;
}

void unknownKeyError(char *error){
    fprintf(stderr, "ERROR: Unknown parameter '%s', ignoring it!\n", error);
}