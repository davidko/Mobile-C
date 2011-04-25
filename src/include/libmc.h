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

/**
 * \file      libmc.h
 * \brief     MobileC api header file
 */

/*!
 * \mainpage MobileC
 * \section intro_sec Introduction
 * Welcome to the MobileC documentation. For a quick user-api reference, please
 * refer to the file libmc.h.
 *
 * This documentation is provided supplementary to the main Mobile-C User Guide. 
 * The official user guide may be obtained at http://www.mobilec.org , and should
 * also be included with your Mobile-C Distribution package in the file
 * docs/mobilec.pdf .
 */

/* Filename: libmc.h */
/* Current functions implemented 
MC_AclDestroy
MC_AclNew
MC_AclPost
MC_AclReply
MC_AclRetrieve
MC_AclSend
MC_AclWaitRetrieve
MC_AddAgent 
MC_Barrier 
MC_BarrierInit 
MC_BarrierDelete 
MC_CallAgentFunc
MC_ChInitializeOptions 
MC_ComposeAgent
MC_CondBroadcast 
MC_CondSignal 
MC_CondWait  
MC_CondReset 
MC_CopyAgent 
MC_DeleteAgent 
MC_DeregisterService
MC_End 
MC_FindAgentByName
MC_FindAgentByID
MC_GetAgentArrivalTime 
MC_GetAgentStatus 
MC_GetAgentXMLString 
MC_GetAgentExecEngine 
MC_GetAgentID
MC_GetAgentName
MC_GetAgentReturnData
MC_GetAgentNumTasks 
MC_GetAgentType 
MC_GetAllAgents 
MC_HaltAgency 
MC_Initialize
MC_InitializeAgencyOptions 
MC_MutexLock 
MC_MutexUnlock 
MC_PrintAgentCode 
MC_RegisterService
MC_ResumeAgency 
MC_RetrieveAgent 
MC_RetrieveAgentCode 
MC_ResetSignal 
MC_SearchForService
MC_SemaphorePost 
MC_SemaphoreWait 
MC_SendCh
MC_SendAgentMigrationMessage
MC_SendAgentMigrationMessageFile
MC_SendSteerCommand 
MC_SetAgentStatus 
MC_SetDefaultAgentStatus
MC_SetThreadOn 
MC_SetThreadsAllOn
MC_SetThreadOff 
MC_SetThreadsAllOff
MC_Steer
MC_SteerControl 
MC_SyncDelete 
MC_SyncInit 
MC_TerminateAgent 
MC_MainLoop 
MC_MainLoop
MC_WaitAgent 
MC_WaitRetrieveAgent 
MC_WaitSignal 

   *** */

#ifndef _MOBILEC_H_
#define _MOBILEC_H_

#ifdef _WIN32
#include<windows.h>
	#ifdef _MC_DLL
		// Building a .dll - export functions as marked
		#define EXPORTMC _declspec(dllexport)
	#else
		// Not building a .dll - building .lib assumed - no export required
		#define EXPORTMC
	#endif
#else
	// Not windows
	#define EXPORTMC
#endif

/* The MC_Wait() function is deprecated as of version 1.9.4.
 * All calls to MC_Wait() should now be changed to MC_MainLoop() */
#define MC_Wait(arg1) \
  MC_MainLoop(arg1)

#include <embedch.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Mobile-C Agent Initialization callback function */
/* Should return 0 on success, non-zero on failure */
struct agent_s;
typedef int (*MC_AgentInitCallbackFunc_t)(
    ChInterp_t interp, 
    struct agent_s* agent, 
    void* user_data);

#ifndef _ERROR_CODE_E_
#define _ERROR_CODE_E_
/*
 * Error Codes
 * NOTE: There is a copy of this enum in mc_error.h
 * */
typedef enum error_code_e {
    MC_SUCCESS = 0,
    MC_ERR,
    MC_ERR_CONNECT,
    MC_ERR_PARSE,
    MC_ERR_EMPTY,
    MC_ERR_INVALID,
    MC_ERR_INVALID_ARGS,
    MC_ERR_NOT_FOUND,
    MC_ERR_MEMORY,
    MC_ERR_SEND,
    MC_WARN_DUPLICATE
} error_code_t;
#endif

/**
 * \brief     MobileC Module indices
 */
enum MC_ThreadIndex_e{ 
    MC_THREAD_DF=0,   /*!< Directory Facilitator */
    MC_THREAD_AMS,    /*!< Agent Managment system */
    MC_THREAD_ACC,    /*!< Agency communications */
    MC_THREAD_CP,     /*!< Command Prompt */
    MC_THREAD_AGENT,  /*!< Agent threads */
    MC_THREAD_ALL };


/**
 * \brief     Available commands for MC_Steer
 */
typedef enum MC_SteerCommand_e {
  MC_RUN = 0,     /*!< Continue the algorithm */
  MC_SUSPEND,     /*!< Suspend/pause the algorithm */
  MC_RESTART,     /*!< Restart the algorithm from the beginning */
  MC_STOP         /*!< Stop the algorithm */
} MC_SteerCommand_t;

/**
 * \brief     MobileC system signals.
 *
 * \note      Each signal is activated after the corresponding action. i.e. The
 *            'MC_RECV_MESSAGE' signal is activated after a message is received.
 *
 * \sa        MC_WaitSignal(), MC_ResetSignal
 */
enum MC_Signal_e{
    MC_NO_SIGNAL      =0x00, /*!< No signal hass been received */
    MC_RECV_CONNECTION=0x01, /*!< A Connection has been received */
    MC_RECV_MESSAGE   =0x02, /*!< A Message has been received from a connection */
    MC_RECV_AGENT     =0x04, /*!< An normal agent has been parsed from a message */
    MC_RECV_RETURN    =0x08, /*!< A return agent has been parsed from a message */
    MC_EXEC_AGENT     =0x10, /*!< A normal agent has been executed. */
    MC_ALL_SIGNALS    =0x20  /*!< Catch any of the above signals */
};

/**
 * \brief     The agency handle
 */
typedef struct agency_s {
    int client;
    int server;
    char *hostName;                     /*!< Local Hostname */
    char *filename;
    int portno;                         /*!< Local port number */
    int portnoc;
    int initInterps;
    struct mc_platform_s* mc_platform;  /*!< Local MobileC Platform */
    int default_agentstatus;            /*!< Agency default agent status */
    int threads;                        /*!< flag which determines which threads to start */
    int enable_security;                /*!< Security flag */
    int stack_size[MC_THREAD_ALL];
    int bluetooth;

		char* priv_key_filename;
		char* known_host_filename;
    error_code_t last_error;

    /* Agent init callback stuff */
    MC_AgentInitCallbackFunc_t agentInitCallback;
    void* agentInitUserData;
} agency_t;
typedef agency_t* agency_p;
typedef agency_p MCAgency_t;

/**
 * \brief User modifiable agency options 
 */
typedef struct MCAgencyOptions_s{
    int threads;              /*!< Threads to start */
    int default_agent_status; /*!< Default agent status */
    int modified;             /*!< unused */
    int enable_security;      /*!< security enable flag */
    unsigned char passphrase[32]; /*!< security enable flag */

    /* Following are some thread stack size options: unix/pthreads only! */
    int stack_size[MC_THREAD_ALL]; /*!< If the stack size is -1, use system def.*/

    char *known_host_filename;
    char *priv_key_filename;
    int initInterps;
    int bluetooth; /* Startup in bluetooth mode */
    ChOptions_t* ch_options;

    unsigned long long initialized; /* This will contain a magic number if the struct
    has been initialized. The magic number is kept in the define
    MC_INITIALIZED_CODE */
} MCAgencyOptions_t;
#define MC_INITIALIZED_CODE 63280583

typedef struct stationary_agent_info_s{
	void* args;
	struct agent_s* agent;
	MCAgency_t attr; /* DEPRECATED: Use the 'agency' member instead */
	MCAgency_t agency;
#ifdef _WIN32
	HANDLE thread;
#else
  pthread_t thread;
#endif
} stationary_agent_info_t;

#ifndef AGENT_T
#define AGENT_T
typedef struct agent_s agent_t;
typedef agent_t* MCAgent_t;
typedef agent_t* agent_p;
#endif

enum MC_AgentType_e{ MC_NONE = -1, MC_REMOTE_AGENT = 0, MC_LOCAL_AGENT, MC_RETURN_AGENT };

/**
 * \brief         An agent's current execution state
 */
enum MC_AgentStatus_e{ 
	MC_NO_STATUS = -1,
    MC_WAIT_CH = 0,    /*!< Waiting to be started */
    MC_WAIT_MESSGSEND, /*!< Finished, waiting to migrate */
    MC_AGENT_ACTIVE,   /*!< Running */
    MC_AGENT_NEUTRAL,  /*!< Not running, but do not flush */
    MC_AGENT_SUSPENDED,/*!< Unused */
    MC_WAIT_FINISHED   /*!< Finished, waiting to be flushed */
};

/** FIPA ACL **/
struct fipa_acl_message_s;
#ifndef _FIPA_PERFORMATIVE_E_
#define _FIPA_PERFORMATIVE_E_
enum fipa_performative_e
{
  FIPA_ERROR=-1,
  FIPA_ZERO,
  FIPA_ACCEPT_PROPOSAL,
  FIPA_AGREE,
  FIPA_CANCEL,
  FIPA_CALL_FOR_PROPOSAL,
  FIPA_CONFIRM,
  FIPA_DISCONFIRM,
  FIPA_FAILURE,
  FIPA_INFORM,
  FIPA_INFORM_IF,
  FIPA_INFORM_REF,
  FIPA_NOT_UNDERSTOOD,
  FIPA_PROPOGATE,
  FIPA_PROPOSE,
  FIPA_PROXY,
  FIPA_QUERY_IF,
  FIPA_QUERY_REF,
  FIPA_REFUSE,
  FIPA_REJECT_PROPOSAL,
  FIPA_REQUEST,
  FIPA_REQUEST_WHEN,
  FIPA_REQUEST_WHENEVER,
  FIPA_SUBSCRIBE
};
#endif

/**
 * \brief         Destroy a FIPA ACL message
 *
 * \param message The ACL message to destroy
 *
 * \return        0 on success, error code on failure.
 */
EXPORTMC int 
MC_AclDestroy(struct fipa_acl_message_s* message);

/**
 * \brief         Allocate a new ACL Message
 *
 * \return        A newly allocated and empty ACL message.
 */
EXPORTMC extern struct fipa_acl_message_s*
MC_AclNew(void);

/**
 * \brief         Post ACL message to agent
 *
 * \param agent   The agent to post the message to
 * \param message The message to post
 *
 * \return        0 if successful, or error_code_t type.
 */
EXPORTMC extern int MC_AclPost(MCAgent_t agent, struct fipa_acl_message_s* message);

/**
 * \brief         Reply to an ACL message
 *
 * \param acl_message The incoming acl message to reply to
 *
 * \return        A newly allocated ACL message
 *
 * \note          This function simply generates a new ACL message with the 'receiver' field
 *                automatically set to the 'sender' field of the incoming message.
 */
EXPORTMC extern struct fipa_acl_message_s*
MC_AclReply(struct fipa_acl_message_s* acl_message);

/**
 * \brief         Retrieve an ACL message
 *
 * \param agent   Agent to retrieve message from.
 * 
 * \return        an ACL message struct on success or NULL on failure
 */
EXPORTMC extern struct fipa_acl_message_s* MC_AclRetrieve(MCAgent_t agent);

/**
 * \brief         Send a composed ACL Message
 *
 * \param attr    An initialized and running MobileC agency
 * \param acl     An allocated and fully composed ACL message.
 *
 * \return        0 if successful, error code on failure.
 */
EXPORTMC extern int MC_AclSend(MCAgency_t attr, struct fipa_acl_message_s* acl);

/**
 * \brief         Wait for and retrieve an ACL message
 *
 * \param agent   Agent to retrieve message from.
 *
 * \return        an ACL message struct on success or NULL on failure
 */
EXPORTMC extern struct fipa_acl_message_s* MC_AclWaitRetrieve(MCAgent_t agent);

/* ACL Helper Functions Here */
enum fipa_performative_e;
enum fipa_protocol_e;

EXPORTMC enum fipa_protocol_e MC_AclGetProtocol(
    struct fipa_acl_message_s* acl);

EXPORTMC char* MC_AclGetConversationID(
    struct fipa_acl_message_s* acl);

EXPORTMC enum fipa_performative_e MC_AclGetPerformative(
    struct fipa_acl_message_s* acl);

EXPORTMC int MC_AclGetSender(
    struct fipa_acl_message_s* acl,
    char** name, /* OUT: Will allocate a text string to be freed by the
                          user. */
    char** address /* OUT: Will allocate a text string to be freed by the user
                    */
    );
  
EXPORTMC const char* MC_AclGetContent(
    struct fipa_acl_message_s* acl);

EXPORTMC int MC_AclSetProtocol(
    struct fipa_acl_message_s* acl,
    enum fipa_protocol_e performative );

EXPORTMC int MC_AclSetConversationID(
    struct fipa_acl_message_s* acl,
    const char* id);

EXPORTMC int MC_AclSetPerformative(
    struct fipa_acl_message_s* acl,
    enum fipa_performative_e performative );

EXPORTMC int MC_AclSetSender(
    struct fipa_acl_message_s* acl,
    const char* name,
    const char* address );
  
EXPORTMC int MC_AclAddReceiver(
    struct fipa_acl_message_s* acl,
    const char* name,
    const char* address );

EXPORTMC int MC_AclAddReplyTo(
    struct fipa_acl_message_s* acl,
    const char* name,
    const char* address);

EXPORTMC int MC_AclSetContent(
    struct fipa_acl_message_s* acl,
    const char* content );

/* End ACL Helper Functions */


/**
 * \brief         Add an agent to the agency 'attr'
 *
 * \param attr    a MobileC agency
 * \param agent   An initialized MobileC agent
 *
 * \return        0 if successful, or error_code_t type
 */
EXPORTMC extern int MC_AddAgent( 
    MCAgency_t attr, 
    MCAgent_t agent);

/**
 * \brief         Add a stationary agent to the agency 
 *
 * \param agency  a MobileC agency
 * \param agent_thread The binary-space agent function
 * \param name    The new agent's name
 * \param agent_args user arguments to pass to the agent
 *
 * \return        0 if successful, or error_code_t type
 */
int MC_AddStationaryAgent(
    MCAgency_t agency, 
    void* (*agent_thread)(stationary_agent_info_t*), 
    const char* name, 
    void* agent_args);

/**
 * \brief         Add an agenct initialization callback function
 *
 * \param agency  a MobileC agency
 * \param function The callback function to add. This callback function will be
 *                 called after an interpreter is initialized for an agent but
 *                 before the agent is executed. 
 * \param user_data A pointer to any user data that should be made available to
 *                  the callback function
 *
 * \return        0 if successful, or error_code_t type
 */
int MC_AddAgentInitCallback(
    MCAgency_t agency,
    MC_AgentInitCallbackFunc_t function,
    void* user_data);

/**
 * \brief           Add a new task to an already existing agent
 *                  
 * \param agent     An existing agent
 * \param code      The C code to use in the new task
 * \param return_var_name The name of the the return variable, or NULL if there
 *                  is no return variable.
 * \param server    The server to perform the task on.
 * \param persistent Whether or not the task is persistent. '1' indicates
 *                  persistent, and '0' indicates not-persistent (the default)
 * \return          Returns a valid agency handle or NULL on failure.
 */
int MC_AgentAddTask(
    MCAgent_t agent, 
    const char* code, 
    const char* return_var_name, 
    const char* server,
    int persistent);

/**
 * \brief           Attach a file to the agent's current task
 *
 * \param agent     An existing agent
 * \param name      The name to give the file
 * \param filepath  The absolute path of the file
 *
 * \return          Returns 0 on success, or an error code on failure.
 */
EXPORTMC int MC_AgentAttachFile(
    MCAgent_t agent,
    const char* name,
    const char* filepath);

/**
 * \brief           Get a list of an agent's attached files
 *
 * \param agent     An existing agent
 * \param names     A pointer two an unallocated two dimentional character
 *                  array. The function will allocated any necessary memory for
 *                  storing the names. If names are allocated, the array must
 *                  be freed by the user.
 * \param num_files A pointer to an integer which will be filled with the number
 *                  of filenames
 * \return          returns 0 on success, -1 on failure.
 */
EXPORTMC int MC_AgentListFiles(
    MCAgent_t agent,
    int task_num,
    char*** names, /*OUT*/
    int* num_files /*OUT*/);

/**
 * \brief           Saves an agent's attached file to the filesystem
 * \param name      The name of the agent's stored file
 * \param save_path The location to save the file (including the filename)
 * \return          Returns 0 on success, -1 on failure. */
EXPORTMC int MC_AgentRetrieveFile(
    MCAgent_t agent, 
    int task_num,
    const char* name,
    const char* save_path);

/**
 * \brief           Add a new task to an already existing agent
 *                  
 * \param agent     An existing agent
 * \param filename  The C code to use in the new task
 * \param return_var_name The name of the the return variable, or NULL if there
 *                  is no return variable.
 * \param server    The server to perform the task on.
 * \param persistent Whether or not the task is persistent. '1' indicates
 *                  persistent, and '0' indicates not-persistent (the default)
 * \return          Returns a valid agency handle or NULL on failure.
 */
int MC_AgentAddTaskFromFile(
    MCAgent_t agent, 
    const char* filename, 
    const char* return_var_name, 
    const char* server,
    int persistent);

/**
 * \brief           Get the host Mobile-C agency handle from a
 *                  "stationary_agent_info_t" data structure.
 *                  
 * \param stationary_agent_info A stationary_agent_info_t type variable
 *                  provided to a stationary agent function
 * \return          Returns a valid agency handle or NULL on failure.
 */
extern MCAgency_t 
MC_AgentInfo_GetAgency(stationary_agent_info_t* stationary_agent_info);

/**
 * \brief           Retrieve an agent handle from the stationary agent
 *                  info from within a stationary agent.
 * \param stationary_agent_info A stationary_agent_info_t type variable
 *                  provided to a stationary agent function
 * \return          Returns a valid agent handle or NULL on failure.
 * \section         Examples
 * The following example demonstrates usage of MC_AgentInfo_GetAgent()
 * \include stationary_agent_communication/server.c
 */
extern MCAgent_t 
MC_AgentInfo_GetAgent(stationary_agent_info_t* stationary_agent_info);

/**
 * \brief           Retrieve the agent argument supplied as the "agent_args"
 *                  parameter in the MC_AddStationaryAgent() function.
 *                  
 * \param stationary_agent_info A stationary_agent_info_t type variable
 *                  provided to a stationary agent function
 * \return          Returns a valid pointer or NULL on failure.
 */
extern void*
MC_AgentInfo_GetAgentArgs(stationary_agent_info_t* stationary_agent_info);

/**
 * \brief           Get the dimension of an agent's return data, if it is an array
 *
 * \param agent     A return agent
 * \param task      task to retrieve the agent return data size from
 *
 * \return          Returns a positive integer on success or -1 on failure.
 */
extern int MC_AgentReturnArrayDim(
    MCAgent_t agent,
    int task_num );

/**
 * \brief           Get the extent of a dimension of an agent's return data, if it is an array
 *
 * \param agent     A return agent
 * \param task      task to retrieve the agent return data size from
 * \param index     The dimension index to get the extent
 *
 * \return          Returns a positive integer on success or -1 on failure.
 */
extern int MC_AgentReturnArrayExtent(
    MCAgent_t agent,
    int task_num,
    int index);

/**
 * \brief           Get the total number of elements of the agent return data.
 *
 * \param agent     A return agent
 * \param task      task to retrieve the agent return data size from
 * \param index     The dimension index to get the extent
 *
 * \return          Returns a positive integer on success or -1 on failure.
 */
extern int MC_AgentReturnArrayNum(
    MCAgent_t agent,
    int task_num);

/**
 * \brief           Get a pointer to the agent's return data
 *
 * \param agent     A return agent
 * \param task      task to retrieve the agent return data from
 *
 * \return          Returns a pointer to the beginning of the task's return
 *                  data on success. Returns NULL on failure.
 */
extern const void* MC_AgentReturnDataGetSymbolAddr(
    MCAgent_t agent,
    int task_num );

/**
 * \brief           Get the size of an agent's return data type. 
 *
 * \param agent     A return agent
 * \param task      task to retrieve the agent return data type size from
 *
 * \return          Returns a positive integer on success or -1 on failure.
 */
extern size_t MC_AgentReturnDataSize(
    MCAgent_t agent,
    int task_num );

/**
 * \brief           Get the type of an agent's return data
 *
 * \param agent     A return agent
 * \param task      task to retrieve the agent return data type from
 *
 * \return          Returns a positive integer on success or -1 on failure.
 *                  The return type is of type ChType_t, defined in ch.h
 */
extern int MC_AgentReturnDataType(
    MCAgent_t agent,
    int task_num );

/**
 * \brief           Check to see whether or not an agent's return data is an array.
 *
 * \param agent     A return agent
 * \param task      task to check to see if the return data is an array.
 *
 * \return          Returns 1 if the return variable is an array, 0 if it is
 *                  not an array, or -1 on failure.
 */
extern int MC_AgentReturnIsArray(
    MCAgent_t agent,
    int task_num );

/**
 * \brief           Retrieve a pointer to a previously saved variable.
 *
 * \param agent     A MobileC agent.
 * \param var_name  The name of the saved variable that has previously
 *                  been saved.
 * \param task_num  The previous completed task from which to retrieve
 *                  the saved variable.
 *
 * \return          A pointer to the data on success or NULL on failure.
 * \section         Examples
 * The following example demonstrates usage of MC_AgentVariableRetrieve() 
 * from agent space.
 * \include agent_saved_variables_example/test1.xml
 */
extern const void* 
MC_AgentVariableRetrieve(
    MCAgent_t agent,
    const char* var_name,
    int task_num);

/**
 * \brief           Retrieve a info about a previously saved variable.
 *
 * \param agent     A MobileC agent.
 * \param var_name  The name of the saved variable that has previously
 *                  been saved.
 * \param task_num  The previous completed task from which to retrieve
 *                  the saved variable.
 * \param data      (Output) The Variable Data
 * \param dim       (Output) The dimension of the data array
 * \param extent    (Output) The extents of the output array
 *
 * \return          Error code.
 */
int
MC_AgentVariableRetrieveInfo(
    MCAgent_t agent,
    const char* var_name,
    int task_num,
    const void** data,
    int* dim,
    const int** extent
    );

/**
 * \brief           Mark an agent variable for saving
 *
 * \param agent     A MobileC agent.
 * \param var_name  The name of the variable to mark for saving.
 *
 * \return          0 on success, non-zero on failure.
 * \sa test1.xml
 * \section         Examples
 * 
 * See agent_saved_variables_example/test1.xml for an example of usage of
 * this api function.
 * \section         Examples
 * The following example demonstrates usage of MC_AgentVariableSave() 
 * from agent space.
 * \include agent_saved_variables_example/test1.xml
 */
extern int 
MC_AgentVariableSave(MCAgent_t agent, const char* var_name);

EXPORTMC int 
MC_Barrier(MCAgency_t attr, int id); 

/**
 * \brief           Find and delete an initialized MobileC Barrier
 *
 * \param attr      A running MobileC agency
 * \param id        The id of the barrier node to delete
 *
 * \return          returns 0 on success, error if the node is not
 *                  found or other failure.
 */
EXPORTMC extern int MC_BarrierDelete(MCAgency_t attr, int id);

/**
 * \brief           Initialize a MobileC Barrier 
 *
 * \param attr      A running MobileC agency
 * \param id        The requested barrier id
 * \param num_procs The number of agents/threads/processes that will wait on the 
 *                  barrier
 *
 * \return          The allocated barrier id. May differ from the requested id if 
 *                  it is alread in use.
 *
 * \section Examples
 * The following example demonstrates an agent which sets up an MC_Barrier.
 * \include mc_barrier_example/comm_agent.xml
 */
EXPORTMC extern int MC_BarrierInit(MCAgency_t attr, int id, int num_procs);

/**
 * \brief           Use custom ChOptions_t type for internal Ch interpretor
 *
 * \param attr      A running MobileC agency
 * \param options   Initialized Ch options structure
 *
 * \return          0 on success, error_code_t type on failure
 */
#ifdef OBS
EXPORTMC extern int MC_ChInitializeOptions(MCAgency_t attr, ChOptions_t *options);
#endif


/**
 * \brief           Calls a function defined in an agent
 *
 * \param agent     An initialized and executed MobileC agent
 * \param funcName  The name of the function to call
 * \param returnVal The agent function's return value
 * \param numArgs   The number of arguments supplied to the agent function
 * \param ...       A variable argument list to be supplied to the
 *                  agent function
 *
 * \return          0 if successful, error_code_t type on failure
 *
 * \section Example
 * \include persistent_example/host.c
 */
EXPORTMC int
MC_CallAgentFunc(
    MCAgent_t agent,
    const char* funcName,
    void* returnVal,
    int numArgs,
    ...);

/**
 * \brief           Calls a function defined in an agent
 *
 * \param agent     An initialized and executed MobileC agent
 * \param funcName  The name of the function to call
 * \param returnVal The agent function's return value
 * \param arg      The agent functions argument
 *
 * \note            The agent function must be of the form
 *                  'void* func(void* arg);'
 *
 * \return          0 if successful, error_code_t type on failure
 *
 * \section Example
 * \include persistent_example/host.c
 */
EXPORTMC extern int MC_CallAgentFuncArg(
        MCAgent_t agent,
        const char* funcName,
        void* returnVal,
	void* arg
        );

/**
 * \brief           Calls a function defined in an agent
 *
 * \param agent     An initialized and executed MobileC agent
 * \param funcName  The name of the function to call
 * \param returnVal The agent function's return value
 * \param ap        A variable argument list 
 *
 * \return          0 if successful, error_code_t type on failure
 */
EXPORTMC extern int MC_CallAgentFuncV(
    MCAgent_t agent,
    const char* funcName,
    void* returnVal,
    va_list ap);

  /*
    \brief          Calls a function defined in an agent
    \param agent    A Mobile-C Agent
    \param funcName The name of the function to call
    \param returnVal (output) Return value from the agent function
    \param arglist  Ch Variable Argument list to pass in
   */
EXPORTMC extern int MC_CallAgentFuncVar
(
 MCAgent_t agent,
 const char* funcName,
 void* returnVal,
 ChVaList_t arglist
 );
					 

/**
 * \brief           Compose a new agent dynamically without using a prewritten XML file.
 *
 * \param name      The desired name of the new agent.
 * \param home      The home of the new agent.
 * \param owner     The owner of the new agent.
 * \param code      The agent code
 * \param return_var_name The name of the agent's return variable. Set to "no-return" if no return variable is desired.
 * \param server    The target destination of the agent.
 * \param persitent A flag indicating whether or not the agent should be persistent. A value of '1' indicates persistence, while a value of '0' indicates default non-persistent behaviour.
 *
 * \return          This function returns a valid MCAgent_t structure on success or NULL on failure.
 */
EXPORTMC extern MCAgent_t
MC_ComposeAgent(
    const char* name,
    const char* home,
    const char* owner, 
    const char* code,
    const char* return_var_name,
    const char* server,
    int persistent
    );

/**
 * \brief           Compose a new agent dynamically without using a prewritten XML file.
 *
 * \param name      The desired name of the new agent.
 * \param home      The home of the new agent.
 * \param owner     The owner of the new agent.
 * \param code      The agent code
 * \param return_var_name The name of the agent's return variable. Set to "no-return" if no return variable is desired.
 * \param server    The target destination of the agent.
 * \param workgroup_code The secret workgroup code of the agent. Only agents with the same workgroup code may perform certain interactions.
 * \param persitent A flag indicating whether or not the agent should be persistent. A value of '1' indicates persistence, while a value of '0' indicates default non-persistent behaviour.
 *
 * \return          This function returns a valid MCAgent_t structure on success or NULL on failure.
 * \note            This function is deprecated. Please use the
 *                  MC_ComposeAgentWithWorkgroup function instead.
 */
EXPORTMC extern MCAgent_t
MC_ComposeAgentS(
    const char* name,
    const char* home,
    const char* owner, 
    const char* code,
    const char* return_var_name,
    const char* server,
    int persistent,
		const char* workgroup_code
    );

/**
 * \brief           Compose a new agent dynamically without using a prewritten XML file.
 *
 * \param name      The desired name of the new agent.
 * \param home      The home of the new agent.
 * \param owner     The owner of the new agent.
 * \param code      The agent code
 * \param return_var_name The name of the agent's return variable. Set to "no-return" if no return variable is desired.
 * \param server    The target destination of the agent.
 * \param workgroup_code The secret workgroup code of the agent. Only agents with the same workgroup code may perform certain interactions.
 * \param persitent A flag indicating whether or not the agent should be persistent. A value of '1' indicates persistence, while a value of '0' indicates default non-persistent behaviour.
 *
 * \return          This function returns a valid MCAgent_t structure on success or NULL on failure.
 * \note            This function is deprecated
 */
EXPORTMC extern MCAgent_t
MC_ComposeAgentWithWorkgroup(
    const char* name,
    const char* home,
    const char* owner, 
    const char* code,
    const char* return_var_name,
    const char* server,
    int persistent,
		const char* workgroup_code
    );

/**
 * \brief           Compose a new agent dynamically from a source code file
 *
 * \param filename  The filename of the source file
 * \param name      The desired name of the new agent.
 * \param home      The home of the new agent.
 * \param owner     The owner of the new agent.
 * \param code      The agent code
 * \param return_var_name The name of the agent's return variable. Set to "no-return" if no return variable is desired.
 * \param server    The target destination of the agent.
 * \param persitent A flag indicating whether or not the agent should be persistent. A value of '1' indicates persistence, while a value of '0' indicates default non-persistent behaviour.
 *
 * \return          This function returns a valid MCAgent_t structure on success or NULL on failure.
 */
EXPORTMC extern MCAgent_t
MC_ComposeAgentFromFile(
    const char* name,
    const char* home,
    const char* owner, 
    const char* filename,
    const char* return_var_name,
    const char* server,
    int persistent
    );

/**
 * \brief           Compose a new agent dynamically from a source code file
 *
 * \param filename  The filename of the source file
 * \param name      The desired name of the new agent.
 * \param home      The home of the new agent.
 * \param owner     The owner of the new agent.
 * \param code      The agent code
 * \param return_var_name The name of the agent's return variable. Set to "no-return" if no return variable is desired.
 * \param server    The target destination of the agent.
 * \param workgroup_code The secret workgroup code of the agent. Only agents with the same workgroup code may perform certain interactions.
 * \param persitent A flag indicating whether or not the agent should be persistent. A value of '1' indicates persistence, while a value of '0' indicates default non-persistent behaviour.
 *
 * \return          This function returns a valid MCAgent_t structure on success or NULL on failure.
 * \note            This function is deprecated. Please use the
 *                  MC_ComposeAgentFromFileWithWorkgroup function instead.
 */
EXPORTMC extern MCAgent_t
MC_ComposeAgentFromFileS(
    const char* name,
    const char* home,
    const char* owner, 
    const char* filename,
    const char* return_var_name,
    const char* server,
    int persistent,
		const char* workgroup_code
    );

/**
 * \brief           Compose a new agent dynamically from a source code file
 *
 * \param filename  The filename of the source file
 * \param name      The desired name of the new agent.
 * \param home      The home of the new agent.
 * \param owner     The owner of the new agent.
 * \param code      The agent code
 * \param return_var_name The name of the agent's return variable. Set to "no-return" if no return variable is desired.
 * \param server    The target destination of the agent.
 * \param workgroup_code The secret workgroup code of the agent. Only agents with the same workgroup code may perform certain interactions.
 * \param persitent A flag indicating whether or not the agent should be persistent. A value of '1' indicates persistence, while a value of '0' indicates default non-persistent behaviour.
 *
 * \return          This function returns a valid MCAgent_t structure on success or NULL on failure.
 */
EXPORTMC extern MCAgent_t
MC_ComposeAgentFromFileWithWorkgroup(
    const char* name,
    const char* home,
    const char* owner, 
    const char* filename,
    const char* return_var_name,
    const char* server,
    int persistent,
		const char* workgroup_code
    );

/**
 * \brief           Wakes up all agents/threads waiting on a condition variable
 *
 * \param attr      A MobileC agency
 * \param id        Synchronization variable id to broadcast to
 *
 * \see             MC_SyncInit(), MC_CondSignal()
 *
 * \return          0 on success, error_code_t type on failure
 */
EXPORTMC extern int MC_CondBroadcast(MCAgency_t attr, int id);

/**
 * \brief           Wakes up at least one thread waiting on a condition variable
 *
 * \param attr      A MobileC agency
 * \param id        synchronization variable id
 *
 * \see             MC_SyncInit()
 *
 * \return          0 on success, error_code_t type on failure
 *
 * \section Example
 * The following example demonstrates the agent-space version of the function,
 * which is nearly identical to the binary space api function.
 * \include agent_cond_example/wake.xml
 */
EXPORTMC extern int MC_CondSignal(MCAgency_t attr, int id);

/**
 * \brief           Reset a previously signalled MobileC condition variable
 *
 * \param attr      A MobileC Agency
 * \param id        The synchronization variable id to reset
 *
 * \see             MC_SyncInit()
 *
 * \return          0 on success, error_code_t type on failure
 */
EXPORTMC extern int MC_CondReset(MCAgency_t attr, int id);

/**
 * \brief           Wait on a MobileC synchronization variable
 *
 * \param attr      A MobileC agency
 * \param id        a synchronization variable id
 *
 * \see             MC_SyncInit()
 *
 * \return          0 on success, error_code_t type on failure
 * \section Example
 * The following example demonstrates the agent-space version of this function.
 * \include agent_cond_example/sleep.xml
 */
EXPORTMC extern int MC_CondWait(MCAgency_t attr, int id);

/**
 * \brief           Performs a deep-copy of an agent structure
 *
 * \param agent_out A pointer to the agent to copy to.
 * \param agent_in  The agent to copy
 *
 * \return          0 on success, error_code_t type on failure.
 *
 */
int MC_CopyAgent(MCAgent_t* agent_out, const MCAgent_t agent_in);

/**
 * \brief           Stop and remove an agent
 *
 * \param agent     An agent in any state (running, waiting, etc)
 *
 * \return          0 on success, error_code_t type on failure
 *
 */
EXPORTMC extern int MC_DeleteAgent(MCAgent_t agent);

/**
 * \brief           Stop and remove an agent in the same workgroup
 *
 * \param calling_agent The calling agent
 * \param agent     An agent in any state (running, waiting, etc)
 *
 * \note            The agents must belong to the same workgroup.
 *
 * \return          0 on success, error_code_t type on failure
 *
 */
EXPORTMC extern int MC_DeleteAgentWG(MCAgent_t calling_agent, MCAgent_t agent);

/**
 * \brief           End an agency
 *
 * \param attr      A running agency
 *
 * \return          0 on success, error_code_t type on failure
 *
 * \section Example
 * \include hello_world/client.c
 */
EXPORTMC extern int MC_End(MCAgency_t attr);

/**
 * \brief           Free memory allocated by a Service Search operation
 *
 * \param agentName agent names returned by a search operation.
 * \param serviceName service names return by a search operation.
 * \param agentID   list of agent id's returned by a search operation.
 * \param numResult The number of hits returned by a search operation.
 *
 * \return          0 on success, error code on failure.
 */
int MC_DestroyServiceSearchResult(
    char** agentName,
    char** serviceName,
    int* agentID,
    int numResult);


/**
 * \brief           Find an agent by its name
 *
 * \param attr      a running agency
 * \param name      name to search for
 *
 * \return          a valid agent on success or NULL on failure
 * \section Example
 * \include multi_task_example/client.c
 */
EXPORTMC extern MCAgent_t MC_FindAgentByName(MCAgency_t attr, const char *name);

/**
 * \brief           Find an agent by its id
 *
 * \param attr      the agency to search
 * \param ID        the id to search for
 *
 * \return          a valid agent on success, NULL on failure
 */
EXPORTMC extern MCAgent_t MC_FindAgentByID(MCAgency_t attr, int ID);

/**
 * \brief           Retrieve an agent's Ch interpreter
 *
 * \param agent     a valid agent
 *
 * \return          a Ch interpreter of type 'ChInterp_t' on success, or
 *                  NULL on failure.
 */
EXPORTMC extern /*ChInterp_t*/ void* MC_GetAgentExecEngine(MCAgent_t agent);

/**
 * \brief           Retrieve an agent's id
 */
EXPORTMC extern int MC_GetAgentID(MCAgent_t agent);

/*
 * \brief           Retrieve an agent's name
 *
 * \return          a malloc'd character string on success, or NULL on failure
 */
EXPORTMC extern char* MC_GetAgentName(MCAgent_t agent);

/**
 * \brief           Retrive the number of tasks an agent has
 * \section Example
 * \include multi_task_example/client.c
 */
EXPORTMC extern int MC_GetAgentNumTasks(MCAgent_t agent);

/**
 * \brief           Get an agent's return data
 *
 * \param agent     a valid agent
 * \param task_num  the task for which to retrieve the return data. The task must
 *                  already be completed.
 * \param data      the return data. May be multi dimensional array.
 * \param dim       the number of dimensions of the return array. 
 * \param extent    the extent of each one of the array dimensions.
 *
 * \section Example
 * This file demonstrates the retrieval of agent return data from an agent
 * \include mc_array_return_example/client.c
 * This is the agent which gets the data
 * \include mc_array_return_example/agent.xml
 */
EXPORTMC extern int MC_GetAgentReturnData(
        MCAgent_t agent,
        int task_num,
        void **data,
        int *dim,
        int **extent);

/**
 * \brief           Get an agent's current status
 *
 * \return          returns type 'enum MC_AgentStatus_e'
 */
EXPORTMC extern int MC_GetAgentStatus(MCAgent_t agent);

/**
 * \brief           Get an agent's type
 *
 * \return          returns type 'enum MC_AgentType_e'
 */
EXPORTMC extern enum MC_AgentType_e MC_GetAgentType(MCAgent_t agent);

/**
 * \brief           Get a list of agents that match a certain agent status
 * 
 * \param agency    A handle to a running Mobile-C agency
 * \param agents    The address of an uninitialized pointer to an agent. An
 *                  array of agents will be allocated, which will need to be
 *                  freed by the user. 
 * \param num_agents A pointer to an integer. The integer will be filled with
 *                  the number of agents allocated for the 'agents' variable. 
 * \param agent_status_flags These contain flags of agent statuses. For
 *                  instance, if a user wishes to retrieve all agents with
 *                  either the status MC_WAIT_CH or MC_AGENT_NEUTRAL, they may
 *                  use an agent_status_flag of ((1<<MC_WAIT_CH) | (1<<MC_AGENT_NEUTRAL)).
 *                  All valid agent statuses are enumerated in the enumeration 
 *                  MC_AgentStatus_e, located in libmc.h.
 */

EXPORTMC int
MC_GetAgents(MCAgency_t attr, MCAgent_t **agents, int* num_agents, unsigned int agent_status_flags);

/**
 * \brief           Get all of the agents on an agency
 *  
 * \param agency    A handle to a running Mobile-C agency
 * \param agents    The address of an uninitialized pointer to an agent. An
 *                  array of agents will be allocated, which will need to be
 *                  freed by the user. 
 * \param num_agents A pointer to an integer. The integer will be filled with
 *                  the number of agents allocated for the 'agents' variable. 
 */
EXPORTMC int 
MC_GetAllAgents(MCAgency_t attr, MCAgent_t **agents, int* num_agents);

/**
 * \brief           Get an agent's xml string
 *
 * \return          a malloc'd character string containing the agent's xml code
 */
EXPORTMC extern char* MC_GetAgentXMLString(MCAgent_t agent);

/**
 * \brief           Halt an agency: Do not process new entries in queues.
 *
 * \param agency    A handle to a running MobileC agency.
 *
 * \return          0 on success, non-zero on failure.
 */
EXPORTMC extern int MC_HaltAgency(MCAgency_t agency);

/**
 * \brief           Initialize and start a MobileC agency
 *
 * \param port      the TCP port the agency should bind to
 * \param options   initialized MobileC options or NULL for default options
 *
 * \return          a handle to a running MobileC agency or NULL on failure
 * \section Example
 * \include hello_world/server.c
 */
EXPORTMC extern MCAgency_t MC_Initialize(
        int port,
        MCAgencyOptions_t *options);

/**
 * \brief           Initialize MobileC options
 *
 * \param options   options to initialize.
 *
 * \return          0 on success, error_code_t on failure
 *
 * \note            MobileC options should be initialized with this function
 *                  before any of its members are modified.
 * \section Example
 * \include hello_world/server.c
 */
EXPORTMC extern int MC_InitializeAgencyOptions(struct MCAgencyOptions_s* options);

/**
 * \brief           Wait indefinitely
 *
 * \note            This function is intended to block the calling thread forever.
 */
 EXPORTMC extern int MC_MainLoop(MCAgency_t attr);

/**
 * \brief           Load an agent from a file into an agency
 *
 * \param agency    A valid and running Mobile-C agency
 * \param filename  Filename containing the agent to load
 *
 * \return          0 on success, non-zero on failure.
 */
EXPORTMC extern int MC_LoadAgentFromFile(MCAgency_t attr, const char* filename);

/**
 * \brief           Migrates a running agent to another host.
 *
 * \param agent     The agent to migrate
 * \param hostname  The new host to migrate the agent to
 * \param port      The new port to migrate the agent to
 *
 * \return          0 on success, error_code_t type on failure.
 */
EXPORTMC extern int MC_MigrateAgent(MCAgent_t agent, const char* hostname, int port);

/**
 * \brief           Locks a MobileC synchronization variable as a mutex
 *
 * \param attr      a MobileC agency handle
 * \param id        the synchronization variable id to lock
 *
 * \return          0 on success, error_code_t type on failure
 * \section Example
 * Consider the following agents, which use the agent-space version of this 
 * api function. Note that the 'sleep' agent is sent first, followed by the
 * 'wake' agent.
 * \include agent_mutex_example/sleep.xml
 * \include agent_mutex_example/wake.xml
 */
EXPORTMC extern int MC_MutexLock(MCAgency_t attr, int id); 

/*
 * \brief           Unlocks a MobileC synchronization variable
 *
 * \param attr      a MobileC agency handle
 * \param id        the synchronization variable id to unlock
 *
 * \return          0 on success, error_code_t type on failure
 *
 * \see             MC_MutexLock()
 */
EXPORTMC extern int MC_MutexUnlock(MCAgency_t attr, int id); 

/**
 * \brief           Register a new service with the Directory Facilitator
 *
 * \param agency    a MobileC agency handle
 * \param agent     (OPTIONAL: See note) a MobileC agent
 * \param agentID   (OPTIONAL: See note) a MobileC agent id
 * \param agentName (OPTIONAL: See note) a MobileC agent name
 * \param serviceNames an array of character strings of service names
 * \param numServices  the number of services described in 'serviceNames'
 *
 * \return          0 on success, error_code_t type on failure
 *
 * \note            Three of the input arguments are optional. The function
 *                  expects as input the arguments 'agent XOR (agentID AND agentName)'.
 * \section Example
 * \include mc_df_service_test/service_provider_1.xml
 */
EXPORTMC extern int MC_RegisterService(
        MCAgency_t agency,
        /* Optional input args: Must input 
         * MCAgent_t OR
         * agentID AND agentName */
        MCAgent_t agent,
        int agentID,
        const char *agentName,
        char **serviceNames,
        int numServices);

/**
 * \brief           Resumes a halted agency.
 *
 * \param agency    An agency previously halted with the MC_HaltAgency() function.
 *
 * \return          0 on success, non-zero on failure.
 */
EXPORTMC extern int MC_ResumeAgency(MCAgency_t agency);

/**
 * \brief           Retrieves the oldest agent from an agency
 *
 * \return          a valid agent or NULL on failure
 */
EXPORTMC extern MCAgent_t MC_RetrieveAgent(MCAgency_t attr);

/**
 * \brief           Post to a MobileC synchronization variable semaphore
 *
 * \param attr      a MobileC agency handle
 * \param id        the synchronization variable id to post to
 *
 * \return          0 on success, error_code_t type on failure
 *
 * \section Example
 * \include agent_semaphore_example/wake.xml
 */
EXPORTMC extern int MC_SemaphorePost(MCAgency_t attr, int id);

/**
 * \brief           Decreases a MobileC synchronization variable semaphore count by one
 *
 * \param attr      a MobileC agency handle
 * \param id        synchronization variable id to wait on
 *
 * \return          0 on MC_SUCCESS, error_code_t type of failure
 *
 * \note            If the semaphore count is already zero, this function will
 *                  block until another thread posts to the semaphore.
 * \section Example
 * \include agent_semaphore_example/sleep.xml
 */
EXPORTMC extern int MC_SemaphoreWait(MCAgency_t attr, int id);

/**
 * \brief           Sets default incoming agent status
 *
 * \param agency    a MobileC agency handle
 * \param status    the status to set all incoming agents
 *
 * \return          0 on success, error_type_t on failure
 *
 * \note            using this function will override any status the incoming
 *                  agent attempts to set for itself.
 */
EXPORTMC extern int MC_SetDefaultAgentStatus(MCAgency_t agency, enum MC_AgentStatus_e status);

/**
 * \brief           Sets a MobileC thread to "on" status.
 *
 * \param options   MobileC options previously initialized with 
 *                  MC_InitializeAgencyOptions()
 * \param index     the thread to set
 *
 * \return          0 on success, error_code_t on failure
 *
 * \note            This function must be called before MC_Initialize(). Once
 *                  an agency is started with MC_Initialize, the MC_SetThread
 *                  functions will have no effect.
 */
EXPORTMC extern int MC_SetThreadOn(MCAgencyOptions_t *options, enum MC_ThreadIndex_e index);

/** 
 * \brief           Set all Mobile-C threads on.
 *
 * \param options   MobileC options structure, initialized with
 *                  MC_InitializeAgencyOptions()
 *
 * \return          0 on success, error code on failure.
 */
EXPORTMC extern int MC_SetThreadsAllOn(MCAgencyOptions_t* options);

/**
 * \brief           Sets a MobileC thread to "off" status.
 *
 * \param options   MobileC options previously initialized with 
 *                  MC_InitializeAgencyOptions()
 * \param index     the thread to set
 *
 * \return          0 on success, error_code_t on failure
 *
 * \note            This function must be called before MC_Initialize(). Once
 *                  an agency is started with MC_Initialize, the MC_SetThread
 *                  functions will have no effect.
 */
EXPORTMC extern int MC_SetThreadOff(MCAgencyOptions_t *options, enum MC_ThreadIndex_e index );

/**
 * \brief           Set all MobileC threads to 'off' status.
 *
 * \param options   a MobileC options structure initialized with with the
 *                  MC_InitializeAgencyOptions() function.
 *
 * \return          0 on success, error code on failure.
 */
EXPORTMC extern int MC_SetThreadsAllOff(MCAgencyOptions_t* options);

/**
 * \brief           Prints an agents code to stdout
 *
 * \return          0 on success, error_code_t on failure
 */
EXPORTMC extern int MC_PrintAgentCode(MCAgent_t agent);

/**
 * \brief           Retrieves an agent's Ch code
 *
 * \return          a malloc'd character string on success, NULL on failure
 */
EXPORTMC extern char * MC_RetrieveAgentCode(MCAgent_t agent);

/**
 * \brief           Reset a MobileC signal
 *
 * \return          0 on success, error_code_t on failure
 *
 * \see             MC_WaitSignal()
 */
EXPORTMC extern int MC_ResetSignal(MCAgency_t attr);

/**
 * \brief           Search the directory facilitator for a service
 *
 * \return          0 on success, error_code_t on failure
 *
 * \param attr      (input) a MobileC agency handle
 * \param searchString (input) substring to search services for
 * \param agentNames (return) array of agent names with matching services
 * \param serviceNames (return) array of matching service names
 * \param agentIDs  (return) array of matching agent IDs
 * \param numResults (return) number of matching results
 *
 * \section Example
 * \include mc_df_service_test/test1.xml
 */
EXPORTMC extern int MC_SearchForService(
        /* Input args */
        MCAgency_t attr, 
        const char *searchString,
        /* Return Args */
        char*** agentNames,
        char*** serviceNames,
        int** agentIDs,
        int* numResults);

/**
 * \brief           Sends an agent migration message
 *
 * \param attr      a MobileC agency handle
 * \param message   a valid MobileC xml agent migration message
 */
EXPORTMC extern int MC_SendAgent(MCAgency_t attr,
        const char *message);

/**
 * \brief           Sends an agent migration message
 *
 * \param attr      a MobileC agency handle
 * \param filename  file containing a valid MobileC xml agent migration message
 */
EXPORTMC extern int MC_SendAgentFile(MCAgency_t attr, 
        const char *filename);

/**
 * \brief           Sends an agent migration message
 *
 * \param attr      a MobileC agency handle
 * \param message   a valid MobileC xml agent migration message
 * \param hostname  host to send the message to
 * \param port      port to send the message to
 */
EXPORTMC extern int MC_SendAgentMigrationMessage(MCAgency_t attr,
        const char *message,
        const char *hostname,
        int port);

/**
 * \brief           Sends an agent migration message
 *
 * \param attr      a MobileC agency handle
 * \param filename  file containing a valid MobileC xml agent migration message
 * \param hostname  hostname to send the agent to
 * \param port      port to send the agent to
 */
EXPORTMC extern int MC_SendAgentMigrationMessageFile(MCAgency_t attr, 
        const char *filename,
        const char *hostname,
        int port);

/**
 * \brief           Set an agent's status
 *
 * \param agent     a MobileC agent
 * \param status    agent status of type 'enum MC_AgentStatus_e'
 *
 * \return          0 on success, or error_code_t on failure
 */
EXPORTMC extern int MC_SetAgentStatus(MCAgent_t agent, enum MC_AgentStatus_e status);

/**
 * \brief           Set up a steerable algorithm
 *
 * \param attr      a MobileC agency handle
 * \param funcptr   a function pointer to the algorithm
 * \param arg       an argument for the algorithm function
 *
 * \return          0 on success, error_code_t on failure
 *
 * \note            The algorithm function must contain a call to 
 *                  MC_SteerControl in order for the algorithm to be
 *                  steerable.
 * \section Example
 * \include steer_example/mc_server.c
 */
EXPORTMC extern int MC_Steer( MCAgency_t attr, int (*funcptr)(void* data), void *arg);

/**
 * \brief           The MobileC user-algorithm steering function
 *
 * \return          The current steering command
 *
 * \note            This function belongs inside a user's steerable algorithm.
 * \see             MC_Steer()
 */
EXPORTMC extern enum MC_SteerCommand_e MC_SteerControl(void);

/**
 * \brief           Deletes a previously initialized synchronization variable
 *
 * \param attr      a MobileC agency handle
 * \param id        the sync variable id to delete
 *
 * \return          0 on success, or error_code_t on failure
 */
EXPORTMC extern int MC_SyncDelete(MCAgency_t attr, int id);

/**
 * \brief           Initializes a new MobileC synchronization variable
 *
 * \param attr      a MobileC agency handle
 * \param id        the requested sync variable id
 *
 * \return          new sync variable's id. May be different than the requested id.
 *
 * \note            Each synchronization variable may be used as a mutex, 
 *                  condition variable, or semaphore. However, it should only
 *                  be used as one type of synchronization variable per instance,
 *                  or undefined behaviour may result.
 * \see             MC_MutexLock(), MC_MutexUnlock(), MC_CondWait(), MC_CondSignal(),
 *                  MC_CondBroadcast, MC_SemaphorePost(), MC_SemaphoreWait()
 */
EXPORTMC extern int MC_SyncInit(MCAgency_t attr, int id);

/**
 * \brief           Halt a running agent
 *
 * \return          0 on success, error_code_t on failure
 */
EXPORTMC extern int MC_TerminateAgent(MCAgent_t agent);


EXPORTMC extern int 
MC_TerminateAgentWG(MCAgent_t calling_agent, MCAgent_t agent);
/**
 * \brief           Wait indefinitely
 *
 * \note            This function is intended to block the calling thread forever.
 */
/* EXPORTMC extern int MC_Wait(MCAgency_t attr); */

/**
 * \brief           Wait for an agent arrival event
 *
 * \note            This function blocks until an agent arrival signal is
 *                  triggered, at which point in unblocks.
 */
EXPORTMC extern int MC_WaitAgent(MCAgency_t attr);

/**
 * \brief           Wait and retrieve an agent
 *
 * \return          a valid MobileC agent on success, or NULL on failure
 *
 * \note            This function blocks until the arrival of an agent. The agent
 *                  is retrieved after it is initialized, but before it is executed.
 */
EXPORTMC extern MCAgent_t MC_WaitRetrieveAgent(MCAgency_t attr);

/**
 * \brief           Wait for a MobileC signal
 *
 * \param attr      a MobileC agency handle
 * \param signals   a flag of signals to wait for, of type 'enum MC_Signal_e'
 *
 * \return          0 on success, error_code_t on failure
 *
 * \note            the parameter 'signals' may be something like
 *                  'MC_RECV_MESSAGE | MC_RECV_AGENT', etc. 
 * \section Example
 * \include multi_task_example/client.c
 */
EXPORTMC extern int MC_WaitSignal(MCAgency_t attr, int signals);

#ifdef __cplusplus
}
#endif

#endif /* _MOBILEC_H_ */

