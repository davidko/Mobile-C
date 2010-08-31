#include <stdio.h>
#include <libmc.h>
#include <string.h>

#define NUM_DATA 20

int main(int argc, char *argv[]) {
  int num_teams;
  int agency_num;
  int local_port;
  int i, j, count = 0;
  ChOptions_t ch_options;

  struct timeval temp1, temp2;
  long elapsed_utime;            // elapsed time in microseconds
  long elapsed_mtime[NUM_DATA];  // elapsed time in milliseconds
  long elapsed_seconds;          // time difference in seconds
  long elapsed_useconds;         // time difference in microseconds
  int num_hosts;

  FILE *fptr;

  MCAgency_t agency;
  MCAgencyOptions_t options;

  if(argc != 5 && argc != 6) {
    printf("Usage: %s <num_teams> <agency_num> <num_hosts> <local_port> [embed_ch_home] \n", argv[0]);
    exit(0);
  }

  num_teams = atoi(argv[1]);
  agency_num = atoi(argv[2]);
  num_hosts = atoi(argv[3]);
  local_port = atoi(argv[4]);
  ch_options.shelltype = CH_REGULARCH;

  MC_InitializeAgencyOptions(&options);
  //MC_SetThreadOff(&options, MC_THREAD_CP); // Turn off the command prompt
  options.stack_size[MC_THREAD_AGENT] = 400000;
  options.initInterps = num_teams*2; 

  if(argc == 6) {
    ch_options.chhome = (char*)malloc((strlen(argv[5]) + 1) * sizeof(char));
    strcpy(ch_options.chhome, argv[5]);
    //MC_ChInitializeOptions(agency, &ch_options);
    options.ch_options = &ch_options;
  }

  agency = MC_Initialize(local_port, &options);

  // initializing condition variable(s)
  for(i=1; i<=num_teams; i++) {
    for(j=0; j<=num_hosts; j++) {
      MC_SyncInit(agency, 100*i+j);
    }
  }

  if(agency_num == 1) {
    while(1) {
      while(count < NUM_DATA) {
        MC_BarrierInit(agency, 555, num_teams+1);
        MC_BarrierInit(agency, 556, num_teams+1);

        //printf("Waiting for %d trigger agent(s) ...\n", num_teams);
        MC_Barrier(agency, 555);
        gettimeofday(&temp1, NULL);

        //printf("Waiting for %d last-leg runner agent(s) ...\n", num_teams);
        MC_Barrier(agency, 556);
        gettimeofday(&temp2, NULL);

        elapsed_seconds = temp2.tv_sec - temp1.tv_sec;
        elapsed_useconds = temp2.tv_usec - temp1.tv_usec;

        // elapsed time in millisec is given by
        // 1000 * (difference in seconds) + (difference in microseconds)
        elapsed_mtime[count] = (elapsed_seconds) * 1000 + elapsed_useconds / 1000.0;
        //elapsed_utime = (elapsed_seconds) * 1000000 + elapsed_useconds;

        //printf("Elapsed time = %ld milliseconds\n", elapsed_mtime);
        //printf("Elapsed time = %ld microseconds\n", elapsed_utime);

        usleep(500000);
        MC_BarrierDelete(agency, 555);
        usleep(500000);
        MC_BarrierDelete(agency, 556);
        usleep(500000);
        count++;
      }
      fptr = fopen("./output", "w");
      for(i=0; i<NUM_DATA; i++) {
        fprintf(fptr, "%d\n", elapsed_mtime[i]);
      }
      fclose(fptr);
      printf("Done!\n");
      count = 0;
    }
  }
  else { 
    MC_MainLoop(agency);
  }
}
