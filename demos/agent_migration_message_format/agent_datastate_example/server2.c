#include <stdio.h>
#include <libmc.h>

int main() 
{
    MCAgency_t agency;
    int local_port=5052;
    agency = MC_Initialize(local_port, NULL);

    MC_MainLoop(agency);
    return 0;
}
