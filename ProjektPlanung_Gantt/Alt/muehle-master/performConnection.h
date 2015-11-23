#ifndef PERFORMCONNECTION_H
#define PERFORMCONNECTION_H

#include "connector.h"

/*!
 * \brief Performs the prolog phase
 * \param sfd   open socket for communication
 * \return -1 on failure otherwise 0
 */
int performConnection(int sfd);

#endif // PERFORMCONNECTION_H
