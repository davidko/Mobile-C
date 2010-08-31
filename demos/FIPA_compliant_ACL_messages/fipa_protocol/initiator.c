#include <stdio.h>

#include <mobilec.h>
#include <fipa_comm.h>

int cn_callback(int numBids, char** bids, char* winners);

int main() {

  printf("%s: Started at %s\n", mc_agent_name, mc_agent_address);

  fipa_comm_p fcomm = mc_FIPAComm_New();
  mc_FIPAComm_SetAgent(fcomm, mc_current_agent, mc_agent_name, mc_agent_address);
  fipa_comm_protocol_cn_s proto;

  proto.contract_func = cn_callback;
  proto.time_out = 10;
  proto.require_inform = 0;
  proto.cfp_request = "Survey Area 5";

  mc_FIPAComm_RegisterProtocol(
    fcomm, 
    FIPA_PROTOCOL_CONTRACT_NET,
    "cnet-01",
    (void*)&proto,
    "listener@http://localhost:5051 listener@http://localhost:5052");

  mc_FIPAComm_StartProtocol(fcomm, FIPA_PROTOCOL_CONTRACT_NET, "cnet-01");

  printf("%s: Finished at %s\n", mc_agent_name, mc_agent_address);

  return 0;
}

int cn_callback(int numBids, char** bids, char* winners)
{
  int i;
  printf("numBids = %d\n", numBids);
  for(i=0;i<numBids;i++)
    printf("bid[%d] = %s\n", i, bids[i]);
  winners[0] = 1;
  return 0;
}
