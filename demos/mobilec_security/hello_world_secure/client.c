#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>
#include <string.h>

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port=5125;
  int remote_port = 5126;
  char remote_host[] = "localhost";
  
  MC_InitializeAgencyOptions(&options);
  strcpy( options.passphrase, "alpha1234");

  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  printf("Sending agent...");
  MC_SendAgentFile(agency, "test1.xml");
  printf(" Done.\n");
  MC_End(agency);
  exit(0);
}
