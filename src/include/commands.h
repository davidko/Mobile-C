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

#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "macros.h"
#include "mc_platform.h"

/*Func Prototypes*/
#define COMMAND(name, cmd, desc)    int handler_##name (void *arg, mc_platform_p global);
#include "commands.x.h"
#undef COMMAND

typedef int (*cmd_handler_t) (void *, mc_platform_p global);

enum command_indices_e{
#define COMMAND(name, cmd, description)     COMMAND_##name ,
#include "commands.x.h"
#undef COMMAND
    COMMAND_COUNT
};

cmd_handler_t cmd_handlers[] = {
#define COMMAND(name, cmd, description)     &handler_##name , 
#include"commands.x.h"
#undef COMMAND
    NULL 
};

char *command_descriptions[] = {
#define COMMAND(name, cmd, description)     description , 
#include"commands.x.h"
#undef COMMAND
    NULL
};

char *command_cmds[] = {
#define COMMAND(name, cmd, description)     cmd ,
#include"commands.x.h"
#undef COMMAND
    NULL
};


#endif
