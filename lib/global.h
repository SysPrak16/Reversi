//
// Created by kad on 24.11.15.
//

#ifndef REVERSI_GLOBAL_H
#define REVERSI_GLOBAL_H
#define BUF_SIZE 512

#define DEF_GAMEKINDNAME "Reversi"
#define DEF_PORTNUMBER 1357
#define DEF_HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
/*
 * Protocol Globals - Client
 */
#define CVERSION "VERSION 1.0\n"
#define CTHINK "THINKING\n"
#define CWAIT "OKWAIT\n"
#define GID "ID "
#define PLAYER0 "PLAYER 0\n"
#define PLAYER1 "PLAYER 1\n"
#define PLAYER2 "PLAYER 2\n"
/*
 * Protocol Globals - Server
 */
#define MNM_SERVER "+ MNM Gameserver v1.0 accepting connections"
#define VERSION_ACCEPTED "+ Client version accepted - please send Game-ID to join"
#define CON_TIMEOUT "- Socket timeout - please be quicker next time"
#define CON_TIMEOUT_ERR_MSG "ERROR: Server connection timeout!\n"
#define GAMEKIND_ERROR_MSG "ERRR: Wrong game type!\n"
#define VERSION_ERROR "- We implement protocol version 1.0"
#define VERSION_ERROR_MSG "ERROR: Wrong client version detected! Expected version 1.x!"
#define INVALID_MOVE "- Invalid position."
#define INVALID_MOVE_MSG "ERROR: Invalid move! You died!"
#define NO_FREE_PLAYER "- No free computer player found for that game - exiting"
#define NO_FREE_PLAYER_MSG "ERROR: No free computer player found! Exiting!"
#define NO_GAME_ERROR "- exiting"
#define NO_GAME_ERROR_MSG "ERROR: No game found for this ID! Exiting!"
/*
 * @NAME MTS (Memory to Share)
 *
 */
typedef struct{
    int *field;
    int height;
    int width;
}MTS;

/*
 * AI Types:
 */
#define AI_RAND 0
/*
 * Struct, which represents the config file types
 */
typedef struct{
    char gameID[11];
    char gamename[256];
    unsigned short port;
    char hostname[256];
    int aiType;
    MTS field;
}config_t;

#endif //REVERSI_GLOBAL_H
