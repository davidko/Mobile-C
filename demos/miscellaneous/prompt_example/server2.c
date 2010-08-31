/* File: prompt_example/server.c */

#include <stdio.h>
#include <libmc.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) 
{
  MCAgency_t agency;
  int local_port = 5051;
  printf("Starting Agency... \n");
  printf("You may now try the following commands:\n\n");
  printf("  compose_send helloworld.c localhost 5050\n\n");
  printf("The previous command will compose an agent using the source\n\
code in \"helloworld.c\" and send it to the other agency at\n\
local_port 5050. Then try this command:\n\n\
  compose_send helloworld.c localhost 5051\n\n\
This will send the \"hello_world.c\" agent to the local agency.\n\
You may also try these commands at the other agency.\n");
  agency = MC_Initialize(local_port, NULL);

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
