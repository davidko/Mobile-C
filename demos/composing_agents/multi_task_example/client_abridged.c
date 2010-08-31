/* File: multi_task_example/client.c */

#include <stdio.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  MCAgent_t agent;
  double *agent_return_value;
  int task_num;
  int local_port=5050;
  int remote_port=5051;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  /* Compose the agent from a task source file */
  agent = MC_ComposeAgentFromFile(
      "mobagent3",        /* Name */
      "localhost:5050",   /* Home - This is the host the agent will return to
                           * when it has finished its tasks. */
      "IEL",              /* Owner */
      "task1.c",          /* Code filename */
      "results_task1",     /* Return Variable Name */
      "localhost:5051",   /* server/destination host */
      0                   /* persistent */
      );

  /* Add one more task */
  MC_AgentAddTaskFromFile(
      agent,              /* Agent handle */
      "task2.c",          /* Task code file name */
      "results_task2",       /* Return Variable Name */
      "localhost:5052",   /* server/destination host */
      0 );                /* Persistent */
  
  /* Add the agent */
  MC_AddAgent(agency, agent);

  MC_End(agency);
  return 0;
}
