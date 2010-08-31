/* SVN FILE INFO
 * $Revision: 530 $ : Last Committed Revision
 * $Date: 2010-06-16 16:52:19 -0700 (Wed, 16 Jun 2010) $ : Last Committed Date */
/*[
 * Copyright (c) 2007 Integration Engineering Laboratory
                      University of California, Davis
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the complete modified source code.  Modifications are to
 * be distributed as patches to the released version.  Permission to
 * distribute binaries produced by compiling modified sources is granted,
 * provided you
 *   1. distribute the corresponding source modifications from the
 *    released version in the form of a patch file along with the binaries,
 *   2. add special version identification to distinguish your version
 *    in addition to the base release version number,
 *   3. provide your name and address as the primary contact for the
 *    support of your modified version, and
 *   4. retain our contact information in regard to use of the base
 *    software.
 * Permission to distribute the released version of the source code along
 * with corresponding source modifications in the form of a patch file is
 * granted with same provisions 2 through 4 for binary distributions.
 *
 * This software is provided "as is" without express or implied warranty
 * to the extent permitted by applicable law.
]*/
#ifndef _WIN32
#include "config.h"
#else
#include "winconfig.h"
#endif

#include <stdio.h>
#include <mxml.h>
#include <time.h>
#include "include/fipa_acl_envelope.h"
#include "include/mc_error.h"
#include "include/mc_platform.h"

/* fipa_acl_envelope_Received */
fipa_acl_envelope_Received_t* fipa_acl_envelope_Received_New(void)
{
  fipa_acl_envelope_Received_t* received;
  received = (fipa_acl_envelope_Received_t*)malloc(
      sizeof(fipa_acl_envelope_Received_t) );
  memset(received, 0, sizeof(fipa_acl_envelope_Received_t));
  return received;
}

int fipa_acl_envelope_Received_Destroy(fipa_acl_envelope_Received_t* received)
{
  if (received == NULL) return 0;
  fipa_url_Destroy(received->received_by);
  fipa_url_Destroy(received->received_from);
  fipa_DateTime_Destroy(received->received_date);
  if (received->received_id != NULL) free(received->received_id);
  fipa_url_Destroy(received->received_via);
  free(received);
  return 0;
}

fipa_acl_envelope_Received_t* fipa_acl_envelope_Received_Copy(
    fipa_acl_envelope_Received_t* received)
{
  fipa_acl_envelope_Received_t* copy;
  if (received == NULL) return NULL;
  copy = fipa_acl_envelope_Received_New();
  copy->received_by = fipa_url_Copy(received->received_by);
  copy->received_from = fipa_url_Copy(received->received_from);
  copy->received_date = fipa_DateTime_Copy(received->received_date);
  copy->received_id = strdup(received->received_id);
  copy->received_via = fipa_url_Copy(received->received_via);

  return copy;
}

/* fipa_acl_Param */
fipa_acl_Param_t* fipa_acl_Param_New(void)
{
  fipa_acl_Param_t* param;
  param = (fipa_acl_Param_t*)malloc(sizeof(fipa_acl_Param_t));
  memset(param, 0, sizeof(fipa_acl_Param_t));
  return param;
}

int fipa_acl_Param_Destroy(fipa_acl_Param_t* param)
{
  if (param == NULL) return 0;
  fipa_agent_identifier_set_Destroy(param->to);
  fipa_agent_identifier_Destroy(param->from);
  if (param->comments != NULL) free(param->comments);
  if (param->acl_representation != NULL) free(param->acl_representation);
  if (param->payload_length != NULL) free(param->payload_length);
  if (param->payload_encoding != NULL) free (param->payload_encoding);
  fipa_DateTime_Destroy(param->date);
  fipa_agent_identifier_set_Destroy(param->intended_receiver);
  fipa_acl_envelope_Received_Destroy(param->received);

  free(param);
  return 0;
}

fipa_acl_Param_t* fipa_acl_Param_Copy(fipa_acl_Param_t* param)
{
  fipa_acl_Param_t* copy;
  if (param == NULL) return NULL;
  copy = fipa_acl_Param_New();
  copy->to = fipa_agent_identifier_set_Copy(param->to);
  copy->from = fipa_agent_identifier_Copy(param->from);
  copy->comments = strdup(param->comments);
  copy->acl_representation = strdup(param->acl_representation);
  copy->payload_length = strdup(param->payload_length);
  copy->payload_encoding = strdup(param->payload_encoding);
  copy->date = fipa_DateTime_Copy(param->date);
  copy->intended_receiver = fipa_agent_identifier_set_Copy(param->intended_receiver);
  copy->received = fipa_acl_envelope_Received_Copy(param->received);

  return copy;
}

/* fipa_acl_envelope */
fipa_acl_envelope_t* fipa_acl_envelope_New(void)
{
  fipa_acl_envelope_t* envelope;
  envelope = (fipa_acl_envelope_t*)malloc(sizeof(fipa_acl_envelope_t));
  memset(envelope, 0, sizeof(fipa_acl_envelope_t));
  return envelope;
}

int fipa_acl_envelope_Destroy(fipa_acl_envelope_t* envelope)
{
  int i;
  if (envelope == NULL) return 0;
  if (envelope->params == NULL) return 0;
  for(i = 0; i < envelope->num_params; i++) {
    fipa_acl_Param_Destroy(envelope->params[i]);
  }
  free(envelope->params);
  free(envelope);
  return 0;
}

fipa_acl_envelope_t* fipa_acl_envelope_Copy(fipa_acl_envelope_t* envelope)
{
  int i;
  fipa_acl_envelope_t* copy;
  if (envelope == NULL) return NULL;
  copy = fipa_acl_envelope_New();
  copy->num_params = envelope->num_params;
  copy->params = (fipa_acl_Param_t**)malloc(sizeof(fipa_acl_Param_t*)*copy->num_params);
  for (i = 0; i < copy->num_params; i++) {
    copy->params[i] = fipa_acl_Param_Copy(envelope->params[i]);
  }
  return copy;
}

/* Parsing Functions */
int fipa_envelope_Parse(
    struct fipa_acl_envelope_s* envelope,
    const char* message)
{
  /* Call mxml to parse the message */
  int retval;
  mxml_node_t* root_node;
  root_node = mxmlLoadString(NULL, message, MXML_NO_CALLBACK);
  retval = fipa_envelope_HandleEnvelope(envelope, root_node);
  mxmlDelete(root_node);
  return retval;
}

int fipa_envelope_HandleEnvelope(
    struct fipa_acl_envelope_s* envelope, 
    mxml_node_t* node)
{
  /* Find the 'envelope' element */
  mxml_node_t *envelope_node;
  envelope_node = mxmlFindElement(
      node,
      node,
      "envelope",
      NULL,
      NULL,
      MXML_DESCEND_FIRST
      );
  if (envelope_node == NULL) {
    return MC_ERR_PARSE;
  }
  return fipa_envelope_HandleParams(envelope, envelope_node);
}

int fipa_envelope_HandleParams(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* node)
{
  mxml_node_t* param;
  int i;
  char buf[10];

  /* Lets figure out how hany params there are */
  for (i = 1; ; i++) {
    sprintf(buf, "%d", i);
    param = mxmlFindElement(
        node,
        node,
        "params",
        "index",
        buf,
        MXML_DESCEND_FIRST);
    if (param == NULL) { break; }
  }
  i--;
  if (i == 0) {
    /* There should be at least 1 param */
    return MC_ERR_PARSE;
  }
  envelope->num_params = i;
  envelope->params = (struct fipa_acl_Param_s**)malloc(
      sizeof(struct fipa_acl_Param_s*) * i);
  memset(envelope->params, 0, i*sizeof(struct fipa_acl_Param_s*) );

  for (i = 1; i <= envelope->num_params; i++) {
    sprintf(buf, "%d", i);
    param = mxmlFindElement(
        node,
        node,
        "params",
        "index",
        buf,
        MXML_DESCEND_FIRST);
    fipa_envelope_HandleTo(envelope, param, i-1);
    fipa_envelope_HandleFrom(envelope, param, i-1);
    fipa_envelope_HandleComments(envelope, param, i-1);
    fipa_envelope_HandleAclRepresentation(envelope, param, i-1);
    fipa_envelope_HandlePayloadLength(envelope, param, i-1);
    fipa_envelope_HandlePayloadEncoding(envelope, param, i-1);
    fipa_envelope_HandleDate(envelope, param, i-1);
    fipa_envelope_HandleIntendedReceiver(envelope, param, i-1);
    fipa_envelope_HandleReceived(envelope, param, i-1);
  }
  return 0;
}

int fipa_envelope_HandleTo(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  int i;
  mxml_node_t* to_node;
  mxml_node_t* aid_node;

  to_node = mxmlFindElement(
      param_node,
      param_node,
      "to",
      NULL, NULL, 
      MXML_DESCEND_FIRST );
  if (to_node == NULL) return MC_ERR_PARSE;

  /* There are a bunch of agent identifiers that must be parsed inside the 
   * 'to' node. Let us first count the number */
  aid_node = mxmlFindElement(
      to_node,
      to_node,
      "agent-identifier",
      NULL, NULL,
      MXML_DESCEND_FIRST );
  if (aid_node == NULL) return MC_ERR_PARSE;

  for(
      i = 1;
      (aid_node = mxmlFindElement(
        aid_node,
        to_node,
        "agent-identifier",
        NULL,NULL,
        MXML_NO_DESCEND )) != NULL;
      i++
     );

  envelope->params[cur_param] = fipa_acl_Param_New();
  envelope->params[cur_param]->to = fipa_agent_identifier_set_New();
  envelope->params[cur_param]->to->num = i;
  envelope->params[cur_param]->to->retain_order = 0;
  envelope->params[cur_param]->to->fipa_agent_identifiers = 
    (struct fipa_agent_identifier_s**) malloc(
        sizeof(struct fipa_agent_identifier_s*) * 
        envelope->params[cur_param]->to->num );

  /* Now we parse each one */
  aid_node = mxmlFindElement(
      to_node,
      to_node,
      "agent-identifier",
      NULL, NULL,
      MXML_DESCEND_FIRST );
  if (aid_node == NULL) return MC_ERR_PARSE;

  fipa_envelope_ParseAgentIdentifier(
      &(envelope->params[cur_param]->to->fipa_agent_identifiers[0]),
      aid_node );

  for(
      i = 1;
      (aid_node = mxmlFindElement(
        aid_node,
        to_node,
        "agent-identifier",
        NULL,NULL,
        MXML_NO_DESCEND ) );
      i++
     )
  {
    fipa_envelope_ParseAgentIdentifier(
        &(envelope->params[cur_param]->to->fipa_agent_identifiers[i]),
        aid_node );
  }
  return 0;
}

int fipa_envelope_HandleFrom(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  mxml_node_t* from_node;
  mxml_node_t* aid_node;
  
  from_node = mxmlFindElement(
      param_node,
      param_node,
      "from",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (from_node == NULL) return 0;

  aid_node = mxmlFindElement(
      from_node,
      from_node,
      "agent-identifier",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if (aid_node == NULL) return MC_ERR_PARSE;

  return fipa_envelope_ParseAgentIdentifier(
      &envelope->params[cur_param]->from,
      aid_node);
}

int fipa_envelope_HandleComments(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  mxml_node_t* comments_node;
  /* find the comments node, if there is one */
  comments_node = mxmlFindElement(
      param_node,
      param_node,
      "comments",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (comments_node == NULL) return 0;
  
  /* The comments node exists. Lets get it's contents */
  if (comments_node->child->type != MXML_TEXT) return MC_ERR_PARSE;
  envelope->params[cur_param]->comments = (char*)malloc(
      sizeof(char) * (strlen(comments_node->child->value.text.string)+1));
  strcpy(envelope->params[cur_param]->comments, 
      comments_node->child->value.text.string);
  return 0;
}

int fipa_envelope_HandleAclRepresentation(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  mxml_node_t* acl_representation_node;
  /* find the comments node, if there is one */
  acl_representation_node = mxmlFindElement(
      param_node,
      param_node,
      "acl-representation",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (acl_representation_node == NULL) return 0;
  
  /* The comments node exists. Lets get it's contents */
  if (acl_representation_node->child->type != MXML_TEXT) return MC_ERR_PARSE;
  envelope->params[cur_param]->acl_representation = (char*)malloc(
      sizeof(char) * (strlen(acl_representation_node->child->value.text.string)+1));
  strcpy(envelope->params[cur_param]->acl_representation,
      acl_representation_node->child->value.text.string);
  return 0;
}

int fipa_envelope_HandlePayloadLength(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  mxml_node_t* payload_length_node;

  payload_length_node = mxmlFindElement(
      param_node,
      param_node,
      "payload-length",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (payload_length_node == NULL) return 0;

  if (payload_length_node->child->type != MXML_TEXT) return MC_ERR_PARSE;
  envelope->params[cur_param]->payload_length = (char*)malloc(
      sizeof(char) * (strlen(payload_length_node->child->value.text.string)+1) );
  strcpy( envelope->params[cur_param]->payload_length,
      payload_length_node->child->value.text.string );
  return 0;
}

int fipa_envelope_HandlePayloadEncoding(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  mxml_node_t* payload_encoding_node;

  payload_encoding_node = mxmlFindElement(
      param_node,
      param_node,
      "payload-encoding",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (payload_encoding_node == NULL) return 0;

  if (payload_encoding_node->child->type != MXML_TEXT) return MC_ERR_PARSE;

  envelope->params[cur_param]->payload_encoding = (char*)malloc(
      sizeof(char) * (strlen(payload_encoding_node->child->value.text.string)+1) );
  strcpy( envelope->params[cur_param]->payload_encoding,
      payload_encoding_node->child->value.text.string );
  return 0;
}

int fipa_envelope_HandleDate(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  char date_string[25];
  mxml_node_t* date_node;
  fipa_message_string_t message_string;

  date_node = mxmlFindElement(
      param_node,
      param_node,
      "date",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (date_node == NULL) return 0;

  if (date_node->child->type != MXML_TEXT) return MC_ERR_PARSE;
  if ( strlen(date_node->child->value.text.string) > 22) return MC_ERR_PARSE;

  strcpy(date_string, date_node->child->value.text.string);

  message_string.message = date_string;
  message_string.parse = date_string;

  return fipa_datetime_Parse(
      &(envelope->params[cur_param]->date),
      &message_string);
}

int fipa_envelope_HandleIntendedReceiver(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  int i;
  mxml_node_t* intended_receiver_node;
  mxml_node_t* aid_node;

  intended_receiver_node = mxmlFindElement(
      param_node,
      param_node,
      "intended_receiver",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (intended_receiver_node == NULL) {
    envelope->params[cur_param]->intended_receiver = NULL;
    return 0;
  }
  envelope->params[cur_param]->intended_receiver = fipa_agent_identifier_set_New();

  /* Count the number of agent id's in the node */
  aid_node = mxmlFindElement(
      intended_receiver_node,
      intended_receiver_node,
      "agent-identifier",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if (aid_node == NULL) return MC_ERR_PARSE;
  for(
      i = 1; 
      ( aid_node = mxmlFindElement(
        aid_node,
        intended_receiver_node,
        "agent-identifier",
        NULL,NULL,
        MXML_NO_DESCEND)) ;
      i++
     );
  /* Allocate space in envelope */
  envelope->params[cur_param]->intended_receiver->num = i;
  envelope->params[cur_param]->intended_receiver->fipa_agent_identifiers = 
    (struct fipa_agent_identifier_s**)malloc(
        sizeof(struct fipa_agent_identifier_s*) * i);
  envelope->params[cur_param]->intended_receiver->retain_order = 0;

  /* Parse each one */
  aid_node = mxmlFindElement(
      intended_receiver_node,
      intended_receiver_node,
      "agent-identifier",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if (aid_node == NULL) return MC_ERR_PARSE;
  fipa_envelope_ParseAgentIdentifier(
      &(envelope->params[cur_param]->intended_receiver->
        fipa_agent_identifiers[0]),
      aid_node );
      
  for(
      i = 1; 
      (aid_node = mxmlFindElement(
        aid_node,
        intended_receiver_node,
        "agent-identifier",
        NULL,NULL,
        MXML_NO_DESCEND)) ;
      i++
     )
  {
    fipa_envelope_ParseAgentIdentifier(
        &(envelope->params[cur_param]->intended_receiver->
          fipa_agent_identifiers[i]),
        aid_node );
  }
  return 0;
} 

int fipa_envelope_HandleReceived(
    struct fipa_acl_envelope_s* envelope,
    mxml_node_t* param_node,
    int cur_param)
{
  int err;
  mxml_node_t* received_node;

  mxml_node_t* node;
  const char* attr;

  fipa_message_string_t message_string;

  char date_string[25];

  received_node = mxmlFindElement(
      param_node,
      param_node,
      "received",
      NULL, NULL, 
      MXML_DESCEND_FIRST);

  if (received_node == NULL) return 0;
  envelope->params[cur_param]->received = fipa_acl_envelope_Received_New(); 
  /* Need to parse the following nodes:
   *   received-by
   *   received-date
   * o received-from
   * o received-id
   * o received-via */

  /* received-by */
  node = mxmlFindElement(
      received_node,
      received_node,
      "received-by",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if (node == NULL) return MC_ERR_PARSE;
  attr = mxmlElementGetAttr(node, "value");
  envelope->params[cur_param]->received->received_by = fipa_url_New();
  envelope->params[cur_param]->received->received_by->str = 
    (char*)malloc(sizeof(char) * (strlen(attr)+1));
  strcpy(envelope->params[cur_param]->received->received_by->str,
      attr);

  /* received-date */
  node = mxmlFindElement(
      received_node,
      received_node,
      "received-date",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if (node == NULL) return MC_ERR_PARSE;
  attr = mxmlElementGetAttr(node, "value");
  if (attr == NULL) return MC_ERR_PARSE;
  if ( strlen(attr) > 22 ) return MC_ERR_PARSE;
  strcpy(date_string, attr);
  message_string.message = date_string;
  message_string.parse = date_string;

  err = fipa_datetime_Parse(
      &(envelope->params[cur_param]->received->received_date),
      &message_string);
  if (err) return err;

  /* received-from */
  node = mxmlFindElement(
      received_node,
      received_node,
      "received-from",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if(node == NULL) {
    envelope->params[cur_param]->received->received_from = NULL;
  } else {
    attr = mxmlElementGetAttr(node, "value");
    if (attr == NULL) return MC_ERR_PARSE;
    envelope->params[cur_param]->received->received_from = 
      (struct fipa_url_s*)malloc(sizeof(struct fipa_url_s));
    envelope->params[cur_param]->received->received_from->str = 
      (char*)malloc(sizeof(char)*(strlen(attr)+1));
    strcpy(
        envelope->params[cur_param]->received->received_from->str,
        attr );
  }

  /* received-id */
  node = mxmlFindElement(
      received_node,
      received_node,
      "received-id",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if(node == NULL) {
    envelope->params[cur_param]->received->received_id = NULL;
  } else {
    attr = mxmlElementGetAttr(node, "value");
    if (attr == NULL) return MC_ERR_PARSE;
    envelope->params[cur_param]->received->received_id = 
      (char*)malloc(sizeof(char) * (strlen(attr)+1));
    strcpy(envelope->params[cur_param]->received->received_id,
        attr);
  }

  /* received-via */
  node = mxmlFindElement(
      received_node,
      received_node,
      "received-via",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if(node == NULL) {
    envelope->params[cur_param]->received->received_via = NULL;
  } else {
    attr = mxmlElementGetAttr(node, "value");
    if (attr == NULL) return MC_ERR_PARSE;
    envelope->params[cur_param]->received->received_via = 
      (struct fipa_url_s*)malloc(sizeof(struct fipa_url_s));
    envelope->params[cur_param]->received->received_via->str = 
      (char*)malloc(sizeof(char) * (strlen(attr)+1));
    strcpy(envelope->params[cur_param]->received->received_via->str,
        attr);
  }
  return 0;
}

int fipa_envelope_ParseAgentIdentifier(
    struct fipa_agent_identifier_s** aid, 
    mxml_node_t* agent_identifier_node
    )
{
  int err;
  mxml_node_t* name_node;
  mxml_node_t* addresses_node;
  mxml_node_t* resolvers_node;

  name_node = mxmlFindElement(
      agent_identifier_node,
      agent_identifier_node,
      "name",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  /* The child of 'name' better be some text! */
  if(name_node->child->type != MXML_TEXT) {
    return MC_ERR_PARSE;
  } else {
    *aid = fipa_agent_identifier_New();
    (*aid)->name = (char*)malloc(sizeof(char) * 
        (strlen(name_node->child->value.text.string)+1) );
    strcpy((*aid)->name,
        name_node->child->value.text.string );
  }

  addresses_node = mxmlFindElement(
      agent_identifier_node,
      agent_identifier_node,
      "addresses",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (addresses_node != NULL) {
    err = fipa_envelope_ParseAddresses(*aid, addresses_node);
  }
  if (err) return err;
  resolvers_node = mxmlFindElement(
      agent_identifier_node,
      agent_identifier_node,
      "resolvers",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (resolvers_node != NULL) {
    err = fipa_envelope_ParseResolvers(*aid, resolvers_node);
  }
  if (err) return err;
  return 0;
}

int fipa_envelope_ParseAddresses(
    struct fipa_agent_identifier_s* aid, 
    mxml_node_t* addresses_node)
{
  int i;
  mxml_node_t* url_node;
  /* Here we expect any number of 'url' elements.
   * Let us count the number */
  url_node = mxmlFindElement(
      addresses_node, 
      addresses_node,
      "url",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (url_node == NULL) {
    return MC_ERR_PARSE;
  }
  for (
      i = 1; 
      url_node = mxmlFindElement(
        url_node, 
        addresses_node, 
        "url", 
        NULL, NULL, 
        MXML_NO_DESCEND) ; 
      i++
      );
  aid->addresses = (fipa_url_sequence_t*)malloc(sizeof(fipa_url_sequence_t));
  aid->addresses->num = i;
  aid->addresses->urls = (struct fipa_url_s**)malloc(
      sizeof(struct fipa_url_s*) * aid->addresses->num );
  /* Now we must copy each one */
  url_node = mxmlFindElement(
      addresses_node, 
      addresses_node,
      "url",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if (url_node->child->type != MXML_TEXT) {
    return MC_ERR_PARSE;
  }
  aid->addresses->urls[0] = fipa_url_New();
  aid->addresses->urls[0]->str = (char*)malloc(
      sizeof(char) * (strlen(url_node->child->value.text.string)+1));
  strcpy(aid->addresses->urls[0]->str, url_node->child->value.text.string);
  for (
      i = 1; 
      url_node = mxmlFindElement(
        url_node, 
        addresses_node, 
        "url", 
        NULL, NULL, 
        MXML_NO_DESCEND) ; 
      i++
      )
  {
    if (url_node->child->type != MXML_TEXT) {
      return MC_ERR_PARSE;
    }
    aid->addresses->urls[i] = fipa_url_New();
    aid->addresses->urls[i]->str = (char*)malloc(
        sizeof(char) * (strlen(url_node->child->value.text.string)+1));
    strcpy(aid->addresses->urls[i]->str, url_node->child->value.text.string);
  }
  return 0;
}
    
int fipa_envelope_ParseResolvers(
    struct fipa_agent_identifier_s* aid,
    mxml_node_t* resolvers_node)
{
  int i;
  mxml_node_t* agent_id_node;
  /* Here we must parse a set of agent identifiers. */
  
  /* First, find out how many resolvers there are */
  agent_id_node = mxmlFindElement(
      resolvers_node,
      resolvers_node,
      "agent-identifier",
      NULL, NULL, 
      MXML_DESCEND_FIRST);
  if (agent_id_node == NULL) {
    return MC_ERR_PARSE;
  }
  for(
      i = 1; 
      agent_id_node = mxmlFindElement(
        agent_id_node,
        resolvers_node,
        "agent-identifier",
        NULL, NULL, 
        MXML_NO_DESCEND);
      i++);

  /* Now we allocate space and parse */
  aid->resolvers = (fipa_agent_identifier_set_t*)malloc(sizeof(fipa_agent_identifier_set_t));
  aid->resolvers->num = i;
  aid->resolvers->retain_order = 0;
  aid->resolvers->fipa_agent_identifiers = (struct fipa_agent_identifier_s**)malloc(
      sizeof(struct fipa_agent_identifier_s*) * aid->resolvers->num);
  agent_id_node = mxmlFindElement(
      resolvers_node,
      resolvers_node,
      "agent-identifier",
      NULL, NULL,
      MXML_DESCEND_FIRST);
  if (agent_id_node == NULL) {
    return MC_ERR_PARSE;
  }
  fipa_envelope_ParseAgentIdentifier(
      &(aid->resolvers->fipa_agent_identifiers[0]), agent_id_node);
  for(
      i = 1; 
      agent_id_node = mxmlFindElement(
        agent_id_node,
        resolvers_node,
        "agent-identifier",
        NULL, NULL,
        MXML_NO_DESCEND);
      i++
     )
  {
    fipa_envelope_ParseAgentIdentifier(
        &(aid->resolvers->fipa_agent_identifiers[i]), agent_id_node);
  }
  return 0;
}

/* 
 * Composing Functions
 * */
char* fipa_envelope_Compose(fipa_acl_message_t* fipa_acl)
{
  char* retval;
  mxml_node_t* node;
  node = mxmlLoadString
    (
     NULL,
     "<?xml version=\"1.0\"?>",
     MXML_NO_CALLBACK
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__envelope(fipa_acl)
    );
  retval = mxmlSaveAllocString(
      node,
      MXML_NO_CALLBACK);
  mxmlDelete(node);
  return retval;
}

mxml_node_t* 
fipa_envelope_Compose__envelope(fipa_acl_message_t* fipa_acl)
{
  mxml_node_t* node;

  node = mxmlNewElement(
      NULL,
      "envelope"
      );
  /* FIXME: Only create 1 param for now. What's with multiple params? */
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__params(fipa_acl)
    );
  return node;
}

mxml_node_t*
fipa_envelope_Compose__params(fipa_acl_message_t* fipa_acl)
{
  mxml_node_t* node;
  node = mxmlNewElement(
      NULL,
      "params"
      );
  mxmlElementSetAttr(node, "index", "1");
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__to(fipa_acl)
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__from(fipa_acl)
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__acl_representation(fipa_acl)
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__payload_encoding(fipa_acl)
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__date(fipa_acl)
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__intended_receiver(fipa_acl)
    );
/*  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     fipa_envelope_Compose__received(fipa_acl)
    ); */
  return node;
}

mxml_node_t*
fipa_envelope_Compose__to(fipa_acl_message_t* fipa_acl)
{
  int i;
  int j;
  mxml_node_t* node;
  mxml_node_t* agent_identifier_node;
  mxml_node_t* name_node;
  mxml_node_t* addresses_node;
  mxml_node_t* url_node;
  node = mxmlNewElement(
      NULL,
      "to");
  for(i = 0; i < fipa_acl->receiver->num; i++) {
    agent_identifier_node = mxmlNewElement(
        node,
        "agent-identifier");
    name_node = mxmlNewElement(
        agent_identifier_node,
        "name");
    mxmlNewText(
        name_node,
        0,
        fipa_acl->receiver->fipa_agent_identifiers[i]->name);

    addresses_node = mxmlNewElement(
        agent_identifier_node,
        "addresses");
    for(
        j = 0; 
        j < fipa_acl->receiver->fipa_agent_identifiers[i]->addresses->num;
        j++
       ) 
    {
      url_node = mxmlNewElement(
          addresses_node,
          "url" );
      mxmlNewText(
          url_node,
          0,
          fipa_acl->receiver->fipa_agent_identifiers[i]->addresses->urls[j]->str );
    }
  }
  return node;
}

mxml_node_t* 
fipa_envelope_Compose__from(fipa_acl_message_t* fipa_acl)
{
  int i;
  extern mc_platform_p g_mc_platform;
  mxml_node_t* from_node;
  mxml_node_t* agent_identifier_node;
  mxml_node_t* name_node;
  mxml_node_t* addresses_node;
  mxml_node_t* url_node;
  dynstring_t* hostname;
  char buf[20];

  from_node = mxmlNewElement(
      NULL,
      "from" );
  agent_identifier_node = mxmlNewElement(
      from_node,
      "agent-identifier" );
  name_node = mxmlNewElement(
      agent_identifier_node,
      "name" );
  mxmlNewText(
      name_node,
      0,
      fipa_acl->sender->name );

  /* Compose Addresses */
  addresses_node = mxmlNewElement(
      agent_identifier_node,
      "addresses");
  if (fipa_acl->sender->addresses != NULL) {
    if (fipa_acl->sender->addresses->num != 0) {
      for(i = 0; i < fipa_acl->sender->addresses->num; i++) {
        url_node = mxmlNewElement(
            addresses_node,
            "url" );
        // uncommented the following
        mxmlNewText(
            url_node,
            0,
            fipa_acl->sender->addresses->urls[i]->str );

        // Why was this hardcoded before?
        /* mxmlNewText(
            url_node,
            0,
            "http://localhost:5555/acc"); */ 
      }
    }
  } else {
    hostname = dynstring_New();
    url_node = mxmlNewElement(
        addresses_node,
        "url" );
    dynstring_Append(hostname, "http://");
    dynstring_Append(hostname, g_mc_platform->hostname);
    sprintf(buf, ":%d", g_mc_platform->port);
    dynstring_Append(hostname, buf);
    mxmlNewText(
        url_node,
        0,
        hostname->message );
    dynstring_Destroy(hostname);
  }
  return from_node;
}

mxml_node_t*
fipa_envelope_Compose__acl_representation(fipa_acl_message_t* fipa_acl)
{
  mxml_node_t* node;
  node = mxmlNewElement(
      NULL,
      "acl-representation");
  mxmlNewText(
      node,
      0,
      "fipa.acl.rep.string.std");
  return node;
}

mxml_node_t* 
fipa_envelope_Compose__payload_encoding(fipa_acl_message_t* fipa_acl)
{
  mxml_node_t* node;
  node = mxmlNewElement(
      NULL,
      "payload-encoding");
  mxmlNewText(
      node,
      0,
      "US-ASCII" );
  return node;
}

mxml_node_t*
fipa_envelope_Compose__date(fipa_acl_message_t* fipa_acl)
{
  time_t t;
  struct tm* mytm;
  char buf[30];
  mxml_node_t* node;
  t = time(NULL);
  mytm = localtime(&t);
  if (mytm == NULL) {
    fprintf(stderr, "time failure. %s:%d\n", __FILE__, __LINE__);
    return NULL;
  }
  if (strftime(buf, sizeof(buf), "%Y%m%dT%H%M%S000", mytm) == 0) {
    fprintf(stderr, "strftime failure. %s:%d\n", __FILE__, __LINE__);
    return NULL;
  }

  node = mxmlNewElement(
      NULL,
      "date" );
  mxmlNewText(
      node,
      0,
      buf );

  return node;
}

mxml_node_t*
fipa_envelope_Compose__intended_receiver(fipa_acl_message_t* fipa_acl)
{
  int i;
  int j;
  mxml_node_t* node;
  mxml_node_t* agent_identifier_node;
  mxml_node_t* name_node;
  mxml_node_t* addresses_node;
  mxml_node_t* url_node;
  node = mxmlNewElement(
      NULL,
      "intended-receiver");
  i = 0;
  agent_identifier_node = mxmlNewElement(
      node,
      "agent-identifier");
  name_node = mxmlNewElement(
      agent_identifier_node,
      "name");
  mxmlNewText(
      name_node,
      0,
      fipa_acl->receiver->fipa_agent_identifiers[i]->name);

  addresses_node = mxmlNewElement(
      agent_identifier_node,
      "addresses");
  for(
      j = 0; 
      j < fipa_acl->receiver->fipa_agent_identifiers[i]->addresses->num;
      j++
     ) 
  {
    url_node = mxmlNewElement(
        addresses_node,
        "url" );
    mxmlNewText(
        url_node,
        0,
        fipa_acl->receiver->fipa_agent_identifiers[i]->addresses->urls[j]->str );
  }
  return node;
}
