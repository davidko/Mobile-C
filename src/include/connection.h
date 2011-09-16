/* SVN FILE INFO
 * $Revision: 316 $ : Last Committed Revision
 * $Date: 2009-04-29 16:40:54 -0700 (Wed, 29 Apr 2009) $ : Last Committed Date */
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

#ifndef _CONNECTION_H_
#define _CONNECTION_H_
#include <sys/types.h>
#ifndef _WIN32
#include <netinet/in.h>
#else
#include <windows.h>
#endif

struct mc_platform_s;

typedef struct connection_s {

  /* the primary id of the connection */
  int connect_id;
  /* session identifier */
  int nonce;

  /* socket - Network Variables */
  char *remote_hostname;

  struct sockaddr_in addr;

  /* file descriptors */ 
  u_long clientfd;
  u_long serverfd;

  /* connection key  */
  unsigned char *AES_key;
  
}connection_t;
typedef connection_t* connection_p;

/* glocal functions - basic */
connection_p 
connection_New(void);

int 
connection_Destroy(connection_p connection);

connection_p 
connection_Copy(connection_p connection);
int connection_Print(connection_p connection);

void connection_Close(connection_p connection);
void connection_SetHosts(connection_p connection, char* remote_host, char *local_host);

/* global functions special */
int connection_CreateMessage(connection_p connection);
int connection_SendtoConnection(connection_p connection, char * send_buffer);
int connection_RecvFromConnection(connection_p connection,
    struct mc_platform_s *global);

int closeSocket(int sockfd);

#endif
