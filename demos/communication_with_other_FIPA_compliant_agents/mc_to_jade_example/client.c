#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

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
  printf("Sending agent to self...\n");
  MC_LoadAgentFromFile(agency,
      "agent.xml");
  printf("Done.\n");
  MC_MainLoop(agency);
  exit(0);
}
