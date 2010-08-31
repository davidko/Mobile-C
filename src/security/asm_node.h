/* SVN FILE INFO
 * $Revision: 199 $ : Last Committed Revision
 * $Date: 2008-07-11 10:33:31 -0700 (Fri, 11 Jul 2008) $ : Last Committed Date */
#ifndef _ASM_NODE_H_
#define _ASM_NODE_H_


#include <mxml.h>
#ifndef _WIN32
#include <netinet/in.h>
#else
#include <windows.h>
#include <winsock.h>
#endif
#include "../include/macros.h"
#include "../include/message.h"
#include "xyssl-0.9/include/xyssl/dhm.h"
#include "xyssl-0.9/include/xyssl/rsa.h"
#include "xyssl-0.9/include/xyssl/aes.h"
#ifndef _WIN32
#include "config.h"
#else
#include "../winconfig.h"
#endif

#ifdef MC_SECURITY

typedef enum asm_encryption_type_e 
{
  NONE,                 /* No Encryption */
  DH,                   /* Diffie-Hellman */
  NUM_ENCRYPT_METHODS
} asm_encryption_type_t;

/* 
 * Holds data for a Diffie-Hellman encrypted data exchange session 
 * */
typedef struct dh_data_s
{
  dhm_context dhm;
  rsa_context rsa;
  aes_context aes;
} dh_data_t;
typedef dh_data_t* dh_data_p;

/* Use a union here in case we want to support multiple methods of
 * authentication / encryption */
typedef union asm_encryption_data_u
{
  struct dh_data_s* dh_data; /* Diffie-Hellman Data */
  /* Add other structs here for other protocols */
} asm_encryption_data_t;
typedef asm_encryption_data_t* asm_encryption_data_p;

typedef struct asm_node_s
{
  int id;
  struct sockaddr_in* remote_addr; /* hostname:port */
  asm_encryption_type_t type;
  union asm_encryption_data_u data;
  mxml_node_t* xml_encrypt_root;
} asm_node_t;
typedef asm_node_t* asm_node_p;

int
asm_node_Destroy
(
 asm_node_p asm_node
 );

struct mc_asm_s;
asm_node_p
asm_node_Initialize(message_p message, struct mc_asm_s* security_manager);

#endif /*MC_SECURITY*/
#endif
