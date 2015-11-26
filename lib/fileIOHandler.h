//
// Created by kad on 26.11.15.
//

#ifndef REVERSI_FILEIOHANDLER_H
#define REVERSI_FILEIOHANDLER_H
#define CONFIG_ENTRY "^[[:space:]]*([[:alnum:]]*){1}[[:space:]]*+=[[:space:]]*(.*){1}\\n$"

/*
 * Reads values from config files
 * @param: const chat *filename
 * @returns: -1 on failure, 0 else;
 */
int readCfg(const char *filename);

#endif //REVERSI_FILEIOHANDLER_H
