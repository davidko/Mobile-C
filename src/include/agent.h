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

#ifndef _AGENT_H_
#define _AGENT_H_

#include "agent_datastate.h"
#include "agent_task.h"
#include "libmc.h"
#include "message.h"
#include "macros.h"

struct mc_platform_s;

struct agent_s {

  /* basic id numbers to determine agent characteristics */
  u_long id;
  char* name;
  u_long connect_id;
#ifndef _WIN32
  time_t arrival_time;
#else
  SYSTEMTIME arrival_time;
#endif

  char *owner;
  char *home;
  char *sender;
  int home_port;
	char *wg_code; /* The workgroup code of the agent. Only agents of the same workgroup should
                    be able to manipulate each other. */
	char *agent_address;

  int orphan; /* If an agent is an 'orphan', it means it is not attached to an
                 agency. */

  /* primary agent data */
  agent_datastate_p datastate;  
  enum MC_AgentType_e agent_type; 
  enum MC_AgentStatus_e agent_status;

  int return_data;

  /* threading variables */
  ChInterp_t* agent_interp;
  /* run_lock should be locked whenever the interpreter is running
   * to prevent simultaneous runs. */
  MUTEX_T *run_lock;

#ifndef _WIN32
  int agent_thread_id; 
#else
  DWORD agent_thread_id;
#endif
  THREAD_T agent_thread;

  /* Agent Mailbox */
  list_t* mailbox; /* Holds fipa_acl_message_t* */

  /* Agent Flags */
  int agent_pipe_active;
  int agent_ready_to_send;
  int agent_pipe_ready_to_read;
  int agent_script_ready;
  int agent_persistent;

  struct mc_platform_s *mc_platform;

  MUTEX_T* lock;
  MUTEX_T* agent_status_lock;
  COND_T*  agent_status_cond;

  list_t* agent_share_data_queue;

	int binary; /* Indicates whether or not this is a binary space agent */
};
#ifndef AGENT_T
#define AGENT_T
typedef struct agent_s agent_t;
typedef agent_t* MCAgent_t;
typedef agent_t* agent_p;
#endif

int 
agent_AddPersistentVariable(agent_p agent, int task_num, const char* var_name);

agent_p
agent_New(void);

int agent_Init(agent_p agent);

agent_p 
agent_NewBinary( struct mc_platform_s *mc_platform);

agent_p
agent_Copy(const agent_p agent);

  int
agent_Destroy(agent_p agent);

agent_p
agent_Initialize(
    struct mc_platform_s* mc_platform,
    message_p message,
    int id);

int 
agent_Destroy(agent_p agent);

void agent_RunChScript(MCAgent_t agent, struct mc_platform_s* global);
#ifndef _WIN32
void * agent_RunChScriptThread(void * agent);
#else
DWORD WINAPI agent_RunChScriptThread(void * ChAgent);
#endif

void*
agent_ChScriptInitVar(ChInterp_t* interp);

int agent_Print(agent_t* agent);
int agent_CmpName(const void* key, void* element);
int agent_CmpID(int* id, agent_t* agent);
#endif 
