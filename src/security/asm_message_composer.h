/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
#ifndef _ASM_MESSAGE_COMPOSER_H_
#define _ASM_MESSAGE_COMPOSER_H_

#include <mxml.h>
#include "asm.h"
#include "config.h"

#ifdef MC_SECURITY

struct mc_platform;
mxml_node_t*
message_xml_compose__RequestEncryptionInit(struct mc_platform_s* mc_platform);

  
mxml_node_t*
message_xml_compose__EncryptionInitialize(mc_asm_p security_manager);

mxml_node_t*
message_xml_compose__encryption_data(mc_asm_p security_manager);

mxml_node_t*
message_xml_compose__p(mc_asm_p security_manager);

mxml_node_t*
message_xml_compose__g(mc_asm_p security_manager);

mxml_node_t*
message_xml_compose__y(mc_asm_p security_manager);
#endif /*MC_SECURITY*/
#endif
