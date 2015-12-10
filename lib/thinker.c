//
// Created by kad on 26.11.15.
//

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "thinker.h"
#include "connector.h"

int think(){
    /*pid_t pid = fork();

    if(pid==-1){
        // If fork() returns -1, an error happened.
        perror("Fehler: fork()");
        exit(EXIT_FAILURE);
    }else if(pid== 0){
        // If fork() returns 0, we are in the child process.
        // call connector
        printf("Connecting to server...");
        connectToServer(DEF_PORTNUMBER, DEF_HOSTNAME); //TODO: PORTNUMMER und DEF_HOSTNAME momentan überflüssig, da in prologue.c definiert
        _exit(EXIT_SUCCESS);
    } else{
        printf("not Connecting to server...");
    }*/

    pid_t pid = 0;               // Prozess-ID des Kindes
    int ret_code = 0;            // Hilfsvariable zum Speichern von zurueckgegebenen Werten
    int fd[2];                   // Dateideskriptor zur Kommunikation mit dem Kindprozess
    MTS fMTS;
    char *check;
    check=malloc(256* sizeof(char));
    int n = (int) 256* sizeof(char);

    ret_code = pipe(fd);
    if (ret_code < 0) {
        perror ("Fehler beim Einrichten der Pipe.");
        exit(EXIT_FAILURE);
    }

    /*
     * Zwei Prozesse, die den Test durchfuehren
     * => fork() um Kindprozess zu erstellen
     */
    pid = ret_code = fork();
    if (ret_code < 0) {
        perror ("Fehler bei fork().");
        exit(EXIT_FAILURE);
    }

    /*
     * Elternprozess
     */
    if (pid > 0) {
        printf("Elternprozess\n");
        close(fd[1]);
        // Warten auf den Kindprozess
        printf("Auf Kindprozess warten...\n");
        ret_code = waitpid(pid, NULL, 0);
        if (ret_code < 0) {
            perror ("Fehler beim Warten auf Kindprozess.");
            exit(EXIT_FAILURE);
        }

        // Ergebnis des Kindprozesses abholen und auswerten
        // - Aus Pipe lesen
        // - Zahl wieder aus den 4 Bytes zusammenbauen und pruefen, ob ungleich 0
        read (fd[0], check, n);
        printf("%s",check);
    }
        /*
         * Kindprozess
         */
    else {
        printf("Kindprozess\n");
        printf("Connecting to server...\n");
        connectToServer(DEF_PORTNUMBER,
                        DEF_HOSTNAME); //TODO: PORTNUMMER und DEF_HOSTNAME momentan überflüssig, da in prologue.c definiert

        check = "K->E: Pipe\n";
        ret_code = write(fd[1], check, n);
        if (ret_code != n) {
            perror("Fehler bei write().");
            exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
    }
    return 0;
}