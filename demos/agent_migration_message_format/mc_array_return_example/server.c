/* File: mc_array_return_example/server.c */
#include <libmc.h>
#include <stdio.h>
int main()
{
    MCAgency_t agency;
    int i;
    int local_port=5050;
    agency = MC_Initialize(
        local_port,
        NULL);

    MC_MainLoop(agency);
    MC_End(agency);
    return 0;
}

