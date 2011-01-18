#include <libmc.h>
#include <embedch.h>
#include <stdio.h>

int main()
{
  MCAgency_t agency;
  MCAgent_t agent;
  int retval;
  MCAgencyOptions_t options;
  int local_port=5051;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */

  /* Init the agency */
  agency = MC_Initialize(
      local_port,
      &options);

  printf("Please press 'enter' once the sample agent has arrived.\n");
  getchar();
  agent = MC_FindAgentByName(agency, "mobagent1");
  if (agent == NULL) {
    printf("Could not find agent!\n");
    exit(0);
  }
  /* The following executution of code may be performed two different
ways: The first way, which is commented out in this example, 
involves retrieving the agent's interpreter with
MC_GetAgentExecEngine() and using the Embedded Ch api to call
the function. The second method involves using the Mobile-C
api to call the function. Both of these methods used here produce
identical results. */
  MC_CallAgentFunc(
      agent,
      "hello",
      &retval,
      2, /* Num Arguments */
      5,
      7 );

  printf("Value of %d was returned.\n", retval);

  /* End the persistent agent */
  MC_DeleteAgent(agent);

  MC_End(agency);
  return 0;
}
