//
// Created by kad on 24.11.15.
//

#ifndef REVERSI_GLOBAL_H
#define REVERSI_GLOBAL_H
#define BUF_SIZE 1024

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
#define SHM_ERROR "ERROR: Could not attach the shared memory!\n"
#define SIG_ERROR "ERROR: kill()\n"
#define FATAL_ERROR "ERROR: FATAL EXCEPTION!"
#define DETATCH_ERROR "ERROR: Could not detach the Shared memory segment!\n"

#include <sys/types.h>
#include <sys/ipc.h>

/*
 * NEW SHARED MEMORY
 */
typedef struct{
    int number;                 //Spielernummer
    char name[BUF_SIZE];        //Spielername
    int flag;                   //Ready Flag
}player_t;

typedef struct{
    int height;                 //Höhe
    int width;                  //Breite
    int *field;                 //Aktuelles Feld
}gameField_t;

typedef struct {
    int         in;
    int         out;
} pipe_t;

typedef struct{
    char gamename[BUF_SIZE];    //Spielname
    int playerCount;            //Anzahl Spieler
    int playerID;               //client player ID
    int gameOver;               //Gameover!
    int movetime;
    pipe_t pipe;

    pid_t processIDParent;      //Process ID Parent
    pid_t processIDChild;       //Process ID Child

    //==========Shared Memory segment IDs START =========//
    int shmid_gameData;         //own ID
    int shmid_field;            //field
    int shmid_players;           //player info
    //==========Shared Memory segment IDs END ==========//
    gameField_t *fieldAddress;
    int thinkerMakeMove;        //signal
} gameData_t;

gameData_t *gameData;

/*
 * NEW SHARED MEMORY ENDS
 */


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
}config_t;

#endif //REVERSI_GLOBAL_H
