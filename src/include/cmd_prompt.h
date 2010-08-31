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

#ifndef _CMD_PROMPT_H_
#define _CMD_PROMPT_H_
#define CMDLINE_SIZE 80
#include "macros.h"

struct mc_platform_s;
typedef struct cmd_prompt_s {
  THREAD_T thread;
} cmd_prompt_t;
typedef cmd_prompt_t* cmd_prompt_p;

typedef struct command_s {
  int index;
  int num_args;
  char **args;
} command_t;
typedef command_t* command_p;

int
cmd_prompt_Destroy(cmd_prompt_p cmd_prompt);

cmd_prompt_p
cmd_prompt_Initialize(struct mc_platform_s* mc_platform);

void
cmd_prompt_Start( struct mc_platform_s* mc_platform );

int split_string(char ***args, const char *buf);
int process_command(command_t *cmd);
#ifndef _WIN32
void*
cmd_prompt_Thread(void* arg);
#else
DWORD WINAPI
cmd_prompt_Thread(LPVOID arg);
#endif
int exec_command(command_t cmd, struct mc_platform_s* global);
int dealloc_command(command_t *cmd);
#endif
