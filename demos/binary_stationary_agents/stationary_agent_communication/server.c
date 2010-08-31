/* File: stationary_agent_communication/server.c */

#include <stdio.h>
#include <libmc.h>
#include <fipa_acl.h>

void* stationary_agent_func(stationary_agent_info_t* stationary_agent_info)
{
  /* Wait for and receive a message */
  fipa_acl_message_t* acl_message;
  fipa_acl_message_t* reply_message;

  printf("Stationary agent online.\n");
  printf("Stationary agent waiting for ACL message...\n");
  acl_message = MC_AclWaitRetrieve(MC_AgentInfo_GetAgent(stationary_agent_info));
  if (acl_message != NULL) {
    printf("Received an ACL message.\n");
    printf("ACL message content is \"%s\"\n", 
        MC_AclGetContent(acl_message));
    printf("Composing a reply to the message...\n");
    reply_message = MC_AclReply(acl_message);
    MC_AclSetPerformative(reply_message, FIPA_INFORM);
    MC_AclSetSender(reply_message, "agent1", "http://localhost:5051/acc");
    MC_AclSetContent(reply_message, "Hello to you too, agent2!");
    MC_AclSend(
        MC_AgentInfo_GetAgency(stationary_agent_info), 
        reply_message);
  } else {
    printf("Error retrieving ACL message\n");
  }
  return NULL;
}

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port = 5051;

  MC_InitializeAgencyOptions(&options);
  /* If the following line is uncommented, the command prompt
   * will be disabled. */
  MC_SetThreadOff(&options, MC_THREAD_CP); 

  agency = MC_Initialize(local_port, &options);

  MC_AddStationaryAgent(agency, stationary_agent_func, "agent1", NULL);

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
