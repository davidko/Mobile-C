#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>
#define TotalMA 2

int main()
{
  MCAgency_t agency;
  MCAgent_t agent;
  MCAgent_t* agents;
  int num_agents;
  MCAgencyOptions_t options;
  int my_port = 5125;
  int dim, i;
  const double *data;		
  char *name;  
  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(my_port, &options);
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  /* Sending to first host */
  MC_SendAgentFile(agency, "test1.xml");
  /* Sending to second host */	
  MC_SendAgentFile(agency, "test2.xml");	
  MC_SendAgentFile(agency, "test3.xml");	

  /* Wait for a few seconds for agents to return */
  printf("Waiting for agents to return...\n");
#ifdef _WIN32
  Sleep(3000);
#else
  sleep(3);
#endif

  /* Try getting all of the active agents */
  printf("Getting active agents...\n");

  /* First lock the agent queue so agents stick around while we print them */
  MC_AgentProcessingBegin(agency);
  MC_GetAgents(agency, &agents, &num_agents, (1<<MC_AGENT_ACTIVE));
  for(i = 0; i < num_agents; i++) {
    printf("Agent name: %s\n", MC_GetAgentName(agents[i]));
  }

  free(agents);
  /* Try getting all neutral agents */
  printf("Getting neutral agents...\n");
  MC_GetAgents(agency, &agents, &num_agents, (1<<MC_AGENT_NEUTRAL));
  for(i = 0; i < num_agents; i++) {
    printf("Agent name: %s\n", MC_GetAgentName(agents[i]));
  }
  /* Agent processing done */
  MC_AgentProcessingEnd(agency);
  
  MC_End(agency);
  exit(0);
}
