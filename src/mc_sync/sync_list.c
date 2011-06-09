/* SVN FILE INFO
 * $Revision: 517 $ : Last Committed Revision
 * $Date: 2010-06-11 12:06:47 -0700 (Fri, 11 Jun 2010) $ : Last Committed Date */
#ifndef _WIN32
#include <pthread.h>
#endif
#include "sync_list.h"
#include "../mc_list/list.h"
#include "../include/mc_error.h"

int syncListNodeInit(struct syncListNode_s *node) { /*{{{*/
    node->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
    node->cond = (COND_T*)malloc(sizeof(COND_T));
    node->sem  = (SEMAPHORE_T*)malloc(sizeof(SEMAPHORE_T));
    CHECK_NULL(node->lock, exit(1););
    CHECK_NULL(node->cond, exit(1););
    CHECK_NULL(node->sem , exit(1););

    MUTEX_INIT(node->lock);
    COND_INIT(node->cond);
    SEMAPHORE_INIT(node->sem);
    return 0;
} /*}}}*/

struct syncListNode_s*
syncListNodeNew(void) {
    struct syncListNode_s *ret;
    ret = (struct syncListNode_s*)malloc(sizeof(struct syncListNode_s));
    CHECK_NULL(ret, exit(1););
    ret->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
    ret->cond = (COND_T*)malloc(sizeof(COND_T));
    ret->sem  = (SEMAPHORE_T*)malloc(sizeof(SEMAPHORE_T));
    CHECK_NULL(ret->lock, exit(1););
    CHECK_NULL(ret->cond, exit(1););
    CHECK_NULL(ret->sem , exit(1););

    MUTEX_INIT(ret->lock);
    COND_INIT(ret->cond);
    SEMAPHORE_INIT(ret->sem);
    ret->signalled=0;
    return ret;
}

int syncListNodeDestroy(struct syncListNode_s *node) { /*{{{*/
    MUTEX_DESTROY(node->lock);
    COND_DESTROY(node->cond);
    SEMAPHORE_DESTROY(node->sem);

    free(node->lock);
    free(node->cond);
    free(node->sem);
    free(node);
    return 0;
} /*}}}*/

int syncListAddNode(struct syncListNode_s *node, list_t *list) { /*{{{*/
    /* Check to see if there are identical ID nums */
    listNode_t *tmp;
    ListWRLock(list);
    tmp = ListSearchCB(list, &node->id, (ListSearchFunc_t)syncListNode_CmpID);
    if(tmp) {
      fprintf(stderr, 
          "Warning: Identical COND ID's! %s:%d\n",
          __FILE__, __LINE__);
    }
    ListAdd( list, (void*) node);
    ListWRUnlock(list);
    return 0;
} /*}}}*/

int syncListNew(int id, list_t *list) { /*{{{*/
    syncListNode_t *node;
    node = (syncListNode_t *)malloc(sizeof(syncListNode_t));
    syncListNodeInit(node);
    node->id = id;
    syncListAddNode(
            node,
            list);
    return id;
} /*}}}*/

int syncListDelete(int id, list_t *list) { /*{{{*/
    syncListNode_t *tmp;
    ListWRLock(list);
    tmp = ListDeleteCB(list, &id, (ListSearchFunc_t)syncListNode_CmpID);
    ListWRUnlock(list);
    if(tmp) {
      syncListNodeDestroy(tmp);
      return 0;
    }
    return MC_ERR_NOT_FOUND;
}
        
syncListNode_t* syncListRemove(int id, list_t *list) { /*{{{*/
    syncListNode_t *tmp;
    ListWRLock(list);
    tmp = ListDeleteCB(list, &id, (ListSearchFunc_t)syncListNode_CmpID);
    ListWRUnlock(list);
    return tmp;
}

int syncListNode_CmpID(int* key, syncListNode_t* node)
{
  return *key - node->id;
}
