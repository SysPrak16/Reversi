//
// Created by kad on 24.11.15.
//

#ifndef REVERSI_GLOBAL_H
#define REVERSI_GLOBAL_H
#define BUF_SIZE 256

#define DEF_GAMEKINDNAME "Reversi"
#define DEF_PORTNUMBER 1357
#define DEF_HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"

/*
 * Struct, which represents the config file types
 */
/*typedef struct {
    char gamename[7];
    int portnumber;
    char hostname[256];
} config;*/

typedef struct {
    char gamename[256];
    unsigned short port;
    char hostname[256];
} config;

#endif //REVERSI_GLOBAL_H
