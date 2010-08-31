/* mc_sample_app.c
 *
 * This sample program uses the Mobile C library to build
 * a simple command-line driven client/server app. 
 * 
 * 12/15/2006
 * */

#ifndef _WIN32
#include <libmc.h>
#else
#include <libmc.h>
#include <windows.h>
#endif
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif

int main()
{
  /* Init the agency */
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port=5051;
  int remote_port=5050;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "comm_agent.xml");
#ifndef _WIN32
  sleep(1);
#else
  Sleep(1000);
#endif
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "agent_1.xml");
#ifndef _WIN32
  sleep(1);
#else
  Sleep(1000);
#endif
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "agent_2.xml");

  MC_End(agency);
  return 0;
}
