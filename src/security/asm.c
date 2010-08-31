/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
#include "../include/mc_platform.h"
#include "../include/message.h"
#include "asm.h"
#include "asm_message_composer.h"
#include "config.h"
#include "mc_dh.h"

#ifdef MC_SECURITY

AP_QUEUE_STD_DEFN_TEMPLATE
(
 asm_queue,
 asm_node
 )

AP_QUEUE_SEARCH_TEMPLATE
(
 asm_queue,
 SearchAddr,
 asm_node,
 struct sockaddr_in*,
 (
  (key->sin_addr.s_addr == node->remote_addr->sin_addr.s_addr) &&
  (key->sin_port == node->remote_addr->sin_port)
 )
 )

AP_QUEUE_REMOVE_TEMPLATE
(
 asm_queue,
 RemoveAddr,
 asm_node,
 struct sockaddr_in*,
 (
  (key->sin_addr.s_addr == node->remote_addr->sin_addr.s_addr) &&
  (key->sin_port == node->remote_addr->sin_port)
 )
 )

int
asm_Destroy(mc_asm_p mc_asm)
{
  if (mc_asm == NULL) return 0;
  message_queue_Destroy(mc_asm->lost_message_queue);
  asm_node_Destroy(mc_asm->home_encryption_info);
  free(mc_asm);
  return 0;
}

mc_asm_p
asm_Initialize(mc_platform_p mc_platform)
{
  mc_asm_p mc_asm;
  mc_asm = (mc_asm_p)malloc(sizeof(mc_asm_t));
  CHECK_NULL(mc_asm, exit(0););

  mc_asm->waiting = 0;
  mc_asm->waiting_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(mc_asm->waiting_lock);
  mc_asm->waiting_cond = (COND_T*)malloc(sizeof(COND_T));
  COND_INIT(mc_ams->waiting_cond);

  mc_asm->mc_platform = mc_platform;
  mc_asm->home_encryption_info = (asm_node_p)malloc(sizeof(asm_node_t));
  
  /* Initialize the home encryption node */
  mc_asm->home_encryption_info->id = 0;
  mc_asm->home_encryption_info->type = DH;
  mc_asm->home_encryption_info->data.dh_data = dh_data_Initialize();
  
  mc_asm->lost_message_queue = message_queue_Initialize();
  return mc_asm;
}

  void
asm_RequestInitFromAddr(mc_platform_p mc_platform, char* addr)
{
  message_p message = message_Initialize();

  message->to_address = (char*)malloc
    (
     sizeof(char) * 
     (strlen(addr)+1)
    );
  CHECK_NULL(message->to_address, exit(0); );

  strcpy(message->to_address, addr);

  message->message_type = REQUEST_ENCRYPTION_INITIALIZE;

  message->xml_root = message_xml_compose__RequestEncryptionInit(mc_platform);

  message->message_body = mxmlSaveAllocString
    (
     message->xml_root,
     MXML_NO_CALLBACK
    );

  message_queue_Add(mc_platform->message_queue, message);
}

int
asm_SendEncryptionData(
    mc_asm_p security_manager,
    const char* address
    )
{
  message_p message = message_Initialize();

  message->to_address = (char*)malloc
    (
     sizeof(char) * (strlen(address)+1)
    );
  CHECK_NULL(message->to_address, exit(0););
  strcpy(message->to_address, address);

/*  if(_message->addr != NULL) {
    message->addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    *(message->addr) = *(_message->addr);
  } */

  message->message_type = ENCRYPTION_INITIALIZE;

  message->xml_root = 
    message_xml_compose__EncryptionInitialize
    (
     security_manager
    );

  message->message_body = mxmlSaveAllocString
    (
     message->xml_root,
     MXML_NO_CALLBACK
    );

  message_queue_Add
    (
     security_manager->mc_platform->message_queue,
     message
    );
  return MC_SUCCESS;
}

  void
asm_Start(mc_platform_p mc_platform)
{
  mc_asm_p security_manager = mc_platform->security_manager;
#ifndef _WIN32
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  if(mc_platform->stack_size[MC_THREAD_ASM] != -1) {
    pthread_attr_setstacksize
      (
       &attr,
       mc_platform->stack_size[MC_THREAD_ASM]
      );
  }
#else
  int stack_size;
  if (mc_platform->stack_size[MC_THREAD_ASM] < 1) {
    /* In windows, 0 is default, not min */
    stack_size = mc_platform->stack_size[MC_THREAD_ASM]+1; 
  } else {
    stack_size = mc_platform->stack_size[MC_THREAD_ASM];
  }
#endif
  THREAD_CREATE
    (
     &security_manager->thread,
     asm_Thread,
     mc_platform
    );
}

#ifndef _WIN32
  void*
asm_Thread(void* arg)
#else
  DWORD WINAPI
asm_Thread(LPVOID arg)
#endif
{
  /* This thread will encrypt any unencrypted messages added to its
   * queue and decrypt encrypted messages. */
  mc_platform_p mc_platform = (mc_platform_p)arg;
  message_p message;
  asm_node_p asm_node;
  int old_asm_queue_size;
  int i;

  MUTEX_LOCK(mc_platform->asm_queue->lock);
  old_asm_queue_size = mc_platform->asm_queue->size;
  MUTEX_UNLOCK(mc_platform->asm_queue->lock);
  while(1) 
  {
    MUTEX_LOCK(mc_platform->asm_message_queue->lock);
    while
      (
       (mc_platform->asm_message_queue->size == 0) &&
       (mc_platform->asm_queue->size == old_asm_queue_size)
      ) 
      {
        /* Set waiting flag */
        MUTEX_LOCK(mc_platform->security_manager->waiting_lock);
        mc_platform->security_manager->waiting = 1;
        COND_BROADCAST(mc_platform->security_manager->waiting_cond);
        MUTEX_UNLOCK(mc_platform->security_manager->waiting_lock);
        /* Wait for activity */
        COND_WAIT
          (
           mc_platform->asm_message_queue->cond,
           mc_platform->asm_message_queue->lock
          );
      }
    MUTEX_UNLOCK(mc_platform->asm_message_queue->lock);
    MUTEX_LOCK(mc_platform->asm_queue->lock);
    while (old_asm_queue_size != mc_platform->asm_queue->size)
    {
      old_asm_queue_size = mc_platform->asm_queue->size;
      MUTEX_UNLOCK(mc_platform->asm_queue->lock);
      /* Go through each of the lost messages and see if they can be
       * decrypted/encrypted. */
      for
        (
         i = 0;
         i < mc_platform->security_manager->lost_message_queue->size;
         i++
        )
        {
          message = message_queue_Pop
            (
             mc_platform->security_manager->lost_message_queue
            );
          if (message == NULL) {
            message_queue_Add
              (
               mc_platform->security_manager->lost_message_queue,
               message
              );
            break;
          }
          if (message->addr == NULL) {
            message_queue_Add
              (
               mc_platform->security_manager->lost_message_queue,
               message
              );
            break;
          }
          if 
            (
             asm_queue_SearchAddr(mc_platform->asm_queue, message->addr)
            ) 
            {
              message_queue_Add
                (
                 mc_platform->message_queue,
                 message
                );
            } else {
            message_queue_Add
              (
               mc_platform->security_manager->lost_message_queue,
               message
              );
          }
        }
    }
    MUTEX_UNLOCK(mc_platform->asm_queue->lock);
    message = message_queue_Pop(mc_platform->asm_message_queue);
    if (message != NULL) 
    {
      asm_node = asm_queue_SearchAddr
        (
         mc_platform->asm_queue,
         message->addr
        );
      switch(message->message_type)
      {
        case ENCRYPTED_DATA:
          if (asm_node == NULL) {
            /* The encryption session was never initialized with this host,
             * according to this agency. Let us initialize it now. */
            asm_RequestInitFromAddr
              (
               mc_platform,
               message->from_address
              );
            message_queue_Add
              (
               mc_platform->security_manager->lost_message_queue,
               message
              );
            continue;
          }
          if (
              message_Decrypt(message, asm_node)
          ) 
          { 
            fprintf(stderr, "Decrypt error. %s:%d\n", __FILE__, __LINE__);
            message_Destroy(message); 
          } else {
            message_queue_Add
              (
               mc_platform->message_queue,
               message
              );
          }
          break;
        case ENCRYPTION_INITIALIZE:
          asm_queue_RemoveAddr
            (
             mc_platform->asm_queue,
             message->addr
            );
          asm_queue_Add
            (
             mc_platform->asm_queue,
             asm_node_Initialize(message, mc_platform->security_manager)
            );
          message_Destroy(message);
          break;
        default:
          if (asm_node == NULL) {
            /* Send our encryption data to other host */
            asm_SendEncryptionData
              (
               mc_platform->security_manager,
               message->to_address
              );
            /* Request an initialization from the other host */
            asm_RequestInitFromAddr
              (
               mc_platform,
               message->to_address
              );
            /* Add message to queue of lost message. We must wait until
             * we receive encryption data back from the other host
             * before we can proceed. */
            message_queue_Add
              (
               mc_platform->security_manager->lost_message_queue,
               message
              );
          } else {
            message_Encrypt(message, asm_node);
            message_queue_Add
              (
               mc_platform->message_queue,
               message
              );
          }
          break;
      }
    }
  }
}

#endif /* MC_SECURITY */
