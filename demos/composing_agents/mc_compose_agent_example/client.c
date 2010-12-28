#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  MCAgent_t agent;
  MCAgencyOptions_t options;
  int local_port=5050;
  char address[100];

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);
  sprintf(address, "127.0.0.1:%d", 5051);

  agent = MC_ComposeAgent(
    "TestAgent",
    address,
    "localhost",
    "int main(void){\nprintf(\"hello world\\n\");\nreturn 0;\n}",
    NULL,
    address,
    0);

/* Agents composed with a workgroup code only allow other agents with a
 * matching workgroup code to perform certain operations on them, such as
 * deletion. To compose an agent with a workgroup code, use the following
 * function: */
 /*
  agent = MC_ComposeAgentS(
    "TestAgent",
    address,
    "localhost",
    "int main(void){\nprintf(\"hello world\\n\");\nreturn 0;\n}",
    NULL,
    address,
    "workgroup_code", // This text string can be anything. 
                     // Random secure text is recommended
    0
    );
  */

  MC_AddAgent(agency,agent);
#ifndef _WIN32
  sleep(5);
#else
  Sleep(5000);
#endif

  agent = MC_ComposeAgentFromFile(
    "MobileAgent1",
    address,
    "localhost",
    "agent.c",
    NULL,
    address,
    0);
  MC_AddAgent(agency,agent);

#ifndef _WIN32
  sleep(5);
#else
  Sleep(5000);
#endif

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile(agency, "agent.xml");


  MC_MainLoop(agency);
  MC_End(agency);
  exit(0);
}
