#include "performConnection.h"

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netdb.h>
#include <stdarg.h>
#include <netinet/in.h>

#include "config.h"
#include "connector.h"


typedef enum {
    PROLOG_NEXT_UNKNOWN,
    PROLOG_NEXT_GAMENAME,
} PrologNextMessage;


// returns non zero if str1 and str2 are equal

int strbeg(const char *str1, const char *str2)
{
    return strncmp(str1, str2, strlen(str2)) == 0;
}


int performConnection(int sfd)
{
    char buffer[2048];
    int n;

    int readFromPipe = 0;

    char playername[32] = { 0 };
    player_t *players = NULL;
    piecelist_t *pieceList = NULL;

    PrologNextMessage nextMessage = PROLOG_NEXT_UNKNOWN;

    fd_set set;
    FD_ZERO(&set);

    for (;;) {
        FD_SET(sfd, &set);
        FD_SET(game->pipe.in, &set);

        int result = select(sfd > game->pipe.in ? sfd + 1 : game->pipe.in + 1, &set, NULL, NULL, NULL);
        if(result==-1) {
            perror("Fehler: select()");
            return -1;
        } else if (result == 0) {
            //no data
        } else {
            if(FD_ISSET(sfd, &set)) {
                if ((n = receiveMessage(sfd, buffer, sizeof(buffer))) < 2) {
                    fprintf(stderr, "Fehler: Ungültige Antwort vom Server!\n");
                    return -1;
                }

                for (const char *line = strtok(buffer, "\n"); line != NULL; line = strtok(NULL, "\n")) {
                    if (strbeg(line, "- ")) {
                        fprintf(stderr, "Fehler: %s\n", &line[2]);
                        return -1;
                    }

                    // skip '+ '

                    line += 2;

                    if (nextMessage == PROLOG_NEXT_GAMENAME) {
                        nextMessage = PROLOG_NEXT_UNKNOWN;

                        strncpy(game->gamename, line, 20);
                        fprintf(stdout, "Der Spielname lautet '%s'.\n", game->gamename);

                        sendMessage(sfd, "PLAYER");

                    } else if (strbeg(line, "MNM Gameserver")) {
                        int versionMajor, versionMinor;
                        sscanf(line, "%*s %*s %*c%d%*c%d", &versionMajor, &versionMinor);

                        fprintf(stdout, "Der Gameserver v%d.%d ist bereit fuer den Spielbeitritt.\n", versionMajor, versionMinor);

                        sendMessage(sfd, "VERSION %d.%d", VERSION_MAJOR, VERSION_MINOR);

                    } else if (strbeg(line, "Client version accepted")) {
                        sendMessage(sfd, "ID %s", config.gameid);

                    } else if (strbeg(line, "PLAYING")) {
                        if (strcmp(&line[8], config.gamekind) != 0) {
                            fprintf(stderr, "Fehler: Client Spiel und Server Spiel nicht kompatibel:\n\tClient: %s\n\tServer: %s\n", config.gamekind, &line[8]);
                            return -1;
                        }

                        fprintf(stdout, "Das aktuelle Spiel ist '%s'.\n", config.gamekind);

                        nextMessage = PROLOG_NEXT_GAMENAME;

                    } else if (strbeg(line, "YOU")) {
                        sscanf(line, "%*s %d %31c", &game->clientPlayerId, playername);

                        fprintf(stdout, "Ihnen wurde vom Server die Spielernummer %d (%s) zugeteilt.\n", game->clientPlayerId, playername);

                    } else if (strbeg(line, "TOTAL")) {
                        // parse number of players

                        sscanf(line, "%*s %d", &game->numPlayers);

                        fprintf(stdout, "Es nehmen insgesamt %d Spieler am Spiel teil.\n", game->numPlayers);

                        // create new SM segment big enough to store player information

                        game->shmidPlayers = shmget(IPC_PRIVATE, sizeof(player_t) * game->numPlayers, IPC_CREAT | 0666);
                        if (game->shmidPlayers == -1) {
                            perror("Fehler: shmget()");
                            return -1;
                        }

                        // get address of SM segment

                        players = shmat(game->shmidPlayers, NULL, 0);
                        if (players == (player_t *) -1) {
                            perror("Fehler: shmat()");
                            return -1;
                        }

                        // copy the client player information (from 'YOU' server message) into the SM segment

                        strcpy(players[game->clientPlayerId].name, playername);
                        players[game->clientPlayerId].id = game->clientPlayerId;

                        // process next lines (players) in buffer

                        for (line = strtok(NULL, "\n"); !strbeg(line, "+ ENDPLAYERS"); line = strtok(NULL, "\n")) {
                            if (line[0] != '+') {
                                fprintf(stderr, "Fehler: %s\n", line);
                                return -1;
                            }

                            // skip '+ '

                            line += 2;

                            // parse player id

                            int playerId;
                            sscanf(line, "%d", &playerId);

                            // player id is index to n-th player in the array

                            if (playerId > game->numPlayers) {
                                fprintf(stderr, "Fehler: playerId > numplayers!\n");
                                return -1;
                            }

                            player_t *player = &players[playerId];
                            player->id = playerId;

                            // parse player state. it's the next number after the last space of the line

                            char *lastSpace = strrchr(line, ' ');
                            sscanf(lastSpace, "%d", (int *) &player->state);
                            *lastSpace = 0;

                            // parse player name

                            sscanf(line, "%*d %31c", player->name);

                            fprintf(stdout, "Spieler %d (%s) ist %sbereit.\n", player->id, player->name, player->state == PLAYER_READY ? "" : "noch nicht ");
                        }

                        fprintf(stdout, "Ende des Prolog.\n");

                    } else if (strbeg(line, "WAIT")) {
                        sendMessage(sfd, "OKWAIT");

                    } else if (strbeg(line, "MOVEOK")) {


                    } else if (strbeg(line, "MOVE")) {
                        float maxMoveTime;
                        sscanf(line, "%*s%f", &maxMoveTime);

                        fprintf(stdout, "Die maximale Zeit für diesen Spielzug beträgt %.2f Sekunden.\n", maxMoveTime / 1000);

                    } else if (strbeg(line, "CAPTURE")) {
                        // create SM segment for piece list

                        game->shmidPieceList = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0666);
                        if (game->shmidPieceList == -1) {
                            perror("Fehler: shmget()");
                            return -1;
                        }

                        // get address of SM segment

                        pieceList = (piecelist_t *) shmat(game->shmidPieceList, NULL, 0);
                        if (pieceList == (piecelist_t *) -1) {
                            perror("Fehler: shmat()");
                            return -1;
                        }

                        sscanf(line, "%*s%d", &pieceList->numPiecesToCapture);

                        fprintf(stdout, "Anzahl zu schlagender Steine: %d.\n", pieceList->numPiecesToCapture);

                    } else if (strbeg(line, "PIECELIST")) {
                        if (!game->gameover)
                            sendMessage(sfd, "THINKING");


                        // parse number of players and number of pieces per player
                        // format: PIECELIST <numPlayers>,<numPieces>

                        sscanf(line, "%*s %d,%d", &pieceList->numPlayers, &pieceList->numPieces);

                        //fprintf(stdout, "Spieleranzahl: %d\nAnzahl Steine pro Spieler: %d\n", pieceList->numPlayers, pieceList->numPieces);

                        // create SM segment for piece positions

                        game->shmidPieces = shmget(IPC_PRIVATE, pieceList->numPlayers * pieceList->numPieces * sizeof(piece_t), IPC_CREAT | 0666);
                        if (game->shmidPieces == -1) {
                            perror("Fehler: shmget");
                            return -1;
                        }

                        piece_t *pieces = (piece_t *) shmat(game->shmidPieces, NULL, 0);
                        if (pieces == (piece_t *) -1) {
                            perror("Fehler: shmat");
                            return -1;
                        }

                        // process next lines (pieces) in buffer

                        for (line = strtok(NULL, "\n"); !strbeg(line, "+ ENDPIECELIST"); line = strtok(NULL, "\n")) {
                            if (line[0] != '+') {
                                fprintf(stderr, "Fehler: %s\n", line);
                                return -1;
                            }

                            // skip '+ '

                            line += 2;

                            if (strbeg(line, "PIECE")) {
                                // parse player id, piece id and it's position
                                // format: PIECE<playerId>.<pieceId> <state>

                                int playerId, pieceId;
                                char position[3] = { 0 };

                                sscanf(line, "%*5c%d.%d %2c", &playerId, &pieceId, position);

                                //fprintf(stdout, "Spielstein %d von Spieler %d: %s\n", pieceId, playerId, position);

                                if (playerId < pieceList->numPlayers && pieceId < pieceList->numPieces) {
                                    // the SM segment shmpieces will need to contain all information about any given piece
                                    // actually it just needs the position and the player that it belongs to. but the ID might be needed in the future
                                    pieces[playerId * pieceList->numPieces + pieceId].playerID =  playerId;
                                    pieces[playerId * pieceList->numPieces + pieceId].pieceID = pieceId;
                                    strcpy(pieces[playerId * pieceList->numPieces + pieceId].position, position);
                                }
                            }
                        }

                        game->thinkerMayRead = true;

                        kill(game->thinker, SIGUSR1);

                    } else if (strbeg(line, "OKTHINK")) {
                        readFromPipe = 1;

                    } else if (strbeg(line, "GAMEOVER")) {
                        game->gameover = 1;

                        int playerId;
                        char playerName[32] = { 0 };

                        // player id and name are optional and only provided if a player wins the game
                        // format: GAMEOVER [playerId] [playerName]

                        if (sscanf(line, "%*s %d %31c", &playerId, playerName) == 2) {
                            if (playerId == game->clientPlayerId)
                                fprintf(stdout, "Glückwunsch! Sie haben das Spiel gewonnen!\n");
                            else
                                fprintf(stdout, "Spieler #%d (%s) hat das Spiel gewonnen!\n", playerId, playerName);
                        }

                    } else if (strbeg(line, "QUIT")) {
                        fprintf(stdout, "Das Spiel ist beendet.\n");
                        return 0;

                    } else {
                        fprintf(stdout, "Warnung: Unbehandelte Nachricht vom Server:\n\t%s\n", line);
                    }
                }
            }


            if (readFromPipe && FD_ISSET(game->pipe.in, &set)) {
                readFromPipe = 0;

                char move[6];
                read(game->pipe.in, move, sizeof(move));

                sendMessage(sfd, "PLAY %s", move);
            }
        }
    }

    return 0;
}
