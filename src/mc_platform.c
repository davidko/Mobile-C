/* SVN FILE INFO
 * $Revision: 560 $ : Last Committed Revision
 * $Date: 2010-08-30 15:09:20 -0700 (Mon, 30 Aug 2010) $ : Last Committed Date */
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

#ifndef _WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "include/acc.h"
#include "include/agent.h"
#include "include/mc_platform.h"
#include "include/macros.h"

#define DEFAULT_HOSTNAME_LENGTH 200
mc_platform_p
mc_platform_Initialize(MCAgency_t agency, ChOptions_t* ch_options)
{
  int i;
#ifdef _WIN32 /* WinSock variables */
  WORD wVersionRequested = MAKEWORD(1,1);
  int nret;
#endif
  struct hostent* localhost;
  char hostname[DEFAULT_HOSTNAME_LENGTH];

  mc_platform_p mc_platform;
	ChInterp_t* interp;
  
  /* Allocate Memory */
  mc_platform = (mc_platform_p)malloc(sizeof(mc_platform_t));
  CHECK_NULL(mc_platform, agency->last_error = MC_ERR_MEMORY; return NULL);

  /* Initialize members */
  mc_platform->err = 0;
  mc_platform->default_agentstatus = agency->default_agentstatus;

  for(i = 0; i < MC_THREAD_ALL; i++) {
    mc_platform->stack_size[i] = agency->stack_size[i];
  }

  mc_platform->bluetooth = agency->bluetooth;
  mc_platform->agency = agency;

  /* WINDOWS ONLY: Initialize WinSock */
#ifdef _WIN32
  nret = WSAStartup(wVersionRequested, &(mc_platform->wsaData));
  if (nret != 0) {
	printf("\nWSAStartup Error %d. %s:%d\n", nret, __FILE__,__LINE__);
	exit(0);
  }
  if (mc_platform->wsaData.wVersion != wVersionRequested) {
	printf("\nWrong Winsock Version %s:%d\n", __FILE__,__LINE__);
	exit(0);
  }
#endif /* _WIN32 */

  gethostname(hostname, DEFAULT_HOSTNAME_LENGTH);
  if (strlen(hostname) < 1) {
	  strcpy(hostname, "localhost");
  }
  localhost = gethostbyname(hostname); /* FIXME */
  if(localhost == NULL) {
#ifdef _WIN32
	  printf("Fatal Error: %d  %s:%d\n", WSAGetLastError(), __FILE__, __LINE__);
#else
	  fprintf(stderr, "Fatal Error %s:%d\n", __FILE__, __LINE__);
#endif
	  //exit(0);
  }
  mc_platform->hostname = (char*)malloc(sizeof(char)*DEFAULT_HOSTNAME_LENGTH);
  CHECK_NULL(mc_platform->hostname, agency->last_error = MC_ERR_MEMORY;return NULL);

  if(localhost == NULL) 
    strcpy(mc_platform->hostname, "127.0.0.1"); 
  else if(localhost->h_name)
    strcpy(mc_platform->hostname, localhost->h_name); 
  else
    // might always be 127.0.0.1 which is not good for ACL messaging on a network
    strcpy(mc_platform->hostname, inet_ntoa( *(struct in_addr*)localhost->h_addr) ); 
  
  mc_platform->hostname = (char*)realloc( mc_platform->hostname, sizeof(char) * (strlen(mc_platform->hostname)+1));
  CHECK_NULL(mc_platform->hostname, agency->last_error = MC_ERR_MEMORY;return NULL);
  mc_platform->port = agency->portno;
  mc_platform->interp_options = ch_options;

  mc_platform->message_queue     = ListInitialize();
  mc_platform->agent_queue       = ListInitialize();
  mc_platform->connection_queue  = ListInitialize();
  
  mc_platform->syncList          = ListInitialize();
  mc_platform->barrier_queue     = ListInitialize();

  mc_platform->interpreter_queue = ListInitialize();
  mc_platform->initInterps = agency->initInterps;

  //printf("Mobile-C:  Initializing %d interpreters\n", mc_platform->initInterps);
  /* Fill the interpreter queue with interpreters, initialized and ready to go. */
  for(i = 0; i < mc_platform->initInterps; i++) {
    interp = (ChInterp_t*)malloc(sizeof(ChInterp_t));
    if( mc_platform->interp_options == NULL ) {
      if(Ch_Initialize(interp, NULL)) {
        printf("CH INIT ERROR \n");
        exit(EXIT_FAILURE);
      }
    } 
    else {
      if(Ch_Initialize(interp, mc_platform->interp_options)) {
        printf("CH INIT ERROR \n");
        exit(EXIT_FAILURE);
      }
    }

    // Initialize all of the global variables
    agent_ChScriptInitVar(interp);

    ListWRLock(mc_platform->interpreter_queue);
    ListAdd(mc_platform->interpreter_queue, (void*)interp);
    ListWRUnlock(mc_platform->interpreter_queue);
  }

  /* Allocate sync variables */
  mc_platform->MC_signal_cond = (COND_T*)malloc(sizeof(COND_T));
  mc_platform->MC_sync_cond   = (COND_T*)malloc(sizeof(COND_T));
  mc_platform->MC_signal_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  mc_platform->MC_sync_lock   = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  mc_platform->MC_steer_lock  = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  mc_platform->MC_steer_cond  = (COND_T*)malloc(sizeof(COND_T));

  /* Check memory */
  CHECK_NULL( mc_platform->MC_signal_cond,
      agency->last_error = MC_ERR_MEMORY;return NULL );
  CHECK_NULL( mc_platform->MC_sync_cond  ,
      agency->last_error = MC_ERR_MEMORY;return NULL );
  CHECK_NULL( mc_platform->MC_signal_lock,
      agency->last_error = MC_ERR_MEMORY;return NULL );
  CHECK_NULL( mc_platform->MC_sync_lock  ,
      agency->last_error = MC_ERR_MEMORY;return NULL );
  CHECK_NULL( mc_platform->MC_steer_lock ,
      agency->last_error = MC_ERR_MEMORY;return NULL );
  CHECK_NULL( mc_platform->MC_steer_cond ,
      agency->last_error = MC_ERR_MEMORY;return NULL );

  /* Init sync variables */
  COND_INIT ( mc_platform->MC_signal_cond );
  COND_INIT ( mc_platform->MC_sync_cond );
  MUTEX_INIT( mc_platform->MC_signal_lock );
  MUTEX_INIT( mc_platform->MC_sync_lock );
  MUTEX_INIT( mc_platform->MC_steer_lock );
  COND_INIT ( mc_platform->MC_steer_cond );

  /* Initialize quit flag */
  mc_platform->quit = 0;
  mc_platform->quit_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(mc_platform->quit_lock);
  mc_platform->quit_cond = (COND_T*)malloc(sizeof(COND_T));
  COND_INIT(mc_platform->quit_cond);

  /* Initialize MC_signal flag */
  mc_platform->MC_signal = MC_NO_SIGNAL;

  /* Allocate and init giant lock */
  mc_platform->giant = 1;
  mc_platform->giant_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  CHECK_NULL(mc_platform->giant_lock,
      agency->last_error = MC_ERR_MEMORY;return NULL );
  mc_platform->giant_cond = (COND_T*)malloc(sizeof(COND_T));
  CHECK_NULL(mc_platform->giant_cond,
      agency->last_error = MC_ERR_MEMORY;return NULL );

  MUTEX_INIT( mc_platform->giant_lock );
  COND_INIT ( mc_platform->giant_cond );

  /* Initialize Agency Modules */
  mc_platform->df         = df_Initialize(mc_platform);
  mc_platform->ams        = ams_Initialize(mc_platform);
  mc_platform->acc        = acc_Initialize(mc_platform);
  mc_platform->cmd_prompt = cmd_prompt_Initialize(mc_platform);
  if (GET_THREAD_MODE(agency->threads, MC_THREAD_DF)) {
    df_Start(mc_platform);
    MUTEX_LOCK(mc_platform->df->waiting_lock);
    /* Wait for the thread to fully initialize before continuing */
    while(mc_platform->df->waiting == 0) {
      COND_WAIT(mc_platform->df->waiting_cond, mc_platform->df->waiting_lock);
    }
    MUTEX_UNLOCK(mc_platform->df->waiting_lock);
  }
  if (GET_THREAD_MODE(agency->threads, MC_THREAD_AMS)) {
    ams_Start(mc_platform);
    MUTEX_LOCK(mc_platform->ams->waiting_lock);
    /* Wait for the thread to fully initialize before continuing */
    while(mc_platform->ams->waiting == 0) {
      COND_WAIT(mc_platform->ams->waiting_cond, mc_platform->ams->waiting_lock);
    }
    MUTEX_UNLOCK(mc_platform->ams->waiting_lock);
  }
  if (GET_THREAD_MODE(agency->threads, MC_THREAD_ACC)) {
    acc_Start(mc_platform);
    MUTEX_LOCK(mc_platform->acc->waiting_lock);
    /* Wait for the thread to fully initialize before continuing */
    while(mc_platform->acc->waiting == 0) {
      COND_WAIT(mc_platform->acc->waiting_cond, mc_platform->acc->waiting_lock);
    }
    MUTEX_UNLOCK(mc_platform->acc->waiting_lock);
  }
  if (GET_THREAD_MODE(agency->threads, MC_THREAD_CP))
    cmd_prompt_Start(mc_platform);
  return mc_platform;
}

int
mc_platform_Destroy(mc_platform_p platform)
{
#if 0
  /* This step is now performed inside of the listen_thread */
  /* Close the listen socket */
#ifdef _WIN32
  closesocket(platform->sockfd);
#else
  if(close(platform->sockfd) <0 ) {
		SOCKET_ERROR();
	}
#endif
#endif
  ChInterp_t* interp;

  ListWRLock(platform->message_queue);
  ListClearCB(platform->message_queue, (ListElemDestroyFunc_t)message_Destroy);
  ListWRUnlock(platform->message_queue);
  ListTerminate(platform->message_queue);

  ListWRLock(platform->agent_queue);
  ListClearCB(platform->agent_queue, (ListElemDestroyFunc_t)agent_Destroy);
  ListWRUnlock(platform->agent_queue);
  ListTerminate(platform->agent_queue);

  ListWRLock(platform->connection_queue);
  ListClearCB(platform->connection_queue, (ListElemDestroyFunc_t)connection_Destroy);
  ListWRUnlock(platform->connection_queue);
  ListTerminate(platform->connection_queue);

  df_Destroy(platform->df);

  ams_Destroy(platform->ams);

  acc_Destroy(platform->acc);

  cmd_prompt_Destroy(platform->cmd_prompt);

  /* FIXME: Destroy syncList and barrierList here */
  ListWRLock(platform->syncList);
  ListClearCB(platform->syncList, (ListElemDestroyFunc_t)syncListNodeDestroy);
  ListWRUnlock(platform->syncList);
  ListTerminate(platform->syncList);

  ListWRLock(platform->barrier_queue);
  ListClearCB(platform->barrier_queue, (ListElemDestroyFunc_t)barrier_node_Destroy);
  ListWRUnlock(platform->barrier_queue);
  ListTerminate(platform->barrier_queue);

  free(platform->hostname);

  /* End each of the allocated Ch Interpreters */
  ListWRLock(platform->interpreter_queue);
  while((interp = (ChInterp_t*)ListPop(platform->interpreter_queue))) {
    Ch_End(*interp);
    free(interp);
  }
  ListWRUnlock(platform->interpreter_queue);
  ListTerminate(platform->interpreter_queue);

  COND_DESTROY(platform->MC_signal_cond);
  free(platform->MC_signal_cond);
  COND_DESTROY(platform->MC_sync_cond);
  free(platform->MC_sync_cond);
  MUTEX_DESTROY(platform->MC_signal_lock);
  free(platform->MC_signal_lock);
  MUTEX_DESTROY(platform->MC_sync_lock);
  free(platform->MC_sync_lock);

  MUTEX_DESTROY(platform->MC_steer_lock);
  free(platform->MC_steer_lock);
  COND_DESTROY(platform->MC_steer_cond);
  free(platform->MC_steer_cond);

  MUTEX_DESTROY(platform->quit_lock);
  free(platform->quit_lock);
  COND_DESTROY(platform->quit_cond);
  free(platform->quit_cond);

  MUTEX_DESTROY(platform->giant_lock);
  free(platform->giant_lock);
  COND_DESTROY(platform->giant_cond);
  free(platform->giant_cond);

  /* Free up the interp options */
  if (platform->interp_options != NULL) {
    if (platform->interp_options->chhome != NULL) {
      free(platform->interp_options->chhome);
    }
    free(platform->interp_options);
  }

  free(platform);
  return MC_SUCCESS;
}

ChInterp_t* interpreter_queue_CreateRetrieve( list_t *queue , ChOptions_t* interp_options) {
	/* This function retrieves an interpreter from the list if there are any
		on the queue. Otherwise, it creates a new one and returns it. */
	ChInterp_t* interp;
  ListWRLock(queue);
	interp = (ChInterp_t*) ListPop(queue);
	if (interp == NULL) {
		interp = (ChInterp_t*)malloc(sizeof(ChInterp_t));
		if( Ch_Initialize(interp, interp_options) != CH_OK) {
      /* Could not initialize another interpreter */
      interp = NULL;
    } else {
    /* Initialize the agent intepreter variables */
      agent_ChScriptInitVar(interp);
    }
	}
  ListWRUnlock(queue);
	return interp;
}

