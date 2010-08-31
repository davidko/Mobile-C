#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmc.h>

int main(int argc, char *argv[]) {
  int i;
  int j;
  int k;
  int test_times = 20;
  int local_port = 6000;
  int *remote_port;
  int num_agencies;
  int num_teams;

  char agent_path[100];
  char destination[100];
  char **hostname;
  char *hostfile = "host_list.txt";
  char *host_str;
  char *port_str;

  FILE *fp;

  MCAgency_t agency;
  MCAgencyOptions_t options;

  if(argc != 3 && argc != 4) {
    printf("Usage: %s <num_agencies> <num_teams> [num_trials]\n", argv[0]);
    exit(0);
  }

  num_agencies = atoi(argv[1]);
  num_teams = atoi(argv[2]);
  if (argc == 4) {
    test_times = atoi(argv[3]);
  }

  hostname = (char **)malloc(num_agencies*sizeof(char *));
  for(i=0; i<num_agencies; i++) {
    hostname[i] = (char *)malloc(100*sizeof(char));
  }

  remote_port = (int *)malloc(num_agencies*sizeof(int));

  fp = fopen(hostfile, "r");
  for(i=0; i<num_agencies; i++) {
    fgets(destination, sizeof(destination), fp);
    host_str = strtok(destination, ":");
    port_str = strtok(NULL, ":");
    strcpy(hostname[i], host_str);
    sscanf(port_str, "%d", &(remote_port[i]));
  }
  fclose(fp);

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP);
  options.stack_size[MC_THREAD_AGENT] = 400000;
  options.initInterps = num_teams*2;
  agency = MC_Initialize(local_port, &options);

  for(k=0; k<test_times; k++) {
    printf("TRIAL %d\n", k);
    for(j=1; j<=num_teams; j++) {
      for(i=0; i<num_agencies; i++) {
        sprintf(agent_path, "team%d/agent%d_team%d.xml", j, i+1, j);
        printf("Sending %s to %s:%d ...\n", agent_path, hostname[i], remote_port[i]);
        MC_SendAgentMigrationMessageFile(agency, agent_path, hostname[i], remote_port[i]);
        usleep(100000);
      }
    }
    sleep(5);

    for(j=1; j<=num_teams; j++) {
      sprintf(agent_path, "team%d/agenttrigger_team%d.xml", j, j);
      printf("Sending %s to %s:%d ...\n", agent_path, hostname[0], remote_port[0]);
      MC_SendAgentMigrationMessageFile(agency, agent_path, hostname[0], remote_port[0]);
      usleep(100000);
    }
    //sleep(20);
    getchar();
  }

  for(i=0; i<num_agencies; i++) {
    free(hostname[i]);
  }
  free(hostname);
  free(remote_port);

  return 0;
}
