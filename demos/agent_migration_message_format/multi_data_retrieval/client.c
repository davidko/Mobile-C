#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>
#define TotalMA 2

int main()
{
  MCAgency_t agency;
  MCAgent_t agent;
  MCAgencyOptions_t options;
  int my_port = 5125;
  int remote_port1 = 5130, remote_port2 = 5052;
  int dim, i;
  const double *data;		
  char *name;  
  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(my_port, &options);
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  /* Sending to first host */
  MC_SendAgentFile(agency, "test1.xml");
  /* Sending to second host */	
  MC_SendAgentFile(agency, "test2.xml");	
  
 /* This loop is iterated until all mobile agents are received */
  for(i=0; i<TotalMA; i++){
	/* Wait for return-agent arrival signal */
	MC_WaitSignal(agency, MC_RECV_RETURN);
    
	/* Catching the mobile agent */  	
	agent = MC_RetrieveAgent(agency);
        name = MC_GetAgentName(agent);
        printf("%s\n", name);
	if (agent == NULL) {
		fprintf(stderr, "Did not receive correct agent. \n");
		exit(1);
	}
    dim = MC_AgentReturnArrayDim(agent, 0);
    data = MC_AgentReturnDataGetSymbolAddr(agent, 0);
    printf("Return Data from agent %d is %0.3f \n",i+1, data[0]);
    MC_DeleteAgent(agent);
    MC_ResetSignal(agency); 	
  }// end for
  MC_End(agency);
  exit(0);
}
