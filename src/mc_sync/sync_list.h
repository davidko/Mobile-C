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

int syncListNodeInit(struct syncListNode_s *node);
int syncListNodeDestroy(struct syncListNode_s *node);
struct syncListNode_s* syncListNodeNew(void);

int syncListDelete(int id, list_t *list);
syncListNode_t* syncListRemove(int id, list_t *list);
int syncListAddNode(struct syncListNode_s *node, list_t *list);
int syncListNew(int id, list_t *list);
syncListNode_t* syncListGet(int id, list_t *list);

int syncListNode_CmpID(int* key, syncListNode_t* node);
#endif
