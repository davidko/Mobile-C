/* mc_sample_app.c
 *
 * This sample program uses the Mobile C library to build
 * a simple command-line driven client/server app. 
 * 
 * 12/15/2006
 * */

#include <libmc.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int i;
    int local_port=5050;

    /* We want _all_ the threads on: including the command
     * prompt thread, which is off by default */

    MC_InitializeAgencyOptions(&options);

    /* Turn on all threads.
     * Note: This is actually not necessary, since they are all on by default,
     * but this code does provide a good example of how to manipulate MobileC
     * threads. */
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }

    if (argc == 2) {
        printf("Starting agency listening on local_port %d.\n",
                atoi(argv[1]) );
        agency = MC_Initialize(
                atoi(argv[1]),
                &options
                );
    } else {
        agency = MC_Initialize(
                local_port,
                &options);
    }

    MC_End(agency);
    return 0;
}

