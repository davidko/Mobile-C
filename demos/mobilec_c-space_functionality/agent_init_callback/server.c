/* File: hello_world/server.c */

#include <stdio.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  int local_port = 5051;
  setbuf(stdout, NULL);
  
  agency = MC_Initialize(local_port, NULL);

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
