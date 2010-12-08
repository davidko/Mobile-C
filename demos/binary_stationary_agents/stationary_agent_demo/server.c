#include <stdio.h>
#include <libmc.h>
#include <fipa_acl.h>

void* stationary_agent_func(stationary_agent_info_t* stationary_agent_info)
{
	/* Wait for and receive a message */
	fipa_acl_message_t* acl_message;

	printf("Stationary agent online.\n");
	printf("Stationary agent waiting for ACL message...\n");
	acl_message = MC_AclWaitRetrieve(MC_AgentInfo_GetAgent(stationary_agent_info));
	if (acl_message != NULL) {
		printf("Received an acl message.\n");
		printf("Acl message content is \"%s\"\n", MC_AclGetContent(acl_message));
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
  int local_port = 5051;
 
  MC_InitializeAgencyOptions(&options);
  /* If the following line is uncommented, the command prompt
   * will be disabled. */
  /*MC_SetThreadOff(&options, MC_THREAD_CP); */

  agency = MC_Initialize(local_port, &options);

	MC_AddStationaryAgent(agency, stationary_agent_func, "agent1", NULL);

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
