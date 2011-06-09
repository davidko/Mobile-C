/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
#ifndef _ASM_H_
#define _ASM_H_

#include "config.h"
#include "../mc_list/list.h"
#include "asm_node.h"

#ifdef MC_SECURITY

AP_QUEUE_DECL_TEMPLATE
(
 asm_queue,
 asm_node
 );

AP_QUEUE_GENERIC_DECL_TEMPLATE
(
 asm_queue,
 SearchAddr,
 asm_node_p,
 struct sockaddr_in* 
 );

AP_QUEUE_GENERIC_DECL_TEMPLATE
(
 asm_queue,
 RemoveAddr,
 int,
 struct sockaddr_in* 
 );

typedef struct mc_asm_s
{
  struct mc_platform_s* mc_platform;
  THREAD_T thread;
  asm_node_p home_encryption_info;

  /* Waiting Flag */
  int waiting;
  MUTEX_T* waiting_lock;
  COND_T* waiting_cond;

  /* The following queue is used to hold messages that have arrived
   * encrypted, but no information about the encryption could be attained.
   * The messages are checked when new encryption data arrives, to
   * see if they can be decrypted. */
  message_queue_p lost_message_queue; 
} mc_asm_t;
typedef mc_asm_t* mc_asm_p;

struct mc_platform_s;
int
asm_Destroy
(
 mc_asm_p mc_asm
 );

mc_asm_p
asm_Initialize
(
 struct mc_platform_s* mc_platform
 );

struct message_s;
int
asm_SendEncryptionData
(
 mc_asm_p security_manager,
 const char* address
 );

void
asm_Start(struct mc_platform_s* mc_platform);

#ifndef _WIN32
void*
asm_Thread(void* arg);
#else
DWORD WINAPI
asm_Thread(LPVOID arg);
#endif

#endif /*MC_SECURITY*/
#endif
