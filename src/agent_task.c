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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/agent_task.h"
#include "include/mc_error.h"

agent_task_p
agent_task_New(void)
{
  agent_task_p task;
  task = (agent_task_p)malloc(sizeof(agent_task_t));
  if(task == NULL) {
    fprintf(stderr, "Memory Error. %s:%d\n", __FILE__, __LINE__);
  } else {
    memset(task, 0, sizeof(agent_task_t)); 
  }
  task->agent_variable_list = agent_variable_list_New();

  task->saved_variables = NULL;
  task->num_saved_variables = 0;

  task->agent_file_list = agent_file_list_New();

  return task;
}

agent_task_p 
agent_task_Copy(agent_task_p task)
{
	int i;
	interpreter_variable_data_t* interp_data;
	agent_task_p cp_task;
	cp_task = (agent_task_p)malloc(sizeof(agent_task_t));

	cp_task->number_of_elements = task->number_of_elements;
	cp_task->size_of_element_array = task->size_of_element_array;
	cp_task->persistent = task->persistent;
	cp_task->init_agent_status = task->init_agent_status;

	cp_task->var_name = (char*)malloc
		(
		 sizeof(char) * 
		 (strlen(task->var_name) + 1)
		);
	strcpy(cp_task->var_name, task->var_name);

	cp_task->server_name = (char*)malloc
		(
		 sizeof(char) * 
		 (strlen(task->server_name) + 1)
		);
	strcpy(cp_task->server_name, task->server_name);

	if (task->code_id != NULL) {
		cp_task->code_id = (char*)malloc
			(
			 sizeof(char) * 
			 (strlen(task->code_id) + 1)
			);
		strcpy(cp_task->code_id, task->code_id);
	} else {
		cp_task->code_id = NULL;
	}

	if(task->agent_return_data != NULL) {
		cp_task->agent_return_data = interpreter_variable_data_Copy(task->agent_return_data);
	}

	/* Copy the agent variable list */
	cp_task->agent_variable_list = agent_variable_list_New();
	for(i = 0; i < task->agent_variable_list->size; i++) {
		interp_data = (interpreter_variable_data_t*)ListSearch(
				(list_p)task->agent_variable_list,
				i );
		if (interp_data == NULL) { continue; }
		interp_data = interpreter_variable_data_Copy(interp_data);
		ListAdd(
				(list_p)cp_task->agent_variable_list,
				interp_data
				);
	}

	cp_task->saved_variables = (char**)malloc(sizeof(char*) * (task->num_saved_variables+1));
	for(i = 0; i < task->num_saved_variables; i++) {
		cp_task->saved_variables[i] = strdup(task->saved_variables[i]);
	}
	cp_task->saved_variables[i] = NULL;
	cp_task->num_saved_variables = task->num_saved_variables;

	return cp_task;
}

int
agent_task_Destroy( agent_task_p agent_task )
{
  int i;
  if(agent_task == NULL) {
    return MC_SUCCESS;
  }
  if (agent_task->var_name != NULL) {
    free(agent_task->var_name);
  }
  if (agent_task->server_name != NULL) {
    free(agent_task->server_name);
  }
  if (agent_task->code_id != NULL) {
    free(agent_task->code_id);
  }

  agent_variable_list_Destroy(agent_task->agent_variable_list);

  if(agent_task->saved_variables != NULL) {
    for(i = 0; agent_task->saved_variables[i] != NULL; i++) {
      free(agent_task->saved_variables[i]);
    }
    free(agent_task->saved_variables);
  }

  interpreter_variable_data_Destroy(agent_task->agent_return_data);

  free(agent_task);

  return MC_SUCCESS;
}

