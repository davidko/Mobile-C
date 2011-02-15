/* SVN FILE INFO
 * $Revision: 402 $ : Last Committed Revision
 * $Date: 2009-08-23 13:44:39 -0700 (Sun, 23 Aug 2009) $ : Last Committed Date */
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

#ifndef _XML_COMPOSE_H_
#define _XML_COMPOSE_H_
#include "agent.h"

mxml_node_t*
agent_xml_compose(agent_p agent);

mxml_node_t*
agent_xml_compose__gaf_message(agent_p agent);

mxml_node_t*
agent_xml_compose__message(agent_p agent);

mxml_node_t*
agent_xml_compose__mobile_agent(agent_p agent);

mxml_node_t*
agent_xml_compose__agent_data(agent_p agent);

mxml_node_t*
agent_xml_compose__name(agent_p agent);

mxml_node_t*
agent_xml_compose__owner(agent_p agent);

mxml_node_t*
agent_xml_compose__home(agent_p agent);

mxml_node_t*
agent_xml_compose__wg_code(agent_p agent);

mxml_node_t*
agent_xml_compose__tasks(agent_p agent);

mxml_node_t*
agent_xml_compose__task(agent_p agent, int index);

mxml_node_t*
agent_xml_compose__data(agent_p agent, int index, interpreter_variable_data_t* interp_variable);

mxml_node_t*
agent_xml_compose__file(agent_p agent, int index, agent_file_data_t* agent_file_data);

mxml_node_t*
agent_xml_compose__agent_code(agent_p agent, int index);

mxml_node_t*
agent_xml_compose__row(interpreter_variable_data_t* interp_variable, int index);

mxml_node_t* 
agent_xml_compose__create_row_nodes(
 void* data, 
 int index,
 int *extent,
 ChType_t type,
 int dim,
 int extent_index
 );

#endif
