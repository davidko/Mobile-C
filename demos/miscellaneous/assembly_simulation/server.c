#include <libmc.h>
#include <stdio.h>
int main()
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int i;
    int local_port = 5051;
    int remote_port = 5052;
    MC_InitializeAgencyOptions(&options);

    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    /* If the following line is uncommented, the command prompt will
     * be turned off. */
 /*   MC_SetThreadOff(&options, MC_THREAD_CP);  */

    agency = MC_Initialize(
            local_port,
            &options);

    /* Set up a condition variable */
    MC_SyncInit(agency, 42);
    
    /* Wait on the condition variable in an infinite loop. Every time the 
     * condition variable is signalled, send our 'data retrieval' agent. */
    while(1) {
      MC_CondWait(agency, 42);
      MC_SendAgentFile( agency, "retrieval_agent.xml");
      MC_CondReset(agency, 42);
    }

    /* Wait forever... */
    MC_MainLoop(agency);
    return 0;
}

