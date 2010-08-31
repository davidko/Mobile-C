/* SVN FILE INFO
 * $Revision: 199 $ : Last Committed Revision
 * $Date: 2008-07-11 10:33:31 -0700 (Fri, 11 Jul 2008) $ : Last Committed Date */
#ifndef _WIN32
#include <netdb.h>
#include "config.h"
#else
#include <windows.h>
#include "../winconfig.h"
#endif
#include "asm.h"
#include "asm_message_parser.h"
#include "asm_node.h"
#include "xyssl-0.9/include/xyssl/havege.h"
#include "xyssl-0.9/include/xyssl/bignum.h"
  
#ifdef MC_SECURITY

  int
asm_node_Destroy(asm_node_p asm_node)
{
  if (asm_node == NULL) {
    return MC_SUCCESS;
  }

  if (asm_node->xml_encrypt_root != NULL) {
    mxmlDelete(asm_node->xml_encrypt_root);
  }

  if (asm_node->remote_addr != NULL) {
    free(asm_node->remote_addr);
  }

  if (asm_node->data.dh_data) {
    dh_data_Destroy(asm_node->data.dh_data);
  }

  free(asm_node);
  return MC_SUCCESS;
}

  asm_node_p
asm_node_Initialize(message_p message, mc_asm_p security_manager)
{
  int n = 0;
  unsigned char *buf;
  char* hostname;
  char* port_str;
  int port;
#ifndef _WIN32
  char* save_ptr; /* For re-entrant strtok */
#endif
  asm_node_p asm_node;
  havege_state hs;
  havege_init(&hs);
  buf = (unsigned char*)malloc(sizeof(char) * 1024);
  CHECK_NULL(buf, exit(0););
  asm_node = (asm_node_p)malloc(sizeof(asm_node_t));
  CHECK_NULL(asm_node, exit(0););
  memset(asm_node, 0, sizeof(asm_node_t));
  asm_node->data.dh_data = (dh_data_p)malloc(sizeof(dh_data_t));
  CHECK_NULL(asm_node->data.dh_data, exit(0););
  memset(asm_node->data.dh_data, 0, sizeof(dh_data_t));
  asm_node->remote_addr = (struct sockaddr_in*)malloc
    (
     sizeof(struct sockaddr_in)
    );
  CHECK_NULL(asm_node->remote_addr, exit(0););
  hostname = strtok_r
    (
     message->from_address,
     ":",
     &save_ptr
    );
  port_str = strtok_r
    (
     NULL,
     ":",
     &save_ptr
    );
  port = atoi(port_str);

  *(asm_node->remote_addr) = *(message->addr);
  asm_node->remote_addr->sin_port = htons(port);
  asm_node->xml_encrypt_root = message->xml_payload;
  if(asm_message_parse(asm_node)) {
    fprintf(stderr, "ASM Parse Error %s:%d\n", __FILE__, __LINE__);
    goto err_cleanup;
  } else {
    /* At this point all the dh data should be initialized. Here, 
     * we will determine the secret shared key, and set up the aes. */
    if
      (
       mpi_copy
       (
        &(asm_node->data.dh_data->dhm.X),
        &(security_manager->home_encryption_info->data.dh_data->dhm.X)
       )
      )
      {
        fprintf(stderr, "Error. %s:%d\n", __FILE__,__LINE__);
      }

    if 
      ( 
       dhm_calc_secret
       (
        &(asm_node->data.dh_data->dhm),
        buf,
        &n
       )
      )
      {
        fprintf(stderr, "Error. %s:%d\n", __FILE__, __LINE__);
        goto err_cleanup;
      }
    aes_set_key
      (
       &(asm_node->data.dh_data->aes),
       buf,
       256
      );
    free(buf);
    return asm_node;
  }
err_cleanup:
  free(asm_node);
  free(buf);
  return NULL;
}

#endif /*MC_SECURITY*/
