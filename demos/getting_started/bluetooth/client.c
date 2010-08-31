/* File: getting_started/bluetooth/client.c */

#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  MCAgent_t agent;
  int local_port=20;

  MC_InitializeAgencyOptions(&options);
  options.bluetooth = 1;
  agency = MC_Initialize(local_port, &options);

  agent = MC_ComposeAgentFromFile(
      "mobagent1",      /* Name */
      "localhost:5050", /* Home */
      "IEL",            /* Owner */
      "hello_world.c",  /* Filename */
      NULL,             /* Return var name. NULL for no return */
      "00:80:37:27:03:D8 20", /* Server to execute task on */
                           /* Note that the address is a bluetooth
                            * device address, aka MAC address. The number after
                            * the space is the RFCOMM channel to send to. */
      0 );              /* Persistent. 0 for no persistence. */

  /* Add the agent to the agency to start it */
  MC_AddAgent(agency, agent);

  MC_MainLoop(agency);
  MC_End(agency);
  exit(0);
}
