//
// Created by kad on 26.11.15.
//

#include "thinker.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//array groesse breite*breite
//0 ist links oben
//(breite^2)-1 ist rechts unten




int oben(int position, int groesse, int feld[])
{
    if ((position - groesse)<0){
        return -groesse;
    } else if (feld[(position - groesse)] == 0){	//leer
        return -groesse;
    } else if (feld[(position - groesse)] == 2){	//gegner
        return (1 +  oben(position - groesse, groesse, feld));
    } else if (feld[(position - groesse)] == 1){	//eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug oben(), Position %i", position );
        return -groesse;
    }
}

int linksoben(int position, int groesse, int feld[])
{
    if ((position - groesse)<0){
        return -groesse;
    } else if (feld[(position - (groesse+1))] == 0){	//leer
        return -groesse;
    } else if (feld[(position - (groesse+1))] == 2){	//gegner
        return (1 +  linksoben(position - (groesse+1), groesse, feld));
    } else if (feld[(position - (groesse+1))] == 1){	//eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug linksoben(), Position %i", position );
        return -groesse;
    }
}

int rechtsoben(int position, int groesse, int feld[])
{
    if ((position - groesse)<0){
        return -groesse;
    } else if (feld[(position - (groesse-1))] == 0){	//leer
        return -groesse;
    } else if (feld[(position - (groesse-1))] == 2){	//gegner
        return (1 +  rechtsoben(position - (groesse-1), groesse, feld));
    } else if (feld[(position - (groesse-1))] == 1){	//eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug rechtsoben(), Position %i", position );
        return -groesse;
    }
}

int unten(int position, int groesse, int feld[])
{
    if ((position + groesse)>(groesse * groesse)){
        return -groesse;
    } else if (feld[(position + (groesse-0))] == 0){	//leer
        return -groesse;
    } else if (feld[(position + (groesse-0))] == 2){	//gegner
        return (1 +  unten(position + (groesse-0), groesse, feld));
    } else if (feld[(position + (groesse-0))] == 1){	//eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug unten(), Position %i", position );
        return -groesse;
    }
}

int linksunten(int position, int groesse, int feld[])
{
    if ((position + groesse)>(groesse * groesse)){
        return -groesse;
    } else if (feld[(position + (groesse-1))] == 0){	//leer
        return -groesse;
    } else if (feld[(position + (groesse-1))] == 2){	//gegner
        return (1 +  linksunten(position + (groesse-1), groesse, feld));
    } else if (feld[(position + (groesse-1))] == 1){	//eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug linksunten(), Position %i", position );
        return -groesse;
    }
}

int rechtsunten(int position, int groesse, int feld[])
{
    if ((position + groesse)>(groesse * groesse)){
        return -groesse;
    } else if (feld[(position + (groesse+1))] == 0){	//leer
        return -groesse;
    } else if (feld[(position + (groesse+1))] == 2){	//gegner
        return (1 +  rechtsunten(position + (groesse+1), groesse, feld));
    } else if (feld[(position + (groesse+1))] == 1){	//eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug rechtsunten(), Position %i", position );
        return -groesse;
    }
}

int links(int position, int groesse, int feld[])
{
    if (position % groesse == 0){
        return -groesse;
    } else if (feld[position -1] == 0){	//leer
        return -groesse;
    } else if (feld[position -1] == 2){	//gegner
        return (1 +  links(position -1, groesse, feld));
    } else if (feld[position -1] == 1){	//eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug links(), Position %i", position );
        return -groesse;
    }
}

int rechts(int position, int groesse, int feld[])
{
    if (position % groesse == (groesse -1)){
        return -groesse;
    } else if (feld[position +1] == 0){	//leer
        return -groesse;
    } else if (feld[position +1] == 2){	//gegner
        return (1 +  rechts(position + 1, groesse, feld));
    } else if (feld[position +1] == 1){	//eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug links(), Position %i", position );
        return -groesse;
    }
}

int randomAI (int zuege[], int anzahl){
    time_t t;
    srand((unsigned) time(&t));
    int r = rand() % anzahl;
    return zuege[r];
}

char* convertMove(char * spielzug, int position, int groesse){
    spielzug[0] = (position % groesse) + 65;
    int y = groesse - (position / groesse);
    if (y>9) {
        spielzug[1] = 49;
        spielzug[2] = y % 10 + 48;
    }
    else {
        spielzug[1] = y+48;
        spielzug[2] = 0;
    }
    //printf("Zug: %s\n", spielzug);
    return spielzug;
}

int gueltigeZuege(int feld[], int groesse){
    int i, n, gain, sizeZuege, zug;

    int * zuege = malloc(groesse*groesse * sizeof(int));
    if (NULL == zuege) {
        fprintf(stderr, "malloc failed\n");
        return(-1);
    }

    sizeZuege = 0;
    for (i=0; i<(groesse*groesse);i++){
        if (feld[i]==0)
        {
            gain = 0;
            if ((i - groesse)>0){
                if ((n=oben(i, groesse,feld))>0){
                    gain = gain + n;
                }
                if ((n=linksoben(i, groesse,feld))>0){
                    gain = gain + n;
                }
                if ((n=rechtsoben(i, groesse,feld))>0){
                    gain = gain + n;
                }
            }
            if ((i + groesse)<(groesse * groesse)){
                if ((n=unten(i, groesse,feld))>0){
                    gain = gain + n;
                }
                if ((n=linksunten(i, groesse,feld))>0){
                    gain = gain + n;
                }
                if ((n=rechtsunten(i, groesse,feld))>0){
                    gain = gain + n;
                }
            }
            if (i % groesse != 0){
                if ((n=links(i, groesse,feld))>0){
                    gain = gain + n;
                }
            }
            if (i % groesse != (groesse -1)){
                if ((n=rechts(i, groesse,feld))>0){
                    gain = gain + n;
                }
            }
            if (gain > 0)
            {
                zuege[sizeZuege] = i;
                sizeZuege++;
            }}
    }

    if (sizeZuege == 0)
    { 
        printf("kein gültiger Zug berechenbar");
        return -1;  }
    zug = randomAI(zuege,sizeZuege);
    free(zuege);
    return zug;
}

