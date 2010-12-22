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

syncListNode_t *syncListFind(int id, struct syncList_s *list) { /*{{{*/
    listNode_t *tmp;
    RWLOCK_RDLOCK(list->lock);
    tmp = (listNode_t*)list->list->listhead;
    while (tmp != NULL) {
        if (((syncListNode_t*)(tmp->node_data))->id == id) {
            RWLOCK_RDUNLOCK(list->lock);
            return  
                ((syncListNode_t*)tmp->node_data);
        }
        tmp = tmp->next;
    }
    RWLOCK_RDUNLOCK(list->lock);
    return NULL;
} /* }}} */

struct syncList_s* syncListInit(void) /*{{{*/
{
    struct syncList_s* newList;
    newList = (struct syncList_s*)malloc(sizeof(struct syncList_s));
    newList->lock = (RWLOCK_T*)malloc(sizeof(RWLOCK_T));
    RWLOCK_INIT(newList->lock);

    newList->giant_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
    MUTEX_INIT(newList->giant_lock);

    newList->list = ListInitialize();
    return newList;
} /*}}}*/

int syncListDestroy(struct syncList_s* list)
{
    int i;
    syncListNode_t *tmp;
    RWLOCK_WRLOCK(list->lock);
    for(i = 0; i < list->list->size; i++) {
        tmp = (syncListNode_t*)ListSearch(list->list, i);
        ListDelete(list->list, i);
        syncListNodeDestroy(tmp);
        RWLOCK_WRUNLOCK(list->lock);
    }
    RWLOCK_WRUNLOCK(list->lock);
    ListTerminate(list->list);
    MUTEX_DESTROY(list->lock);
    free(list->lock);
    MUTEX_DESTROY(list->giant_lock);
    free(list->giant_lock);
    free(list);
    return 0;
}

int syncListAddNode(struct syncListNode_s *node, struct syncList_s *list) { /*{{{*/
    /* Check to see if there are identical ID nums */
    listNode_t *tmp;
    RWLOCK_WRLOCK(list->lock);
    tmp = (listNode_t *)list->list->listhead;
    while (tmp != NULL) {
        if (((syncListNode_t*)(tmp->node_data))->id == node->id) {
            fprintf(stderr, 
                    "Warning: Identical COND ID's! %s:%d\n",
                    __FILE__, __LINE__);
            continue;
        }
        tmp = tmp->next;
    }
    ListAdd( list->list, (DATA) node);
    RWLOCK_WRUNLOCK(list->lock);
    return 0;
} /*}}}*/

int syncListNew(int id, struct syncList_s *list) { /*{{{*/
    syncListNode_t *node;
    node = (syncListNode_t *)malloc(sizeof(syncListNode_t));
    syncListNodeInit(node);
    syncListAddNode(
            node,
            list);
    return id;
} /*}}}*/

int syncListDelete(int id, struct syncList_s *list) { /*{{{*/
    int i;
    syncListNode_t *tmp;
    RWLOCK_WRLOCK(list->lock);
    for(i = 0; i < list->list->size; i++) {
        tmp = (syncListNode_t*)ListSearch(list->list, i);
        if(tmp->id == id) {
            ListDelete(list->list, i);
            syncListNodeDestroy(tmp);
            RWLOCK_WRUNLOCK(list->lock);
            return 0;
        }
    }
    RWLOCK_WRUNLOCK(list->lock);
    return MC_ERR_NOT_FOUND;
}
        
syncListNode_t* syncListRemove(int id, struct syncList_s *list) { /*{{{*/
    int i;
    syncListNode_t *tmp;
    RWLOCK_WRLOCK(list->lock);
    for(i = 0; i < list->list->size; i++) {
        tmp = (syncListNode_t*)ListSearch(list->list, i);
        if (tmp == NULL) {
            /* Not found */
            RWLOCK_WRUNLOCK(list->lock);
            return NULL;
        }
        if(tmp->id == id) {
            if (ListDelete(list->list, i) == NULL) {
                fprintf(stderr, "Fatal error. %s:%d\n",
                        __FILE__,
                        __LINE__ );
                exit(1);
            }
            RWLOCK_WRUNLOCK(list->lock);
            return tmp;
        }
    }
    RWLOCK_WRUNLOCK(list->lock);
    return NULL;
}
