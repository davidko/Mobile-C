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

#ifndef _AMS_H_
#define _AMS_H_
#include "macros.h"
struct mc_platform_s;

STRUCT
( ams,
  struct mc_platform_s* mc_platform;
  MUTEX_T *runflag_lock;
  COND_T *runflag_cond;
  int run;

  /* waiting flag */
  int waiting;
  MUTEX_T* waiting_lock;
  COND_T* waiting_cond;

  THREAD_T thread;
  )

ams_p 
ams_Initialize( struct mc_platform_s* _mc_platform );

int 
ams_Destroy(ams_p ams);

int 
ams_RemoveFinishedAgents(ams_p ams);

int 
ams_ManageAgentList(ams_p ams);

void 
ams_Print(ams_p ams);

  void
ams_Start(struct mc_platform_s* mc_platform);

#ifndef _WIN32
void*
ams_Thread(void* arg);
#else
DWORD WINAPI
ams_Thread( LPVOID arg );
#endif


#endif
