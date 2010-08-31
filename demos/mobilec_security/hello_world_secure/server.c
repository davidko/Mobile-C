#include <stdio.h>
#include <libmc.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main() 
{
  MCAgency_t agency;
  int local_port = 5126;
  //unsigned char passphrase[] = "alpha1234";
  MCAgencyOptions_t options;

  MC_InitializeAgencyOptions(&options);  
  strcpy(options.passphrase, "alpha1234");
   
  agency = MC_Initialize(local_port, &options);
  
  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}

