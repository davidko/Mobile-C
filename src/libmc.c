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
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#else
#include <windows.h>
#include <memory.h>
#endif
#include <embedch.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "include/libmc.h"
#include "include/macros.h"
#include "include/mc_platform.h"
#include "include/message.h"
#include "include/data_structures.h"
#include "include/fipa_acl_envelope.h"
#include "include/fipa_acl.h"
#include "include/agent.h"
#include "include/agent_task.h"
#include "include/agent_lib.h"
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

/* The Global platform variable */
mc_platform_p g_mc_platform;

/* **************************** *
 * Libmc Binary Space Functions *
 * **************************** */

EXPORTMC int 
MC_AclDestroy(struct fipa_acl_message_s* message)
{
  return fipa_acl_message_Destroy(message);
}

EXPORTMC fipa_acl_message_t* 
MC_AclNew(void) {
  return fipa_acl_message_New();
}

EXPORTMC int 
MC_AclPost(MCAgent_t agent, struct fipa_acl_message_s* message)
{
  return agent_mailbox_Post(agent->mailbox, message);
}

EXPORTMC fipa_acl_message_t*
MC_AclReply(fipa_acl_message_t* acl_message)
{
  return fipa_Reply(acl_message);
}

EXPORTMC fipa_acl_message_t*
MC_AclRetrieve(MCAgent_t agent)
{
  return agent_mailbox_Retrieve(agent->mailbox);
}

EXPORTMC int
MC_AclSend(MCAgency_t attr, fipa_acl_message_t* acl)
{
  /* FIXME: We may want to multithread this later for asynchronous
   * sending to multiple hosts. But for now, we will use a simple
   * loop. */
  int i;
  int err;
  mtp_http_t* msg;
  dynstring_t* msg_string;
  message_p mc_message;

  char* host;
  int port;
  char* target;
  MCAgent_t agent;
  int num_addresses = 0;

	fipa_acl_message_t* tmp;

  err = fipa_acl_Compose(&msg_string, acl);
  if( err ) {
    fprintf(stderr, "ACL Message Compose Error. %s:%d\n", __FILE__, __LINE__);
    return err;
  }
  for(i = 0; i < acl->receiver->num; i++) {
    /* The receiver may or may not have an address. If it does, send the
     * message to that address. If not, assume that the target agent is
     * local and try to send to that agent. */
    if (acl->receiver->fipa_agent_identifiers[i]->addresses == NULL) {
      num_addresses = 0;
    } else {
      num_addresses = acl->receiver->fipa_agent_identifiers[i]->addresses->num;
    }

    if (num_addresses == 0) {
      agent = MC_FindAgentByName(
          attr,
          acl->receiver->fipa_agent_identifiers[i]->name );
      if (agent == NULL) {
        fprintf(stderr, "Could not find local agent:%s. %s:%d\n",
            acl->receiver->fipa_agent_identifiers[i]->name,
            __FILE__, __LINE__);
				continue;
      }
			tmp = fipa_acl_message_Copy(acl);
      MC_AclPost(agent, tmp);
    } else {
      msg = mtp_http_New();
      /* Send to the first address listed */
      err = http_to_hostport(
          acl->receiver->fipa_agent_identifiers[i]->addresses->urls[0]->str,
          &host,
          &port,
          &target );
      if (err) {
        fprintf(stderr, "Invalid address. %s:%d\n", __FILE__, __LINE__);
        return err;
      }
      msg->host = strdup(host);
      msg->target = strdup(target);
      msg->message_parts = 2;
      msg->content = (mtp_http_content_t *)malloc(
          sizeof(mtp_http_content_t) * 2);

      /* Set up the message envelope */
      msg->content[0].data = (void*)fipa_envelope_Compose(acl);
      
      msg->content[0].content_type = strdup("application/xml");

      /* Set up the ACL message */
      msg->content[1].data = (void*)strdup(msg_string->message);
      msg->content[1].content_type = strdup("application/text");

      mc_message = mtp_http_CreateMessage(
          msg,
          host,
          port );
      mc_message->message_type = FIPA_ACL;
      mc_message->target = strdup("acc");

      message_Send
        (
				 attr->mc_platform,
         mc_message,
				 attr->mc_platform->private_key
        );
      //message_Destroy(mc_message);
			// We may no longer destroy the message here since we do not know how long it will take message_Send to complete.
      mtp_http_Destroy(msg);
      free(host);
      free(target);
    }
  }
  dynstring_Destroy(msg_string);
  return 0;
}

EXPORTMC fipa_acl_message_t* 
MC_AclWaitRetrieve(MCAgent_t agent)
{
  return agent_mailbox_WaitRetrieve(agent->mailbox);
}

/* ACL Helper Functions Here */

EXPORTMC enum fipa_protocol_e MC_AclGetProtocol(
    struct fipa_acl_message_s* acl)
{
  return (enum fipa_protocol_e)acl->performative;
}

EXPORTMC char* MC_AclGetConversationID(
    struct fipa_acl_message_s* acl)
{
  return acl->conversation_id->content.string->content;
}

EXPORTMC enum fipa_performative_e MC_AclGetPerformative(
    struct fipa_acl_message_s* acl)
{
  return acl->performative;
}

EXPORTMC int MC_AclGetSender(
    struct fipa_acl_message_s* acl,
    char** name, /* OUT: Will allocate a text string to be freed by the
                          user. */
    char** address /* OUT: Will allocate a text string to be freed by the user
                    */
    )
{
  *name = strdup(acl->sender->name);
  *address = strdup( acl->sender->addresses->urls[0]->str );
  return 0;
}

EXPORTMC const char* MC_AclGetContent(
    struct fipa_acl_message_s* acl)
{
  return acl->content->content;
}

EXPORTMC int MC_AclSetProtocol(
    fipa_acl_message_t* acl,
    enum fipa_protocol_e protocol)
{
  if(protocol <= FIPA_PROTOCOL_ERROR ||
     protocol >= FIPA_PROTOCOL_END)
  {
    fprintf(stderr, 
      "AclSetProtocol: Incorrect protocol. %s:%d\n", 
      __FILE__, __LINE__);
    return -1;
  }
  acl->protocol = protocol;
  return 0;
}

EXPORTMC int MC_AclSetConversationID(
    fipa_acl_message_t* acl,
    char* id)
{

  if(!id)
  {
    fprintf(stderr, 
      "AclSetConversationID: NULL id string. %s:%d\n", 
      __FILE__, __LINE__);
    return -1;
  }
  acl->conversation_id = fipa_expression_New();
  acl->conversation_id->type = FIPA_EXPR_STRING;
  acl->conversation_id->content.string = fipa_string_New();
  acl->conversation_id->content.string->content = strdup(id);
  return 0;
}

EXPORTMC int MC_AclSetPerformative(
    fipa_acl_message_t* acl,
    enum fipa_performative_e performative )
{
  acl->performative = performative;
  return 0;
}

EXPORTMC int MC_AclSetSender(
    fipa_acl_message_t* acl,
    const char* name,
    const char* address )
{
  if(acl->sender != NULL) {
    /* There is already a sender. Lets over-write it */
    fipa_agent_identifier_Destroy(acl->sender);
  }
  acl->sender = fipa_agent_identifier_New();
  acl->sender->name = strdup(name);
  if (address != NULL) {
    acl->sender->addresses = fipa_url_sequence_New();
    acl->sender->addresses->num = 1;
    acl->sender->addresses->urls = (struct fipa_url_s**)malloc(
        sizeof(struct fipa_url_s*));
    acl->sender->addresses->urls[0] = fipa_url_New();
    acl->sender->addresses->urls[0]->str = strdup(address);
  }

  return 0;
}
  
EXPORTMC int MC_AclAddReceiver(
    fipa_acl_message_t* acl,
    const char* name,
    const char* address )
{
  int i;
  struct fipa_agent_identifier_s** tmp;
  if (acl->receiver == NULL) {
    acl->receiver = fipa_agent_identifier_set_New();
  }
  acl->receiver_num++;

  acl->receiver->num++;
  tmp = (struct fipa_agent_identifier_s**)malloc(
      sizeof(struct fipa_agent_identifier_s*)
      * acl->receiver->num);
  /* Copy existing addresses to new array */
  for(i = 0; i < acl->receiver->num-1; i++) {
    tmp[i] = acl->receiver->fipa_agent_identifiers[i];
  }
  /* Create new receiver */
  tmp[i] = fipa_agent_identifier_New();
  tmp[i]->name = strdup(name);
  if(address != NULL) {
    tmp[i]->addresses = fipa_url_sequence_New();
    tmp[i]->addresses->num = 1;
    tmp[i]->addresses->urls = (struct fipa_url_s**)malloc(
        sizeof(struct fipa_url_s*));
    tmp[i]->addresses->urls[0] = fipa_url_New();
    tmp[i]->addresses->urls[0]->str = strdup(address);
  }
  free(acl->receiver->fipa_agent_identifiers);
  acl->receiver->fipa_agent_identifiers = tmp;
  return 0;
}

EXPORTMC int MC_AclAddReplyTo(
    fipa_acl_message_t* acl,
    const char* name,
    const char* address)
{
  int i;
  struct fipa_agent_identifier_s** tmp;
  if (acl->reply_to == NULL) {
    acl->reply_to = fipa_agent_identifier_set_New();
  }

  acl->reply_to->num++;
  tmp = (struct fipa_agent_identifier_s**)malloc(
      sizeof(struct fipa_agent_identifier_s*)
      * acl->reply_to->num);
  /* Copy existing addresses to new array */
  for(i = 0; i < acl->reply_to->num-1; i++) {
    tmp[i] = acl->reply_to->fipa_agent_identifiers[i];
  }
  /* Create new reply_to */
  tmp[i] = fipa_agent_identifier_New();
  tmp[i]->name = strdup(name);
  if(address != NULL) {
    tmp[i]->addresses = fipa_url_sequence_New();
    tmp[i]->addresses->num = 1;
    tmp[i]->addresses->urls = (struct fipa_url_s**)malloc(
        sizeof(struct fipa_url_s*));
    tmp[i]->addresses->urls[0] = fipa_url_New();
    tmp[i]->addresses->urls[0]->str = strdup(address);
  }
  free (acl->reply_to->fipa_agent_identifiers);
  acl->reply_to->fipa_agent_identifiers = tmp;
  return 0;
}

EXPORTMC int MC_AclSetContent(
    fipa_acl_message_t* acl,
    const char* content )
{
  if (acl->content != NULL) {
    /* There is already content. Lets over-write it. */
    fipa_string_Destroy(acl->content);
  }
  acl->content = fipa_string_New();
  acl->content->content = strdup(content);

  return 0;
}

/* End ACL Helper Functions */

EXPORTMC int 
MC_AddAgent(MCAgency_t attr, MCAgent_t agent) /*{{{*/
{
  agent->mc_platform = attr->mc_platform;

  agent_queue_Add(attr->mc_platform->agent_queue, agent);

  MUTEX_LOCK(attr->mc_platform->ams->runflag_lock);
  attr->mc_platform->ams->run = 1;
  COND_SIGNAL(attr->mc_platform->ams->runflag_cond);
  MUTEX_UNLOCK(attr->mc_platform->ams->runflag_lock);
  return 0;
} /*}}}*/

EXPORTMC int MC_AddStationaryAgent(
    MCAgency_t agency, 
    void* (*agent_thread)(stationary_agent_info_t*), 
    const char* name, 
    void* agent_args)
{
#ifndef _WIN32
	pthread_attr_t attr;
	pthread_attr_init(&attr);
#else
	int stack_size = 0;
#endif
	stationary_agent_info_t* stationary_agent_info;
	stationary_agent_info = (stationary_agent_info_t*)malloc(sizeof(stationary_agent_info_t));
	stationary_agent_info->args = agent_args;
	stationary_agent_info->agent = agent_NewBinary(agency->mc_platform);
	stationary_agent_info->agent->name = strdup(name);
	stationary_agent_info->attr = agency;
	stationary_agent_info->agency = agency;
	agent_queue_Add(agency->mc_platform->agent_queue, stationary_agent_info->agent);
#ifndef _WIN32
	THREAD_CREATE(&stationary_agent_info->thread, agent_thread, stationary_agent_info);
#else
	THREAD_CREATE(&stationary_agent_info->thread, (LPTHREAD_START_ROUTINE)agent_thread, stationary_agent_info);
#endif
  return 0;
}

EXPORTMC MCAgency_t MC_AgentInfo_GetAgency(stationary_agent_info_t* stationary_agent_info)
{
  return stationary_agent_info->agency;
}

EXPORTMC MCAgent_t MC_AgentInfo_GetAgent(stationary_agent_info_t* stationary_agent_info)
{
  return stationary_agent_info->agent;
}

EXPORTMC void* MC_AgentInfo_GetAgentArgs(stationary_agent_info_t* stationary_agent_info)
{
  return stationary_agent_info->args;
}

EXPORTMC int MC_AgentAddTask(
    MCAgent_t agent, 
    const char* code, 
    const char* return_var_name, 
    const char* server,
    int persistent)
{
  int new_num_tasks;
  int num_code_ids;
  int i, j;
  agent->datastate->number_of_tasks++;
  new_num_tasks = agent->datastate->number_of_tasks;

  /* Allocate new code id */
  /* First, we need to find the number of agent_code_ids */
  for(num_code_ids = 0; 
      agent->datastate->agent_code_ids[num_code_ids] != NULL; 
      num_code_ids++);

  agent->datastate->agent_code_ids = (char**)realloc(
      agent->datastate->agent_code_ids,
      sizeof(char*)*(num_code_ids + 2));
  agent->datastate->agent_code_ids[num_code_ids] = NULL;
  agent->datastate->agent_code_ids[num_code_ids+1] = NULL;
  if(agent->datastate->agent_code_ids == NULL) {
    fprintf(stderr, "Memory Error %s:%d\n", __FILE__, __LINE__);
  }

  /* If there is an agent code without an ID, then we must find all tasks
   * without an agent code and assign the un-id'd code to those tasks. */
  for(i = 0; agent->datastate->agent_code_ids[i] != NULL; i++) {
    if(strlen(agent->datastate->agent_code_ids[i]) == 0) {
      free(agent->datastate->agent_code_ids[i]);
      agent->datastate->agent_code_ids[i] = (char*)malloc(sizeof(char)*20);
      sprintf(agent->datastate->agent_code_ids[i], "code%d", rand());
      for(j = 0; j < agent->datastate->number_of_tasks-1; j++) {
        if (agent->datastate->tasks[j]->code_id == NULL) {
          agent->datastate->tasks[j]->code_id = strdup(agent->datastate->agent_code_ids[i]);
        }
      }
      break;
    }
  }

  /* Make a new code id for the new task */
  agent->datastate->agent_code_ids[num_code_ids] = (char*)malloc(sizeof(char)*20);
  sprintf(agent->datastate->agent_code_ids[num_code_ids], "code%d", rand());

  /* Allocate new agent_code */
  agent->datastate->agent_codes = (char**)realloc(
      agent->datastate->agent_codes,
      sizeof(char*)*(num_code_ids + 2));
  agent->datastate->agent_codes[num_code_ids] = NULL;
  agent->datastate->agent_codes[num_code_ids+1] = NULL;
  agent->datastate->agent_codes[num_code_ids] = strdup(code);

  /* Allocate new task */
  agent->datastate->tasks = (agent_task_t**)realloc(
      agent->datastate->tasks,
      sizeof(agent_task_t*) * new_num_tasks );
  agent->datastate->tasks[new_num_tasks-1] = agent_task_New();
  if(return_var_name == NULL) {
    agent->datastate->tasks[new_num_tasks-1]->var_name = strdup("no-return");
  } else {
    agent->datastate->tasks[new_num_tasks-1]->var_name = strdup(return_var_name);
  }
  agent->datastate->tasks[new_num_tasks-1]->server_name = strdup(server);
  agent->datastate->tasks[new_num_tasks-1]->persistent = persistent;
  agent->datastate->tasks[new_num_tasks-1]->code_id = 
      strdup(agent->datastate->agent_code_ids[num_code_ids]);
  return 0;
}

EXPORTMC int MC_AgentAddTaskFromFile(
    MCAgent_t agent, 
    const char* filename, 
    const char* return_var_name, 
    const char* server,
    int persistent)
{
  struct stat buffer;
  char *code;
  int codesize;
  FILE* fptr;
  int return_val;
  if(stat(filename, &buffer)) {
    /* Stat failed. Maybe file does not exist? */
    fprintf(stderr, "MC_AgentAddTaskFromFile failed. File %s does not exist.\n",
        filename);
    return -1;
  }
  codesize = buffer.st_size;
  code = (char *)malloc((codesize+1)*sizeof(char));
  memset(code, 0, codesize+1);
  fptr = fopen(filename, "r");
  fread(code, sizeof(char), codesize, fptr);
  fclose(fptr);
  return_val = MC_AgentAddTask(
      agent,
      code,
      return_var_name,
      server,
      persistent );
  free(code);
  return return_val;
}

EXPORTMC int MC_AgentReturnArrayExtent(
    MCAgent_t agent,
    int task_num,
    int index )
{
  if (task_num >= agent->datastate->number_of_tasks) {
    return -1;
  }

  if (
      agent->datastate->tasks[task_num]->
      agent_return_data->data_type == -1
     )
  {
    return -1;
  }

  if (index >= agent->datastate->tasks[task_num]->agent_return_data->array_dim) {
    return -1;
  }

  return agent->datastate->tasks[task_num]->agent_return_data->array_extent[index];
}

EXPORTMC int MC_AgentReturnArrayDim(
    MCAgent_t agent,
    int task_num )
{
  if (task_num >= agent->datastate->number_of_tasks) {
    return -1;
  }

  if (
      agent->datastate->tasks[task_num]->
      agent_return_data->data_type == -1
     )
  {
    return -1;
  }
  return agent->datastate->tasks[task_num]->agent_return_data->array_dim;
}

EXPORTMC int MC_AgentReturnArrayNum(
    MCAgent_t agent,
    int task_num )
{
  int num_elements, i;
  if (task_num >= agent->datastate->number_of_tasks) {
    return -1;
  }

  if (
      agent->datastate->tasks[task_num]->
      agent_return_data->data_type == -1
     )
  {
    return -1;
  }

  num_elements = 1;
  for (
      i = 0; 
      i < agent->datastate->tasks[task_num]->agent_return_data->array_dim;
      i++
      )
  {
    num_elements *= agent->datastate->
      tasks[task_num]->agent_return_data->array_extent[i];
  }
  return num_elements;
}

EXPORTMC const void* MC_AgentReturnDataGetSymbolAddr(
    MCAgent_t agent,
    int task_num )
{
  if (task_num >= agent->datastate->number_of_tasks) {
    return NULL;
  }
  if (
      agent->datastate->tasks[task_num]->
      agent_return_data->data_type == -1
     )
  {
    return NULL;
  }
  return agent->datastate->tasks[task_num]->agent_return_data->data;
}

EXPORTMC size_t MC_AgentReturnDataSize(
    MCAgent_t agent,
    int task_num )
{
  int size;
  if (task_num >= agent->datastate->number_of_tasks) {
    return -1;
  }

  if (
      agent->datastate->tasks[task_num]->
      agent_return_data->data_type == -1
     )
  {
    return -1;
  }

  CH_DATATYPE_SIZE(
      agent->datastate->tasks[task_num]->agent_return_data->data_type,
      size);
  return size;
}

EXPORTMC int MC_AgentReturnDataType(
    MCAgent_t agent,
    int task_num )
{
  if (task_num >= agent->datastate->number_of_tasks) {
    return -1;
  }
  return agent->datastate->tasks[task_num]->agent_return_data->data_type;
}

EXPORTMC int MC_AgentReturnIsArray(
    MCAgent_t agent,
    int task_num )
{
  if (task_num >= agent->datastate->number_of_tasks) {
    return -1;
  }
  if (
      agent->datastate->tasks[task_num]->
      agent_return_data->data_type == -1
     )
  {
    return -1;
  }
  if(agent->datastate->tasks[task_num]->agent_return_data->array_dim) {
    return 1;
  } else {
    return 0;
  }
}

EXPORTMC const void* MC_AgentVariableRetrieve(MCAgent_t agent, const char* var_name, int task_num)
{
  interpreter_variable_data_t* interp_var;

  if (task_num > agent->datastate->task_progress) {
    return NULL;
  }

  interp_var = agent_variable_list_Search(
      agent->datastate->tasks[task_num]->agent_variable_list,
      var_name );
  if (interp_var == NULL) {
    return NULL;
  }

  return interp_var->data;
}

EXPORTMC int MC_AgentVariableRetrieveInfo(MCAgent_t agent, const char* var_name, int task_num, const void** data, int* dim, const int** extent)
{
  interpreter_variable_data_t* interp_var;

  if (task_num >= agent->datastate->task_progress) {
    return MC_ERR_NOT_FOUND;
  }

  interp_var = agent_variable_list_Search(
      agent->datastate->tasks[task_num]->agent_variable_list,
      var_name );
  if (interp_var == NULL) {
    return MC_ERR_NOT_FOUND;
  }
  *data = interp_var->data;
  *dim = interp_var->array_dim;
  *extent = interp_var->array_extent;

  return MC_SUCCESS;
}

EXPORTMC int MC_AgentVariableSave(MCAgent_t agent, const char* var_name)
{
  int current_task = agent->datastate->task_progress;
  const int default_num_vars = 50;
  agent_task_p task = agent->datastate->tasks[current_task];

  if(task->num_saved_variables == 0) {
    task->saved_variables = (char**)malloc(sizeof(char*)*default_num_vars);
    memset(task->saved_variables, 0, sizeof(char*)*default_num_vars);
  }
  task->saved_variables[task->num_saved_variables] = strdup(var_name);
  if(task->saved_variables[task->num_saved_variables] == NULL) {
    fprintf(stderr, "Memory error. %s:%d\n", __FILE__, __LINE__);
    return MC_ERR_MEMORY;
  }
  task->num_saved_variables++;

  return 0;
}

EXPORTMC int 
MC_Barrier(MCAgency_t attr, int id) /*{{{*/
{
    barrier_queue_p list = attr->mc_platform->barrier_queue;
    barrier_node_p node;
    node = barrier_queue_Get(list, id);
    if(node == NULL) {
        return MC_ERR_NOT_FOUND;
    }

    MUTEX_LOCK(node->lock);
    node->num_waiting++;
    if (node->num_waiting >= node->num_registered) {
        /* Wake all agents/threads waiting on this barrier */
        COND_BROADCAST(node->cond);
        MUTEX_UNLOCK(node->lock);
        return MC_SUCCESS;
    } else {
        while (node->num_waiting < node->num_registered) {
            COND_WAIT(node->cond, node->lock);
        }
        MUTEX_UNLOCK(node->lock);
    }
    return MC_SUCCESS;
} /*}}}*/

EXPORTMC int
MC_BarrierInit(MCAgency_t attr, int id, int num_procs) /*{{{*/
{
    barrier_node_p node;
    /* First see if there already exists a barrier of the same ID. */
    node = barrier_queue_Get(attr->mc_platform->barrier_queue, id);
    if (node != NULL) {
      return MC_ERR;
    }
    node = barrier_node_Initialize(id, num_procs);
    barrier_queue_Add(attr->mc_platform->barrier_queue, node);
    return MC_SUCCESS;
} /*}}}*/

EXPORTMC int 
MC_BarrierDelete(MCAgency_t attr, int id) /*{{{*/
{
  return barrier_queue_Delete(id, attr->mc_platform->barrier_queue);
} /*}}}*/

EXPORTMC int
MC_CallAgentFunc(
    MCAgent_t agent,
    const char* funcName,
    void* returnVal,
    int numArgs,
    ...)
{
  int return_code;
  va_list vl;
  va_start(vl, numArgs);
  MUTEX_LOCK(agent->run_lock);
  return_code = Ch_CallFuncByNamev(
      *agent->agent_interp,
      funcName,
      returnVal,
      vl );
  MUTEX_UNLOCK(agent->run_lock);
  return return_code;
}

EXPORTMC int 
MC_CallAgentFuncArg( /*{{{*/
        MCAgent_t agent,
        const char* funcName,
        void* returnVal, /* FIXME: Should this be a double pointer? */
        void* arg)
{
    int return_code;

    MUTEX_LOCK(agent->run_lock); 
    return_code = Ch_CallFuncByName(
            agent->agent_interp,
            funcName,
            returnVal,
            arg);
    MUTEX_UNLOCK(agent->run_lock); 
    return return_code;
} /*}}}*/ 

EXPORTMC int
MC_CallAgentFuncV(
    MCAgent_t agent,
    const char* funcName,
    void* returnVal,
    va_list ap)
{
  int return_code;
  MUTEX_LOCK(agent->run_lock);
  return_code = Ch_CallFuncByNamev
    (
     agent->agent_interp,
     funcName,
     returnVal,
     ap
    );
  MUTEX_UNLOCK(agent->run_lock);
  return return_code;
}

EXPORTMC int
MC_CallAgentFuncVar(
		    MCAgent_t agent,
		    const char* funcName,
		    void* returnVal,
		    ChVaList_t varg)
{
  int return_code;
  MUTEX_LOCK(agent->run_lock);
  return_code = Ch_CallFuncByNameVar
    (
     *agent->agent_interp,
     funcName,
     returnVal,
     varg
     );
  MUTEX_UNLOCK(agent->run_lock);
  return return_code;
}

#ifdef OBS
EXPORTMC int 
MC_ChInitializeOptions(MCAgency_t attr, ChOptions_t *options) { /*{{{*/
    if(attr->mc_platform == NULL) {
        fprintf(stderr, "MC_ChInitializeOptions must be called after MC_Start()\n");
        fprintf(stderr, "Using default interpretor options...\n");
        return 1;
    }
    else {
        if (attr->mc_platform->interp_options == NULL) {
          attr->mc_platform->interp_options = (ChOptions_t*)malloc(
              sizeof(ChOptions_t) );
        }
        *attr->mc_platform->interp_options = *options;
        attr->mc_platform->interp_options->chhome = strdup(options->chhome);
        return 0;
    }
} /*}}}*/
#endif

EXPORTMC MCAgent_t
MC_ComposeAgent(
    const char* name,
    const char* home,
    const char* owner, 
    const char* code,
    const char* return_var_name,
    const char* server,
    int persistent
    )
{
  return  MC_ComposeAgentWithWorkgroup(
      name,
      home,
      owner,
      code,
      return_var_name,
      server,
      persistent,
      NULL
      );
}

EXPORTMC MCAgent_t
MC_ComposeAgentS(
    const char* name,
    const char* home,
    const char* owner, 
    const char* code,
    const char* return_var_name,
    const char* server,
    int persistent,
		const char* workgroup_code
    )
{
  printf("\nThe MC_ComposeAgentS function is now deprecated due to its\n\
  misleading name. Please use the MC_ComposeAgentWithWorkgroup() function\n\
  instead.\n");
  return MC_ComposeAgentWithWorkgroup(
      name,
      home,
      owner,
      code,
      return_var_name,
      server,
      persistent,
      workgroup_code );
}

EXPORTMC MCAgent_t
MC_ComposeAgentWithWorkgroup(
    const char* name,
    const char* home,
    const char* owner, 
    const char* code,
    const char* return_var_name,
    const char* server,
    int persistent,
		const char* workgroup_code
    )
{
  agent_p agent;
  agent = agent_New();
  if (agent == NULL) return NULL;
  agent->name = strdup(name);
  agent->home = strdup(home);
  agent->owner = strdup(owner);

  agent->orphan = 1;

  agent->agent_type = MC_LOCAL_AGENT;
  agent->agent_status = MC_WAIT_MESSGSEND;

  agent->datastate = agent_datastate_New();
  agent->datastate->number_of_tasks = 1;
  agent->datastate->agent_code_ids = (char**)malloc(
      sizeof(char*)*2);
  if(agent->datastate->agent_code_ids == NULL) {
    fprintf(stderr, "Memory Error %s:%d\n", __FILE__, __LINE__);
  }
  agent->datastate->agent_code_ids[0] = strdup("");
  if(agent->datastate->agent_code_ids[0] == NULL) {
    fprintf(stderr, "Memory Error %s:%d\n", __FILE__, __LINE__);
  }
  agent->datastate->agent_code_ids[1] = NULL;

  agent->datastate->agent_codes = (char**)malloc(
      sizeof(char*)*2);
  if(agent->datastate->agent_codes == NULL) {
    fprintf(stderr, "Memory Error %s:%d\n", __FILE__, __LINE__);
  }
  agent->datastate->agent_codes[0] = strdup(code);
  if(agent->datastate->agent_codes[0] == NULL) {
    fprintf(stderr, "Memory Error %s:%d\n", __FILE__, __LINE__);
  }
  agent->datastate->agent_codes[1] = NULL;

  agent->datastate->agent_code = agent->datastate->agent_codes[0];

  agent->datastate->tasks = (agent_task_t**)malloc(
      sizeof(agent_task_t*));
  agent->datastate->tasks[0] = agent_task_New();
  if(return_var_name == NULL) {
    agent->datastate->tasks[0]->var_name = strdup("no-return");
  } else {
    agent->datastate->tasks[0]->var_name = strdup(return_var_name);
  }
  if(agent->datastate->tasks[0]->var_name == NULL) {
    fprintf(stderr, "Memory Error %s:%d\n", __FILE__, __LINE__);
  }

  agent->datastate->tasks[0]->server_name = strdup(server);
  if(agent->datastate->tasks[0]->server_name == NULL) {
    fprintf(stderr, "Memory Error %s:%d\n", __FILE__, __LINE__);
  }

	if(workgroup_code != NULL) {
		agent->wg_code = strdup(workgroup_code);
		if (agent->wg_code == NULL) {
			fprintf(stderr, "Memory Error %s:%d\n", __FILE__, __LINE__);
		}
	}
  agent->datastate->tasks[0]->persistent = persistent;
 
  return agent;
}

EXPORTMC MCAgent_t MC_ComposeAgentFromFile(
    const char* name,
    const char* home,
    const char* owner, 
    const char* filename,
    const char* return_var_name,
    const char* server,
    int persistent
    )
{
  return MC_ComposeAgentFromFileWithWorkgroup(
      name,
      home,
      owner,
      filename,
      return_var_name,
      server,
      persistent,
      NULL
      );
}

EXPORTMC MCAgent_t MC_ComposeAgentFromFileS(
    const char* name,
    const char* home,
    const char* owner, 
    const char* filename,
    const char* return_var_name,
    const char* server,
    int persistent,
		const char* workgroup_code
    )
{
  printf("\nThe MC_ComposeAgentFromFileS() function is deprecated due to \n\
  its misleading name. Please use the MC_ComposeAgentFromFileWithWorkgroup\n\
  function instead.\n");
  return MC_ComposeAgentFromFileWithWorkgroup(
      name,
      home,
      owner,
      filename,
      return_var_name,
      server,
      persistent,
      workgroup_code);
}

EXPORTMC MCAgent_t MC_ComposeAgentFromFileWithWorkgroup(
    const char* name,
    const char* home,
    const char* owner, 
    const char* filename,
    const char* return_var_name,
    const char* server,
    int persistent,
		const char* workgroup_code
    )
{
  MCAgent_t agent;
  struct stat buffer;
  char *code;
  int codesize;
  FILE* fptr;

  if(stat(filename, &buffer)) {
    /* Stat failed. Maybe file does not exist? */
    return NULL;
  }
  codesize = buffer.st_size;
  code = (char *)malloc((codesize+1)*sizeof(char));
  memset(code, 0, codesize+1);
  fptr = fopen(filename, "r");
  fread(code, sizeof(char), codesize, fptr);
  fclose(fptr);

  agent = MC_ComposeAgentWithWorkgroup(
     name, 
     home, 
     owner, 
     code, 
     return_var_name, 
     server,
     persistent,
		 workgroup_code
     );
  free(code);

  return agent;
}

EXPORTMC int 
MC_CondBroadcast(MCAgency_t attr, int id) /*{{{*/
{
    syncListNode_t *condnode;
    condnode = syncListFind(id, attr->mc_platform->syncList);
    if (condnode == NULL) {
        return MC_ERR_NOT_FOUND;
    }
    MUTEX_LOCK(condnode->lock);
    condnode->signalled=1;
    COND_BROADCAST(condnode->cond);
    MUTEX_UNLOCK(condnode->lock);
    return 0;
} /*}}}*/

EXPORTMC int 
MC_CondSignal(MCAgency_t attr, int id) /*{{{*/
{
    syncListNode_t *condnode;
    condnode = syncListFind(id, attr->mc_platform->syncList);
    if (condnode == NULL) {
        return MC_ERR_NOT_FOUND;
    }
    MUTEX_LOCK(condnode->lock);
    condnode->signalled=1;
    COND_SIGNAL(condnode->cond);
    MUTEX_UNLOCK(condnode->lock);
    return 0;
} /*}}}*/

EXPORTMC int 
MC_CondWait(MCAgency_t attr, int id)  /*{{{*/
{
    syncListNode_t *condnode;
    condnode = syncListFind(id, attr->mc_platform->syncList);
    if (condnode == NULL) {
        return MC_ERR_NOT_FOUND;
    }
    MUTEX_LOCK(condnode->lock);
    if (condnode->signalled) {
        MUTEX_UNLOCK(condnode->lock);
        return 1;
    }

    while (condnode->signalled == 0) {
      COND_WAIT(condnode->cond, condnode->lock);
    }
    MUTEX_UNLOCK(condnode->lock);

    return 0;
} /*}}}*/

EXPORTMC int 
MC_CondReset(MCAgency_t attr, int id) /*{{{*/
{
    syncListNode_t *condnode;
    condnode = syncListFind(id, attr->mc_platform->syncList);
    if (condnode == NULL) {
        return MC_ERR_NOT_FOUND;
    }
    MUTEX_LOCK(condnode->lock);
    if (condnode->signalled) {
        condnode->signalled = 0;
        MUTEX_UNLOCK(condnode->lock);
        return 0;
    }
    MUTEX_UNLOCK(condnode->lock);
    return 1;
} /*}}}*/

EXPORTMC int 
MC_CopyAgent(MCAgent_t* agent_out, const MCAgent_t agent_in) /*{{{*/
{
  *agent_out = agent_Copy(agent_in);
  return MC_SUCCESS;
} /*}}}*/

EXPORTMC int 
MC_DeleteAgent(MCAgent_t agent) /*{{{*/
{
    /* Error Checking */
    CHECK_NULL(agent, return MC_ERR_INVALID;);

    /* If the agent has a workgroup code, then the user must use MC_DeleteAgentWG() */
    if ( agent->wg_code != NULL ) {
      return MC_ERR_INVALID_ARGS;
    }

    /* First, make sure the agent is no longer running */
    MC_TerminateAgent(agent);

    return MC_SUCCESS;
} /*}}}*/

EXPORTMC int 
MC_DeleteAgentWG(MCAgent_t calling_agent, MCAgent_t agent) /*{{{*/
{
    /* Error Checking */
    CHECK_NULL(agent, return MC_ERR_INVALID;);

    /* The agents must belong to the same workgroup*/
    if (agent->wg_code != NULL) {
      if (calling_agent->wg_code == NULL) {
        return MC_ERR_INVALID_ARGS;
      } else if (strcmp(calling_agent->wg_code, agent->wg_code)) {
        return MC_ERR_INVALID_ARGS;
      }
    }

    /* First, make sure the agent is no longer running */
    MC_TerminateAgentWG(calling_agent, agent);

    return MC_SUCCESS;
} /*}}}*/

EXPORTMC int MC_DestroyServiceSearchResult(
    char** agentName,
    char** serviceName,
    int* agentID,
    int numResult)
{
  int i;
  for(i = 0;i < numResult; i++)
  {
    free(agentName[i]);
    free(serviceName[i]);
  }
  free(agentName);
  free(serviceName);
  free(agentID);

  return 0;
}

EXPORTMC int 
MC_DeregisterService( /*{{{*/
        MCAgency_t agency,
        int agentID,
        const char *serviceName)
{
    int err_code;

    df_request_list_node_t *req_node;
    df_deregister_p deregister_data;

    req_node = df_request_list_node_New();
    req_node->command = (char*)malloc(sizeof(char)*11);

    strcpy((char*)req_node->command, "deregister");

    deregister_data = (df_deregister_p)malloc(sizeof(df_deregister_t));
    deregister_data->agent_id = agentID;
    deregister_data->service_name = (char*)serviceName;

    req_node->data = deregister_data;

    err_code = df_AddRequest(
            agency->mc_platform->df,
            req_node
            );
    return err_code;
} /*}}}*/

EXPORTMC int
MC_End(MCAgency_t agency) /*{{{*/
{
#ifndef _WIN32
  int skt;
  struct addrinfo hints, *res;
  char buf[20];
#else
  SOCKET skt;
  SOCKADDR_IN sktin;
  struct hostent host;
  struct hostent* host_result;
#endif

  /* Now, we must stop all the running pthreads somehow... */
  /* We will set the quit flag and signal some important threads to make
   * sure they exit cleanly. We want the df, ams, and especially the acc
   * to be done with whatever they are doing before exiting. */
  MUTEX_LOCK(agency->mc_platform->quit_lock);
  agency->mc_platform->quit = 1;
  COND_BROADCAST( agency->mc_platform->quit_cond );
  MUTEX_UNLOCK(agency->mc_platform->quit_lock);
  /* We need to send a ghost message to the local listening port to get out of
   * the "accept()" function. */
#ifndef _WIN32
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  sprintf(buf, "%d", agency->portno);
  getaddrinfo("localhost", buf, &hints, &res);
  skt = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  connect(skt, res->ai_addr, res->ai_addrlen);
  send(skt, "\0", 1, 0);
  close(skt);

  sleep(1);
#else
  sktin.sin_family = PF_INET;
  sktin.sin_port = htons(agency->portno);
  host_result = gethostbyname("localhost");
  host = *host_result;
  memcpy(&sktin.sin_addr, host.h_addr, host.h_length);
  sktin.sin_addr.s_addr = inet_addr("localhost");
  connect(skt, (struct sockaddr *) &sktin, sizeof(sktin));
  send(skt, "\0", 1, 0);
  closesocket(skt);

  Sleep(1000);
#endif

  /* Stop the listen thread */
  if( GET_THREAD_MODE( agency->threads, MC_THREAD_ACC)) {
    THREAD_CANCEL( agency->mc_platform->acc->listen_thread );
  } 

  /* Stop the connection queue */
  MUTEX_LOCK(agency->mc_platform->connection_queue->lock);
  COND_SIGNAL(agency->mc_platform->connection_queue->cond);
  MUTEX_UNLOCK(agency->mc_platform->connection_queue->lock);
  if( GET_THREAD_MODE( agency->threads, MC_THREAD_ACC)) {
    THREAD_JOIN(agency->mc_platform->acc->thread);
  }

  /* Stop the AMS */
  MUTEX_LOCK(agency->mc_platform->ams->runflag_lock);
  COND_SIGNAL(agency->mc_platform->ams->runflag_cond);
  MUTEX_UNLOCK(agency->mc_platform->ams->runflag_lock);
  if( GET_THREAD_MODE( agency->threads, MC_THREAD_AMS)) {
    THREAD_JOIN(agency->mc_platform->ams->thread);
  }

  /* Stop the message queue */
  MUTEX_LOCK(agency->mc_platform->message_queue->lock);
  COND_SIGNAL(agency->mc_platform->message_queue->cond);
  MUTEX_UNLOCK(agency->mc_platform->message_queue->lock);
  if( GET_THREAD_MODE( agency->threads, MC_THREAD_ACC)) {
    THREAD_JOIN(agency->mc_platform->acc->message_handler_thread);
  }

  /* Stop the DF */
  MUTEX_LOCK(agency->mc_platform->df->request_list->lock);
  COND_SIGNAL(agency->mc_platform->df->request_list->cond);
  MUTEX_UNLOCK(agency->mc_platform->df->request_list->lock);
  if( GET_THREAD_MODE( agency->threads, MC_THREAD_DF)) {
    THREAD_JOIN(agency->mc_platform->df->thread);
  }

  /* Stop the command prompt */
  if( GET_THREAD_MODE( agency->threads, MC_THREAD_CP)) {
    THREAD_CANCEL( agency->mc_platform->cmd_prompt->thread );
  }

  mc_platform_Destroy(agency->mc_platform);

  if (agency->hostName)
    free(agency->hostName); 
  free(agency);

  return 0;
} /*}}}*/

EXPORTMC MCAgent_t 
MC_FindAgentByName( MCAgency_t attr, /*{{{*/
    const char *name) 
{
  extern mc_platform_p g_mc_platform;
  if (attr == NULL) {
    return agent_queue_SearchName(g_mc_platform->agent_queue, name);
  } else {
    return agent_queue_SearchName(attr->mc_platform->agent_queue,
        name);
  }
} /*}}}*/

EXPORTMC MCAgent_t
MC_FindAgentByID( MCAgency_t attr, /*{{{*/
    int ID)
{
  extern mc_platform_p g_mc_platform;
  if (attr == NULL) {
    return agent_queue_Search(g_mc_platform->agent_queue, ID);
  } else {
    return agent_queue_Search(attr->mc_platform->agent_queue,
        ID);
  }
} /*}}}*/

EXPORTMC 
#ifndef _WIN32
  time_t 
#else
  SYSTEMTIME
#endif
MC_GetAgentArrivalTime(MCAgent_t agent) /*{{{*/
{
  if (agent != NULL) {
    return agent->arrival_time;
  } else {
#ifndef _WIN32
    return (time_t)-1;
#else
    SYSTEMTIME oy;
    return oy;
#endif
  }
} /*}}}*/

EXPORTMC int 
MC_GetAgentStatus(MCAgent_t agent) /*{{{*/
{
  int status;
  MUTEX_LOCK(agent->lock);
  status = agent->agent_status;
  MUTEX_UNLOCK(agent->lock);
  return status;
} /*}}}*/

EXPORTMC char*
MC_GetAgentXMLString(MCAgent_t agent) /*{{{*/
{
  char *ret;
  ret = mxmlSaveAllocString(
      agent->datastate->xml_agent_root,
      NULL
      );
  return ret;
} /*}}}*/

/*ChInterp_t */
EXPORTMC void*
MC_GetAgentExecEngine(MCAgent_t agent) /*{{{*/
{
  return agent->agent_interp;
} /*}}}*/

EXPORTMC int 
MC_GetAgentID( /*{{{*/
    MCAgent_t agent
    )
{
  return agent->id;
} /*}}}*/

EXPORTMC char* 
MC_GetAgentName( /*{{{*/
    MCAgent_t agent
    )
{
  char *name;
  MUTEX_LOCK(agent->lock);
  name = (char*)malloc(sizeof(char) * 
      (strlen (agent->name) + 1)
      );
  strcpy(
      name,
      agent->name
      );
  MUTEX_UNLOCK(agent->lock);
  return name;
} /*}}}*/

EXPORTMC int
MC_GetAgentReturnData( /*{{{*/
    MCAgent_t agent,
    int task_num,
    void **data,
    int *dim,
    int **extent) 
{
  int num_elements;
  int size;
  int i;
  printf("\nThe function MC_GetAgentReturnData() is deprecated.\n\
  Please use the MC_AgentReturn*() series of functions instead.\n");
  if (task_num >= agent->datastate->number_of_tasks) {
    *data = NULL;
    *dim = 0;
    *extent = NULL;
    return 1;
  }
  if (
      agent->datastate->tasks[task_num]->
      agent_return_data->data_type == -1
     )
  {
    return 1;
  }
  CH_DATATYPE_SIZE(
      agent->datastate->tasks[task_num]->agent_return_data->data_type,
      size);
  num_elements = 1;
  for (
      i = 0; 
      i < agent->datastate->tasks[task_num]->agent_return_data->array_dim;
      i++
      )
  {
    num_elements *= agent->datastate->
      tasks[task_num]->agent_return_data->array_extent[i];
  }


  *data = malloc(num_elements * size);
  memcpy(
      *data,
      agent->datastate->tasks[task_num]->
      agent_return_data->data,
      size * num_elements
      );
  *dim = agent->datastate->tasks[task_num]->agent_return_data->array_dim;
  *extent = (int*)malloc(
      sizeof(int) * 
      agent->datastate->tasks[task_num]->agent_return_data->array_dim
      );
  for (i = 0; i < *dim; i++) {
    (*extent)[i] = 
      agent->datastate->tasks[task_num]->agent_return_data->array_extent[i];
  }
  /*    memcpy(
   *extent,
   agent->datastate->tasks[0]->agent_return_data->array_extent,
   *dim
   ); */
  return 0;
} /*}}}*/

EXPORTMC int 
MC_GetAgentNumTasks(MCAgent_t agent) /*{{{*/
{
  return agent->datastate->number_of_tasks;
} /*}}}*/

EXPORTMC enum MC_AgentType_e
MC_GetAgentType(MCAgent_t agent) /*{{{*/
{
  if (agent != NULL) {
    return agent->agent_type;
  } else {
    return (enum MC_AgentType_e)0;
  }
} /*}}}*/

EXPORTMC int 
MC_GetAllAgents(MCAgency_t attr, MCAgent_t **agents, int* num_agents) /*{{{*/
{
  int halt;
  int index = 0;
  MUTEX_LOCK(attr->mc_platform->giant_lock);
  halt = (attr->mc_platform->giant == 1) ? 1 : 0;
  MUTEX_UNLOCK(attr->mc_platform->giant_lock);
  if (halt)
    MC_HaltAgency(attr);
  /* Count the number of agents */
  while (agent_queue_SearchIndex(attr->mc_platform->agent_queue, index) != NULL) {
    index++;
  }

  if (index == 0) {
    *agents = NULL;
    *num_agents = 0;
    return -1;
  }

  *agents = (MCAgent_t *)malloc(sizeof(MCAgent_t*) * index);
  *num_agents = index;
  /* Assign the agents */
  for(index = 0; index < *num_agents; index++) {
    (*agents)[index] = agent_queue_SearchIndex
      (
       attr->mc_platform->agent_queue,
       index
      );
  }
  if(halt)
    MC_ResumeAgency(attr);
  return 0;
} /*}}}*/

EXPORTMC int
MC_HaltAgency(MCAgency_t attr) /*{{{*/
{
  MUTEX_LOCK(attr->mc_platform->giant_lock);
  attr->mc_platform->giant=0;
  MUTEX_UNLOCK(attr->mc_platform->giant_lock);
  return 0;
} /*}}}*/

EXPORTMC MCAgency_t 
MC_Initialize( /*{{{*/
    int port,
    MCAgencyOptions_t *options)
{
  MCAgency_t ret;
  int i=0;
  int options_malloc = 0;
  char privkey[1210];   // private key after decryption
#ifdef NEW_SECURITY
  char buf[5];
  FILE* f;
#endif

  memset(privkey, '\0', 1210);
  ret = (MCAgency_t)malloc(sizeof(struct agency_s));
  if (ret == NULL) {return NULL;}

  ret->hostName = (char*)malloc(HOST_NAME_MAX);
  if (ret->hostName == NULL) {return NULL;}
  gethostname(ret->hostName, HOST_NAME_MAX);
  /*Save some memory */
  ret->hostName = (char*)realloc(ret->hostName, sizeof(char)*(strlen(ret->hostName)+1));
  if (ret->hostName == NULL) {return NULL;}
  ret->portno = port;
  ret->server = 1;
  ret->client = 0;
  ret->default_agentstatus = -1;

  /* Set up agency options */
  if(options==NULL) {
    options = (MCAgencyOptions_t*)malloc(sizeof(MCAgencyOptions_t));
    MC_InitializeAgencyOptions(options);
    options_malloc = 1;
  }
  ret->threads = options->threads;
  ret->default_agentstatus = options->default_agent_status;
  for(i = 0; i < MC_THREAD_ALL; i++) {
    ret->stack_size[i] = options->stack_size[i];
  }
  ret->initInterps = options->initInterps;
  /* End agency options */

  ret->bluetooth = options->bluetooth;
  ret->mc_platform = mc_platform_Initialize(ret, options->ch_options);
  //ret->mc_platform->interp_options = options->ch_options;

#ifdef NEW_SECURITY
	if (options->priv_key_filename != NULL) {
		ret->priv_key_filename = strdup(options->priv_key_filename);
	} else {
		ret->priv_key_filename = strdup("rsa_priv");
	}
	if (options->known_host_filename != NULL) {
		ret->known_host_filename = strdup(options->known_host_filename);
	} else {
		ret->known_host_filename = strdup("known_host");
	}
  /* check whether private key file is encryt or not */
 memset(buf,'\0', 4);

  if( (f = fopen(ret->priv_key_filename, "r") ) == NULL) {
    fprintf(stderr, "rsa_priv_en file not found \n");
		fprintf(stderr, "Aborting... at %s:%d\n", __FILE__, __LINE__);
		exit(0);
	}

  fread (buf, 1, 4, f);

  if(buf[0]=='N' && buf[1] == ' ' && buf[2]=='=' && buf[3]==' '){
    fread(privkey, 1210, 1, f);
    fclose(f);
  }
  else{   
    fclose(f);
    /* prompt for passphrase if its NULL */ 
    if(options->passphrase[0] == '\0'){
      printf("Please enter the passphrase to decrypt private key  ");
      scanf("%s", options->passphrase); 
    }  
    /* decrypting Private key */  
    if( read_encrypted_file(ret->priv_key_filename, privkey, options->passphrase) == -1){
      printf("Unable to read private key: passphase is wrong or file not exist\n");
      printf("Press any key to exit \n");
      getchar();
      exit(0); 
    }
  }

  if (privkey == NULL){
     printf("Note: private key is Null \n");	
	 printf("please verify the private key file or generate it again. \n");
	 printf("Exiting Mobile-C Agency ... \n");
	 getchar();
	 exit(0);
  }
  memset(ret -> mc_platform -> private_key, '\0', 1210);
  strcpy(ret -> mc_platform -> private_key, privkey);
//  printf("My private key length =%d \n%s.\n",strlen(privkey), privkey);
#endif /* NEW_SECURITY */

  /* Set up the global platform */
  g_mc_platform = ret->mc_platform;

  if (options_malloc)
    free(options);

  return ret;
} /*}}}*/

EXPORTMC int 
MC_InitializeAgencyOptions(struct MCAgencyOptions_s* options) /*{{{*/
{
  int i;
  memset(options, 0, sizeof(struct MCAgencyOptions_s));
  /* Set the default options */
  options->threads = 0xFFFF;
  options->default_agent_status = MC_WAIT_CH;
  options->modified = 0;
  for(i = 0; i < MC_THREAD_ALL; i++) {
    options->stack_size[i] = -1;
  }
  memset(options->passphrase, '\0', 32);

	options->known_host_filename = NULL;
	options->priv_key_filename = NULL;
  options->bluetooth = 0;

  options->initInterps = 4; // 4 interpreters are loaded
  return 0;
} /*}}}*/

EXPORTMC int
MC_LoadAgentFromFile(MCAgency_t attr, const char* filename)
{
  struct stat filestat;
  char *buf;
  FILE *fp;
  message_p message;
  extern mc_platform_p g_mc_platform;
  buf = NULL;
  filestat.st_size = 0;
  stat(filename, &filestat);
  if (filestat.st_size != 0 ) {
    buf = (char*)malloc( sizeof(char) * (filestat.st_size+1) );
    memset(buf, 0, filestat.st_size+1);
  } else {
    fprintf(stderr, "Error: File %s not found.\n", filename);
    return 1;
  }

  fp = fopen(filename, "r");
  fread((void*)buf, filestat.st_size, 1, fp);
  fclose(fp);

  message = message_New();
  if ( 
      message_InitializeFromString (
      attr->mc_platform,
      message,
      buf,
      "",
      5050,
      "ams"
      )
     )
  {
    message_Destroy(message);
  }
  free(message->to_address);
  message->to_address = NULL;
  message->xml_root = mxmlLoadString(
      NULL,
      buf,
      NULL );
  if (message->xml_root == NULL) {
    fprintf(stderr, "Error loading agent. %s:%d\n", __FILE__, __LINE__);
    message_Destroy(message);
    return 1;
  }
  message->xml_payload = mxmlFindElement(
      message->xml_root,
      message->xml_root,
      "MOBILE_AGENT",
      NULL,
      NULL,
      MXML_DESCEND );
  if(message->xml_payload == NULL) {
    fprintf(stderr, "Error loading agent: <MOBILE_AGENT> tag not found. %s:%d\n",
        __FILE__, __LINE__ );
    message_Destroy(message);
    return 1;
  }

  message_queue_Add
    (
     attr->mc_platform->message_queue,
     message
    );
  return 0;
}

EXPORTMC int
MC_MigrateAgent(MCAgent_t agent, const char* hostname, int port)
{
  /* Rewrite the current task to be on "hostname" and "port" and
   * resend the agent to its new destination */
  int progress;
  progress = agent->datastate->task_progress;
  
  /* Rewind the task progress */
  agent->datastate->progress_modifier = -1;

  /* Set the new servername */
  free(agent->datastate->tasks[progress]->server_name);
  agent->datastate->tasks[progress]->server_name = (char*)malloc(
      sizeof(char)*(strlen(hostname) + 10) );
  sprintf(agent->datastate->tasks[progress]->server_name, "%s:%d", 
      hostname, port);

  /* That is all. */
  return 0;
}

EXPORTMC int 
MC_MutexLock(MCAgency_t attr, int id) /*{{{*/
{
  syncListNode_t *syncnode;
  syncnode = syncListFind(id, attr->mc_platform->syncList);
  if (syncnode == NULL) {
    return 1;
  }
  MUTEX_LOCK(syncnode->lock);
  return 0;
} /*}}}*/

EXPORTMC int 
MC_MutexUnlock(MCAgency_t attr, int id) /*{{{*/
{
  syncListNode_t *syncnode;
  syncnode = syncListFind(id, attr->mc_platform->syncList);
  if (syncnode == NULL) {
    return 1;
  }
  MUTEX_UNLOCK(syncnode->lock);
  return 0;
} /*}}}*/

EXPORTMC int
MC_PrintAgentCode(MCAgent_t agent) /*{{{*/
{
  int progress;
  MUTEX_LOCK(agent->lock);
  progress = agent->datastate->task_progress;
  /* If progress is past the last task, print the last task's code. */
  if (progress >= agent->datastate->number_of_tasks) {
    progress = agent->datastate->number_of_tasks - 1;
  }
  printf("%s\n",
      agent->datastate->agent_code);
  MUTEX_UNLOCK(agent->lock);
  return 0;
} /*}}}*/

EXPORTMC int 
MC_RegisterService( /*{{{*/
    MCAgency_t agency,
    MCAgent_t agent,
    int agentID,
    const char *agentName,
    char **serviceNames,
    int numServices)
{
  df_request_list_node_t *req_node;
  df_node_t *new_node;
  int i;
  /* Error checking: Either an agent, or agent Name must be
   * provided. */
  if (agent == NULL && agentName == NULL) {
    return MC_ERR_INVALID_ARGS;
  }
  /* init the request node */
  req_node = df_request_list_node_New();
  req_node->command = (char*)malloc(sizeof(char)*9);
  strcpy((char*)req_node->command, "register");

  new_node = (df_node_t*)malloc(sizeof(df_node_t));
  CHECK_NULL(new_node, return MC_ERR_MEMORY);

  /* Init the lock */
  new_node->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  CHECK_NULL(new_node->lock, return MC_ERR_MEMORY);
  MUTEX_INIT(new_node->lock);

  /* Init the agentID */
  if (agent==NULL) {
    new_node->agent_id = agentID;
  } else {
    new_node->agent_id = agent->id;
  }

  /* Init the agent name */
  if (agent==NULL) {
    new_node->agent_name = 
      (char*)malloc(sizeof(char)*(strlen(agentName)+1));
    CHECK_NULL(new_node->agent_name, return MC_ERR_MEMORY;);
    strcpy(new_node->agent_name, agentName);
  } else {
    new_node->agent_name = 
      (char*)malloc(
          sizeof(char) * 
          (strlen(agent->name)+1)
          );
    CHECK_NULL(new_node->agent_name, return MC_ERR_MEMORY;);
    strcpy(new_node->agent_name, agent->name);
  }

  /* Init the services */
  new_node->service_names = (char**)malloc(
      sizeof(char*) * numServices
      );
  CHECK_NULL(new_node->service_names, return MC_ERR_MEMORY;);
  for (i = 0; i < numServices; i++) {
    new_node->service_names[i] = (char*) malloc(
        sizeof(char) * (strlen(serviceNames[i]) + 1)
        );
    CHECK_NULL(new_node->service_names[i], return MC_ERR_MEMORY;);
    strcpy(
        new_node->service_names[i],
        serviceNames[i]
        );
  }
  new_node->num_services = numServices;

  req_node->data = (void*)new_node;
  req_node->data_size = (sizeof(new_node));

  return df_AddRequest(
      agency->mc_platform->df,
      req_node
      );
} /*}}}*/

EXPORTMC int 
MC_ResumeAgency(MCAgency_t attr) /*{{{*/
{
  MUTEX_LOCK(attr->mc_platform->giant_lock);
  attr->mc_platform->giant = 1;
  MUTEX_UNLOCK(attr->mc_platform->giant_lock);
  return 0;
} /*}}}*/

EXPORTMC MCAgent_t
MC_RetrieveAgent(MCAgency_t attr) /*{{{*/
  /* This function retrieves the first agent with agent_status
     MC_AGENT_NEUTRAL it finds. If there are no agents with
     the specified attributes, return value is NULL. */
{
  int i;
  MCAgent_t agent=NULL;
  MUTEX_LOCK(attr->mc_platform->agent_queue->lock);
  for (i = 0; i < attr->mc_platform->agent_queue->size; i++) {
    agent = (MCAgent_t)ListSearch(
        attr->mc_platform->agent_queue->list, i);
    if (agent->agent_status == MC_AGENT_NEUTRAL) {
      break;
    }
  }
  if (agent == NULL) {
    MUTEX_UNLOCK(attr->mc_platform->agent_queue->lock);
    return NULL;
  }
  if (agent->agent_status != MC_AGENT_NEUTRAL) {
    MUTEX_UNLOCK(attr->mc_platform->agent_queue->lock);
    return NULL;
  }
  MUTEX_UNLOCK(attr->mc_platform->agent_queue->lock);
  return agent;
}/*}}}*/

EXPORTMC char * 
MC_RetrieveAgentCode(MCAgent_t agent) /*{{{*/
{
  char *buf;
  int len, progress;
  MUTEX_LOCK(agent->lock);
  progress = agent->datastate->task_progress;
  len = strlen(
      agent->datastate->agent_code);
  buf = (char*)malloc( (len+1)*sizeof(char));
  strcpy(buf,
      agent->datastate->agent_code);
  MUTEX_UNLOCK(agent->lock);
  return buf;
} /*}}}*/

EXPORTMC int 
MC_ResetSignal(MCAgency_t attr) /*{{{*/
{
  MUTEX_LOCK(attr->mc_platform->giant_lock);
  attr->mc_platform->giant = 1;
  attr->mc_platform->MC_signal = MC_NO_SIGNAL;
  COND_SIGNAL(attr->mc_platform->giant_cond);
  MUTEX_UNLOCK(attr->mc_platform->giant_lock);
  return 0;
} /*}}}*/

EXPORTMC int 
MC_SearchForService( /*{{{*/
    /* Input args */
    MCAgency_t attr, 
    const char *searchString,
    /* Return Args */
    char*** agentNames,
    char*** serviceNames,
    int** agentIDs,
    int* numResults)
{
  df_request_search_p search;
  df_search_results_p results;
  df_request_list_node_p request;
  search = df_request_search_New();
  CHECK_NULL(search, return MC_ERR_MEMORY;);
  results = (df_search_results_p)malloc(sizeof(df_search_results_t));
  CHECK_NULL(results, return MC_ERR_MEMORY;);
  request = df_request_list_node_New();
  CHECK_NULL(request, return MC_ERR_MEMORY;);


  search->search_results = results;
  search->search_string = (char*)searchString;

  request->data = (void*)search;
  request->command = (char*)malloc(sizeof(char) * 7);
  strcpy((char*)request->command, "search");
  request->data_size = sizeof(df_request_search_t);

  COND_SLEEP_ACTION(
      search->cond,
      search->lock,

      df_AddRequest(attr->mc_platform->df, request);
      );
  /* When we get here, search->results should be filled. */
  *agentNames = search->search_results->agent_names;
  *serviceNames = search->search_results->service_names;
  *agentIDs = search->search_results->agent_ids;
  *numResults = search->search_results->num_results;

  /* Free unused data structures */
  free((void*)request->command);
  df_request_list_node_Destroy(request);
  df_request_search_Destroy(search);

  return MC_SUCCESS;
} /*}}}*/

EXPORTMC int 
MC_SemaphorePost(MCAgency_t attr, int id) /*{{{*/
{
  syncListNode_t *syncnode;
  syncnode = syncListFind(id, attr->mc_platform->syncList);
  if (syncnode == NULL) {
    return 1;
  }
  SEMAPHORE_POST(syncnode->sem);
  return 0;
} /*}}}*/

EXPORTMC int 
MC_SemaphoreWait(MCAgency_t attr, int id) /*{{{*/
{
  syncListNode_t *syncnode;
  syncnode = syncListFind(id, attr->mc_platform->syncList);
  if (syncnode == NULL) {
    return 1;
  }
  SEMAPHORE_WAIT(syncnode->sem);
  return 0;
} /*}}}*/

EXPORTMC int
MC_SendCh(MCAgency_t attr, /*{{{*/
    const char *filename,
    const char *remotehost,
    int port)
{
  printf("Sorry, not implemented yet.\n");
  return -1;
} /*}}}*/

EXPORTMC int 
MC_SendAgent(MCAgency_t attr, /*{{{*/
    const char *string)
{
  message_p message;
  MCAgent_t agent;
  extern mc_platform_p g_mc_platform;
  mc_platform_p platform;

  if (attr == NULL) {
    platform = g_mc_platform;
  } else {
    platform = attr->mc_platform;
  }

  message = message_New();
  if(
      message_InitializeFromString
      (
       attr->mc_platform,
       message,
       string,
       NULL,
       0,
       "ams"
      )
    )
  {
    message_Destroy(message);
    return MC_ERR;
  } else {
    /* Here, we wish to fully parse the message and create a fully qualified
     * agent, then let the agency send the agent. */
    agent = agent_Initialize( platform, message, rand() );
    if (agent == NULL) {
      fprintf(stderr, "Error initializing agent. %s:%d", __FILE__, __LINE__);
    }
    agent->agent_status = MC_WAIT_MESSGSEND;
    agent_queue_Add( platform->agent_queue, agent);
    /* Set the ams to run */
    MUTEX_LOCK(platform->ams->runflag_lock);
    platform->ams->run = 1;
    COND_BROADCAST(platform->ams->runflag_cond);
    MUTEX_UNLOCK(platform->ams->runflag_lock);
  }
  return 0;
} /*}}}*/

EXPORTMC int
MC_SendAgentFile(MCAgency_t attr,  /*{{{*/
    const char *filename)
{
  struct stat filestat;
  char *buf;
  FILE *fp;
  int ret = 0;
  message_p message;
  agent_t *agent;
  extern mc_platform_p g_mc_platform;
  mc_platform_p platform;
  buf = NULL;
  filestat.st_size = 0;
  stat(filename, &filestat);
  if (filestat.st_size != 0 ) {
    buf = (char*)malloc( sizeof(char) * (filestat.st_size+1) );
    memset(buf, 0, filestat.st_size+1);
  } else {
    fprintf(stderr, "Error: File %s not found.\n", filename);
    return 1;
  }

  if (attr == NULL) {
    platform = g_mc_platform;
  } else {
    platform = attr->mc_platform;
  }

  fp = fopen(filename, "r");
  fread((void*)buf, filestat.st_size, 1, fp);
  fclose(fp);

  message = message_New();
  if( 
      message_InitializeFromString
      (
       platform,
       message,
       buf,
       NULL,
       0,
       "ams"
      )
    )
  {
    message_Destroy(message);
  } else {
    /* Here, we wish to fully parse the message and create a fully qualified
     * agent, then let the agency send the agent. */
    agent = agent_Initialize( platform, message, rand() );
    if (agent == NULL) {
      fprintf(stderr, "Error initializing agent. %s:%d", __FILE__, __LINE__);
    }
    agent->agent_status = MC_WAIT_MESSGSEND;
    agent_queue_Add( platform->agent_queue, agent);
    /* Set the ams to run */
    MUTEX_LOCK(platform->ams->runflag_lock);
    platform->ams->run = 1;
    COND_BROADCAST(platform->ams->runflag_cond);
    MUTEX_UNLOCK(platform->ams->runflag_lock);
  }
  free(buf);
  return ret;
} /*}}}*/

EXPORTMC int 
MC_SendAgentMigrationMessage(MCAgency_t attr, /*{{{*/
    const char *string,
    const char *hostname,
    int port)
{

  message_p message;
  printf("\nThe function MC_SendAgentMigrationMessage() is deprecated.\n\
  Please use MC_SendAgent() instead.\n");
  message = message_New();
  if(
      message_InitializeFromString
      (
       attr->mc_platform,
       message,
       string,
       hostname,
       port,
       "ams"
      )
    )
  {
    message_Destroy(message);
    return MC_ERR;
  } else {
    return message_queue_Add
      (
       attr->mc_platform->message_queue,
       message
      );
  }
} /*}}}*/

EXPORTMC int
MC_SendAgentMigrationMessageFile(MCAgency_t attr,  /*{{{*/
    const char *filename, 
    const char *hostname,
    int port)
{
  struct stat filestat;
  char *buf;
  FILE *fp;
  int ret = 0;
  message_p message;
  agent_t *agent;
  extern mc_platform_p g_mc_platform;
  mc_platform_p platform;
  printf("\nThe function MC_SendAgentMigrationMessage() is deprecated.\n\
  Please use MC_SendAgentFile() instead.");
  buf = NULL;
  filestat.st_size = 0;
  stat(filename, &filestat);
  if (filestat.st_size != 0 ) {
    buf = (char*)malloc( sizeof(char) * (filestat.st_size+1) );
    memset(buf, 0, filestat.st_size+1);
  } else {
    fprintf(stderr, "Error: File %s not found.\n", filename);
    return 1;
  }

  if (attr == NULL) {
    platform = g_mc_platform;
  } else {
    platform = attr->mc_platform;
  }

  fp = fopen(filename, "r");
  fread((void*)buf, filestat.st_size, 1, fp);
  fclose(fp);

  message = message_New();
  if( 
      message_InitializeFromString
      (
       platform,
       message,
       buf,
       hostname,
       port,
       "ams"
      )
    )
  {
    message_Destroy(message);
  } else {
    /* Here, we wish to fully parse the message and create a fully qualified
     * agent, then let the agency send the agent. */
    agent = agent_Initialize( platform, message, rand() );
    if (agent == NULL) {
      fprintf(stderr, "Error initializing agent. %s:%d", __FILE__, __LINE__);
    }
    agent->agent_status = MC_WAIT_MESSGSEND;
    agent_queue_Add( platform->agent_queue, agent);
    /* Set the ams to run */
    MUTEX_LOCK(platform->ams->runflag_lock);
    platform->ams->run = 1;
    COND_BROADCAST(platform->ams->runflag_cond);
    MUTEX_UNLOCK(platform->ams->runflag_lock);
  }
  free(buf);
  return ret;
} /*}}}*/

EXPORTMC int 
MC_SendSteerCommand(MCAgency_t attr, enum MC_SteerCommand_e cmd) /*{{{*/
{
  MUTEX_LOCK(attr->mc_platform->MC_steer_lock);
  attr->mc_platform->MC_steer_command = cmd;
  COND_BROADCAST(attr->mc_platform->MC_steer_cond);
  MUTEX_UNLOCK(attr->mc_platform->MC_steer_lock);
  return 0;
} /*}}}*/

EXPORTMC int
MC_SetAgentStatus(MCAgent_t agent, enum MC_AgentStatus_e status) /*{{{*/
{
  MUTEX_LOCK(agent->lock);
  agent->agent_status = status;
  if (!agent->orphan) {
    MUTEX_LOCK(agent->mc_platform->ams->runflag_lock);
    agent->mc_platform->ams->run = 1;
    COND_SIGNAL(agent->mc_platform->ams->runflag_cond);
    MUTEX_UNLOCK(agent->mc_platform->ams->runflag_lock);
  }
  MUTEX_UNLOCK(agent->lock);
  return 0;
} /*}}}*/

EXPORTMC int 
MC_SetDefaultAgentStatus(/*{{{*/
    MCAgency_t agency,
    enum MC_AgentStatus_e status
    ) 
{
  agency->mc_platform->default_agentstatus = status;
  return 0;
} /*}}}*/

EXPORTMC int 
MC_SetThreadOn(MCAgencyOptions_t *options, enum MC_ThreadIndex_e index) /*{{{*/
{
  SET_THREAD_ON(options->threads, index);
  return 0;
} /*}}}*/

EXPORTMC int 
MC_SetThreadsAllOn(MCAgencyOptions_t* options)
{
  int i;
  for(i = 0; i < MC_THREAD_ALL; i++) {
    SET_THREAD_ON(options->threads, i);
  }
  return 0;
}

EXPORTMC int
MC_SetThreadOff(MCAgencyOptions_t *options, enum MC_ThreadIndex_e index) /*{{{*/
{
  SET_THREAD_OFF(options->threads, index);
  return 0;
} /*}}}*/

EXPORTMC int
MC_SetThreadsAllOff(MCAgencyOptions_t* options)
{
  int i;
  for(i = 0; i < MC_THREAD_ALL; i++) {
    SET_THREAD_OFF(options->threads, i);
  }
  return 0;
}

EXPORTMC int 
MC_Steer(                        /*{{{*/
    MCAgency_t attr,
    int (*funcptr)(void* data),
    void *arg
    )
{
  MUTEX_LOCK(attr->mc_platform->MC_steer_lock);
  do {
    attr->mc_platform->MC_steer_command = MC_RUN;
    MUTEX_UNLOCK(attr->mc_platform->MC_steer_lock);
    (*funcptr)(arg);
  } while 
  (
   attr->mc_platform->MC_steer_command == MC_RESTART 
  );
  return 0;
} /*}}}*/

EXPORTMC enum MC_SteerCommand_e 
MC_SteerControl(void) /*{{{*/
{
  extern mc_platform_p g_mc_platform;
  /* Check to see what current command is */
  MUTEX_LOCK(g_mc_platform->MC_steer_lock);
  while (g_mc_platform->MC_steer_command == MC_SUSPEND) {
    COND_WAIT(
        g_mc_platform->MC_steer_cond,
        g_mc_platform->MC_steer_lock
        );
  }
  MUTEX_UNLOCK(g_mc_platform->MC_steer_lock);
  return g_mc_platform->MC_steer_command;
} /*}}}*/

EXPORTMC int 
MC_SyncDelete(MCAgency_t attr, int id) /*{{{*/
{
  syncListNode_t *sync_node;
  /* First, lock the entire list. */
  MUTEX_LOCK(attr->mc_platform->syncList->giant_lock);

  /* Find and lock the node */
  sync_node = syncListFind(id, attr->mc_platform->syncList);
  if (sync_node == NULL) {
    MUTEX_UNLOCK(attr->mc_platform->syncList->giant_lock);
    return MC_ERR_NOT_FOUND;
  }
  MUTEX_LOCK(sync_node->lock);

  /* Remove it from the list so it may no longer be used */
  if (syncListRemove(id, attr->mc_platform->syncList) == NULL) {
    fprintf(stderr, "Fatal error. %s:%d\n",
        __FILE__,
        __LINE__ );
    exit(0);
  }

  /* Now, unlock and destroy */
  MUTEX_UNLOCK(sync_node->lock);
  MUTEX_UNLOCK(attr->mc_platform->syncList->giant_lock);

  return syncListNodeDestroy(sync_node);
} /*}}}*/

EXPORTMC int
MC_SyncInit(MCAgency_t attr, int id) /*{{{*/
{
  syncListNode_t *node;
  node = syncListNodeNew();
  MUTEX_LOCK(attr->mc_platform->syncList->giant_lock);
  if (id == 0) {
    id = rand();
  }
  while (
      syncListFind(id, attr->mc_platform->syncList) != NULL
      ) 
  {
    id = rand();
  }

  node->id = id;
  syncListAddNode(
      node,
      attr->mc_platform->syncList
      );
  MUTEX_UNLOCK(attr->mc_platform->syncList->giant_lock);
  return id;
}/*}}}*/

EXPORTMC int 
MC_TerminateAgent(MCAgent_t agent) /*{{{*/
{
  int status=0;
  if(agent->agent_interp != NULL) {
    status = Ch_Abort (*agent->agent_interp);
  }
  return status;
} /*}}}*/

EXPORTMC int 
MC_TerminateAgentWG(MCAgent_t calling_agent, MCAgent_t agent) /*{{{*/
{
  int status=0;
  /* The agents must belong to the same workgroup*/
  if (agent->wg_code != NULL) {
    if (calling_agent->wg_code == NULL) {
      return MC_ERR_INVALID_ARGS;
    } else if (strcmp(calling_agent->wg_code, agent->wg_code)) {
      return MC_ERR_INVALID_ARGS;
    }
  }
  if(agent->agent_interp != NULL) {
    status = Ch_Abort (*agent->agent_interp);
  }
  return status;
} /*}}}*/

EXPORTMC int 
MC_MainLoop(MCAgency_t attr) 
{
  MUTEX_LOCK( attr->mc_platform->quit_lock );
  while( attr->mc_platform->quit != 1 ) {
    COND_WAIT( attr->mc_platform->quit_cond, attr->mc_platform->quit_lock );
  }
  MUTEX_UNLOCK( attr->mc_platform->quit_lock );
  return 0;
} /*}}}*/

EXPORTMC int
MC_WaitAgent(MCAgency_t attr) /*{{{*/
{
  int size;
  MUTEX_LOCK(attr->mc_platform->agent_queue->lock);
  while(1) {
    size = attr->mc_platform->agent_queue->size;
    COND_WAIT(
        attr->mc_platform->agent_queue->cond,
        attr->mc_platform->agent_queue->lock
        );
    if (size < attr->mc_platform->agent_queue->size) {
      MUTEX_UNLOCK(attr->mc_platform->agent_queue->lock);
      break;
    } 
  }
  MUTEX_UNLOCK(attr->mc_platform->agent_queue->lock);
  return 0;
} /*}}}*/

EXPORTMC MCAgent_t
MC_WaitRetrieveAgent(MCAgency_t attr) /*{{{*/
{
  int index;
  MCAgent_t agent;
  MC_WaitSignal(attr, MC_RECV_AGENT);
  MUTEX_LOCK(attr->mc_platform->agent_queue->lock);
  index = attr->mc_platform->agent_queue->size-1;
  agent = (MCAgent_t)ListSearch(
      attr->mc_platform->agent_queue->list, index);
  MUTEX_UNLOCK(attr->mc_platform->agent_queue->lock);
  return agent;
} /*}}}*/

/* MC_WaitSignal */
/* This function blocks until one of the signals in argument
 * 'signals' is signalled. 'signals' must be a binary | combination
 * of enum MC_Signal_e type. */
EXPORTMC int
MC_WaitSignal(MCAgency_t attr, int signals) /*{{{*/
{
  MUTEX_LOCK(attr->mc_platform->MC_signal_lock);
  while(! (signals & attr->mc_platform->MC_signal)) {
    COND_WAIT(
        attr->mc_platform->MC_signal_cond,
        attr->mc_platform->MC_signal_lock
        );
  }
  MUTEX_UNLOCK(attr->mc_platform->MC_signal_lock);
  MUTEX_LOCK(attr->mc_platform->giant_lock);
  attr->mc_platform->giant = 0;
  MUTEX_UNLOCK(attr->mc_platform->giant_lock);
  return 0;
} /*}}}*/

/* *********************** *
 * Ch Space chdl functions *
 * *********************** */

/* MC_AclDestroy */
EXPORTMC int MC_AclDestroy_chdl(void* varg)
{
  int retval;
  fipa_acl_message_t* acl_message;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  acl_message = Ch_VaArg(interp, ap, fipa_acl_message_t*);
  retval = MC_AclDestroy(acl_message);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclNew */
EXPORTMC void* MC_AclNew_chdl(void* varg)
{
  void* retval;
  retval = (void*)MC_AclNew();
  return retval;
}

/* MC_AclPost */
EXPORTMC int MC_AclPost_chdl(void* varg)
{
  int retval;
  agent_p agent;
  fipa_acl_message_t* acl_message;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, agent_p);
  acl_message = Ch_VaArg(interp, ap, fipa_acl_message_t*);
  retval = MC_AclPost(agent, acl_message);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclReply */
EXPORTCH void*
MC_AclReply_chdl(void* varg)
{
  void* retval;
  fipa_acl_message_t* acl_message;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  acl_message = Ch_VaArg(interp, ap, fipa_acl_message_t*);
  retval = (void*)MC_AclReply(acl_message);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclRetrieve */
EXPORTCH void*
MC_AclRetrieve_chdl(void* varg)
{
  void* retval;
  MCAgent_t agent;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  retval = MC_AclRetrieve(agent);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclSend */
EXPORTCH int
MC_AclSend_chdl(void* varg)
{
  int retval;
  fipa_acl_message_t* acl_message;
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;

  ChInterp_t interp;
  ChVaList_t ap;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  acl_message = (fipa_acl_message_t*) Ch_VaArg(interp, ap, void*);
  retval = MC_AclSend(temp_attr, acl_message);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
}

/* MC_AclWaitRetrieve */
EXPORTCH void*
MC_AclWaitRetrieve_chdl(void *varg)
{
  void* retval;
  MCAgent_t agent;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  retval = MC_AclWaitRetrieve(agent);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* BEGIN Acl helper functions */

/* MC_AclSetProtocol */
EXPORTCH int 
MC_AclGetProtocol_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  retval = MC_AclGetProtocol(acl);
  Ch_VaEnd(interp, ap);
  return retval;
}

EXPORTCH char*
MC_AclGetConversationID_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  char* retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  retval = MC_AclGetConversationID(acl);
  Ch_VaEnd(interp, ap);
  return retval;
}

EXPORTCH int
MC_AclGetPerformative_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  retval = MC_AclGetPerformative(acl);
  Ch_VaEnd(interp, ap);
  return retval;
}

EXPORTCH int
MC_AclGetSender_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  char** name;
  char** address;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  name = Ch_VaArg(interp, ap, char**);
  address = Ch_VaArg(interp, ap, char**);
  retval = MC_AclGetSender(acl, name, address);
  Ch_VaEnd(interp, ap);
  return retval;
}

EXPORTCH char*
MC_AclGetContent_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  char* retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  retval = (char*)MC_AclGetContent(acl);
  Ch_VaEnd(interp, ap);
  return retval;
}

EXPORTCH int 
MC_AclSetProtocol_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  enum fipa_protocol_e protocol;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  protocol = Ch_VaArg(interp, ap, enum fipa_protocol_e);
  retval = MC_AclSetProtocol(acl, protocol);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclSetContentID */
EXPORTCH int
MC_AclSetConversationID_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  char* id;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  id = Ch_VaArg(interp, ap, char*);
  retval = MC_AclSetConversationID(acl, id);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclSetPerformative */
EXPORTCH int
MC_AclSetPerformative_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  enum fipa_performative_e performative;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  performative = Ch_VaArg(interp, ap, enum fipa_performative_e);
  retval = MC_AclSetPerformative(acl, performative);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclSetSender */
EXPORTCH int
MC_AclSetSender_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  char* name;
  char* address;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  name = Ch_VaArg(interp, ap, char*);
  address = Ch_VaArg(interp, ap, char*);
  retval = MC_AclSetSender(acl, name, address);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclAddReceiver */
EXPORTCH int
MC_AclAddReceiver_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  char* name;
  char* address;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  name = Ch_VaArg(interp, ap, char*);
  address = Ch_VaArg(interp, ap, char*);
  retval = MC_AclAddReceiver(acl, name, address);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclAddReplyTo */
EXPORTCH int
MC_AclAddReplyTo_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  char* name;
  char* address;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  name = Ch_VaArg(interp, ap, char*);
  address = Ch_VaArg(interp, ap, char*);
  retval = MC_AclAddReplyTo(acl, name, address);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AclSetContent */
EXPORTCH int
MC_AclSetContent_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct fipa_acl_message_s* acl;
  char* content;
  int retval;

  Ch_VaStart(interp, ap, varg);
  acl = Ch_VaArg(interp, ap, struct fipa_acl_message_s*);
  content = Ch_VaArg(interp, ap, char*);
  retval = MC_AclSetContent(acl, content);
  Ch_VaEnd(interp, ap);
  return retval;
}

/* END Acl Helper Functions */

/* MC_AddAgent */
EXPORTCH int
MC_AddAgent_chdl(void *varg) /*{{{*/
{
  int retval;
  MCAgent_t agent;
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;

  ChInterp_t interp;
  ChVaList_t ap;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  agent = (MCAgent_t) Ch_VaArg(interp, ap, void*);
  retval = MC_AddAgent(temp_attr, agent);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_AgentAddTask */
EXPORTCH int 
MC_AgentAddTask_chdl(void *varg) /*{{{*/
{
  int retval;

  /* Function Args */
  MCAgent_t agent;
  const char* code;
  const char* return_var_name;
  const char* server;
  int persistent;

  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);

  agent = Ch_VaArg(interp, ap, MCAgent_t);
  code = Ch_VaArg(interp, ap, const char*);
  return_var_name = Ch_VaArg(interp, ap, const char*);
  server = Ch_VaArg(interp, ap, const char*);
  persistent = Ch_VaArg(interp, ap, int);

  retval= MC_AgentAddTask(
      agent,
      code,
      return_var_name,
      server,
      persistent);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_AgentVariableRetrieve */
EXPORTCH const void*
MC_AgentVariableRetrieve_chdl(void* varg)
{
  const void* retval;
  MCAgent_t agent;
  const char* var_name;
  int task_num;

  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);

  agent = Ch_VaArg(interp, ap, MCAgent_t);
  var_name = Ch_VaArg(interp, ap, const char* );
  task_num = Ch_VaArg(interp, ap, int);

  retval = MC_AgentVariableRetrieve(agent, var_name, task_num);

  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_AgentVariableSave */
EXPORTCH int
MC_AgentVariableSave_chdl(void *varg)
{
  int retval;
  MCAgent_t agent;
  const char* var_name;

  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);

  agent = Ch_VaArg(interp, ap, MCAgent_t);
  var_name = Ch_VaArg(interp, ap, const char*);

  retval = MC_AgentVariableSave(agent, var_name);

  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_CallAgentFunc */
EXPORTCH int
MC_CallAgentFunc_chdl(void *varg) /*{{{*/
{
  int retval;
  /* Function Args */
  MCAgent_t agent;
  const char* funcName;
  void* returnVal;
  ChVaList_t args;

  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);

  agent =     Ch_VaArg(interp, ap, MCAgent_t);
  funcName =  Ch_VaArg(interp, ap, const char*);
  returnVal = Ch_VaArg(interp, ap, void*);
  args =      Ch_VaArg(interp, ap, void*);

  retval = MC_CallAgentFuncVar(
      agent,
      funcName,
      returnVal,
      args);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_Barrier_chdl*/
EXPORTCH int
MC_Barrier_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  retval = MC_Barrier(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_BarrierDelete_chdl*/
EXPORTCH int
MC_BarrierDelete_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  retval = MC_BarrierDelete(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_BarrierInit_chdl*/
EXPORTCH int
MC_BarrierInit_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;
  int num_procs;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  num_procs = Ch_VaArg(interp, ap, int);
  retval = MC_BarrierInit(temp_attr, id, num_procs);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_CondBroadcast_chdl*/
EXPORTCH int
MC_CondBroadcast_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  retval = MC_CondBroadcast(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_ComposeAgent */
EXPORTCH MCAgent_t 
MC_ComposeAgent_chdl(void *varg) /*{{{*/
{
  MCAgent_t retval;

  /* Function Args */
  const char* name;
  const char* home;
  const char* owner;  
  const char* code;
  const char* return_var_name;
  const char* server;
  int persistent;

  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);

  name  = Ch_VaArg(interp, ap, const char*);
  home  = Ch_VaArg(interp, ap, const char*);
  owner = Ch_VaArg(interp, ap, const char*);
  code = Ch_VaArg(interp, ap, const char*);
  return_var_name = Ch_VaArg(interp, ap, const char*);
  server = Ch_VaArg(interp, ap, const char*);
  persistent = Ch_VaArg(interp, ap, int);

  retval= MC_ComposeAgentWithWorkgroup(
      name,
      home,
      owner,
      code,
      return_var_name,
      server,
      persistent,
      NULL);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_ComposeAgent */
EXPORTCH MCAgent_t 
MC_ComposeAgentWithWorkgroup_chdl(void *varg) /*{{{*/
{
  MCAgent_t retval;

  /* Function Args */
  const char* name;
  const char* home;
  const char* owner;  
  const char* code;
  const char* return_var_name;
  const char* server;
  const char* workgroup_code;
  int persistent;

  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);

  name  = Ch_VaArg(interp, ap, const char*);
  home  = Ch_VaArg(interp, ap, const char*);
  owner = Ch_VaArg(interp, ap, const char*);
  code = Ch_VaArg(interp, ap, const char*);
  return_var_name = Ch_VaArg(interp, ap, const char*);
  server = Ch_VaArg(interp, ap, const char*);
  persistent = Ch_VaArg(interp, ap, int);
  workgroup_code = Ch_VaArg(interp, ap, const char*);

  retval= MC_ComposeAgentS(
      name,
      home,
      owner,
      code,
      return_var_name,
      server,
      persistent,
			workgroup_code);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_ComposeAgentFromFile */
EXPORTCH MCAgent_t 
MC_ComposeAgentFromFile_chdl(void *varg) /*{{{*/
{
  MCAgent_t retval;

  /* Function Args */
  const char* name;
  const char* home;
  const char* owner;  
  const char* filename;
  const char* return_var_name;
  const char* server;
  int persistent;

  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);

  name  = Ch_VaArg(interp, ap, const char*);
  home  = Ch_VaArg(interp, ap, const char*);
  owner = Ch_VaArg(interp, ap, const char*);
  filename = Ch_VaArg(interp, ap, const char*);
  return_var_name = Ch_VaArg(interp, ap, const char*);
  server = Ch_VaArg(interp, ap, const char*);
  persistent = Ch_VaArg(interp, ap, int);

  retval= MC_ComposeAgentFromFileWithWorkgroup(
      name,
      home,
      owner,
      filename,
      return_var_name,
      server,
      persistent,
      NULL);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_ComposeAgentFromFileWithWorkgroup */
EXPORTCH MCAgent_t 
MC_ComposeAgentFromFileWithWorkgroup_chdl(void *varg) /*{{{*/
{
  MCAgent_t retval;

  /* Function Args */
  const char* name;
  const char* home;
  const char* owner;  
  const char* filename;
  const char* return_var_name;
  const char* server;
  const char* workgroup;
  int persistent;

  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);

  name  = Ch_VaArg(interp, ap, const char*);
  home  = Ch_VaArg(interp, ap, const char*);
  owner = Ch_VaArg(interp, ap, const char*);
  filename = Ch_VaArg(interp, ap, const char*);
  return_var_name = Ch_VaArg(interp, ap, const char*);
  server = Ch_VaArg(interp, ap, const char*);
  workgroup = Ch_VaArg(interp, ap, const char*);
  persistent = Ch_VaArg(interp, ap, int);

  retval= MC_ComposeAgentFromFileWithWorkgroup(
      name,
      home,
      owner,
      filename,
      return_var_name,
      server,
      persistent,
      workgroup);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_CondSignal_chdl*/
EXPORTCH int
MC_CondSignal_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  retval = MC_CondSignal(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_CondReset_chdl*/
EXPORTCH int
MC_CondReset_chdl(void *varg) /*{{{*/   
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  retval = MC_CondReset(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_CondWait_chdl*/
EXPORTCH int
MC_CondWait_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  retval = MC_CondWait(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

EXPORTCH int
MC_DeleteAgent_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  MCAgent_t agent;
  const char* agentName;
  int retval;

  Ch_VaStart(interp, ap, varg);
  agentName = Ch_VaArg(interp, ap, const char*);
  agent = MC_FindAgentByName(NULL, agentName);
  if (agent == NULL) {
    return MC_ERR_NOT_FOUND;
  }
  retval = MC_DeleteAgent(agent);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

EXPORTCH int
MC_DeleteAgentWG_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  const char* agentName;
  MCAgent_t calling_agent;
  MCAgent_t agent;
  int retval;

  Ch_VaStart(interp, ap, varg);
  calling_agent = Ch_VaArg(interp, ap, MCAgent_t);
  agentName = Ch_VaArg(interp, ap, const char*);
  agent = MC_FindAgentByName(NULL, agentName);
  if (agent == NULL) {
    return MC_ERR_NOT_FOUND;
  }
  retval = MC_DeleteAgentWG(calling_agent, agent);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

EXPORTCH int
MC_DestroyServiceSearchResult_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  char** agentName;
  char** serviceName;
  int* agentID;
  int numResult;
  int retval;

  Ch_VaStart(interp, ap, varg);
  agentName = Ch_VaArg(interp, ap, char**);
  serviceName = Ch_VaArg(interp, ap, char**);
  agentID = Ch_VaArg(interp, ap, int*);
  numResult = Ch_VaArg(interp, ap, int);

  retval = MC_DestroyServiceSearchResult(
      agentName,
      serviceName,
      agentID,
      numResult );
  Ch_VaEnd(interp, ap);
  return retval;
}

/* MC_DeregisterService_chdl */
EXPORTCH int
MC_DeregisterService_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int agentID;
  char *serviceName;
  int retval;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  agentID = Ch_VaArg(interp, ap, int);
  serviceName = (char*)Ch_VaArg(interp, ap, const char*);
  retval = MC_DeregisterService(
      temp_attr,
      agentID,
      serviceName );
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

EXPORTCH int
MC_End_chdl(void *varg) /* {{{ */
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  retval = MC_End(temp_attr);

  return retval;
} /* }}} */

/* MC_FindAgentByID_chdl*/
EXPORTCH MCAgent_t
MC_FindAgentByID_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  MCAgent_t retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  retval = MC_FindAgentByID(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_FindAgentByName_chdl*/
EXPORTCH MCAgent_t
MC_FindAgentByName_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  MCAgent_t retval;
  ChInterp_t interp;
  ChVaList_t ap;
  const char *name;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  name = Ch_VaArg(interp, ap, const char *);
  retval = MC_FindAgentByName(temp_attr, name);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_GetAgentArrivalTime */
#ifndef _WIN32
EXPORTCH time_t
#else
EXPORTCH SYSTEMTIME
#endif
MC_GetAgentArrivalTime_chdl(void *varg) /*{{{*/
{
  MCAgent_t agent;
  ChInterp_t interp;
  ChVaList_t ap;
#ifndef _WIN32
  time_t arrival_time;
#else
  SYSTEMTIME arrival_time;
#endif

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  arrival_time = MC_GetAgentArrivalTime(agent);
  Ch_VaEnd(interp, ap);
  return arrival_time;
} /* }}} */

/* MC_GetAgentID */
EXPORTCH int
MC_GetAgentID_chdl(void *varg) /*{{{*/
{
  MCAgent_t agent;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  id = MC_GetAgentID(agent);
  Ch_VaEnd(interp, ap);
  return id;
} /*}}}*/

/* MC_GetAgentName */
EXPORTCH char*
MC_GetAgentName_chdl(void *varg) /*{{{*/
{
  MCAgent_t agent;
  ChInterp_t interp;
  ChVaList_t ap;
  char* name;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  name = MC_GetAgentName(agent);
  Ch_VaEnd(interp, ap);
  return name;
} /*}}}*/

/* MC_GetAgentNumTasks_chdl */
EXPORTCH int
MC_GetAgentNumTasks_chdl(void *varg)
{
  MCAgent_t agent;
  ChInterp_t interp;
  ChVaList_t ap;
  int num_tasks;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  num_tasks = MC_GetAgentNumTasks(agent);
  Ch_VaEnd(interp, ap);
  return num_tasks;
}

/* MC_GetAgentStatus_chdl */
EXPORTCH int
MC_GetAgentStatus_chdl(void *varg) /*{{{*/
{
  MCAgent_t agent;
  int status;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  status = MC_GetAgentStatus(agent);
  Ch_VaEnd(interp, ap);
  return status;
} /*}}}*/

/* MC_GetAgentXMLString_chdl */
EXPORTCH char *
MC_GetAgentXMLString_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  MCAgent_t agent;
  char *retval;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  retval = MC_GetAgentXMLString(agent);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

#ifndef _WIN32
EXPORTCH int
MC_GetTimeOfDay_chdl(void *varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  struct timeval *tv;
  Ch_VaStart(interp, ap, varg);
  tv = Ch_VaArg(interp, ap, struct timeval*);
  gettimeofday(tv, NULL);
  Ch_VaEnd(interp, ap);
  return 0;
}
#endif

/* MC_HaltAgency_chdl */
EXPORTCH int
MC_HaltAgency_chdl(void *varg)
{
  MCAgency_t temp_attr;
  int retval;
  extern mc_platform_p g_mc_platform;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  retval = MC_HaltAgency(temp_attr);

  free(temp_attr);
  return retval;
}

EXPORTCH int
MC_MigrateAgent_chdl(void *varg)
{
  MCAgent_t agent;
  const char* hostname;
  int port;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  hostname = Ch_VaArg(interp, ap, const char*);
  port = Ch_VaArg(interp, ap, int);
  retval = MC_MigrateAgent(agent, hostname, port);
  return retval;
}

/* MC_MutexLock_chdl */
EXPORTCH int
MC_MutexLock_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;

  ChInterp_t interp;
  ChVaList_t ap;
  int id;
  int retval;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int );
  retval = MC_MutexLock(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_MutexUnlock_chdl */
EXPORTCH int
MC_MutexUnlock_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;

  ChInterp_t interp;
  ChVaList_t ap;
  int id;
  int retval;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int );
  retval = MC_MutexUnlock(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_PrintAgentCode_chdl */
EXPORTCH int
MC_PrintAgentCode_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  MCAgent_t agent;
  int retval;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  retval = MC_PrintAgentCode(agent);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_RegisterService_chdl */
EXPORTCH int
MC_RegisterService_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  int retval;
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;

  /* varg arguments */
  MCAgent_t agent;
  char **serviceNames;
  int numServices;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  serviceNames = Ch_VaArg(interp, ap, char **);
  numServices = Ch_VaArg(interp, ap, int);

  retval = MC_RegisterService(
      temp_attr,      /* agency */
      agent,          /* agent */ 
      0,              /* agent id */
      NULL,           /* agent name */
      serviceNames,   /* services */
      numServices
      );
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_ResumeAgency_chdl */
EXPORTCH int
MC_ResumeAgency_chdl(void *varg)
{
  MCAgency_t temp_attr;
  int retval;
  extern mc_platform_p g_mc_platform;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  retval = MC_ResumeAgency(temp_attr);

  free(temp_attr);
  return retval;
}

/* MC_RetrieveAgent_chdl */
EXPORTCH MCAgent_t
MC_RetrieveAgent_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  MCAgent_t agent;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  agent = MC_RetrieveAgent(temp_attr);
  free(temp_attr);
  return agent;
} /*}}}*/

/* MC_RetrieveAgentCode_chdl */
EXPORTCH char *
MC_RetrieveAgentCode_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  MCAgent_t agent;
  char *retval;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  retval = MC_RetrieveAgentCode(agent);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_SaveData_chdl */
EXPORTCH int
MC_SaveData_chdl(void* varg)
{
  ChInterp_t interp;
  ChVaList_t ap;
  interpreter_variable_data_p interp_var_data;
  MCAgent_t agent;
  int progress;
  const char* name;
  int size;
  void* data;
  
  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  name = Ch_VaArg(interp, ap, char*);
  size = Ch_VaArg(interp, ap, int);
  data = Ch_VaArg(interp, ap, void*);

  progress = agent->datastate->task_progress;

  interp_var_data = interpreter_variable_data_New();
  interp_var_data->name = strdup(name);
  interp_var_data->size = size;
  interp_var_data->data_type = CH_VOIDPTRTYPE;
  interp_var_data->data = malloc(size);
  memcpy(interp_var_data->data, data, size);

  agent_variable_list_Add(
      agent->datastate->tasks[progress]->agent_variable_list,
      interp_var_data );

  return 0;
}

/* MC_SearchForService_chdl */
EXPORTCH int
MC_SearchForService_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  int retval;
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;

  /* Args */
  const char* searchString;
  char*** agentNames;
  char*** serviceNames;
  int** agentIDs;
  int* numResults;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  searchString = Ch_VaArg(interp, ap, const char*);
  agentNames = Ch_VaArg(interp, ap, char***);
  serviceNames = Ch_VaArg(interp, ap, char***);
  agentIDs = Ch_VaArg(interp, ap, int**);
  numResults = Ch_VaArg(interp, ap, int*);

  retval = MC_SearchForService(
      temp_attr,
      searchString,
      agentNames,
      serviceNames,
      agentIDs,
      numResults
      );
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_SemaphorePost_chdl */
EXPORTCH int
MC_SemaphorePost_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;

  ChInterp_t interp;
  ChVaList_t ap;
  int id;
  int retval;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int );
  retval = MC_SemaphorePost(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_SemaphoreWait_chdl */
EXPORTCH int 
MC_SemaphoreWait_chdl(void *varg) /*{{{*/
{   
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;

  ChInterp_t interp;
  ChVaList_t ap;
  int id;
  int retval;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int );
  retval = MC_SemaphoreWait(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_SendAgentMigrationMessage_chdl*/
EXPORTCH int
MC_SendAgentMigrationMessage_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  const char *message, *hostname;
  int port, retval;
  ChInterp_t interp;
  ChVaList_t ap;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  message = Ch_VaArg(interp, ap, char *);
  hostname = Ch_VaArg(interp, ap, char *);
  port = Ch_VaArg(interp, ap, int);
  retval = MC_SendAgentMigrationMessage(temp_attr, message, hostname, port);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_SendAgentMigrationMessageFile_chdl*/
EXPORTCH int
MC_SendAgentMigrationMessageFile_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  char *filename, *hostname;
  int port, retval;
  ChInterp_t interp;
  ChVaList_t ap;

  temp_attr = NULL;

  Ch_VaStart(interp, ap, varg);
  filename = Ch_VaArg(interp, ap, char *);
  hostname = Ch_VaArg(interp, ap, char *);
  port = Ch_VaArg(interp, ap, int);
  retval = MC_SendAgentMigrationMessageFile(temp_attr, filename, hostname, port);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

EXPORTCH int
MC_SendSteerCommand_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  int retval;
  enum MC_SteerCommand_e command;
  extern mc_platform_p g_mc_platform;
  ChInterp_t interp;
  ChVaList_t ap;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  command = Ch_VaArg(interp, ap, enum MC_SteerCommand_e );
  retval = MC_SendSteerCommand(temp_attr, command);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_SetAgentStatus_chdl */
EXPORTCH int
MC_SetAgentStatus_chdl(void *varg) /*{{{*/
{
  MCAgent_t agent;
  int status;
  int ret;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  agent = Ch_VaArg(interp, ap, MCAgent_t);
  status = Ch_VaArg(interp, ap, int);
  ret = MC_SetAgentStatus(agent, (enum MC_AgentStatus_e) status);
  Ch_VaEnd(interp, ap);
  return ret;
} /*}}}*/

/* MC_SetDefaultAgentStatus_chdl */
EXPORTCH int
MC_SetDefaultAgentStatus_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int status;
  int ret;
  ChInterp_t interp;
  ChVaList_t ap;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(1););
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  status = Ch_VaArg(interp, ap, int);
  ret = (int)MC_SetDefaultAgentStatus(temp_attr, (enum MC_AgentStatus_e)status);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return ret;
} /*}}}*/

/* MC_SyncDelete_chdl*/
EXPORTCH int
MC_SyncDelete_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);
  retval = MC_SyncDelete(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_SyncInit_chdl*/
EXPORTCH int
MC_SyncInit_chdl(void *varg) /*{{{*/
{
  MCAgency_t temp_attr;
  extern mc_platform_p g_mc_platform;
  int retval;
  ChInterp_t interp;
  ChVaList_t ap;
  int id;

  temp_attr = (MCAgency_t)malloc(sizeof(struct agency_s));
  CHECK_NULL(temp_attr, exit(-1));
  temp_attr->mc_platform = g_mc_platform;

  Ch_VaStart(interp, ap, varg);
  id = Ch_VaArg(interp, ap, int);

  retval = MC_SyncInit(temp_attr, id);
  Ch_VaEnd(interp, ap);
  free(temp_attr);
  return retval;
} /*}}}*/

/* MC_TerminateAgent_chdl */
EXPORTCH int
MC_TerminateAgent_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  const char* agentName;
  MCAgent_t agent;
  int retval;

  Ch_VaStart(interp, ap, varg);
  agentName = Ch_VaArg(interp, ap, const char*);
  agent = MC_FindAgentByName(NULL, agentName);
  if (agent == NULL) {
    return MC_ERR_NOT_FOUND;
  }
  retval = MC_TerminateAgent(agent);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/

/* MC_TerminateAgentWG_chdl */
EXPORTCH int
MC_TerminateAgentWG_chdl(void *varg) /*{{{*/
{
  ChInterp_t interp;
  ChVaList_t ap;
  const char* agentName;
  MCAgent_t calling_agent;
  MCAgent_t agent;
  int retval;

  Ch_VaStart(interp, ap, varg);
  calling_agent = Ch_VaArg(interp, ap, MCAgent_t);
  agentName = Ch_VaArg(interp, ap, const char*);
  agent = MC_FindAgentByName(NULL, agentName);
  if (agent == NULL) {
    return MC_ERR_NOT_FOUND;
  }
  retval = MC_TerminateAgentWG(calling_agent, agent);
  Ch_VaEnd(interp, ap);
  return retval;
} /*}}}*/
