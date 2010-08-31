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

#include "include/interpreter_variable_data.h"
#include "include/agent.h"

interpreter_variable_data_p
interpreter_variable_data_New(void)
{
  interpreter_variable_data_p agent_variable_data;
  agent_variable_data = (interpreter_variable_data_p)malloc(sizeof(interpreter_variable_data_t));
  CHECK_NULL(agent_variable_data, exit(0););
  agent_variable_data->name = NULL;
  agent_variable_data->size = 0;
  agent_variable_data->data_type = (ChType_t)0;
  agent_variable_data->array_dim = 0;
  agent_variable_data->array_extent = NULL;
  agent_variable_data->data = NULL;
  return agent_variable_data;
}

interpreter_variable_data_p 
interpreter_variable_data_InitializeFromAgent(agent_p agent)
{
    int i;
    int size;
    int data_type_size;
    int progress;
    interpreter_variable_data_t *agent_return;
    agent_return = (interpreter_variable_data_t*)malloc(sizeof(interpreter_variable_data_t));
    agent_return->name = strdup(
            agent->datastate->tasks[agent->datastate->task_progress]
            ->var_name );

    /* Get the array data type */
    agent_return->data_type = Ch_DataType(
            *agent->agent_interp,
            agent->datastate->tasks[agent->datastate->task_progress]
            ->var_name );
    /* Get the array dimension */
    agent_return->array_dim = Ch_ArrayDim(
            *agent->agent_interp,
            agent->datastate->tasks[agent->datastate->task_progress]
            ->var_name );
    /* Get the array extents */
    agent_return->array_extent = (int*)malloc(
            sizeof(int) * agent_return->array_dim );
    for (i=0; i<agent_return->array_dim; i++) {
        agent_return->array_extent[i] = 
            Ch_ArrayExtent(
                    *agent->agent_interp,
                    agent->datastate
                    ->tasks[agent->datastate->task_progress]
                    ->var_name,
                    i );
    }
    /* Finally, allocate and point returnData to the right place. */
    size = 1;
    for (i=0; i < agent_return->array_dim; i++) {
        size *= agent_return->array_extent[i];
    }

    /* Now get the data type size */
    CH_DATATYPE_SIZE(agent_return->data_type, data_type_size);

    agent_return->data = (void*)malloc(size * data_type_size);
    CHECK_NULL(agent_return->data, exit(0));
    /* Copy the data over from the agent */
    /* For now, only support statically allocated global vars. */
    progress = agent->datastate->task_progress;
    i = 0;

    if (agent_return->array_dim == 0) {
        memcpy(
                agent_return->data,
                (void*)Ch_GlobalSymbolAddrByName(
                    *agent->agent_interp,
                    agent->datastate->tasks[progress]->var_name),
                size*data_type_size
              );

    } else {
        memcpy(
                agent_return->data,
                (void*)Ch_GlobalSymbolAddrByName(
                    *agent->agent_interp,
                    agent->datastate->tasks[progress]->var_name),
                size*data_type_size
              );
    }
    agent_return->size = size*data_type_size;

    /* getAgentReturnArrays(
       agent_return->returnData,
       Ch_SymbolAddrByName(
       agent->agent_interp,
       agent->datastate->tasks[progress]->var_name),
       &i,
       agent_return->array_dim,
       agent_return->array_extent,
       agent_return->data_type ); */

    return agent_return;
}

interpreter_variable_data_p
interpreter_variable_data_Initialize(agent_p agent, const char* varname)
{
  int i;
  int size;
  int data_type_size;
  interpreter_variable_data_t *interp_variable;

  /* Make sure the agent is not running */
  MUTEX_LOCK(agent->run_lock);

  interp_variable = (interpreter_variable_data_t*)malloc(sizeof(interpreter_variable_data_t));
  interp_variable->name = strdup(varname);

  /* Get the array data type */
  interp_variable->data_type = Ch_DataType(
      *agent->agent_interp,
      varname );
  /* Get the array dimension */
  interp_variable->array_dim = Ch_ArrayDim(
      *agent->agent_interp,
      varname );
  /* Get the array extents */
  interp_variable->array_extent = (int*)malloc(
      sizeof(int) * interp_variable->array_dim );
  for (i=0; i<interp_variable->array_dim; i++) {
    interp_variable->array_extent[i] = 
      Ch_ArrayExtent(
          *agent->agent_interp,
          varname,
          i );
  }
  /* Finally, allocate and point returnData to the right place. */
  size = 1;
  for (i=0; i < interp_variable->array_dim; i++) {
    size *= interp_variable->array_extent[i];
  }

  /* Now get the data type size */
  CH_DATATYPE_SIZE(interp_variable->data_type, data_type_size);

  interp_variable->data = (void*)malloc(size * data_type_size);
  CHECK_NULL(interp_variable->data, exit(0));
  /* Copy the data over from the agent */
  /* For now, only support statically allocated global vars. */
  i = 0;

  if (interp_variable->array_dim == 0) {
    memcpy(
        interp_variable->data,
        (void*)Ch_GlobalSymbolAddrByName(
          *agent->agent_interp,
          varname),
        size*data_type_size
        );

  } else {
    memcpy(
        interp_variable->data,
        (void*)Ch_GlobalSymbolAddrByName(
          *agent->agent_interp,
          varname),
        size*data_type_size
        );
  }
  interp_variable->size = size*data_type_size;

  MUTEX_UNLOCK(agent->run_lock);
  return interp_variable;
}

int
interpreter_variable_data_Destroy(interpreter_variable_data_p agent_variable_data)
{
  if (agent_variable_data == NULL) {
    return MC_SUCCESS;
  }
  if (agent_variable_data->name != NULL) {
    free(agent_variable_data->name);
  }
  if (agent_variable_data->array_extent != NULL) {
    free(agent_variable_data->array_extent);
  }
  if (agent_variable_data->data != NULL) {
    free(agent_variable_data->data);
  }
  free(agent_variable_data);
  return MC_SUCCESS;
}

interpreter_variable_data_p
interpreter_variable_data_Copy(interpreter_variable_data_p src)
{
  struct interpreter_variable_data_s* tmp;
  tmp = interpreter_variable_data_New();
  if(src->name != NULL) 
    tmp->name = strdup(src->name);
  tmp->size = src->size;
  tmp->data_type = src->data_type;
  tmp->array_dim = src->array_dim;
  if(src->array_extent != NULL) {
    tmp->array_extent = (int*)malloc(sizeof(int)*src->array_dim);
    memcpy(tmp->array_extent, src->array_extent, sizeof(int)*src->array_dim);
  }
  if(src->data != NULL) {
    tmp->data = memcpy(tmp->array_extent, src->array_extent, src->size);
  }

  return tmp;
}
