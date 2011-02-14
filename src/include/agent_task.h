/* SVN FILE INFO
 * $Revision: 279 $ : Last Committed Revision
 * $Date: 2009-04-03 13:48:18 -0700 (Fri, 03 Apr 2009) $ : Last Committed Date */
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

#ifndef _AGENT_TASK_H_
#define _AGENT_TASK_H_
#include "interpreter_variable_data.h"
#include "data_structures.h"

typedef struct agent_task_s{
 
  int number_of_elements;
  int size_of_element_array;
  int persistent;
  int init_agent_status;

  char* var_name;
  char* server_name;
  char* code_id;

  /* data elements */
  interpreter_variable_data_t* agent_return_data;
  struct agent_variable_list_s* agent_variable_list;
  char** saved_variables;
  int num_saved_variables;

  struct agent_file_list_s* agent_file_list;

} agent_task_t;

typedef agent_task_t* agent_task_p;

agent_task_p
agent_task_New(void);

agent_task_p 
agent_task_Copy(agent_task_p task);

int
agent_task_Destroy(agent_task_p agent_task);
#endif
