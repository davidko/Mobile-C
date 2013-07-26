/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */
/* This is an implementation of a dynamically growing string. */
#ifndef _DYNSTRING_H_
#define _DYNSTRING_H_

#define COMPOSE_BLOCKSIZE 1024
typedef struct dynstring_s {
  int len; /* Length of current string being held */
  int size; /* Current allocated size of buffer */
  char* message;
} dynstring_t;
typedef dynstring_t* dynstring_p;

dynstring_t* dynstring_New(void);

int dynstring_Append(dynstring_t* msg, const char* str);

int dynstring_Destroy(dynstring_t* dynstring);

#endif
