#include <libmc.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
int main()
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    MCAgent_t agent;
    char *str;
    int i;
    int local_port=5051;

    MC_InitializeAgencyOptions(&options);
    
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    MC_SetThreadOff(&options, MC_THREAD_CP);  /* Turn off the command prompt */

    agency = MC_Initialize(
            local_port,
            &options);
	MC_ResetSignal(agency);
    /* Retrieve the first arriving agent */
    /* Note: MC_WaitRetrieveAgent() pauses the agency: We'll need to unpause 
     * it later with MC_SignalReset() */
    agent = MC_WaitRetrieveAgent(agency);
	if (agent != NULL)
	{
		printf("The agent status is: %d\n", MC_GetAgentStatus(agent));
		printf("This agent has %d task(s).\n", MC_GetAgentNumTasks(agent));
		str = MC_GetAgentXMLString(agent);
		printf("Agent XML String:\n%s\n", str);
		free(str);
		str = MC_RetrieveAgentCode(agent);
		printf("Agent Code:\n%s\n", str);
		free(str);
		MC_ResetSignal(agency);
		MC_MainLoop(agency);
	}
	else
		printf("Error: returned NULL pointer for agent.\n");

    return 0;
}

