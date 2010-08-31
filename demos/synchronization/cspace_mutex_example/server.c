#include <libmc.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#define MUTEX_ID 55
int main()
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int i;
    int local_port=5051;

    MC_InitializeAgencyOptions(&options);

    /* We want _all_ the threads on: EXCEPT, the command prompt thread */
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    MC_SetThreadOff(&options, MC_THREAD_CP);  

    agency = MC_Initialize(
            local_port,
            &options);

    MC_SyncInit(agency, MUTEX_ID);
    /* Now, lets perform a simulated task which accesses a shared resource
     * 20 times. */
    for(i = 0; i < 20; i++) {
        printf("C Space: Attempting to lock mutex...\n");
        MC_MutexLock(agency, MUTEX_ID);
        printf("C Space: Mutex Locked. Performing task.\n");
#ifndef _WIN32
        sleep(1);
#else
        Sleep(1000);
#endif
        printf("C Space: Unlocking Mutex...\n");
        MC_MutexUnlock(agency, MUTEX_ID);
        printf("C Space: Mutex Unlocked.\n");
    }

    MC_SyncDelete(agency, MUTEX_ID);
    MC_End(agency);
    return 0;
}

