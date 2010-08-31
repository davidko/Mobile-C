/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
#ifndef _ASM_MESSAGE_PARSER_H_
#define _ASM_MESSAGE_PARSER_H_
#include "../include/mc_error.h"
#include "../include/xml_parser.h"
#include "asm_node.h"
#ifndef _WIN32
#include "config.h"
#else
#include "../winconfig.h"
#endif

#ifdef MC_SECURITY

error_code_t 
asm_message_parse(asm_node_p asm_node);

error_code_t 
asm_message_parse__encryption_data
(
 asm_node_p asm_node, 
 xml_parser_p xml_parser
 );

error_code_t
asm_message_parse__p(asm_node_p asm_node, xml_parser_p xml_parser);

error_code_t
asm_message_parse__g(asm_node_p asm_node, xml_parser_p xml_parser);

error_code_t
asm_message_parse__y(asm_node_p asm_node, xml_parser_p xml_parser);

#endif /*MC_SECURITY*/
#endif
