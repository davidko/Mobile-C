/* SVN FILE INFO
 * $Revision: 517 $ : Last Committed Revision
 * $Date: 2010-06-11 12:06:47 -0700 (Fri, 11 Jun 2010) $ : Last Committed Date */
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

#include "include/ams.h"
#include "include/agent.h"
#include "include/data_structures.h"
#include "include/mc_platform.h"

  int
ams_Destroy(ams_p ams)
{
  MUTEX_DESTROY(ams->runflag_lock);
  free(ams->runflag_lock);
  COND_DESTROY(ams->runflag_cond);
  free(ams->runflag_cond);
  MUTEX_DESTROY(ams->waiting_lock);
  free(ams->waiting_lock);
  COND_DESTROY(ams->waiting_cond);
  free(ams->waiting_cond);
  free(ams);
  return MC_SUCCESS;
}

  ams_p 
ams_Initialize(mc_platform_p mc_platform)
{
  ams_p ams;
  ams = (ams_p)malloc(sizeof(ams_t));
  CHECK_NULL(ams, exit(0););
  ams->mc_platform = mc_platform;

  ams->runflag_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  CHECK_NULL(ams->runflag_lock, exit(0););
  MUTEX_INIT(ams->runflag_lock);

  ams->runflag_cond = (COND_T*)malloc(sizeof(COND_T));
  CHECK_NULL(ams->runflag_cond, exit(0););
  COND_INIT(ams->runflag_cond);

  ams->run = 0;

  ams->waiting = 0;
  ams->waiting_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(ams->waiting_lock);
  ams->waiting_cond = (COND_T*)malloc(sizeof(COND_T));
  COND_INIT(ams->waiting_cond);

  return ams;
}

  void 
ams_Print(ams_p ams)
{
  int i;
  MCAgent_t agent;
  agent_queue_p alist;

  alist = ams->mc_platform->agent_queue;

  MUTEX_LOCK(alist->lock);

  if(alist->size == 0) 
  {
    MUTEX_UNLOCK(alist->lock);
    return;
  }

  /* find all agents and print their relvent information to the screen  */
  printf("%d total agents on board.\n", alist->size);
  for(i=0; i<alist->size; i++)
  {
    agent = (MCAgent_t)ListSearch(alist->list, i);
    MUTEX_LOCK(agent->agent_status_lock);
    printf("Agent id: %lu, Connect id: %lu, status: %u\n",
        agent->id,
        agent->connect_id,
        agent->agent_status);
    MUTEX_UNLOCK(agent->agent_status_lock);
  }

  MUTEX_UNLOCK(alist->lock);
  return;
}

  extern int 
ams_ManageAgentList(ams_p ams)
{
  /*variables */
  MCAgent_t current_agent;
  int index = 0;
  agent_queue_p alist;
  mc_platform_p global;
  message_p message;

  alist = ams->mc_platform->agent_queue;
  global = ams->mc_platform;

  /* looks through the agent list and performs action on agent 
     depending upon the status that the agent displays */
  MUTEX_LOCK(alist->lock);
  for(index=0; index<alist->size; index++)
  {
    if((current_agent = (MCAgent_t)ListSearch(alist->list, index)))
    {
			if(current_agent->binary) {continue;} /* Do not deal with binary agents */
      MUTEX_UNLOCK(alist->lock);
      MUTEX_LOCK(current_agent->lock);
      current_agent->orphan = 0;
      MUTEX_LOCK(global->quit_lock);
      MUTEX_LOCK(current_agent->agent_status_lock);
      if(global->quit && current_agent->agent_status != MC_WAIT_MESSGSEND) {
        MUTEX_UNLOCK(current_agent->agent_status_lock);
        MUTEX_UNLOCK(global->quit_lock);
        MUTEX_UNLOCK(current_agent->lock);
        MC_TerminateAgent(current_agent);
        /* If we can lock the agent's run lock, then it must not be running. */
        MUTEX_LOCK(current_agent->run_lock);
        MUTEX_UNLOCK(current_agent->run_lock);
        continue;
      } else {
        MUTEX_UNLOCK(current_agent->agent_status_lock);
      }
      MUTEX_UNLOCK(global->quit_lock);
      MUTEX_LOCK(current_agent->agent_status_lock);
      switch(current_agent->agent_status)
      {
        case MC_WAIT_CH :
          MUTEX_UNLOCK(current_agent->lock);
          MUTEX_UNLOCK(current_agent->agent_status_lock);
          agent_RunChScript(current_agent, global);
          break;
        case MC_AGENT_ACTIVE :
          MUTEX_UNLOCK(current_agent->lock);
          MUTEX_UNLOCK(current_agent->agent_status_lock);
          /* nothing is done if the agent in question is ACTIVE */
          break;
        case MC_WAIT_MESSGSEND :
          current_agent->agent_status = MC_WAIT_FINISHED;
          COND_BROADCAST(current_agent->agent_status_cond);
          MUTEX_UNLOCK(current_agent->agent_status_lock);
          MUTEX_UNLOCK(current_agent->lock);
          MUTEX_LOCK(ams->runflag_lock);
          ams->run = 1;
          MUTEX_UNLOCK(ams->runflag_lock);
          MUTEX_UNLOCK(current_agent->lock);
          message = message_New();
          if (
              message_InitializeFromAgent
              (
               ams->mc_platform,
               message,
               current_agent
              )
             )
          {
	    fprintf(stderr, "Error initializing message from agent. %s:%d\n", __FILE__, __LINE__);
            message_Destroy(message);
            message = NULL;
          } else {
            /* Rename the agent: This is done so that if the agent is coming back here
             * very quickly, the names will not conflict. */
            current_agent->name = (char*)realloc(
                current_agent->name,
                sizeof(char) * (strlen(current_agent->name) + 10)
                );
            strcat(current_agent->name, "_SENDING");
            message_queue_Add(
                ams->mc_platform->message_queue,
                message
                );
          }
          break;
        case MC_AGENT_NEUTRAL :
          MUTEX_UNLOCK(current_agent->agent_status_lock);
          MUTEX_UNLOCK(current_agent->lock);
          break;
        case MC_WAIT_FINISHED :
          MUTEX_UNLOCK(current_agent->agent_status_lock);
          MUTEX_UNLOCK(current_agent->lock);
          agent_queue_RemoveIndex(alist, index);
          // Change index = 0 to index-- to fix the problem where agents 
          // remain in an agency when they should be removed from the agency.
          // Yu-Cheng Chou July 28, 2009
          index--;
          break;
        default : 
          printf("ERROR IN AGENT FORMAT"); 
          printf("Agent Format %d not recognized.",
                 current_agent->agent_status);
          /* Flush the invalid agent. */
          current_agent->agent_status = MC_WAIT_FINISHED;
          COND_BROADCAST(current_agent->agent_status_cond);
          MUTEX_UNLOCK(current_agent->agent_status_lock);
          MUTEX_UNLOCK(current_agent->lock);
      }
    } else {
      MUTEX_UNLOCK( alist->lock );
    }
    MUTEX_LOCK( alist->lock );
  }
  MUTEX_UNLOCK( alist->lock );
  return 0 ;
}

  void
ams_Start(mc_platform_p mc_platform)
{
  ams_p ams = mc_platform->ams;
#ifndef _WIN32
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  if(mc_platform->stack_size[MC_THREAD_AMS] != -1) {
    pthread_attr_setstacksize
      (
       &attr,
       mc_platform->stack_size[MC_THREAD_AMS]
      );
  }
#else
  int stack_size;
  if (mc_platform->stack_size[MC_THREAD_AMS] < 1) {
    /* In windows, 0 is default, not min */
    stack_size = mc_platform->stack_size[MC_THREAD_AMS]+1;
  } else {
    stack_size = mc_platform->stack_size[MC_THREAD_AMS];
  }
#endif
  THREAD_CREATE
    (
     &ams->thread,
     ams_Thread,
     mc_platform
    );
}
#ifndef _WIN32
  void*
ams_Thread(void* arg)
#else
  DWORD WINAPI
ams_Thread( LPVOID arg )
#endif
{
  mc_platform_p mc_platform = (mc_platform_p)arg;
  ams_p ams = mc_platform->ams;
  while(1) {
    MUTEX_LOCK(ams->runflag_lock);
    MUTEX_LOCK(mc_platform->quit_lock);
    while(ams->run == 0 && !mc_platform->quit) {
      MUTEX_UNLOCK(mc_platform->quit_lock);
      /* Set waiting flag */
      MUTEX_LOCK(ams->waiting_lock);
      ams->waiting = 1;
      COND_BROADCAST(ams->waiting_cond);
      MUTEX_UNLOCK(ams->waiting_lock);
      /* Wait for activity */
      COND_WAIT
        (
         ams->runflag_cond,
         ams->runflag_lock
        );
      MUTEX_LOCK(mc_platform->quit_lock);
    }
    /* Set waiting flag */
    MUTEX_LOCK(ams->waiting_lock);
    ams->waiting = 0;
    COND_BROADCAST(ams->waiting_cond);
    MUTEX_UNLOCK(ams->waiting_lock);
    if (ams->run == 0 && mc_platform->quit) {
      MUTEX_UNLOCK(mc_platform->quit_lock);
      MUTEX_UNLOCK(ams->runflag_lock);
      ams_ManageAgentList(ams);
      THREAD_EXIT();
    }
    ams->run = 0;
    MUTEX_UNLOCK(mc_platform->quit_lock);
    MUTEX_UNLOCK(ams->runflag_lock);
    ams_ManageAgentList(ams);
  }
  THREAD_EXIT();
}
