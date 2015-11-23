#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

#include "constants.h"

typedef enum {
    AI_DUMB,
    AI_AWESOME,
} AiMode;

typedef struct {
    char hostname[256];
    unsigned short port;
    char gamekind[32];
    char gameid[GAMEID_LENGTH + 1];
    AiMode aiMode;
} config_t;

config_t config; /*!< global variable holding configuration parameters */

void initconfig();

/*!
 * \brief Parses a configuration file and stores the results in @see <config>
 * \param filename  name of the configuration file
 * \return
 */
int readconfig(const char *filename);


#endif // CONFIG_H
