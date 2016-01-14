//
// Created by kad on 26.11.15.
//
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "thinker.h"
#include "connector.h"

extern MTS myMTS;



typedef struct {
    int *field;
    int height;
    int width;
    int flag;
    int fielda[8*8];
}SHM;

int think(){

    int ret;    //return of connectTOServer

    int running=1;

    //SHM
    //void *shared_memory=(void*)0;
    uSHY *myuSHY;
    int shmid;
    int mykey = getuid();

    srand((unsigned int)getpid());

    shmid = shmget((key_t)mykey, sizeof(SHM),0666| IPC_CREAT);

    if(shmid==-1){
        perror("Fehler bei shmget().");
        exit(EXIT_FAILURE);
    }

    myuSHY = (uSHY *) shmat(shmid,(void*)0,0);
    if(myuSHY==(uSHY*)-1){
        perror("Fehler bei shmat.");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n",(int)myuSHY);

    //myuSHY = (uSHY*)shared_memory;

    myuSHY->flag=-1;        //For 1st step Child

    pid_t pid = 0;


    //SHM for Players
    int p_shm_id;           // Shared Memory ID
    key_t p_shm_key;        // Shared Memory Key
    player_info *iPlay;     // Shared Memory Data

    //SHM for Field
    int f_shm_id;           // Shared Memory ID
    key_t f_shm_key;        // Shared Memory Key
    int *field;             // Shared Memory Data





    pid=fork();
    if(pid<0){
        perror("Fehler bei fork().");
        exit(EXIT_FAILURE);
    }
    if(pid>0){
        printf("Elternprozess\n");

        printf("Writing PID...\n");
        myuSHY->PIDP = getpid();
        printf("\tPID Parent: %d\n",myuSHY->PIDP);

        while (running){
            if(myuSHY->flag>0){
                switch(myuSHY->flag){
                    case 1:{
                        printf("1.Eltern\n");
                        //mySHM->field=malloc(mySHM->width*mySHM->height* sizeof(int));
                        printf("\tGetting Gamename...\n\tGamename: %s\n",myuSHY->gamename);
                        myuSHY->flag=-2;
                        break;
                    }
                    case 2:{
                        printf("2.Eltern\n");
                        printf("\tGetting Spielernummer...\n\tSpielernummer: %d\n",myuSHY->playernum);
                        //TODO SHM für Spieler
                        myuSHY->flag=-3;
                        break;
                    }
                    case 3:{
                        printf("3.Eltern\n\tGetting Spieleranzahl...\n\tSpieleranzahl: %d\n",myuSHY->players);

                        //SHM for Players
                        printf("Creating %d more SHM for Players...\n",myuSHY->players-1);
                        p_shm_key = ftok(".", 'P');
                        p_shm_id = shmget (p_shm_key, (myuSHY->players-1)*sizeof(iPlay), IPC_CREAT | 0666);
                        if (p_shm_id < 0){
                            printf("shmget error\n");
                            exit(1);
                        }

                        iPlay = (player_info*) shmat(p_shm_id,NULL,0);

                        if ((int) iPlay == -1) {
                            printf("*** shmat error (server) ***\n");
                            exit(1);
                        }

                        //pid_t pid = 0;
                        printf("Memory attached at %X\n",(int) iPlay);

                        myuSHY->flag=-4;
                        break;
                    }
                    case 4:{
                        printf("4.Eltern\n\tGetting Player Info...\n\tNumber: %d\n\tName: %s\n\tFlag: %d\n",iPlay->playernum,iPlay->playername,iPlay->flag);
                        myuSHY->flag=-5;
                        break;
                    }
                    case 5:{
                        printf("5.Eltern\n\tGetting Height...\n\tHeight: %d\n",myuSHY->height);
                        printf("\tGetting Width...\n\tWidth: %d\n",myuSHY->width);
                        //SHM Field
                        printf("\tInitialising Field SHM...\n");
                        f_shm_key = ftok(".",'F');
                        f_shm_id = shmget(f_shm_key,myuSHY->width*myuSHY->height*sizeof(int),0666);
                        if(f_shm_id < 0){
                            printf("shmget error\n");
                            exit(1);
                        }
                        field = (int*) shmat(f_shm_id,NULL,0);
                        if((int) field == -1){
                            printf("shmat error\n");
                            exit(1);
                        }
                        printf("\tMemory attached at %X\n",(int) field);

                        myuSHY->flag=-6;
                        break;
                    }
                    case 6:{
                        printf("6. Eltern:\n\tGetting Field...\n");

                        char row;
                        int *pf = field;
                        printf("\n\t");
                        int i,j= 0;
                        for (i; i < myuSHY->height+4; i++) {
                            row = 'A';
                            if(i==0||i==myuSHY->height+3)
                                printf("   ");
                            else if(i==1||i==myuSHY->height+2)
                                printf("  +");
                            else
                                (i-1>9)?(printf("%d| ",abs(i-myuSHY->height-2))):(printf(" %d| ",abs(i-myuSHY->height-2)));
                            for (j; j < myuSHY->height; j++) {
                                if(i==0||i==myuSHY->height+3)
                                    printf(" %c",row++);
                                else if(i==1||i==myuSHY->height+2)
                                    printf("--");
                                else{
                                    if(*pf == 1) {                                          // It's our's !
                                        if(myuSHY->playernum==0)                            // We are Player 0
                                            printf("W ");                                   // So it's a White one
                                        else                                                // We are Player 1
                                            printf("B ");                                   // So it's a Black one
                                    }else if(*pf == 2) {                                    // It's the other's !
                                        if(iPlay->playernum==0)                             // They are Player 0
                                            printf("W ");                                   // So it's a White one
                                        else                                                // They are Player 1
                                            printf("B ");                                   // So it's a Black one
                                    }else if(*pf == 0)                                      // Wait...
                                        printf("* ");                                       // There is nothing yet !
                                    *pf++;
                                }
                            }
                            if(i==0||i==myuSHY->height+3) {
                            }
                            else if(i==1||i==myuSHY->height+2){
                                printf("-+");
                            }
                            else
                                printf("|%d",abs(i-myuSHY->height-2));
                            printf("\n\t");
                        }
                        printf("\n");
                        myuSHY->flag=-6;
                        break;
                    }
                    /*case 42*/default:{
                        //running=0;
                        printf("Closing SHM Server...\n");
                        /*if (shmdt(shared_memory) == -1) {
                            fprintf(stderr, "shmdt failed\n");
                            exit(EXIT_FAILURE);
                        }

                        if (shmctl(shmid, IPC_RMID, 0) == -1) {
                            fprintf(stderr, "shmctl(IPC_RMID)_1 failed\n");
                            exit(EXIT_FAILURE);
                        }*/
                        //TODO shmdt...
                        if (shmdt(iPlay) == -1) {
                            fprintf(stderr, "shmdt failed\n");
                            exit(EXIT_FAILURE);
                        }

                        if (shmctl(f_shm_id, IPC_RMID, 0) == -1) {
                            fprintf(stderr, "shmctl(IPC_RMID) failed\n");
                            exit(EXIT_FAILURE);
                        }
                        running=0;
                        break;
                    }

                }
            }
        }
    }
    else{
        printf("Kindprozess\n");
        printf("Writing PID...\n");
        myuSHY->PIDP = getpid();
        printf("\tPID Child: %d\n",myuSHY->PIDP);

        //int runningc=1;
        void *shared_memory_c = (void*)0;
        uSHY *myuSHYc;
        int shmidc;
        int mykeyc=getuid();

        shmidc = shmget((key_t)mykeyc, sizeof(SHM),0666 | IPC_CREAT);

        if(shmidc ==-1){
            perror("Fehler bei shmget\n");
            exit(EXIT_FAILURE);
        }

        shared_memory_c = shmat(shmidc,(void*)0,0);
        if(shared_memory_c==(void*)-1){
            perror("Fehler bei shmat.");
            exit(EXIT_FAILURE);
        }

        printf("Memory attached at %X\n",(int) shared_memory_c);
        myuSHYc = (uSHY*) shared_memory_c;

        ret = connectToServer();
        myuSHYc->flag=42;
        if (shmdt(shared_memory_c) == -1) {
            fprintf(stderr, "shmdt failed\n");
            exit(EXIT_FAILURE);
        }

        if(ret==-1){
            //TODO return EXIT_FAILURE abfangen & uSHY->flag auf 42 setzen zum beenden d. Programms

        }
    }
    return ret;
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