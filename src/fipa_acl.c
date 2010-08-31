/* SVN FILE INFO
 * $Revision: 517 $ : Last Committed Revision
 * $Date: 2010-06-11 12:06:47 -0700 (Fri, 11 Jun 2010) $ : Last Committed Date */
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
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <strings.h>
#else
#define strcasecmp(a, b) \
  _stricmp(a, b)
#endif
#include "include/fipa_acl.h"
#include "include/mc_error.h"
#include "include/macros.h"

#define FREEMEM(x) \
  if (x != NULL) free(x)

/* fipa_acl_message */
fipa_acl_message_t* fipa_acl_message_New(void)
{
  fipa_acl_message_t* acl;
  acl = (fipa_acl_message_t*)malloc(sizeof(fipa_acl_message_t));
  memset(acl, 0, sizeof(fipa_acl_message_t));
  return acl;
}

int fipa_acl_message_Destroy(fipa_acl_message_t* message)
{

  if (message == NULL) return 0;
  fipa_agent_identifier_Destroy(message->sender);
  fipa_agent_identifier_set_Destroy(message->receiver);
  fipa_agent_identifier_set_Destroy(message->reply_to);
  fipa_string_Destroy(message->content);
  fipa_expression_Destroy(message->language);
  fipa_expression_Destroy(message->encoding);
  fipa_expression_Destroy(message->ontology);
  //fipa_word_Destroy(message->protocol);
  fipa_expression_Destroy(message->conversation_id);
  fipa_expression_Destroy(message->reply_with);
  fipa_expression_Destroy(message->in_reply_to);
  fipa_DateTime_Destroy(message->reply_by);
  
  free(message);
  return 0;
}

fipa_acl_message_t* fipa_acl_message_Copy(fipa_acl_message_t* src)
{
  fipa_acl_message_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_acl_message_New();
  copy->performative = src->performative;
  copy->sender = fipa_agent_identifier_Copy(src->sender);
  copy->receiver = fipa_agent_identifier_set_Copy(src->receiver);
  copy->reply_to = fipa_agent_identifier_set_Copy(src->reply_to);
  copy->content = fipa_string_Copy(src->content);
  copy->language = fipa_expression_Copy(src->language);
  copy->encoding = fipa_expression_Copy(src->encoding);
  copy->ontology = fipa_expression_Copy(src->ontology);
  //copy->protocol = fipa_word_Copy(src->protocol);
  copy->protocol = src->protocol;
  copy->conversation_id = fipa_expression_Copy(src->conversation_id);
  copy->reply_with = fipa_expression_Copy(src->reply_with);
  copy->in_reply_to = fipa_expression_Copy(src->in_reply_to);
  copy->reply_by = fipa_DateTime_Copy(src->reply_by);

  return copy;
}

/* fipa_message_string */
fipa_message_string_t* fipa_message_string_New(void)
{
  fipa_message_string_t* str;
  str = (fipa_message_string_t*)malloc(sizeof(fipa_message_string_t));
  memset(str, 0, sizeof(fipa_message_string_t));
  return str;
}

int fipa_message_string_Destroy(fipa_message_string_t* message)
{
  if (message == NULL) return 0;
  if (message->message != NULL) {
    free(message->message);
  }
  free(message); 
  return 0;
}

fipa_message_string_t* fipa_message_string_Copy(fipa_message_string_t* src)
{
  fipa_message_string_t* copy;
  if (src == NULL) return NULL;
  copy->message = strdup(src->message);
  copy->parse = copy->message + (src->parse - src->message);
  return copy;
}

/* fipa_url_sequence */
fipa_url_sequence_t* fipa_url_sequence_New(void)
{
  fipa_url_sequence_t* sequence;
  sequence = (fipa_url_sequence_t*)malloc(sizeof(fipa_url_sequence_t));
  memset(sequence, 0, sizeof(fipa_url_sequence_t));
  return sequence;
}

int fipa_url_sequence_Destroy(fipa_url_sequence_t* sequence)
{
  int i;
  if (sequence == NULL) return 0;
  for (i = 0; i < sequence->num; i++) {
    fipa_url_Destroy(sequence->urls[i]);
  }
  free(sequence->urls); 
  free(sequence);
  return 0;
}

fipa_url_sequence_t* fipa_url_sequence_Copy(fipa_url_sequence_t* src)
{
  int i;
  fipa_url_sequence_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_url_sequence_New();
  copy->num = src->num;
  copy->urls = (fipa_url_t**)malloc(
      sizeof(fipa_url_t*) * src->num);
  for( i = 0; i < src->num; i++) {
    copy->urls[i] = fipa_url_Copy(src->urls[i]);
  }
  return copy;
}

/* fipa_agent_identifier_set */
fipa_agent_identifier_set_t* fipa_agent_identifier_set_New(void)
{
  fipa_agent_identifier_set_t* set;
  set = (fipa_agent_identifier_set_t*)malloc(sizeof(fipa_agent_identifier_set_t));
  memset(set, 0, sizeof(fipa_agent_identifier_set_t));
  return set;
}

int fipa_agent_identifier_set_Destroy(fipa_agent_identifier_set_t* idset)
{
  int i;
  if (idset == NULL) return 0;
  for(i = 0; i < idset->num; i++) {
    fipa_agent_identifier_Destroy(idset->fipa_agent_identifiers[i]);
  }
  free(idset->fipa_agent_identifiers); 
  free(idset);
  return 0;
}

fipa_agent_identifier_set_t* fipa_agent_identifier_set_Copy(
    fipa_agent_identifier_set_t* src)
{
  int i;
  fipa_agent_identifier_set_t* copy;

  if (src == NULL) { return NULL; }
  copy = fipa_agent_identifier_set_New();
  copy->num = src->num;
  copy->retain_order = src->retain_order;
  copy->fipa_agent_identifiers = (fipa_agent_identifier_t**)malloc(
      sizeof(fipa_agent_identifier_t*) * src->num);
  for(i = 0; i < src->num; i++) {
    copy->fipa_agent_identifiers[i] = fipa_agent_identifier_Copy(
        src->fipa_agent_identifiers[i]);
  }

  return copy;
}

/* fipa_agent_identifier */
fipa_agent_identifier_t* fipa_agent_identifier_New(void)
{
  fipa_agent_identifier_t* id;
  id = (fipa_agent_identifier_t*)malloc(sizeof(fipa_agent_identifier_t));
  memset(id, 0, sizeof(fipa_agent_identifier_t));
  return id;
}

int fipa_agent_identifier_Destroy(fipa_agent_identifier_t* id)
{
  if (id == NULL) return 0;
  if (id->name != NULL) {
    free(id->name);
  }
  fipa_url_sequence_Destroy(id->addresses);
  fipa_agent_identifier_set_Destroy(id->resolvers);
  free(id); 
  return 0;
}

fipa_agent_identifier_t* fipa_agent_identifier_Copy(fipa_agent_identifier_t* src)
{
  fipa_agent_identifier_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_agent_identifier_New();
  copy->name = strdup(src->name);
  copy->addresses = fipa_url_sequence_Copy(src->addresses);
  copy->resolvers = fipa_agent_identifier_set_Copy(src->resolvers);
  return copy;
}

/* fipa_expression */
fipa_expression_t* fipa_expression_New(void)
{
  fipa_expression_t* expr;
  expr = (fipa_expression_t*)malloc(sizeof(fipa_expression_t));
  memset(expr, 0, sizeof(fipa_expression_t));
  return expr;
}

int fipa_expression_Destroy(fipa_expression_t* expr)
{
  int i;
  if (expr == NULL) return 0;
  switch (expr->type) {
    case FIPA_EXPR_WORD:
      fipa_word_Destroy(expr->content.word);
      break;
    case FIPA_EXPR_STRING:
      fipa_string_Destroy(expr->content.string);
      break;
    case FIPA_EXPR_NUMBER:
      fipa_number_Destroy(expr->content.number);
      break;
    case FIPA_EXPR_DATETIME:
      fipa_DateTime_Destroy(expr->content.datetime);
      break;
    case FIPA_EXPR_EXPRESSION:
      if (expr->content.expression == NULL) break;
      for (i = 0; expr->content.expression[i] != NULL; i++) {
        fipa_expression_Destroy(expr->content.expression[i]);
      }
      FREEMEM(expr->content.expression);
      break;
    default:
      break;
  }
  free(expr);
  return 0;
}

fipa_expression_t* fipa_expression_Copy(fipa_expression_t* src)
{
  int i, num;
  fipa_expression_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_expression_New();
  copy->type = src->type;
  switch(src->type) {
    case FIPA_EXPR_WORD:
      copy->content.word = fipa_word_Copy(src->content.word);
      break;
    case FIPA_EXPR_STRING:
      copy->content.string = fipa_string_Copy(src->content.string);
      break;
    case FIPA_EXPR_NUMBER:
      copy->content.number = fipa_number_Copy(src->content.number);
      break;
    case FIPA_EXPR_DATETIME:
      copy->content.datetime = fipa_DateTime_Copy(src->content.datetime);
      break;
    case FIPA_EXPR_EXPRESSION:
      /* We need to figure out how many expressions there are first. */
      for(i = 0; src->content.expression[i] != NULL; i++);
      /* Now copy them */
      num = i;
      copy->content.expression = (fipa_expression_t**)malloc(
          sizeof(fipa_expression_t*) * (num + 1));
      for(i = 0; i < num; i++) {
        copy->content.expression[i] = fipa_expression_Copy(
            src->content.expression[i] );
      }
      copy->content.expression[num] = NULL;
      break;
    default:
      fipa_expression_Destroy(copy);
      return NULL;
  }
  return copy;
}
      
/* fipa_word */
fipa_word_t* fipa_word_New(void)
{
  fipa_word_t* word;
  word = (fipa_word_t*)malloc(sizeof(fipa_word_t));
  memset(word, 0, sizeof(fipa_word_t));
  return word;
}

int fipa_word_Destroy(fipa_word_t* word)
{
  if (word == NULL) return 0;
  if (word->content != NULL) {
    free( word->content );
  }
  free(word);
  return 0;
}

fipa_word_t* fipa_word_Copy(fipa_word_t* src)
{
  fipa_word_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_word_New();
  copy->content = strdup(src->content);
  return copy;
}

/* fipa_string */
fipa_string_t* fipa_string_New(void)
{
  fipa_string_t* str;
  str = (fipa_string_t*)malloc(sizeof(fipa_string_t));
  memset(str, 0, sizeof(fipa_string_t));
  return str;
}

int fipa_string_Destroy(fipa_string_t* str)
{
  if (str == NULL) return 0;
  if (str->content != NULL) {
    free(str->content);
  }
  free(str);
  return 0;
}

fipa_string_t* fipa_string_Copy(fipa_string_t* src)
{
  fipa_string_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_string_New();
  copy->content = strdup(src->content);
  return copy;
}

/* fipa_DateTime */
fipa_DateTime_t* fipa_DateTime_New(void)
{
  fipa_DateTime_t* dt;
  dt = (fipa_DateTime_t*)malloc(sizeof(fipa_DateTime_t));
  memset(dt, 0, sizeof(fipa_DateTime_t));
  return dt;
}

int fipa_DateTime_Destroy(fipa_DateTime_t* dt)
{
  if(dt == NULL) return 0;
  free(dt);
  return 0;
}

fipa_DateTime_t* fipa_DateTime_Copy(fipa_DateTime_t* src)
{
  fipa_DateTime_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_DateTime_New();
  *copy = *src;
  return copy;
}

/* fipa_url */
fipa_url_t* fipa_url_New(void)
{
  fipa_url_t* url;
  url = (fipa_url_t*)malloc(sizeof(fipa_url_t));
  memset(url, 0, sizeof(fipa_url_t));
  return url;
}

int fipa_url_Destroy(fipa_url_t* url)
{
  if (url == NULL) return 0;
  if (url->str != NULL) {
    free(url->str);
  }
  free(url);
  return 0;
}

fipa_url_t* fipa_url_Copy(fipa_url_t* src)
{
  fipa_url_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_url_New();
  copy->str = strdup(src->str);
  return copy;
}

/* fipa_Number */
fipa_number_t* fipa_number_New(void)
{
  fipa_number_t* num;
  num = (fipa_number_t*)malloc(sizeof(fipa_number_t));
  memset(num, 0, sizeof(fipa_number_t));
  return num;
}

int fipa_number_Destroy(fipa_number_t* number)
{
  if (number == NULL) return 0;
  if (number->str != NULL){
    free(number->str);
  }
  free(number);
  return 0;
}

fipa_number_t* fipa_number_Copy(fipa_number_t* src)
{
  fipa_number_t* copy;
  if (src == NULL) return NULL;
  copy = fipa_number_New();
  copy->str = strdup(src->str);
  return copy;
}

/* Parsing Functions */
int fipa_acl_Parse(fipa_acl_message_p acl, fipa_message_string_p message)
{
  int err = 0;
  if (fipa_GetAtom(message,'(')) {
    err = MC_ERR_PARSE;
    goto exit;
  }
  if (fipa_message_type_Parse(&(acl->performative), message)) {
    err = MC_ERR_PARSE;
    goto exit;
  }
  while(fipa_GetAtom(message, ')')){
    err = fipa_message_parameter_Parse(acl, message);
    if (err) return err;
  }

exit:
  return err;
}

int fipa_message_parameter_Parse(fipa_acl_message_p acl, fipa_message_string_p message)
{
  int err;
  fipa_word_t* word = NULL;
  char* parameter;
  if((err = fipa_GetAtom(message, ':'))) return err;
  if((err = fipa_word_Parse(&word, message))) return err;
  parameter = word->content;
  if (!strcmp(parameter, "sender")) {
    err = fipa_agent_identifier_Parse(&(acl->sender), message);
  } else if (!strcmp(parameter, "receiver")) {
    err = fipa_agent_identifier_set_Parse(&(acl->receiver), message);
  } else if (!strcmp(parameter, "content")) {
    err = fipa_string_Parse(&(acl->content), message);
  } else if (!strcmp(parameter, "reply-with")) {
    err = fipa_expression_Parse(&(acl->reply_with), message);
  } else if (!strcmp(parameter, "reply-by")) {
    err = fipa_datetime_Parse(&(acl->reply_by), message);
  } else if (!strcmp(parameter, "in-reply-to")) {
    err = fipa_expression_Parse(&(acl->in_reply_to), message);
  } else if (!strcmp(parameter, "reply-to")) {
    err = fipa_agent_identifier_set_Parse(&(acl->reply_to), message);
  } else if (!strcmp(parameter, "language")) {
    err = fipa_expression_Parse(&(acl->language), message);
  } else if (!strcmp(parameter, "encoding")) {
    err = fipa_expression_Parse(&(acl->encoding), message);
  } else if (!strcmp(parameter, "ontology")) {
    err = fipa_expression_Parse(&(acl->ontology), message);
  } else if (!strcmp(parameter, "protocol")) {
    //err = fipa_word_Parse(word, message);
    err = fipa_protocol_type_Parse(&(acl->protocol), message);
  } else if (!strcmp(parameter, "conversation-id")) {
    err = fipa_expression_Parse(&(acl->conversation_id), message);
  } else {
    /* FIXME: We do not deal with user defined parameters yet. */
    fprintf(stderr, "FIXME: No handling of user defined parameters. %s:%d\n",
        __FILE__, __LINE__);
    err = MC_ERR_PARSE;
  }
  fipa_word_Destroy(word);
  return err;
}

int fipa_protocol_type_Parse(
    enum fipa_protocol_e* protocol, 
    fipa_message_string_p message
    )
{
  int err = 0;
  fipa_word_p word;
  err = fipa_word_Parse(&word, message);
  if (err) return err;
  if(!strcasecmp(word->content, "fipa-request")) {
    *protocol = FIPA_PROTOCOL_REQUEST;
  } else if (!strcasecmp(word->content, "fipa-query")) {
    *protocol = FIPA_PROTOCOL_QUERY;
  } else if (!strcasecmp(word->content, "fipa-request-when")) {
    *protocol = FIPA_PROTOCOL_REQUEST_WHEN;
  } else if (!strcasecmp(word->content, "fipa-contract-net")) {
    *protocol = FIPA_PROTOCOL_CONTRACT_NET;
  } else if (!strcasecmp(word->content, "fipa-iterated-contract-net")) {
    *protocol = FIPA_PROTOCOL_ITERATED_CONTRACT_NET;
  } else if (!strcasecmp(word->content, "fipa-auction-english")) {
    *protocol = FIPA_PROTOCOL_ENGLISH_AUCTION;
  } else if (!strcasecmp(word->content, "fipa-auction-dutch")) {
    *protocol = FIPA_PROTOCOL_DUTCH_AUCTION;
  } else if (!strcasecmp(word->content, "fipa-brokering")) {
    *protocol = FIPA_PROTOCOL_BROKERING;
  } else if (!strcasecmp(word->content, "fipa-recruiting")) {
    *protocol = FIPA_PROTOCOL_RECRUITING;
  } else if (!strcasecmp(word->content, "fipa-subscribe")) {
    *protocol = FIPA_PROTOCOL_SUBSCRIBE;
  } else if (!strcasecmp(word->content, "fipa-propose")) {
    *protocol = FIPA_PROTOCOL_PROPOSE;
  } else {
    fprintf(stderr, "Unknown protocol: '%s'. %s:%d\n",
        word->content, __FILE__, __LINE__);
    err = MC_ERR_PARSE;
  }
  fipa_word_Destroy(word);
  return err;
}

int fipa_message_type_Parse(
    enum fipa_performative_e* performative, 
    fipa_message_string_p message
    )
{
  int err = 0;
  fipa_word_p word;
  err = fipa_word_Parse(&word, message);
  if (err) return err;
  if(!strcasecmp(word->content, "accept-proposal")) {
    *performative = FIPA_ACCEPT_PROPOSAL;
  } else if (!strcasecmp(word->content, "agree")) {
    *performative = FIPA_AGREE;
  } else if (!strcasecmp(word->content, "cancel")) {
    *performative = FIPA_CANCEL;
  } else if (!strcasecmp(word->content, "call-for-proposal")) {
    *performative = FIPA_CALL_FOR_PROPOSAL;
  } else if (!strcasecmp(word->content, "confirm")) {
    *performative = FIPA_CONFIRM;
  } else if (!strcasecmp(word->content, "disconfirm")) {
    *performative = FIPA_DISCONFIRM;
  } else if (!strcasecmp(word->content, "failure")) {
    *performative = FIPA_FAILURE;
  } else if (!strcasecmp(word->content, "inform")) {
    *performative = FIPA_INFORM;
  } else if (!strcasecmp(word->content, "inform-if")) {
    *performative = FIPA_INFORM_IF;
  } else if (!strcasecmp(word->content, "inform-ref")) {
    *performative = FIPA_INFORM_REF;
  } else if (!strcasecmp(word->content, "not-understood")) {
    *performative = FIPA_NOT_UNDERSTOOD;
  } else if (!strcasecmp(word->content, "propogate")) {
    *performative = FIPA_PROPOGATE;
  } else if (!strcasecmp(word->content, "propose")) {
    *performative = FIPA_PROPOSE;
  } else if (!strcasecmp(word->content, "proxy")) {
    *performative = FIPA_PROXY;
  } else if (!strcasecmp(word->content, "query-if")) {
    *performative = FIPA_QUERY_IF;
  } else if (!strcasecmp(word->content, "query-ref")) {
    *performative = FIPA_QUERY_REF;
  } else if (!strcasecmp(word->content, "refuse")) {
    *performative = FIPA_REFUSE;
  } else if (!strcasecmp(word->content, "reject-proposal")) {
    *performative = FIPA_REJECT_PROPOSAL;
  } else if (!strcasecmp(word->content, "request")) {
    *performative = FIPA_REQUEST;
  } else if (!strcasecmp(word->content, "request-when")) {
    *performative = FIPA_REQUEST_WHEN;
  } else if (!strcasecmp(word->content, "request-whenever")) {
    *performative = FIPA_REQUEST_WHENEVER;
  } else if (!strcasecmp(word->content, "subscribe")) {
    *performative = FIPA_SUBSCRIBE;
  } else {
    fprintf(stderr, "Unknown performative: '%s'. %s:%d\n",
        word->content, __FILE__, __LINE__);
    err = MC_ERR_PARSE;
  }
  fipa_word_Destroy(word);
  return err;
}

int fipa_GetAtom(
    fipa_message_string_p message,
    char expected_atom
    )
{
  while
    (
     (*(message->parse) >= 0x00) &&
     (*(message->parse) <= 0x20)
    )
    {
      if (*(message->parse) == 0x00)
        return MC_ERR_PARSE;
      message->parse++;
    }
  if( *(message->parse) == expected_atom) {
    message->parse++;
    return MC_SUCCESS;
  } else {
    return MC_ERR_PARSE;
  }
}

int fipa_word_Parse(fipa_word_t** word, fipa_message_string_p message)
{
  char* tmp;
  int i = 0;
  /* Get rid of leading whitespace */
  while
    (
     (*(message->parse)>=0x00) &&
     (*(message->parse)<=0x20)
    )
    {
      /* If we encounter a null zero, return error. */
      if (*(message->parse) == '\0') {
        return MC_ERR_PARSE;
      }
      message->parse++;
    }
  /* Count number of characters in word */
  tmp = message->parse;
  while (*tmp > 0x20) {
    tmp++;
    i++;
  }
  *word = (fipa_word_t*)malloc(sizeof(fipa_word_t));
  CHECK_NULL(*word, exit(0););
  (*word)->content = (char*)malloc
    (
     sizeof(char) * (i+1)
    );
  CHECK_NULL((*word)->content, exit(0););

  /* Copy word */
  i = 0;
  while( *(message->parse) > 0x20 ) {
    ((*word)->content)[i] = *(message->parse);
    message->parse++;
    i++;
  }
  ((*word)->content)[i] = '\0';
  return MC_SUCCESS;
}

int fipa_CheckNextToken(const fipa_message_string_p message, const char* token)
{
  char* tmp = message->parse;
  while 
    (
     (*tmp >= 0x00) &&
     (*tmp <= 0x20)
    )
      tmp++;
  while (*token != '\0') {
    if (*token != *tmp) {
      return 0; /* False value, not error code */
    }
    token++;
    tmp++;
  }
  return 1; /* True */
}

int fipa_expression_Parse(fipa_expression_t** expression, fipa_message_string_p message)
{
  int i=0;
  *expression = (fipa_expression_t*)malloc(sizeof(fipa_expression_t));
  /* The expression may contain a word, string, date, or list of expressions. First,
   * lets check the recursive case, which is a parentheses-bound list of 
   * expressions. */
  if (fipa_CheckNextToken(message, "(")) {
    (*expression)->type = FIPA_EXPR_EXPRESSION;
    if(fipa_GetAtom(message, '(')) {
      /* This should never happen */
      fprintf(stderr, "Fatal error. %s:%d\n", __FILE__, __LINE__);
      exit(0);
    }
    for 
      (
       i = 0; 
       !fipa_expression_Parse( &(((*expression)->content.expression)[i]), message);
       i++
      );
    if(fipa_GetAtom(message, ')')) {
      fprintf(stderr, "Parse error. %s:%d\n", __FILE__, __LINE__);
      return MC_ERR_PARSE;
    }
  } else if (
      /* The expression may be a date-time */
      !fipa_datetime_Parse(&((*expression)->content.datetime), message)
      ) 
  {
    (*expression)->type = FIPA_EXPR_DATETIME;
  } else if (
      /* The expression may be a string */
      !fipa_string_Parse(&((*expression)->content.string), message)
      )
  {
    (*expression)->type = FIPA_EXPR_STRING;
  } else if (
      /* The expression may be a word */
      !fipa_word_Parse(&((*expression)->content.word), message)
      )
  {
    (*expression)->type=FIPA_EXPR_WORD;
  }
  else
  {
    /* It's not correct */
    return MC_ERR_PARSE;
  }
  return MC_SUCCESS;
}

int fipa_GetNextWord(char** word, const fipa_message_string_p message)
{
  char *tmp;
  int i = 0;
  int j;
  /* Skip leading whitespace */
  tmp = message->parse;
  while
    (
     (*tmp >= 0x00) &&
     (*tmp <= 0x20)
    )
      tmp++;
  /* See how big the word is */
  /* The first character has special rules */
  if
    (
     ((*tmp >= 0x00) && (*tmp <= 0x20)) ||
     (*tmp == '(') ||
     (*tmp == ')') ||
     (*tmp == '#') ||
     ((*tmp >= 0x30) && (*tmp <= 0x39)) || /* May not start with a digit */
     (*tmp == '-') ||
     (*tmp == '@')
    )
      return ERR;
  i++;
  tmp++;
  /* Count the rest of the chars */
  while
    (
     ((*tmp < 0x00) || (*tmp > 0x20)) &&
     (*tmp != '(') &&
     (*tmp != ')')
    ) {
      i++;
      tmp++;
    }
  /* Allocate the memory */
  *word = (char*)malloc(sizeof(char) * (i+1));

  for (j = 0; j < i; j++) {
    *((*word) + j) = *(message->parse+j);
  }
  *((*word)+j) = '\0';
  return MC_SUCCESS;
}

int fipa_GetWholeToken(char** word, fipa_message_string_p message)
{
  char *tmp;
  int i = 0;
  int j;
  /* Skip leading whitespace */
  tmp = message->parse;
  while
    (
     (*tmp >= 0x00) &&
     (*tmp <= 0x20)
    )
    {
      tmp++;
      message->parse++;
    }
  /* See how big the word is */
  i++;
  tmp++;
  /* Count the rest of the chars */
  while
    (
     ((*tmp < 0x00) || (*tmp > 0x20))
    ) {
      i++;
      tmp++;
    }
  /* Allocate the memory */
  *word = (char*)malloc(sizeof(char) * (i+1));

  for (j = 0; j < i; j++) {
    *((*word) + j) = *(message->parse+j);
  }
  *((*word)+j) = '\0';
  return MC_SUCCESS;
}

int fipa_datetime_Parse(fipa_DateTime_p* datetime, fipa_message_string_p message)
{
  char *word;
  char *tmp;
  int i;
  char buf[5];
  /* Check to see if there is a sign */
  fipa_GetWholeToken(&word, message);
  tmp = word;
  if (
      (*tmp == '+') ||
      (*tmp == '-')
     )
    tmp++;
  /* The next 8 characters must be digits */
  for(i = 0; i < 8; i++) {
    if (*tmp < 0x30 || *tmp > 0x39) {
      free(word);
      return MC_ERR_PARSE;
    }
    tmp++;
  }
  /* Next character must be 'T' */
  if (*tmp == 'T') {
    tmp++;
  } else {
    free(word);
    return MC_ERR_PARSE;
  }
  /* Next 9 characters must be digits */
  for(i = 0; i < 9; i++) {
    if(*tmp < 0x30 || *tmp > 0x39) {
      free(word);
      return MC_ERR_PARSE;
    }
    tmp++;
  }

  /* If we get here, the string is definately a date-time. */
  *datetime = (fipa_DateTime_p)malloc(sizeof(fipa_DateTime_t));
  tmp = word;
  switch(*tmp) {
    case '+':
      (*datetime)->sign = '+';
      tmp++;
      message->parse++;
      break;
    case '-':
      (*datetime)->sign = '-';
      tmp++;
      message->parse++;
      break;
    default:
      break;
  }

  /* Get the year */
  for(i = 0; i < 4; i++) {
    buf[i] = *tmp;
    tmp++;
    message->parse++;
  }
  buf[i] = '\0';
  (*datetime)->year = atoi(buf);

  /* Get the month */
  for(i = 0; i < 2; i++) {
    buf[i] = *tmp;
    tmp++;
    message->parse++;
  }
  buf[i] = '\0';
  (*datetime)->month = atoi(buf);

  /* Get the day */
  for(i = 0; i < 2; i++) {
    buf[i] = *tmp;
    tmp++;
    message->parse++;
  }
  buf[i] = '\0';
  (*datetime)->month = atoi(buf);

  /* Skip the T */
  if (*tmp != 'T') {
    /* Something is very wrong */
    fprintf(stderr, "Fatal Error. %s:%d\n", __FILE__, __LINE__);
    exit(0);
  }
  tmp++;
  message->parse++;

  /* Get the hour */
  for(i = 0; i < 2; i++) {
    buf[i] = *tmp;
    tmp++;
    message->parse++;
  }
  buf[i] = '\0';
  (*datetime)->hour = atoi(buf);

  /* Get the minute */
  for(i = 0; i < 2; i++) {
    buf[i] = *tmp;
    tmp++;
    message->parse++;
  }
  buf[i] = '\0';
  (*datetime)->minute = atoi(buf);

  /* Get the second */
  for(i = 0; i < 2; i++) {
    buf[i] = *tmp;
    tmp++;
    message->parse++;
  }
  buf[i] = '\0';
  (*datetime)->second = atoi(buf);

  /* Get the millisecond */
  for(i = 0; i < 3; i++) {
    buf[i] = *tmp;
    tmp++;
    message->parse++;
  }
  buf[i] = '\0';
  (*datetime)->millisecond = atoi(buf);

  if (*tmp == 'Z') {
    (*datetime)->is_utc = 1;
    message->parse++;
  }
  else
    (*datetime)->is_utc = 0;
  free(word);
  return MC_SUCCESS;
}

int fipa_string_Parse( fipa_string_p* fipa_string, fipa_message_string_p message)
{
  int len;
  char *tmp;
  /* Eat the starting quotation mark */
  if(fipa_GetAtom(message, '\"')) {
    return MC_ERR_PARSE;
  }

  tmp = message->parse;
  len = 0;
  while 
    (
     (*tmp != '\0')
    )
    {
      if (*tmp == '\"') {
        break;
      }
      if (*tmp == '\\') {
        tmp++;
        len++;
      }
      tmp++;
      len++;
    }
  *fipa_string = (fipa_string_p)malloc(sizeof(fipa_string_t));
  (*fipa_string)->content = (char*)malloc
    (
     sizeof(char) * (len + 1)
    );
  len = 0;
  while (message->parse < tmp) {
    ((*fipa_string)->content)[len] = *(message->parse);
    len++;
    message->parse++;
  }
  ((*fipa_string)->content)[len] = '\0';
  /* Eat ending quotation mark */
  if(fipa_GetAtom(message, '\"')) {
    return MC_ERR_PARSE;
  }
  return MC_SUCCESS;
}

int fipa_agent_identifier_Parse(fipa_agent_identifier_p* aid, fipa_message_string_p message)
{
  int err = 0;
  fipa_word_t* word = NULL;
  char *rewind;
  if
    (
     (err = fipa_GetAtom(message, '(') ) 
    ) return err;
  if
    (
     (err = fipa_word_Parse(&word, message) )
    ) 
    {
      fipa_word_Destroy(word);
      return err;
    }
  if (strcmp(word->content, "agent-identifier")) {
    free(word->content);
    fipa_word_Destroy(word);
    return MC_ERR_PARSE;
  }
  fipa_word_Destroy(word);
  word = NULL;
  if 
    (
     (err = fipa_GetAtom(message, ':') )
    ) return err;
  if
    (
     (err = fipa_word_Parse(&word, message))
    ) 
    {
      fipa_word_Destroy(word);
      return err;
    }
  if (strcmp(word->content, "name")) {
    return MC_ERR_PARSE;
  }
  fipa_word_Destroy(word);
  word = NULL;
  /* This is probably a valid aid, so allocate it. */
  *aid = (fipa_agent_identifier_p)malloc(sizeof(fipa_agent_identifier_t));
  memset(*aid, 0, sizeof(fipa_agent_identifier_t));
  if
    (
     (err = fipa_word_Parse(&word, message))
    )
    {
      fipa_word_Destroy(word);
      return err;
    }
  (*aid)->name = (char*)malloc
    (
     sizeof(char) * 
     (strlen(word->content)+1)
    );
  CHECK_NULL((*aid)->name, exit(0););
  strcpy((*aid)->name, word->content);
  /* No need to keep the word around... */
  fipa_word_Destroy(word);

  /* Now we need to see if there are addresses and/or resolvers. */

  rewind = message->parse;
  if
    (
     (!(err = fipa_GetAtom(message, ':')))
    ) 
    {
      if
        (
         (err = fipa_word_Parse(&word, message))
        ) {
          message->parse = rewind;
          fipa_word_Destroy(word);
          return MC_ERR_PARSE;
        }

      if (!strcmp(word->content, "addresses"))
      {
        err = fipa_url_sequence_Parse(&((*aid)->addresses), message);
        if(err) {
          message->parse = rewind;
          fipa_word_Destroy(word);
          return err;
        }
      } else if (!strcmp(word->content, "resolvers"))
      {
        err = fipa_agent_identifier_set_Parse(&((*aid)->resolvers), message);
        if (err) {
          message->parse = rewind;
          fipa_word_Destroy(word);
          return err;
        }
      } else {
        message->parse = rewind;
      }
    }
  /* Parse final ')' */
  err = fipa_GetAtom(message, ')');
  fipa_word_Destroy(word);
  if (err) {return MC_ERR_PARSE;}
  return MC_SUCCESS;
  /* FIXME: We will deal with resolvers and custom fields later */
}

int fipa_url_sequence_Parse(fipa_url_sequence_p* urls, fipa_message_string_p message)
{
  int err;
  fipa_word_p word;
  int i;
  if
    (
     (err = fipa_GetAtom(message, '(') )
    ) return err;
  if
    (
     (err = fipa_word_Parse(&word, message) )
    ) return err;
  if ( strcmp(word->content, "sequence")) {
    fipa_word_Destroy(word);
    return MC_ERR_PARSE;
  }
  fipa_word_Destroy(word);
  *urls = fipa_url_sequence_New();
  /* FIXME: We only alloc space for 20 addresses. In the future, we should count
   * how many addresses there are and allocate that much. */
  (*urls)->urls = (fipa_url_t**)malloc(sizeof(fipa_url_t*)*20);
  i = 0;
  (*urls)->num = 0;
  while( fipa_GetAtom(message, ')') ) {
    fipa_url_Parse(&(*urls)->urls[i], message);
    i++;
    (*urls)->num++;
  }
  return 0;
}

int fipa_url_Parse(fipa_url_p* url, fipa_message_string_p message)
{
  fipa_word_p word = NULL;
  int err;
  *url = (fipa_url_t*)malloc(sizeof(fipa_url_t));
  err = fipa_word_Parse(&word, message);
  if (err) {
    free(*url);
    if(word == NULL) fipa_word_Destroy(word);
    fprintf(stderr, "Error parsing. %s:%d\n", __FILE__, __LINE__);
    return err;
  }
  (*url)->str = strdup(word->content);
  fipa_word_Destroy(word);
  return 0;
}

/* We will use the following function to parse both agent identifier
 * sets and sequences, since they are exactly the same except sequences
 * retain the order of agent id's. */
int fipa_agent_identifier_set_Parse(fipa_agent_identifier_set_p* agent_ids, fipa_message_string_p message)
{
  int err;
  fipa_word_p word;
  int i=0;
  /* FIXME */
  if
    (
     (err = fipa_GetAtom(message, '(') )
    ) return err;
  if
    (
     (err = fipa_word_Parse(&word, message) )
    ) return err;
  if (!strcmp(word->content, "set")) {
    *agent_ids = (fipa_agent_identifier_set_p)malloc(sizeof(struct fipa_agent_identifier_set_s));
    (*agent_ids)->retain_order = 0;
  } else if (!strcmp(word->content, "sequence")) {
    *agent_ids = (fipa_agent_identifier_set_p)malloc(sizeof(struct fipa_agent_identifier_set_s));
    (*agent_ids)->retain_order = 1;
  } else {
    free(word->content);
    free(word);
    return MC_ERR_PARSE;
  }
  free(word->content);
  free(word);
  (*agent_ids)->fipa_agent_identifiers = 
    (fipa_agent_identifier_p*)malloc(20 * sizeof(fipa_agent_identifier_t*));
  while( fipa_GetAtom(message, ')') ) {
    err = fipa_agent_identifier_Parse
      (&(((*agent_ids)->fipa_agent_identifiers)[i]), message);
    if(err) return err;
    i++;
  }
  (*agent_ids)->num = i;
  return MC_SUCCESS;
}

/* Composing Functions */

int fipa_acl_Compose(dynstring_t** msg, fipa_acl_message_t* acl)
{
  *msg = dynstring_New();
  dynstring_Append(*msg, "(");
  fipa_performative_Compose(*msg, acl->performative);
  if (acl->sender != NULL) {
    dynstring_Append(*msg, ":sender ");
    fipa_agent_identifier_Compose(*msg, acl->sender);
    dynstring_Append(*msg, "\n");
  }
  if (acl->receiver != NULL) {
    dynstring_Append(*msg, ":receiver ");
    fipa_agent_identifier_set_Compose(*msg, acl->receiver);
    dynstring_Append(*msg, "\n");
  }
  if (acl->reply_to) {
    dynstring_Append(*msg, ":reply-to ");
    fipa_agent_identifier_set_Compose(*msg, acl->reply_to);
    dynstring_Append(*msg, "\n");
  }
  if (acl->content) {
    dynstring_Append(*msg, ":content ");
    fipa_string_Compose(*msg, acl->content);
    dynstring_Append(*msg, "\n");
  }
  if (acl->language) {
    dynstring_Append(*msg, ":language ");
    fipa_expression_Compose(*msg, acl->language);
    dynstring_Append(*msg, "\n");
  }
  if (acl->encoding) {
    dynstring_Append(*msg, ":encoding ");
    fipa_expression_Compose(*msg, acl->encoding);
    dynstring_Append(*msg, "\n");
  }
  if (acl->ontology) {
    dynstring_Append(*msg, ":ontology ");
    fipa_expression_Compose(*msg, acl->ontology);
    dynstring_Append(*msg, "\n");
  }
  if (acl->protocol > FIPA_PROTOCOL_NONE) {
    dynstring_Append(*msg, ":protocol ");
    //fipa_word_Compose(*msg, acl->protocol);
    fipa_protocol_Compose(*msg, acl->protocol);
    dynstring_Append(*msg, "\n");
  }
  if (acl->conversation_id) {
    dynstring_Append(*msg, ":conversation-id ");
    fipa_expression_Compose(*msg, acl->conversation_id);
    dynstring_Append(*msg, "\n");
  }
  if (acl->reply_with) {
    dynstring_Append(*msg, ":reply-with ");
    fipa_expression_Compose(*msg, acl->reply_with);
    dynstring_Append(*msg, "\n");
  }
  if (acl->in_reply_to) {
    dynstring_Append(*msg, ":in-reply-to ");
    fipa_expression_Compose(*msg, acl->in_reply_to);
    dynstring_Append(*msg, "\n");
  }
  if (acl->reply_by) {
    dynstring_Append(*msg, ":reply-by ");
    fipa_DateTime_Compose(*msg, acl->reply_by);
    dynstring_Append(*msg, "\n");
  }
  dynstring_Append(*msg, ")");
  return 0;
}

int fipa_protocol_Compose(dynstring_t* msg, enum fipa_protocol_e protocol)
{
  switch(protocol)
  {
    case FIPA_PROTOCOL_REQUEST:                                                                   
      dynstring_Append(msg, "fipa-request"); 
      break;
    case FIPA_PROTOCOL_QUERY:
      dynstring_Append(msg, "fipa-query"); 
      break;
    case FIPA_PROTOCOL_REQUEST_WHEN:                                                              
      dynstring_Append(msg, "fipa-request-when"); 
      break;
    case FIPA_PROTOCOL_CONTRACT_NET:
      dynstring_Append(msg, "fipa-contract-net"); 
      break;
    case FIPA_PROTOCOL_ITERATED_CONTRACT_NET:                                                     
      dynstring_Append(msg, "fipa-iterated-contract-net"); 
      break;
    case FIPA_PROTOCOL_ENGLISH_AUCTION:                                                           
      dynstring_Append(msg, "fipa-auction-english"); 
      break;
    case FIPA_PROTOCOL_DUTCH_AUCTION:                                                             
      dynstring_Append(msg, "fipa-auction-dutch"); 
      break;
    case FIPA_PROTOCOL_BROKERING:
      dynstring_Append(msg, "fipa-brokering"); 
      break;
    case FIPA_PROTOCOL_RECRUITING:                                                                
      dynstring_Append(msg, "fipa-recruiting"); 
      break;
    case FIPA_PROTOCOL_SUBSCRIBE:                                                                 
      dynstring_Append(msg, "fipa-subscribe"); 
      break;
    case FIPA_PROTOCOL_PROPOSE:                                                                   
      dynstring_Append(msg, "fipa-propose"); 
      break;
    default:
      return MC_ERR_PARSE;
  }
  return 0;
}

int fipa_performative_Compose(dynstring_t* msg, enum fipa_performative_e performative)
{
  switch(performative) {
    case FIPA_ACCEPT_PROPOSAL:
      dynstring_Append(msg, "accept-proposal ");
      break;
    case FIPA_AGREE:
      dynstring_Append(msg, "agree ");
      break;
    case FIPA_CANCEL:
      dynstring_Append(msg, "cancel ");
      break;
    case FIPA_CALL_FOR_PROPOSAL:
      dynstring_Append(msg, "call-for-proposal ");
      break;
    case FIPA_CONFIRM:
      dynstring_Append(msg, "confirm ");
      break;
    case FIPA_DISCONFIRM:
      dynstring_Append(msg, "disconfirm ");
      break;
    case FIPA_FAILURE:
      dynstring_Append(msg, "failure ");
      break;
    case FIPA_INFORM:
      dynstring_Append(msg, "inform ");
      break;
    case FIPA_INFORM_IF:
      dynstring_Append(msg, "inform-if ");
      break;
    case FIPA_INFORM_REF:
      dynstring_Append(msg, "inform-ref ");
      break;
    case FIPA_NOT_UNDERSTOOD:
      dynstring_Append(msg, "not-understood ");
      break;
    case FIPA_PROPOGATE:
      dynstring_Append(msg, "propogate ");
      break;
    case FIPA_PROPOSE:
      dynstring_Append(msg, "propose ");
      break;
    case FIPA_PROXY:
      dynstring_Append(msg, "proxy ");
      break;
    case FIPA_QUERY_IF:
      dynstring_Append(msg, "query-if ");
      break;
    case FIPA_QUERY_REF:
      dynstring_Append(msg, "query-ref ");
      break;
    case FIPA_REFUSE:
      dynstring_Append(msg, "refuse ");
      break;
    case FIPA_REJECT_PROPOSAL:
      dynstring_Append(msg, "reject-proposal ");
      break;
    case FIPA_REQUEST:
      dynstring_Append(msg, "request ");
      break;
    case FIPA_REQUEST_WHEN:
      dynstring_Append(msg, "request-when ");
      break;
    case FIPA_REQUEST_WHENEVER:
      dynstring_Append(msg, "request-whenever ");
      break;
    case FIPA_SUBSCRIBE:
      dynstring_Append(msg, "subscribe ");
      break;
    default:
      return MC_ERR_PARSE;
  }
  return 0;
}

int fipa_url_sequence_Compose(dynstring_t* msg, fipa_url_sequence_t* urls)
{
  int i;
  if(urls == NULL) return 0;
  if(urls->num == 0) return 0;
  dynstring_Append(msg, "(sequence ");
  for(i = 0; i < urls->num; i++) {
    fipa_url_Compose(msg, urls->urls[i]);
  }
  dynstring_Append(msg, ") ");
  return 0;
}

int fipa_agent_identifier_set_Compose(dynstring_t* msg, fipa_agent_identifier_set_t* ids)
{
  int i;
  if(ids == NULL) return 0;
  if(ids->num == 0) return 0;
  dynstring_Append(msg, "(set ");
  for(i = 0; i < ids->num; i++) {
    fipa_agent_identifier_Compose(msg, ids->fipa_agent_identifiers[i]);
  }
  dynstring_Append(msg, ") ");
  return 0;
}

int fipa_agent_identifier_Compose(dynstring_t* msg, fipa_agent_identifier_t* id)
{
  if(id == NULL) return 0;
  dynstring_Append(msg, "(agent-identifier ");
  dynstring_Append(msg, ":name ");
  dynstring_Append(msg, id->name);
  dynstring_Append(msg, " ");

  if (id->addresses != NULL) {
    if (id->addresses->num != 0) {
      dynstring_Append(msg, ":addresses ");
      fipa_url_sequence_Compose(msg, id->addresses);
    }
  }

  if (id->resolvers != NULL) {
    if (id->resolvers->num != 0) {
      dynstring_Append(msg, ":resolvers ");
      fipa_agent_identifier_set_Compose(msg, id->resolvers);
    }
  }

  dynstring_Append(msg, ") ");
  return 0;
}

int fipa_expression_Compose(dynstring_t* msg, fipa_expression_t* expr)
{
  fipa_expression_t* tmp_expr;
  if (expr == NULL) return 0;
  switch(expr->type) {
    case FIPA_EXPR_WORD:
      fipa_word_Compose(msg, expr->content.word);
      break;
    case FIPA_EXPR_STRING:
      fipa_string_Compose(msg, expr->content.string);
      break;
    case FIPA_EXPR_NUMBER:
      fipa_number_Compose(msg, expr->content.number);
      break;
    case FIPA_EXPR_DATETIME:
      fipa_DateTime_Compose(msg, expr->content.datetime);
      break;
    case FIPA_EXPR_EXPRESSION:
      tmp_expr = expr->content.expression[0];
      while(tmp_expr != NULL) {
        fipa_expression_Compose(msg, tmp_expr);
        tmp_expr++;
      }
      break;
    default:
      return MC_ERR_PARSE;
  }
  return 0;
}

int fipa_word_Compose(dynstring_t* msg, fipa_word_t* word)
{
  if (word == NULL) return 0;
  dynstring_Append(msg, word->content);
  dynstring_Append(msg, " ");
  return 0;
}

int fipa_string_Compose(dynstring_t* msg, fipa_string_t* string)
{
  if (string == NULL) return 0;
  dynstring_Append(msg, "\"");
  dynstring_Append(msg, string->content);
  dynstring_Append(msg, "\" ");
  return 0;
}

int fipa_DateTime_Compose(dynstring_t* msg, fipa_DateTime_t* date)
{
  char buf[40];

  if(date == NULL) return 0;
  dynstring_Append(msg, &date->sign);
  sprintf(buf, "%04d%02d%02dT%02d%02d%02d%03d", 
      date->year,
      date->month,
      date->day,
      date->hour,
      date->minute,
      date->second,
      date->millisecond
      );
  dynstring_Append(msg, buf);
  return 0;
}


int fipa_url_Compose(dynstring_t* msg, fipa_url_t* url)
{
  if(url == NULL) return 0;
  dynstring_Append(msg, url->str);
  dynstring_Append(msg, " ");
  return 0;
}

int fipa_number_Compose(dynstring_t* msg, fipa_number_t* number)
{
  if (number == NULL) return 0;
  dynstring_Append(msg, number->str);
  dynstring_Append(msg, " ");
  return 0;
}

struct fipa_acl_message_s* fipa_Reply(
    struct fipa_acl_message_s* acl)
{
  /* Constuct a reply to the function argument */
  struct fipa_acl_message_s* acl_reply;

  acl_reply = fipa_acl_message_New();

  /* Set up the receiver */
  /* If the 'reply-to' field is set, use that. */
  if (acl->reply_to != NULL && acl->reply_to->num != 0) {
    acl_reply->receiver = fipa_agent_identifier_set_Copy(acl->reply_to);
  } else {
    acl_reply->receiver = fipa_agent_identifier_set_New();
    acl_reply->receiver->num = 1;
    acl_reply->receiver->retain_order = 0;
    acl_reply->receiver->fipa_agent_identifiers = 
       (fipa_agent_identifier_t**)malloc(sizeof(fipa_agent_identifier_t*));
    acl_reply->receiver->fipa_agent_identifiers[0] = 
       fipa_agent_identifier_Copy(acl->sender );
    acl_reply->protocol = acl->protocol;
    if(acl->conversation_id)
       acl_reply->conversation_id = fipa_expression_Copy(acl->conversation_id);
  }
  
  return acl_reply;
}

#undef FREEMEM
