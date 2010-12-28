/* File: stationary_agent_communication/client.c */

#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>
#include <fipa_acl.h>

void* stationary_agent_func(stationary_agent_info_t* stationary_agent_info)
{
  /* Wait for and receive a message */
  fipa_acl_message_t* acl_message;

  printf("Stationary agent online.\n");
  printf("Creating a new ACL Message...\n");
  acl_message = MC_AclNew();
  MC_AclSetPerformative(acl_message, FIPA_INFORM);
  MC_AclSetSender(acl_message, "agent2", "http://localhost:5050/acc");
  MC_AclAddReceiver(acl_message, "agent1", "http://localhost:5051/acc");
  MC_AclSetContent(acl_message, "Hello agent1");
  printf("Sending ACL Message...\n");
  MC_AclSend(
      MC_AgentInfo_GetAgency(stationary_agent_info), acl_message);
  printf("Waiting for reply...\n");
  acl_message = MC_AclWaitRetrieve(
      MC_AgentInfo_GetAgent(stationary_agent_info));
  if (acl_message != NULL) {
    printf("Received an acl message.\n");
    printf("Acl message content is \"%s\"\n", 
        MC_AclGetContent(acl_message));
  } else {
    printf("Error retrieving ACL message\n");
  }
#ifndef _WIN32
  fflush(stdout);
#endif

  return NULL;
}

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port=5050;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_AddStationaryAgent(agency, stationary_agent_func, "agent2", NULL);

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
