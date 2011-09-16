/* SVN FILE INFO
 * $Revision: 402 $ : Last Committed Revision
 * $Date: 2009-08-23 13:44:39 -0700 (Sun, 23 Aug 2009) $ : Last Committed Date */
/*[
 * Copyright (c) 2007 Integration Engineering Laboratory
                      University of California, Davis
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the complete modified source code.  Modifications are to
 * be distributed as patches to the released version.  Permission to
 * distribute binaries produced by compiling modified sources is granted,
 * provided you
 *   1. distribute the corresponding source modifications from the
 *    released version in the form of a patch file along with the binaries,
 *   2. add special version identification to distinguish your version
 *    in addition to the base release version number,
 *   3. provide your name and address as the primary contact for the
 *    support of your modified version, and
 *   4. retain our contact information in regard to use of the base
 *    software.
 * Permission to distribute the released version of the source code along
 * with corresponding source modifications in the form of a patch file is
 * granted with same provisions 2 through 4 for binary distributions.
 *
 * This software is provided "as is" without express or implied warranty
 * to the extent permitted by applicable law.
]*/
#ifndef _WIN32
#include "config.h"
#else
#include "winconfig.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/connection.h"
#include "include/mc_error.h"
#include "include/macros.h"

int
connection_Destroy(connection_p connection)
{
  if(connection == NULL) {
    return MC_SUCCESS;
  }
  /* Close the connection, just to make sure. */
#ifndef _WIN32
  /*if(close(connection->clientfd) <0) 
		SOCKET_ERROR(); */
#else
  closeSocket(connection->clientfd);
#endif

  if(connection->remote_hostname != NULL) {
    free(connection->remote_hostname);
  }

  free(connection);
  return MC_SUCCESS;
}

connection_p connection_New(void)
{
  connection_p connection;
  connection = (connection_p)malloc(sizeof(connection_t));
  if (connection==NULL) {
    fprintf(stderr, "Memory error. %s:%d\n", __FILE__, __LINE__);
  }
  memset(connection, 0, sizeof(connection_t));
  return connection;
}

connection_p connection_Copy(connection_p connection)
{
  connection_p tmp = connection_New();
  tmp->connect_id = connection->connect_id;
  tmp->remote_hostname = strdup(connection->remote_hostname);
  tmp->addr = connection->addr;
  tmp->clientfd = connection->clientfd;
  tmp->serverfd = connection->serverfd;

  return tmp;
}

int connection_Print(connection_p connection)
{
  printf("Connection %d: %s\n", connection->connect_id, connection->remote_hostname);
  return 0;
}

int closeSocket(int sockfd)
{
#ifndef _WIN32
  /* Do this in a multi-part way; first, send FIN by disallowing writes. Then,
   * read all the data. Then, close the socket for real. */
  char *buf[80];
  shutdown(sockfd, 1); /* Disallow sends */

  /* wait for remote side to close connection */
  while(recv(sockfd, buf, 80, 0) > 0);
  close(sockfd);
  return 0;
#else
  char *buf[80];

  /* wait for remote side to close connection */
  while(recvFrom(sockfd, buf, (size_t)80, 0, (struct sockaddr*) 0, 0) > 0);
  closesocket(sockfd);
  return 0;
#endif
}
