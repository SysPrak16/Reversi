#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <regex.h>

//#define KEY_VALUE_PATTERN "^[[:space:]]*(.)+[[:space:]]*=[[:space:]]*(.)+\n$"
#define KEY_VALUE_PATTERN "^[[:space:]]*([[:alnum:]]*){1}[[:space:]]*+=[[:space:]]*(.*){1}\n$"

int readconfig(const char *filename)
{
    // open configuration file

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Fehler: fopen");
        return -1;
    }

    // compile regular expression

    regex_t regex;

    int result;
    if ((result = regcomp(&regex, KEY_VALUE_PATTERN, REG_EXTENDED)) != 0) {
        char errorMessage[256];
        regerror(result, &regex, errorMessage, sizeof(errorMessage));
        fprintf(stderr, "regexp: %s\n", errorMessage);
        return -1;
    }

    // matches[0] is the whole match, matches[1] and matches[2] sub matches (key and value)

    regmatch_t matches[3];

    char line[256];
    char key[256];
    char value[256];

    // read each line from configuration file

    while (fgets(line, sizeof(line), file)) {

        // if regexec returns 0 the line matches the regular expression

        if (regexec(&regex, line, 3, matches, 0) == 0) {

            // extract key

            int keyLength = matches[1].rm_eo - matches[1].rm_so;
            strncpy(key, &line[matches[1].rm_so], keyLength);
            key[keyLength] = 0;

            // extract value

            int valueLength = matches[2].rm_eo - matches[2].rm_so;
            strncpy(value, &line[matches[2].rm_so], valueLength);
            value[valueLength] = 0;

            // parse keys

            if (strcmp("hostname", key) == 0) {
                strncpy(config.hostname, value, sizeof(config.hostname));

            } else if (strcmp("port", key) == 0) {
                config.port = (unsigned short) atoi(value);

            } else if (strcmp("gamekind", key) == 0) {
                strncpy(config.gamekind, value, sizeof(config.gamekind));

            } else {
                fprintf(stderr, "Unbekannter Parameter '%s': Ignoriere\n", key);
            }
        }
    }

    // free regex

    regfree(&regex);

    return 0;
}

void initconfig()
{
    memset(&config, 0, sizeof(config));
    config.aiMode = AI_DUMB;
}
