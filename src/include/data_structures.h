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

#ifndef _AP_MODULES_H_
#define _AP_MODULES_H_
#include "ap_queue_template.h"
#include "../mc_list/list.h"
struct mc_platform_s;
/* Connection Queue */
#include "connection.h"
/* Fipa mailbox */
#include "fipa_acl.h"
/* Agent Mailbox */
#include "agent_mailbox.h"
AP_QUEUE_DECL_TEMPLATE(
    connection_queue, /* name */
    connection /* node_type */
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    connection_queue,
    Search,
    connection_t*,
    int
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    connection_queue,
    Remove,
    int,
    int
    )

int 
connection_queue_Print(connection_queue_p clist);

/* Message Queue */
#include"message.h"
AP_QUEUE_DECL_TEMPLATE(
    message_queue,
    message
    )

int 
message_queue_Print(message_queue_p queue);

/* Agent Queue */
struct agent_s;
AP_QUEUE_DECL_TEMPLATE(
    agent_queue,
    agent
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    agent_queue,
    Search,
    struct agent_s*,
    int
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    agent_queue,
    SearchName,
    struct agent_s*,
    char*
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    agent_queue,
    Remove,
    int,
    int
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    agent_queue,
    RemoveName,
    int,
    char*
    )

int
agent_queue_Print(agent_queue_p queue);
int
agent_queue_Flush(agent_queue_p queue);

/* Agent variables */
#include "interpreter_variable_data.h"
AP_QUEUE_DECL_TEMPLATE(
    agent_variable_list,
    interpreter_variable_data)

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    agent_variable_list,
    Search,
    interpreter_variable_data_t*,
    char* )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    agent_variable_list,
    Remove,
    int,
    char* )

/* Mail Queue */
AP_QUEUE_DECL_TEMPLATE(
    mail_queue,
    fipa_acl_message
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    mail_queue,
    SearchReceivers,
    fipa_acl_message_p,
    char*
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    mail_queue,
    RemoveName,
    int,
    char*
    )

/* Mailbox Queue */
AP_QUEUE_DECL_TEMPLATE(
    mailbox_queue,
    agent_mailbox
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    mailbox_queue,
    SearchName,
    agent_mailbox_p,
    char*
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    mailbox_queue,
    SearchID,
    agent_mailbox_p,
    int
    )

AP_QUEUE_GENERIC_DECL_TEMPLATE(
    mailbox_queue,
    RemoveID,
    int,
    int
    )

/* Interpereter Queue */
AP_QUEUE_DECL_TEMPLATE(
		interpreter_queue,
		AP_GENERIC
		)

AP_GENERIC_p AP_GENERIC_Copy(AP_GENERIC_p AP_GENERIC);
int AP_GENERIC_Destroy(AP_GENERIC_t* blah);

#endif
