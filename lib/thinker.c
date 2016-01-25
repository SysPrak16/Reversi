//
// Created by kad on 26.11.15.
//
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include "thinker.h"
#include "connector.h"

//extern MTS myMTS;
extern config_t config;

/*
 * Cleanup process
 */

int cleanupGameData(){
    printf("Reached: cleanupGameData()\n");
    if(shmctl (gameData->shmid_gameData, IPC_RMID, NULL)<0)
    {
        perror(DETATCH_ERROR);
        return -1;
    }
    printf("All memories retconed!\n");
    return 0;
}

/*
 * datahandler for signal call:
 */
void handler(int parameter) {
    if (parameter == SIGUSR1 && gameData->thinkerMakeMove==1) {
        gameData->thinkerMakeMove = -1;
        printf("%s\n",gameData->gamename);
        char move[4];
        memset(move, 0, sizeof(move));
        write(gameData->pipe.out, move, 4);
        //TODO: draw the board
        //TODO: drawBoard();
    } else{
        printf("Reached: cleanupGameData\n\tID to detach: %i\n", gameData->shmid_gameData);
        if(gameData->thinkerMakeMove<=0) {
            //perror(FATAL_ERROR);
            cleanupGameData();
        }
    }
}

int think1()
{
    //creating the shared memory segment:
    int shmid = shmget(IPC_PRIVATE, sizeof(gameData_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror(SHM_ERROR);
        return -1;
    }
    //Actually attaching it:
    gameData = (gameData_t *) shmat(shmid, NULL, 0);
    if (gameData == (gameData_t *) -1) {
        perror(SHM_ERROR);
        return -1;
    }

    //initialise the gameData
    initgameData();
    gameData->shmid_gameData=shmid;

    //create the pipe
    int p[2];
    if (pipe(p) == -1) {
        perror("ERROR: Could not open the pipe!");
        return -1;
    }

    gameData->pipe.in  = p[0];
    gameData->pipe.out = p[1];
    //forking:

    pid_t pid = fork();
    if (pid==-1){
        perror("ERROR: fork()");
        return -1;
    }else if(pid==0){
        //child:
        gameData->processIDChild=getpid();
        //close pipe in:
        close(gameData->pipe.in);
        //connect:
        return connectToServer();
    } else {
        //Parent:
        gameData->processIDParent=getpid();
        gameData->thinkerMakeMove=0;
        //closing pipe out:
        close(gameData->pipe.out);

        //signal:
        signal(SIGUSR1, &handler);

        int connectorStatus;
        waitpid(pid, &connectorStatus, 0);
    }
    return EXIT_SUCCESS;
}



int oben(int position, int groesse, int feld[])
{
    if ((position - groesse)<0){                    //oberer Rand
        return -groesse;
    } else if (feld[(position - groesse)] == 0){	//oben leer
        return -groesse;
    } else if (feld[(position - groesse)] == 2){	//oben gegner
        return (1 +  oben(position - groesse, groesse, feld));
    } else if (feld[(position - groesse)] == 1){	//oben eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug oben(), Position %i", position );
        return -groesse;
    }
}

int linksoben(int position, int groesse, int feld[])
{
    if ((position - groesse)<0){                        //oberere Rand
        return -groesse;
    } else if (position % groesse == 0){                //linker Rand
        return -groesse;
    } else if (feld[(position - (groesse+1))] == 0){	//linksoben leer
        return -groesse;
    } else if (feld[(position - (groesse+1))] == 2){	//linksoben gegner
        return (1 +  linksoben(position - (groesse+1), groesse, feld));
    } else if (feld[(position - (groesse+1))] == 1){	//linksoben eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug linksoben(), Position %i", position );
        return -groesse;
    }
}

int rechtsoben(int position, int groesse, int feld[])
{
    if ((position - groesse)<0){                        //oberer Rand
        return -groesse;
    } else if (position % groesse == groesse -1) {      //rechter Rand
        return -groesse;
    } else if (feld[(position - (groesse-1))] == 0){	//rechtsoben leer
        return -groesse;
    } else if (feld[(position - (groesse-1))] == 2){	//rechtsoben gegner
        return (1 +  rechtsoben(position - (groesse-1), groesse, feld));
    } else if (feld[(position - (groesse-1))] == 1){	//rechtsoben eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug rechtsoben(), Position %i", position );
        return -groesse;
    }
}

int unten(int position, int groesse, int feld[])
{
    if ((position + groesse)>(groesse * groesse)){      //unterer Rand
        return -groesse;
    }else if (feld[(position + (groesse-0))] == 0){	    //unten leer
        return -groesse;
    } else if (feld[(position + (groesse-0))] == 2){	//unten gegner
        return (1 +  unten(position + (groesse-0), groesse, feld));
    } else if (feld[(position + (groesse-0))] == 1){	//unten eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug unten(), Position %i", position );
        return -groesse;
    }
}

int linksunten(int position, int groesse, int feld[])
{
    if ((position + groesse)>(groesse * groesse)){      //unterer Rand
        return -groesse;
    } else if (position % groesse == 0){               //linker Rand
        return -groesse;
    } else if (feld[(position + (groesse-1))] == 0){	//linksunten leer
        return -groesse;
    } else if (feld[(position + (groesse-1))] == 2){	//linksunten gegner
        return (1 +  linksunten(position + (groesse-1), groesse, feld));
    } else if (feld[(position + (groesse-1))] == 1){	//linksunten eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug linksunten(), Position %i", position );
        return -groesse;
    }
}

int rechtsunten(int position, int groesse, int feld[])
{
    if ((position + groesse)>(groesse * groesse)){      //unterer Rand
        return -groesse;
    } else if (position % groesse == groesse -1) {      //rechter Rand
        return -groesse;
    }  else if (feld[(position + (groesse+1))] == 0){	//rechtsunten leer
        return -groesse;
    } else if (feld[(position + (groesse+1))] == 2){	//rechtsunten gegner
        return (1 +  rechtsunten(position + (groesse+1), groesse, feld));
    } else if (feld[(position + (groesse+1))] == 1){	//rechtsunten eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug rechtsunten(), Position %i", position );
        return -groesse;
    }
}

int links(int position, int groesse, int feld[])
{
    if (position % groesse == 0){       //linker Rand
        return -groesse;
    } else if (feld[position -1] == 0){	//links leer
        return -groesse;
    } else if (feld[position -1] == 2){	//links gegner
        return (1 +  links(position -1, groesse, feld));
    } else if (feld[position -1] == 1){	//links eigen
        return 0;
    } else {
        printf("Fehler bei Berechnung des Spielzug links(), Position %i", position );
        return -groesse;
    }
}

int rechts(int position, int groesse, int feld[])
{
    if (position % groesse == (groesse -1)){    //rechter Rand
        return -groesse;
    } else if (feld[position +1] == 0){	//rechts leer
        return -groesse;
    } else if (feld[position +1] == 2){	//rechts gegner
        return (1 +  rechts(position + 1, groesse, feld));
    } else if (feld[position +1] == 1){	//rechts eigen
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

int sligtlyEnhancedRandomAI (int zuege[], int anzahl, int groesse){
    int i;
    for (int i = 0; i<anzahl; i++)      //überprüfe ob folgende Positionen in der Liste sind
    {
        if (zuege[i]==0){       //links oben
            return 0;}
        if (zuege[i]== (groesse-1)){    //rechts oben
            return (zuege[i]);}
        if (zuege[i]== (groesse*groesse-1)){    //rechts unten
            return (zuege[i]);}
        if (zuege[i]== (groesse*(groesse-1))){  //links unten
            return (zuege[i]);}
    }
    time_t t;       //Ansonsten wähle nen zufälligen Zug
    srand((unsigned) time(&t));
    int r = rand() % anzahl;
    return zuege[r];
}

char* convertMove(char * spielzug, int position, int groesse){
    //ASCII umrechnung der Position
    // char * spielzug enthält den fertigen String
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

int gueltigerZug(int *feld, int groesse){
    int i, n, gain, sizeZuege, zug, currentMaxGain, maxGainZug;

    int * zuege = malloc(groesse*groesse * sizeof(int));
    if (NULL == zuege) {
        fprintf(stderr, "malloc failed\n");
        return(-1);
    }
    currentMaxGain = 0;
    maxGainZug = -1;
    sizeZuege = 0;
    for (i=0; i<(groesse*groesse);i++){
        if (feld[i]==0)
        {
            gain = 0;                   // gain dient dazu, wieviele Steine mit dieser Position übernommen werden können
            if ((i - groesse)>0){       //oberer Rand
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
            if ((i + groesse)<(groesse * groesse)){     //unterer Rand
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
            if (i % groesse != 0){                      //linker Rand
                if ((n=links(i, groesse,feld))>0){
                    gain = gain + n;
                }
            }
            if (i % groesse != (groesse -1)){           // rechter Rand
                if ((n=rechts(i, groesse,feld))>0){
                    gain = gain + n;
                }
            }
            if (gain > 0)                               // wenn Steine umgedreht werden können, speichere die Position im Array
            {
                zuege[sizeZuege] = i;
                sizeZuege++;
            }
            if (gain > currentMaxGain)
            {
                maxGainZug = i;
            }
        }
    }

    if (sizeZuege == 0)
    {
        printf("kein gültiger Zug berechenbar");
        return -1;  }
    zug = randomAI(zuege,sizeZuege);
    // zug = sligtlyEnhancedRandomAI (zuege,sizeZuege,groesse);
    // Max Gain KI:
    // zug = maxGainZug;
    free(zuege);
    return zug;
}
