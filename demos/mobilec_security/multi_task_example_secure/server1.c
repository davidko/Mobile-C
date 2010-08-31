#include <stdio.h>
#include <libmc.h>
#include <string.h>

int main() 
{
    MCAgency_t agency;
    int local_port=5126;
    MCAgencyOptions_t options;
 
    printf("Hello I am task server 1 \n");
 
    MC_InitializeAgencyOptions(&options);
    strcpy(options.passphrase, "alpha1234");
 
    agency = MC_Initialize(local_port, &options);

    MC_MainLoop(agency);
    return 0;
}
