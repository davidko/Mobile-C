/* SVN FILE INFO
 * $Revision: 517 $ : Last Committed Revision
 * $Date: 2010-06-11 12:06:47 -0700 (Fri, 11 Jun 2010) $ : Last Committed Date */
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

#ifndef _XML_HELPER_H_
#define _XML_HELPER_H_
#include<mxml.h>

mxml_node_t *
xml_find_sibling(const mxml_node_t *node, const char *sibling_name);

char * 
xml_get_cdata(const mxml_node_t *node);

mxml_node_t *
xml_get_child(const mxml_node_t *node, const char *child_name, int descend);

mxml_node_t *
xml_get_deep_child(const mxml_node_t *parent, const char **child_path);

mxml_node_t *
xml_get_next_element(const mxml_node_t *node);

char * 
xml_get_text(const mxml_node_t *node);

char* 
xml_get_element_name(const mxml_node_t *node);

mxml_node_t *
xml_new_cdata(mxml_node_t* parent, const char* text);

const char* 
whitespace_cb( mxml_node_t *node, 
        int where ) ;

#endif

