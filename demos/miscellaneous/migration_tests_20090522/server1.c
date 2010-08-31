#include <stdio.h>
#include <libmc.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) 
{
  MCAgency_t agency;
  int local_port = 5050;
	int num_teams;
	MCAgencyOptions_t options;

	struct timeval temp1, temp2;
	long elapsed_utime;    // elapsed time in microseconds
	long elapsed_mtime;    // elapsed time in milliseconds
	long elapsed_seconds;  // time difference in seconds
	long elapsed_useconds; // time difference in microseconds


	if (argc != 2) {
		printf("Usage: %s <num_teams>\n", argv[0]);
		exit(0);
	}

	num_teams = atoi(argv[1]);

  MC_InitializeAgencyOptions(&options);
	MC_SetThreadOff(&options, MC_THREAD_CP); // Turn off command prompt
  
  agency = MC_Initialize(local_port, &options);


	printf("Waiting for %d trigger agent(s) ... \n", num_teams);
	MC_BarrierInit(agency, 555, num_teams+1);
	MC_Barrier(agency, 555);
	gettimeofday(&temp1, NULL);

	printf("Waiting for %d runner agent(s) ... \n", num_teams);
	MC_BarrierInit(agency, 556, num_teams+1);
	MC_Barrier(agency, 556);
	gettimeofday(&temp2, NULL);

	elapsed_seconds = temp2.tv_sec - temp1.tv_sec;
	elapsed_useconds = temp2.tv_usec - temp1.tv_usec;

	// elapsed time in millisec is given by
	// 1000 * (difference in seconds) + (difference in microseconds)
	elapsed_mtime = (elapsed_seconds) * 1000 + elapsed_useconds / 1000.0;
	elapsed_utime = (elapsed_seconds) * 1000000 + elapsed_useconds;

	printf("Elapsed time = %ld microseconds\n", elapsed_utime);
	printf("Elapsed time = %ld milliseconds\n", elapsed_mtime);
	getchar();


  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
