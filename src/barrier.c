/* SVN FILE INFO
 * $Revision: 400 $ : Last Committed Revision
 * $Date: 2009-08-23 13:23:47 -0700 (Sun, 23 Aug 2009) $ : Last Committed Date */
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

#include "include/barrier.h"
#include "include/mc_error.h"

/* Node Funcs */
  barrier_node_p
barrier_node_Initialize(int id, int num_registered) 
{
  barrier_node_p node;

  /* Memory Allocation */
  node = (barrier_node_p)malloc(sizeof(barrier_node_t));
  CHECK_NULL(node, return NULL;);
  node->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  CHECK_NULL(node->lock, return NULL;);
  node->cond = (COND_T*)malloc(sizeof(COND_T));
  CHECK_NULL(node->cond, return NULL;);

  /* Sync Init */
  MUTEX_INIT(node->lock);
  COND_INIT(node->cond);

  /* Initial values */
  node->id = id;
  node->num_registered = num_registered;
  node->num_waiting = 0;

  return node;
}

  int
barrier_node_Destroy(barrier_node_p node)
{
  /* Sync Destroy */
  MUTEX_DESTROY(node->lock);
  COND_DESTROY(node->cond);

  /* Free Memory */
  free(node->cond);
  free(node->lock);
  free(node);

  return MC_SUCCESS;
}

/* List Funcs */

  int
barrier_queue_Add(barrier_queue_p list, barrier_node_p node)
{
  /* Check for identical IDs and warn */
  int err_code = MC_SUCCESS;
  listNode_t *tmp;
  RWLOCK_WRLOCK(list->lock);
  tmp = (listNode_t*)list->list->listhead;
  while(tmp != NULL) {
    if (((barrier_node_p)(tmp->node_data))->id == node->id) {
      fprintf(stderr,
          "Warning: Barrier node id %d reregistered. %s:%d\n",
          node->id, __FILE__, __LINE__ );
      err_code = MC_WARN_DUPLICATE;
      continue;
    }
    tmp = tmp->next;
  }
  ListAdd(list->list, (DATA) node);
  list->size++;
  RWLOCK_WRUNLOCK(list->lock);
  return err_code;
}

  int 
barrier_queue_Delete(int id, barrier_queue_p list)
{
  int i;
  barrier_node_p tmp;
  RWLOCK_WRLOCK(list->lock);
  for (i = 0; i < list->list->size; i++) {
    tmp = (barrier_node_p)ListSearch(list->list, i);
    if (tmp->id == id) {
      ListDelete(list->list, i);
      barrier_node_Destroy(tmp);
      list->size--;
      RWLOCK_WRUNLOCK(list->lock);
      return MC_SUCCESS;
    }
  }
  RWLOCK_WRUNLOCK(list->lock);
  return MC_ERR_NOT_FOUND;
}

  int
barrier_queue_Destroy(barrier_queue_p queue)
{
  barrier_node_p node;
  while((node = barrier_queue_Pop(queue)) != NULL) {
    barrier_node_Destroy(node);
  }
  ListTerminate(queue->list);
  RWLOCK_DESTROY(queue->lock);
  free(queue->lock);
  free(queue);
  return MC_SUCCESS;
}

  barrier_node_p
barrier_queue_Get(barrier_queue_p list, int id)
{
  listNode_t* tmp;
  RWLOCK_RDLOCK(list->lock);
  tmp = (listNode_t*)list->list->listhead;
  while (tmp != NULL) {
    if (((barrier_node_p)(tmp->node_data))->id == id) {
      RWLOCK_RDUNLOCK(list->lock);
      return ((barrier_node_p)tmp->node_data);
    }
    tmp = tmp->next;
  }
  RWLOCK_RDUNLOCK(list->lock);
  return NULL;
}

  barrier_queue_p
barrier_queue_New(void)
{
  barrier_queue_p new_list;
  new_list = (barrier_queue_p)malloc(sizeof(barrier_queue_t));
  CHECK_NULL(new_list, return NULL;);
  new_list->lock = (RWLOCK_T*)malloc(sizeof(RWLOCK_T));
  CHECK_NULL(new_list->lock, return NULL;);
  RWLOCK_INIT(new_list->lock);

  new_list->list = ListInitialize();
  return new_list;
}

  barrier_node_p
barrier_queue_Pop(barrier_queue_p queue)
{
  barrier_node_p node = (barrier_node_p)ListPop(queue->list);
  return node;
}

