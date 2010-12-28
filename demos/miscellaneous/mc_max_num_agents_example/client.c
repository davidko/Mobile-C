#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>
#ifndef _WIN32
#include <stdlib.h>
#endif

int main() 
{
  MCAgency_t agency;
  MCAgent_t agent;
  MCAgencyOptions_t options;
  int local_port=5050;
  char address[100];
  char agent_name[80];
  int i;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);
  sprintf(address, "127.0.0.1:%d", 5050);

  for(i = 0; ; i++) { 
    printf("Adding agent number %d.\n", i);
    sprintf(agent_name, "MobileAgent%d", i);
    agent = MC_ComposeAgentFromFile(
        agent_name,
        address,
        "localhost",
        "agent.c",
        NULL,
        address,
        0);
    MC_AddAgent(agency,agent);

#ifndef _WIN32
    sleep(1);
#else
    Sleep(1000);
#endif
  }

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile(agency, "agent.xml");


  MC_MainLoop(agency);
  MC_End(agency);
  exit(0);
}
