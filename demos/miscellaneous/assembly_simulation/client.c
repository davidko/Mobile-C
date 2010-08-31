#include <stdio.h>
#include <libmc.h>
#ifdef _WIN32
#include <windows.h>
#endif
#define WAIT_TIME 5
int main() 
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int i;
    int local_port = 5050;
    int remote_port = 5051;
    MC_InitializeAgencyOptions(&options);
    /* We want _all_ the threads on: EXCEPT, the command prompt thread */
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    
    MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */

    agency = MC_Initialize(local_port, &options);

    MC_SendAgentFile( agency, "assembly_agent.xml");
    printf("Terminating...\n");
    MC_End(agency);
    return 0;
}
