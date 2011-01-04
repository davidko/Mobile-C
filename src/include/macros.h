/* SVN FILE INFO
 * $Revision: 530 $ : Last Committed Revision
 * $Date: 2010-06-16 16:52:19 -0700 (Wed, 16 Jun 2010) $ : Last Committed Date */
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

/* Macro Synopses
 *
 *      Error Checking
 *
 * CHECK_NULL(var, action) : Checks value of 'var' for NULL. If it is 
 *      null, an error message is printed to stderr and 'action' is 
 *      performed.
 *
 *      Thread Creation/Cancellation
 *
 * GET_THREAD_MODE(started_threads, thread): Returns 1 if 'thread' is set 
 *      run in 'started_threads'.
 * SET_THREAD_ON(a, b) : Sets thread 'b' to 'on' in status 'a'.
 * SET_THREAD_OFF(a, b) : See SET_THREAD_ON
 *
 *      Thread Syncronization
 *
 * MUTEX_T : Platform independant mutex data type.
 * COND_T : Platform independant condition variable data type.
 * MUTEX_INIT( mutex ) : Platform ind. mutex initialization.
 * COND_INIT(cond) : Platform ind. condition variable initialization.
 * MUTEX_DESTROY(mutex) : Platform ind. mutex variable destruction.
 * COND_DESTROY(cond) : Platform ind. cond variable destruction.
 * MUTEX_LOCK(mutex) : Locks mutex 'mutex'.
 * MUTEX_UNLOCK(mutex) : Unlocks mutex 'mutex'.
 * SIGNAL(cond, mutex, action) : Sends a signal to condition variable
 *      'cond' protected by mutex 'mutex'. Executes action 'action' after
 *      locking mutex 'mutex' but before signalling the condition 
 *      variable.
 * WAKE_QUEUE(queue) : Signals and activates a thread sleeping on one
 *      of the system queues.
 * COND_SLEEP(cond, mutex) : Causes a thread to sleep on a condition 
 *      variable 'cond', protected by 'mutex'.
 * COND_RESET(cond, mutex) : Resets condition variables after being
 *      woken from COND_SLEEP.
 *
 *      Ch Datatype Conversions
 *
 * CH_DATATYPE_SIZE(type, size) : Takes 'ChType_t type' as input,
 *      fills 'size' with the byte size of 'type'.
 * CH_DATATYPE_STRING(type, string) : Takes 'ChType_t type' as input,
 *      fills 'char* string' with a string of the type. 
 * CH_STRING_DATATYPE(string, type) : Takes 'char* string' as input
 *      and fills 'ChType_t type'
 * Ch_DATATYPE_STR_TO_VAL(type, string, val) : Takes 'ChType_t type' and
 *      'char* string' as input and fills 'void* val' with the value
 *      contained within 'string'.
 */
#ifndef _MACROS_H_
#define _MACROS_H_

#ifndef _WIN32
#include <pthread.h>
#include <semaphore.h>
#include "config.h"
#include <errno.h>
#else
#include <windows.h>
#endif

/* * * * * * * * * * * * * * *
 * T H R E A D   S T A T U S *
 * * * * * * * * * * * * * * */

/*************************************
 * AI => Agent Initializing Thread   * 
 * AM => Agent Managing Thread       * 
 * CL => Connection Listening Thread * 
 * DF => Directory Facilitator Thr.  *
 * MR => Message Receiving Thread    * 
 * MS => Message Sending Thread      * 
 * CP => Command Prompt Thread       * 
 *************************************/

/* GET_THREAD_MODE Macro:
 * Usage: GET_THREAD_MODE(runthreads, MC_THREAD_AM)
 * Returns 1 if thread is set to run, 0 otherwise. */
#define GET_THREAD_MODE(a, b) ( (a & (1<<b)) / (1<<b) )

/* SET_THREAD_XXX(runthreads, XX_THREAD); */
#define SET_THREAD_ON(a, b) a = (a | (1<<b))
#define SET_THREAD_OFF(a, b) a = (a & (~(1<<b)))


/* Struct Macro */
#define STRUCT( name, members ) \
  typedef struct name##_s { \
    members \
  } name##_t; \
typedef name##_t* name##_p;


/* * * * * * * * * * * * *
 * U N I X   M A C R O S *
 * * * * * * * * * * * * */               /* Windows macros follow */
#ifndef _WIN32

#define SOCKET_ERROR() \
	printf("Socket error. %s:%d\nerrno:%d", __FILE__, __LINE__, errno); \
	sleep(500)

/* ****** *
 * THREAD *
 * ****** */
#define PTHREAD_STACK_SIZE 131072 /* This is PTHREAD_STACK_MIN * 8 for me */

#ifndef THREAD_T
#define THREAD_T pthread_t
#endif

#define THREAD_CREATE( thread_handle, function, arg ) \
  while(pthread_create( \
      thread_handle, \
      &attr, \
      function, \
      (void*) arg \
      ) < 0) { \
			printf("pthread_create failed. Trying again...\n"); \
	}

#define THREAD_CANCEL( thread_handle ) \
  pthread_cancel( thread_handle )

#define THREAD_JOIN(thread_handle ) \
  pthread_join( thread_handle, NULL )

#define THREAD_DETACH(thread_handle) \
	if(pthread_detach(thread_handle) < 0) { \
		printf("pthread_detach failed. %s:%d\n", __FILE__, __LINE__); \
	}

#define THREAD_EXIT() \
  pthread_exit(NULL)

/* ***** */
/* MUTEX */
/* ***** */

/* Typedef */
#define MUTEX_T pthread_mutex_t
/* Init */
#define MUTEX_INIT(mutex) \
  pthread_mutex_init(mutex, NULL)
/* Destroy */
#define MUTEX_DESTROY(mutex) \
  pthread_mutex_destroy(mutex)
/* functions */
#define MUTEX_LOCK(mutex)                                                   \
  if (pthread_mutex_lock( mutex ))                                        \
fprintf(stderr, "pthread lock error: %s:%d\n", __FILE__, __LINE__)
#define MUTEX_UNLOCK(mutex) \
  pthread_mutex_unlock( mutex )
#define MUTEX_NEW(mutex) \
  mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)); \
  if (mutex == NULL)  \
    fprintf(stderr, "Memory Error. %s:%d\n", __FILE__,__LINE__); \

/* **** *
 * COND *
 * **** */
/* Typedef */
#define COND_T pthread_cond_t
/* Init */
#define COND_INIT(cond) \
  pthread_cond_init(cond, NULL)
/* New */
#define COND_NEW(cond) \
  cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)); \
  if (cond == NULL)  \
    fprintf(stderr, "Memory Error. %s:%d\n", __FILE__,__LINE__); \
/* Destroy */
#define COND_DESTROY(cond) \
  pthread_cond_destroy(cond)
/* functions */
#define COND_WAIT( cond , mutex ) \
  pthread_cond_wait(cond, mutex )
/* Wait until 'test' is true */
#define COND_SLEEP( cond, mutex, test )       \
  if (pthread_mutex_lock( mutex ))    \
printf("pthread lock error: %s:%d\n", __FILE__, __LINE__); \
if (!test) { \
  pthread_cond_wait( cond, mutex ); \
} 
#define COND_RESET( cond, mutex ) \
  pthread_mutex_unlock( mutex );
#define COND_SLEEP_ACTION(cond, mutex, action) \
  if (pthread_mutex_lock( mutex )) \
printf("pthread lock error: %s:%d\n", __FILE__, __LINE__); \
action; \
pthread_cond_wait( cond, mutex ); 
#define SIGNAL(cond, mutex, action) \
  pthread_mutex_lock( mutex ); \
action; \
pthread_cond_signal( cond ); \
pthread_mutex_unlock( mutex )
#define COND_BROADCAST(cond) \
  pthread_cond_broadcast( cond )
#define COND_SIGNAL(cond) \
  pthread_cond_signal( cond )

/* ********* *
 * SEMAPHORE *
 * ********* */
/* Typedef */
#define SEMAPHORE_T sem_t
/* Init */
#define SEMAPHORE_INIT(sem) \
  sem_init(sem, 0, 0)
/* Destroy */
#define SEMAPHORE_DESTROY(sem) \
  sem_destroy(sem)
/* Functions */
#define SEMAPHORE_WAIT(sem) \
  sem_wait(sem)
#define SEMAPHORE_POST(sem) \
  sem_post(sem)

/* ******* *
 * RW_LOCK *
 * ******* */
/* Typedef */
#ifdef HAVE_PTHREAD_RWLOCK_T
#define RWLOCK_T pthread_rwlock_t
#else
#define RWLOCK_T mc_rwlock_t
#endif
/* Init */
#ifdef HAVE_PTHREAD_RWLOCK_T
#define RWLOCK_INIT(rwlock) \
  pthread_rwlock_init(rwlock, NULL)
#else
#define RWLOCK_INIT(rwlock) \
  mc_rwlock_init(rwlock)
#endif
/* Destroy */
#ifdef HAVE_PTHREAD_RWLOCK_T
#define RWLOCK_DESTROY(rwlock) \
  pthread_rwlock_destroy(rwlock)
#else
#define RWLOCK_DESTROY(rwlock) \
  mc_rwlock_destroy(rwlock)
#endif
/* Functions */
#ifdef HAVE_PTHREAD_RWLOCK_T
#define RWLOCK_RDLOCK(rwlock) \
  if (pthread_rwlock_rdlock(rwlock)) \
fprintf(stderr, "rwlock error: %s:%d\n", __FILE__, __LINE__)
#define RWLOCK_RDUNLOCK(rwlock) \
  if (pthread_rwlock_unlock(rwlock)) \
fprintf(stderr, "rwunlock error: %s:%d\n", __FILE__, __LINE__)
#define RWLOCK_WRLOCK(rwlock) \
  if (pthread_rwlock_wrlock(rwlock)) \
fprintf(stderr, "rwlock error: %s:%d\n", __FILE__, __LINE__)
#define RWLOCK_WRUNLOCK(rwlock) \
  if (pthread_rwlock_unlock(rwlock)) \
fprintf(stderr, "rwunlock error: %s:%d\n", __FILE__, __LINE__)
#else
#define RWLOCK_RDLOCK(rwlock) \
  mc_rwlock_rdlock(rwlock)
#define RWLOCK_RDUNLOCK(rwlock) \
  mc_rwlock_rdunlock(rwlock)
#define RWLOCK_WRLOCK(rwlock) \
  mc_rwlock_wrlock(rwlock)
#define RWLOCK_WRUNLOCK(rwlock) \
  mc_rwlock_wrunlock(rwlock)
#endif


/* ***************** *
 * SYSTEM QUEUE SYNC *
 * ***************** */
#define WAKE_QUEUE(queue, action)                \
  if (pthread_mutex_trylock( queue->lock ) == 0) {    \
    action;                                                 \
    pthread_cond_signal( queue->cond);           \
    pthread_mutex_unlock( queue->lock);            \
  }                                           
#define SLEEP_QUEUE( queue )                                        \
  if (pthread_mutex_lock( queue->thread_mutex ))                  \
printf("pthread lock error: %s:%d\n", __FILE__, __LINE__);  \
pthread_cond_wait( queue->touched_signal, queue->thread_mutex )
#define SLEEP_RESET( queue )                        \
  pthread_mutex_unlock( queue->thread_mutex )



#else 
/* * * * * * * * * * * * * * * *
 * W I N D O W S   M A C R O S *
 * * * * * * * * * * * * * * * */

#define SOCKET_ERROR() 
/* ******* *
 * THREADS *
 * ******* */
#ifndef THREAD_T
#define THREAD_T HANDLE
#endif

#define THREAD_CREATE(thread_handle, function, arg) \
  *(thread_handle) = CreateThread( \
      NULL, \
      (SIZE_T)stack_size, \
      function, \
      arg, \
      0, \
      NULL \
      )

#define THREAD_CANCEL(thread_handle)  \
  TerminateThread( thread_handle, 0)

#define THREAD_JOIN(thread_handle) \
  WaitForSingleObject(thread_handle, INFINITE)

#define THREAD_EXIT() \
  ExitThread(0)

#define THREAD_DETACH() 



/* ***** */
/* MUTEX */
/* ***** */
/* Typedef */
#define MUTEX_T HANDLE
/* Init */
#define MUTEX_INIT(mutex) \
  *mutex = CreateMutex(NULL, FALSE, NULL)
/* Destroy */
#define MUTEX_DESTROY(mutex)
/* Functions */
#define MUTEX_LOCK(mutex)           \
  WaitForSingleObject(            \
      *mutex ,                 \
      INFINITE)
#define MUTEX_UNLOCK(mutex)         \
  ReleaseMutex( *mutex )
#define MUTEX_NEW(mutex) \
  mutex = (HANDLE*)malloc(sizeof(HANDLE)); \
  if(mutex == NULL) \
    fprintf(stderr, "Memory Error. %s:%d\n", __FILE__, __LINE__)


/* **** *
 * COND *
 * **** */
/* Typedef */
#define COND_T HANDLE
/* Init */
#define COND_INIT(cond) \
  *cond = CreateEvent(NULL, TRUE, TRUE, NULL);\
  ResetEvent(*cond)
/* Destroy */
#define COND_DESTROY(cond)
/* Functions */

#define COND_WAIT( cond , mutex ) \
ResetEvent(*cond); \
ReleaseMutex(*mutex); \
WaitForSingleObject( *cond, INFINITE)
#define COND_SLEEP( cond, mutex, test )   \
  ResetEvent( *cond );             \
if (!test){ \
  WaitForSingleObject( *cond, INFINITE); \
}
#define COND_RESET( cond, mutex ) \
	ResetEvent(*cond)
#define COND_SLEEP_ACTION(cond, mutex, action) \
  ResetEvent( *cond ); \
action; \
WaitForSingleObject( *cond, INFINITE)
#define SIGNAL(cond, mutex, action) \
  action; \
SetEvent( *cond )
#define COND_BROADCAST(cond) \
  PulseEvent(*cond)
#define COND_SIGNAL(cond) \
  SetEvent(*cond)


/* ********* *
 * SEMAPHORE *
 * ********* */
/* Typedef */
#define SEMAPHORE_T HANDLE
/* Init */
#define SEMAPHORE_INIT(sem) \
  *sem = CreateSemaphore( \
      NULL, \
      0, \
      1024, \
      NULL ) 
/* Destroy */
#define SEMAPHORE_DESTROY(sem) 
/* Functions */
#define SEMAPHORE_WAIT(sem) \
  WaitForSingleObject(sem, INFINITE)
#define SEMAPHORE_POST(sem) \
  ReleaseSemaphore(sem, 1, NULL)


/* ******* *
 * RW_LOCK *
 * ******* */
/* Typedef */
#define RWLOCK_T mc_rwlock_t
/* Init */
#define RWLOCK_INIT(rwlock) \
  mc_rwlock_init(rwlock)
/* Destroy */
#define RWLOCK_DESTROY(rwlock) mc_rwlock_destroy(rwlock)
/* Functions */
#define RWLOCK_RDLOCK(rwlock) \
  mc_rwlock_rdlock(rwlock)
#define RWLOCK_RDUNLOCK(rwlock) \
  mc_rwlock_rdunlock(rwlock)
#define RWLOCK_WRLOCK(rwlock) \
  mc_rwlock_wrlock(rwlock)
#define RWLOCK_WRUNLOCK(rwlock) \
  mc_rwlock_wrunlock(rwlock)


/* ***************** *
 * SYSTEM QUEUE SYNC *
 * ***************** */
#define SLEEP_QUEUE( queue )                                    \
  ResetEvent( *(queue->touched_signal) );                        \
WaitForSingleObject( *(queue->touched_signal), INFINITE )
#define WAKE_QUEUE(queue, action)               \
  action;                             \
  SetEvent( *(queue->cond))
#define SLEEP_RESET( queue ) 

#endif /* End unix/windows macros */



/* * * * * * * * * * * * * * *
 * M I S C E L L A N E O U S *
 * * * * * * * * * * * * * * */
/* Check to see if a data structure is NULL: Perform 'action' if it is null */
#define CHECK_NULL( var, action )                                  \
  if ( var == NULL ) {                                        \
    fprintf(stderr, "Pointer var is null: expected otherwise.\n");    \
    fprintf(stderr, "Error occured at %s:%d", __FILE__, __LINE__);    \
    action; \
  }

#define WARN( message ) \
  fprintf(stderr, "\nWARNING: "); \
  fprintf(stderr, message ); \
  fprintf(stderr, " %s:%d\n", __FILE__, __LINE__ )

/* Macro for determining size of a CH data type */
/* Arguments:
 *  type: CH_Type_t type
 *  size: int
 *  */
#define CH_DATATYPE_SIZE(type, size)                            \
  switch(type) {                                              \
    case CH_CHARTYPE:                                       \
      size = sizeof(char);                                \
    break;                                              \
    case CH_INTTYPE:                                        \
      size = sizeof(int);                                 \
    break;                                              \
    case CH_UINTTYPE:                                       \
      size = sizeof(unsigned int);                        \
    break;                                              \
    case CH_SHORTTYPE:                                      \
      size = sizeof(short);                               \
    break;                                              \
    case CH_USHORTTYPE:                                     \
      size = sizeof(unsigned short);                      \
    break;                                              \
    case CH_FLOATTYPE:                                      \
      size = sizeof(float);                               \
    break;                                              \
    case CH_DOUBLETYPE:                                     \
      size = sizeof(double);                              \
    break;                                              \
    default:                                                \
      fprintf(stderr, "Unknown data type: %d at %s:%d",   \
        type, __FILE__, __LINE__);                  \
      size=0;                                             \
  }

/* Macro for converting a CH_Type_t into a string */
/* Argument 'string' must be an allocated array of characters. */
#define CH_DATATYPE_STRING(type, string)                    \
  switch(type) {                                          \
    case CH_CHARTYPE:                                   \
      strcpy(string, "char");                         \
    break;                                          \
    case CH_INTTYPE:                                    \
      strcpy(string, "int");                          \
    break;                                          \
    case CH_UINTTYPE:                                   \
      strcpy(string, "unsigned int");                 \
    break;                                          \
    case CH_SHORTTYPE:                                  \
      strcpy(string, "short");                        \
    break;                                          \
    case CH_USHORTTYPE:                                 \
      strcpy(string, "unsigned short");               \
    break;                                          \
    case CH_FLOATTYPE:                                  \
      strcpy(string, "float");                        \
    break;                                          \
    case CH_DOUBLETYPE:                                 \
      strcpy(string, "double");                       \
    break;                                          \
    default:                                            \
      fprintf(stderr,                                 \
      "Unsupported data type: %d %s:%d\n",    \
      type, __FILE__, __LINE__ );             \
  }

/* CH_DATATYPE_VALUE_STRING
 * A Macro for converting data pointed to by a pointer into 
 * a string. */
#define CH_DATATYPE_VALUE_STRING(type, string, p) \
  switch(type) {                                      \
    case CH_CHARTYPE:                                 \
      sprintf(string, "%c", *((char*)p));             \
      break;                                          \
    case CH_INTTYPE:                                \
      sprintf(string, "%d", *((int*)p));          \
    break;                                      \
    case CH_UINTTYPE:                               \
      sprintf(string, "%d", *((unsigned int*)p)); \
    break;                                      \
    case CH_SHORTTYPE:                              \
      sprintf(string, "%d", *((short*)p));         \
    break;                                      \
    case CH_USHORTTYPE:                             \
      sprintf(string, "%d", *((unsigned short*)p)); \
    break;                                      \
    case CH_FLOATTYPE:                              \
      sprintf(string, "%f", *((float*)p));        \
    break;                                      \
    case CH_DOUBLETYPE:                             \
      sprintf(string, "%f", *((double*)p));       \
    break;                                      \
    default:                                        \
      fprintf(stderr,                             \
      "Unsupported data type: %d %s:%d\n",    \
      type, __FILE__, __LINE__);          \
  }

#define CH_STRING_DATATYPE(string, type) \
  if (!strcmp(string, "int")) {                       \
    type = CH_INTTYPE;                              \
  } else if (!strcmp(string, "float")) {              \
    type = CH_FLOATTYPE;                            \
  } else if (!strcmp(string, "double")) {             \
    type = CH_DOUBLETYPE;                           \
  } else if (!strcmp(string, "unsigned int")) {       \
    type = CH_UINTTYPE;                             \
  } else if (!strcmp(string, "short")) {              \
    type = CH_SHORTTYPE;                            \
  } else if (!strcmp(string, "unsigned short")) {     \
    type = CH_USHORTTYPE;                           \
  } else if (!strcmp(string, "char")) {             \
    type = CH_CHARTYPE;                             \
  } else {                                            \
    fprintf(stderr,                                 \
        "Unsupported data type: %d %s:%d\n",    \
        type, __FILE__, __LINE__ );             \
  }

#ifndef _WIN32
#define CH_DATATYPE_STR_TO_VAL(type, string, val) \
  switch (type) { \
    case CH_INTTYPE: \
                     *(int*)val = atoi(string); \
    break; \
    case CH_UINTTYPE: \
                      *(unsigned int*)val = atoi(string); \
    break; \
    case CH_SHORTTYPE: \
                       *(short*)val = (short)atoi(string); /*FIXME*/ \
    break; \
    case CH_USHORTTYPE: \
                        *(unsigned short*)val = (unsigned short)atoi(string); /*FIXME*/ \
    break; \
    case CH_FLOATTYPE: \
                       *(float*)val = strtof(string, NULL); \
    break; \
    case CH_DOUBLETYPE: \
                        *(double*)val = strtod(string, NULL); \
    break; \
    default: \
             fprintf(stderr, \
                 "Unsupported data type: %d %s:%d\n", \
                 type, __FILE__, __LINE__ ); \
  }
#else
#define CH_DATATYPE_STR_TO_VAL(type, string, val) \
  switch (type) { \
    case CH_INTTYPE: \
                     *(int*)val = atoi(string); \
    break; \
    case CH_UINTTYPE: \
                      *(unsigned int*)val = atoi(string); \
    break; \
    case CH_SHORTTYPE: \
                       *(short*)val = (short)atoi(string); /*FIXME*/ \
    break; \
    case CH_USHORTTYPE: \
                        *(unsigned short*)val = (unsigned short)atoi(string); /*FIXME*/ \
    break; \
    case CH_FLOATTYPE: \
                       *(float*)val = (double)strtod(string, NULL); \
    break; \
    case CH_DOUBLETYPE: \
                        *(double*)val = strtod(string, NULL); \
    break; \
    default: \
             fprintf(stderr, \
                 "Unsupported data type: %d %s:%d\n", \
                 type, __FILE__, __LINE__ ); \
  }
#endif


#endif
