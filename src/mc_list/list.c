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

    return list;
}

/***********************************************************
 * Function Name : ListTerminate
 * Purpose : Delete and Deallocates all data on list
 * Returns :void
 ***********************************************************/
void ListTerminate(list_p list)
{
    /* ensure that the list has no more nodes */
    if(list->size > 0)
    {
        printf("ERROR: MEMORY leak Created \n");
        return;
    } 

    /* deallocate the listhead */
    if(list->listhead != NULL)
        free(list->listhead);

    /* deallocate the list */
    free(list);

    return;
}

/**********************************************************
 * Function Name : ListGetSize()
 * Purpose : 
 * Return : size of the list;
 *********************************************************/
int list_pGetSize(list_p list)
{
    return list->size;
}


DATA ListGetHead(list_p list)
{
    return list->listhead->node_data;
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
    parsenode = (listNode_t *)list->listhead;

    if(list->listhead == NULL)
        return NULL;

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
        return data;
    }
    else
    {
        printf("There is nothing in the list we are returning NULL \n");
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
    int i;

    /* check to make sure list is not null */
    if(list->listhead == NULL)
        return NULL;

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
        if(i == index)
            break;
        if(i == list->size)
            return NULL;
        i++;
    }

    if (parsenode == NULL) {
      return NULL;
    }

    /* return the entry that matches index */
    return parsenode->node_data;
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
    parsenode = list->listhead;
    previous_parsenode = NULL;

    /* run through the list until the index element is found */
    if(index >= list->size || index < 0)
        return NULL;

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
    return return_data;

}

