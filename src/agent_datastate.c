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

#include <mxml.h>
#include "include/agent_datastate.h"
#include "include/macros.h"
#include "include/mc_error.h"

agent_datastate_p
agent_datastate_Copy(const agent_datastate_p datastate)
{
  char** code_id_itr;
  char** code_itr;
  int num_agent_codes = 0;
  int i;
  agent_datastate_p cp_data;
  cp_data = agent_datastate_New();
  /* Copy agent_codes */
  code_itr = datastate->agent_codes;
  /* First, count how many of them there are. */
  while (*code_itr != NULL) {
    num_agent_codes++;
    code_itr++;
  }
  cp_data->agent_code_ids = (char**) malloc
    (
     num_agent_codes + 1
    );
  cp_data->agent_codes = (char**)malloc
    (
     num_agent_codes + 1
    );
  code_id_itr = datastate->agent_code_ids;
  code_itr = datastate->agent_codes;
  i = 0;
  while (*code_id_itr != NULL && *code_itr != NULL) {
    cp_data->agent_code_ids[i] = (char*)malloc
      (
       sizeof(char) * 
       (strlen(*code_id_itr) + 1)
      );
    strcpy(cp_data->agent_code_ids[i], *code_id_itr);

    cp_data->agent_codes[i] = (char*)malloc
      (
       sizeof(char) * 
       (strlen(*code_itr) + 1 )
      );
    strcpy(cp_data->agent_codes[i], *code_itr);

    i++;
    code_id_itr++;
    code_itr++;
  }
  cp_data->agent_code_ids[i] = NULL;
  cp_data->agent_codes[i] = NULL;
  cp_data->agent_code = cp_data->agent_codes[0];

  cp_data->task_progress = datastate->task_progress;
  cp_data->return_data = datastate->return_data;
  cp_data->number_of_tasks = datastate->number_of_tasks;
  cp_data->persistent = datastate->persistent;
  cp_data->init_agent_status = datastate->init_agent_status;

  /* Copy the tasks */
  cp_data->tasks = (agent_task_t**)malloc
    (
     sizeof(agent_task_t*) * cp_data->number_of_tasks
    );
  for (i = 0; i < cp_data->number_of_tasks; i++) {
    cp_data->tasks[i] = agent_task_Copy(datastate->tasks[i]);
  }

  return cp_data;
}
  
agent_datastate_p
agent_datastate_New( void )
{
  agent_datastate_p agent_datastate;
  agent_datastate = (agent_datastate_p)malloc(sizeof(agent_datastate_t));
  CHECK_NULL(agent_datastate, exit(0););
  
  agent_datastate->agent_code = NULL;
  agent_datastate->tasks = NULL;
  agent_datastate->xml_agent_root = NULL;
  agent_datastate->xml_root = NULL;
  agent_datastate->task_progress = 0;
  agent_datastate->return_data = 0;
  agent_datastate->number_of_tasks = 0;
  agent_datastate->persistent = 0;
  agent_datastate->init_agent_status = 0;
	agent_datastate->progress_modifier = 0;

  return agent_datastate;
}

int
agent_datastate_Destroy( agent_datastate_p agent_datastate )
{
  int i;
	if(agent_datastate == NULL) {return 0;}
  if (agent_datastate->agent_codes != NULL) {
    i = 0;
    while (agent_datastate->agent_codes[i] != NULL) {
      free(agent_datastate->agent_codes[i]);
      i++;
    }
    free(agent_datastate->agent_codes);
  }
  if (agent_datastate->agent_code_ids != NULL) {
    i = 0;
    while(agent_datastate->agent_code_ids[i] != NULL) {
      free(agent_datastate->agent_code_ids[i]);
      i++;
    }
    free(agent_datastate->agent_code_ids);
  }
  for 
    ( 
     i = 0;
     i < agent_datastate->number_of_tasks;
     i++
    )
  {
    agent_task_Destroy(agent_datastate->tasks[i]);
  }
  free(agent_datastate->tasks);

  if(agent_datastate->xml_root != NULL) {
    mxmlDelete(agent_datastate->xml_root);
  }
  free(agent_datastate);
  return MC_SUCCESS;
}

