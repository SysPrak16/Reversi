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
#define VERSION_ERROR_MSG "ERROR: Wrong client version detected! Expected version 1.x!\n"
#define INVALID_MOVE "- Invalid position."
#define INVALID_MOVE_MSG "ERROR: Invalid move! You died!\n"
#define NO_FREE_PLAYER "- No free computer player found for that game - exiting"
#define NO_FREE_PLAYER_MSG "ERROR: No free computer player found! Exiting!\n"
#define NO_GAME_ERROR "- exiting"
#define NO_GAME_ERROR_MSG "ERROR: No game found for this ID! Exiting!\n"

/*
 * Defaults
 */
#define DEFAULT_CONFIG "default.conf"

/*
 * Error Messages
 */
#define NO_ID_ERROR "ERROR: No GameID has been provided!\n"
#define ENTER_GAME_ID "Please enter your 11 digit GameID: \n"

#include <sys/types.h>
#include <sys/ipc.h>
/*
 * @NAME MTS (Memory to Share)
 *
 */

#define TEXT_SZ 2048            //Testing

typedef struct{
    int *field;
    int height;
    int width;
    int written_by_you;         //Testing
    char some_text[TEXT_SZ];    //Testing
}MTS;

typedef struct{
    char gamename[BUF_SIZE];    //Spielname
    int playernum;              //Spielernummer
    int players;                //Anzahl Spieler
    pid_t PIDP;                 //Process ID Parent
    pid_t PIDC;                 //Process ID Child

    int flag;                   //For Navigating

    int height;
    int width;
}uSHY;

typedef struct{
    int playernum;                  //The Players Number
    char playername[BUF_SIZE];      //The Players Name
    int flag;                       //Player registered 0 for no 1 for yes
}player_info;

typedef struct{
    int height;                 //Höhe
    int width;                  //Breite
    int *field;                 //Aktuelles Feld
}gameField;

extern int uSHYflag;

typedef struct{
    int number;                 //Spielernummer
    char name[BUF_SIZE];        //Spielername
    int flag;                   //Ready Flag
}player;

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
