#include <stdio.h>
#include <libmc.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port = 5050;
 
  MC_InitializeAgencyOptions(&options);
  /* If the following line is uncommented, the command prompt
   * will be disabled. */
  /*MC_SetThreadOff(&options, MC_THREAD_CP); */

  agency = MC_Initialize(local_port, &options);
  
  MC_LoadAgentFromFile(agency, "test1.xml");
  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
