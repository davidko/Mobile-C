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

#ifndef _XML_PARSER_H_
#define _XML_PARSER_H_
#include <mxml.h>
#include "macros.h"
#include "agent.h"

STRUCT ( xml_parser,
    mxml_node_t *root;
    mxml_node_t *node; );


error_code_t 
agent_xml_parse(agent_p agent);

error_code_t 
agent_xml_parse__mobile_agent
(
 agent_p agent, 
 xml_parser_p xml_parser
 );

error_code_t
agent_xml_parse__agent_data
(
 agent_p agent,
 xml_parser_p xml_parser
 );

error_code_t
agent_xml_parse__name(agent_p agent, xml_parser_p xml_parser);

error_code_t
agent_xml_parse__owner(agent_p agent, xml_parser_p xml_parser);

error_code_t
agent_xml_parse__home(agent_p agent, xml_parser_p xml_parser);

error_code_t
agent_xml_parse__sender(agent_p agent, xml_parser_p xml_parser);

error_code_t
agent_xml_parse__wg_code(agent_p agent, xml_parser_p xml_parser);

error_code_t
agent_xml_parse__tasks(agent_p agent, xml_parser_p xml_parser);

error_code_t
agent_xml_parse__task(agent_p agent, xml_parser_p xml_parser, int index);

error_code_t
agent_xml_parse__data(agent_p agent, xml_parser_p xml_parser, int index);

error_code_t
agent_xml_parse__file(agent_p agent, xml_parser_p xml_parser, int index);

error_code_t
agent_xml_parse__row(interpreter_variable_data_t* interp_variable, xml_parser_p xml_parser, int index);

void agent_xml_parse__fill_row_data(
        void *data,
        ChType_t type,
        int *extent,
        const mxml_node_t* node,
        int *index);


error_code_t
agent_xml_parse__agent_code(agent_p agent, int index, xml_parser_p xml_parser);

error_code_t
message_xml_parse(message_p message);

error_code_t
message_xml_parse__message(message_p message, xml_parser_p xml_parser);
#endif 
