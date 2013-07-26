
#pragma package "chmobilec"

#include <stdio.h>
#include <string.h>
#include <fipa_acl.h>

int main()
{
  srand(time(NULL));
  /* Talk with the newscast manager and get a list of its peers */
  fipa_acl_message_t* message;
  message = mc_AclNew();
  mc_AclSetPerformative(message, FIPA_INFORM);
  mc_AclSetSender(message, mc_agent_name, mc_agent_address);
  mc_AclAddReceiver(message, "Manager", mc_agent_address);
  mc_AclSetContent(message, "get peers");
  mc_AclSend(message);
  mc_AclDestroy(message);
  /* Now wait for a message to come back */
  printf("%s: Waiting for a message.\n", mc_agent_name);
  message = mc_AclWaitRetrieve(mc_current_agent);

  printf("\tContent is '%s'.\n", mc_AclGetContent(message));
  int mytime = time(NULL);
  while(time(NULL) < mytime+3);
  char* hosts = strdup(mc_AclGetContent(message));
  char *host[128];
  char *saveptr;
  char *sub;
  sub = strtok_r(hosts, "%", &saveptr);
  host[0] = sub;
  int i = 0;
  for(i = 1; (sub = strtok_r(NULL, "%", &saveptr))!=NULL; i++) {
    host[i] = sub;
  }
  /* Select a random host */
  int index = rand() % i;
  char *hostname;
  int port;
  if(host[index] != NULL) {
    hostname = strtok_r(host[index], ":", &saveptr);
    sscanf(strtok_r(NULL, ":", &saveptr), "%d", &port);
    mc_MigrateAgent(mc_current_agent, hostname, port);
  }

  mc_AclDestroy(message);
}
