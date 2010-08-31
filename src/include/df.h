/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
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

#ifndef _DF_H_
#define _DF_H_
#include "../mc_list/list.h"
#include "../include/macros.h"
#ifdef _WIN32
#include <windows.h>
#endif

struct mc_platform_s;

/* Command Indices */
enum df_request_list_index_e{
#define REQUEST(name, string, description)     REQUEST_##name, 
#include "df_request.x.h"
#undef REQUEST
    REQUEST_COUNT
};

enum service_types_e {
    ZERO,
    MISC,
    INSERT,
    SOME,
    TYPES,
    HERE
};

STRUCT(df_request_list_node,

    MUTEX_T *lock;
    COND_T *cond;
    const char* command;
    void* data;
    int data_size;
    )

STRUCT( df_request_list,

    MUTEX_T *lock;
    COND_T *cond;
    int size;
    list_p request_list;
    )

STRUCT( df_search_results,

    char **agent_names;
    char **service_names;
    int *agent_ids;
    int num_results;
    )

STRUCT( df_request_search,
    MUTEX_T *lock;
    COND_T *cond;
    char *search_string;
    df_search_results_p search_results;
    )

STRUCT( df_node,

    MUTEX_T *lock;

    int agent_id;
    char *agent_name;

    int num_services;
    char **service_names;
    enum service_types_e service_types;
    )

STRUCT( df,
    struct mc_platform_s* mc_platform;
    MUTEX_T *lock;
    COND_T *cond;
    list_p service_list;
    df_request_list_p request_list;
    
    int num_entries;
    int waiting; /* Flag indicating if thread is waiting for activity */
    MUTEX_T *waiting_lock;
    COND_T *waiting_cond;
    THREAD_T thread;
    )

STRUCT( df_deregister,

    int agent_id;
    char *service_name;
    )


/* df functions */
int 
df_Add(struct df_s* df, struct df_node_s * node);

int 
df_AddRequest(struct df_s* df, struct df_request_list_node_s* node);

int 
df_Destroy(df_p df);

df_p 
df_Initialize(struct mc_platform_s* mc_platform);

int df_ProcessRequest(
        struct mc_platform_s* global
        );

int 
df_SearchForService(
        df_p df,
        const char* searchstring, 
        char*** agent_names,
        char*** service_names,
        int** agent_ids,
        int* num_entries
);

void
df_Start(struct mc_platform_s* mc_platform);

/* df_node functions */
  int
df_node_Destroy(df_node_p df_node);

/* df_request_list_node functions */
int 
df_request_list_node_Destroy(df_request_list_node_p node);

df_request_list_node_p 
df_request_list_node_New(void);

/* df_request_list functions */
int
df_request_list_Destroy(df_request_list_p df_request_list);

df_request_list_p 
df_request_list_New(void);

df_request_list_node_p
df_request_list_Pop(df_request_list_p requests);

/* df_request_search functions */

df_request_search_p
df_request_search_New(void);

int 
df_request_search_Destroy(df_request_search_p node);

#ifndef _WIN32
void* df_Thread(void* arg);
#else
DWORD WINAPI df_Thread( LPVOID arg );
#endif

/* Request Handler Function Prototypes */
#define REQUEST(name, string, description)  \
    int request_handler_##name (struct mc_platform_s *global, void* data);
#include "df_request.x.h"
#undef REQUEST


#endif /* AP_DF_H */
