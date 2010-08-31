#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main(int argc, char *argv[]) 
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int i;
    char* filename;
    char* hostname;
    int port;
    int local_port=5050;
    /* We want _all_ the threads on: EXCEPT, the command prompt thread */
    MC_InitializeAgencyOptions(&options);
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */

    filename = argv[1];
    hostname = argv[2];
    port = atoi(argv[3]);
    agency = MC_Initialize(local_port, &options);

 /* Note: The third argument of the following function may also be a
    valid IP address in the form of a string. i.e. 192.168.0.1 */
    MC_SendAgentFile(agency, filename);
    MC_End(agency);
    return 0;
}
