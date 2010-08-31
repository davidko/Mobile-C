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
#ifndef _WIN32
#include "config.h"
#else
#include "winconfig.h"
#endif

#include "include/ap_queue_template.h"
#include "include/data_structures.h"
/* Connection Queue */
AP_QUEUE_STD_DEFN_TEMPLATE(connection_queue, connection)

AP_QUEUE_SEARCH_TEMPLATE(
    connection_queue,
    Search,
    connection,
    int,
    (node->connect_id == key)
    )

AP_QUEUE_REMOVE_TEMPLATE(
    connection_queue,
    Remove,
    connection,
    int,
    (node->connect_id == key)
    )

int 
connection_queue_Print(connection_queue_p queue)
{
  int i;
  connection_p node;
  MUTEX_LOCK(queue->lock);
  for(i = 0; i < queue->size; i++) {
    node = (connection_p)ListSearch(queue->list, i);
    if (node != NULL) {
      printf("Connection id %d:\n\tremote:%s\n",
          node->connect_id,
          node->remote_hostname
          );
    }
  }
  MUTEX_UNLOCK(queue->lock);
  return i;
}

/* Agent variables */
#include "include/interpreter_variable_data.h"
AP_QUEUE_STD_DEFN_TEMPLATE(agent_variable_list, interpreter_variable_data)

AP_QUEUE_SEARCH_TEMPLATE(
    agent_variable_list,
    Search,
    interpreter_variable_data,
    char*,
    (!strcmp(node->name, key))
    )

AP_QUEUE_REMOVE_TEMPLATE(
    agent_variable_list,
    Remove,
    interpreter_variable_data,
    char*,
    (!strcmp(node->name, key))
    )
        
/* Message Queue */
AP_QUEUE_STD_DEFN_TEMPLATE(
    message_queue,
    message
    )

AP_QUEUE_SEARCH_TEMPLATE(
    message_queue,
    Search,
    message,
    int,
    (node->message_id == key)
    )

AP_QUEUE_REMOVE_TEMPLATE(
    message_queue,
    Remove,
    message,
    int,
    (node->message_id == key)
    )

int
message_queue_Print(message_queue_p queue)
{
  int i;
  message_p node;
  MUTEX_LOCK(queue->lock);
  for(i = 0; i < queue->size; i++) {
    node = (message_p)ListSearch(queue->list, i);
    if (node != NULL) {
      printf("Connection id %d:\n\tfrom:%s\n\tto:%s\n",
          node->message_id,
          node->from_address,
          node->to_address
          );
    }
  }
  MUTEX_UNLOCK(queue->lock);
  return i;
}


/* Agent Queue */
#include "include/agent.h"
AP_QUEUE_STD_DEFN_TEMPLATE(
    agent_queue,
    agent
    )
AP_QUEUE_SEARCH_TEMPLATE(
    agent_queue,
    Search,
    agent,
    int,
    (node->id == key)
    )
AP_QUEUE_SEARCH_TEMPLATE(
    agent_queue,
    SearchName,
    agent,
    char*,
    (!strcmp(node->name, key))
    )
AP_QUEUE_REMOVE_TEMPLATE(
    agent_queue,
    Remove,
    agent,
    int,
    (node->id == key)
    )
AP_QUEUE_REMOVE_TEMPLATE(
    agent_queue,
    RemoveName,
    agent,
    char*,
    (!strcmp(node->name, key))
    )

int
agent_queue_Print(agent_queue_p queue)
{
  int i;
  agent_p node;
  MUTEX_LOCK(queue->lock);
  for(i = 0; i < queue->size; i++) {
    node = (agent_p)ListSearch(queue->list, i);
    if (node != NULL) {
      printf("agent id %d:\n\tname:%s  status:%d\n",
          (int)node->id,
          node->name, node->agent_status);
    }
  }
  MUTEX_UNLOCK(queue->lock);
  return i;
}

int
agent_queue_Flush(agent_queue_p queue)
{
  int i;
  agent_p node;
  MUTEX_LOCK(queue->lock);
  for(i = 0; i < queue->size; i++) {
    node = (agent_p)ListSearch(queue->list, i);
    if (node != NULL) {
      printf("flushing agent id %d: name:%s status:%d\n",
          (int)node->id, node->name, node->agent_status);
      agent_queue_RemoveIndex((agent_queue_p)queue->list, i);
    }
  }
  MUTEX_UNLOCK(queue->lock);
  return i;
}
/* Mail Queue */
AP_QUEUE_STD_DEFN_TEMPLATE(
    mail_queue,
    fipa_acl_message
    )

fipa_acl_message_p mail_queue_SearchReceivers( mail_queue_p mail_queue, const char* key)
{
  listNode_t* parsenode;
  fipa_acl_message_t* node = NULL;
  fipa_acl_message_t* ret = NULL;
  int i;

  MUTEX_LOCK(mail_queue->lock);
  if (mail_queue->list->listhead == NULL) {
    MUTEX_UNLOCK(mail_queue->lock);
    return NULL;
  }
  for(
      parsenode = (listNode_t*)mail_queue->list->listhead;
      parsenode != NULL;
      parsenode = (listNode_t*)parsenode->next
     )
  {
    node = (fipa_acl_message_t*)parsenode->node_data;
    for(i = 0; i < node->receiver->num; i++) {
      if ( !strcmp(
            node->receiver->fipa_agent_identifiers[i]->name,
            key) )
      {
        ret = node;
        parsenode = NULL;
        break;
      }
    }
  }
  return ret;
}

/* Mailbox Queue */
AP_QUEUE_STD_DEFN_TEMPLATE(
    mailbox_queue,
    agent_mailbox
    )

/* Interpreter Queue */

AP_QUEUE_STD_DEFN_TEMPLATE(
		interpreter_queue,
		AP_GENERIC
		)

AP_GENERIC_p interpreter_queue_CreateRetrieve( struct interpreter_queue_s *queue , ChOptions_t* interp_options) {
	/* This function retrieves an interpreter from the list if there are any
		on the queue. Otherwise, it creates a new one and returns it. */
	struct AP_GENERIC_s *ret;
	ChInterp_t* interp;
	MUTEX_LOCK(queue->lock);
	if (queue->size <= 0) {
		interp = (ChInterp_t*)malloc(sizeof(ChInterp_t));
		if( Ch_Initialize(interp, interp_options) != CH_OK) {
      /* Could not initialize another interpreter */
      MUTEX_UNLOCK(queue->lock);
      return NULL;
    }
    /* Initialize the agent intepreter variables */
    agent_ChScriptInitVar(interp);
		ret = (struct AP_GENERIC_s*)interp;
		MUTEX_UNLOCK(queue->lock);
		return ret;
	}
	ret = (struct AP_GENERIC_s*)ListPop(queue->list);
	queue->size--;
	COND_SIGNAL(queue->cond);
	MUTEX_UNLOCK(queue->lock);
	return ret;
}

int AP_GENERIC_Destroy(AP_GENERIC_t* blah) {
	return 0;
}

AP_GENERIC_p AP_GENERIC_Copy(AP_GENERIC_p AP_GENERIC) {
	return NULL;
}
