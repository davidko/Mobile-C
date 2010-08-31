#include <libmc.h>
#include <stdio.h>
int main()
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int i;
    int local_port=5051;

    MC_InitializeAgencyOptions(&options);

    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    /* if the following line is uncommented, the command prompt
     * will be disabled. */
 /*   MC_SetThreadOff(&options, MC_THREAD_CP);  */

    agency = MC_Initialize(
            local_port,
            &options);

    /* Wait forever... */
    MC_MainLoop(agency);
    return 0;
}

