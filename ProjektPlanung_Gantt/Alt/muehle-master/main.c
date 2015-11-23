#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "constants.h"
#include "config.h"
#include "connector.h"
#include "thinker.h"


int cleanupshm(int shmid)
{
    struct shmid_ds buf;

    if (shmid == -1)
        return 0;

    if (shmctl(shmid, IPC_RMID, &buf) == -1) {
        perror("Fehler: shmctl()");
        return -1;
    }

    return 0;
}

void cleanup()
{
    fprintf(stdout, "Cleanup.\n");

    if (game == NULL || game == (game_t *) -1)
        return;

    if (cleanupshm(game->shmidPieces) == -1) {
        fprintf(stderr, "Fehler beim Freigeben des Shared Memory Segments (Spielsteine)!\n");
    }

    if (cleanupshm(game->shmidPieceList) == -1) {
        fprintf(stderr, "Fehler beim Freigeben des Shared Memory Segments (Spielsteinliste)!\n");
    }

    if (cleanupshm(game->shmidPlayers) == -1) {
        fprintf(stderr, "Fehler beim Freigeben des Shared Memory Segments (Spieler)!\n");
    }

    if (cleanupshm(game->shmidGame) == -1) {
        fprintf(stderr, "Fehler beim Freigeben des Shared Memory Segments (Spielinfo)!\n");
    }

    //if (cleanupshm(game->shmidFlag) == -1) {
    //    fprintf(stderr, "Fehler beim Freigeben des Shared Memory Segments (Spielinfo)!\n");
    //}
}

void printUsage(const char *arg)
{
    fprintf(stdout, "Verwendung: %s [-g] Game-ID [-c] Konfigurationsdatei (optional)\n", arg);
}


int main(int argc, char *argv[])
{
    fprintf(stdout, "Muehle Client gestartet...\n\n");

    game = NULL;
    atexit(&cleanup);
    initconfig();

    // set default configuration file

    char filename[256];
    strcpy(filename, DEFAULT_CONFIG);

    // parse command line arguments

    int opt;

    AiMode aiMode = AI_DUMB;

    while ((opt = getopt(argc, argv, "dg:c:")) != -1) {
        switch (opt) {
        case 'g':
            if (strlen(optarg) != GAMEID_LENGTH) {
                fprintf(stderr, "Fehler: Die Game-ID muss aus %d Zeichen bestehen.\n", GAMEID_LENGTH);
                exit(EXIT_FAILURE);
            } else {
                strcpy(config.gameid, optarg);
            }
            break;
        case 'c':
            strncpy(filename, optarg, sizeof(filename));
            break;
        case 'd':
            aiMode = AI_AWESOME;
            break;
        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // exit if no game id was supplied

    if (strlen(config.gameid) == 0) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Uebergebene Parameter:\n");
    fprintf(stdout, "Game-ID: %s\n", config.gameid);
    fprintf(stdout, "Kondigurationsdatei: %s\n\n", filename);

    fprintf(stdout, "Lese Konfigurationsdatei...\n");
    if (readconfig(filename) == -1) {
        fprintf(stderr, "Fehler beim Einlesen der Konfigurationsdatei!\n");
        exit(EXIT_FAILURE);
    }

    config.aiMode = aiMode;

    fprintf(stdout, "hostname = %s\n", config.hostname);
    fprintf(stdout, "port     = %d\n", config.port);
    fprintf(stdout, "gamekind = %s\n", config.gamekind);
    fprintf(stdout, "ai       = %s\n", config.aiMode == AI_DUMB ? "dumb" : "awesome");
    fprintf(stdout, "\n");

    return think();
}
