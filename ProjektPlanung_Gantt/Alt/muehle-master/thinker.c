#include "thinker.h"

#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* waitpid */
#include <stdio.h>     /* printf, perror */
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* _exit, fork */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
//#include <time.h>

#include "connector.h"
#include "config.h"

int think()
{
    // create the shared memory segment

    int shmid = shmget(IPC_PRIVATE, sizeof(game_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Fehler: shmget()");
        return -1;
    }

    // attach the shared memory segment

    game = (game_t *) shmat(shmid, NULL, 0);
    if (game == (game_t *) -1) {
        perror("Fehler: shmat()");
        return -1;
    }

    initgame();
    game->shmidGame = shmid;

    // create pipe

    int mypipe[2];

    if (pipe(mypipe) == -1) {
        perror("Fehler: pipe()");
        return EXIT_FAILURE;
    }

    game->pipe.in  = mypipe[0];
    game->pipe.out = mypipe[1];

    pid_t pid = fork();

    if (pid == -1) {
        // If fork() returns -1, an error happened.
        perror("Fehler: fork()");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // If fork() returns 0, we are in the child process.
        // call connector

        close(game->pipe.out);
        connector();

        _exit(EXIT_SUCCESS);  // exit() is unreliable here, so _exit must be used (!)

    } else {
        game->connector      = pid;
        game->thinker        = getpid();
        game->thinkerMayRead = false;

        close(game->pipe.in);

        // the thinker has to wait for kill signal with SIGUSR1 from connector
        // then evaluate shm-flag and start thinking

        signal(SIGUSR1, &handler);

        // wait for the connector process to finish

        int connectorStatus;
        waitpid(pid, &connectorStatus, 0);
    }

    return EXIT_SUCCESS;
}

void handler(int parameter) {
    if (parameter == SIGUSR1 && game->thinkerMayRead) {
        game->thinkerMayRead = false;

        char move[6];
        memset(move, 0, sizeof(move));

        if (config.aiMode == AI_DUMB)
            calculateRandomMove(move);
        else
            calculateMove(move);

        write(game->pipe.out, move, 6);

        // draw board only after we calculated our response. saves time

        drawBoard();
    }
}



int charToInt(char c)
{
    return (int) c - (int) '0';
}

char intToChar(int i)
{
    return (char) ( (int) '0' + i );
}



piece_t *getPiece(piecelist_t *pieceList, piece_t *pieces, int playerId, int pieceId)
{
    return &pieces[(pieceList->numPieces * playerId) + pieceId];
}

int getRingIndices(const char *position, int *ringIndex, int *fieldIndex)
{
    if (strcmp(position, "A") == 0 || strcmp(position, "C") == 0)
        return 0;

    *ringIndex  = charToInt(position[0]) - ('A' - '0');
    *fieldIndex = charToInt(position[1]);

    return 1;
}


void calculateRandomMove(char *move)
{
    piecelist_t *pieceList = shmat(game->shmidPieceList, NULL, 0);
    piece_t *pieces = shmat(game->shmidPieces, NULL, 0);

    char fields[3][8];

    // initialize all fields with '+'

    memset(fields, '+', sizeof(fields));

    int ringIndex;
    int fieldIndex;

    // set occupied fields

    for (int i = 0; i < game->numPlayers; ++i) {
        for (int j = 0; j < pieceList->numPieces; ++j) {
            piece_t *piece = getPiece(pieceList, pieces, i, j);

            if (!getRingIndices(piece->position, &ringIndex, &fieldIndex))
                continue;

            fields[ringIndex][fieldIndex] = i;
        }
    }


    if (pieceList->numPiecesToCapture) {
        // we need to capture a piece

        for (int i = 0; i < pieceList->numPlayers; ++i) {
            if (i == game->clientPlayerId)
                continue;

            for (int j = 0; j < pieceList->numPieces; ++j) {
                piece_t* piece = getPiece(pieceList, pieces, i, j);
                if (strcmp(piece->position, "A") == 0 || strcmp(piece->position, "C") == 0)
                    continue; // cannot capture available or already captured piece

                sprintf(move, "%s", piece->position);
            }
        }


    } else {
        // check if we have any available pieces left

        piece_t *availablePiece = NULL;

        for (int i = 0; i < pieceList->numPieces; ++i) {
            piece_t* piece = getPiece(pieceList, pieces, game->clientPlayerId, i);
            if (strcmp(piece->position, "A") == 0) {
                availablePiece = piece;
                break;
            }
        }

        if (!availablePiece) {
            // fprintf(stdout, "Moving piece!\n");

            // find next piece which can be moved

            for (int i = 0; i < pieceList->numPieces; ++i) {
                piece_t* piece = getPiece(pieceList, pieces, game->clientPlayerId, i);

                // skip (available) and captured pieces

                if (!getRingIndices(piece->position, &ringIndex, &fieldIndex))
                    continue;

                // check field +1

                int neighbourFieldIndex = fieldIndex == 7 ? 0 : fieldIndex + 1;

                if (fields[ringIndex][neighbourFieldIndex] == '+') {
                    sprintf(move, "%s:%c%d", piece->position, 'A' + ringIndex, neighbourFieldIndex);
                    return;
                }

                // check field -1

                neighbourFieldIndex = fieldIndex == 0 ? 7 : fieldIndex - 1;

                if (fields[ringIndex][neighbourFieldIndex] == '+') {
                    sprintf(move, "%s:%c%d", piece->position, 'A' + ringIndex, neighbourFieldIndex);
                    return;
                }

                if (fieldIndex % 2 != 0) {
                    if (ringIndex == 0 || ringIndex == 2) {
                        // check b ring if current piece is in a or c ring

                        int neighbourRingIndex = 1;

                        if (fields[neighbourRingIndex][fieldIndex] == '+') {
                            sprintf(move, "%s:%c%d", piece->position, 'A' + neighbourRingIndex, fieldIndex);
                            return;
                        }

                    } else {
                        // check a and c ring if current piece is in b ring

                        int neighbourRingIndex = 0;

                        if (fields[neighbourRingIndex][fieldIndex] == '+') {
                            sprintf(move, "%s:%c%d", piece->position, 'A' + neighbourRingIndex, fieldIndex);
                            return;
                        }

                        neighbourRingIndex = 2;

                        if (fields[neighbourRingIndex][fieldIndex] == '+') {
                            sprintf(move, "%s:%c%d", piece->position, 'A' + neighbourRingIndex, fieldIndex);
                            return;
                        }
                    }
                }
            }

            // couldn't move any existing pieces; jump with first piece to first position

            piece_t* freePiece = NULL;

            for (int i = 0; i < pieceList->numPieces; ++i) {
                piece_t *piece = getPiece(pieceList, pieces, game->clientPlayerId, i);
                if (strcmp(piece->position, "A") == 0 || strcmp(piece->position, "C") == 0)
                    continue;

                freePiece = piece;
                break;
            }

            // choose the next free field as move

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 8; ++j) {
                    if (fields[i][j] == '+') {
                        sprintf(move, "%s:%c%d", freePiece->position, 'A' + i, j);
                        return;
                    }
                }
            }


        } else {
            // fprintf(stdout, "Placing piece!\n");

            // choose the next free field as move

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 8; ++j) {
                    if (fields[i][j] == '+') {
                        sprintf(move, "%c%d", 'A' + i, j);
                        return;
                    }
                }
            }
        }
    }
}

void calculateMove(char *move) {
    printf("%s()\n", __func__);

    piecelist_t *pieceList = shmat(game->shmidPieceList, NULL, 0);
    piece_t *pieces = shmat(game->shmidPieces, NULL, 0);

    // 3 rings á 8 fields
    // first ring:  A (0-7)
    // second ring: B (8-15)
    // third ring:  C (15-23)

    char fields[3 * 8];

    int playedPieces = pieceList->numPieces;
    int capturedPieces = 0;

    int priorityList[3 * 8];
    int priorityListEnemy[3 * 8];
    int finalPriority[3 * 8];
    int movePriority[3 * 8];

    memset(priorityList, 0, sizeof(priorityList));
    memset(priorityListEnemy, 0, sizeof(priorityListEnemy));
    memset(finalPriority, 0 ,sizeof(finalPriority));
    memset(movePriority, 0, sizeof(movePriority));

    // initialize all fields with '+'
    memset(fields, '+', sizeof(fields));

    int alphaToNumOffset = (int) 'A' - (int) '0';

    for (int i = 0; i < game->numPlayers; ++i) {
        //char playerNumberAsChar = intToChar(i);

        for (int j = 0; j < pieceList->numPieces; ++j) {
            piece_t *piece = &pieces[(i * pieceList->numPieces) + j];

            if (strcmp(piece->position, "A") == 0) {
                playedPieces = playedPieces-1;
                continue; // available; skip
            }
            else if (strcmp(piece->position, "C") == 0) {
                capturedPieces = capturedPieces+1;
                continue; // captured; skip

            int ringIndex = charToInt(piece->position[0]) - alphaToNumOffset;
            int ringFieldIndex = charToInt(piece->position[1]);

            fields[(ringIndex * 8) + ringFieldIndex] = i; //playerNumberAsChar;
            }
        }
        if (pieceList->numPiecesToCapture) {
            // we need to capture a piece

            for (int i = 0; i < pieceList->numPlayers; ++i) {
                if (i == game->clientPlayerId)
                    continue;

                for (int j = 0; j < pieceList->numPieces; ++j) {
                    piece_t* piece = getPiece(pieceList, pieces, i, j);
                    int ringIndex = charToInt(piece->position[0]) - alphaToNumOffset;
                    int ringFieldIndex = charToInt(piece->position[1]);
                    if (strcmp(piece->position, "A") == 0 || strcmp(piece->position, "C") == 0) {
                        continue; // cannot capture available or already captured piece
                    }
                    //RingIndex comparisons
                    if(ringFieldIndex == 0 || ringFieldIndex == 1) {
                        if(fields[(ringIndex * 8) + 7] == piece->pieceID                        /*player on upper-left corner detects his own piece one position to the left*/) {
                            movePriority[(ringIndex * 8)] = movePriority[(ringIndex * 8)] - 1;
                        }
                        if(fields[(ringIndex * 8) + 6] == piece->pieceID                   /*player on upper-left corner detects his own piece two positions to the left*/) {
                            movePriority[(ringIndex * 8)] = movePriority[(ringIndex * 8)] - 1;
                        }
                        if(fields[(ringIndex * 8) + 7] == piece->pieceID                   /*player on position right to upper-left corner detects his own piece two positions to the left*/) {
                            movePriority[(ringIndex * 8) + 1] = movePriority[(ringIndex * 8) + 1] - 1;
                        }
                        if(fields[ringIndex * 8] == piece->pieceID                         /*player on position right to upper-left corner detects his own piece one position to the left*/) {
                            movePriority[(ringIndex * 8) + 1] = movePriority[(ringIndex * 8) + 1] - 1;
                        }
                        if(fields[(ringIndex * 8) + ringFieldIndex+2] == piece->pieceID    /*player detects his own piece two positions to the right*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = movePriority[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if(fields[(ringIndex * 8) + ringFieldIndex+1] == piece->pieceID    /*player detects his own piece one position to the right*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = movePriority[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                    }
                    else if((ringFieldIndex == (ringIndex * 8) + 7) || (ringFieldIndex == (ringIndex * 8) + 6)) {
                        if(fields[((ringIndex-1) * 8) + 8] == piece->pieceID                    /*player on position left to upper-left corner detects his own piece one position to the right*/) {
                            movePriority[((ringIndex-1) * 8) + 7] = movePriority[((ringIndex-1) * 8) + 7] - 1;
                        }
                        if(fields[((ringIndex-1) * 8) + 9] == piece->pieceID               /*player on position left to upper-left corner detects his own piece two positions to the right*/) {
                            movePriority[((ringIndex-1) * 8) + 7] = movePriority[((ringIndex-1) * 8) + 7] - 1;
                        }
                        if(fields[((ringIndex-1) * 8) + 8] == piece->pieceID               /*player on position two left to upper-left corner detects his own piece two positions to the right*/) {
                            movePriority[((ringIndex-1) * 8) + 6] = movePriority[((ringIndex-1) * 8) + 6] - 1;
                        }
                        if(fields[((ringIndex-1) * 8) + 7] == piece->pieceID               /*player on position two left to upper-left corner detects his own piece one position to the right*/) {
                            movePriority[((ringIndex-1) * 8) + 6] = movePriority[((ringIndex-1) * 8) + 6] - 1;
                        }
                        if(fields[(ringIndex * 8) + ringFieldIndex-2] == piece->pieceID    /*player detects his own piece two positions to the left*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if(fields[(ringIndex * 8) + ringFieldIndex-1] == piece->pieceID    /*player detects his own piece one position to the left*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                    }
                    else {
                        if(fields[(ringIndex * 8) + ringFieldIndex+2] == piece->pieceID    /*player detects his own piece two positions to the right*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if(fields[(ringIndex * 8) + ringFieldIndex+1] == piece->pieceID    /*player detects his own piece one position to the right*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if(fields[(ringIndex * 8) + ringFieldIndex-2] == piece->pieceID    /*player detects his own piece two positions to the left*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if(fields[(ringIndex * 8) + ringFieldIndex-1] == piece->pieceID    /*player detects his own piece one position to the left*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                    }
                    //Ring comparisons
                    if(ringFieldIndex % 2 == 1) {
                        //one ring distance, same FieldIndex comparison
                        if((fields[(ringIndex * 8) + ringFieldIndex + 8] == piece->pieceID) && (ringIndex == 0) /*player on A detects his own piece on B*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if((fields[(ringIndex * 8) + ringFieldIndex + 8] == piece->pieceID) && (ringIndex == 1) /*player on B detects his own piece on C*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if((fields[(ringIndex * 8) + ringFieldIndex - 8] == piece->pieceID) && (ringIndex == 2) /*player on C detects his own piece on B*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if((fields[(ringIndex * 8) + ringFieldIndex - 8] == piece->pieceID) && (ringIndex == 1) /*player on B detects his own piece on A*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        //two rings distance, same FieldIndex comparison
                        if((fields[(ringIndex * 8) + ringFieldIndex + 16] == piece->pieceID) && (ringIndex == 0) /*player on A detects his own piece on C*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        if((fields[(ringIndex * 8) + ringFieldIndex - 16] == piece->pieceID) && (ringIndex == 2) /*player on C detects his own piece on A*/) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                        //one ring distance, different FieldIndex comparison
                            //Comparison inwards from A or B
                        if( ((fields[(ringIndex * 8) + ringFieldIndex + 9] == piece->pieceID) || (fields[(ringIndex * 8) + ringFieldIndex + 7] == piece->pieceID)) && ((ringIndex == 0) || (ringIndex == 1)) ) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                            //Comparison outwards from B or C
                        if( ((fields[(ringIndex * 8) + ringFieldIndex - 9] == piece->pieceID) || (fields[(ringIndex * 8) + ringFieldIndex - 7] == piece->pieceID)) && ((ringIndex == 1) || (ringIndex == 2)) ) {
                            movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                        }
                    }
                }
                int tempMin = 0;
                for(int k = 0; k<2; k++){
                    for(int l = 0; l<23; l++){
                        if(tempMin >= movePriority[l]){
                            tempMin = movePriority[l];
                        }
                    }
                }
                for(int m = 0; m<pieceList->numPieces; m++){
                    piece_t* piece = getPiece(pieceList, pieces, i, m);
                    int ringIndex = charToInt(piece->position[0]) - alphaToNumOffset;
                    int ringFieldIndex = charToInt(piece->position[1]);
                    if(movePriority[(ringIndex*8)+ringFieldIndex]==tempMin){
                        sprintf(move, "%s", piece->position);
                        capturedPieces++;
                    }
                }
            }


        }

        else if (playedPieces < pieceList->numPieces){
                //There are still available pieces
            for (int l = 0; l < pieceList->numPieces; ++l) {

                piece_t *piece = &pieces[(i * pieceList->numPieces) + l];
                int ringIndex = charToInt(piece->position[0]) - alphaToNumOffset;
                int ringFieldIndex = charToInt(piece->position[1]);



                    if (game->clientPlayerId == piece->pieceID)/*your piece*/ {
                        //RingIndex comparisons
                        if(ringFieldIndex == 0 || ringFieldIndex == 1) {
                            if(fields[(ringIndex * 8) + 7] == piece->pieceID                        /*player on upper-left corner detects his own piece one position to the left*/) {
                                priorityList[(ringIndex * 8) + 6] = priorityList[(ringIndex * 8) + 6] + 2;
                                movePriority[(ringIndex * 8)] = movePriority[(ringIndex * 8)] - 1;
                            }
                            if(fields[(ringIndex * 8) + 6] == piece->pieceID                   /*player on upper-left corner detects his own piece two positions to the left*/) {
                                priorityList[(ringIndex * 8) + 7] = priorityList[(ringIndex * 8) + 7] + 2;
                                movePriority[(ringIndex * 8)] = movePriority[(ringIndex * 8)] - 1;
                            }
                            if(fields[(ringIndex * 8) + 7] == piece->pieceID                   /*player on position right to upper-left corner detects his own piece two positions to the left*/) {
                                priorityList[ringIndex * 8] = priorityList[ringIndex * 8]+2;
                                movePriority[(ringIndex * 8) + 1] = movePriority[(ringIndex * 8) + 1] - 1;
                            }
                            if(fields[ringIndex * 8] == piece->pieceID                         /*player on position right to upper-left corner detects his own piece one position to the left*/) {
                                priorityList[(ringIndex * 8) + 7] = priorityList[(ringIndex * 8) + 7]+2;
                                movePriority[(ringIndex * 8) + 1] = movePriority[(ringIndex * 8) + 1] - 1;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex+2] == piece->pieceID    /*player detects his own piece two positions to the right*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex+1] = priorityList[(ringIndex * 8) + ringFieldIndex+1]+2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = movePriority[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex+1] == piece->pieceID    /*player detects his own piece one position to the right*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex+2] = priorityList[(ringIndex * 8) + ringFieldIndex+2]+2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = movePriority[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                        }
                        else if((ringFieldIndex == (ringIndex * 8) + 7) || (ringFieldIndex == (ringIndex * 8) + 6)) {
                            if(fields[((ringIndex-1) * 8) + 8] == piece->pieceID                    /*player on position left to upper-left corner detects his own piece one position to the right*/) {
                                priorityList[((ringIndex-1) * 8) + 6] = priorityList[((ringIndex-1) * 8) + 6] + 2;
                                movePriority[((ringIndex-1) * 8) + 7] = movePriority[((ringIndex-1) * 8) + 7] - 1;
                            }
                            if(fields[((ringIndex-1) * 8) + 9] == piece->pieceID               /*player on position left to upper-left corner detects his own piece two positions to the right*/) {
                                priorityList[((ringIndex-1) * 8) + 8] = priorityList[((ringIndex-1) * 8) + 8] + 2;
                                movePriority[((ringIndex-1) * 8) + 7] = movePriority[((ringIndex-1) * 8) + 7] - 1;
                            }
                            if(fields[((ringIndex-1) * 8) + 8] == piece->pieceID               /*player on position two left to upper-left corner detects his own piece two positions to the right*/) {
                                priorityList[((ringIndex-1) * 8) + 7] = priorityList[((ringIndex-1) * 8) + 7] + 2;
                                movePriority[((ringIndex-1) * 8) + 6] = movePriority[((ringIndex-1) * 8) + 6] - 1;
                            }
                            if(fields[((ringIndex-1) * 8) + 7] == piece->pieceID               /*player on position two left to upper-left corner detects his own piece one position to the right*/) {
                                priorityList[((ringIndex-1) * 8) + 8] = priorityList[((ringIndex-1) * 8) + 8] + 2;
                                movePriority[((ringIndex-1) * 8) + 6] = movePriority[((ringIndex-1) * 8) + 6] - 1;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex-2] == piece->pieceID    /*player detects his own piece two positions to the left*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex-1] = priorityList[(ringIndex * 8) + ringFieldIndex-1] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex-1] == piece->pieceID    /*player detects his own piece one position to the left*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex-2] = priorityList[(ringIndex * 8) + ringFieldIndex-2] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                        }
                        else {
                            if(fields[(ringIndex * 8) + ringFieldIndex+2] == piece->pieceID    /*player detects his own piece two positions to the right*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex+1] = priorityList[(ringIndex * 8) + ringFieldIndex+1] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex+1] == piece->pieceID    /*player detects his own piece one position to the right*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex+2] = priorityList[(ringIndex * 8) + ringFieldIndex+2] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex-2] == piece->pieceID    /*player detects his own piece two positions to the left*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex-1] = priorityList[(ringIndex * 8) + ringFieldIndex-1] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex-1] == piece->pieceID    /*player detects his own piece one position to the left*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex-2] = priorityList[(ringIndex * 8) + ringFieldIndex-2] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                        }
                        //Ring comparisons
                        if(ringFieldIndex % 2 == 1) {
                            //one ring distance, same FieldIndex comparison
                            if((fields[(ringIndex * 8) + ringFieldIndex + 8] == piece->pieceID) && (ringIndex == 0) /*player on A detects his own piece on B*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex + 16] = priorityList[(ringIndex * 8) + ringFieldIndex + 16] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if((fields[(ringIndex * 8) + ringFieldIndex + 8] == piece->pieceID) && (ringIndex == 1) /*player on B detects his own piece on C*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex - 8] = priorityList[(ringIndex * 8) + ringFieldIndex - 8] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if((fields[(ringIndex * 8) + ringFieldIndex - 8] == piece->pieceID) && (ringIndex == 2) /*player on C detects his own piece on B*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex - 16] = priorityList[(ringIndex * 8) + ringFieldIndex - 16] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if((fields[(ringIndex * 8) + ringFieldIndex - 8] == piece->pieceID) && (ringIndex == 1) /*player on B detects his own piece on A*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex + 8] = priorityList[(ringIndex * 8) + ringFieldIndex + 8] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            //two rings distance, same FieldIndex comparison
                            if((fields[(ringIndex * 8) + ringFieldIndex + 16] == piece->pieceID) && (ringIndex == 0) /*player on A detects his own piece on C*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex + 8] = priorityList[(ringIndex * 8) + ringFieldIndex + 8] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            if((fields[(ringIndex * 8) + ringFieldIndex - 16] == piece->pieceID) && (ringIndex == 2) /*player on C detects his own piece on A*/) {
                                priorityList[(ringIndex * 8) + ringFieldIndex - 8] = priorityList[(ringIndex * 8) + ringFieldIndex - 8] + 2;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                            //one ring distance, different FieldIndex comparison
                                //Comparison inwards from A or B
                            if( ((fields[(ringIndex * 8) + ringFieldIndex + 9] == piece->pieceID) || (fields[(ringIndex * 8) + ringFieldIndex + 7] == piece->pieceID)) && ((ringIndex == 0) || (ringIndex == 1)) ) {
                                priorityList[(ringIndex * 8) + ringFieldIndex + 8] = priorityList[(ringIndex * 8) + ringFieldIndex + 8] + 1;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                                //Comparison outwards from B or C
                            if( ((fields[(ringIndex * 8) + ringFieldIndex - 9] == piece->pieceID) || (fields[(ringIndex * 8) + ringFieldIndex - 7] == piece->pieceID)) && ((ringIndex == 1) || (ringIndex == 2)) ) {
                                priorityList[(ringIndex * 8) + ringFieldIndex - 8] = priorityList[(ringIndex * 8) + ringFieldIndex - 8] + 1;
                                movePriority[(ringIndex * 8) + ringFieldIndex] = priorityList[(ringIndex * 8) + ringFieldIndex] - 1;
                            }
                        }
                        //Diagonal comparisons
                        if(ringFieldIndex % 2 == 0) {
                            if((ringFieldIndex == 0) || (ringFieldIndex == 2) || (ringFieldIndex == 8) || (ringFieldIndex == 10) || (ringFieldIndex == 16) || (ringFieldIndex == 18)) {
                                if(fields[(ringIndex * 8) + ringFieldIndex + 4] == piece->pieceID) {
                                    priorityList[(ringIndex * 8) + ringFieldIndex + 2] = priorityList[(ringIndex * 8) + ringFieldIndex + 2]+1;
                                    if((ringFieldIndex == 0) || (ringFieldIndex == 8) || (ringFieldIndex == 16)) {
                                        priorityList[(ringIndex * 8) + ringFieldIndex + 6] = priorityList[(ringIndex * 8) + ringFieldIndex + 6]+1;
                                    }
                                    else {
                                        priorityList[(ringIndex * 8) + ringFieldIndex - 2] = priorityList[(ringIndex * 8) + ringFieldIndex - 2]+1;
                                    }
                                }
                            }
                            else {
                                if(fields[(ringIndex * 8) + ringFieldIndex - 4] == piece->pieceID) {
                                    priorityList[(ringIndex * 8) + ringFieldIndex - 2] = priorityList[(ringIndex * 8) + ringFieldIndex - 2]+1;
                                    if((ringFieldIndex == 6) || (ringFieldIndex == 14) || (ringFieldIndex == 22)) {
                                        priorityList[(ringIndex * 8) + ringFieldIndex - 6] = priorityList[(ringIndex * 8) + ringFieldIndex - 6]+1;
                                    }
                                    else {
                                        priorityList[(ringIndex * 8) + ringFieldIndex + 2] = priorityList[(ringIndex * 8) + ringFieldIndex + 2]+1;
                                    }
                                }
                            }
                        }
                    }
                    else /*enemy piece*/ {
                        //RingIndex comparisons
                        if((ringFieldIndex == ringIndex * 8) || ringFieldIndex == (ringIndex * 8) + 1) {
                            if(fields[(ringIndex * 8) + 7] == piece->pieceID                        /*enemy on upper-left corner detects his own piece one position to the left*/) {
                                priorityListEnemy[(ringIndex * 8) + 6] = priorityListEnemy[(ringIndex * 8) + 6] + 2;
                            }
                            if(fields[(ringIndex * 8) + 6] == piece->pieceID                   /*enemy on upper-left corner detects his own piece two positions to the left*/) {
                                priorityListEnemy[(ringIndex * 8) + 7] = priorityListEnemy[(ringIndex * 8) + 7] + 2;
                            }
                            if(fields[(ringIndex * 8) + 7] == piece->pieceID                   /*enemy on position right to upper-left corner detects his own piece two positions to the left*/) {
                                priorityListEnemy[ringIndex * 8] = priorityListEnemy[ringIndex * 8]+2;
                            }
                            if(fields[ringIndex * 8] == piece->pieceID                         /*enemy on position right to upper-left corner detects his own piece one position to the left*/) {
                                priorityListEnemy[(ringIndex * 8) + 7] = priorityListEnemy[(ringIndex * 8) + 7]+2;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex+2] == piece->pieceID    /*enemy detects his own piece two positions to the right*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex+1] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex+1]+2;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex+1] == piece->pieceID    /*enemy detects his own piece one position to the right*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex+2] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex+2]+2;
                            }
                        }
                        else if((ringFieldIndex == (ringIndex * 8) + 7) || (ringFieldIndex == (ringIndex * 8) + 6)) {
                            if(fields[((ringIndex-1) * 8) + 8] == piece->pieceID                    /*enemy on position left to upper-left corner detects his own piece one position to the right*/) {
                                priorityListEnemy[((ringIndex-1) * 8) + 6] = priorityListEnemy[((ringIndex-1) * 8) + 6] + 2;
                            }
                            if(fields[((ringIndex-1) * 8) + 9] == piece->pieceID               /*enemy on position left to upper-left corner detects his own piece two positions to the right*/) {
                                priorityListEnemy[((ringIndex-1) * 8) + 8] = priorityListEnemy[((ringIndex-1) * 8) + 8] + 2;
                            }
                            if(fields[((ringIndex-1) * 8) + 8] == piece->pieceID               /*enemy on position two left to upper-left corner detects his own piece two positions to the right*/) {
                                priorityListEnemy[((ringIndex-1) * 8) + 7] = priorityListEnemy[((ringIndex-1) * 8) + 7]+2;
                            }
                            if(fields[((ringIndex-1) * 8) + 7] == piece->pieceID               /*enemy on position two left to upper-left corner detects his own piece one position to the right*/) {
                                priorityListEnemy[(ringIndex * 8) + 8] = priorityListEnemy[((ringIndex-1) * 8) + 8]+2;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex-2] == piece->pieceID    /*enemy detects his own piece two positions to the left*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex-1] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex-1]+2;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex-1] == piece->pieceID    /*enemy detects his own piece one position to the left*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex-2] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex-2]+2;
                            }
                        }
                        else {
                            if(fields[(ringIndex * 8) + ringFieldIndex+2] == piece->pieceID    /*enemy detects his own piece two positions to the right*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex+1] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex+1]+2;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex+1] == piece->pieceID    /*enemy detects his own piece one position to the right*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex+2] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex+2]+2;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex-2] == piece->pieceID    /*enemy detects his own piece two positions to the left*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex-1] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex-1]+2;
                            }
                            if(fields[(ringIndex * 8) + ringFieldIndex-1] == piece->pieceID    /*enemy detects his own piece one position to the left*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex-2] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex-2]+2;
                            }
                        }
                        //Ring comparisons
                        if(ringFieldIndex % 2 == 1) {
                            //one ring distance, same FieldIndex comparison
                            if((fields[(ringIndex * 8) + ringFieldIndex + 8] == piece->pieceID) && (ringIndex == 0) /*enemy on A detects his own piece on B*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 16] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 16]+2;
                            }
                            if((fields[(ringIndex * 8) + ringFieldIndex + 8] == piece->pieceID) && (ringIndex == 1) /*enemy on B detects his own piece on C*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 8] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 8]+2;
                            }
                            if((fields[(ringIndex * 8) + ringFieldIndex - 8] == piece->pieceID) && (ringIndex == 2) /*enemy on C detects his own piece on B*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 16] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 16]+2;
                            }
                            if((fields[(ringIndex * 8) + ringFieldIndex - 8] == piece->pieceID) && (ringIndex == 1) /*enemy on B detects his own piece on A*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 8] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 8]+2;
                            }
                            //two rings distance, same FieldIndex comparison
                            if((fields[(ringIndex * 8) + ringFieldIndex + 16] == piece->pieceID) && (ringIndex == 0) /*enemy on A detects his own piece on C*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 8] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 8]+2;
                            }
                            if((fields[(ringIndex * 8) + ringFieldIndex - 16] == piece->pieceID) && (ringIndex == 2) /*enemy on C detects his own piece on A*/) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 8] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 8]+2;
                            }
                            //one ring distance, different FieldIndex comparison
                                //Comparison inwards from A or B
                            if( ((fields[(ringIndex * 8) + ringFieldIndex + 9] == piece->pieceID) || (fields[(ringIndex * 8) + ringFieldIndex + 7] == piece->pieceID)) && ((ringIndex == 0) || (ringIndex == 1)) ) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 8] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 8]+1;
                            }
                                //Comparison outwards from B or C
                            if( ((fields[(ringIndex * 8) + ringFieldIndex - 9] == piece->pieceID) || (fields[(ringIndex * 8) + ringFieldIndex - 7] == piece->pieceID)) && ((ringIndex == 1) || (ringIndex == 2)) ) {
                                priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 8] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 8]+1;
                            }
                        }
                        //Diagonal comparisons
                        if(ringFieldIndex % 2 == 0) {
                            if((ringFieldIndex == 0) || (ringFieldIndex == 2) || (ringFieldIndex == 8) || (ringFieldIndex == 10) || (ringFieldIndex == 16) || (ringFieldIndex == 18)) {
                                if(fields[(ringIndex * 8) + ringFieldIndex + 4] == piece->pieceID) {
                                    priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 2] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 2]+1;
                                    if((ringFieldIndex == 0) || (ringFieldIndex == 8) || (ringFieldIndex == 16)) {
                                        priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 6] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 6]+1;
                                    }
                                    else {
                                        priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 2] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 2]+1;
                                    }
                                }
                            }
                            else {
                                if(fields[(ringIndex * 8) + ringFieldIndex - 4] == piece->pieceID) {
                                    priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 2] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 2]+1;
                                    if((ringFieldIndex == 6) || (ringFieldIndex == 14) || (ringFieldIndex == 22)) {
                                        priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 6] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex - 6]+1;
                                    }
                                    else {
                                        priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 2] = priorityListEnemy[(ringIndex * 8) + ringFieldIndex + 2]+1;
                                    }
                                }
                            }
                        }

                    }

            }

        }
    }

    int tempMax = 0;
    int tempMinm = 0;

    for(int n = 0;n<2;n++){
        int temppossibleMoves = 0;
        int temppossiblePieces = 0;
        for(int m = 0;m<23;m++) {
            finalPriority[m] = priorityList[m] + priorityListEnemy[m];
            if(tempMinm <= movePriority[m]){
                tempMinm = movePriority[m];
            }
            if(tempMax <= finalPriority[m]){
                tempMax = finalPriority[m];
                temppossibleMoves++;
            }
        }
        int selectedMoves [temppossibleMoves];
        int selectedPieces [temppossiblePieces];
        memset(selectedMoves, 0 ,sizeof(selectedMoves));
        memset(selectedPieces, 0, sizeof(selectedPieces));
        //srand(time(NULL)); //not longer needed
        //int r = rand() % temppossibleMoves; //not longer needed
        for(int i = 0;i<temppossibleMoves;i++){
            for(int p = 0;p<23;p++) {
            if(finalPriority[i] == tempMax){
                selectedMoves[i] = p;
            }
            }
        }
        for(int i = 0;i<temppossiblePieces;i++){
            for(int p = 0;p<23;p++){
            if(finalPriority[i] == tempMinm){
                selectedPieces[i] = p;
            }
            }
        }
        int playedPieces2 = pieceList->numPieces;
        int capturedPieces2  = 0;
        for (int j = 0; j < pieceList->numPieces; ++j) {
            piece_t *piece = &pieces[(game->clientPlayerId * pieceList->numPieces) + j];

            if (strcmp(piece->position, "A") == 0) {
                playedPieces2 = playedPieces-1;
                continue;
            }
            else if (strcmp(piece->position, "C") == 0) {
                capturedPieces2 = capturedPieces2+1;
                continue;
            }
        }
        //min-max comparison to find one of the best possible moves
        if((pieceList->numPieces - capturedPieces2) == 3)/*comparison for ending/jump phase*/{
            for(int i = 0;i<pieceList->numPieces;i++){
                piece_t *piece = &pieces[(game->clientPlayerId * pieceList->numPieces) + i];
                int ringIndex = charToInt(piece->position[0]) - alphaToNumOffset;
                int ringFieldIndex = charToInt(piece->position[1]);

                if(movePriority[(ringIndex*8)+ringFieldIndex] == tempMinm)/*is selected piece a low-priority piece?*/{
                    for(int k=0; k<23; k++){
                        if(finalPriority[k] == tempMax){
                            sprintf(move, "%s:%c%d", piece->position, ringIndex, ringFieldIndex+1);
                            break; //valid move completed;
                        }
                    }
                }
            }
        }
        else if(playedPieces2 < pieceList->numPieces)/*comparison for opening/place phase*/{
            for (int i = 0; i < pieceList->numPieces; ++i) {
                piece_t* piece = getPiece(pieceList, pieces, game->clientPlayerId, i);
                if (strcmp(piece->position, "A") == 0) {
                        for(int k=0; k<23; k++){
                            if(fields[k]==0 || fields[k]==1){
                                continue;
                            }
                            else if(finalPriority[k] == tempMax){
                                if(k < 8){
                                    piece->position[0] ='A';
                                    piece->position[1] = intToChar(k);
                                }
                                else if(k >=16){
                                    piece->position[0] ='C';
                                    piece->position[1] = intToChar(k-16);
                                }
                                else {
                                    piece->position[0] ='B';
                                    piece->position[1] = intToChar(k-8);
                                }
                                sprintf(move, "%s", piece->position);
                                break; //valid move completed;
                            }
                        }
                }
            }

        }
        else /*comparison for main/move phase*/ {
            for(int k = 0; k>tempMinm; k++){
                int done = 0;
                for(int j = 0; j<3;j++){
                    for(int i = 0;i<pieceList->numPieces;i++){
                        piece_t *piece = &pieces[(game->clientPlayerId * pieceList->numPieces) + i];
                        int ringIndex = charToInt(piece->position[0]) - alphaToNumOffset;
                        int ringFieldIndex = charToInt(piece->position[1]);

                        if(movePriority[(ringIndex*8)+ringFieldIndex] == tempMinm)/*is selected piece a low-priority piece?*/{
                            if(ringFieldIndex == 0)/*left upper corner special case*/{
                                if(finalPriority[(ringIndex * 8) + 1] == tempMax){
                                    sprintf(move, "%s:%c%d", piece->position, ringIndex, ringFieldIndex+1);
                                    done = 1;
                                    break; //valid move completed;
                                }
                                else if(finalPriority[(ringIndex * 8) + 7] == tempMax){
                                    sprintf(move, "%s:%c%d", piece->position, ringIndex, ringFieldIndex+7);
                                    done = 1;
                                    break; //valid move completed
                                }
                            }
                            else if(ringFieldIndex == 7)/*left edge special case*/{
                                if(finalPriority[((ringIndex - 1) * 8) + 8] == tempMax){
                                    sprintf(move, "%s:%c%d", piece->position, ringIndex, ringFieldIndex-7);
                                    done = 1;
                                    break; //valid move completed
                                }
                                else if(finalPriority[(ringIndex * 8) + 6] == tempMax){
                                    sprintf(move, "%s:%c%d", piece->position, ringIndex, ringFieldIndex-1);
                                    done = 1;
                                    break; //valid move completed
                                }
                            }
                            else if(ringFieldIndex % 2 == 1)/*edge case*/{
                                if(ringIndex == 0 || ringIndex == 1){
                                    if(finalPriority[(ringIndex * 8) + 8] == tempMax){
                                        sprintf(move, "%s:%c%d", piece->position, ringIndex+1, ringFieldIndex);
                                        done = 1;
                                        break; //valid move completed
                                    }
                                }
                                else if(ringIndex == 1 || ringIndex == 2){
                                    if(finalPriority[(ringIndex * 8) - 8] == tempMax){
                                        sprintf(move, "%s:%c%d", piece->position, ringIndex-1, ringFieldIndex);
                                        done = 1;
                                        break; //valid move completed
                                    }
                                }
                            }
                            else /*corner case*/ {
                                if(finalPriority[(ringIndex * 8) + ringFieldIndex + 1] == tempMax){
                                    sprintf(move, "%s:%c%d", piece->position, ringIndex, ringFieldIndex+1);
                                    done = 1;
                                    break; //valid move completed
                                }
                                else if(finalPriority[(ringIndex * 8) + ringFieldIndex - 1] == tempMax){
                                    sprintf(move, "%s:%c%d", piece->position, ringIndex, ringFieldIndex-1);
                                    done = 1;
                                    break; //valid move completed
                                }
                            }
                        }
                    }
                    if(done){
                        break;
                    }
                    tempMinm++;
                }
                if(done){
                    break;
                }
                tempMax--;
            }
        }
    }
/*(no longer needed target-field selection)
        if(selectedMoves[r] < 8){
            prematureMove[0] ='A';
            prematureMove[1] = intToChar(selectedMoves[r]);
        }
        else if(selectedMoves[r] >=16){
            prematureMove[0] ='C';
            prematureMove[1] = intToChar(selectedMoves[r]-16);
        }
        else {
            prematureMove[0] ='B';
            prematureMove[1] = intToChar(selectedMoves[r]-8);
        }
*/


}



void drawBoard()
{
    piecelist_t *pieceList = shmat(game->shmidPieceList, NULL, 0);
    piece_t *pieces = shmat(game->shmidPieces, NULL, 0);

    // 3 rings á 8 fields

    char fields[3][8];

    // initialize all fields with '+'

    memset(fields, '+', sizeof(fields));

    int ringIndex;
    int fieldIndex;

    // loop through each player and player's pieces and set the fields occupied by them

    for (int i = 0; i < game->numPlayers; ++i) {
        char playerNumberAsChar = intToChar(i);

        for (int j = 0; j < pieceList->numPieces; ++j) {
            piece_t *piece = getPiece(pieceList, pieces, i, j);

            if (!getRingIndices(piece->position, &ringIndex, &fieldIndex))
                continue; // skip available and captured pieces

            fields[ringIndex][fieldIndex] = playerNumberAsChar;
        }
    }

    // finally draw our board

    printf("%c-----------%c-----------%c\n", fields[0][0], fields[0][1], fields[0][2]);
    printf("|           |           |\n");
    printf("|   %c-------%c-------%c   |\n", fields[1][0], fields[1][1], fields[1][2]);
    printf("|   |       |       |   |\n");
    printf("|   |   %c---%c---%c   |   |\n", fields[2][0], fields[2][1], fields[2][2]);
    printf("|   |   |       |   |   |\n");
    printf("%c---%c---%c       %c---%c---%c\n", fields[0][7], fields[1][7], fields[2][7], fields[2][3], fields[1][3], fields[0][3]);
    printf("|   |   |       |   |   |\n");
    printf("|   |   %c---%c---%c   |   |\n", fields[2][6], fields[2][5], fields[2][4]);
    printf("|   |       |       |   |\n");
    printf("|   %c-------%c-------%c   |\n", fields[1][6], fields[1][5], fields[1][4]);
    printf("|           |           |\n");
    printf("%c-----------%c-----------%c\n", fields[0][6], fields[0][5], fields[0][4]);
}
