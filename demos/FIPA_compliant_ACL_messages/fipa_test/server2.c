/* File: fipa_test/server2.c */

#include <stdio.h>
#include <libmc.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port = 5052;
 
  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOn(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
