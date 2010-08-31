/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
/*
 * Copyright (c) 2006 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the Integration Engineering Laboratory of the University of 
 * California, Davis. The name of the University may not be used to 
 * endorse or promote products derived from this software without 
 * specific prior written permission. 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

/* Filename: list.h */

#ifndef LIST_H
#define LIST_H

#include<stdio.h>
#include<stdlib.h>

#define DATA void*

typedef struct listNode_s{

  DATA node_data;
  struct listNode_s *next; 

}listNode_t;

typedef listNode_t* listNode_p;

typedef struct list_s{

  listNode_p listhead;
  int size;

}list_t;

/* create a pointer type for the LinkedList */
typedef list_t* list_p;

/* some basic functions that help  */
list_p ListInitialize(void);
  /* note: this Terminate Function should only be called on empty lists */
void ListTerminate(list_p list);

/* functions that return intergers */
int ListGetSize(list_p list);
int ListAdd(list_p list, DATA data);
int ListInsert(list_p list, DATA data, const int index);

/* functions that will return pointers to the data */
DATA ListGetHead(list_p list);
DATA ListPop(list_p list);
DATA ListSearch(list_p list, const int index);
DATA ListDelete(list_p list, const int index);


#define QUEUE_TEMPLATE( name, node_type, search_type, search_var_name ) \
typedef struct name##_s name \
{ \
  int size; \
  list_p list; \
  MUTEX_T* lock; \
  COND_T* cond; \
} name##_t; \
  \
typedef name##_t* name##_p; \
  \
name##_p name##Initialize( void ); \
void name##Destroy( name##_p name ); \
int name##Add( name##_p name, node_type ); \
name##_p name##Pop( name##_p name ); \
name##_p name##Search( name##_p name, search_type key ); \
int name##Remove(name##_p name, search_type key ); \
name##_p name##SearchIndex( name##_p name, int index ); \
int name##RemoveIndex(name##_p name, int index); \
  \
name##_p name##Initialize( void ) \
{ \
  name##_p temp; \
  temp = (name##_p)malloc(sizeof(name##_t)); \
  temp->size = 0; \
  temp->list = ListInitialize(); \
  \
  temp->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T)); \
  temp->cond = (COND_T*)malloc(sizeof(COND_T)); \
  return temp; \
} \
  \
int name##Destroy( name##_p name ) \
{ \
  ListTerminate(name->list); \
  MUTEX_DESTROY(name->lock); \
  COND_DESTROY(name->cond); \
  free(name->lock); \
  free(name->cond); \
  return 0; \
} \
  \
int name##Add( name##_p name, node_type* node ) \
{ \
  MUTEX_LOCK(name->lock); \
  ListAdd(name->list, node); \
  name->size++; \
  COND_SIGNAL(name->cond); \
  MUTEX_UNLOCK(name->lock); \
  return 0; \
} \
  \
node_type* name##Pop( name##_p name ) \
{ \
  node_type *ret; \
  MUTEX_LOCK(name->lock); \
  ret = ListPop(name->list); \
  name->size--; \
  COND_SIGNAL(name->cond); \
  MUTEX_UNLOCK(name->lock); \
  return ret; \
} \
  \
node_type* name##Search( name##_p name, search_type value ) \
{ \
  listNode_t* parsenode; \
  node_type* node; \
  node = NULL; \
  \
  MUTEX_LOCK(name->lock); \
  if (name->list->listhead == NULL) { \
    MUTEX_UNLOCK(name->lock); \
    return NULL; \
  } \
  for( \
      parsenode = (listNode_t*)name->list->listhead; \
      parsenode->next != NULL; \
      parsenode = (listNode_t*)parsenode->next \
     ) \
  { \
    node = (node_type*)parsenode->node_data; \
    if (node->search_var_name == value ) \
      break; \
  } \
  MUTEX_UNLOCK(name->lock); \
  return node; \
} \
  \
int name##Remove( name##_p name, search_type key ) \
{ \
  listNode_t* parsenode; \
  node_type* node; \
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
    node = (node_type*)parsenode->node_data; \
    if (node->search_var_name == key) \
      break; \
  } \
  MUTEX_UNLOCK(name->lock); \
} \
  \
name##_p name##SearchIndex( name##_p name, int index ) \
{ \
  node_type* node; \
  MUTEX_LOCK(name->lock); \
  node = (node_type*)ListSearch(name->list, index); \
  MUTEX_UNLOCK(name->lock); \
  return node; \
} \
  \
int name##RemoveIndex( name##_p name, int index ) \
{ \
  node_type* node; \
  MUTEX_LOCK(name->lock); \
  node = ListDelete(name->list, index); \
  node_type##Destroy(node); \
  MUTEX_UNLOCK(name->lock); \
  return 0; \
} 

#endif
