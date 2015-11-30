//
// Created by kad on 26.11.15.
//

#include "fileIOHandler.h"
#include "errorHandler.h"
#include "global.h"
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>

//global variable
config_t config;

int readCfg(const char *filename) {
    /*
     * open configuration file
     * found in ~/config/client.conf
     */
    FILE *cfgFile = fopen(filename, "r");
    if (!cfgFile) {
       return fileNotFoundError();
    }



    /*
     * create regular expression
     */
    regex_t regular_expression;

    int result;
    if ((result = regcomp(&regular_expression, CONFIG_ENTRY, REG_EXTENDED)) != 0) {
        char errorMessage[256];
        regerror(result, &regular_expression, errorMessage, sizeof(errorMessage));
        fprintf(stderr, "regexp: %s\n", errorMessage);
        return -1;
    }

    /*
     * Setting up the match patterns from the compiled regex
     * match[0] is a full line match
     * match[1] is a key match
     * match[2] is a key value match
     *
     * e.g.: key01="Hello World!"
     * expected value for key01 would hav been "Bye!"
     * so this will look like
     * match[0]=-1
     * match[1]=0
     * match[2]=0
     */
    regmatch_t compKeyComposition[3];

    //The actual chars:
    char line[BUF_SIZE],key[BUF_SIZE],value[BUF_SIZE];

    /*
     * Now for the actual comparison, in theory we could store whatever we wanted in the config file.
     * Shouldn't even matter in what order the keys arrive.
     * Oh, ad actually read it, provided the required regex key is compiled, going to mark this though.
     * loops until file end
     */
    while (fgets(line, sizeof(line), cfgFile)) {
        /*
         * regexec should return 0 for a matching full line, everything else either translates to non valid values,
         * which should be errored appropriately or means the line is bs.
         */
        if (regexec(&regular_expression, line, 3, compKeyComposition, 0) == 0) {
            //reading the key from the line:
            int keyLength = compKeyComposition[1].rm_eo - compKeyComposition[1].rm_so;
            strncpy(key, &line[compKeyComposition[1].rm_so], keyLength);
            //It matches? Great, let's zero it then!
            key[keyLength] = 0;
            //reading the value from the line:
            int valueLength = compKeyComposition[2].rm_eo - compKeyComposition[2].rm_so;
            //So why exactly can I strncpy this???
            strncpy(value, &line[compKeyComposition[2].rm_so], valueLength);
            //It matches? Great, let's zero it then!
            value[valueLength] = 0;

            /*
             * Now comes the more important part.
             * We've just read our values, now let's do something with'em.
             * Let's do a simple pattern match - well not so simple, as this isn't Haskell, but still.
             */
            if (strcmp("hostname", key) == 0) {
                //Do we have a key hostname?
                strncpy(config.hostname, value, sizeof(config.hostname));
            } else if (strcmp("port", key) == 0){
                //No, we have a key hostname then?
                //config.port = (unsigned short) atoi(value);
                config.port = (unsigned short) atoi(value);
            } else if (strcmp("gamename", key) == 0){
                //strncpy(config.gamename, value, sizeof(config.gamename));
                strncpy(config.gamename, value, sizeof(config.gamename));
            } else {
                unknownKeyError(key);
            }
        }
    }
    regfree(&regular_expression);
    return 0;
}
