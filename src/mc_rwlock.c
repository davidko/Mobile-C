/* SVN FILE INFO
 * $Revision: 316 $ : Last Committed Revision
 * $Date: 2009-04-29 16:40:54 -0700 (Wed, 29 Apr 2009) $ : Last Committed Date */
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
#ifndef _WIN32
#include "config.h"
#else
#include "winconfig.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "include/macros.h"
#include "include/mc_error.h"
#include "include/mc_rwlock.h"

int mc_rwlock_init(mc_rwlock_p rwlock)
{
    /* Init values */
    rwlock->num_readers = 0;
    rwlock->write_flag = 0;
    rwlock->write_request = 0;

    /* Alloc sync */
    rwlock->lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
    CHECK_NULL(rwlock->lock, return MC_ERR_MEMORY;);
    rwlock->cond = (COND_T*)malloc(sizeof(COND_T));
    CHECK_NULL(rwlock->cond, return MC_ERR_MEMORY;);

    /* Init sync */
    MUTEX_INIT(rwlock->lock);
    COND_INIT(rwlock->cond);

    return 0;
}

int mc_rwlock_destroy(mc_rwlock_p rwlock)
{
    /* Destroy mutex/conds */
    MUTEX_DESTROY(rwlock->lock);
    COND_DESTROY(rwlock->cond);

    /* De-alloc */
    free(rwlock->lock);
    free(rwlock->cond);
    
    return 0;
}

int mc_rwlock_rdlock(mc_rwlock_p rwlock)
{
    MUTEX_LOCK(rwlock->lock);
    while (
            (rwlock->write_flag != 0) ||
            (rwlock->write_request != 0)
          ) 
    {
        COND_WAIT(rwlock->cond, rwlock->lock);
    }
    rwlock->num_readers++;
    MUTEX_UNLOCK(rwlock->lock);
    return 0;
}

int mc_rwlock_rdunlock(mc_rwlock_p rwlock)
{
    MUTEX_LOCK(rwlock->lock);
    if (rwlock->num_readers > 0) {
        rwlock->num_readers--;
    }
    if (rwlock->num_readers == 0) {
        COND_SIGNAL( rwlock->cond );
    }
    MUTEX_UNLOCK(rwlock->lock);
    return 0;
}

int mc_rwlock_wrlock(mc_rwlock_p rwlock)
{
    MUTEX_LOCK(rwlock->lock);
    rwlock->write_request++;

    while (
            (rwlock->num_readers != 0) || 
            (rwlock->write_flag != 0) 
          )
    {
        COND_WAIT(rwlock->cond, rwlock->lock);
    }

    rwlock->write_request--;
    rwlock->write_flag++;
    MUTEX_UNLOCK(rwlock->lock);
    return 0;
}

int mc_rwlock_wrunlock(mc_rwlock_p rwlock)
{
    MUTEX_LOCK(rwlock->lock);
    rwlock->write_flag = 0;
    COND_SIGNAL(rwlock->cond);
    MUTEX_UNLOCK(rwlock->lock);
    return 0;
}
