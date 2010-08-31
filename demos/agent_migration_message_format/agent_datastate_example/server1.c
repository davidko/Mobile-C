#include <stdio.h>
#include <libmc.h>

int main() 
{
  int local_port=5051;
  MCAgency_t agency;
  agency = MC_Initialize(local_port, NULL);

  MC_MainLoop(agency);
  return 0;
}
