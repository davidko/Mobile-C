/* SVN FILE INFO
 * $Revision: 392 $ : Last Committed Revision
 * $Date: 2009-08-02 17:47:35 -0700 (Sun, 02 Aug 2009) $ : Last Committed Date */
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

#ifndef _FIPA_ACL_MESSAGE_H_
#define _FIPA_ACL_MESSAGE_H_
#include "dynstring.h"

#ifndef _FIPA_PERFORMATIVE_E_
#define _FIPA_PERFORMATIVE_E_
enum fipa_performative_e
{
  FIPA_ERROR=-1,
  FIPA_ZERO,
  FIPA_ACCEPT_PROPOSAL,
  FIPA_AGREE,
  FIPA_CANCEL,
  FIPA_CALL_FOR_PROPOSAL,
  FIPA_CONFIRM,
  FIPA_DISCONFIRM,
  FIPA_FAILURE,
  FIPA_INFORM,
  FIPA_INFORM_IF,
  FIPA_INFORM_REF,
  FIPA_NOT_UNDERSTOOD,
  FIPA_PROPOGATE,
  FIPA_PROPOSE,
  FIPA_PROXY,
  FIPA_QUERY_IF,
  FIPA_QUERY_REF,
  FIPA_REFUSE,
  FIPA_REJECT_PROPOSAL,
  FIPA_REQUEST,
  FIPA_REQUEST_WHEN,
  FIPA_REQUEST_WHENEVER,
  FIPA_SUBSCRIBE
};
#endif

#ifndef _FIPA_PROTOCOL_E_
#define _FIPA_PROTOCOL_E_
enum fipa_protocol_e
{
  FIPA_PROTOCOL_ERROR=-1,
  FIPA_PROTOCOL_NONE,       // default = 0
  FIPA_PROTOCOL_REQUEST,                                                                   
  FIPA_PROTOCOL_QUERY,
  FIPA_PROTOCOL_REQUEST_WHEN,                                                              
  FIPA_PROTOCOL_CONTRACT_NET,
  FIPA_PROTOCOL_ITERATED_CONTRACT_NET,                                                     
  FIPA_PROTOCOL_ENGLISH_AUCTION,                                                           
  FIPA_PROTOCOL_DUTCH_AUCTION,                                                             
  FIPA_PROTOCOL_BROKERING,
  FIPA_PROTOCOL_RECRUITING,                                                                
  FIPA_PROTOCOL_SUBSCRIBE,                                                                 
  FIPA_PROTOCOL_PROPOSE,                                                                   
  FIPA_PROTOCOL_END                   
};
#endif

int fipa_performative_Compose(dynstring_t* msg, enum fipa_performative_e performative);
int fipa_protocol_Compose(dynstring_t* msg, enum fipa_protocol_e protocol);

typedef enum fipa_expression_type_e
{
  FIPA_EXPR_ZERO,
  FIPA_EXPR_WORD,
  FIPA_EXPR_STRING,
  FIPA_EXPR_NUMBER,
  FIPA_EXPR_DATETIME,
  FIPA_EXPR_EXPRESSION
} fipa_expression_type_t; 

/* fipa_acl_message */
typedef struct fipa_acl_message_s
{
  enum fipa_performative_e performative;

  struct fipa_agent_identifier_s* sender;

  struct fipa_agent_identifier_set_s* receiver;
  int receiver_num;

  struct fipa_agent_identifier_set_s* reply_to;

  /*struct fipa_content_s* content; */
  struct fipa_string_s* content; /* We will write the parser for this later */

  struct fipa_expression_s* language;

  struct fipa_expression_s* encoding;
  
  struct fipa_expression_s* ontology;

  //struct fipa_word_s* protocol;
  enum fipa_protocol_e protocol;

  struct fipa_expression_s* conversation_id;

  struct fipa_expression_s* reply_with;

  struct fipa_expression_s* in_reply_to;

  struct fipa_DateTime_s* reply_by;
} fipa_acl_message_t;
typedef fipa_acl_message_t* fipa_acl_message_p;
fipa_acl_message_t* fipa_acl_message_New(void);
int fipa_acl_message_Destroy(fipa_acl_message_t* message);
fipa_acl_message_t* fipa_acl_message_Copy(fipa_acl_message_t* src);
int fipa_acl_Compose(dynstring_t** msg, fipa_acl_message_t* acl);

/* fipa_message_string */
typedef struct fipa_message_string_s
{
  char* message;
  char* parse;    /* This pointer points to some part of "message" */
} fipa_message_string_t;
typedef fipa_message_string_t* fipa_message_string_p;
fipa_message_string_t* fipa_message_string_New(void);
int fipa_message_string_Destroy(fipa_message_string_t* message);
fipa_message_string_t* fipa_message_string_Copy(fipa_message_string_t* src);

/* fipa_url_sequence */
typedef struct fipa_url_sequence_s
{
  int num;
  struct fipa_url_s** urls;
} fipa_url_sequence_t;
typedef fipa_url_sequence_t* fipa_url_sequence_p;
fipa_url_sequence_t* fipa_url_sequence_New(void);
int fipa_url_sequence_Destroy(fipa_url_sequence_t* sequence);
fipa_url_sequence_t* fipa_url_sequence_Copy(fipa_url_sequence_t* src);
int fipa_url_sequence_Compose(dynstring_t* msg, fipa_url_sequence_t* urls);

/* fipa_agent_identifier_set */
typedef struct fipa_agent_identifier_set_s
{
  int num;
  int retain_order;
  struct fipa_agent_identifier_s** fipa_agent_identifiers;
} fipa_agent_identifier_set_t;
typedef fipa_agent_identifier_set_t* fipa_agent_identifier_set_p;
fipa_agent_identifier_set_t* fipa_agent_identifier_set_New(void);
int fipa_agent_identifier_set_Destroy(fipa_agent_identifier_set_t* idset);
fipa_agent_identifier_set_t* fipa_agent_identifier_set_Copy(
    fipa_agent_identifier_set_t* src);
int fipa_agent_identifier_set_Compose(dynstring_t* msg, fipa_agent_identifier_set_t* ids);

/* fipa_agent_identifier */
typedef struct fipa_agent_identifier_s
{
  char* name;
  struct fipa_url_sequence_s* addresses;
  struct fipa_agent_identifier_set_s* resolvers;
} fipa_agent_identifier_t;
typedef fipa_agent_identifier_t* fipa_agent_identifier_p;
fipa_agent_identifier_t* fipa_agent_identifier_New(void);
int fipa_agent_identifier_Destroy(fipa_agent_identifier_t* id);
fipa_agent_identifier_t* fipa_agent_identifier_Copy(fipa_agent_identifier_t* src);
int fipa_agent_identifier_Compose(dynstring_t* msg, fipa_agent_identifier_t* id);

/* fipa_expression */
typedef struct fipa_expression_s
{
  enum fipa_expression_type_e type;
  union content_u
  {
    struct fipa_word_s* word;
    struct fipa_string_s* string;
    struct fipa_number_s* number;
    struct fipa_DateTime_s* datetime;
    struct fipa_expression_s** expression;
  } content;
} fipa_expression_t;
typedef fipa_expression_t* fipa_expression_p;
fipa_expression_t* fipa_expression_New(void);
int fipa_expression_Destroy(fipa_expression_t* expr);
fipa_expression_t* fipa_expression_Copy(fipa_expression_t* src);
int fipa_expression_Compose(dynstring_t* msg, fipa_expression_t* expr);

/* fipa_word */
typedef struct fipa_word_s
{
  char* content;
} fipa_word_t;
typedef fipa_word_t* fipa_word_p;
fipa_word_t* fipa_word_New(void);
int fipa_word_Destroy(fipa_word_t* word);
fipa_word_t* fipa_word_Copy(fipa_word_t* src);
int fipa_word_Compose(dynstring_t* msg, fipa_word_t* word);

/* fipa_string */
typedef struct fipa_string_s
{
  char* content;
} fipa_string_t;
typedef fipa_string_t* fipa_string_p;
fipa_string_t* fipa_string_New(void);
int fipa_string_Destroy(fipa_string_t* str);
fipa_string_t* fipa_string_Copy(fipa_string_t* src);
int fipa_string_Compose(dynstring_t* msg, fipa_string_t* string);

/* fipa_DateTime */
typedef struct fipa_DateTime_s
{
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  int millisecond;
  char sign; /* May be '+', '-', or '\0' */
  int is_utc; /* May be 1 (yes) or 0 (no) */
} fipa_DateTime_t;
typedef fipa_DateTime_t* fipa_DateTime_p;
fipa_DateTime_t* fipa_DateTime_New(void);
int fipa_DateTime_Destroy(fipa_DateTime_t* dt);
fipa_DateTime_t* fipa_DateTime_Copy(fipa_DateTime_t* src);
int fipa_DateTime_Compose(dynstring_t* msg, fipa_DateTime_t* date);

/* fipa_url */
typedef struct fipa_url_s
{
  char *str;
} fipa_url_t;
typedef fipa_url_t* fipa_url_p;
fipa_url_t* fipa_url_New(void);
int fipa_url_Destroy(fipa_url_t* url);
fipa_url_t* fipa_url_Copy(fipa_url_t* src);
int fipa_url_Compose(dynstring_t* msg, fipa_url_t* url);

/* fipa_Number */
typedef struct fipa_number_s
{
  char *str;
} fipa_number_t;
typedef fipa_number_t* fipa_number_p;
fipa_number_t* fipa_number_New(void);
int fipa_number_Destroy(fipa_number_t* number);
fipa_number_t* fipa_number_Copy(fipa_number_t* src);
int fipa_number_Compose(dynstring_t* msg, fipa_number_t* number);


/* Parsing Functions */
int fipa_acl_Parse(struct fipa_acl_message_s* acl, fipa_message_string_p message);
int fipa_message_parameter_Parse(struct fipa_acl_message_s* acl, fipa_message_string_p message);
int fipa_protocol_type_Parse(enum fipa_protocol_e* protocol, fipa_message_string_p message);
int fipa_message_type_Parse(enum fipa_performative_e* performative, fipa_message_string_p message);
int fipa_GetAtom( fipa_message_string_p message, char expected_atom);
int fipa_word_Parse(fipa_word_t** word, fipa_message_string_p message);
int fipa_word_Destroy(fipa_word_t* word);
int fipa_CheckNextToken(const fipa_message_string_p message, const char* token);
int fipa_expression_Parse(fipa_expression_t** expression, fipa_message_string_p message);
int fipa_GetNextWord(char** word, const fipa_message_string_p message);
int fipa_GetWholeToken(char** word, const fipa_message_string_p message);
int fipa_datetime_Parse(fipa_DateTime_p* datetime, fipa_message_string_p message);
int fipa_string_Parse( fipa_string_p* fipa_string, fipa_message_string_p message);
int fipa_agent_identifier_Parse(fipa_agent_identifier_p* aid, fipa_message_string_p message);
int fipa_agent_identifier_set_Parse(fipa_agent_identifier_set_p* agent_ids, fipa_message_string_p message);
int fipa_url_sequence_Parse(fipa_url_sequence_p* urls, fipa_message_string_p message);
int fipa_url_Parse(fipa_url_p* url, fipa_message_string_p message);

struct fipa_acl_message_s* fipa_Reply(
    struct fipa_acl_message_s* acl);

#endif
