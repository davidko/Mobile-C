 /* SVN FILE INFO
 * $Revision: 522 $ : Last Committed Revision
 * $Date: 2010-06-11 16:52:39 -0700 (Fri, 11 Jun 2010) $ : Last Committed Date */
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
#include <unistd.h>
#include "config.h"
#else
#include "winconfig.h"
#endif

#include <embedch.h>

#include "include/libmc.h"
#include "include/agent.h"
#include "include/agent_share_data.h"
#include "include/mc_platform.h"
#include "include/message.h"
#include "include/agent_lib.h"
#include "include/interpreter_variable_data.h"
#include "include/xml_parser.h"
#include "include/fipa_acl.h"

int agent_AddPersistentVariable(agent_p agent, int task_num, const char* var_name)
{
  int i;
  int size;
  int data_type_size;
  int progress;
  interpreter_variable_data_t *agent_variable_data;
  interpreter_variable_data_t *tmp;
  agent_variable_data = (interpreter_variable_data_t*)malloc(sizeof(interpreter_variable_data_t));
  agent_variable_data->name = strdup(var_name);

  /* Get the array data type */
  agent_variable_data->data_type = Ch_DataType(
      *agent->agent_interp,
      var_name );
  /* Make sure the variable exists */
  if (agent_variable_data->data_type == CH_UNDEFINETYPE) {
    free(agent_variable_data);
    fprintf(stderr, "Warning: agent %s attempted saving of invalid variable, %s. %s:%d\n",
        agent->name, var_name, __FILE__, __LINE__);
    return MC_ERR;
  }
  /* Get the array dimension */
  agent_variable_data->array_dim = Ch_ArrayDim(
      *agent->agent_interp,
      var_name );
  /* Get the array extents */
  agent_variable_data->array_extent = (int*)malloc(
      sizeof(int) * agent_variable_data->array_dim );
  for (i=0; i<agent_variable_data->array_dim; i++) {
    agent_variable_data->array_extent[i] = 
      Ch_ArrayExtent(
          *agent->agent_interp,
          var_name,
          i );
  }
  /* Finally, allocate and point returnData to the right place. */
  size = 1;
  for (i=0; i < agent_variable_data->array_dim; i++) {
    size *= agent_variable_data->array_extent[i];
  }

  /* Now get the data type size */
  CH_DATATYPE_SIZE(agent_variable_data->data_type, data_type_size);

  agent_variable_data->data = (void*)malloc(size * data_type_size);
  CHECK_NULL(agent_variable_data->data, exit(0));
  /* Copy the data over from the agent */
  /* For now, only support statically allocated global vars. */
  progress = agent->datastate->task_progress;
  i = 0;

  if (agent_variable_data->array_dim == 0) {
    memcpy(
        agent_variable_data->data,
        (void*)Ch_GlobalSymbolAddrByName(
          *agent->agent_interp,
          var_name),
        size*data_type_size
        );

  } else {
    memcpy(
        agent_variable_data->data,
        (void*)Ch_GlobalSymbolAddrByName(
          *agent->agent_interp,
          var_name),
        size*data_type_size
        );
  }
  agent_variable_data->size = size*data_type_size;
  
  /* Make sure that the variable is not already in the agent's list already. */
  ListWRLock(agent->datastate->tasks[task_num]->agent_variable_list);
  tmp = (interpreter_variable_data_t*)ListDeleteCB(
      agent->datastate->tasks[task_num]->agent_variable_list, 
      var_name,
      (ListSearchFunc_t)interpreter_variable_data_CmpName);
  if(tmp) interpreter_variable_data_Destroy(tmp);
  ListAdd(
      agent->datastate->tasks[task_num]->agent_variable_list, 
      agent_variable_data);
  ListWRUnlock(agent->datastate->tasks[task_num]->agent_variable_list);
  return 0;
}

  agent_p
agent_Copy(const agent_p agent)
{
  agent_p cp_agent;
  cp_agent = agent_New();

  MUTEX_LOCK(agent->lock);
  /* id */
  cp_agent->id = agent->id;
  /* name */
  cp_agent->name = (char*)malloc
    (
     sizeof(char) * 
     (strlen(agent->name) + 1)
    );
  strcpy(cp_agent->name, agent->name);
  /* connect_id: Not Needed */
  /* arrival_time */
  cp_agent->arrival_time = agent->arrival_time;
  /*owner*/
  cp_agent->owner = (char*)malloc
    (
     sizeof(char) * 
     (strlen(agent->owner) + 1)
    );
  strcpy(cp_agent->owner, agent->owner);
  /*home*/
  cp_agent->home = (char*)malloc
    (
     sizeof(char) * 
     (strlen(agent->home) + 1)
    );
  strcpy(cp_agent->home, agent->home);
  /*home_port*/
  cp_agent->home_port = agent->home_port;
  /*datastate*/
  cp_agent->datastate = agent_datastate_Copy(agent->datastate);
  /* Agent is an orphan */
  cp_agent->orphan = 1;
  /*agent_type*/
  cp_agent->agent_type = agent->agent_type;
  /*agent_status*/
  cp_agent->agent_status = agent->agent_status;
  /*return_data*/
  cp_agent->return_data = agent->return_data;
  /*agent_interp*/
  cp_agent->agent_interp = NULL;
  /*run_lock*/
  cp_agent->run_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(cp_agent->run_lock);
  /*agent_persistent*/
  cp_agent->agent_persistent = agent->agent_persistent;
  
  /*lock*/
  cp_agent->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(cp_agent->lock);
  MUTEX_NEW(cp_agent->agent_status_lock);
  MUTEX_INIT(cp_agent->agent_status_lock);
  COND_NEW(cp_agent->agent_status_cond);
  COND_INIT(cp_agent->agent_status_cond);

	/* Mailbox */
	cp_agent->mailbox = ListInitialize();

  cp_agent->agent_share_data_queue = ListCopy(
      agent->agent_share_data_queue, 
      (ListElemCopyFunc_t)agent_share_data_Copy);

  return cp_agent;
}

agent_p
agent_New(void)
{
  agent_p agent;
  agent = (agent_p)malloc(sizeof(agent_t));
  if(agent==NULL) {
    fprintf(stderr, "Memory error at %s:%d\n",
        __FILE__, __LINE__);
    return NULL;
  }
  agent_Init(agent);
  return agent;
}

int agent_Init(agent_p agent)
{
  /* Just init everything to zero */
  memset(agent, 0, sizeof(agent_t));

  /* Lets go ahead and allocate the threading stuff */
  MUTEX_NEW(agent->run_lock);
  MUTEX_INIT(agent->run_lock);

  MUTEX_NEW(agent->lock);
  MUTEX_INIT(agent->lock);

  MUTEX_NEW(agent->agent_status_lock);
  MUTEX_INIT(agent->agent_status_lock);
  COND_NEW(agent->agent_status_cond);
  COND_INIT(agent->agent_status_cond);

  agent->mailbox = ListInitialize();

  agent->agent_share_data_queue = ListInitialize();
  return 0;
}

  agent_p 
agent_NewBinary( struct mc_platform_s *mc_platform)
{
  agent_p agent; 

  /* malloc memory for the agent */
  agent = (MCAgent_t)malloc(sizeof(agent_t));
  memset(agent, 0, sizeof(agent_t));

  /* Set up general agent data access mutex */
  agent->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(agent->lock);

  /* set up agent status mutex and cond */
  MUTEX_NEW(agent->agent_status_lock);
  MUTEX_INIT(agent->agent_status_lock);
  COND_NEW(agent->agent_status_cond);
  COND_INIT(agent->agent_status_cond);

  /* Set up run_lock mutex */
  agent->run_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(agent->run_lock);

  /* set flags and variables for the agent */
  agent->id = rand();
#ifndef _WIN32
  agent->arrival_time = time(NULL);
#else
  GetSystemTime( &(agent->arrival_time) );
#endif

  agent->home = NULL;
  agent->sender = NULL;

  /* set flags */
  agent->orphan = 0;
  agent->agent_script_ready = 1;
  agent->agent_pipe_ready_to_read = 0;
  agent->agent_ready_to_send = 0;
  agent->agent_pipe_active = 0;
	agent->binary = 1;

  /* set the agent thread to null until activated */
  agent->agent_thread_id = 0;

  /* Set up an empty mailbox */
  agent->mailbox = ListInitialize();

  /* Set up data share */
  agent->agent_share_data_queue = ListInitialize();

  /* In the future we will compare the current tasks server name to 
     the one on the server, presently this is not implemented */

  /* set the CH exectution flag */
  /* FIXME: This should be in the xml parser */
  /*if (agent->datastate->tasks[agent->datastate->task_progress]->init_agent_status != -1) {
    agent->agent_status = agent->datastate->tasks[agent->datastate->task_progress]->init_agent_status;
    } else {
    agent->agent_status = mc_platform->default_agentstatus;
    }
    */
  MUTEX_LOCK(agent->agent_status_lock);
  agent->agent_status = MC_AGENT_ACTIVE;
  COND_BROADCAST(agent->agent_status_cond);
  MUTEX_UNLOCK(agent->agent_status_lock);

  agent->mc_platform = mc_platform;

	agent->agent_address = (char*)malloc(sizeof(char) * 
			(strlen(agent->mc_platform->hostname) + 12 + 10)
		);
	if (agent->agent_address == NULL) {
		fprintf(stderr, "Memory error. %s:%d\n", __FILE__, __LINE__);
		exit(-1);
	}
  sprintf(agent->agent_address,
      "http://%s:%d/acc",
      agent->mc_platform->hostname,
      agent->mc_platform->port
      );
  /* return */
  return agent;
}
  agent_p 
agent_Initialize(
    struct mc_platform_s *mc_platform,
    message_p message,
    int id)
{
  agent_p agent; 
  int err_code;

  /* malloc memory for the agent */
  agent = (MCAgent_t)malloc(sizeof(agent_t));
  memset(agent, 0, sizeof(agent_t));

  /* Set up general agent data access mutex */
  agent->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(agent->lock);

  /* Set up agent status cond/mutex */
  MUTEX_NEW(agent->agent_status_lock);
  MUTEX_INIT(agent->agent_status_lock);
  COND_NEW(agent->agent_status_cond);
  COND_INIT(agent->agent_status_cond);

  /* Set up run_lock mutex */
  agent->run_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(agent->run_lock);

  /* set flags and variables for the agent */
  agent->id = id;
#ifndef _WIN32
  agent->arrival_time = time(NULL);
#else
  GetSystemTime( &(agent->arrival_time) );
#endif

  agent->home = NULL;
  agent->sender = NULL;

  /* set flags */
  agent->orphan = 0;
  agent->agent_script_ready = 1;
  agent->agent_pipe_ready_to_read = 0;
  agent->agent_ready_to_send = 0;
  agent->agent_pipe_active = 0;

  /* set the agent thread to null until activated */
  agent->agent_thread_id = 0;

  /* Set up an empty mailbox */
  agent->mailbox = ListInitialize();

  /* Set up agent share */
  agent->agent_share_data_queue = ListInitialize();

  /* parse the xml */
  agent->datastate = agent_datastate_New();
  agent->datastate->xml_agent_root = message->xml_payload;
  agent->datastate->xml_root = message->xml_root;
  message->agent_xml_flag = 1;
  
  if (agent->datastate->xml_agent_root != NULL) {
    switch(message->message_type) {
      case MOBILE_AGENT:
        agent->agent_type = MC_REMOTE_AGENT;
        if( (err_code = agent_xml_parse(agent))) {
          fprintf(stderr, "error code %d. %s:%d\n",
              err_code, __FILE__, __LINE__ );
          agent_Destroy(agent);
          return NULL;
        }
        if (mc_platform->default_agentstatus != -1) {
          MUTEX_LOCK(agent->agent_status_lock);
          agent->agent_status = (enum MC_AgentStatus_e)mc_platform->default_agentstatus;
          COND_BROADCAST(agent->agent_status_cond);
          MUTEX_UNLOCK(agent->agent_status_lock);
        }
        break;
      case RETURN_MSG:
        agent->agent_type = MC_RETURN_AGENT;
        if( (err_code = agent_xml_parse(agent))) {
          fprintf(stderr, "error code %d. %s:%d\n",
              err_code, __FILE__, __LINE__ );
          agent_Destroy(agent);
          return NULL;
        }
        break;
      default:
        fprintf(stderr, "Invalid agent type: %d %s:%d\n",
            agent->agent_type, __FILE__, __LINE__ );
    }
  } else {
    mc_platform->err = MC_ERR_PARSE;
    /* Free up memory. */
    MUTEX_DESTROY(agent->lock);
    free(agent->lock);
    MUTEX_DESTROY(agent->run_lock);
    free(agent->run_lock);
    MUTEX_DESTROY(agent->agent_status_lock);
    free(agent->agent_status_lock);
    COND_DESTROY(agent->agent_status_cond);
    free(agent->agent_status_cond);

    free(agent);
    return NULL;
  }

  /* In the future we will compare the current tasks server name to 
     the one on the server, presently this is not implemented */

  /* set the CH exectution flag */
  /* FIXME: This should be in the xml parser */
  /*if (agent->datastate->tasks[agent->datastate->task_progress]->init_agent_status != -1) {
    agent->agent_status = agent->datastate->tasks[agent->datastate->task_progress]->init_agent_status;
    } else {
    agent->agent_status = mc_platform->default_agentstatus;
    }
    */
  MUTEX_LOCK(agent->agent_status_lock);
  agent->agent_status = MC_WAIT_CH;
  COND_BROADCAST(agent->agent_status_cond);
  MUTEX_UNLOCK(agent->agent_status_lock);

  agent->mc_platform = mc_platform;

	agent->agent_address = (char*)malloc(sizeof(char) * 
			(strlen(agent->mc_platform->hostname) + 12 + 10)
		);
	if (agent->agent_address == NULL) {
		fprintf(stderr, "Memory error. %s:%d\n", __FILE__, __LINE__);
		exit(-1);
	}
  sprintf(agent->agent_address,
      "http://%s:%d/acc",
      agent->mc_platform->hostname,
      agent->mc_platform->port
      );
  /* return */
  return agent;
}

  int
agent_Destroy(agent_p agent)
{
  if (agent == NULL) {
    return MC_SUCCESS;
  }
  MUTEX_LOCK(agent->lock);
  if (agent->name != NULL) {
    free(agent->name);
  }
  if (agent->owner != NULL) {
    free(agent->owner);
  }
  if (agent->home != NULL) {
    free(agent->home);
  }
  if (agent->sender != NULL) {
    free(agent->sender);
  }
	if (agent->wg_code != NULL) {
		free(agent->wg_code);
	}
  if (agent->agent_address != NULL) {
    free(agent->agent_address);
  }
  /* Terminate the agent datastate memory */
  MUTEX_LOCK(agent->agent_status_lock);
  if (agent->agent_status == MC_AGENT_NEUTRAL) {
    MUTEX_UNLOCK(agent->agent_status_lock);
    if ((agent->agent_interp) != NULL) {
      Ch_Reset(*agent->agent_interp);
      ListWRLock(agent->mc_platform->interpreter_queue);
			ListAdd(agent->mc_platform->interpreter_queue, agent->agent_interp);
      ListWRUnlock(agent->mc_platform->interpreter_queue);
    }
  } else {
    MUTEX_UNLOCK(agent->agent_status_lock);
  }
  MUTEX_DESTROY(agent->agent_status_lock);
  free(agent->agent_status_lock);
  COND_DESTROY(agent->agent_status_cond);
  free(agent->agent_status_cond);

  MUTEX_UNLOCK(agent->lock);
  MUTEX_DESTROY(agent->lock);
  free(agent->lock);
  agent_datastate_Destroy(agent->datastate);
  free(agent->run_lock);
  /* Delete agent mailbox */
  ListWRLock(agent->mailbox);
  ListClearCB(agent->mailbox, (ListElemDestroyFunc_t)fipa_acl_message_Destroy);
  ListWRUnlock(agent->mailbox);
  ListTerminate(agent->mailbox);
  /* Delete agent shared data */
  ListWRLock(agent->agent_share_data_queue);
  ListClearCB(agent->agent_share_data_queue, (ListElemDestroyFunc_t)agent_share_data_Destroy);
  ListWRUnlock(agent->agent_share_data_queue);
  ListTerminate(agent->agent_share_data_queue);
  /* deallocate the agent */
  free(agent);
  agent = NULL;
  return MC_SUCCESS;
}

  extern void 
agent_RunChScript(agent_p agent, mc_platform_p mc_platform)
{
#ifndef _WIN32
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  if(mc_platform->stack_size[MC_THREAD_AGENT] != -1) {
    pthread_attr_setstacksize
      (
       &attr, 
       mc_platform->stack_size[MC_THREAD_AGENT]
      );
  }
  pthread_attr_setdetachstate /* Automatically reclaim thread memory for this thread */
    (
     &attr,
     PTHREAD_CREATE_DETACHED
    );
#else
  int stack_size;
  if (mc_platform->stack_size[MC_THREAD_AGENT] < 1) {
	  stack_size = mc_platform->stack_size[MC_THREAD_AGENT]+1;
  } else {
	  stack_size = mc_platform->stack_size[MC_THREAD_AGENT];
  }
#endif
  MUTEX_LOCK(agent->agent_status_lock);
  agent->agent_status = MC_AGENT_ACTIVE;
  COND_BROADCAST(agent->agent_status_cond);
  MUTEX_UNLOCK(agent->agent_status_lock);
  agent->mc_platform = mc_platform;

  THREAD_CREATE(&agent->agent_thread,
      agent_RunChScriptThread,
      agent );

  return;
}


/* Added for now */
void *
agent_ChScriptInitVar(ChInterp_t* interp)
{
  char *tmp_buf;

  /* Declare special variables for holding the agent id and name */
  tmp_buf = (char*)malloc(sizeof(char) * 200);
  tmp_buf[0] = '\0';
  sprintf(tmp_buf, "int mc_agent_id = -1;");
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  tmp_buf[0] = '\0';
  //sprintf(tmp_buf, "char mc_agent_name[100] = \"agent_name\";");
  sprintf(tmp_buf, "char* mc_agent_name = NULL;");
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  tmp_buf[0] = '\0';
  sprintf(tmp_buf, "void* mc_current_agent = NULL;");
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  tmp_buf[0] = '\0';
  //sprintf(tmp_buf, "char mc_host_name[100] = \"host_name\";");
  sprintf(tmp_buf, "char* mc_host_name = NULL;");
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  tmp_buf[0] = '\0';
  sprintf(tmp_buf, "int mc_host_port = -1;\n");
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  tmp_buf[0] = '\0';
  sprintf(tmp_buf, "int mc_task_progress = -1;\n");
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  tmp_buf[0] = '\0';
  sprintf(tmp_buf, "int mc_num_tasks = -1;\n");
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  /* Declare standard error code enum */
  tmp_buf[0] = '\0';
  sprintf(tmp_buf, "enum error_code_e {MC_SUCCESS = 0, MC_ERR, MC_ERR_CONNECT, MC_ERR_PARSE, MC_ERR_EMPTY, MC_ERR_INVALID, MC_ERR_INVALID_ARGS, MC_ERR_NOT_FOUND, MC_ERR_MEMORY, MC_ERR_SEND, MC_WARN_DUPLICATE };" );
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  Ch_DeclareVar(
      *interp,
      "char* mc_agent_address;"
      );


  tmp_buf[0] = '\0';
  sprintf(tmp_buf, "enum MC_SteerCommand_e {MC_RUN = 0, MC_SUSPEND, MC_RESTART, MC_STOP};" );
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  tmp_buf[0] = '\0';
  sprintf(tmp_buf, "enum mc_AgentStatus_e { MC_WAIT_CH, MC_WAIT_MESSGSEND, MC_AGENT_ACTIVE, MC_AGENT_NEUTRAL, MC_AGENT_SUSPENDED, MC_WAIT_FINISHED};"); 
  Ch_DeclareVar(
      *interp,
      tmp_buf
      );

  free(tmp_buf);
  /* Add the MCAgent_t typedef */
  Ch_DeclareVar(
      *interp,
      "void* MCAgent_t;"
      );
  Ch_DeclareTypedef(
      *interp,
      "MCAgent_t"
      );

  /* Following are the declarations of the agent-space api functions. */
  Ch_DeclareFunc(
      *interp,
      "int mc_AclDestroy(void* acl_message);",
      (ChFuncdl_t)MC_AclDestroy_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "void* mc_AclNew(void);",
      (ChFuncdl_t)MC_AclNew_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclPost(void* agent, void* acl_message);",
      (ChFuncdl_t)MC_AclPost_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "void* mc_AclRetrieve(void* agent);",
      (ChFuncdl_t)MC_AclRetrieve_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "void* mc_AclReply(void* acl_message);",
      (ChFuncdl_t)MC_AclReply_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclSend(void* acl_message);",
      (ChFuncdl_t)MC_AclSend_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "void* mc_AclWaitRetrieve(void* agent);",
      (ChFuncdl_t)MC_AclWaitRetrieve_chdl
      );
  /* begin Acl Helper Functions */
  Ch_DeclareFunc(
      *interp,
      "int mc_AclGetProtocol(void* acl_message);",
      (ChFuncdl_t)MC_AclGetProtocol_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "char* mc_AclGetConversationID(void* acl_message);",
      (ChFuncdl_t)MC_AclGetConversationID_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclGetPerformative(void* acl_message);",
      (ChFuncdl_t)MC_AclGetPerformative_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclGetSender(void* acl_message, char** name, char** address);",
      (ChFuncdl_t)MC_AclGetSender_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "char* mc_AclGetContent(void* acl_message);",
      (ChFuncdl_t)MC_AclGetContent_chdl
      );

  Ch_DeclareFunc(
      *interp,
      "int mc_AclSetProtocol(void* acl_message, int protocol);",
      (ChFuncdl_t)MC_AclSetProtocol_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclSetConversationID(void* acl_message, char* id);",
      (ChFuncdl_t)MC_AclSetConversationID_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclSetPerformative(void* acl_message, int performative);",
      (ChFuncdl_t)MC_AclSetPerformative_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclSetSender(void* acl_message, char* name, char* address);",
      (ChFuncdl_t)MC_AclSetSender_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclAddReceiver(void* acl_message, char* name, char* address);",
      (ChFuncdl_t)MC_AclAddReceiver_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclAddReplyTo(void* acl_message, char* name, char* address);",
      (ChFuncdl_t)MC_AclAddReplyTo_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AclSetContent(void* acl_message, char* content);",
      (ChFuncdl_t)MC_AclSetContent_chdl
      );
  /* end Acl Helper Functions */
  Ch_DeclareFunc(
      *interp,
      "int mc_AddAgent(void* agent);",
      (ChFuncdl_t)MC_AddAgent_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AgentAddTask(void* agent, const char* code, const char* return_var_name, const char* server, int persistent);",
      (ChFuncdl_t)MC_AgentAddTask_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AgentAttachFile(void* agent, const char* name, const char* filepath);",
      (ChFuncdl_t)MC_AgentAttachFile_chdl
      );
  if(sizeof(size_t) == 8) {
    Ch_DeclareFunc(
        *interp,
        "int mc_AgentDataShare_Add(void* agent, const char* name, const void* data, unsigned long long size);",
        (ChFuncdl_t)MC_AgentDataShare_Add_chdl
        );
    Ch_DeclareFunc(
        *interp,
        "int mc_AgentDataShare_Retrieve(void* agent, const char* name, const void** data, unsigned long long *size);",
        (ChFuncdl_t)MC_AgentDataShare_Retrieve_chdl
        );
  } else {
    Ch_DeclareFunc(
        *interp,
        "int mc_AgentDataShare_Add(void* agent, const char* name, const void* data, unsigned int size);",
        (ChFuncdl_t)MC_AgentDataShare_Add_chdl
        );
    Ch_DeclareFunc(
        *interp,
        "int mc_AgentDataShare_Retrieve(void* agent, const char* name, const void** data, unsigned int* size);",
        (ChFuncdl_t)MC_AgentDataShare_Retrieve_chdl
        );
  }
  Ch_DeclareFunc(
      *interp,
      "int mc_AgentListFiles(void* agent, int task_num, char*** names, int* num_files);",
      (ChFuncdl_t)MC_AgentListFiles_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AgentRetrieveFile(void* agent, int task_num, const char* name, const char* save_path);",
      (ChFuncdl_t)MC_AgentRetrieveFile_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "const void* mc_AgentVariableRetrieve(void* agent, const char* var_name, int task_num);",
      (ChFuncdl_t)MC_AgentVariableRetrieve_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_AgentVariableSave(void* agent, const char* var_name);",
      (ChFuncdl_t)MC_AgentVariableSave_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_Barrier(int id);",
      (ChFuncdl_t)MC_Barrier_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_BarrierDelete(int id);",
      (ChFuncdl_t)MC_BarrierDelete_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_BarrierInit(int id, int num_procs);",
      (ChFuncdl_t)MC_BarrierInit_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_CallAgentFunc(char* agentName, const char* funcName, void* returnVal,  ...);",
      (ChFuncdl_t)MC_CallAgentFunc_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "MCAgent_t mc_ComposeAgent(const char* name, *home, *owner, *code, *return_var_name, *server, int persistent);",
      (ChFuncdl_t)MC_ComposeAgent_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "MCAgent_t mc_ComposeAgentS(const char* name, *home, *owner, *code, *return_var_name, *server, *workgroup_code, int persistent);",
      (ChFuncdl_t)MC_ComposeAgentWithWorkgroup_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "MCAgent_t mc_ComposeAgentWithWorkgroup(const char* name, *home, *owner, *code, *return_var_name, *server, *workgroup_code, int persistent);",
      (ChFuncdl_t)MC_ComposeAgentWithWorkgroup_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "MCAgent_t mc_ComposeAgentFromFile(const char* name, *home, *owner, *filename, *return_var_name, *server, int persistent);",
      (ChFuncdl_t)MC_ComposeAgentFromFile_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "MCAgent_t mc_ComposeAgentFromFileWithWorkgroup(const char* name, *home, *owner, *filename, *return_var_name, *server, *workgroup_code, int persistent);",
      (ChFuncdl_t)MC_ComposeAgentFromFileWithWorkgroup_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_CondBroadcast(int id);",
      (ChFuncdl_t)MC_CondBroadcast_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_CondSignal(int id);",
      (ChFuncdl_t)MC_CondSignal_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_CondReset(int id);",
      (ChFuncdl_t)MC_CondReset_chdl 
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_CondWait(int id);",
      (ChFuncdl_t)MC_CondWait_chdl 
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_DeleteAgent(const char* agentName);",
      (ChFuncdl_t)MC_DeleteAgent_chdl 
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_DeleteAgentWG(MCAgent_t calling_agent, const char* agentName);",
      (ChFuncdl_t)MC_DeleteAgentWG_chdl 
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_DeregisterService(int agentID, char* serviceName);",
      (ChFuncdl_t)MC_DeregisterService_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_DestroyServiceSearchResult( char** agentName, char** serviceName, int* agentID, int numResult);",
      (ChFuncdl_t)MC_DestroyServiceSearchResult_chdl 
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_End(void);",
      (ChFuncdl_t)MC_End_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "void *mc_FindAgentByID(int id);",
      (ChFuncdl_t)MC_FindAgentByID_chdl
      );
  /* FIXME: This function will probably be deprecated for security reasons */
  Ch_DeclareFunc(
      *interp,
      "void *mc_FindAgentByName(const char *name);",
      (ChFuncdl_t)MC_FindAgentByName_chdl
      );
  /* FIXME -- This block of code does not work: Ch does not
   * understand 'time_t' */
  /*
  Ch_DeclareFunc(
      *interp,
      "time_t mc_GetAgentArrivalTime(void* agent);",
      (ChFuncdl_t)MC_GetAgentArrivalTime_chdl
      );
      */
  Ch_DeclareFunc(
      *interp,
      "int MC_GetAgentID(void* agent);",
      (ChFuncdl_t)MC_GetAgentStatus_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "char* MC_GetAgentName(void* agent);",
      (ChFuncdl_t)MC_GetAgentStatus_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_GetAgentStatus(void* agent);",
      (ChFuncdl_t)MC_GetAgentStatus_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "char *mc_GetAgentXMLString(void* agent);",
      (ChFuncdl_t)MC_GetAgentXMLString_chdl
      );

#ifndef _WIN32
  Ch_DeclareFunc(
      *interp,
      "int mc_gettimeofday(void* tv);",
      (ChFuncdl_t)MC_GetTimeOfDay_chdl
      );
#endif

  Ch_DeclareFunc(
      *interp,
      "int mc_HaltAgency(void);",
      (ChFuncdl_t)MC_HaltAgency_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_MigrateAgent(void* agent, const char* hostname, int port);",
      (ChFuncdl_t)MC_MigrateAgent_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_MutexLock(int id);",
      (ChFuncdl_t)MC_MutexLock_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_MutexUnlock(int id);",
      (ChFuncdl_t)MC_MutexUnlock_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_PrintAgentCode(void* agent);",
      (ChFuncdl_t)MC_PrintAgentCode_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_ResumeAgency(void);",
      (ChFuncdl_t)MC_ResumeAgency_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SearchForService(const char* searchString, char*** agentNames, char*** serviceNames, int** agentIDs, int* numResults);",
      (ChFuncdl_t)MC_SearchForService_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SendSteerCommand(enum MC_SteerCommand_e command);",
      (ChFuncdl_t)MC_SendSteerCommand_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_RegisterService(MCAgent_t agent, char **serviceNames, int numServices);",
      (ChFuncdl_t)MC_RegisterService_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "void *mc_RetrieveAgent(void);",
      (ChFuncdl_t)MC_RetrieveAgent_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "char *mc_RetrieveAgentCode(void* agent);",
      (ChFuncdl_t)MC_RetrieveAgentCode_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SaveData(MCAgent_t agent, char* name, int size, void* data);",
      (ChFuncdl_t)MC_SaveData_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SemaphoreWait(int id);",
      (ChFuncdl_t)MC_SemaphoreWait_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SemaphorePost(int id);",
      (ChFuncdl_t)MC_SemaphorePost_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SendAgentMigrationMessage(char *message, char *hostname, int port);",
      (ChFuncdl_t)MC_SendAgentMigrationMessage_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SendAgentMigrationMessageFile(char *filename, char *hostname, int port);",
      (ChFuncdl_t)MC_SendAgentMigrationMessageFile_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SetAgentStatus(void* agent, int status);",
      (ChFuncdl_t)MC_SetAgentStatus_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SetDefaultAgentStatus(int status);",
      (ChFuncdl_t)MC_SetDefaultAgentStatus_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SyncDelete(int id);",
      (ChFuncdl_t)MC_SyncDelete_chdl 
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_SyncInit(int id);",
      (ChFuncdl_t)MC_SyncInit_chdl 
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_TerminateAgent(const char* agentName);",
      (ChFuncdl_t)MC_TerminateAgent_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_TerminateAgentWG(void* callingAgent, const char* agentName);",
      (ChFuncdl_t)MC_TerminateAgentWG_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "int mc_GetAgentID(void* agent);",
      (ChFuncdl_t)MC_GetAgentID_chdl
      );
  Ch_DeclareFunc(
      *interp,
      "char *mc_GetAgentName(void* agent);",
      (ChFuncdl_t)MC_GetAgentName_chdl
      );

  return NULL;
}

#ifndef _WIN32
  void* 
agent_RunChScriptThread(void * ChAgent)
#else
  DWORD WINAPI 
agent_RunChScriptThread(void* ChAgent)
#endif
{
#ifndef _WIN32
  int fd;
#endif
  MCAgent_t agent;
  mc_platform_p mc_platform;
  int i,n;
  FILE *TEMP_FILE;
  char *temp_store_file;
  char *ChShellArg[2];
  void *result;
  int progress;
  int callbackErrCode;
  int persistent = 0;

  /* set up the agent object */
  agent = (MCAgent_t)ChAgent;
  progress = agent->datastate->task_progress;
  mc_platform = agent->mc_platform;

  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  /* check to see if the agent is null */
  if(ChAgent == NULL)
  {
    printf("ERROR, AGENT NULL \n");
#ifndef _WIN32
    return NULL;
#else
    return 0;
#endif
  }

  /* Get an interpreter from the list of agency interpreters */
  agent->agent_interp = (ChInterp_t *)interpreter_queue_CreateRetrieve(mc_platform->interpreter_queue,
		agent->mc_platform->interp_options );
  if(agent->agent_interp == NULL) {
    /* Could not get a new interpreter. Abort. */
    WARN("Could not initialize another Ch interperter. Please make more copies of the chmt*.dl file.\n");
    return NULL;
  }

  /* Setup the global variables */
  //printf("mc_agent_id\n");
  Ch_SetVar(*agent->agent_interp, "mc_agent_id",   
      CH_INTTYPE, (int)agent->id);
  //printf("mc_agent_name\n");
  Ch_SetVar(*agent->agent_interp, "mc_agent_name", 
      CH_CHARPTRTYPE, agent->name);
  //printf("mc_current_agent\n");
  Ch_SetVar(*agent->agent_interp, "mc_current_agent", 
      CH_VOIDPTRTYPE, (void*)((size_t)agent)); 
  //printf("mc_host_name\n");
  Ch_SetVar(*agent->agent_interp, "mc_host_name", 
      CH_CHARPTRTYPE, agent->mc_platform->hostname);
  //printf("mc_host_port\n");
  Ch_SetVar(*agent->agent_interp, "mc_host_port",   
      CH_INTTYPE, (int)agent->mc_platform->port);
  //printf("mc_task_progress\n");
  Ch_SetVar(*agent->agent_interp, "mc_task_progress",   
      CH_INTTYPE, (int)agent->datastate->task_progress);
  //printf("mc_num_tasks\n");
  Ch_SetVar(*agent->agent_interp, "mc_num_tasks",   
      CH_INTTYPE, (int)agent->datastate->number_of_tasks);

	Ch_SetVar(*agent->agent_interp, "mc_agent_address",
			CH_CHARPTRTYPE, agent->agent_address);

  /* Originally, we hope to use append runscript if the buffer for code is less 
     than 51 bytes. Otherwise we must open a file and save the data to the 
     filename. However, since the mobile agent codes for testing are complete 
     programs which contain preprocessing directives like "#include" and are 
     larger than 51 bytes, we use the following statement to save the data to a 
     file instead of putting it into the buffer. 

     FIXME: This is silly
  */
  if(strlen(agent->datastate->agent_code) < 5)
  {
    MUTEX_LOCK(agent->run_lock);
    if(Ch_AppendRunScript(
          *((MCAgent_t)ChAgent)->agent_interp, 
          ((MCAgent_t)ChAgent)->datastate->agent_code))
    {
      /* DEBUG */
      printf("CH Failure \n");
      printf("***************************************************\nCode was:\n%s\n\n", agent->datastate->agent_code);
      //exit(EXIT_FAILURE);
    }
    if(Ch_CallFuncByName(*((MCAgent_t)ChAgent)->agent_interp, "main", NULL))
    {
      printf("CH2 failure \n");
      exit(EXIT_FAILURE);
    } 
    MUTEX_UNLOCK(agent->run_lock);
  }
  else
  {
    /* If there is a callback registered, we must call it and inspect the
     * result. */
    if(mc_platform->agency->agentInitCallback) {
      callbackErrCode = (mc_platform->agency->agentInitCallback)(
          *agent->agent_interp,
          (struct agent_s*)agent,
          mc_platform->agency->agentInitUserData );
      if(callbackErrCode) {
        /* Clean up and exit the thread */
        ((MCAgent_t) ChAgent)->agent_status = MC_AGENT_NEUTRAL;
#ifndef _WIN32
        pthread_exit(ChAgent);  
#else
        return 0;
#endif
      }
    }

#ifndef _WIN32
    /* save the agent to an external file %agentname%%d%d */
    temp_store_file = (char *)malloc(sizeof(char)*30);

    strcpy(temp_store_file, "agentchscriptXXXXXX");
    fd = mkstemp(temp_store_file);
    if (fd == -1) {
      fprintf(stderr, "Could not create temporary file:%s. %s:%d\n",
          temp_store_file,
          __FILE__,
          __LINE__ );
      exit(EXIT_FAILURE);
    }
    close(fd);
#else
    temp_store_file = _tempnam(".", "agentchscript");
#endif
    TEMP_FILE = fopen(temp_store_file, "w");
    
    /* write the data to a ch-file */
    n = fwrite(
        (void *)agent->datastate->agent_code, 
        sizeof(char), 
        strlen(agent->datastate->agent_code), 
        TEMP_FILE);

    fclose(TEMP_FILE);

    /* set the Ch Shell arguments as appropriate */
    ChShellArg[0] = temp_store_file;
    ChShellArg[1] = NULL;
    MUTEX_LOCK(agent->run_lock);
    if(Ch_RunScript(*agent->agent_interp, ChShellArg) < 0) {
      fprintf(stderr, "Ch_RunScript error. %s:%d\n", __FILE__, __LINE__);
    } else {
      /* Flush the output buffer, in case the agent printed anything */
      fflush(stdout);
    }

    /* if(Ch_CallFuncByName(((MCAgent_t)ChAgent)->agent_interp, "main", NULL))
       {
         printf("CH2 failure \n");
         exit(EXIT_FAILURE);
       } */

    /* remove the temp file after its usage */
    remove(temp_store_file);
#ifndef _WIN32
    free(temp_store_file);
#endif
    MUTEX_UNLOCK(agent->run_lock);
  }

  /* now add the data element returned from the Ch execution into the agent data structure */
  if(
      agent->datastate->tasks[progress]->var_name != NULL &&
      strcmp(agent->datastate->tasks[progress]->var_name, "no-return")
    )
  {
    result = interpreter_variable_data_InitializeFromAgent(agent);
    /* Free old result extracted from XML agent */
    interpreter_variable_data_Destroy(
        agent->datastate->tasks[progress]->agent_return_data
        );
    /* Replace with new freshly calculated one */
    agent->datastate->tasks[progress]->agent_return_data = 
      (interpreter_variable_data_t*)result;
  } else {
    interpreter_variable_data_Destroy(
        agent->datastate->tasks[progress]->agent_return_data );
    agent->datastate->tasks[progress]->agent_return_data = NULL;
  }

  /* This is where we want to save all of the agent variables that the agent
   * wishes to keep for its trip. */
  /* First, empty the list */
  ListClearCB(
      agent->datastate->tasks[progress]->agent_variable_list,
      (ListElemDestroyFunc_t) interpreter_variable_data_Destroy);

  for(i = 0; i < agent->datastate->tasks[progress]->num_saved_variables; i++) {
	/*agent_variable_list_Remove(
        agent->datastate->tasks[progress]->agent_variable_list,
	agent->datastate->tasks[progress]->saved_variables[i]
	); */ /* FIXME: Why doesn't this work? */
    ListWRLock(agent->datastate->tasks[progress]->agent_variable_list);
    ListAdd(
        agent->datastate->tasks[progress]->agent_variable_list,
        interpreter_variable_data_Initialize(
          agent,
          agent->datastate->tasks[progress]->saved_variables[i] )
        );
    ListWRUnlock(agent->datastate->tasks[progress]->agent_variable_list);
  }

  /* Apply progress modifier */
  agent->datastate->task_progress += agent->datastate->progress_modifier;

  if (agent->datastate->persistent || 
      agent->datastate->tasks[progress]->persistent ) {
    persistent = 1;
    /* TODO: We need a large while loop here that waits on a condition 
       variable. Upon waking up, we will need to check a 'mailbox' for
       a struct containing
       1. A message/command
       2. A void* to generic data
       3. The size of the data.
       It should then execute the command, and check to see if the
       persistent flag is still set. If it is, loop again. 
       */
    /* For now, let us just not end the Ch interpreter and set the 
     * agent_status to MC_AGENT_NEUTRAL to cause it to hang. */
    ((MCAgent_t) ChAgent)->agent_status = MC_AGENT_NEUTRAL;
  } else {
    if ((((MCAgent_t)ChAgent)->agent_interp) != NULL) {
      /* Reset the interpreter and put it back into the queue */
      Ch_Reset(*agent->agent_interp);
      ListWRLock(mc_platform->interpreter_queue);
      ListAdd(mc_platform->interpreter_queue, agent->agent_interp);
      ListWRUnlock(mc_platform->interpreter_queue);
    }

    /* Perform some housekeeping regarding agent status */
    if (
        (agent->datastate->task_progress ==
         (agent->datastate->number_of_tasks-1))
       ) 
    {
      /* If the agent is done... */
      ((MCAgent_t) ChAgent)->agent_status = MC_WAIT_FINISHED;
      /* If _any_ of the tasks have return data, we should generate
       * a return message. */
      for(i = 0; 
          i < agent->datastate->number_of_tasks;
          i++)
      {
        if (agent->datastate->tasks[i]->agent_return_data != NULL) {
          ((MCAgent_t) ChAgent)->agent_status = MC_WAIT_MESSGSEND;
        }
      }
    }
    else {
      ((MCAgent_t) ChAgent)->agent_status = MC_WAIT_MESSGSEND;
    }
  }

  /* Increment task progress */
  agent->datastate->task_progress++;

  if (
      (agent->datastate->task_progress >= agent->datastate->number_of_tasks)
     )
  {
    agent->agent_type = MC_RETURN_AGENT;
  }

  SIGNAL(
      mc_platform->MC_signal_cond,
      mc_platform->MC_signal_lock,
      mc_platform->MC_signal = MC_EXEC_AGENT;
      );

  MUTEX_LOCK( mc_platform->MC_signal_lock);
  MUTEX_UNLOCK( mc_platform->MC_signal_lock );
  MUTEX_LOCK(mc_platform->ams->runflag_lock);
  mc_platform->ams->run = 1;
  COND_SIGNAL(mc_platform->ams->runflag_cond);
  MUTEX_UNLOCK(mc_platform->ams->runflag_lock);
  
  if(persistent)
  {
    /* If the agent was persistent, we do not want to terminate the thread
     * until the agent is purged. */
    MUTEX_LOCK(agent->agent_status_lock);
    while(agent->agent_status == MC_AGENT_NEUTRAL) {
      COND_WAIT(agent->agent_status_cond, agent->agent_status_lock);
    }
    MUTEX_UNLOCK(agent->agent_status_lock);
  }
#ifndef _WIN32
  pthread_exit(ChAgent);  
#else
  return 0;
#endif
}

int agent_Print(agent_t* agent) {
    MUTEX_LOCK(agent->agent_status_lock);
    printf("Agent id: %lu, Connect id: %lu, status: %u\n",
        agent->id,
        agent->connect_id,
        agent->agent_status);
    MUTEX_UNLOCK(agent->agent_status_lock);
    return 0;
}

int agent_CmpName(const void* key, void* element)
{
  const char* name = (const char*)key;
  agent_t* agent = (agent_t*)element;
  int ret;
  MUTEX_LOCK(agent->lock);
  ret = strcmp(name, agent->name);
  MUTEX_UNLOCK(agent->lock);
  return ret;
}

int agent_CmpID(int* id, agent_t* agent) 
{
  int ret;
  MUTEX_LOCK(agent->lock);
  ret = *id - agent->id;
  MUTEX_UNLOCK(agent->lock);
  return ret;
}

