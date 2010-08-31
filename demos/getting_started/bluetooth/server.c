/* File: getting_started/bluetooth/server.c */

#include <stdio.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  int local_port = 20; /* Bluetooth RFCOMM ports only go from 0 to 30 */

  MCAgencyOptions_t options;
  MC_InitializeAgencyOptions(&options);
  options.bluetooth = 1;
  options.initInterps = 2;

  printf("Initializing...\n");  
  agency = MC_Initialize(local_port, &options);
  printf("Done Initializing.\n");

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
