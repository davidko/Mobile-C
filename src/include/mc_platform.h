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

#ifndef _MC_PLATFORM_H_
#define _MC_PLATFORM_H_
#include "acc.h"
#include "ams.h"
#include "barrier.h"
#include "cmd_prompt.h"
#include "config.h"
#include "connection.h"
#include "data_structures.h"
#include "df.h"
#include "libmc.h"
#include "../mc_sync/sync_list.h"
#include "../security/asm.h"

struct mc_platform_s{
#ifdef _WIN32
  WSADATA wsaData;
#endif
  int err;
#ifdef MC_SECURITY
  int enable_security;
#endif

  char* hostname;
  unsigned short port;
  int initInterps;

  /* These are the standard agency data structs. */
  message_queue_p asm_message_queue;
#ifdef MC_SECURITY
  asm_queue_p asm_queue; /* Holds encryption info for each remote hoste */
#endif
  message_queue_p message_queue;
  agent_queue_p agent_queue;
  connection_queue_p connection_queue;

  df_p df;
  ams_p ams;
  acc_p acc;
  cmd_prompt_p cmd_prompt;
#ifdef MC_SECURITY
  mc_asm_p security_manager;
#endif

  syncList_p syncList;
  barrier_queue_p barrier_queue;

  listen_thread_arg_p listen_thread_arg;
  listen_thread_arg_p client_thread_arg;

  int default_agentstatus;

  int stack_size[MC_THREAD_ALL];

  ChOptions_t *interp_options;

  /* MC Cond Signal System */
  COND_T              *MC_signal_cond;
  COND_T              *MC_sync_cond;
  MUTEX_T             *MC_signal_lock;
  MUTEX_T             *MC_sync_lock;
  enum MC_Signal_e    MC_signal;

  /* MC Steer System */
  enum MC_SteerCommand_e MC_steer_command;
  MUTEX_T *MC_steer_lock;
  COND_T *MC_steer_cond;

  /* giant lock : Threads will pause if giant==0. 
   * giant_lock and giant_cond protect giant. */
  int giant;
  MUTEX_T *giant_lock;
  COND_T  *giant_cond;

  int quit;
  MUTEX_T *quit_lock;
  COND_T *quit_cond;
  int sockfd; /* File Descriptor for listen socket */
  char private_key[1210]; // private key of agency

  MCAgency_t agency; /* A pointer to the platform's agency structure */

	/* Need a linked list of Ch interpreters. */
	interpreter_queue_p interpreter_queue;

  /* Are we using bluetooth? */
  int bluetooth;
};
typedef struct mc_platform_s mc_platform_t;
typedef mc_platform_t* mc_platform_p;

mc_platform_p 
mc_platform_Initialize(MCAgency_t agency, ChOptions_t* ch_options);

int 
mc_platform_Destroy(mc_platform_p mc_platform);
#endif 
