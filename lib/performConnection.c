#include "performConnection.h"
#include "global.h"
#include "connector.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>

typedef struct {
    int *field;
    int height;
    int width;
    int flag;
    int fielda[8*8];
}SHM;

MTS myMTS;  //global Var for sharing in SHM later on
extern config_t config;

int strbeg(const char *str1, const char *str2)
{
    return strncmp(str1, str2, strlen(str2)) == 0;
}


int performConnection(int socket_fd)
{

    /*int *shm_ptr;
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


    // A common header file to describe the shared memory we wish to pass about.


#define TEXT_SZ 2048

    struct shared_use_st {
        int written_by_you;
        char some_text[TEXT_SZ];
    };

Our first program is a consumer. After the headers the shared memory segment
 (the size of our shared memory structure) is created with a call to shmget,
 with the IPC_CREAT bit specified.



The second program is the producer and allows us to enter data for consumers.
 It's very similar to shm1.c and looks like this.


        int running = 1;
        void *shared_memory = (void *)0;
        MTS *myMTS2;
        char buffer2[TEXT_SZ ];
        int shmid;
        int mykey = getuid();


        shmid = shmget((key_t)mykey, sizeof(MTS), 0666 | IPC_CREAT);

        if (shmid == -1) {
            fprintf(stderr, "shmget failed\n");
            exit(EXIT_FAILURE);
        }

        shared_memory = shmat(shmid, (void *)0, 0);
        if (shared_memory == (void *)-1) {
            fprintf(stderr, "shmat failed\n");
            exit(EXIT_FAILURE);
        }

        printf("Memory attached at %X\n", (int)shared_memory);
*/







    //int runningc=1;
    //void *shared_memory_c = (void*)0;
    uSHY *myuSHYc;
    int shmidc;
    int mykeyc=getuid();

    shmidc = shmget((key_t)mykeyc, sizeof(SHM),0666 | IPC_CREAT);

    if(shmidc ==-1){
        perror("Fehler bei shmget\n");
        exit(EXIT_FAILURE);
    }

    myuSHYc = (uSHY*) shmat(shmidc,(void*)0,0);
    if(myuSHYc==(uSHY*)-1){
        perror("Fehler bei shmat.");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n",(int) myuSHYc);
    //myuSHYc = (uSHY*) shared_memory_c;




    //SHM Players
    int p_shm_id;           // Shared Memory ID
    key_t p_shm_key;        // Shared Memory Key
    player_info *iPlay;     // Shared Memory Data

    //SHM for Field
    int f_shm_id;           // Shared Memory ID
    key_t f_shm_key;        // Shared Memory Key
    int *field;             // Shared Memory Data
    int ptrs;               // For Saving the Pointer





    memset(&myMTS, 0, sizeof(myMTS));

    int opi=0;      //@var Opponent Info Flag
    int ggN=0;      //@var Get Game Name Flag
    int getField=0; //@var Helps to check if we are between +FIELD and +ENDFIELD to know when to read out the mom Field

    char buffer[BUF_SIZE];
    printf("Your Game ID: %s\n",config.gameID);
    int n,i,j;
    //TODO: Actual question and answer game!
    //Placeholder ID: WAIT
    //TODO: resolve test variables!
    //TEST--Start
    char play[]="PLAY F3\n";
    //TEST--End
    // ID: AoNuBicHhoc

    // Creating gameID-token for protocol
    char gid[16];
    strcpy(gid, "ID ");
    strcat(gid, config.gameID);
    strcat(gid, "\n");
    //Split by \n
    const char splitToken[2] = "\n";

    i=0;
    while((n = receiveMessage(socket_fd, buffer, sizeof(buffer))) > 0)
    {

        char *lineBuf;
        
        //get the first token
        lineBuf = strtok(buffer, splitToken);

        // walk through other tokens
        while( lineBuf != NULL ) {
            fprintf(stdout, "S: %s\n", lineBuf);
            //strncpy(lineBuf, lineBuf, BUF_SIZE);

            if (strbeg(lineBuf, CON_TIMEOUT)) {
                printf(CON_TIMEOUT_ERR_MSG);
                return -1;
            } else if (strbeg(lineBuf, VERSION_ERROR)) {
                printf(VERSION_ERROR_MSG);
                return -1;
            } else if (strbeg(lineBuf, NO_FREE_PLAYER)) {
                printf(NO_FREE_PLAYER_MSG);
                return -1;
            } else if (strbeg(lineBuf, NO_GAME_ERROR)) {
                printf(NO_GAME_ERROR_MSG);
                return -1;
            } else if (strbeg(lineBuf, INVALID_MOVE)) {
                printf(INVALID_MOVE_MSG);
                /*printf("Closing SHM Client...");
                if (shmdt(shared_memory_c) == -1) {
                    fprintf(stderr, "shmdt_1 failed at child\n");
                    exit(EXIT_FAILURE);
                }
                if (shmdt(iPlay) == -1) {
                    fprintf(stderr, "shmdt_p failed at child\n");
                    exit(EXIT_FAILURE);
                }
                if (shmdt(field) == -1) {
                    fprintf(stderr, "shmdt_f failed at child\n");
                    exit(EXIT_FAILURE);
                }
                if (shmctl(shmidc, IPC_RMID, 0) == -1) {
                    fprintf(stderr, "shmctl(IPC_RMID)_1 failed at child\n");
                    exit(EXIT_FAILURE);
                }
                if (shmctl(p_shm_id, IPC_RMID, 0) == -1) {
                    fprintf(stderr, "shmctl(IPC_RMID)_p failed at child\n");
                    exit(EXIT_FAILURE);
                }
                if (shmctl(f_shm_id, IPC_RMID, 0) == -1) {
                    fprintf(stderr, "shmctl(IPC_RMID)_f failed at child\n");
                    exit(EXIT_FAILURE);
                }*/
                return -1;
            } else if (strbeg(lineBuf, MNM_SERVER)) {
                if (send(socket_fd, CVERSION, strlen(CVERSION), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, CVERSION);
                }
                //printf("Client sending Client Version\n");
            } else if (strbeg(lineBuf, VERSION_ACCEPTED)) {
                if (send(socket_fd, gid, strlen(gid), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, gid);
                }
                //printf("Client sending Game-ID\n");
            } else if (strbeg(lineBuf, "+ PLAYING")) {
                ggN=1;
                // Auslesen von GameKind-Name
                if (!strbeg(lineBuf, "+ PLAYING Reversi")) {
                    printf(GAMEKIND_ERROR_MSG);
                    return -1;
                } else {
                    if (strbeg(lineBuf, "+ PLAYING Reversi")) {
                        // Senden von Spielernummer
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            puts("Send failed");
                            return -1;
                        } else {
                            printf("C: %.*s", n, PLAYER1);
                        }
                    } else {
                        n = receiveMessage(socket_fd, lineBuf, sizeof(lineBuf));
                        if (send(socket_fd, PLAYER1, strlen(PLAYER1), 0) < 0) {
                            puts("Send failed");
                            return -1;
                        } else {
                            printf("C: %.*s", n, PLAYER1);
                        }
                    }
                }
            } else if (ggN&&strbeg(lineBuf, "+ ")) {    //Getting Gamename
                if(myuSHYc->flag==-1){
                    printf("1.Kind\n\tWriting Gamename...\n");
                    sscanf(lineBuf,"+ %s",myuSHYc->gamename);
                    myuSHYc->flag=1;     //For 1st Step Parent
                }
                printf("\tWaiting for parent to read...\n");
                while(myuSHYc->flag>0){
                    sleep(1/100);
                }
                ggN=0;
            } else if (strbeg(lineBuf, "+ YOU")) {
                if(myuSHYc->flag==-2){

                    printf("2.Kind\n\tWriting Spielernummer...\n");
                    sscanf(lineBuf,"+ YOU %i",&myuSHYc->playernum);

                    printf("\tWriting Spielername...\n");
                    //TODO Später in SHM schreiben

                    myuSHYc->flag=2;     //For 2nd Step Parent
                }
                printf("\tWaiting for parent to read...\n");
                while(myuSHYc->flag>0){
                    sleep(1/100);
                }


            } else if (strbeg(lineBuf, "+ TOTAL")) {
                if(myuSHYc->flag==-3){
                    printf("3.Kind\n\tWriting Spielerzahl...\n");
                    sscanf(lineBuf,"+ TOTAL %d",&myuSHYc->players);
                    myuSHYc->flag=3;
                }
                printf("\tWaiting for parent to read...\n");
                while(myuSHYc->flag>0){
                    sleep(1/100);
                }
                opi=1;
            }else if(opi&&strbeg(lineBuf, "+ ")){
                if(myuSHYc->flag==-4){
                    p_shm_key = ftok(".", 'P');
                    p_shm_id = shmget (p_shm_key, (myuSHYc->players-1)*sizeof(iPlay), IPC_CREAT | 0666);
                    if (p_shm_id < 0){
                        printf("shmget error\n");
                        exit(1);
                    }

                    iPlay = (player_info*) shmat(p_shm_id,NULL,0);

                    if ((int) iPlay == -1) {
                        printf("*** shmat error (server) ***\n");
                        exit(1);
                    }

                    pid_t pid = 0;
                    printf("\tMemory attached at %X\n",(int) iPlay);

                    printf("\tWriting Player Info...\n");
                    sscanf(lineBuf, "+ %d %s %d",&iPlay[0].playernum,iPlay[0].playername,&iPlay[0].flag);
                    //TODO Für mehr als 2 Spieler möglich machen
                    myuSHYc->flag=4;
                }
                printf("\tWaiting for parent to read...\n");
                while(myuSHYc->flag>0){
                    sleep(1/100);
                }
                opi=0;
            } else if (strbeg(lineBuf, "+ ENDPLAYERS")) {

            } else if (strbeg(lineBuf, "+ WAIT")) {
                if (send(socket_fd, CWAIT, strlen(CWAIT), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, CWAIT);
                }
            }else if (strbeg(lineBuf, "+ MOVE")){
                getField=1;
            }else if (strbeg(lineBuf, "+ FIELD")){
                sscanf(lineBuf, "+ FIELD %d,%d", &myMTS.width, &myMTS.height);
                //printf("   Deine Höhe: %d\n   Deine Breite: %d\n",myMTS.height,myMTS.width);      //Just for Testing
                myMTS.field=malloc(myMTS.height*myMTS.width*sizeof(int));
                if(myuSHYc->flag==-5){
                    printf("4.Kind\n\tWriting Height...\n");
                    printf("\tWriting Width...\n");
                    sscanf(lineBuf, "+ FIELD %d,%d", &myuSHYc->width, &myuSHYc->height);

                    //SHM Field
                    printf("\tInitialising Field SHM...\n");
                    f_shm_key = ftok(".",'F');
                    f_shm_id = shmget(f_shm_key,myuSHYc->width*myuSHYc->height*sizeof(int), IPC_CREAT | 0666);
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
                    ptrs = (int)field;
                    printf("ptrs: %d\n",ptrs);

                    myuSHYc->flag=5;
                    printf("\tWaiting for parent to read...\n");
                    while(myuSHYc->flag>0){
                        sleep(1/100);
                    }
                }
            }
            else if (strbeg(lineBuf, "+ ") && getField) {
                int line;
                //int *fp =field[0];
                //printf("FP: %d\n",fp);
                sscanf(lineBuf, "+ %d", &line);
                //printf("            %d   ",line);                                                 //Just for Testing
                //TODO Testing Player 0 -> W Player 1 -> B ?
                for (j = 0; j < myMTS.width; j++) {
                    if(lineBuf[4 + j * 2] == 'W') {
                        if(myuSHYc->playernum==0) {
                            myMTS.field[line - 1 * myMTS.width + j] = 1;
                            //fp[line - 1 * myMTS.width + j] = 1;
                        }else {
                            myMTS.field[line - 1 * myMTS.width + j] = 2;
                            //fp[line-1*myMTS.width+j] = 2;
                        }
                    }else if(lineBuf[4 + j * 2] == 'B') {
                        if(myuSHYc->playernum==0) {
                            myMTS.field[line - 1 * myMTS.width + j] = 2;
                            //fp[line-1*myMTS.width+j] = 2;
                        }else{
                            myMTS.field[line-1*myMTS.width+j] = 1;
                            //fp[line-1*myMTS.width+j] = 1;
                        }
                    }else if(lineBuf[4 + j * 2] == '*') {
                        myMTS.field[line-1*myMTS.width+j] = 0;
                        //fp[line-1*myMTS.width+j] = 0;
                    }
                    //printf("%d ",myMTS.field[line-1*myMTS.width+j]);                                //Just for Testing
                    //printf("%d\n",line);
                    *field++=myMTS.field[line-1*myMTS.width+j];           //TODO Fix: Working just the 1st time
                    //printf("*Field: %d\n",(int)field);
                    //fp[line-1*myMTS.width+j]=myMTS.field[line-1*myMTS.width+j];
                    //printf("%d ",field[line-1*myMTS.width+j]);                                //Just for Testing

                    //fp[line-1*myMTS.width+j] = j;

                    //TEST
                }
                //printf("\n");                                                                     //Just for Testing
                if(line==1)
                    getField=0;
            } else if (strbeg(lineBuf, "+ ENDFIELD")) {
                if (send(socket_fd, CTHINK, strlen(CTHINK), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, CTHINK);
                }
                if(myuSHYc->flag==-6){
                    printf("6.Kind:\n\tWriting Field...\n");
                    //printf("*Field: %d\n",(int)field);                            //Just for Testing
                    field =(int*)ptrs;  //TODO
                    //printf("*Field: %d\n",(int)field);                            //Just for Testing
                    myuSHYc->flag=6;
                }
                printf("\tWaiting for parent to read...\n");
                while(myuSHYc->flag>0){
                    sleep(1/100);
                }
            } else if (strbeg(lineBuf, "+ OKTHINK")) {
                //TODO PLAY -- MOVE
                if (send(socket_fd, play, strlen(play), 0) < 0) {
                    puts("Send failed");
                    return -1;
                } else {
                    printf("C: %.*s", n, play);
                }
            }
            else {
                printf("Fehlerhafte Nachricht vom Server\n");
                //printf("%s\n", lineBuf);
                    myuSHYc->flag=42;       //Bricht SHM in Eleternprozess ab -> Beenden des Programmes
                return -1;
            }
            i++;
            lineBuf = strtok(NULL, splitToken);
        }
    }
    return 0;       //fallback
}