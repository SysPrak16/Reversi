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
#include "global.h"

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
    void *shared_memory=(void*)0;
    uSHY *myuSHY;
    int shmid;
    int mykey = getuid();

    srand((unsigned int)getpid());

    shmid = shmget((key_t)mykey, sizeof(SHM)/*2048*/,0666| IPC_CREAT);

    if(shmid==-1){
        perror("Fehler bei shmget().");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid,(void*)0,0);
    if(shared_memory==(void*)-1){
        perror("Fehler bei shmat.");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n",(int)shared_memory);

    myuSHY = (uSHY*)shared_memory;

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
                        for (int i = 0; i < myuSHY->height+4; i++) {
                            row = 'A';
                            if(i==0||i==myuSHY->height+3)
                                printf("   ");
                            else if(i==1||i==myuSHY->height+2)
                                printf("  +");
                            else
                                (i-1>9)?(printf("%d| ",abs(i-myuSHY->height-2))):(printf(" %d| ",abs(i-myuSHY->height-2)));
                            for (int j = 0; j < myuSHY->height; j++) {
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
                    case 42:{
                        //running=0;
                        printf("Closing SHM Server...");
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