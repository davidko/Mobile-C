/* SVN FILE INFO
 * $Revision: 181 $ : Last Committed Revision
 * $Date: 2008-07-01 15:32:24 -0700 (Tue, 01 Jul 2008) $ : Last Committed Date */
#include "fipa_acl.h"

/* fipa_acl_envelope_Received */
typedef struct fipa_acl_envelope_Received_s
{
  struct fipa_url_s* received_by;
  struct fipa_url_s* received_from;
  struct fipa_DateTime_s* received_date;
  char* received_id;
  struct fipa_url_s* received_via;
} fipa_acl_envelope_Received_t;

fipa_acl_envelope_Received_t* fipa_acl_envelope_Received_New(void);
int fipa_acl_envelope_Received_Destroy(fipa_acl_envelope_Received_t* received);
fipa_acl_envelope_Received_t* fipa_acl_envelope_Received_Copy(
    fipa_acl_envelope_Received_t* received);

/* fipa_acl_Param */
typedef struct fipa_acl_Param_s
{
  struct fipa_agent_identifier_set_s* to; 
  struct fipa_agent_identifier_s* from;
  char* comments;
  char* acl_representation;
  char* payload_length;
  char* payload_encoding;
  struct fipa_DateTime_s* date;
  struct fipa_agent_identifier_set_s* intended_receiver; 
  struct fipa_acl_envelope_Received_s* received;
} fipa_acl_Param_t;

fipa_acl_Param_t* fipa_acl_Param_New(void);
int fipa_acl_Param_Destroy(fipa_acl_Param_t* param);
fipa_acl_Param_t* fipa_acl_Param_Copy(fipa_acl_Param_t* param);

/* fipa_acl_envelope */
typedef struct fipa_acl_envelope_s
{
  int num_params;
  struct fipa_acl_Param_s** params;
} fipa_acl_envelope_t;
typedef fipa_acl_envelope_t* fipa_acl_envelope_p;
fipa_acl_envelope_t* fipa_acl_envelope_New(void);
int fipa_acl_envelope_Destroy(fipa_acl_envelope_t* envelope);
fipa_acl_envelope_t* fipa_acl_envelope_Copy(fipa_acl_envelope_t* envelope);

int fipa_envelope_Parse(
    struct fipa_acl_envelope_s* envelope,
    const char* message);

int fipa_envelope_HandleEnvelope(
    struct fipa_acl_envelope_s* envelope, 
    mxml_node_t* node);

int fipa_envelope_HandleParams(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* node);

int fipa_envelope_HandleTo(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_HandleFrom(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_HandleComments(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_HandleAclRepresentation(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_HandlePayloadLength(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_HandlePayloadEncoding(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_HandleDate(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_HandleIntendedReceiver(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_HandleReceived(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param);

int fipa_envelope_ParseAgentIdentifier(
    struct fipa_agent_identifier_s** aid, 
    mxml_node_t* agent_identifier_node
    );

int fipa_envelope_ParseAddresses(
    struct fipa_agent_identifier_s* aid, 
    mxml_node_t* addresses_node);
    
int fipa_envelope_ParseResolvers(
    struct fipa_agent_identifier_s* aid,
    mxml_node_t* resolvers_node);

char* fipa_envelope_Compose(fipa_acl_message_t* fipa_acl);

mxml_node_t* 
fipa_envelope_Compose__envelope(fipa_acl_message_t* fipa_acl);

mxml_node_t*
fipa_envelope_Compose__params(fipa_acl_message_t* fipa_acl);

mxml_node_t*
fipa_envelope_Compose__to(fipa_acl_message_t* fipa_acl);

mxml_node_t* 
fipa_envelope_Compose__from(fipa_acl_message_t* fipa_acl);

mxml_node_t*
fipa_envelope_Compose__acl_representation(fipa_acl_message_t* fipa_acl);

mxml_node_t* 
fipa_envelope_Compose__payload_encoding(fipa_acl_message_t* fipa_acl);

mxml_node_t*
fipa_envelope_Compose__date(fipa_acl_message_t* fipa_acl);

mxml_node_t*
fipa_envelope_Compose__intended_receiver(fipa_acl_message_t* fipa_acl);
