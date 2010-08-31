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

/* These are xml helper functions.
 *
 * They were created to ease the switch from libxml2 to mxml.
 *
 * Written by David Ko <dko@ucdavis.edu> 2006 */
#ifndef _WIN32
#include "config.h"
#else
#include "winconfig.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mxml.h>
#include "include/xml_helper.h"
#include "include/macros.h"

mxml_node_t *
xml_find_sibling(const mxml_node_t *node, const char *sibling_name)
{
    if ( !strcmp(
                xml_get_element_name(node),
                sibling_name
                )
       ) {
        return (mxml_node_t *)node;
    } else {
        return mxmlFindElement( (mxml_node_t *)node,
                (mxml_node_t *)node->parent,
                sibling_name,
                NULL,
                NULL,
                MXML_NO_DESCEND );
    }
}

/* xml_get_cdata 
 * Expects an MXML_ELEMENT with CDATA (returns the data) or a 
 * MXML_TEXT node (returns the text). */
char * 
xml_get_cdata(const mxml_node_t *node) {
    char *cdata_str;
    char *tmp;
    char *buf;
    if (node == NULL) {
        return NULL;
    } else if ( node->type != MXML_ELEMENT) {
        return NULL;
    }
    /* The name of the node should begin with '![CDATA[' and end with ']]' */
    buf = xml_get_element_name(node);
    cdata_str = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
    tmp = strstr(buf, "![CDATA[");
    if (tmp == NULL) {
        return NULL;
    }
    tmp = tmp + strlen("![CDATA["); /* Now tmp points to the beginning of the string. */
    strcpy(cdata_str, tmp);
    for (tmp = cdata_str; *(tmp+2) != '\0'; tmp++) {
        if (*tmp == '\0') {
            return NULL;
        }
    }
    /* Last two characters must be ']]' */
    if ( strcmp(tmp, "]]") ) {
        return NULL;
    }
    CHECK_NULL(tmp, return NULL);
    *tmp = '\0';
    return cdata_str;
}

mxml_node_t *
xml_get_child(const mxml_node_t *node, const char *child_name, int descend)
{
    return mxmlFindElement( (mxml_node_t *)node,
            (mxml_node_t *)node,
            child_name,
            NULL,
            NULL,
            descend );
}

/* Function:  xml_get_deep_child
Arguments: parent: The parent node
child_path: The relative path to the child from the
parent. It is an array of strings terminated
by NULL.
Return Value: A valid xmlNodePtr to the child on success, NULL
on failure.
 */
    mxml_node_t *
xml_get_deep_child(const mxml_node_t *parent, const char **child_path)
{
    int i;
    mxml_node_t *node;
    node = (mxml_node_t *)parent;
    for (i=0; child_path[i] != NULL; i++) {
        if ( (node = xml_get_child(node, child_path[i], MXML_NO_DESCEND)) == NULL) {
            return NULL;
        }
    }
    return node;
}

mxml_node_t *
xml_get_next_element(const mxml_node_t *node)
{
    node = node->next;
    while (node != NULL) {
        if (node->type == MXML_ELEMENT) {
            break;
        }
        node = node->next;
    }
    return (mxml_node_t *)node;
}

/* Function: xml_get_text
   Given a node, it will malloc and return a character string with 
   the text of the node. For instance, for a node that looks like
   <node> Hello There <\node>
   xml_get_text(node_ptr) will return the string "Hello There". */

char * xml_get_text(const mxml_node_t *node)
{
    char *ret;
    char *tmp;
    mxml_node_t *it; /* iteration temp node */
    int len = 0;
    if (node->child) {
        node = node->child;
    } else {
        return NULL;
    }

    if (node->type == MXML_TEXT || node->type == MXML_ELEMENT) {
        /* First, find total string length. */
        it = (mxml_node_t *)node;
        while (it != NULL && 
                    (it->type == MXML_TEXT || it->type == MXML_ELEMENT)
              ){
            if (it->type == MXML_TEXT) {
                len += strlen(it->value.text.string);
                len++; /*Account for whitespace*/
                it = it->next;
            } else if (it->type == MXML_ELEMENT) {
                if ((tmp = strstr(it->value.element.name, "![CDATA["))){
                    len += strlen(tmp);
                    it = it->next;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    } else {
        return NULL;
    }
    ret = (char*)malloc( sizeof(char) * (len + 1));
    *ret = '\0';
    for ( 
            it = (mxml_node_t *)node; 
            it != NULL && (it->type == MXML_TEXT || it->type == MXML_ELEMENT); 
            it = it->next) {
        if (it->type == MXML_TEXT) {
            if (it->value.text.whitespace == 1) {
                strcat(ret, " ");
            } 
            strcat(ret, it->value.text.string);
        } else if (it->type == MXML_ELEMENT) {
            if ((tmp = xml_get_cdata(it))) {
/*                strcat(ret, " "); */
                strcat(ret, tmp);
                free(tmp);
            } else {
                break;
            }
        } else { /* Should never get here */
            CHECK_NULL( NULL, exit(0) );
        }
    }
    return ret;
}

char* xml_get_element_name(const mxml_node_t *node)
{
    if (node->type != MXML_ELEMENT) {
        return NULL;
    } else {
        return node->value.element.name;
    }
}

/* xml_new_cdata
 *
 * Creates a new cdata node under 'parent'. */
mxml_node_t *
xml_new_cdata(mxml_node_t* parent, const char* text) {
    char *tmp;
    int namelen;
    mxml_node_t * node;
    namelen = (strlen(text) + strlen("![CDATA[]]")+1) * sizeof(char);
    tmp = (char*)malloc(namelen);
    CHECK_NULL(tmp, exit(0) );
    *tmp = '\0';
    strcat(tmp, "![CDATA[");
    strcat(tmp, text);
    strcat(tmp, "]]");
    node = mxmlNewElement(
            parent,
            (const char*)tmp );
    free((char*)tmp); 
    return node;
}
/* This is a simple whitespace callback for mxml */
    const char* 
whitespace_cb( mxml_node_t *node, 
        int where ) 
{
    if (where == MXML_WS_BEFORE_OPEN || where == MXML_WS_AFTER_CLOSE) {
        return("\n");
    } else {
        return NULL;
    }
}
