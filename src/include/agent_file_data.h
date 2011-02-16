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

#ifndef _AGENT_FILE_DATA_H_
#define _AGENT_FILE_DATA_H_

typedef struct agent_file_data_s{
  /* Data is stored in Base64 format via libb64 */
  char* name;
  char* data; /* Should be a null terminated string in MIME format*/
} agent_file_data_t;

typedef struct agent_file_data_s* agent_file_data_p;

agent_file_data_p
agent_file_data_New(void);

agent_file_data_p
agent_file_data_NewWithData(const char* name, const char* data);

agent_file_data_p
agent_file_data_Copy(agent_file_data_p src);

agent_file_data_p
agent_file_data_InitializeFromFilename(const char* filename);

agent_file_data_p
agent_file_data_Encode(const char* name, void* data, int size);

int
agent_file_data_SaveToFile(const char* filename, agent_file_data_p afd);

int agent_file_data_Destroy(agent_file_data_p agent_file_data);

#endif
