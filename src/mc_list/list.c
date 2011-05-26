/* SVN FILE INFO
 * $Revision: 452 $ : Last Committed Revision
 * $Date: 2010-05-07 14:29:13 -0700 (Fri, 07 May 2010) $ : Last Committed Date */
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
 * THIS SOFTWARE IS PROVIDED ''AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* Filename: list.c */

#include "list.h"
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************
 * Function Name : ListInitialize()
 * Purpose : allocates and initializes the list pointer
 * Return : a pointer to the new linked list. 
 **********************************************************/
list_p ListInitialize(void)
{
  list_p list;
  /* allocate memory for the list */
  list = (list_p)malloc(sizeof(list_t));
  list->size = 0;
  list->listhead = NULL;
  MUTEX_NEW(list->lock);
  MUTEX_INIT(list->lock);
  COND_NEW(list->cond);
  COND_INIT(list->cond);

  return list;
}

/***********************************************************
 * Function Name : ListTerminate
 * Purpose : Delete and Deallocates all data on list
 * Returns :void
 ***********************************************************/
void ListTerminate(list_p list)
{
  MUTEX_LOCK(list->lock);
  /* ensure that the list has no more nodes */
  if(list->size > 0)
  {
    printf("ERROR: MEMORY leak Created \n");
    return;
  } 

  /* deallocate the listhead */
  if(list->listhead != NULL)
    free(list->listhead);

  MUTEX_UNLOCK(list->lock);
  MUTEX_DESTROY(list->lock);
  COND_DESTROY(list->cond);
  free(list->lock);
  free(list->cond);

  /* deallocate the list */
  free(list);

  return;
}

/**********************************************************
 * Function Name : ListGetSize()
 * Purpose : 
 * Return : size of the list;
 *********************************************************/
int ListGetSize(list_p list)
{
  int size;
  MUTEX_LOCK(list->lock);
  size = list->size;
  MUTEX_UNLOCK(list->lock);
  return size;
}


DATA ListGetHead(list_p list)
{
  void* node_data;
  MUTEX_LOCK(list->lock);
  node_data = list->listhead->node_data;
  MUTEX_UNLOCK(list->lock);
  return node_data;
}

/********************************************************
 * Function Name : ListPop()
 * Purpose : Removes and returns the first data element on the list
 * Return : DATA at the head of the linked list
 *******************************************************/
DATA ListPop(list_p list) 
{
  listNode_t *parsenode;
  DATA data;
  MUTEX_LOCK(list->lock);
  parsenode = (listNode_t *)list->listhead;

  if(list->listhead == NULL) {
    MUTEX_UNLOCK(list->lock);
    return NULL;
  }

  /* find the element, return and then delete */
  if(parsenode != NULL)
  {
    list->listhead = (listNode_t *)list->listhead->next;
    data = parsenode->node_data;
    free(parsenode);
    if(data == NULL)
    {
      printf("returning NULL data \n");
      exit(EXIT_FAILURE);
    }
    list->size--;
    MUTEX_UNLOCK(list->lock);
    COND_SIGNAL(list->cond);
    return data;
  }
  else
  {
    MUTEX_UNLOCK(list->lock);
    return (DATA)NULL;
  }
}

/***************************************************************
 * Function Name  : ListSearch()
 * Purpose : goes through the list by order of insertion, returns DATA 
 * Returns: DATA element at the index place of the list
 **************************************************************/
DATA ListSearch(list_p list, const int index)
{
  /* variables */
  listNode_t *parsenode;
  void* node_data;
  int i;

  MUTEX_LOCK(list->lock);
  /* check to make sure list is not null */
  if(list->listhead == NULL) {
    MUTEX_UNLOCK(list->lock);
    return NULL;
  }

  /* initialize variables */
  parsenode = list->listhead;
  i = 0;

  /* look for the index */
  for(
      parsenode = (listNode_t *)list->listhead;
      parsenode != NULL;
      parsenode = (listNode_t *)parsenode->next
     )
  {
    if(i == index) {
      break;
    }
    if(i == list->size) {
      MUTEX_UNLOCK(list->lock);
      return NULL;
    }
    i++;
  }

  if (parsenode == NULL) {
    MUTEX_UNLOCK(list->lock);
    return NULL;
  }

  /* return the entry that matches index */
  node_data = parsenode->node_data;
  COND_SIGNAL(list->cond);
  MUTEX_UNLOCK(list->lock);
  return node_data;
}

/*************************************************************
 * Function Name : ListAdd
 * Purpose : Adds a data element to the end of the list
 * Returns : 0 on success -1 on failure. 
 **************************************************************/
int ListAdd(list_p list, DATA data)
{
  /* variables */
  listNode_t *parsenode;
  listNode_t *new_listnode;
  MUTEX_LOCK(list->lock);
  parsenode = (listNode_t *) list->listhead;

  /* create the new node that will be inserted into the list */
  new_listnode = (listNode_t *)malloc(sizeof(listNode_t));
  new_listnode->node_data = data;

  /* If the list is currently empty, we can insert into the first one */
  if(list->listhead == NULL)
  {
    list->listhead = new_listnode;
    list->listhead->next = NULL;
    list->size = 1;
    COND_SIGNAL(list->cond);
    MUTEX_UNLOCK(list->lock);
    return 0;
  }

  /* look for the next empty spot to place a new node */
  for(
      parsenode = (listNode_t *) list->listhead; 
      parsenode->next != NULL; 
      parsenode = (listNode_t *) parsenode->next
     );

  /* parsenode->next = (struct listNode_t *)new_listnode; */
  parsenode->next = (listNode_t *)new_listnode;
  new_listnode->next = NULL;
  list->size++;

  /* return 0 for success */
  COND_SIGNAL(list->cond);
  MUTEX_UNLOCK(list->lock);
  return 0;
}

/****************************************************************
 * Function Name : ListInsert
 * Purpose: To Add a DATA element to the idex place in the linked  list
 *
 ****************************************************************/
int ListInsert(list_p list, DATA data, const int index)
{
  /* Function has not been written yet 
     currently there is no need for this utility */

  return 0;
}

/******************************************************
 * Function Name: ListDelete 
 * Purpose: Deletes an element from the list
 * Return : 0 success , -1 failure 
 *****************************************************/
DATA ListDelete(list_p list, const int index)
{
  /* variables */
  listNode_t *parsenode;
  int i;
  listNode_t *previous_parsenode;
  DATA return_data;
  MUTEX_LOCK(list->lock);
  parsenode = list->listhead;
  previous_parsenode = NULL;

  /* run through the list until the index element is found */
  if(index >= list->size || index < 0) {
    MUTEX_UNLOCK(list->lock);
    return NULL;
  }

  if(index == 0) 
  {
    /* Delete and return the head */
    parsenode = list->listhead;
    list->listhead = list->listhead->next;
    list->size--;
    return_data = parsenode->node_data;
  } else {

    for(i = 1; i < list->size && parsenode != NULL; i++)
    {
      previous_parsenode = parsenode;
      parsenode = (listNode_t *) parsenode->next;
      if(i == index)
        break;
    }

    /* destroy the pointer */  
    previous_parsenode->next = parsenode->next;

    /* save the data from the node */
    return_data = parsenode->node_data;

    list->size--;
  }
  /* free the memory for the node */
  free(parsenode); 

  /* return a pointer of the data */
  MUTEX_UNLOCK(list->lock);
  return return_data;
}

DATA ListSearchCB(list_p list, void* key, ListSearchFunc_t cb)
{
  /* variables */
  listNode_t *parsenode;
  void* node_data;
  int i;

  MUTEX_LOCK(list->lock);

  /* check to make sure list is not null */
  if(list->listhead == NULL) {
  MUTEX_UNLOCK(list->lock);
    return NULL;
  }

  /* initialize variables */
  parsenode = list->listhead;
  i = 0;

  /* look for the index */
  for(
      parsenode = (listNode_t *)list->listhead;
      parsenode != NULL;
      parsenode = (listNode_t *)parsenode->next
     )
  {
    if(!cb(key, parsenode->node_data)) {
      break;
    }
    if(i == list->size) {
      MUTEX_UNLOCK(list->lock);
      return NULL;
    }
    i++;
  }

  if (parsenode == NULL) {
    MUTEX_UNLOCK(list->lock);
    return NULL;
  }

  /* return the entry that matches index */
  node_data = parsenode->node_data;
  MUTEX_UNLOCK(list->lock);
  return node_data;
}

DATA ListDeleteCB(list_p list, void* key, ListSearchFunc_t cb)
{
  /* variables */
  listNode_t *parsenode;
  listNode_t *lastparsenode;
  void* node_data;

  MUTEX_LOCK(list->lock);

  /* check to make sure list is not null */
  if(list->listhead == NULL) {
    MUTEX_UNLOCK(list->lock);
    return NULL;
  }

  /* initialize variables */
  parsenode = list->listhead;

  /* Check to see if the head is a match */
  if(!cb(key, parsenode)) {
    node_data = parsenode->node_data;
    list->listhead = parsenode->next;
    free(parsenode);
    list->size--;
    COND_SIGNAL(list->cond);
    MUTEX_UNLOCK(list->lock);
    return node_data;
  }

  /* If not, continue with the search */
  lastparsenode = parsenode;
  parsenode = parsenode->next;
  while(parsenode != NULL) {
    if(!cb(key, parsenode->node_data)) {
      /* Found a match; delete and return it */
      node_data = parsenode->node_data;
      lastparsenode->next = parsenode->next;
      free(parsenode);
      list->size--;
      COND_SIGNAL(list->cond);
      MUTEX_UNLOCK(list->lock);
      return node_data;
    }
  }
  MUTEX_UNLOCK(list->lock);
  return NULL;
}
