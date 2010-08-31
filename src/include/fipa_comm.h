#ifndef _FIPA_COMM_H_
#define _FIPA_COMM_H_

#include <fipa_acl.h>

#ifndef true
#define true 1
#endif

#ifndef false 
#define false 0
#endif

#define MC_NUM_PERFORMATIVES 23

// Function pointers
typedef int (*fipa_comm_callback)(char* received_content, char** reply_content);
typedef int (*protocol_contract_net_callback)(int numBids, char** bids, char* winners);

enum fipa_comm_type_e {
  FIPA_COMM_ACTION,
  FIPA_COMM_CHECK,
  FIPA_COMM_REPLY,
  FIPA_COMM_CALLBACK,
  FIPA_COMM_END
};

enum fipa_comm_logic_type_e {
  FIPA_COMM_LOGIC_ERROR = -1,
  FIPA_COMM_LOGIC_OR,
  FIPA_COMM_LOGIC_AND
};

enum fipa_comm_check_type_e {
  FIPA_COMM_CHECK_ERROR = -1,
  FIPA_COMM_CHECK_SENDER_NAME,
  FIPA_COMM_CHECK_SENDER_ADDRESS,
  FIPA_COMM_CHECK_CONTENT,
  FIPA_COMM_CHECK_CONVERSATIONID,
  FIPA_COMM_CHECK_LANGUAGE,
  FIPA_COMM_CHECK_ONTOLOGY,
  FIPA_COMM_CHECK_ENCODING
};

enum fipa_comm_action_type_e {
  FIPA_COMM_ACTION_ERROR = -1,
  FIPA_COMM_ACTION_BOTH,
  FIPA_COMM_ACTION_REPLY,
  FIPA_COMM_ACTION_CALLBACK,
  FIPA_COMM_ACTION_NONE
};

/*
char* fipa_comm_protocol_names[] = { 
  "fipa-request",
  "fipa-query",
  "fipa-request-when",
  "fipa-contract-net",
  "fipa-iterated-contract-net",
  "fipa-auction-english",
  "fipa-auction-dutch",
  "fipa-brokering",
  "fipa-recruiting",
  "fipa-subscribe",
  "fipa-propose"
};
*/

// ---------------------------------------------------------------
// fipa comm message check structure
// ---------------------------------------------------------------
struct fipa_comm_message_check_t
{
  char* macro;
  enum fipa_comm_check_type_e type;
  struct fipa_comm_message_check_t* next;
};

typedef struct fipa_comm_message_check_t  fipa_comm_message_check_s;
typedef struct fipa_comm_message_check_t* fipa_comm_message_check_p;
// ---------------------------------------------------------------

// ---------------------------------------------------------------
// fipa comm reply structure
// ---------------------------------------------------------------
struct fipa_comm_reply_t
{
  enum fipa_performative_e type;
  char* content;
};

typedef struct fipa_comm_reply_t  fipa_comm_reply_s;
typedef struct fipa_comm_reply_t* fipa_comm_reply_p;
// ---------------------------------------------------------------

// ---------------------------------------------------------------
// fipa comm action structure
// ---------------------------------------------------------------
struct fipa_comm_action_t
{
  enum fipa_comm_action_type_e type;
  fipa_comm_callback callback;
  fipa_comm_message_check_p check_list;
  fipa_comm_reply_p reply;
  struct fipa_comm_action_t* next;
};

typedef struct fipa_comm_action_t  fipa_comm_action_s;
typedef struct fipa_comm_action_t* fipa_comm_action_p;
// ---------------------------------------------------------------

// ---------------------------------------------------------------
// fipa comm performative structure
// ---------------------------------------------------------------
struct fipa_comm_performative_t
{
  fipa_comm_action_p action_list;
  fipa_comm_reply_p default_reply;
};
typedef struct fipa_comm_performative_t  fipa_comm_performative_s;
typedef struct fipa_comm_performative_t* fipa_comm_performative_p;
// ---------------------------------------------------------------

// ---------------------------------------------------------------
// fipa comm protocol contract net structure
// ---------------------------------------------------------------
enum fipa_comm_protocol_cn_state_e {
  FIPA_COMM_PROTOCOL_CN_ERROR = -1,
  FIPA_COMM_PROTOCOL_CN_START,
  FIPA_COMM_PROTOCOL_CN_CFP,
  FIPA_COMM_PROTOCOL_CN_BID,
  FIPA_COMM_PROTOCOL_CN_RESULT,
  FIPA_COMM_PROTOCOL_CN_DONE
};

struct fipa_list_t {
  void* data;
  struct fipa_list_t* next;
};
typedef struct fipa_list_t  fipa_list_s;
typedef struct fipa_list_t* fipa_list_p;

struct fipa_comm_protocol_cn_t {
  // FIXME: change the following to be more generic
  protocol_contract_net_callback contract_func; // called when bids are returned
  fipa_comm_callback             proposal_func; // called with initial requests
  char require_inform;
  int time_out;
  char* cfp_request;

  // The following should not be touched by user
  fipa_list_p replies;
  fipa_list_p queue;
};
typedef struct fipa_comm_protocol_cn_t  fipa_comm_protocol_cn_s;
typedef struct fipa_comm_protocol_cn_t* fipa_comm_protocol_cn_p;
// ---------------------------------------------------------------

// ---------------------------------------------------------------
// fipa comm protocol structure
//   We implement protocols as finite state machines
// ---------------------------------------------------------------
struct fipa_comm_protocol_t {
  char error;
  char state;
  char* conversation_id;
  char* network;
  enum fipa_protocol_e type;
  void* info;
  struct fipa_comm_protocol_t* next;
};
typedef struct fipa_comm_protocol_t* fipa_comm_protocol_p;
typedef struct fipa_comm_protocol_t  fipa_comm_protocol_s;
// ---------------------------------------------------------------

// ---------------------------------------------------------------
// fipa comm structure
// ---------------------------------------------------------------
struct fipa_comm_t
{
  int error;
  char* agent_name;
  char* agent_address;
  void* agent;
  char debug;
  fipa_comm_performative_p performative[MC_NUM_PERFORMATIVES];
  fipa_comm_reply_p default_reply;
  fipa_comm_protocol_p protocol[FIPA_PROTOCOL_END];
  fipa_comm_callback pCallbacks[FIPA_PROTOCOL_END];
  //void*  pCallbacks[FIPA_PROTOCOL_END];
};
typedef struct fipa_comm_t  fipa_comm_s;
typedef struct fipa_comm_t* fipa_comm_p;
typedef struct fipa_comm_t  MCFIPAComm_s;
typedef struct fipa_comm_t* MCFIPAComm_p;
// -------------------------------------------------------------


fipa_comm_p mc_FIPAComm_New();
int mc_FIPAComm_HandleMessage(
  fipa_comm_p fcomm, 
  fipa_acl_message_p acl);
int mc_FIPAComm_HandleMessageOnce(fipa_comm_p fcomm);
int mc_FIPAComm_HandleMessageWait(fipa_comm_p fcomm);
void mc_FIPAComm_Destroy(fipa_comm_p);


// Low level comm ----------------------------------------------

int mc_FIPAComm_RegisterPerformative(
  fipa_comm_p fcomm, 
  enum fipa_performative_e type);

int mc_FIPAComm_DeregisterPerformative(
  fipa_comm_p fcomm, 
  enum fipa_performative_e type);

int mc_FIPAComm_SetDefaultReply(
  fipa_comm_p fcomm, 
  enum fipa_performative_e type, 
  char* content);

int mc_FIPAComm_SetPerformativeDefaultReply(
  fipa_comm_p fcomm, 
  enum fipa_performative_e ptype, 
  enum fipa_performative_e rtype, 
  char* content);

int mc_FIPAComm_SetAgent(
  fipa_comm_p fcomm, 
  void* agent,
  char* agent_name, 
  char* agent_address);

int mc_FIPAComm_RegisterAction(
  fipa_comm_p fcomm, 
  enum fipa_performative_e pType, 
  enum fipa_comm_action_type_e aType, ...);

int mc_FIPAComm_RegisterActionS(
  fipa_performative_e type, 
  fipa_comm_action_p act);

// Low level comm ----------------------------------------------

// Protocol comm -----------------------------------------------

fipa_comm_protocol_p mc_FIPAProtocol_New();

int mc_FIPAComm_RegisterProtocolCallback(
  fipa_comm_p fcomm, 
  enum fipa_protocol_e protocolType, 
  fipa_comm_callback func);

int mc_FIPAComm_RegisterProtocol(
  fipa_comm_p fcomm, 
  enum fipa_protocol_e protocolType, 
  char* protocolID, 
  void* info, 
  char* network);

void fipa_comm_protocol_info_COPY(
  fipa_comm_protocol_p proto, 
  void* info);

int fipa_list_Add(
  fipa_list_p* list, 
  fipa_acl_message_p acl);

int mc_FIPAComm_StartProtocol(
  fipa_comm_p fcomm, 
  enum fipa_protocol_e type, 
  char* id);

int mc_FIPAComm_HandleProtocol(
  fipa_comm_p fcomm, 
  fipa_acl_message_p acl);

fipa_comm_protocol_p mc_FIPAComm_AddNewProtocol(
  fipa_comm_p fcomm, 
  enum fipa_protocol_e type, 
  char* id);

int mc_FIPAComm_FSM_CN(
  fipa_comm_p fcomm, 
  fipa_acl_message_p acl);

fipa_comm_protocol_p mc_FIPAComm_NewProtocol(
  enum fipa_protocol_e type, 
  char* id);

int mc_FIPAComm_SetProtocolState(
  fipa_comm_protocol_p protocol, 
  char state);

// Protocol comm -----------------------------------------------

#pragma importf <fipa_comm.chf>

#endif
