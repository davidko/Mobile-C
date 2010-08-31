#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main(int argc, char *argv[]) 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port = 6000;
  int remote_port = 5050;
	int i;

	if(argc == 1) {
		printf("Usage: %s <file1> <file2> <etc...>\n", argv[0]);
		exit(0);
	}

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
	for(i = 1; i < argc; i++) {
		MC_SendAgentMigrationMessageFile(agency,
				argv[i],
				"rabbit.engr.ucdavis.edu",
				remote_port);
	}
  MC_End(agency);
  exit(0);
}
