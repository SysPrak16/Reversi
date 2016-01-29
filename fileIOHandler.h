//
// Created by kad on 26.11.15.
//

#ifndef REVERSI_FILEIOHANDLER_H
#define REVERSI_FILEIOHANDLER_H

#include <stdio.h>

#define CONFIG_ENTRY "^[[:space:]]*([[:alnum:]]*){1}[[:space:]]*+=[[:space:]]*(.*){1}\\n$"
#define CFG_TEST "^([a-zA-Z_]\\w*)\\s*=\\s*([^#]+?)\n$"

/*
 * Reads values from config files
 * @param: const chat *filename
 * @returns: -1 on failure, 0 else;
 */
int readCfg(const char *filename);

/*
 * Initialises not currently stored values from default
 */
void initconfig();
#endif //REVERSI_FILEIOHANDLER_H
