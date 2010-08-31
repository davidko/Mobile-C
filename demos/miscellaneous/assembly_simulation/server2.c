#include <libmc.h>
#include <stdio.h>
int main()
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int local_port = 5052;
    int i;
    MC_InitializeAgencyOptions(&options);
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    /* If the following line is uncommented, the command prompt will be
     * turned off. */
 /*   MC_SetThreadOff(&options, MC_THREAD_CP);  */

    agency = MC_Initialize(
            local_port,
            &options);


    /* Wait forever... */
    MC_MainLoop(agency);
    return 0;
}

