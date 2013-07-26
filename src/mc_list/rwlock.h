#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include "../include/macros.h"

typedef struct rwlock_s
{
  int readers;
  MUTEX_T* lock;
  COND_T* reader_cond;
  COND_T* writer_cond;
} rwlock_t;

int rwlock_wrlock(rwlock_t* rwlock);
int rwlock_wrunlock(rwlock_t* rwlock);

int rwlock_rdlock(rwlock_t* rwlock);
int rwlock_rdunlock(rwlock_t* rwlock);

int rwlock_rdwait(rwlock_t* rwlock);

/* Currently own wr lock, but want to wait until something happens, then regain
 * write-lock */
int rwlock_wrwait(rwlock_t* rwlock);

rwlock_t* rwlock_New(void);

int rwlock_Destroy(rwlock_t* rwlock);
#endif
