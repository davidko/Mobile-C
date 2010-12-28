/* File: fipa_test/client.c */

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
  MC_SendAgentFile(agency, "test1.xml");
  printf("Sending next agent in 3 seconds...\n");
#ifndef _WIN32
  sleep(3);
#else
  Sleep(3000);
#endif
  MC_SendAgentFile(agency, "test2.xml");
  MC_End(agency);
  exit(0);
}
