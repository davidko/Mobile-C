#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port=5050;
  int i;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile(agency, "test1.xml");
	printf("Sending termination agent in 3 seconds...\n");
  for(i = 2; i >= 1; i--) {
#ifndef _WIN32
    sleep(1);
#else
    Sleep(1000);
#endif
    printf("%d...\n", i);
  }

  MC_SendAgentFile(agency, "test2.xml");
  MC_End(agency);
  exit(0);
}
