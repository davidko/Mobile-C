/* SVN FILE INFO
 * $Revision: 529 $ : Last Committed Revision
 * $Date: 2010-06-16 16:00:50 -0700 (Wed, 16 Jun 2010) $ : Last Committed Date */
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

/* Filename: df.c */

#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#include "config.h"
#else
#include "winconfig.h"
#endif
#include "include/mc_platform.h"
#include "include/df.h"

/* df functions */
  int 
df_Add(struct df_s* df, struct df_node_s* node)
{
  int err;
  ListWRLock(df->service_list);
  err = ListAdd(df->service_list, (void*) node);
  if (err == MC_SUCCESS) 
    df->num_entries++;
  ListWRUnlock(df->service_list);
  return err;
}

  int 
df_AddRequest(struct df_s* df, struct df_request_list_node_s* node)
{
  int err;

  ListWRLock(df->request_list);
  err = ListAdd(
      df->request_list,
      (void*)node );
  ListWRUnlock(df->request_list);
  return err;
}

  int
df_Destroy(df_p df)
{
  df_node_p df_node;
  MUTEX_LOCK(df->lock);
  ListWRLock(df->service_list);
  while ( (df_node = (df_node_p)ListPop(df->service_list)) != NULL) {
    df_node_Destroy(df_node);
  }
  ListWRUnlock(df->service_list);
  ListTerminate(df->service_list);
  df_request_list_Destroy(df->request_list);
  MUTEX_DESTROY(df->lock);
  COND_DESTROY(df->cond);
  free(df->lock);
  free(df->cond);
  free(df->waiting_lock);
  free(df->waiting_cond);
  free(df);
  return MC_SUCCESS;
}

  df_p 
df_Initialize(mc_platform_p mc_platform)
{
  df_p df;
  df = (df_p)malloc(sizeof(df_t)); 

  df->mc_platform = mc_platform;

  /* Mutex Init */
  df->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(df->lock);

  /* Cond Init */
  df->cond = (COND_T*)malloc(sizeof(COND_T));
  COND_INIT(df->cond);

  /* Initialize the Service List */
  df->service_list = ListInitialize();

  /* Initialize the Request List */
  df->request_list = ListInitialize();

  df->num_entries = 0;
  df->waiting = 0;
  df->waiting_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(df->waiting_lock);
  df->waiting_cond = (COND_T*)malloc(sizeof(COND_T));
  COND_INIT(df->waiting_cond);
  return df;
}

int 
df_ProcessRequest(
    struct mc_platform_s* global
    )
{
  int return_code;
  int handler_code;
  enum df_request_list_index_e request_code;
  df_request_list_node_t *request;
  ListWRLock(global->df->request_list);
  if ( 
      (
       request = (df_request_list_node_t*)ListPop( global->df->request_list ) 
      ) == NULL
     )
  {
    ListWRUnlock(global->df->request_list);
    printf("Empty.\n");
    return MC_ERR_EMPTY;
  }
  ListWRUnlock(global->df->request_list);

  /* Process the request, call the correct handler */
#define REQUEST(name, string, description) \
  if ( !strcmp(request->command, string ) ) { \
    return_code = MC_SUCCESS; \
    handler_code = request_handler_##name(  \
        global, \
        request->data ); \
    request_code = REQUEST_##name; \
  } else
#include "include/df_request.x.h"
#undef REQUEST
  { 
    fprintf(stderr, "No such register command: %s. %s:%d\n",
        request->command,
        __FILE__,
        __LINE__ );
    return MC_ERR_INVALID;
  }

  return handler_code;
}

/* returns error code. */
/* Third argument is a return argument: array of matching agent names. */
/* Fourth argument is a return argument: Number of matching service names. */
/* Fifth argument is a return argument: Array of matching agent IDs */
int df_SearchForService(
    df_p df,
    const char* searchstring, 
    char*** agent_names,
    char*** service_names,
    int** agent_ids,
    int* num_entries)
{
  int i=0;
  int j=0;
  int found_entries=0;
  listNode_p list_node;
  df_node_p df_node;

  /* check for empty df */
  if(df->num_entries < 1) {
    *num_entries = 0;
    return MC_ERR_NOT_FOUND;
  }

  /* Lock the list mutex to prevent simultaneous searches */
  MUTEX_LOCK(df->lock);
   /* We'll run the following loop twice. The first time to find
   * the number of elements we need to allocate, the second
   * to actually assign some values. */
  list_node = df->service_list->listhead;
  while (list_node != NULL) {
    /* Lock the df_node to prevent deletions/changes from happening
     * mid-search */
    MUTEX_LOCK( ((df_node_p)(list_node->node_data))->lock );
    df_node = (df_node_p)list_node->node_data;
    for(i = 0; i < df_node->num_services; i++) {
      if ( strstr(df_node->service_names[i], searchstring) ) {
        /* Found an entry. */
        found_entries++;
      }
    }
    MUTEX_UNLOCK(df_node->lock);
    list_node = list_node->next;
  }
  if (found_entries == 0) {
    /* Nothing was found. Unlock mutexes and return */
    MUTEX_UNLOCK(df->lock);
    *num_entries = 0;
    return MC_ERR_NOT_FOUND;
  }

  /* Allocate return arguments */
  *agent_names   = (char**)malloc(sizeof(char*) * found_entries);
  *service_names = (char**)malloc(sizeof(char*) * found_entries);
  *agent_ids = (int*)malloc(sizeof(int) * found_entries);
  /* Re run the loop */
  list_node = df->service_list->listhead;
  while (list_node != NULL) {
    /* Lock the df_node to prevent deletions/changes from happening
     * mid-search */
    MUTEX_LOCK( ((df_node_p)(list_node->node_data))->lock );
    df_node = (df_node_p)list_node->node_data;
    for(i = 0; i < df_node->num_services; i++) {
      if ( strstr(df_node->service_names[i], searchstring) ) {
        /* Found an entry. */
        /* Copy name into return argument */
        (*agent_names)[j] = (char*)malloc(
            sizeof(char) * (strlen(df_node->agent_name)+1)
            );
        strcpy((*agent_names)[j], df_node->agent_name);
        /* Copy service name into return arg */
        (*service_names)[j] = (char*)malloc(
            sizeof(char) * (strlen((df_node->service_names)[i])+1)
            );
        strcpy((*service_names)[j], (df_node->service_names)[i]);
        /* Copy agent id into return arg */
        (*agent_ids)[j] = df_node->agent_id;
        j++;
      }
    }
    MUTEX_UNLOCK(df_node->lock);
    list_node = list_node->next;
  }
  MUTEX_UNLOCK(df->lock);
  *num_entries = found_entries;
  return MC_SUCCESS;
}

  void 
df_Start(mc_platform_p mc_platform)
{
#ifndef _WIN32
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  if (mc_platform->stack_size[MC_THREAD_DF] != -1) {
  pthread_attr_setstacksize
    (
     &attr, 
     mc_platform->stack_size[MC_THREAD_DF]
    );
  }
#else
  int stack_size;
  if (mc_platform->stack_size[MC_THREAD_DF] < 1) {
    /* In windows, 0 is default, not min */
    stack_size = mc_platform->stack_size[MC_THREAD_DF]+1; 
  } else {
    stack_size = mc_platform->stack_size[MC_THREAD_DF];
  }
#endif
  THREAD_CREATE
    (
     &mc_platform->df->thread,
     df_Thread,
     mc_platform
    );
}

/* df_request_list_node functions */
int 
df_request_list_node_Destroy(df_request_list_node_p node)
{
  MUTEX_DESTROY(node->lock);
  free(node->lock);
  COND_DESTROY(node->cond);
  free(node->cond);
  free(node);
  return MC_SUCCESS;
}

df_request_list_node_p 
df_request_list_node_New(void)
{
  df_request_list_node_p node;
  node = (df_request_list_node_p)
    malloc(sizeof(df_request_list_node_t));
  CHECK_NULL(node, return NULL;);
  node->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(node->lock);
  node->cond = (COND_T*)malloc(sizeof(COND_T));
  COND_INIT(node->cond);
  node->data_size = 0;
  node->command = NULL;
  node->data = NULL;
  return node;
}

/* df_request_list functions */
  int
df_request_list_Destroy(list_t* df_request_list)
{
  df_request_list_node_p node;
  ListWRLock(df_request_list);
  while 
    ( 
     (
      node = 
      (df_request_list_node_p)ListPop
      (
       df_request_list
      )
     ) != NULL
    )
    {
      df_request_list_node_Destroy(node);
    }
  ListWRUnlock(df_request_list);
  ListTerminate(df_request_list);
  return MC_SUCCESS;
}

/*df_request_search functions */
df_request_search_p 
df_request_search_New(void)
{
  df_request_search_p search;
  search = (df_request_search_p)malloc(sizeof(df_request_search_t));
  CHECK_NULL(search, return NULL;);
  search->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  CHECK_NULL(search->lock, return NULL;);
  search->cond = (COND_T*)malloc(sizeof(COND_T));
  CHECK_NULL(search->cond, return NULL;);
  MUTEX_INIT(search->lock);
  COND_INIT(search->cond);
  return search;
}

int 
df_request_search_Destroy(df_request_search_p node)
{
  MUTEX_DESTROY(node->lock);
  free(node->lock);
  COND_DESTROY(node->cond);
  free(node->cond);

  free(node);
  return MC_SUCCESS;
}


  int
df_node_Destroy(df_node_p df_node)
{
  int i;
  MUTEX_LOCK(df_node->lock);
  free(df_node->agent_name);
  for(i = 0; i < df_node->num_services; i++) {
    free(df_node->service_names[i]);
  }
  free(df_node->service_names);
  free(df_node);
  return MC_SUCCESS;
}

#ifndef _WIN32
void* df_Thread(void* arg)
#else
DWORD WINAPI df_Thread( LPVOID arg )
#endif
{
  int err_code;
  mc_platform_p global = (mc_platform_p)arg;
  while(1) {
    ListRDLock(global->df->request_list);
    MUTEX_LOCK(global->quit_lock);
    while 
      (
       (global->df->request_list->size <= 0) &&
       !global->quit
      ) 
      {
        MUTEX_UNLOCK(global->quit_lock);
        /* Set waiting flag on */
        MUTEX_LOCK(global->df->waiting_lock);
        global->df->waiting = 1;
        COND_BROADCAST(global->df->waiting_cond);
        MUTEX_UNLOCK(global->df->waiting_lock);
        /* Wait for activity */
        ListRDWait(global->df->request_list);
        MUTEX_LOCK(global->quit_lock);
      }
    /* Set waiting flag off */
    MUTEX_LOCK(global->df->waiting_lock);
    global->df->waiting = 0;
    COND_BROADCAST(global->df->waiting_cond);
    MUTEX_UNLOCK(global->df->waiting_lock);
    if 
      (
       global->df->request_list->size == 0 && global->quit
      ) {
        MUTEX_UNLOCK(global->quit_lock);
        ListRDUnlock(global->df->request_list);
        THREAD_EXIT();
      }
    MUTEX_UNLOCK(global->quit_lock);
    ListRDUnlock(global->df->request_list);
    if ( 
        (err_code = df_ProcessRequest(
                                      global
                                     )) != MC_SUCCESS ) {
      fprintf(stderr,
          "Error Code %d: %s:%d\n",
          err_code,
          __FILE__,
          __LINE__ );
    }
  }
  return 0;
}

/* Request Handlers */

int request_handler_REGISTER(struct mc_platform_s* global, void* data)
{
  /* Insert new struct into DF */
  return df_Add(global->df, (struct df_node_s*)data);
}

int request_handler_SEARCH(struct mc_platform_s* global, void* data)
{
  df_request_search_p search;
  search = (df_request_search_p)data;
  df_SearchForService(
      global->df,
      search->search_string,
      &search->search_results->agent_names,
      &search->search_results->service_names,
      &search->search_results->agent_ids,
      &search->search_results->num_results
      );
  SIGNAL(search->cond,
      search->lock,
      NULL
      );
  return MC_SUCCESS;
}

int request_handler_SUBSCRIBE(struct mc_platform_s* global, void* data)
{
  return 0;
}

int request_handler_DEREGISTER(struct mc_platform_s* global, void* data)
{
  int i, j;
  df_deregister_p deregister;
  listNode_p node;
  df_node_p df_node;
  int num_deregistered=0;
  df_p df = global->df;
  deregister = (df_deregister_p)data;
  /* Find and remove all entries matching search terms. */
  MUTEX_LOCK(df->lock);
  if (df->service_list->listhead== NULL) {
    MUTEX_UNLOCK(df->lock);
    return 0;
  }
  node = df->service_list->listhead;
  while (node != NULL) {
    df_node = (df_node_p)node->node_data;
    if (df_node->agent_id == deregister->agent_id) {
      for (i = 0; i < df_node->num_services; i++) {
        if (!strcmp(
              df_node->service_names[i],
              deregister->service_name 
              ) 
           ) 
        {
          free(df_node->service_names[i]);
          for (j = i; j < df_node->num_services-1; j++) {
            df_node->service_names[j] = df_node->service_names[j+1];
          }
          df_node->num_services--;
          num_deregistered++;

          if (df_node->num_services == 0) {
            /* Just get rid of the entire node altogether */
            /* FIXME */
          }
        }
      }
    }
    node = node->next;
  }
  MUTEX_UNLOCK(df->lock);
  return MC_SUCCESS;
}
