#include <libmc.h>

#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

int main()
{
  /* Init the agency */
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port=5050;
  int remote_port=5051;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "test1.xml");
  MC_End(agency);

  return 0;
}
