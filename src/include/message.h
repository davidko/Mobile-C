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

#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include "mtp_http.h"
#include <mxml.h>
#ifndef _WIN32
#include <netinet/in.h>
#include "config.h"
#else 
#include <windows.h>
#include "winconfig.h"
#endif


#ifdef MC_SECURITY
#include "security/interface.h"
#endif
struct asm_node_s;


typedef enum message_type_e {
  RELAY, 
  REQUEST, 
  SUBSCRIBE,
  CANCEL,
  N_UNDRSTD,
  MOBILE_AGENT,
  QUER_IF,
  QUER_REF,
  AGENT_UPDATE,
  RETURN_MSG,
  FIPA_ACL,
  ENCRYPTED_DATA,
  ENCRYPTION_INITIALIZE,
  REQUEST_ENCRYPTION_INITIALIZE,
  NUM_MESSAGE_TYPE,
  MSG_SEC	
} message_type_t;

/*typedef enum http_type_e {
  HTTP_GET, 
  HTTP_POST, 
  HTTP_NOT
}http_type_t;   */


typedef struct message_s{
  /* The addr of the sender of the message */
  struct sockaddr_in* addr;

  /* id numbers */
  int connect_id;
  int message_id;

  /* Flag if it is an http message already */
  int isHTTP;

  /* the type of the message as discerned from the http header */
  enum message_type_e message_type;
  enum http_performative_e http_type;

  mxml_node_t* xml_root;
  mxml_node_t* xml_payload;

  /* the http header and the rest of the message */
  char* message_body;

  char *update_name;
  int update_num;

  char* from_address;
  char* to_address;

  char* target; /* This is the agency module to send the message to. It
                   will either be 'ams' for migration messages, or 'acc'
                   for ACL messages. ( And perhaps things like 'df' in the 
                   future) */

  /* This flag is set if an agent is using the xml nodes, so we know
   * not to free/delete them prematurely. */
  int agent_xml_flag;
	char* sending_agent_name;
} message_t;
typedef message_t* message_p;

typedef struct message_send_arg_s
{
	struct mc_platform_s* mc_platform;
	message_p message;
	char* privatekey;
} message_send_arg_t;

#ifdef MC_SECURITY
int
message_Decrypt(message_p message, struct asm_node_s* asm_node);

int
message_Encrypt(message_p message, struct asm_node_s* asm_node);
#endif /*MC_SECURITY*/

message_p 
message_New(void);

message_p 
message_Copy(message_p src);

struct agent_s;
struct mc_platform_s;
int
message_InitializeFromAgent(
    struct mc_platform_s* mc_platform,
    message_p message,
    struct agent_s* agent);

struct connection_s;
int
message_InitializeFromConnection(
    struct mc_platform_s* mc_platform,
    message_p message,
    struct connection_s* connection);

int
message_InitializeFromString(
    struct mc_platform_s* mc_platform,
    message_p message,
    const char* string,
    const char* destination_host,
    int destination_port,
    const char* target);

int 
message_Destroy(message_p message);

int
auth_rece_send_msg(int sockfd, char *hostname, char *message, char *privkey, char *known_host_filename);

int 
message_Send(struct mc_platform_s* mc_platform, message_p message, char *privatekey);

#ifndef _WIN32
void* 
message_send_Thread(void* arg);
#else
DWORD WINAPI
message_send_Thread(LPVOID arg);
#endif

int 
http_to_hostport(const char* http_str, char** host, int* port, char** target);
int message_Print(message_t* message);
#endif
