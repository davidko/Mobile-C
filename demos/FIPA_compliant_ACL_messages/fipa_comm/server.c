#include <stdio.h>
#include <string.h>
#include <libmc.h>

int main(int argc, char** argv) {
    MCAgency_t agency;
    MCAgent_t agent;
    MCAgencyOptions_t options;
    int local_port = 8866;

    MC_InitializeAgencyOptions(&options);
    MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
    agency = MC_Initialize(local_port, &options);

    printf("\n---- FIPA COMM TEST ----\n\n");

    printf("Loading listener agent\n");
    agent = MC_ComposeAgentFromFile(
      "listen",
      "127.0.0.1:8866",
      "localhost",
      "agents/listener.c",
      NULL,
      "127.0.0.1:8866",
      0);
    MC_AddAgent(agency, agent);

#ifndef _WIN32
    sleep(1);
#else
    Sleep(1000);
#endif

    printf("\nLoading talker agent\n");
    agent = MC_ComposeAgentFromFile(
      "talk",
      "127.0.0.1:8866",
      "localhost",
      "agents/talker.c",
      NULL,
      "127.0.0.1:8866",
      0);
    MC_AddAgent(agency, agent);

    if(MC_MainLoop(agency) != 0) {
        MC_End(agency);
        return -1;
    }

    return 0;
}
