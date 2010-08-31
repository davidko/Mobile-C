/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
#include "asm_message_composer.h"
#include "config.h"
#include "../include/mc_platform.h"
#ifdef MC_SECURITY

mxml_node_t*
message_xml_compose__RequestEncryptionInit(mc_platform_p mc_platform)
{
  char* buf;
  mxml_node_t* node;
  mxml_node_t* gaf_message_node;
  mxml_node_t* message_node;
  char xml_root_string[] = 
    "<?xml version=\"1.0\"?>";
  node = mxmlLoadString
    (
     NULL,
     xml_root_string,
     MXML_NO_CALLBACK
    );

  gaf_message_node = mxmlNewElement
    (
     node,
     "MOBILEC_MESSAGE"
    );
  message_node = mxmlNewElement
    (
     gaf_message_node,
     "MESSAGE"
    );
  mxmlElementSetAttr
    (
     message_node,
     "message",
     "REQUEST_ENCRYPTION_INITIALIZE"
    );
  buf = (char*)malloc
    (
     sizeof(char) * 
     (strlen(mc_platform->hostname)+10)
    );
  sprintf(buf, "%s:%d", mc_platform->hostname, mc_platform->port);
  mxmlElementSetAttr
    (
     message_node,
     "from",
     buf
    );
  free(buf);
  return node;
}
  
mxml_node_t*
message_xml_compose__EncryptionInitialize(mc_asm_p security_manager)
{
  char* buf;
  mxml_node_t *node;
  mxml_node_t *gaf_message_node;
  mxml_node_t *message_node;
  node = mxmlLoadString
    (
     NULL,
     "<?xml version=\"1.0\"?>\n<!DOCTYPE myMessage SYSTEM \"mobilec.dtd\">",
     MXML_NO_CALLBACK
    );
  gaf_message_node = mxmlNewElement
    (
     node,
     "MOBILEC_MESSAGE"
    );
  message_node = mxmlNewElement
    (
     gaf_message_node,
     "MESSAGE"
    );
  mxmlElementSetAttr
    (
     message_node,
     "message",
     "ENCRYPTION_INITIALIZE"
    );
  buf = (char*)malloc
    (
     sizeof(char) * 
     (strlen(security_manager->mc_platform->hostname)+10)
    );
  sprintf
    (
     buf, 
     "%s:%d", 
     security_manager->mc_platform->hostname,
     security_manager->mc_platform->port
    );
  mxmlElementSetAttr
    (
     message_node,
     "from",
     buf
    );
  free(buf);

  mxmlAdd
    (
     message_node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     message_xml_compose__encryption_data(security_manager)
    );
  return node;
}

mxml_node_t*
message_xml_compose__encryption_data(mc_asm_p security_manager)
{
  mxml_node_t* node;
  node = mxmlNewElement
    (
     NULL,
     "ENCRYPTION_DATA"
    );
  mxmlElementSetAttr
    (
     node,
     "protocol",
     "diffie-hellman"
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     message_xml_compose__p(security_manager)
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     message_xml_compose__g(security_manager)
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     message_xml_compose__y(security_manager)
    );
  return node;
}

mxml_node_t*
message_xml_compose__p(mc_asm_p security_manager)
{
  mxml_node_t* node = NULL;
  int p_len = 0;
  char *string = NULL;
  node = mxmlNewElement
    (
     NULL,
     "P"
    );
  mpi_write_string
    (
     &(security_manager->home_encryption_info->data.dh_data->dhm.P),
     16,
     string,
     &p_len
    );
  string = (char*) malloc
    (
     sizeof(char) * p_len
    );
  CHECK_NULL(string, exit(0););
  if
    (
     mpi_write_string
     (
      &(security_manager->home_encryption_info->data.dh_data->dhm.P),
      16,
      string,
      &p_len
     )
    )
    {
      goto err_cleanup;
    }
  mxmlNewText
    (
     node,
     0,
     string
    );
  free(string);
  return node;

err_cleanup:
  fprintf(stderr, "Error. %s:%d\n", __FILE__,__LINE__);
  if(string) free(string);
  if(node) mxmlDelete(node);
  return NULL;
}

mxml_node_t*
message_xml_compose__g(mc_asm_p security_manager)
{
  mxml_node_t* node = NULL;
  int p_len = 0;
  char *string = NULL;
  node = mxmlNewElement
    (
     NULL,
     "G"
    );
  mpi_write_string
    (
     &(security_manager->home_encryption_info->data.dh_data->dhm.G),
     16,
     string,
     &p_len
    );
  string = (char*) malloc
    (
     sizeof(char) * p_len
    );
  CHECK_NULL(string, exit(0););
  if
    (
     mpi_write_string
     (
      &(security_manager->home_encryption_info->data.dh_data->dhm.G),
      16,
      string,
      &p_len
     )
    )
    {
      goto err_cleanup;
    }
  mxmlNewText
    (
     node,
     0,
     string
    );
  free(string);
  return node;

err_cleanup:
  fprintf(stderr, "Error. %s:%d\n", __FILE__,__LINE__);
  if(string) free(string);
  if(node) mxmlDelete(node);
  return NULL;
}

mxml_node_t*
message_xml_compose__y(mc_asm_p security_manager)
{
  mxml_node_t* node = NULL;
  int p_len = 0;
  char *string = NULL;
  node = mxmlNewElement
    (
     NULL,
     "Y"
    );
  mpi_write_string
    (
     &(security_manager->home_encryption_info->data.dh_data->dhm.GX),
     16,
     string,
     &p_len
    );
  string = (char*) malloc
    (
     sizeof(char) * p_len
    );
  CHECK_NULL(string, exit(0););
  if
    (
     mpi_write_string
     (
      &(security_manager->home_encryption_info->data.dh_data->dhm.GX),
      16,
      string,
      &p_len
     )
    )
    {
      goto err_cleanup;
    }
  mxmlNewText
    (
     node,
     0,
     string
    );
  free(string);
  return node;

err_cleanup:
  fprintf(stderr, "Error. %s:%d\n", __FILE__,__LINE__);
  if(string) free(string);
  if(node) mxmlDelete(node);
  return NULL;
}

#endif /* MC_SECURITY */
