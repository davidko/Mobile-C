/* File: hello_world/client.c */

#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main(int argc, char *argv[]) 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port=5050;

  MC_InitializeAgencyOptions(&options);
  //MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  printf(
  "Agency 1 started. Please start the second agency by running the command \n\
  \"server2\" on another terminal.\n");  

  MC_MainLoop(agency);
  MC_End(agency);
  return 0;
}
