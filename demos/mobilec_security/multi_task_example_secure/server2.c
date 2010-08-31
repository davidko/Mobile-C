#include <stdio.h>
#include <libmc.h>
#include <string.h>

int main() 
{
    MCAgency_t agency;
    MCAgencyOptions_t options; 		
    int local_port=5127;

    printf("Hello I am task server 2 \n");

    MC_InitializeAgencyOptions(&options);
    strcpy(options.passphrase, "alpha1234");

    agency = MC_Initialize(local_port, &options);

    MC_MainLoop(agency);
    return 0;
}
