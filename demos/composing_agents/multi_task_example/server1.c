/* File: multi_task_example/server1.c */

#include <stdio.h>
#include <libmc.h>

int main() 
{
    MCAgency_t agency;
    int local_port=5051;
    agency = MC_Initialize(local_port, NULL);

    MC_MainLoop(agency);
    return 0;
}
