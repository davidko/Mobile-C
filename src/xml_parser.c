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

#include <mxml.h>
#include <string.h>
#include <stdlib.h>
#define _XOPEN_SOURCE 600
#include <stdlib.h>
#ifndef _WIN32
#include "config.h"
#else
#include "winconfig.h"
#endif
#include "include/interpreter_variable_data.h"
#include "include/message.h"
#include "include/xml_parser.h"
#include "include/xml_helper.h"

/* *** */
/* agent_xml_parse */
error_code_t agent_xml_parse(agent_p agent)
{
  xml_parser_t xml_parser;
  xml_parser.root = agent->datastate->xml_agent_root;
  xml_parser.node = xml_parser.root;
  agent_xml_parse__mobile_agent(agent, &xml_parser);
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__gaf_message */
error_code_t 
agent_xml_parse__mobile_agent
(
 agent_p agent, 
 xml_parser_p xml_parser
 )
{
  /* make sure this is the 'MOBILE_AGENT' tag */
  if ( 
      strcmp(
        (const char*)xml_get_element_name(xml_parser->node),
        "MOBILE_AGENT"
        )
     )
  {
    return MC_ERR_PARSE;
  }
  /* There is only one child node: AGENT_DATA*/
  xml_parser->node = xml_get_child(
      xml_parser->node,
      "AGENT_DATA",
      1);

  return agent_xml_parse__agent_data(agent, xml_parser);
}

/* *** */
/* agent_xml_parse__message */
error_code_t
agent_xml_parse__agent_data
(
 agent_p agent,
 xml_parser_p xml_parser
 )
{
  const mxml_node_t* agent_data_node;
  error_code_t err_code;
  
  if (xml_parser->node == NULL) {
    return MC_ERR_PARSE;
  }

  agent_data_node = xml_parser->node;

  xml_parser->node = xml_get_child(
      agent_data_node,
      "NAME", 
      1
      );
  if ( (err_code = agent_xml_parse__name(agent, xml_parser)) ) {
    return err_code;
  }

  xml_parser->node = xml_get_child(
      agent_data_node,
      "OWNER",
      1
      );
  if ( (err_code = agent_xml_parse__owner(agent, xml_parser)) ) {
    return err_code;
  }

  xml_parser->node = xml_get_child(
      agent_data_node,
      "HOME",
      1
      );
  if ( (err_code = agent_xml_parse__home(agent, xml_parser)) ) {
    return err_code;
  }

  xml_parser->node = xml_get_child(
      agent_data_node,
      "SENDER",
      1
      );
  if( (err_code = agent_xml_parse__sender(agent, xml_parser)) ) {
    return err_code;
  }

  xml_parser->node = xml_get_child(
      agent_data_node,
      "WG_CODE",
      1
      );
  if ( (err_code = agent_xml_parse__wg_code(agent, xml_parser)) ) {
    return err_code;
  }

  xml_parser->node = xml_get_child(
      agent_data_node,
      "TASKS",
      1
      );
  if ( (err_code = agent_xml_parse__tasks(agent, xml_parser)) ) {
    return err_code;
  }
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__name */
error_code_t
agent_xml_parse__name(agent_p agent, xml_parser_p xml_parser)
{
  char* text;
  const mxml_node_t* name_node;
  if (xml_parser->node == NULL) {
    return MC_ERR_PARSE;
  }
  name_node = xml_parser->node;

  text = xml_get_text( name_node );
  CHECK_NULL(text, return MC_ERR_PARSE;);

  agent->name = (char*)malloc(
      sizeof(char)*(strlen(text)+1)
      );
  strcpy(
      agent->name,
      text
      );
  free(text);
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__owner */
error_code_t
agent_xml_parse__owner(agent_p agent, xml_parser_p xml_parser)
{
  char *text;
  const mxml_node_t* owner_node;
  if (xml_parser->node == NULL) {
    /* It's ok if there is no owner node: It is not a required field. */
    agent->owner = NULL;
    return MC_SUCCESS;
  }
  owner_node = xml_parser->node;

  text = xml_get_text( owner_node );
  CHECK_NULL(text, agent->owner=NULL;return MC_SUCCESS;);
  agent->owner = (char*)malloc(
      sizeof(char)*(strlen(text)+1)
      );
  strcpy(
      agent->owner,
      text
      );
  free(text);
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__home */
error_code_t
agent_xml_parse__home(agent_p agent, xml_parser_p xml_parser)
{
  char *text;
  const mxml_node_t* home_node;
  if (xml_parser->node == NULL) {
    /* It's ok if there is no home node: It is not a required field. */
    agent->home= NULL;
    return MC_SUCCESS;
  }
  home_node = xml_parser->node;
  text = xml_get_text( home_node );
  CHECK_NULL(text, agent->home=NULL;return MC_SUCCESS;);
  agent->home = (char*)malloc(
      sizeof(char)*(strlen(text)+1)
      );
  strcpy(
      agent->home,
      text
      );
  free(text);
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__sender */
error_code_t
agent_xml_parse__sender(agent_p agent, xml_parser_p xml_parser)
{
  char *text;
  const mxml_node_t* sender_node;
  if (xml_parser->node == NULL) {
    /* This is fine: sender field is optional for now */
    agent->sender = NULL;
    return MC_SUCCESS;
  }
  sender_node = xml_parser->node;
  text = xml_get_text( sender_node );
  /* If there is no text, just return... */
  CHECK_NULL(text, agent->sender=NULL;return MC_SUCCESS; );

  agent->sender = (char*)malloc(
      sizeof(char)*(strlen(text)+1)
      );
  strcpy(
      agent->sender,
      text
      );
  free(text);
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__wg_code */
error_code_t
agent_xml_parse__wg_code(agent_p agent, xml_parser_p xml_parser)
{
  char *text;
  const mxml_node_t* wg_code_node;
  if (xml_parser->node == NULL) {
    /* This is fine: wg_code field is optional */
    agent->wg_code = NULL;
    return MC_SUCCESS;
  }
  wg_code_node = xml_parser->node;
  text = xml_get_text( wg_code_node );
  /* If there is no text, just return... */
	if (text == NULL) {
		agent->wg_code=NULL;
		return MC_SUCCESS;
	}

  agent->wg_code = (char*)malloc(
      sizeof(char)*(strlen(text)+1)
      );
  strcpy(
      agent->wg_code,
      text
      );
  free(text);
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__tasks */
error_code_t
agent_xml_parse__tasks(agent_p agent, xml_parser_p xml_parser)
{
  int i;
  int code_num=0;
  int err_code;
  const char* attribute;
  mxml_node_t* tasks_node;
  char buf[20];

  tasks_node = xml_parser->node;

  /* parse the 'task' attribute */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)tasks_node,
      "task"
      );
  if (attribute == NULL) {
    agent->datastate->number_of_tasks = 1;
  } else {
    agent->datastate->number_of_tasks = atoi((char*)attribute);
  }
  agent->datastate->tasks = (agent_task_p*)malloc(
      sizeof(agent_task_p) * agent->datastate->number_of_tasks
      );

  /* parse the 'num' attribute */
  attribute = mxmlElementGetAttr(
      tasks_node,
      "num"
      );
  if (attribute == NULL) {
    agent->datastate->task_progress = 0;
  } else {
    agent->datastate->task_progress = atoi((char*)attribute);
  }

  /* Allocate each task */
  for(i = 0; i<agent->datastate->number_of_tasks; i++) {
    agent->datastate->tasks[i] = agent_task_New();
  }

  /* Parse each task */
  for(i = 0; i < agent->datastate->number_of_tasks; i++) {
    sprintf(buf, "%d", i);
    xml_parser->node = mxmlFindElement(
        tasks_node,
        tasks_node,
        "TASK",
        "num",
        buf,
        MXML_DESCEND_FIRST );
    if(xml_parser->node == NULL) {
      fprintf(stderr,
          "ERROR: Could not find task num %d! %s:%d\n",
          i, __FILE__, __LINE__);
      return MC_ERR_PARSE;
    }
    agent_xml_parse__task(agent, xml_parser, i);
  }

  /* Need to get all of the agent codes. Even though we may execute only
   * one right now, in the future, the agent may decide at runtime which block
   * to execute, so we need them all. */
  xml_parser->node = mxmlFindElement
      (
       tasks_node,
       tasks_node,
       "AGENT_CODE",
       NULL,
       NULL,
       MXML_DESCEND
      );


  /* First we count the number of code blocks */
  while(xml_parser->node != NULL) {
    code_num++;
    xml_parser->node = mxmlFindElement
      (
       xml_parser->node,
       tasks_node,
       "AGENT_CODE",
       NULL,
       NULL,
       MXML_NO_DESCEND
      );
  }

  /* Allocate correct amount of memory for code blocks. */
  agent->datastate->agent_code_ids = (char**)malloc
    (
     (code_num+1) * sizeof(char*)
    );
  agent->datastate->agent_codes = (char**)malloc
    (
     (code_num+1) * sizeof(char*)
    );
  /* Set sigil */
  agent->datastate->agent_code_ids[code_num] = NULL;
  agent->datastate->agent_codes[code_num] = NULL;

  /* Go through all code again and parse */
  xml_parser->node = mxmlFindElement
      (
       tasks_node,
       tasks_node,
       "AGENT_CODE",
       NULL,
       NULL,
       MXML_DESCEND
      );
  i = 0;
  while(xml_parser->node != NULL) {
    err_code = agent_xml_parse__agent_code(agent, i, xml_parser);
    i++;
    xml_parser->node = mxmlFindElement
      (
       xml_parser->node,
       tasks_node,
       "AGENT_CODE",
       NULL,
       NULL,
       MXML_NO_DESCEND
      );
  }

  if (agent->datastate->agent_code == NULL) {
    /* Something is wrong. */
    fprintf(stderr, "Parse error: Agent code not found. %s:%d\n", __FILE__, __LINE__);
    return MC_ERR_PARSE;
  }

  return (error_code_t)0;
}

/* *** */
/* agent_xml_parse__task */
error_code_t
agent_xml_parse__task(agent_p agent, xml_parser_p xml_parser, int index)
{
  const char* attribute;
  mxml_node_t* task_node;
  error_code_t err_code = MC_SUCCESS;
  CHECK_NULL(xml_parser->node, return MC_ERR_PARSE;);
  task_node = xml_parser->node;

  /* Parse the multiple DATA nodes */
  xml_parser->node = mxmlFindElement(
      task_node,
      task_node,
      "DATA",
      NULL,
      NULL,
      MXML_DESCEND_FIRST);
  while(xml_parser->node != NULL) {
    /* There may be no DATA nodes */
    if ((err_code = agent_xml_parse__data(agent, xml_parser, index)))
    {
      return err_code;
    }
    xml_parser->node = mxmlFindElement(
        xml_parser->node,
        task_node,
        "DATA",
        NULL,
        NULL,
        MXML_NO_DESCEND );
  }

  /* Parse the multiple FILE nodes */
  xml_parser->node = mxmlFindElement(
      task_node,
      task_node,
      "FILE",
      NULL,
      NULL,
      MXML_DESCEND_FIRST);
  while(xml_parser->node != NULL) {
    if ((err_code = agent_xml_parse__file(agent, xml_parser, index)))
    {
      return err_code;
    }
    xml_parser->node = mxmlFindElement(
        xml_parser->node,
        task_node,
        "FILE",
        NULL,
        NULL,
        MXML_NO_DESCEND );
  }

  /* 'persistent' */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)task_node,
      "persistent"
      );
  if (attribute != NULL) {
    agent->datastate->tasks[index]->persistent = 
      atoi((char*)attribute);
  } else {
    agent->datastate->tasks[index]->persistent = 0;
  }


  /* 'code_id' */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)task_node,
      "code_id");
  if (attribute != NULL) {
    agent->datastate->tasks[index]->code_id = (char*)malloc
      (
       sizeof(char) * 
       (strlen(attribute) + 1)
      );
    strcpy(agent->datastate->tasks[index]->code_id, attribute);
  } else {
    agent->datastate->tasks[index]->code_id = NULL;
  }

  /* 'num' - The number of this task (indexed from 0) */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)task_node,
      "num"
      );
  CHECK_NULL(attribute, return MC_ERR_PARSE;);

  /* 'server' - The server this task should be performed on */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)task_node,
      "server"
      );
  CHECK_NULL(attribute, return MC_ERR_PARSE;);
  agent->datastate->tasks[index]->server_name = 
    (char*)malloc(sizeof(char) * (strlen(attribute)+1) );
  strcpy(
      agent->datastate->tasks[index]->server_name,
      attribute
      );

  /* 'return' - The name of the return variable, if there is one */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)task_node,
      "return" );
  if (attribute == NULL) {
    agent->datastate->tasks[index]->var_name = strdup("no-return");
  } else {
    agent->datastate->tasks[index]->var_name = strdup(attribute);
  }
  CHECK_NULL(agent->datastate->tasks[index]->var_name, exit(1););

  return err_code;
}

/* *** */
/* agent_xml_parse__data */
error_code_t
agent_xml_parse__data(agent_p agent, xml_parser_p xml_parser, int index)
{
  const char* attribute;
  const char* attribute2;
  mxml_node_t *data_node;
  int data_type_size;
  interpreter_variable_data_t* interp_variable;
  if (xml_parser->node == NULL) {
    return MC_ERR_PARSE;
  }
  if (strcmp(
        "DATA",
        xml_get_element_name(xml_parser->node) )
     )
  {
    return MC_ERR_PARSE;
  }
  data_node = xml_parser->node;

  /* Check to see if this is the return variable */
  attribute = mxmlElementGetAttr(
      data_node->parent,
      "return" );
  attribute2 = mxmlElementGetAttr(
      data_node,
      "name" );
  if (attribute != NULL && !strcmp(attribute, attribute2)) {
    /* This variable is the return variable. */

    /* Allocate Return data structure */
    /* FIXME: This may not be the right place to do this,
     * but it is safe and leak free. */
    agent->datastate->tasks[index]->agent_return_data = 
      interpreter_variable_data_New(); 
    interp_variable = agent->datastate->tasks[index]->agent_return_data;
  } else {
    interp_variable = interpreter_variable_data_New();
    agent_variable_list_Add(
        agent->datastate->tasks[index]->agent_variable_list,
        interp_variable );
  }


  /* Attributes we need to parse:
   * O dim          - dimension of the return data
   * O name         - name of the return variable
   * X: O persistent   - is the agent persistent for this task?
   *    note: persistent tag is now duplicated in the 'TASK' node. Usage in DATA is deprecated
   * O type         - return variable type
   * O return_value - return value, if not an array
   * O code_id      - ID of the code block to execute. If this
   *                  attribute is missing, execute the first 
   *                  code block available.
   *
   * 'O' denotes optional attribute. */


  /* 'dim' */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)xml_parser->node,
      "dim"
      );
  if (attribute != NULL) {
    interp_variable->array_dim = 
      atoi((char*)attribute);
  } else {
    interp_variable->array_dim = 
      0;
  }

  /* 'name' */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)xml_parser->node,
      "name"
      );
  if (attribute != NULL) {
    interp_variable->name = 
      (char*)malloc(sizeof(char)*(strlen(attribute)+1));
    strcpy(
        interp_variable->name,
        attribute
        );
  }

  /* 'persistent' */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)data_node,
      "persistent"
      );
  if (attribute != NULL) {
    agent->datastate->tasks[index]->persistent = 
      atoi((char*)attribute);
  } else {
    agent->datastate->tasks[index]->persistent = 0;
  }

  /* 'type' */
  attribute = mxmlElementGetAttr(
      (mxml_node_t*)data_node,
      "type"
      );
  if (attribute != NULL) {
    CH_STRING_DATATYPE(
        attribute,
        interp_variable->data_type
        );
    CH_DATATYPE_SIZE(
        interp_variable->data_type,
        data_type_size
        );
  } else {
    interp_variable->data_type = 
      CH_UNDEFINETYPE;
    data_type_size = 0;
  }

  if (interp_variable->array_dim == 0) {
  /* 'return_value' */
    attribute = mxmlElementGetAttr(
        (mxml_node_t*)data_node,
        "value" );
    if (attribute != NULL && data_type_size != 0) {
      interp_variable->data =
        malloc(data_type_size);
      CH_DATATYPE_STR_TO_VAL(
          interp_variable->data_type,
          attribute,
          interp_variable->data
          );
    }
  } else {
    /* The only possible child node to parse are row nodes. */
    xml_parser->node = xml_get_child(
        xml_parser->node,
        "ROW",
        1
        );
    agent_xml_parse__row(interp_variable, xml_parser, index);
  }
  xml_parser->node = data_node;
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__file */
error_code_t
agent_xml_parse__file(agent_p agent, xml_parser_p xml_parser, int index)
{
  char* text;
  const char* name;
  const mxml_node_t* file_node;
  agent_file_data_p afd;
  if(xml_parser->node == NULL) {
    return MC_ERR_PARSE;
  }
  file_node = xml_parser->node;
  text = xml_get_text(file_node);
  CHECK_NULL(text, return MC_ERR_PARSE;);

  /* Get the name attribute */
  name = mxmlElementGetAttr(
    (mxml_node_t*)file_node,
    "name"
    );
  if (name == NULL) {
    free(text);
    return MC_ERR_PARSE;
  }
  afd = agent_file_data_NewWithData(name, text);
  free(text);
  agent_file_list_Add(
      agent->datastate->tasks[index]->agent_file_list,
      afd);
  return MC_SUCCESS;
}

/* *** */
/* agent_xml_parse__row */
error_code_t
agent_xml_parse__row(interpreter_variable_data_t* interp_variable, xml_parser_p xml_parser, int index)
{
  int j;
  int data_type_size;
  int tmp;
  int num_elements;
  const mxml_node_t* row_node;

  if (xml_parser->node == NULL) {
    return MC_SUCCESS;
  }

  if (strcmp(
        xml_get_element_name(xml_parser->node),
        "ROW" )
     )
  {
    return MC_SUCCESS;
  }
  row_node = xml_parser->node;

  /* malloc mem for the task data elements */
  /* First, find the extents of the dimensions */
  if (interp_variable->array_dim != 0) {
    interp_variable->array_extent = (int*)
      malloc
      (
       sizeof(int) * 
       interp_variable->array_dim
      );
    tmp = 0;
    agent_xml_parse__fill_row_data(NULL,
        interp_variable->data_type,
        interp_variable->array_extent,
        row_node,
        &tmp);
    num_elements = 1;
    for (j = 0; j<interp_variable->array_dim; j++) {
      num_elements *= interp_variable->array_extent[j];
    }

    /* Allocate space for the return data */
    CH_DATATYPE_SIZE
      (
       interp_variable->data_type,
       data_type_size
      );
    if (interp_variable->data_type == CH_CHARTYPE) {
      num_elements++;
    }
    interp_variable->data =
      malloc(num_elements * data_type_size);

    /* Get the data */
    tmp = 0;
    agent_xml_parse__fill_row_data(
        interp_variable->data,
        interp_variable->data_type,
        interp_variable->array_extent,
        row_node,
        &tmp );
  } else { 
    return MC_SUCCESS;
  }
  return MC_SUCCESS;
}

void agent_xml_parse__fill_row_data(
        void *data,
        ChType_t type,
        int *extent,
        const mxml_node_t* node,
        int *index) 
{
  mxml_node_t* tmp_node;
  int i=0;
  char *buf;
  char *tmp;
#ifndef _WIN32
  char *saveptr;
#endif
  int datasize;
  /* Check to see if the child is an element or text. All children must be
   * either an element or text. If it is text, that means we are at the very bottom 
   * and we need to retrive data. */
  (*extent) = 0; 
  if (node->child->type == MXML_TEXT) {
    node = node->child;
    /* Now we parse the data */
    CH_DATATYPE_SIZE(type, datasize);
    buf = (char*)malloc(
        sizeof(char) +
        (strlen(node->value.text.string) + 1));
    strcpy(buf, node->value.text.string);
    /* Tokenize by commas */
#ifndef _WIN32
    tmp = strtok_r(buf, ",", &saveptr);
#else
    tmp = strtok(buf, ",");
#endif
    while ( tmp != NULL) {
      switch(type) {
        case CH_CHARTYPE:
          if (data != NULL) {
            ((char*)data)[*index] = *(char*)tmp;
            ((char*)data)[*index+1] = '\0';
          }
          (*index)++;
          break;
        case CH_INTTYPE:
          if (data != NULL)
            ((int*)data)[*index] = strtol(tmp, NULL, 0);
          (*index)++;
          break;
        case CH_UINTTYPE:
          if (data != NULL)
            ((unsigned int*)data)[*index] = strtoul(tmp, NULL, 0);
          (*index)++;
          break;
        case CH_SHORTTYPE:
          if (data != NULL)
            ((short*)data)[*index] = (short)strtol(tmp, NULL, 0);
          (*index)++;
          break;
        case CH_USHORTTYPE:
          if (data != NULL)
            ((unsigned short*)data)[*index] = (unsigned short)strtol(tmp, NULL, 0);
          (*index)++;
          break;
        case CH_FLOATTYPE:
          if (data != NULL)
#ifndef _WIN32
            ((float*)data)[*index] = strtof(tmp, NULL);
#else	
          ((float*)data)[*index] = (float)strtod(tmp, NULL);
#endif
          (*index)++;
          break;
        case CH_DOUBLETYPE:
          if (data != NULL)
            ((double*)data)[*index] = strtod(tmp, NULL);
          (*index)++;
          break;
        default:
          fprintf(stderr, 
              "Unsupported data type: %d %s:%d\n",
              type, __FILE__, __LINE__);
      }
#ifndef _WIN32
      tmp = strtok_r(NULL, ",", &saveptr);
#else
      tmp = strtok(NULL, ",");
#endif
      (*extent)++;
    }
    free(buf);
  } else if (node->type == MXML_ELEMENT) {
    buf = (char*)malloc(sizeof(char)*10);
    buf[0] = '\0';
    sprintf(buf, "%d", i);
    tmp_node = mxmlFindElement(
        (mxml_node_t*)node,
        (mxml_node_t*)node,
        "ROW",
        "index",
        buf,
        MXML_DESCEND_FIRST);
    while (tmp_node != NULL) {
      (*extent)++;
      agent_xml_parse__fill_row_data(data, type,(extent+1), tmp_node, index);
      i++;
      buf[0] = '\0';
      sprintf(buf, "%d", i);
      tmp_node = mxmlFindElement(
          (mxml_node_t*)node,
          (mxml_node_t*)node,
          "ROW",
          "index",
          buf,
          MXML_DESCEND_FIRST);
    }
    free(buf);
  }
}

/* *** */
/* agent_xml_parse__agent_code */
error_code_t
agent_xml_parse__agent_code(agent_p agent, int index, xml_parser_p xml_parser)
{
  const char *attribute;
  int cur_task = agent->datastate->task_progress;
  if( cur_task == agent->datastate->number_of_tasks )
    cur_task--;
  agent->datastate->agent_codes[index] = 
    xml_get_text
    (
     xml_parser->node
    );

  /* Get the code id */
  attribute = mxmlElementGetAttr
    (
     (mxml_node_t*)xml_parser->node,
     "id"
    );
  if (attribute) {
    agent->datastate->agent_code_ids[index] = (char*)malloc
      (
       sizeof(char) * 
       (strlen(attribute) + 1)
      );
    strcpy(agent->datastate->agent_code_ids[index], attribute);
  } else {
    agent->datastate->agent_code_ids[index] = (char*)malloc(sizeof(char));
    *(agent->datastate->agent_code_ids[index]) = '\0';
  }
  if (agent->datastate->tasks[cur_task]->code_id && attribute != NULL) {
    if (!strcmp(attribute, agent->datastate->tasks[cur_task]->code_id)) {
      agent->datastate->agent_code = agent->datastate->agent_codes[index];
    }
  } else {
    agent->datastate->agent_code = agent->datastate->agent_codes[0];
  }
  return MC_SUCCESS;
}

/* agent return parsing ******************************************************/
error_code_t
agent_return_xml_parse(agent_p agent)
{
  xml_parser_t xml_parser;
  xml_parser.root = agent->datastate->xml_root;
  xml_parser.node = xml_get_child(
      xml_parser.root,
      "NAME",
      1);
      
  agent_xml_parse__name(agent, &xml_parser);

  xml_parser.node = xml_get_child(
      xml_parser.root,
      "OWNER",
      1);

  agent_xml_parse__owner(agent, &xml_parser);

  xml_parser.node = xml_get_child(
      xml_parser.root,
      "HOME",
      1);

  agent_xml_parse__home(agent, &xml_parser);

  xml_parser.node = xml_get_child(
      xml_parser.root,
      "TASK",
      1);

  agent_xml_parse__tasks(agent, &xml_parser);
  return MC_SUCCESS;
}
/* message parsing  **********************************************************/
error_code_t
message_xml_parse(message_p message)
{
  error_code_t err_code;
  xml_parser_p xml_parser;
  xml_parser = (xml_parser_p)malloc(sizeof(xml_parser_t));
  xml_parser->root = message->xml_root;
  xml_parser->node = mxmlFindElement
    (
     (mxml_node_t*)xml_parser->root,
     (mxml_node_t*)xml_parser->root,
     "MOBILEC_MESSAGE",
     NULL,
     NULL,
     MXML_NO_DESCEND
    );
  if (xml_parser->node == NULL) {
    xml_parser->node = mxmlFindElement
      (
       (mxml_node_t*)xml_parser->root,
       (mxml_node_t*)xml_parser->root,
       "MOBILEC_MESSAGE",
       NULL,
       NULL,
       MXML_DESCEND
      );
  }
  if (xml_parser->node == NULL) {
    err_code = MC_ERR_PARSE;
    goto cleanup;
  }
  xml_parser->root = xml_parser->node;
  if(
      strcmp(
        (const char*)xml_get_element_name(xml_parser->node),
        "MOBILEC_MESSAGE"
        )
    )
  {
    fprintf(stderr, "Parse error. %s:%d\n", __FILE__, __LINE__);
    err_code = MC_ERR_PARSE;
    goto cleanup;
  }
  xml_parser->node = xml_get_child
    (
     xml_parser->node,
     "MESSAGE",
     1
    );
  err_code = message_xml_parse__message(message, xml_parser);
cleanup:
  free(xml_parser);
  return err_code;
}

error_code_t
message_xml_parse__message(message_p message, xml_parser_p xml_parser)
{
  const char* attribute;
  char* buf;
  char* hostname;
  char* port_str;
#ifndef _WIN32
  char* save_ptr; /* Save ptr for re-entrant strtok */
#endif
  int port;
  if (xml_parser->node == NULL) {
    return MC_ERR_PARSE;
  }
  attribute = mxmlElementGetAttr
    (
     (mxml_node_t*)xml_parser->node,
     "message"
    );
  if (!strcmp(attribute, "MOBILE_AGENT")) {
    message->message_type = MOBILE_AGENT;
    message->xml_payload = xml_get_child
      (
       xml_parser->node,
       "MOBILE_AGENT",
       1
      );
  } else if (!strcmp(attribute, "RETURN_MSG")) {
    message->message_type = RETURN_MSG;
    message->xml_payload = xml_get_child
      (
       xml_parser->node,
       "MOBILE_AGENT",
       1
      );
  } else if (!strcmp(attribute, "ACL")) {
    message->message_type = FIPA_ACL;
  } else if (!strcmp(attribute, "ENCRYPTION_INITIALIZE")) {
    message->message_type = ENCRYPTION_INITIALIZE;
    message->xml_payload = xml_get_child
      (
       xml_parser->node,
       "ENCRYPTION_DATA",
       1
      );
  } else if (!strcmp(attribute, "ENCRYPTED_DATA")) {
    message->message_type = ENCRYPTED_DATA;
    message->xml_payload = xml_get_child
      (
       xml_parser->node,
       "ENCRYPTED_DATA",
       1
      );
  } else if (!strcmp(attribute, "REQUEST_ENCRYPTION_INITIALIZE")) {
    message->message_type = REQUEST_ENCRYPTION_INITIALIZE;
  } else {
    fprintf(stderr, "Parse error. %s:%d\n", __FILE__, __LINE__);
    return MC_ERR_PARSE;
  }
  attribute = mxmlElementGetAttr
    (
     (mxml_node_t*)xml_parser->node,
     "from"
    );
  if(attribute != NULL) {
    /* Free 'from_address' first, if we need. */
    if(message->from_address) free(message->from_address);
    message->from_address = (char*)malloc
      (
       sizeof(char) * 
       (strlen(attribute)+1)
      );
    CHECK_NULL(message->from_address, exit(0););
    strcpy(message->from_address, attribute);
    buf = (char*)malloc
      (
       sizeof(char) * 
       (strlen(message->from_address)+1)
      );
    CHECK_NULL(buf, exit(0););
    strcpy(buf, message->from_address);
    hostname = strtok_r(buf, ":", &save_ptr);
    port_str = strtok_r(NULL, ":", &save_ptr);
    port = atoi(port_str);
    message->addr->sin_port = htons(port);
    free(buf);
  }
  return MC_SUCCESS;
}
  

