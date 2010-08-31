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

#include <mxml.h>
#include "include/agent.h"
#include "include/xml_compose.h"
#include "include/xml_helper.h"

mxml_node_t*
agent_xml_compose(agent_p agent)
{
  mxml_node_t* node;
  node = mxmlLoadString
    (
     NULL,
     "<?xml version=\"1.0\"?>\n<!DOCTYPE myMessage SYSTEM \"mobilec.dtd\">",
     MXML_NO_CALLBACK
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     MXML_ADD_TO_PARENT,
     agent_xml_compose__gaf_message(agent)
    );
  return node;
}

  mxml_node_t*
agent_xml_compose__gaf_message(agent_p agent)
{
  mxml_node_t* node;
  node = mxmlNewElement
    (
     NULL,
     "MOBILEC_MESSAGE"
    );
  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     NULL,
     agent_xml_compose__message(agent)
    );
  return node;
}

  mxml_node_t*
agent_xml_compose__message(agent_p agent)
{
  mxml_node_t* node;
  node = mxmlNewElement
    (
     NULL,
     "MESSAGE"
    );

  if(
      agent->agent_type == MC_REMOTE_AGENT ||
      agent->agent_type == MC_LOCAL_AGENT 
    )
  {
    mxmlElementSetAttr
      (
       node,
       "message",
       "MOBILE_AGENT"
      );
  } else if 
    (
     agent->agent_type == MC_RETURN_AGENT
    )
    {
    mxmlElementSetAttr
      (
       node,
       "message",
       "RETURN_MSG"
      );
  }

  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     NULL,
     agent_xml_compose__mobile_agent(agent)
    );
  return node;
}

  mxml_node_t*
agent_xml_compose__mobile_agent(agent_p agent)
{
  mxml_node_t* node;

  node = mxmlNewElement
    (
     NULL,
     "MOBILE_AGENT"
    );

  mxmlAdd
    (
     node,
     MXML_ADD_AFTER,
     NULL,
     agent_xml_compose__agent_data(agent)
    );
  return node;
}

  mxml_node_t*
agent_xml_compose__agent_data(agent_p agent)
{
  mxml_node_t* node;
  mxml_node_t* tmp_node;

  node = mxmlNewElement
    (
     NULL,
     "AGENT_DATA"
    );

  /* Add the 'name' node */
  tmp_node = agent_xml_compose__name(agent);
  if (tmp_node == NULL) {
    return NULL;
  }
  mxmlAdd(
      node,
      MXML_ADD_AFTER,
      NULL,
      tmp_node
      );

  /* Add the 'owner' node */
  tmp_node = agent_xml_compose__owner(agent);
  if (tmp_node != NULL) {
    mxmlAdd(
        node,
        MXML_ADD_AFTER,
        NULL,
        tmp_node
        );
  }

  /* Add the 'home' node */
  tmp_node = agent_xml_compose__home(agent);
  if (tmp_node != NULL) {
    mxmlAdd(
        node,
        MXML_ADD_AFTER,
        NULL,
        tmp_node
        );
  }

  /* Add the 'wg_code' node */
  tmp_node = agent_xml_compose__wg_code(agent);
  if (tmp_node != NULL) {
    mxmlAdd(
        node,
        MXML_ADD_AFTER,
        NULL,
        tmp_node
        );
  }

  /* Add the 'task' node */
  tmp_node = agent_xml_compose__tasks(agent);
  if (tmp_node != NULL) {
    mxmlAdd(
        node,
        MXML_ADD_AFTER,
        NULL,
        tmp_node
        );
  }

  return node;
}

  mxml_node_t*
agent_xml_compose__name(agent_p agent)
{
  mxml_node_t* node;
  node = mxmlNewElement(
      NULL,
      "NAME"
      );
  mxmlNewText(
      node,
      0,
      agent->name
      );
  return node;
}

  mxml_node_t*
agent_xml_compose__owner(agent_p agent)
{
  mxml_node_t* node;
  node = mxmlNewElement(
      NULL,
      "OWNER"
      );
  mxmlNewText(
      node,
      0,
      agent->owner
      );
  return node;
}

  mxml_node_t*
agent_xml_compose__home(agent_p agent)
{
  mxml_node_t* node;
  node = mxmlNewElement(
      NULL,
      "HOME"
      );
  mxmlNewText(
      node,
      0,
      agent->home
      );
  return node;
}

  mxml_node_t*
agent_xml_compose__wg_code(agent_p agent)
{
  mxml_node_t* node;
  node = mxmlNewElement(
      NULL,
      "WG_CODE"
      );
  mxmlNewText(
      node,
      0,
      agent->wg_code
      );
  return node;
}

  mxml_node_t*
agent_xml_compose__tasks(agent_p agent)
{
  char buf[30];
  int i;
  mxml_node_t* node;
  mxml_node_t* tmp_node;

  node=mxmlNewElement(
      NULL,
      "TASKS" );

  sprintf(buf, "%d", agent->datastate->number_of_tasks);
  mxmlElementSetAttr(
      node,
      "task",
      buf
      );

  buf[0] = '\0';
  sprintf(buf, "%d", agent->datastate->task_progress);
  mxmlElementSetAttr(
      node,
      "num",
      buf
      );

  for (i = 0; i < agent->datastate->number_of_tasks; i++) {
    tmp_node = agent_xml_compose__task(agent, i);
    if (tmp_node != NULL) {
      mxmlAdd(
          node,
          MXML_ADD_AFTER,
          NULL,
          tmp_node
          );
    } else {
      return NULL;
    }
  }

  i=0;
  tmp_node = agent_xml_compose__agent_code(agent, i);
  while (tmp_node != NULL) {
    mxmlAdd(
        node,
        MXML_ADD_AFTER,
        NULL,
        tmp_node
        );
    i++;
    tmp_node = agent_xml_compose__agent_code(agent, i);
  }

  return node;
}

  mxml_node_t*
agent_xml_compose__task(agent_p agent, int index)
{
  char buf[30];
  mxml_node_t* node;
  mxml_node_t* tmp_node;
  interpreter_variable_data_t* tmp_interp_var;
  node = mxmlNewElement(
      NULL,
      "TASK"
      );

  buf[0] = '\0';
  sprintf(buf, "%d", index);
  mxmlElementSetAttr(
      node,
      "num",
      buf );

  /* Set up server name attribute */
  mxmlElementSetAttr(
      node,
      "server",
      agent->datastate->tasks[index]->server_name
      );

  /* Set up the code id */
  if (agent->datastate->tasks[index]->code_id) {
    mxmlElementSetAttr(
        node,
        "code_id",
        agent->datastate->tasks[index]->code_id
        );
  }

  /* Set up persistent flag, only if it _is_ persistent */
  if (agent->datastate->persistent || agent->datastate->tasks[index]->persistent) {
    mxmlElementSetAttr(
        node,
        "persistent",
        "1"
        );
  }

  /* Add 'DATA' nodes */
  /* First, add the return data */
  if(agent->datastate->tasks[index]->var_name != NULL) {
    mxmlElementSetAttr(
        node,
        "return",
        agent->datastate->tasks[index]->var_name );

    if(
        strcmp( 
          "no-return",
          agent->datastate->tasks[index]->var_name
          )
      )
    {

      if (agent->datastate->tasks[index]->agent_return_data != NULL) {
        tmp_node = agent_xml_compose__data(
            agent, 
            index, 
            agent->datastate->tasks[index]->agent_return_data);
      } else {tmp_node = NULL;}

      if(tmp_node != NULL) {
        mxmlAdd(
            node,
            MXML_ADD_AFTER,
            NULL,
            tmp_node );
      }
    }
  }

  /* Now, add all the variables that the agent wants to save */
  while
    (
     ( 
      tmp_interp_var = agent_variable_list_Pop(
        agent->datastate->tasks[index]->agent_variable_list )
     ) != NULL 
    )
  {
    tmp_node = agent_xml_compose__data(
        agent,
        index,
        tmp_interp_var);
    free(tmp_interp_var);
    if(tmp_node == NULL) {
      fprintf(stderr, "Compose error. %s:%d\n", __FILE__, __LINE__);
      return NULL;
    }
    mxmlAdd(
        node,
        MXML_ADD_AFTER,
        NULL,
        tmp_node );
  }

  return node;
}

  mxml_node_t*
agent_xml_compose__data(agent_p agent, int index, interpreter_variable_data_t* interp_variable)
{
  char buf[30];
  mxml_node_t* node;
  mxml_node_t* tmp_node;

  if (interp_variable == NULL) { return NULL; }

  node = mxmlNewElement(
      NULL,
      "DATA"
      );
  /* Set up return variable name attribute */
  mxmlElementSetAttr(
      node,
      "name",
      interp_variable->name
      );

  if (interp_variable != NULL)
  {
    /* Set up the 'dim' attribute */
    sprintf(
        buf, 
        "%d", 
        interp_variable->array_dim
        );
    mxmlElementSetAttr(
        node,
        "dim",
        buf
        );

    /* set up the 'type' attribute */
    CH_DATATYPE_STRING(
        interp_variable->data_type,
        buf
        );
    mxmlElementSetAttr(
        node,
        "type",
        buf
        );

    if (interp_variable->array_dim == 0)
    {
      CH_DATATYPE_VALUE_STRING
        (
         interp_variable->data_type,
         buf,
         interp_variable->data
        );
      mxmlElementSetAttr(
          node,
          "value",
          buf
          );
    } else {
      /* Set up row nodes */
      tmp_node = agent_xml_compose__row(interp_variable, index);
      if (tmp_node != NULL) {
        mxmlAdd(
            node,
            MXML_ADD_AFTER,
            NULL,
            tmp_node
            );
      }
    }
  }
  return node;
}

  mxml_node_t*
agent_xml_compose__agent_code(agent_p agent, int index)
{
  mxml_node_t* node;
  if (agent->datastate->agent_codes[index] == NULL) {
    return NULL;
  }

  node = mxmlNewElement (
      MXML_NO_PARENT,
      "AGENT_CODE"
      );

  xml_new_cdata(
      node,
      agent->datastate->agent_codes[index]
      );
  if (strlen(agent->datastate->agent_code_ids[index]) > 0) {
    mxmlElementSetAttr
      (
       node,
       "id",
       agent->datastate->agent_code_ids[index]
      );
  }
  return node;
}

  mxml_node_t*
agent_xml_compose__row(interpreter_variable_data_t* interp_variable, int index)
{
  mxml_node_t* node;

  if (interp_variable == NULL) {
    return NULL;
  }

  node = agent_xml_compose__create_row_nodes
    (
     interp_variable->data,
     0,
     interp_variable->array_extent,
     interp_variable->data_type,
     interp_variable->array_dim,
     0
    );
  return node;
}

/* This recursive function actually creates the tree of row nodes. */
mxml_node_t* 
agent_xml_compose__create_row_nodes
(
 void* data, 
 int index,
 int *extent,
 ChType_t type,
 int dim,
 int extent_index
 )
{
  mxml_node_t* node;
  char *buf;
  char *varstring;
  int size;
  int i;
  if (dim == 1) { 
    buf = (char*)malloc(sizeof(char) * 20);
    CH_DATATYPE_SIZE(type, size);

    varstring = (char*)malloc(
        (sizeof(char)*64) * *extent);
    varstring[0] = '\0';
    for(i = 0; i < *extent; i++) {
      buf[0] = '\0';
#ifndef _WIN32
      CH_DATATYPE_VALUE_STRING(type, buf, (data+ size*(index+i)));
#else
      CH_DATATYPE_VALUE_STRING(type, buf, ((char*)data+ size*(index+i)));
#endif
      strcat(varstring, buf);
      strcat(varstring, ",");
    }
    node = mxmlNewElement(
        MXML_NO_PARENT,
        "ROW" );
    buf[0] = '\0';
    sprintf(buf, "%d", extent_index);
    mxmlElementSetAttr(
        node,
        "index",
        buf );


    mxmlNewText(
        node,
        1,
        varstring );
    free(buf);
    free(varstring);
    return node;
  } else if (dim < 0) {
    fprintf(stderr, "INTERNAL ERROR: %s:%d\n",
        __FILE__, __LINE__);
    return NULL;
  } else if (dim == 0) {
    return NULL;
  } else {
    /* Create my node, but need to attach node of lower dimensions */
    size = 1;
    for (i = 1; i < dim; i++) {
      size *= extent[i];  /* size is the size of a dimension */
    }
    node = mxmlNewElement(MXML_NO_PARENT, "ROW");
    buf = (char*)malloc(sizeof(char)*10);
    sprintf(buf, "%d", extent_index);
    mxmlElementSetAttr(
        node,
        "index",
        buf );
    for (i = 0; i < *extent; i++) {
      mxmlAdd( 
          node,
          MXML_ADD_AFTER,
          MXML_ADD_TO_PARENT,
          agent_xml_compose__create_row_nodes(
            data,
            index + (size*i),
            extent+1,
            type,
            dim-1,
            i
            )
          );
    }
    free (buf);
    return node;
  }
}

