/* mc_sample_app.c
 *
 * This sample program uses the Mobile C library to build
 * a simple command-line driven client/server app. 
 * 
 * 12/15/2006
 * */

#include <libmc.h>
#include <stdio.h>
int main()
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int local_port=5051;
    int i;
    /* We want _all_ the threads on: EXCEPT, the command prompt thread */
    MC_InitializeAgencyOptions(&options);
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i); /* Turn all threads on */
    }
    /* If the following line is uncommented, the command prompt will be turned
     * off. */
    /*MC_SetThreadOff(&options, MC_THREAD_CP);  */ 

    agency = MC_Initialize(
            local_port,
            &options);

    /* Wait forever... */
    MC_MainLoop(agency);
    return 0;
}

