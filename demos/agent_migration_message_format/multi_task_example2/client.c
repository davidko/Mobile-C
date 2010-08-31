/* File: multi_task_example/client.c */

#include <stdio.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  MCAgent_t agent;
  int dim, *extent;
  double *data;
  int i, j, size;
  int local_port=5050;
  int remote_port=5051;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile(agency, "test.xml");

  /* Wait for return-agent arrival signal */
  MC_WaitSignal(agency, MC_RECV_RETURN);

  /* Make sure we caught the correct agent */
  agent = MC_FindAgentByName(agency, "mobagent3");
  if (agent == NULL) {
    fprintf(stderr, "Did not receive correct agent. \n");
    exit(1);
  }

  /* Print relevant information */
  printf("%d tasks.\n", MC_GetAgentNumTasks(agent) );
  for (i = 0; i < MC_GetAgentNumTasks(agent); i++) {
    MC_GetAgentReturnData(
        agent,
        i,
        (void**)&data,
        &dim,
        &extent );
    printf("Task: %d\n", i);
    size = 1;
    printf("dim is %d\n", dim);
    for (j = 0; j < dim; j++) {
      size *= extent[j];
    }
    printf("Size: %d\n", size);
    printf("Data elements: ");
    for (j = 0; j < size; j++) {
      printf("%f ", data[j]);
    }
    printf("\n\n");
    free(data);
    free(extent);
  }

  /* We must reset the signal that we previously caught with the 
   * MC_WaitSignal() function with MC_ResetSignal() */
  MC_ResetSignal(agency);

  MC_End(agency);
  return 0;
}
