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
MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);
  printf("Welcome to the Mobile-C agent workgroup demonstration example.\n");
  printf("To begin, the demonstration, we will send a worker agent named \n\
\"agent1\" to the server agency. \"agent1\" is not protected by a workgroup\n\
code. Press the return key to send agent1.\n");
  getchar();
  agent = MC_ComposeAgentFromFile(
      "agent1",      /* Name */
      "localhost:5050", /* Home */
      "IEL",            /* Owner */
      "agent1.c",  /* Filename */
      NULL,             /* Return var name. NULL for no return */
      "localhost:5051", /* Server to execute task on */
      0 );              /* Persistent. 0 for no persistence. */

  /* Add the agent to the agency to start it */
  MC_AddAgent(agency, agent);

  printf("agent1 has been sent. You should be able to see agent1's arrival\n\
message on the server agency, and it should begin printing periodic\n\
messages to show that it is working. \n\n\
Next, we will send another worker agent, except this second agent,\n\
\"agent2\", will be protected by a secret workgroup code. This workgroup\n\
code is kept secret and only shared among agents that trust each other.\n\
In our example, the workgroup code we have assigned to \"agent2\" is\n\
\"12345abCD\". Press the return key to send agent2.\n");
  getchar();
  agent = MC_ComposeAgentFromFileWithWorkgroup(
      "agent2",      /* Name */
      "localhost:5050", /* Home */
      "IEL",            /* Owner */
      "agent2.c",  /* Filename */
      NULL,             /* Return var name. NULL for no return */
      "localhost:5051", /* Server to execute task on */
      0,               /* Persistent. 0 for no persistence. */
      "12345abCD");         /* Workgroup */

  /* Add the agent to the agency to start it */
  MC_AddAgent(agency, agent);

  printf("You should now notice two agents performing work at the server\n\
agency. Next, we will send a malicious agent to the server who's goal is to\n\
terminate other agents. This malicous agent does not belong to any workgroups.\n\
Press the return key to send agent3, the malicious agent.\n");
  getchar();
  agent = MC_ComposeAgentFromFile(
      "agent3",      /* Name */
      "localhost:5050", /* Home */
      "IEL",            /* Owner */
      "agent3.c",  /* Filename */
      NULL,             /* Return var name. NULL for no return */
      "localhost:5051", /* Server to execute task on */
      0 );              /* Persistent. 0 for no persistence. */
  MC_AddAgent(agency, agent);

  printf("You should notice now that agent1 has been terminated, but agent2\n\
is still performing its task. This is due to the fact that since agent1 did \n\
not belong to any workgroups, any agent is able to modify it or terminate\n\
it. However, since agent2 belongs to a different workgroup than agent3,\n\
agent3 is unable to terminate agent2.\n\n\
Next, we will send an agent in the same workgroup as agent2 to try to\n\
terminate agent2. Press the return key to send agent4, who is also in\n\
the \"12345abCD\" workgroup who will try to terminate agent2.\n");
  getchar();
  agent = MC_ComposeAgentFromFileWithWorkgroup(
      "agent4",      /* Name */
      "localhost:5050", /* Home */
      "IEL",            /* Owner */
      "agent4.c",  /* Filename */
      NULL,             /* Return var name. NULL for no return */
      "localhost:5051", /* Server to execute task on */
      0,               /* Persistent. 0 for no persistence. */
      "12345abCD");          /* Workgroup */

  /* Add the agent to the agency to start it */
  MC_AddAgent(agency, agent);

  printf("You should notice now that all activity has ceased on the server\n\
agency. agent4 was successfully able to terminate agent2 because they belong\n\
to the same workgroup.\n\n\
Press the return key to end this demo.\n");

  MC_End(agency);
  exit(0);
}
