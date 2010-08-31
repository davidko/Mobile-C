/* SVN FILE INFO
 * $Revision: 423 $ : Last Committed Revision
 * $Date: 2009-09-11 16:14:33 -0700 (Fri, 11 Sep 2009) $ : Last Committed Date */
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

#ifndef _ACC_H_
#define _ACC_H_
#ifndef _WIN32
#include <sys/socket.h>
#else
#include <windows.h>
#include <winsock.h>
#endif
#include "macros.h"

struct mc_platform_s;

struct connection_thread_arg_s;
STRUCT(
    acc,
    struct mc_platform_s* mc_platform;
    THREAD_T thread;
    THREAD_T message_handler_thread;
    THREAD_T listen_thread;
		THREAD_T udplisten_thread;

    /* waiting flag: Note that this flag only applies to the listen_thread */
    int waiting;
    MUTEX_T* waiting_lock;
    COND_T* waiting_cond;

		/* Connection Queue Threads */
		int num_conn_threads;
		THREAD_T conn_thread;
		MUTEX_T conn_thread_lock;
		COND_T conn_thread_cond;
		struct connection_thread_arg_s* connection_thread_arg;

		/* Msg Queue Threads */
		int num_msg_threads;
		THREAD_T msg_thread;
		MUTEX_T msg_thread_lock;
		COND_T msg_thread_cond;
    )

STRUCT(
    listen_thread_arg,
    u_long port;
    struct sockaddr_in *addr;
    unsigned long int client_fd;
    unsigned long int server_fd;
    );

struct connection_s;
STRUCT(
		connection_thread_arg,
		struct mc_platform_s* mc_platform;
		struct connection_s* connection;
		)


acc_p 
acc_Initialize(struct mc_platform_s* mc_platform);

int
acc_Destroy(acc_p acc);

void
acc_Start(struct mc_platform_s*  mc_platform);

#ifndef _WIN32
void* 
acc_MessageHandlerThread(void* arg);

void* 
acc_Thread(void* arg);

void* 
acc_connection_Thread(void* arg);

void* 
listen_Thread(void* arg);

void* 
udplisten_Thread(void* arg);
#else
DWORD WINAPI 
acc_MessageHandlerThread( LPVOID arg );

DWORD WINAPI 
acc_Thread( LPVOID arg );

DWORD WINAPI 
acc_connection_Thread( LPVOID arg );

DWORD WINAPI 
listen_Thread( LPVOID arg );

DWORD WINAPI 
udplisten_Thread( LPVOID arg );
#endif /* _WIN32 */

#endif 
