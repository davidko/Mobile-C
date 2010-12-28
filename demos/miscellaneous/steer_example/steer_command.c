#include <stdio.h>
#include <string.h>
#include <libmc.h>
int main()
{
    MCAgency_t agency;
    MCAgencyOptions_t options;
    int i;
    char buf[50];
    int local_port=5051;
    /* We want _all_ the threads on: including the command
     * prompt thread, which is off by default */
    MC_InitializeAgencyOptions(&options);
    for (i = 0; i < MC_THREAD_ALL; i++) {
        MC_SetThreadOn(&options, i);
    }
    
    /* We need to turn off MobileC's command prompt or it will interfere
     * with our custom command prompt. */
    MC_SetThreadOff(&options, MC_THREAD_CP); 

    agency = MC_Initialize(
        local_port,
        &options);
    printf("Welcome to the steer example command platform.\n");
    printf("----------------------------------------------\n");
    while( strcmp(buf, "quit")) {
      printf("What would you like to do?\n");
      printf("1: Restart the algorithm.\n");
      printf("2: Resume the algorithm.\n");
      printf("3: Stop the algorithm.\n");
      printf("4: Suspend the algorithm.\n");
      printf("q: Quit this program.\n");
      printf("  > ");
      scanf("%s", buf);

      switch(buf[0]) {
        case '1':
          MC_SendAgentFile ( agency, "restart.xml");
          break;
        case '2':
          MC_SendAgentFile ( agency, "resume.xml");
          break;
        case '3':
          MC_SendAgentFile ( agency, "stop.xml");
          break;
        case '4':
          MC_SendAgentFile ( agency, "suspend.xml");
          break;
        case 'q':
          strcpy(buf, "quit");
          break;
        default:
          printf("Invalid command.\n");
      }
    }
    MC_End(agency);
    return 0;
}
