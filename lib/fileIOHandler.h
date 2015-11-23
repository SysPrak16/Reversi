#ifndef FILEIOHANDLER_H
#define FILEIOHANDLER_H

#include <float.h>
#include <stdbool.h>
#include <string.h>

/*
*This is a library which exclusively is responsible for file I/O-Handling which can be called from within the project.
*/

typedef struct {
	char line[char lineContent[]];
} configfile;

/*
* Opens a filedescriptor to the specified filepath.
* Returns 0 if successfully opened, 1 else.
* Creates file if it does not exist
*/
int openFile(char[] filepath);

/*
* creates a file in the specified location
* Returns 0 if successfully opened, 1 else.
* only creates file, does NOT open for edit!
*/
int createFile(char[] filepath, char[] filename);

/*
* Closes specified descriptor
* Returns 0 if successfully opened, 1 else.
*/
int closeFile(char[] filepath);

/*
* The files the program has to be able to read follow a specific rule
* This function tests, whether those rules apply or not.
* Returns 0 if successfully opened, 1 else.
*/
int testFileForValidStruct(filedescriptor fd);

/*
* Reads struct as defined from file
* Can only be called if testFileForValidStruct() returns 0.
* returns struct
*/
<structPlaceholder> readStruct(filedescriptor fd);

/*
* Reads single char from file
* returns single char
*/

char readSingleChar(filedescriptor fd);

/*
* Reads single cline from file
* returns single char
*/

char readSingleLine(filedescriptor fd);

/*
* Creates array of char arrays from the read file
* stores file in archived mem
*/
configfile readFullFile(filedescriptor fd);

#endif