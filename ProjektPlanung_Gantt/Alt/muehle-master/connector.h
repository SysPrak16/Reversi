#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <sys/types.h>
#include <stdio.h>

typedef enum {
    PLAYER_BUSY,
    PLAYER_READY,
} PlayerState;

typedef struct {
    int         id;
    char        name[32];
    PlayerState state;
} player_t;

typedef struct {
    int         numPlayers;
    int         numPieces;
    int         numPiecesToCapture;
} piecelist_t;

typedef struct {
    int         pieceID;
    int         playerID;
    char        position[3];
} piece_t;

typedef struct {
    int         in;
    int         out;
} pipe_t;

typedef struct {
    char        gamename[32];
    int         numPlayers;
    int         clientPlayerId;
    int         gameover;
    pid_t       connector;
    pid_t       thinker;
 //   int         shmidFlag;
    int         shmidGame;
    int         shmidPlayers;
    int         shmidPieceList;
    int         shmidPieces;
    int         thinkerMayRead;
    pipe_t      pipe;
} game_t;

game_t *game;

void initgame();

/*!
 * \brief Reads a '\n' terminated message from a socket into the specified buffer
 * \param sfd       socket to read from
 * \param buffer    pointer to buffer
 * \param size      size of buffer
 * \return -1 on failure otherwise number of characters read from socket sfd
 */
int receiveMessage(int sfd, char *buffer, int size);

/*!
 * \brief Writes a '\n' terminated message into a socket
 * \param sfd       socket to write into
 * \param format    format specifier
 * \return -1 on failure otherwise number of characters written into socket sfd
 */
int sendMessage(int sfd, char *format, ...);

/*!
 * \brief Establishes a connection to a game (server) using the global configuration parameters
 * \return -1 on failure otherwise 0
 */
int connector();

#endif // CONNECTOR_H
