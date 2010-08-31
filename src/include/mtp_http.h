/* SVN FILE INFO
 * $Revision: 316 $ : Last Committed Revision
 * $Date: 2009-04-29 16:40:54 -0700 (Wed, 29 Apr 2009) $ : Last Committed Date */
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

/* The MobileC http parser.
 *
 * We should probably switch to libwww someday, but not today. */

#ifndef _MTP_HTTP_H_
#define _MTP_HTTP_H_

/**
 * \brief     http return status codes
 */
enum http_status_code_e
{
  /*Informational*/
  CONTINUE=100,
  SWITCHING_PROTOCOLS,
  PROCESSING,
  /*Success*/
  OK=200,
  CREATED,
  ACCEPTED,
  NON_AUTHORITATIVE_INFORMATION,
  NO_CONTENT,
  RESET_CONTENT,
  PARTIAL_CONTENT,
  MULTI_STATUS,
  /*Error codes*/
  BAD_REQUEST=400,
  UNAUTHORIZED,
  PAYMENT_REQUIRED,
  FORBIDDEN,
  NOT_FOUND,
  METHOD_NOT_ALLOWED,
  NOT_ACCEPTABLE,
  PROXY_AUTHENTICATION_REQUIRED,
  REQUEST_TIMEOUT,
  CONFLICT,
  GONE,
  LENGTH_REQUIRED,
  PRECONDITION_FAILED,
  REQUST_ENTITY_TOO_LARGE,
  REQUEST_URI_TOO_LONG,
  UNSUPPORTED_MEDIA_TYPE,
  REQUESTED_RANGE_NOT_SATISFIABLE,
  EXPECTATION_FAILED,
  UNPROCESSABLE_ENTITY,
  LOCKED,
  FAILED_DEPENDANCY,
  UNORDERED_COLLECTION,
  UPGRADE_REQUIRED,
  RETRY_WITH
};

/**
 * \brief       http 'verbs'
 */
enum http_performative_e
{
  HTTP_PERFORMATIVE_UNDEF=-1,
  HTTP_PERFORMATIVE_ZERO=0,
  HTTP_HEAD,
  HTTP_GET,
  HTTP_POST,
  HTTP_PUT,
  HTTP_DELETE,
  HTTP_TRACE,
  HTTP_OPTIONS,
  HTTP_CONNECT,
  HTTP_RESPONSE,
  HTTP_NUM_PERFORMATIVES
};

typedef struct mtp_http_content_s
{
  char* content_type;
  void* data;
} mtp_http_content_t;

typedef struct mtp_http_s
{
  enum http_status_code_e http_status_code;
  enum http_performative_e http_performative;
  char* http_version;
  char* host;
  char* return_code;
  char* target;       /*Target location for post/put/get etc */

  char* date;
  char* server;
  char* accept_ranges;
  char* content_length;
  char* connection;
  char* content_type;
  char* user_agent;

  char* cache_control;
  char* mime_version;

  /* The following are only valid if the message is a response */
  int response_code;
  char* response_string;

  /* The message may be a multi-part message */
  /* FIXME: We don't support nested multi-part messages for now */
  int message_parts;
  char* boundary;
  struct mtp_http_content_s* content;

  /* The following is not a part of the HTTP standard, but is useful. */
  int header_length;
} mtp_http_t;
typedef mtp_http_t* mtp_http_p;

#define SOCKET_INPUT_SIZE 4096

/**
 * \brief         Parse an html expression
 *
 * \param string  (input) The html block of text: Will parse the first expression 
 *                pointed to by 'string'.
 * \param expr    (output) The allocated expression
 * \return        A pointer to the next expression segment of the string block, or
 *                NULL.
 */
const char* http_GetExpression(const char* string, char** expr);

/**
 * \brief         Parse an expression into its name and value
 *
 * \param expression_string (input) The expression
 * \param name    (output) An allocated name string or NULL
 * \param value   (output) An allocated value string or NULL
 *
 * \return        error_code_t type
 *
 * \note          an http expression is something like 
 *                'Date: Mon, 23 May 2005 22:38:34 GMT\r\n' where 'Date' is the
 *                name and the remainder of the string is the value
 */
int http_ParseExpression(
    const char* expression_string,
    char** name,
    char** value
    );

const char* http_ParseRequest(
    mtp_http_p http,
    const char* string
    );

const char*
http_GetToken(const char* string, char** token);

int
mtp_http_Destroy(mtp_http_p http);

struct connection_s;
int
mtp_http_InitializeFromConnection(struct mtp_http_s *http, struct connection_s *connection, char *privatekey);

mtp_http_p 
mtp_http_New(void);

struct connection;
const char* 
mtp_http_ParseHeader(struct mtp_http_s* http, const char* string);

int 
mtp_http_Parse(struct mtp_http_s* http, const char* string);

struct message_s;
int 
mtp_http_ComposeMessage(struct message_s* message);

struct message_s* 
mtp_http_CreateMessage(
    mtp_http_t* mtp_http,
    char* hostname,
    int port );
#endif
