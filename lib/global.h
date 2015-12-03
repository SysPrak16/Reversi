//
// Created by kad on 24.11.15.
//

#ifndef REVERSI_GLOBAL_H
#define REVERSI_GLOBAL_H
#define BUF_SIZE 2028

#define DEF_GAMEKINDNAME "Reversi"
#define DEF_PORTNUMBER 1357
#define DEF_HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
#define CVERSION "VERSION 1.0\n"
#define CTHINK "THINKING\n"
#define CWAIT "OKWAIT\n"
#define GID "ID "
#define PLAYER0 "PLAYER 0\n"
#define PLAYER1 "PLAYER 1\n"
#define PLAYER2 "PLAYER 2\n"

/*
 * Struct, which represents the config file types
 */
typedef struct{
    char gamename[256];
    unsigned short port;
    char hostname[256];
}config_t;

#endif //REVERSI_GLOBAL_H
