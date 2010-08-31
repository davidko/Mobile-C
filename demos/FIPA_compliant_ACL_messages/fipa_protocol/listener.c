#include <stdio.h>
#include <mobiler.h>
#include <fipa_comm.h>

int proposal_callback(char*, char**);

int main() {
   
  printf("%s: Started at %s\n", mc_agent_name, mc_agent_address);

  fipa_comm_p fcomm = mc_FIPAComm_New();
  mc_FIPAComm_SetDebug(fcomm);
  mc_FIPAComm_SetAgent(fcomm, mc_current_agent, mc_agent_name, mc_agent_address);
  mc_FIPAComm_RegisterProtocolCallback(fcomm, FIPA_PROTOCOL_CONTRACT_NET, proposal_callback);

  while(1) mc_FIPAComm_HandleMessageWait(fcomm);

  printf("%s: Finished at %s\n", mc_agent_name, mc_agent_address);
  return 0;
}

int proposal_callback(char* content, char** reply)
{
  printf("Got proposal: %s\n", content);
  *reply = (char*)malloc(50);
  sprintf(*reply, "My Bid!");
  return 0;
}
