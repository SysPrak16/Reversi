//
// Created by kad on 24.11.15.
//

#ifndef REVERSI_GLOBAL_H
#define REVERSI_GLOBAL_H
#define BUF_SIZE 1024

#define DEF_PORTNUMBER 1357
#define DEF_HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
/*
 * Protocol Globals - Client
 */
#define CVERSION "VERSION 1.0\n"
#define CTHINK "THINKING\n"
#define CWAIT "OKWAIT\n"
#define PLAYER1 "PLAYER\n"
/*
 * Protocol Globals - Server
 */
#define MNM_SERVER "+ MNM Gameserver v1.0 accepting connections"
#define VERSION_ACCEPTED "+ Client version accepted - please send Game-ID to join"
#define CON_TIMEOUT "- Socket timeout - please be quicker next time"
#define CON_TIMEOUT_ERR_MSG "ERROR: Server connection timeout!"
#define GAMEKIND_ERROR_MSG "ERROR: Wrong game type!"
#define VERSION_ERROR "- We implement protocol version 1.0"
#define VERSION_ERROR_MSG "ERROR: Wrong client version detected! Expected version 1.x!"
#define INVALID_MOVE "- Invalid position."
#define INVALID_MOVE_MSG "ERROR: Invalid move! You died!\n"
#define NO_FREE_PLAYER "- No free computer player found for that game - exiting"
#define NO_FREE_PLAYER_MSG "ERROR: No free computer player found! Exiting!"
#define NO_GAME_ERROR "- exiting"
#define NO_GAME_ERROR_MSG "ERROR: No game found for this ID! Exiting!"
#define SEND_FAILED "ERROR: SENDING TO SERVER FAILED! -EXITING!"

/*
 * Defaults
 */
#define DEFAULT_CONFIG "default.conf"
#define GAME_DATA_ID 1492
#define FIELD_ID 1493
#define PLAYERS_ID 1494
/*
 * Error Messages
 */
#define NO_ID_ERROR "ERROR: No GameID has been provided!\n"
#define ENTER_GAME_ID "Please enter your 11 digit GameID: \n"
#define SHM_ERROR "ERROR: Could not attach the shared memory!\n"
#define SIG_ERROR "ERROR: kill()\n"
#define DETATCH_ERROR "ERROR: Could not detach the Shared memory segment!\n"

#define HOW_TO_USE "\n\tThis application expects the following arguments:\n\t\t-g\tGameID retrievable vial creating a new game at \"http://sysprak.priv.lab.nm.ifi.lmu.de/sysprak/\"\n\t\t\tHas to have the length of 11 characters.\n\n\tThis application accepts the following arguments:\n\t\t-c\tYou can provide the name of a configuration file here.\n\t\t\tMaximum allowed amount of characters is 256.\n\t\t-d\tAn integer which determines the game difficulty.\n\t\t\tCurrently ranges from 0-2."
#define HOW_TO_CONFIG "\n\tTo provide a custom configuration, use the argument -c\n\t\tYou can provide the name of a configuration file here.\n\t\tMaximum allowed amount of characters is 256.\n"
#define HOW_TO_AI "\n\t Did you know you can adjust the AI by using the -d flag?\n\t\tIt takes integers from 0-2, where 0 is the easiest AI.\n"

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
    int field[16*16];                 //Aktuelles Feld
}gameField_t;

typedef struct {
    int write;
    int read;
} pipe_t;

typedef struct{
    char gamename[BUF_SIZE];    //Spielname
    int playerCount;            //Anzahl Spieler
    int playerID;               //client player ID
    int gameOver;               //Gameover!
    int movesize;
    pipe_t pipe;

    pid_t processIDParent;      //Process ID Parent
    pid_t processIDChild;       //Process ID Child

    //==========Shared Memory segment IDs START =========//
    int shmid_gameData;         //own ID
    int shmid_field;            //field
    int shmid_players;           //player info
    //==========Shared Memory segment IDs END ==========//
    int sig_exit;
    int thinkerMakeMove;        //signal
    int server_major;
    int server_minor;
    int moveOK;
} gameData_t;

gameData_t *gameData;

/*
 * NEW SHARED MEMORY ENDS
 */


/*
 * AI Types:
 */
#define AI_RAND 0
#define AGAINST_RAND "\tYou decided to play against an extremely easy AI.\n"
#define AI_ENHANCED_RAND 1
#define AGAINST_ENH_RAND "\tYou decided to play against a very easy AI.\n"
#define AI_MAX_GAIN 2
#define AGAINST_MAX_GAIN "\tYou decided to play against an easy AI.\n"


/*
 * Struct, which represents the config file types
 */
typedef struct{
    char gameID[11];
    char gamekind[256];
    unsigned short port;
    char hostname[256];
    int aiType;
}config_t;

#endif //REVERSI_GLOBAL_H
