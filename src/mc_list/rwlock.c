#include <stdio.h>
#include <stdlib.h>
#include "rwlock.h"

int rwlock_wrlock(rwlock_t* rwlock)
{
  MUTEX_LOCK(rwlock->lock);
  while(rwlock->readers) {
    COND_WAIT(rwlock->writer_cond, rwlock->lock);
  }
  return 0;
}

int rwlock_wrunlock(rwlock_t* rwlock)
{
  COND_SIGNAL(rwlock->reader_cond);
  MUTEX_UNLOCK(rwlock->lock);
  return 0;
}

int rwlock_rdlock(rwlock_t* rwlock)
{
  MUTEX_LOCK(rwlock->lock);
  rwlock->readers++;
  MUTEX_UNLOCK(rwlock->lock);
  return 0;
}

int rwlock_rdunlock(rwlock_t* rwlock)
{
  MUTEX_LOCK(rwlock->lock);
  rwlock->readers--;
  COND_SIGNAL(rwlock->writer_cond);
  MUTEX_UNLOCK(rwlock->lock);
  return 0;
}

int rwlock_rdwait(rwlock_t* rwlock)
{
  MUTEX_LOCK(rwlock->lock);
  rwlock->readers--;
  COND_WAIT(rwlock->reader_cond, rwlock->lock);
  rwlock->readers++;
  MUTEX_UNLOCK(rwlock->lock);
  return 0;
}

rwlock_t* rwlock_New(void)
{
  rwlock_t* rwlock = (rwlock_t*)malloc(sizeof(rwlock_t));
  rwlock->readers = 0;
  MUTEX_NEW(rwlock->lock);
  MUTEX_INIT(rwlock->lock);
  COND_NEW(rwlock->reader_cond);
  COND_INIT(rwlock->reader_cond);
  COND_NEW(rwlock->writer_cond);
  COND_INIT(rwlock->writer_cond);
  return rwlock;
}

int rwlock_Destroy(rwlock_t* rwlock)
{
  MUTEX_DESTROY(rwlock->lock);
  COND_DESTROY(rwlock->reader_cond);
  COND_DESTROY(rwlock->writer_cond);
  free(rwlock->lock);
  free(rwlock->reader_cond);
  free(rwlock->writer_cond);
  free(rwlock);
  return 0;
}
