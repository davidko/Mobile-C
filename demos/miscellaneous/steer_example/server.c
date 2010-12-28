#include <stdio.h>
#include <libmc.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#endif

int algorithm(void* boo);

int main() {
    MCAgency_t agency;
    int local_port = 5050;
    MCAgencyOptions_t options;

    MC_InitializeAgencyOptions(&options);

    MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */

    agency = MC_Initialize(local_port, &options);

    printf("Starting algorithm...\n");
    MC_Steer(
        agency,
        &algorithm,
        NULL
        ); 

    MC_End(agency);
    return 0;
}

int algorithm(void* boo)
{
  int i=0;
  MC_SteerCommand_t command;
  while(1) {
#ifndef _WIN32
    sleep(1);
#else
    Sleep(1000);
#endif
    printf("%d \n", i);
    i++;
    command = MC_SteerControl();
    if( 
        command == MC_RESTART ||
        command == MC_STOP
      ) 
    {
      return 0;
    }
  }
}
    
