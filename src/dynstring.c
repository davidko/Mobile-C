/* SVN FILE INFO
 * $Revision: 402 $ : Last Committed Revision
 * $Date: 2009-08-23 13:44:39 -0700 (Sun, 23 Aug 2009) $ : Last Committed Date */
#ifndef _WIN32
#include "config.h"
#else
#include "winconfig.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/dynstring.h"
dynstring_t* dynstring_New(void)
{
  dynstring_t* message;
  message = (dynstring_t*)malloc(sizeof(dynstring_t));
  message->len = 0;
  message->size = COMPOSE_BLOCKSIZE;
  message->message = (char*)malloc(sizeof(char) * message->size);
  if (message->message == NULL) {
    fprintf(stderr, "Memory error. %s:%d\n", __FILE__, __LINE__);
    exit(0);
  }
  memset(message->message, 0, message->size);
  
  return message;
}

int dynstring_Append(dynstring_t* msg, char* str)
{
  char* tmp;
  /* Check to see if we need to reallocate the message buffer */
  while ( (strlen(str)+4) > (msg->size - msg->len) ) {
    /* Increase the size of the buffer by COMPOSE_BLOCKSIZE */
    tmp = (char*)malloc(
        sizeof(char) * 
        (msg->size + COMPOSE_BLOCKSIZE)
        );
    if (tmp == NULL) {
      fprintf(stderr, "Memory Error. %s:%d\n", __FILE__, __LINE__);
      exit(0);
    }
    msg->size = msg->size + COMPOSE_BLOCKSIZE;
		memset(tmp, 0, msg->size);
    strcpy(tmp, msg->message);
    free(msg->message);
    msg->message = tmp;
  }
  strcat(msg->message, str);
  msg->len += strlen(str);

  return 0;
}

int dynstring_Destroy(dynstring_t* dynstring)
{
  free(dynstring->message);
  free(dynstring);
  return 0;
}
