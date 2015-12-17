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

    shmid = shmget((key_t)mykey, /*sizeof(SHM)*/2048,0666| IPC_CREAT);

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

    myuSHY->flag=-1;     //For 1st step Child


    pid_t pid = 0;



    pid=fork();
    if(pid<0){
        perror("Fehler bei fork().");
        exit(EXIT_FAILURE);
    }
    if(pid>0){
        printf("Elternprozess\n");
        while (running){
            if(myuSHY->flag>0){
                switch(myuSHY->flag){
                    case 1:{
                        printf("1.Eltern\n");
                        //mySHM->field=malloc(mySHM->width*mySHM->height* sizeof(int));
                        printf("\tGetting Gamename...\n Gamename: %s\n",myuSHY->gamename);
                        myuSHY->flag=-2;
                        break;
                    }
                    case 2:{
                        printf("2.Eltern\n");
                        printf("\tGetting Spielernummer...\n\tSpielernummer: %d\n",myuSHY->playernum);
                        myuSHY->flag=-3;
                        break;
                    }
                    case 3:{
                        printf("3.Eltern\n\tGetting Spieleranzahl...\n\tSpieleranzahl: %d\n",myuSHY->players);
                        myuSHY->flag=-4;
                        break;
                    }
                    case 42:{
                        running=0;
                        break;
                    }

                }
            }
        }
    }
    else{
        printf("Kindprozess\n");

        ret = connectToServer();

        if(ret==-1){
            //TODO return EXIT_FAILURE abfangen & uSHY->flag auf 42 setzen zum beenden d. Programms
        }

        /*int runningc=1;
        void *shared_memory_c = (void*)0;
        SHM *mySHMc;
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

        mySHMc = (SHM*) shared_memory_c;
        while (runningc){
            while(mySHMc->flag>0){
                sleep(1);
                //printf("Waiting for parent...\n");
                printf("\n");
            }
            switch (mySHMc->flag){
                case -1:{
                    printf("1.Kind\n\tWriting Height...\n");
                    //mySHMc->height=8;
                    mySHMc->flag=1;     //For 1st Step Parent
                    break;
                }
                case -2:{
                    printf("2.Kind\n\tWriting Width...\n");
                    //mySHMc->width=8;
                    mySHMc->flag=2;     //For 2nd Step Parent
                    //runningc=0;
                    break;
                }
                case -3:{
                    printf("3.Kind\n\tWriting Field...\n");
                    for (int h = 0; h < mySHMc->height; h++) {
                        for (int w = 0; w < mySHMc->width; w++) {
                            mySHMc->fielda[h*mySHMc->width+w]=1;
                        }
                    }
                    mySHMc->flag=3;
                    runningc=0;
                    break;
                }
            }
        }*/
    }
    return ret;
}



    /*pid_t pid = 0;               // Prozess-ID des Kindes
    //int ret_code = 0;            // Hilfsvariable zum Speichern von zurueckgegebenen Werten
    //int fd[2];                   // Dateideskriptor zur Kommunikation mit dem Kindprozess

    //int *shm_ptr;
    key_t shmKey;

    shmKey = ftok(".", 'x');

    int shm_id = shmget(shmKey, sizeof(myMTS), IPC_CREAT | 0666);
    if (shm_id < 0){
        puts("Fehler bei shmget()");
        return -1;
    }

    shm_ptr = (int *) shmat(shm_id, NULL, 0);

    if ((int)shm_ptr == -1){
        puts("Fehler bei shmat()");
        return -1;
    }


     * Zwei Prozesse, die den Test durchfuehren
     * => fork() um Kindprozess zu erstellen

    pid = fork();
    if (pid < 0) {
        perror ("Fehler bei fork().");
        exit(EXIT_FAILURE);
    }


     * Elternprozess

    if (pid > 0) {
        printf("Elternprozess\n");
        //close(fd[1]);         //Pipe
        shm1();
        // Warten auf den Kindprozess

        printf("Auf Kindprozess warten...\n");
        pid = waitpid(pid, NULL, 0);
        if (pid < 0) {
            perror ("Fehler beim Warten auf Kindprozess.");
            exit(EXIT_FAILURE);
        }


    }

          Kindprozess

    else {
        printf("Kindprozess\n");
        printf("Connecting to server...\n");
        connectToServer();
                            check = "K->E: Pipe\n";
                            ret_code = write(fd[1], check, n);
                            if (ret_code != n) {
                                perror("Fehler bei write().");
                                exit(EXIT_FAILURE);
                            }
        _exit(EXIT_SUCCESS);
    }*/

