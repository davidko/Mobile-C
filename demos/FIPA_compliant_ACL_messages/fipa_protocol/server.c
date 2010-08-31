#include <stdio.h>
#include <string.h>
#include <libmc.h>

MCAgent_t makeAgent(int local_port, char* filename, char* agentName)
{
  MCAgent_t agent;
  char  code[20000]={0};
  char  address[100];
  FILE* fptr;

  fptr = fopen(filename,"r");
  fread(code, 1, 20000, fptr);
  fclose(fptr);

  sprintf(address, "monkey.engr.ucdavis.edu:%d", local_port);

  agent = MC_ComposeAgent(agentName, address, 
     "monkey.engr.ucdavis.edu", code, NULL, address, 0);
  return agent;
}

int main(int argc, char** argv) {
    MCAgency_t agency;
    MCAgent_t agent;
    MCAgencyOptions_t options;
    int local_port = 5050;

    if(argc == 2) local_port = atoi(argv[1]);
    MC_InitializeAgencyOptions(&options);
    MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
    agency = MC_Initialize(local_port, &options);

    printf("\n---- FIPA COMM TEST ----\n\n");

    agent = makeAgent(local_port, "listener.c", "listener");
    MC_AddAgent(agency, agent);

    if(MC_MainLoop(agency) != 0) {
        MC_End(agency);
        return -1;
    }

    return 0;
}
