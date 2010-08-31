/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
#include "asm_message_parser.h"
#include "../include/xml_helper.h"
#ifndef _WIN32
#include "config.h"
#else
#include "../winconfig.h"
#endif
#ifdef MC_SECURITY

error_code_t 
asm_message_parse(asm_node_p asm_node)
{
  xml_parser_t xml_parser;
  xml_parser.root = asm_node->xml_encrypt_root;
  xml_parser.node = asm_node->xml_encrypt_root;
  return asm_message_parse__encryption_data(asm_node, &xml_parser);
}

error_code_t 
asm_message_parse__encryption_data
(
 asm_node_p asm_node, 
 xml_parser_p xml_parser
 )
{
  int ret;
  const mxml_node_t *encryption_data_node;
  const char* attribute;
  /* Make sure this is the 'ENCRYPTION_DATA' element */
  if (
      strcmp(
        (const char*)xml_get_element_name((const mxml_node_t*)xml_parser->node),
        "ENCRYPTION_DATA"
        )
     )
  {
    return MC_ERR_PARSE;
  }
  encryption_data_node = xml_parser->node;

  attribute = mxmlElementGetAttr
    (
     (mxml_node_t*)xml_parser->node,
     "protocol"
    );
  if (!strcmp(attribute, "diffie-hellman")) {
    xml_parser->node = xml_get_child
      (
       encryption_data_node,
       "P",
       1
      );
    ret = asm_message_parse__p(asm_node, xml_parser);
    if (ret)
      return ret;
    xml_parser->node = xml_get_child
      (
       encryption_data_node,
       "G",
       1
      );
    ret = asm_message_parse__g(asm_node, xml_parser);
    if(ret)
      return ret;
    xml_parser->node = xml_get_child
      (
       encryption_data_node,
       "Y",
       1
      );
    ret = asm_message_parse__y(asm_node, xml_parser);
    if(ret) return ret;
  } else {
    /* Other protocols not yet supported. */
    return MC_ERR_PARSE;
  }
  return MC_SUCCESS;
}

error_code_t
asm_message_parse__p(asm_node_p asm_node, xml_parser_p xml_parser)
{
  char* P;
  error_code_t ret = MC_SUCCESS;
  if (xml_parser->node == NULL) {
    return MC_ERR_PARSE;
  }
  P = xml_get_text(xml_parser->node);
  if (P != NULL) {
    if (mpi_read_string(&(asm_node->data.dh_data->dhm.P), 16, P)) {
      fprintf(stderr, "Parse error.  %s:%d\n", __FILE__, __LINE__);
      ret = MC_ERR_PARSE;
    }
    goto cleanup;
  } else {
    ret = MC_ERR_PARSE;
    return ret;
  }

cleanup:
  if(P!=NULL)
    free(P);
  return ret;
}

error_code_t
asm_message_parse__g(asm_node_p asm_node, xml_parser_p xml_parser)
{
  char* G;
  error_code_t ret = MC_SUCCESS;
  if (xml_parser->node == NULL) {
    return MC_ERR_PARSE;
  }
  G = xml_get_text(xml_parser->node);
  if (G == NULL) { ret = MC_ERR_PARSE; goto cleanup; }

  if(mpi_read_string(&(asm_node->data.dh_data->dhm.G), 16, G)) {
    fprintf(stderr, "Parse error.  %s:%d\n", __FILE__, __LINE__);
    ret = MC_ERR_PARSE;
  }

cleanup:
  if(G!=NULL)
    free(G);
  return ret;
}

error_code_t
asm_message_parse__y(asm_node_p asm_node, xml_parser_p xml_parser)
{
  char* Y;
  error_code_t ret = MC_SUCCESS;
  if (xml_parser->node == NULL) {
    return MC_ERR_PARSE;
  }
  Y = xml_get_text(xml_parser->node);
  if(Y == NULL) {ret = MC_ERR_PARSE; goto cleanup;}
  if(mpi_read_string(&(asm_node->data.dh_data->dhm.GY), 16, Y)) {
    fprintf(stderr, "Parse error.  %s:%d\n", __FILE__, __LINE__);
    ret = MC_ERR_PARSE;
  }

cleanup:
  if(Y!=NULL)
    free(Y);
  return ret;
}
#endif /*MC_SECURITY*/
