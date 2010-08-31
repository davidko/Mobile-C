#include <libmc.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#define COND_ID 55

int main()
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int i;
    int local_port = 5051;
    MC_InitializeAgencyOptions(&options);
    /* We want _all_ the threads on: EXCEPT, the command prompt thread */
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    MC_SetThreadOff(&options, MC_THREAD_CP);  /* Turn off command prompt */

    agency = MC_Initialize(
            local_port,
            &options);

    MC_SyncInit(agency, COND_ID);
    /* Let us wait on a condition variable. Every time an agent signals that 
     * variable, we will perform some task. */
    while(1) {
        MC_CondWait(agency, COND_ID);
        printf("C space: I am awake! Performing some task.\n");
        MC_CondReset(agency, COND_ID);
    }

    MC_MainLoop(agency);
    return 0;
}

