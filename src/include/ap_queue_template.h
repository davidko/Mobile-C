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

#ifndef _AP_QUEUE_TEMPLATE_H_
#define _AP_QUEUE_TEMPLATE_H_
#include "macros.h"
#include "mc_error.h"
#include <embedch.h>

#error "Deprecated"

struct AP_GENERIC_s {void* none;};
typedef struct AP_GENERIC_s AP_GENERIC_t;
typedef AP_GENERIC_t* AP_GENERIC_p;

/* Fields will be: NAME */
#define AP_QUEUE_DECL_TEMPLATE( name, node_type) \
#define AP_QUEUE_GENERIC_DECL_TEMPLATE(name, func_name, return_type, search_type) \
return_type name##_##func_name(name##_p name, const search_type key);

#define AP_QUEUE_STD_DEFN_TEMPLATE( name, node_type) \
name##_p name##_New( void ) \
{ \
  name##_p temp; \
  temp = (name##_p)malloc(sizeof(name##_t)); \
  temp->size = 0; \
  temp->list = ListInitialize(); \
  \
  temp->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T)); \
  temp->cond = (COND_T*)malloc(sizeof(COND_T)); \
  MUTEX_INIT(temp->lock); \
  COND_INIT(temp->cond); \
  return temp; \
} \
  \
int name##_Destroy( name##_p name ) \
{ \
  struct node_type##_s* node; \
  while ((node = (node_type##_t*)name##_Pop(name)) != NULL) { \
    node_type##_Destroy(node); \
  } \
  ListTerminate(name->list); \
  MUTEX_DESTROY(name->lock); \
  COND_DESTROY(name->cond); \
  free(name->lock); \
  free(name->cond); \
  free(name); \
  return 0; \
} \
  \
name##_p name##_Copy(name##_p name) \
{ \
  int i;\
  name##_p temp; \
  struct node_type##_s* temp_node; \
  temp = (name##_p)malloc(sizeof(name##_t)); \
  if (temp == NULL) { \
    fprintf(stderr, "Memory Error at %s:%d\n", __FILE__, __LINE__); \
    exit(0); \
  } \
  temp->size = 0; \
  temp->list = ListInitialize(); \
  temp->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T)); \
  if (temp->lock == NULL) { \
    fprintf(stderr, "Memory Error at %s:%d\n", __FILE__, __LINE__); \
    exit(0); \
  } \
  temp->cond = (COND_T*)malloc(sizeof(COND_T)); \
  if(temp->cond == NULL) { \
    fprintf(stderr, "Memory Error at %s:%d\n", __FILE__, __LINE__); \
    exit(0); \
  } \
  MUTEX_INIT(temp->lock); \
  COND_INIT(temp->cond); \
  for( \
      i = 0; \
      (temp_node = (node_type##_t*)name##_SearchIndex(name, i)) != NULL; \
      i++ \
     ) \
  { \
    name##_Add(temp, node_type##_Copy(temp_node)); \
  } \
  return temp; \
} \
  \
int name##_GetSize( name##_p name ) \
{ \
  int size; \
  MUTEX_LOCK(name->lock); \
  size = name->size; \
  MUTEX_UNLOCK(name->lock); \
  return size; \
} \
  \
int name##_Add( name##_p name, node_type##_t* node ) \
{ \
  MUTEX_LOCK(name->lock); \
  ListAdd(name->list, node); \
  name->size++; \
  COND_SIGNAL(name->cond); \
  MUTEX_UNLOCK(name->lock); \
  return 0; \
} \
  \
node_type##_t* name##_Pop( name##_p name ) \
{ \
  struct node_type##_s* ret; \
  MUTEX_LOCK(name->lock); \
  if (name->size <= 0) { \
    MUTEX_UNLOCK(name->lock); \
    return NULL; \
  } \
  ret = (node_type##_t*)ListPop(name->list); \
  name->size--; \
  COND_SIGNAL(name->cond); \
  MUTEX_UNLOCK(name->lock); \
  return ret; \
} \
  \
node_type##_t* name##_WaitPop( name##_p name ) \
{ \
  struct node_type##_s* ret; \
  MUTEX_LOCK(name->lock); \
  while (name->size <= 0) { \
		COND_WAIT(name->cond, name->lock); \
  } \
  ret = (node_type##_t*)ListPop(name->list); \
  name->size--; \
  COND_SIGNAL(name->cond); \
  MUTEX_UNLOCK(name->lock); \
  return ret; \
} \
  \
struct node_type##_s* name##_SearchIndex( name##_p name, int index ) \
{ \
  struct node_type##_s* node; \
  MUTEX_LOCK(name->lock); \
  node = (node_type##_t*)ListSearch(name->list, index); \
  MUTEX_UNLOCK(name->lock); \
  return node; \
} \
  \
int name##_RemoveIndex( name##_p name, int index ) \
{ \
  struct node_type##_s* node; \
  MUTEX_LOCK(name->lock); \
  node = (node_type##_t*)ListDelete(name->list, index); \
  node_type##_Destroy(node); \
  name->size--; \
  MUTEX_UNLOCK(name->lock); \
  return 0; \
} 

#define AP_QUEUE_SEARCH_TEMPLATE( name, func_name, node_type, \
    search_type, search_expression ) \
struct node_type##_s* name##_##func_name( name##_p name, const search_type key ) \
{ \
  listNode_t* parsenode; \
  struct node_type##_s* node; \
  struct node_type##_s* ret = NULL; \
  node = NULL; \
  \
  MUTEX_LOCK(name->lock); \
  if (name->list->listhead == NULL) { \
    MUTEX_UNLOCK(name->lock); \
    return NULL; \
  } \
  for( \
      parsenode = (listNode_t*)name->list->listhead; \
      parsenode != NULL; \
      parsenode = (listNode_t*)parsenode->next \
     ) \
  { \
    node = (node_type##_t*)parsenode->node_data; \
    if (search_expression){ \
      ret = node; \
      break; \
    } \
  } \
  MUTEX_UNLOCK(name->lock); \
  return ret; \
} 

#define AP_QUEUE_REMOVE_TEMPLATE( name, func_name, node_type, \
    search_type, search_expression) \
int name##_##func_name( name##_p name, const search_type key ) \
{ \
  int err_code = MC_ERR_NOT_FOUND; \
  struct listNode_s* parsenode; \
  struct listNode_s* parsenode_last = NULL; \
  struct node_type##_s* node; \
  node = NULL; \
  \
  MUTEX_LOCK(name->lock); \
  if (name->list->listhead == NULL) { \
    MUTEX_UNLOCK(name->lock); \
    return MC_ERR_NOT_FOUND; \
  } \
  for( \
      parsenode = (listNode_t*)name->list->listhead; \
      parsenode->next != NULL; \
      parsenode = (listNode_t*)parsenode->next \
     ) \
  { \
    node = (node_type##_t*)parsenode->node_data; \
    if (search_expression) { \
      if(parsenode_last == NULL) { \
        /* Node to remove is the head */ \
        name->list->listhead = parsenode->next; \
        node_type##_Destroy(node); \
        free(parsenode); \
      } else { \
        parsenode_last->next = parsenode->next; \
        node_type##_Destroy(node); \
        free(parsenode); \
      } \
      err_code = MC_SUCCESS; \
      name->size--; \
      name->list->size--; \
      break; \
    } \
    parsenode_last = parsenode; \
  } \
  MUTEX_UNLOCK(name->lock); \
  return err_code; \
}

/* Custom function for interpreter queues */
struct interpreter_queue_s;
AP_GENERIC_p interpreter_queue_CreateRetrieve( struct interpreter_queue_s *queue, ChOptions_t* interp_options );
#endif
