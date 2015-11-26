#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

/*
 * This is a library which exclusively is responsible for Error throwing and catching which can be called from within the project.
 */
int wrongParamError();

int fileNotFoundError();

void unknownKeyError(char *error);

#endif