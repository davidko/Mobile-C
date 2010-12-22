/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
/* CondList is a list of condition variables. Agents may set their own
 * condition variables, which other entities (agents, binary api) may signal.
 * */

#ifndef _COND_LIST_H_
#define _COND_LIST_H_
#include "../include/macros.h"
#include "../mc_list/list.h"
#include "../include/mc_rwlock.h"

/* CondlistNode_t */

typedef struct syncListNode_s {
    MUTEX_T *lock;
    COND_T *cond;
    SEMAPHORE_T *sem;
    int id;
    int signalled;
}syncListNode_t;
typedef syncListNode_t* syncListNode_p;

/* CondList */
typedef struct syncList_s {
    RWLOCK_T *lock; /* Protects the list */
    MUTEX_T *giant_lock; /* Higher level lock: used to protect series
                            of list operations */
    list_p list;
    int size;
}syncList_t;
typedef syncList_t* syncList_p;

int syncListNodeInit(struct syncListNode_s *node);
int syncListNodeDestroy(struct syncListNode_s *node);
syncListNode_t *syncListFind(int id, struct syncList_s *list);
struct syncListNode_s* syncListNodeNew(void);

int syncListDelete(int id, struct syncList_s *list);
syncListNode_t* syncListRemove(int id, struct syncList_s *list);
struct syncList_s* syncListInit(void);
int syncListDestroy(struct syncList_s* list);
int syncListAddNode(struct syncListNode_s *node, struct syncList_s *list);
int syncListNew(int id, struct syncList_s *list);
syncListNode_t* syncListGet(int id, struct syncList_s *list);


#endif
