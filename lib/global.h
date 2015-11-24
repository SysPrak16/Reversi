//
// Created by kad on 24.11.15.
//

#ifndef REVERSI_GLOBAL_H
#define REVERSI_GLOBAL_H

#define GAMEKINDNAME "Reversi"
#define PORTNUMBER 1357
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"

typedef struct {
    char *gamename;
    int *portnumber;
    char *hostname;
} config;


#endif //REVERSI_GLOBAL_H
