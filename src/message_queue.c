/* SVN FILE INFO
 * $Revision: 402 $ : Last Committed Revision
 * $Date: 2009-08-23 13:44:39 -0700 (Sun, 23 Aug 2009) $ : Last Committed Date */
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
#ifdef _WIN32
#include <windows.h>
#elif defined (HPUX)
#else
#include <pthread.h>
#endif
#include "include/mc_platform.h"

/**********************************************************************
 * Function Name : MessgQueueSendOutgoingMessg
 * purpose : parses throught the outoig messg list and sends the messages
 * returns : void
 *
 **********************************************************************/
void message_queue_SendOutgoing(
    struct mc_platform_s* mc_platform,
    list_t* mqueue
    )
{
    /* variables */
    int index = 0;
    message_p message;
    char* local_address;
    local_address = (char*)malloc
      (
       sizeof(char) * 
       (strlen(mc_platform->hostname)+10)
      );
    CHECK_NULL(local_address, exit(0));
    sprintf
      (
       local_address,
       "%s:%d",
       mc_platform->hostname,
       mc_platform->port
      );

    ListRDLock(mqueue);
    while ((message = (message_p)ListSearch(mqueue, index))) {
        if (strcmp(message->to_address, local_address)) {
            message_Send(mc_platform, message, mc_platform -> private_key);
            ListRDtoWR(mqueue);
            ListDelete(mqueue, index);
            ListWRtoRD(mqueue);
        } else {
            index++;
        }
    }
    ListRDUnlock(mqueue);

    free(local_address);
    return ;
}

