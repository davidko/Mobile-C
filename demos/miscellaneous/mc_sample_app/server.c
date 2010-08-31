#include <libmc.h>
#include <string.h>
#include <embedch.h>
int main() {
    MCAgency_t agency;
    int local_port = 5130;

    /***************************************************************** 
     * A typical home directory of Embedded Ch on Windows would be   * 
     * like "C:/Program Files/Company Name/program/embedch". We used * 
     * "C:/Ch/toolkit/embedch" for testing purposes.                 *
     *****************************************************************/
    char embedchhome[] = "C:/Ch/toolkit/embedch";
    ChOptions_t* ch_options;

    MCAgencyOptions_t mc_options;

    MC_InitializeAgencyOptions(&mc_options);

    ch_options = (ChOptions_t*)malloc(sizeof(ChOptions_t));

    ch_options->shelltype = CH_REGULARCH;
    ch_options->chhome = malloc(strlen(embedchhome)+1);
    strcpy(ch_options->chhome, embedchhome);

    mc_options.ch_options = ch_options;


    agency = MC_Initialize(local_port, &mc_options);

    if(MC_MainLoop(agency) != 0) {
        MC_End(agency);
        return -1;
    }

    return 0;
}
