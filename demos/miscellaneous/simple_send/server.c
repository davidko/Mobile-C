#include <stdio.h>
#include <libmc.h>
#ifndef _WIN32
#include <stdlib.h>
#endif

int main() 
{
  MCAgency_t agency;
  int local_port = 5051;
  
  agency = MC_Initialize(local_port, NULL);

#ifndef _WIN32
  sleep(10);
#else
  Sleep(10000);
#endif
  MC_End(agency);
  return 0;
}
