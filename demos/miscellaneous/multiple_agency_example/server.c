#include <libmc.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
int main()
{
    MCAgency_t agency1;
    MCAgency_t agency2;
    MCAgencyOptions_t options;
    int i;
    int port1 = 5051;
    int port2 = 5052;

    MCAgent_t agent;
    MCAgent_t agent_copy;

    MC_InitializeAgencyOptions(&options);

    /* We want _all_ the threads on: EXCEPT, the command prompt thread */
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */

    agency1 = MC_Initialize(
        port1,
        &options);
    agency2 = MC_Initialize(
        port2,
        &options);

    while(1) {
      agent = MC_WaitRetrieveAgent(agency1);
      MC_CopyAgent(&agent_copy, agent);
      MC_SetAgentStatus(agent_copy, MC_WAIT_CH);
      MC_AddAgent(agency2, agent_copy);
      MC_ResetSignal(agency1);
    }

    return 0;
}

