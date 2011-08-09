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

#ifndef _BARRIER_H_
#define _BARRIER_H_

#include"macros.h"
#include"../mc_list/list.h"
#include"mc_rwlock.h"

typedef struct barrier_node_s {
    MUTEX_T* lock;
    COND_T* cond;
    int id;
    int num_registered;
    int num_waiting;
} barrier_node_t;
typedef barrier_node_t* barrier_node_p;

typedef struct barrier_queue_s {
    RWLOCK_T* lock;
    
    list_p list;
    int size;
} barrier_queue_t;
typedef barrier_queue_t* barrier_queue_p;

/* Node Funcs */

barrier_node_p
barrier_node_Initialize(int id, int num_registered);

int 
barrier_node_Destroy(barrier_node_p node);

/* List Funcs */

int barrier_node_CmpID (const void* key, void* element);
#endif
