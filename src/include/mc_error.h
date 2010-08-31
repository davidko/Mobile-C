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

#ifndef _ERROR_CODE_E_
#define _ERROR_CODE_E_
/*
 * Error Codes 
 * NOTE - There is a copy of this enum in libmc.h
 * */
typedef enum error_code_e {
    MC_SUCCESS = 0,
    ERR,
    MC_ERR_CONNECT,
    MC_ERR_PARSE,
    MC_ERR_EMPTY,
    MC_ERR_INVALID,
    MC_ERR_INVALID_ARGS,
    MC_ERR_NOT_FOUND,
    MC_ERR_MEMORY,
    MC_ERR_SEND,
    MC_WARN_DUPLICATE
} error_code_t;

#endif
