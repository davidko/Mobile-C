/* SVN FILE INFO
 * $Revision: 519 $ : Last Committed Revision
 * $Date: 2010-06-11 14:26:03 -0700 (Fri, 11 Jun 2010) $ : Last Committed Date */
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

#ifndef _AGENT_LIB_H_
#define _AGENT_LIB_H_
EXPORTCH int MC_AclGetProtocol_chdl(void* varg);

EXPORTCH char* MC_AclGetConversationID_chdl(void *varg);

EXPORTCH int MC_AclGetPerformative_chdl(void *varg);

EXPORTCH int MC_AclGetSender_chdl(void *varg);
  
EXPORTCH char* MC_AclGetContent_chdl(void *varg);

EXPORTCH int
MC_AclDestroy_chdl(void* varg);

EXPORTCH void* 
MC_AclNew_chdl(void* varg);

EXPORTCH int
MC_AclPost_chdl(void* varg);

EXPORTCH void*
MC_AclReply_chdl(void* varg);

EXPORTCH void*
MC_AclRetrieve_chdl(void* varg);

EXPORTCH int
MC_AclSend_chdl(void* varg);

EXPORTCH void*
MC_AclWaitRetrieve_chdl(void *varg);

/* BEGIN Acl helper functions */

EXPORTCH int
MC_AclSetProtocol_chdl(void *varg);

EXPORTCH int
MC_AclSetConversationID_chdl(void *varg);

EXPORTCH int
MC_AclSetPerformative_chdl(void *varg);

EXPORTCH int
MC_AclSetSender_chdl(void *varg);

EXPORTCH int
MC_AclAddReceiver_chdl(void *varg);

EXPORTCH int
MC_AclAddReplyTo_chdl(void *varg);

EXPORTCH int
MC_AclSetContent_chdl(void *varg);
/* END Acl helper functions */

EXPORTCH int
MC_AddAgent_chdl(void *varg);

EXPORTCH int
MC_AgentAddTask_chdl(void *varg);

EXPORTCH int
MC_AgentAttachFile_chdl(void *varg);

EXPORTCH int
MC_AgentDataShare_Add_chdl(void *varg);

EXPORTCH int
MC_AgentDataShare_Retrieve_chdl(void *varg);

EXPORTCH int
MC_AgentListFiles_chdl(void *varg);

EXPORTCH int
MC_AgentRetrieveFile_chdl(void *varg);

EXPORTCH const void*
MC_AgentVariableRetrieve_chdl(void* varg);

EXPORTCH int
MC_AgentVariableSave_chdl(void *varg);

EXPORTCH int
MC_Barrier_chdl(void *varg);

EXPORTCH int
MC_BarrierDelete_chdl(void *varg);

EXPORTCH int
MC_BarrierInit_chdl(void *varg);

EXPORTCH int
MC_CallAgentFunc_chdl(void *varg);

EXPORTCH MCAgent_t
MC_ComposeAgent_chdl(void *varg);

EXPORTCH MCAgent_t
MC_ComposeAgentWithWorkgroup_chdl(void *varg);

EXPORTCH MCAgent_t 
MC_ComposeAgentFromFile_chdl(void *varg);

EXPORTCH MCAgent_t 
MC_ComposeAgentFromFileWithWorkgroup_chdl(void *varg);

EXPORTCH int
MC_CondBroadcast_chdl(void *varg);

EXPORTCH int
MC_CondWait_chdl(void *varg);

EXPORTCH int
MC_CondReset_chdl(void *varg);

EXPORTCH int
MC_CondSignal_chdl(void *varg);

EXPORTCH int
MC_DeleteAgent_chdl(void *varg);

EXPORTCH int
MC_DeleteAgentWG_chdl(void *varg);

EXPORTCH int
MC_DeregisterService_chdl(void *varg); 

EXPORTCH int
MC_DestroyServiceSearchResult_chdl(void* varg);

EXPORTCH int
MC_End_chdl(void *varg);

EXPORTCH MCAgent_t
MC_FindAgentByID_chdl(void *varg);

EXPORTCH MCAgent_t
MC_FindAgentByName_chdl(void *varg);

#if 0 
/* FIXME */
#ifndef _WIN32
EXPORTCH time_t
#else
EXPORTCH SYSTEMTIME
#endif
MC_GetAgentArrivalTime_chdl(void *varg);
#endif

EXPORTCH int
MC_GetAgentID_chdl(void *varg);

EXPORTCH char*
MC_GetAgentName_chdl(void *varg);

EXPORTCH int
MC_GetAgentNumTasks_chdl(void *varg);

EXPORTCH char *
MC_GetAgentXMLString_chdl(void *varg);

#ifndef _WIN32
EXPORTCH int
MC_GetTimeOfDay_chdl(void *varg);
#endif

EXPORTCH int
MC_HaltAgency_chdl(void *varg);

EXPORTCH int
MC_PrintAgentCode_chdl(void *varg);

EXPORTCH int
MC_MigrateAgent_chdl(void *varg);

EXPORTCH int
MC_MutexLock_chdl(void *varg);

EXPORTCH int
MC_MutexUnlock_chdl(void *varg);

EXPORTCH int
MC_RegisterService_chdl(void *varg);

EXPORTCH int
MC_ResumeAgency_chdl(void *varg);

EXPORTCH MCAgent_t
MC_RetrieveAgent_chdl(void *varg);

EXPORTCH char *
MC_RetrieveAgentCode_chdl(void *varg);

EXPORTCH int
MC_SearchForService_chdl(void *varg);

EXPORTCH int
MC_SemaphorePost_chdl(void *varg);

EXPORTCH int
MC_SemaphoreWait_chdl(void *varg);

EXPORTCH int
MC_SendSteerCommand_chdl(void *varg);

EXPORTCH int
MC_TerminateAgent_chdl(void *varg);

EXPORTCH int
MC_TerminateAgentWG_chdl(void *varg);

EXPORTCH int
MC_GetAgentStatus_chdl(void *varg);

EXPORTCH int
MC_SaveData_chdl(void* varg);

EXPORTCH int
MC_SendAgentMigrationMessage_chdl(void *varg);

EXPORTCH int
MC_SendAgentMigrationMessageFile_chdl(void *varg);

EXPORTCH int
MC_SetAgentStatus_chdl(void *varg);

EXPORTCH int
MC_SetDefaultAgentStatus_chdl(void *varg);

EXPORTCH int
MC_SyncDelete_chdl(void *varg);

EXPORTCH int
MC_SyncInit_chdl(void *varg);

#endif

