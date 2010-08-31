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

#ifndef _AGENT_DATASPACE_H_
#define _AGENT_DATASPACE_H_
#include <mxml.h>
#include "agent_task.h"


typedef struct agent_datastate_s
{
  char** agent_code_ids; /* Stores all of the agent code ids */
  char** agent_codes; /* Stores all of the agent code blocks */
  char* agent_code; /* This points to the code block to be executed. */

  /* an array of tasks */
  agent_task_p *tasks;

  /* xml Parsing attributes */
  mxml_node_t *xml_agent_root;
  mxml_node_t *xml_root;

  /* place in the task list that corresponds to current task */
  int task_progress;
  int return_data;

  /* size variables */
  int number_of_tasks;

  /* execution flags */
  int persistent;
  int init_agent_status;

	/* The progress modifier is a variable that holds an amount to vary the
	progress by at the end of the agents execution. This is necessary because
	some cleanup routines after an agent finished rely on an accurate 
	"progress" variable. This means that the progress variable cannot be 
	changed during an agent's execution. During an agent's execution, only
	the progress_modifier variable should be modified. */
	int progress_modifier;
} agent_datastate_t;
typedef agent_datastate_t* agent_datastate_p;

agent_datastate_p
agent_datastate_Copy(const agent_datastate_p datastate);

agent_datastate_p
agent_datastate_New(void);

int 
agent_datastate_Destroy( agent_datastate_p agent_datastate );
#endif
