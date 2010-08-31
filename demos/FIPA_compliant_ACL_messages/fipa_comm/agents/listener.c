#include <stdio.h>
#include <mobiler.h>
#include <fipa_comm.h>

int data_callback(char*, char**);

int main() {

  printf("%s: Started on %s\n", mc_agent_name, mc_agent_address);

  //The setup
  //  get FIPA_REQUEST
  //    check if from TALKER
  //      reply agree
  //    else
  //      reply refuse
  //    check for DATA
  //       callback and reply data
  //  anything else reply FIPA_NOT_UNDERSTOOD
  //

  fipa_comm_p fcomm = mc_FIPAComm_New();
  //mc_FIPAComm_SetDebug(fcomm);

  mc_FIPAComm_SetAgent(fcomm, mc_current_agent, mc_agent_name, mc_agent_address);
  printf("%s\n", fcomm->agent_address);

  mc_FIPAComm_SetDefaultReply(fcomm, FIPA_NOT_UNDERSTOOD, NULL);

  mc_FIPAComm_SetPerformativeDefaultReply(fcomm, FIPA_REQUEST, FIPA_REFUSE, NULL);

  mc_FIPAComm_RegisterAction(
     fcomm, FIPA_REQUEST, FIPA_COMM_ACTION_REPLY,
     FIPA_COMM_CHECK, FIPA_COMM_CHECK_SENDER_NAME, "talk",
     FIPA_COMM_REPLY, FIPA_AGREE, "Something back",
     FIPA_COMM_END);

  mc_FIPAComm_RegisterAction(
    fcomm, FIPA_REQUEST, FIPA_COMM_ACTION_BOTH, 
    FIPA_COMM_CHECK, FIPA_COMM_CHECK_CONTENT, "DATA",
    FIPA_COMM_REPLY, FIPA_INFORM, NULL,
    FIPA_COMM_CALLBACK, data_callback,
    FIPA_COMM_END);


  // agent code
  while(1)
    mc_FIPAComm_HandleMessageWait(fcomm);
    
  return 0;
}

int data_callback(char* content, char** reply)
{
  printf("\n%s: data_callback parsing '%s'\n\n", mc_agent_name, content);
  *reply = (char*)malloc(20);
  sprintf(*reply, "DATA: 1 2 3 4");
  return 0;
}
