/* File: hello_world/client.c */

#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  MCAgent_t agent;
  int local_port=5050;
  int remote_port = 5051;

  MC_InitializeAgencyOptions(&options);
//MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  agent = MC_ComposeAgentFromFile(
      "mobagent1",      /* Name */
      "localhost:5050", /* Home */
      "IEL",            /* Owner */
      "hello_world.c",  /* Filename */
      NULL,             /* Return var name. NULL for no return */
      "localhost:5051", /* Server to execute task on */
      0 );              /* Persistent. 0 for no persistence. */

  /* Add the agent to the agency to start it */
  MC_AddAgent(agency, agent);

  MC_MainLoop(agency);
  MC_End(agency);
  exit(0);
}
