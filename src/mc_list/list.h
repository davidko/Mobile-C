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
#include"../include/macros.h"

#define DATA void*

/* The following function pointer typedef is the list search callback function.
 * The function should take 2 arguments and compare them, returns 0 for a
 * match, non-zero for a non-match. */
typedef int (*ListSearchFunc_t) (void* key, void* element);

typedef struct listNode_s{

  DATA node_data;
  struct listNode_s *next; 

}listNode_t;

typedef listNode_t* listNode_p;

typedef struct list_s{

  listNode_p listhead;
  int size;
  MUTEX_T* lock;
  COND_T* cond;

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
DATA ListSearchCB(list_p list, void* key, ListSearchFunc_t cb);
DATA ListDeleteCB(list_p list, void* key, ListSearchFunc_t cb);

#endif
