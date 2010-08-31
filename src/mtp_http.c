/* SVN FILE INFO
 * $Revision: 529 $ : Last Committed Revision
 * $Date: 2010-06-16 16:00:50 -0700 (Wed, 16 Jun 2010) $ : Last Committed Date */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef _WIN32
#include <unistd.h>
#include <errno.h>
#include "config.h"
#else
#include "winconfig.h"
#endif
#include "include/connection.h"
#include "include/mtp_http.h"
#include "include/macros.h"
#include "include/mc_error.h"
#include "include/message.h"
#include "include/dynstring.h"

int
mtp_http_Destroy(mtp_http_p http)
{
  int i;
#define SAFE_FREE(elem) \
  if(elem) \
    free(elem)

  SAFE_FREE(http->http_version);
  SAFE_FREE(http->host);
  SAFE_FREE(http->return_code);
  SAFE_FREE(http->target);
  SAFE_FREE(http->date);
  SAFE_FREE(http->server);
  SAFE_FREE(http->accept_ranges);
  SAFE_FREE(http->content_length);
  SAFE_FREE(http->connection);
  SAFE_FREE(http->content_type);
  SAFE_FREE(http->user_agent);
  SAFE_FREE(http->cache_control);
  SAFE_FREE(http->mime_version);
  if(http->content != NULL) {
    for(i = 0; i < http->message_parts; i++) {
      SAFE_FREE(http->content[i].content_type);
      SAFE_FREE(http->content[i].data);
    }
  }
  SAFE_FREE(http->content);
  SAFE_FREE(http->boundary);
  SAFE_FREE(http->response_string);
  SAFE_FREE(http);
#undef SAFE_FREE
  return 0;
}

  mtp_http_p
mtp_http_New(void)
{
  mtp_http_p http;
  http = (mtp_http_p)malloc(sizeof(mtp_http_t));
  CHECK_NULL(http, exit(0););
  memset(http, 0, sizeof(mtp_http_t));
  http->content = NULL;
  return http;
}

int 
rece_de_msg(char *buffer, connection_p con, char *privatekey){

  FILE *fd;
  struct stat stbuf;
  int mode,i,j;
  /* temporary files */
  char *infile;   // = "en-msg"; // encrypted message: 
  char *outfile;  // = "msg";   // message
  char ch;
    
#ifndef _WIN32   
  int fd1;
  infile = (char *)malloc(sizeof(char)*20);         
  strcpy(infile, "en-msgXXXXXX");
  fd1 = mkstemp(infile);
  if (fd1 == -1) {
     fprintf(stderr, "Could not create temporary file:%s. %s:%d\n",
          infile, __FILE__, __LINE__ );
     exit(EXIT_FAILURE);
  }   
  close(fd1);

  outfile = (char *)malloc(sizeof(char)*20);
  strcpy(outfile, "enXXXXXX");
  fd1 = mkstemp(outfile);
  if (fd1 == -1) {
     fprintf(stderr, "Could not create temporary file:%s. %s:%d\n",
	outfile, __FILE__, __LINE__ );
     exit(EXIT_FAILURE);
  }
  close(fd1);
#else
  infile  = _tempnam(".", "msg");
  outfile = _tempnam(".", "en-msg");
#endif

  // Receive encrypted mesg and write in infile   
  if (receive_AES_en_MA(con->clientfd, &(con->nonce), infile, privatekey ) != 1){
     printf("Server: Fail to successfully encrypted msg \n");
	 if( remove(infile) )  printf("mtp_http.c : remove error 1");
     return -1;
  }
  
  // decrypte infile using aes key and put plaintext content in outfile
  mode = 1;  // Decryption mode
  // decrypt infile and write plaintext in outfile
  if (aes_en_de(mode, infile, outfile, con->AES_key, &(con->nonce), con->clientfd) != 1){
     printf("Server: AES Decryption failed \n");	 
     if( remove(infile) )  printf("mtp_http.c : remove error 2");
     if( remove(outfile) ) printf("mtp_http.c : remove error 3");
     return -1;
  } 
   
  if( (fd = fopen(outfile,"r")) == NULL ){
     printf("Cannot find %s file to read msg content", outfile);
     if( remove(infile) )  printf("mtp_http.c : remove error 4");
     if( remove(outfile) ) printf("mtp_http.c : remove error 5");
     return -1;	
  }
  stat(outfile, &stbuf);  
  // copy whole file into buffer
  // read output file except carriage return
  j=0;
  for(i=0; i<(int)stbuf.st_size; i++ ){
  //while( (ch = fgetc(fd)) != EOF ){
       ch = fgetc(fd);
       if(ch != '\r')
           buffer[j++]=ch;
       else
           buffer[j++]=' ';
  }

  fclose(fd);
  // after successfull getting mesg remove temporary files
  if( remove(infile) )  printf("mtp_http.c : remove error 6");
  if( remove(outfile) ) printf("mtp_http.c : remove error 7");
  return strlen(buffer);
}
int
mtp_http_InitializeFromConnection
(
 mtp_http_p http,
 connection_p connection,
 char *privatekey
 )
{
  int i=1;
  int n = 0;
  dynstring_t* message_string;
  char *buffer;
  int message_size = 0;
  int received_len = 0;

  mtp_http_t* http_header;

  buffer = (char*) malloc(sizeof(char) * (SOCKET_INPUT_SIZE + 1));
  CHECK_NULL(buffer, exit(0););
  message_string = dynstring_New();
  memset(buffer, '\0', (sizeof(char) * (SOCKET_INPUT_SIZE + 1)) );

  while(1){
#ifdef NEW_SECURITY
    if ( (n = rece_de_msg(buffer, connection, privatekey)) == -1 ){
   	  printf("Error on receving en msg \n");
	  free(buffer);
	  return 2;
    }
#else
	#ifndef _WIN32
/*	n = recvfrom(connection->clientfd,
		(void *) buffer,
		(size_t) sizeof(char)*SOCKET_INPUT_SIZE,
		0,
		(struct sockaddr *) 0,
		(socklen_t *) 0); */
		//printf("recev message %s\n", buffer);	
		n = recv(connection->clientfd, (void*)buffer, (size_t) sizeof(char)*SOCKET_INPUT_SIZE, 0);
	#else
	n = recvfrom(connection->clientfd,
		buffer,
		(size_t) sizeof(char)*SOCKET_INPUT_SIZE,
		0,
		(struct sockaddr *) 0,
		0);
	#endif
#endif /* NEW_SECURITY */
    received_len += n;
    if (n < 0) {
      free(buffer);
			SOCKET_ERROR();
      return MC_ERR_CONNECT;
    } 
    else if (n == 0 || n < SOCKET_INPUT_SIZE) {
      if (n != 0) {
        buffer[n] = '\0';
        dynstring_Append(message_string, buffer);
      }
      /* Here, we must check the http header to make sure that we have received the
       * entire message. If not, continue the loop. */
      http_header = mtp_http_New();
      if(mtp_http_ParseHeader(http_header, message_string->message) == NULL) {
        fprintf(stderr, "Error parsing HTTP Header. %s:%d\n",
            __FILE__, __LINE__);
				fprintf(stderr, "Message was %s\n", message_string->message);
        dynstring_Destroy(message_string);
        mtp_http_Destroy(http_header);
        free(buffer);
        return 2;
      }
      if ( http_header->content_length != NULL &&
				received_len < (atoi(http_header->content_length) + http_header->header_length) ) {
        mtp_http_Destroy(http_header);
        continue;
      }
      mtp_http_Destroy(http_header);

      free(buffer);
      if(mtp_http_Parse(http, message_string->message)) {
        /* Reply with an HTTP error code */
        buffer = (char*)malloc
          (
           sizeof(char) * 
           (
            strlen
            (
             "HTTP/1.0 503 Service Unavailable\r\nConnection: Close\r\n\r\nMobile C"
            )+1
           )
          );
        strcpy
          (
           buffer,
           "HTTP/1.0 503 Service Unavailable\r\nConnection: Close\r\n\r\nMobile C"
          );
        send
          (
           connection->clientfd,
           buffer,
           sizeof(char)*(strlen(buffer)),
           0
          );
        dynstring_Destroy(message_string);
        free(buffer);
        return ERR;
      } else {
        dynstring_Destroy(message_string);
      }

      if (n < SOCKET_INPUT_SIZE) {
				if (http->http_performative == HTTP_HEAD) {
					buffer = (char*)malloc(sizeof(char) * 300);
					if (buffer == NULL) {
						fprintf(stderr, "Memory Error. %s:%d\n", __FILE__,__LINE__);
					}
					sprintf(buffer, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\nConnection: close\r\nContent-Type:text/html\r\n\r\n%s", strlen(PACKAGE_STRING), PACKAGE_STRING);
					if(send(
							connection->clientfd,
							buffer,
							sizeof(char)*strlen(buffer),
							0 ) < 0) {
						SOCKET_ERROR();
					}
#ifdef _WIN32
					closesocket(connection->clientfd);
#else
					if(close(connection->clientfd) < 0) {
						SOCKET_ERROR();
					}
#endif
					free(buffer);
					return ERR;
				} else {
					buffer = strdup("HTTP/1.0 200 OK\r\nConnection: Close\r\n\r\nMobile C");
					if(send(
							connection->clientfd,
							buffer,
							sizeof(char)*strlen(buffer),
							0 ) < 0 ) {
						SOCKET_ERROR();
					}
#ifdef _WIN32
					closesocket(connection->clientfd);
#else
					if(close(connection->clientfd) < 0) {
						SOCKET_ERROR();
					}
#endif
					free(buffer);
				}
      }
      break;
    } else {
      message_size += n;
      buffer[n] = '\0';
      i++;
      dynstring_Append(message_string, buffer);
      buffer[0] = '\0';
      if (!strcmp
          (
           message_string->message + message_size - 4,
           "\r\n\r\n"
          )
         )
        break;
    }
  }
  return 0;
}

const char* http_GetExpression(const char* string, char** expr)
{
  int i;
  int j;
  int success_flag = 0;
  const char* next_expr_ptr;
  /* Check to see if we are at the end of the HTTP header */
  if( 
      (string[0] == '\n') ||
      (string[0] == '\r' && string[1] == '\n')
    ) 
  {
    for(i = 0; string[i] == '\n' || string[i] == '\r'; i++);
    *expr = NULL;
    return string+i;
  }
  /* FIXME */
  for(i = 0;string[i] != '\0';i++) {
    if (
        (
         (string[i] == '\r') &&
         (string[i+1] == '\n') &&
         (string[i+2] != '\t') &&
         (string[i+2] != ' ')
        ) 
        ||
        (
         string[i] == '\n' && 
         string[i+1] != '\t' &&
         string[i+2] != ' '
        )
       )
    {
      success_flag = 1;
      break;
    } 
  }
  if(success_flag)
  {
    *expr = (char*)malloc
      (
       sizeof(char) * (i+1)
      );
    for(j = 0; j < i; j++) {
      (*expr)[j] = string[j];
    }
    (*expr)[j] = '\0';
    next_expr_ptr = &(string[i]);
    if(next_expr_ptr[0] == '\r' && next_expr_ptr[1] == '\n') {
      next_expr_ptr += 2;
    } else if (next_expr_ptr[0] == '\n') {
      next_expr_ptr++;
    }
    return next_expr_ptr;
  } else {
    return NULL;
  }
}
      
int http_ParseExpression(
    const char* expression_string,
    char** name,
    char** value
    )
{
  int i=0;
  const char* tmp;
  const char* charptr;
  if(expression_string == NULL) {
    *name = NULL;
    *value = NULL;
    return MC_ERR_PARSE;
  }
  tmp = expression_string;
  if (tmp == NULL || (!strncmp(tmp, "\r\n", 2)) || (!strncmp(tmp, "\n", 1))) {
    *name = NULL;
    *value = NULL;
    return MC_ERR_PARSE;
  }
  for(; *tmp!=':' && *tmp!='\0'; tmp++)
    i++;
  if(*tmp == '\0') {
    *name = NULL;
    *value = NULL;
    return MC_ERR_PARSE;
  }
  *name = (char*)malloc
    (
     sizeof(char) * (i+1)
    );
  CHECK_NULL(*name, exit(0););
  charptr = expression_string;
  i=0;
  while(charptr != tmp) {
    (*name)[i] = *charptr;
    i++;
    charptr++;
  }
  (*name)[i] = '\0';

  tmp++;
  while
    (
     (*tmp == ' ') ||
     (*tmp == '\t')
    )
      tmp++;

  *value = (char*)malloc
    (
     sizeof(char) * 
     (strlen(tmp) + 1)
    );
  CHECK_NULL(*value, exit(0););
  strcpy(*value, tmp);
  return MC_SUCCESS;
}

const char* mtp_http_ParseHeader(struct mtp_http_s* http, const char* string)
{
  const char* line = NULL;
  char* expr = NULL;
  char* name = NULL;
  char* value = NULL;

  int err_code = 0;

  line = string;
  line = http_ParseRequest
    (
     http,
     line
    );
  do
  {
		if(strlen(line) == 0) {break;}
    line = http_GetExpression
      (
       line,
       &expr
      );

    err_code = http_ParseExpression
      (
       expr,
       &name,
       &value
      );
    if
      (
       (name == NULL) ||
       (value == NULL)
      )
      {
        if (expr != NULL) {
          free(expr);
        }
        break;
      }
#define HTTP_PARSE_EXPR( parse_name, struct_name ) \
    if ( !strcmp(name, parse_name) ) { \
      http->struct_name = (char*)malloc \
      ( \
        sizeof(char) * \
        (strlen(value)+1) \
      ); \
      strcpy(http->struct_name, value); \
    } else

    HTTP_PARSE_EXPR( "Host", host )
    HTTP_PARSE_EXPR( "Date", date )
    HTTP_PARSE_EXPR( "Server", server )
    HTTP_PARSE_EXPR( "Accept-Ranges", accept_ranges )
    HTTP_PARSE_EXPR( "Content-Length", content_length)
    HTTP_PARSE_EXPR( "Connection", connection )
    HTTP_PARSE_EXPR( "Content-Type", content_type)
    HTTP_PARSE_EXPR( "User-Agent", user_agent)
    HTTP_PARSE_EXPR( "Cache-Control", cache_control)
    HTTP_PARSE_EXPR( "MIME-Version", mime_version)
    HTTP_PARSE_EXPR( "Mime-Version", mime_version)
    {
      fprintf(stderr, "Warning: Unknown http name: %s. %s:%d\n",
          name, __FILE__, __LINE__);
    }
#undef HTTP_PARSE_EXPR

#define SAFE_FREE( object ) \
    if(object) free(object); \
    object = NULL

    SAFE_FREE(expr);
    SAFE_FREE(name);
    SAFE_FREE(value);
#undef SAFE_FREE
     
  } while(line != NULL && err_code == MC_SUCCESS);

  http->header_length = line - string - 1;
  return line;
}

int
mtp_http_Parse(struct mtp_http_s* http, const char* string)
{
  const char* line;
  char* expr = NULL;
  char* name = NULL;
  char* value = NULL;
  const char* tmp;
  const char* tmp2;
  int i;

  int err_code = 0;

  line = mtp_http_ParseHeader(http, string);
  /* If the content type is multipart/mixed, then we need to
   * figure out how many parts there are. */
  if( 
      http->content_type != NULL &&
      !strncmp(
        http->content_type,
        "multipart/mixed",
        strlen("multipart/mixed")
        ) 
    ) 
  {
    tmp = strstr(http->content_type, "boundary=");
    tmp += strlen("boundary=.");
    tmp2 = strchr(tmp, '\"');
    http->boundary = (char*)malloc(sizeof(char) * (tmp2 - tmp + 3));
    /* We must remember to include the leading '--' for the boundary */
    http->boundary[0] = '-';
    http->boundary[1] = '-';
    for (i = 0; tmp != tmp2; i++, tmp++) {
      http->boundary[i+2] = *tmp;
    }
    http->boundary[i+2] = '\0';

    /* Count the number of message parts in the message */
    tmp = (char*)line;
    http->message_parts = 0;
    while((tmp = strstr(tmp, http->boundary))) {
      http->message_parts++;
      tmp++;
    }
    http->message_parts--;
  } else {
    http->boundary = NULL;
    http->message_parts = 1;
  }

  if (http->message_parts == 1) { 
    http->content = (struct mtp_http_content_s*)malloc(sizeof(struct mtp_http_content_s));
    memset(http->content, 0, sizeof(struct mtp_http_content_s));
    /* Copy rest of contents into the data member */
    if (line != NULL && http->content_length > 0) {
      http->content->data = (void*)malloc
        (
         sizeof(char) * 
         (strlen(line)+1)
        );
      strcpy((char*)http->content->data, line);
      if (http->content_type != NULL) {
        http->content->content_type = strdup(http->content_type);
      }
    }
  } else {
    http->content = (struct mtp_http_content_s*)malloc(
        sizeof(struct mtp_http_content_s) * http->message_parts );
    memset(http->content, 0, sizeof(struct mtp_http_content_s) * http->message_parts);
    /* Find the boundary */
    line = strstr(line, http->boundary);
    line += strlen(http->boundary);
    line = strchr(line, '\n');
    line++;
    for(i = 0; i < http->message_parts; i++) {
      /* For each part, we must:
       * 1. Find the boundary
       * 2. Parse the attributes, until we
       * 3. Find the empty line
       * 4. Copy the data up to the next boundary */
      
      /* Find the end boundary */
      tmp = strstr(line + strlen(http->boundary), http->boundary);
      /* Parse the attributes */
      do{
        /* FIXME */

        line = http_GetExpression
          (
           line,
           &expr
          );

        err_code = http_ParseExpression
          (
           expr,
           &name,
           &value
          );
        if
          (
           (name == NULL) ||
           (value == NULL)
          )
          {
            if (expr != NULL) {
              free(expr);
            }
            break;
          }
        if (!strcmp(name, "Content-Type")) {
          http->content[i].content_type = (char*)malloc(
              sizeof(char) * (strlen(value)+1));
          strcpy(http->content[i].content_type, value);
        }

        /* Clean up memory */
        if (expr != NULL) {
          free(expr);
          expr = NULL;
        }
        if (name != NULL) {
          free(name);
          name = NULL;
        }
        if (value != NULL) {
          free(value);
          value = NULL;
        }
      } while(line != NULL && err_code == MC_SUCCESS);
      /* Copy the data */
      http->content[i].data = (void*)malloc(tmp-line+sizeof(char));
      memcpy(http->content[i].data, line, tmp-line);
      ((char*)http->content[i].data)[tmp-line] = '\0';
      /* Move 'line' to the next boundary */
      line = tmp + strlen(http->boundary);
      line = strchr(line, '\n');
      line++;
    }
  }
  if (
      (http->http_performative == HTTP_POST) ||
      (http->http_performative == HTTP_PUT) ||
      (http->http_performative == HTTP_RESPONSE) ||
			(http->http_performative == HTTP_HEAD)
     )
    return 0;
  else
    return 1;
}

const char* http_ParseRequest(
    mtp_http_p http,
    const char* string )
{
  const char* cur;
  char* token;
  char* tmp = NULL;
  char* target;
  int len;

  cur = string;
  cur = http_GetToken(cur, &token);
  if (token == NULL) {
    return NULL;
  }
  if (!strcmp(token, "GET")) {
    http->http_performative = HTTP_GET;
    cur = http_GetToken(cur, &tmp);
    /* We don't support 'get' yet */
    if(tmp) free(tmp);
  } else if(!strcmp(token, "HEAD")) {
    /* Don't support this yet */
    http->http_performative = HTTP_HEAD;
  } else if(!strcmp(token, "POST")) {
    http->http_performative = HTTP_POST;
    cur = http_GetToken(cur, &tmp);
    if(tmp != NULL) {
      if(!strncmp(tmp, "http://",7)) {
        target = strchr(tmp+7, (int)'/');
      } else {
        target = strchr(tmp, (int)'/');
      }
      if (target == NULL)
        target = tmp;
      http->target = (char*) malloc(sizeof(char) * (strlen(target)+1));
      strcpy(http->target, target);
      free(tmp);
    }
  } else if(!strcmp(token, "PUT")) {
    http->http_performative = HTTP_PUT;
    cur = http_GetToken(cur, &tmp);
    if (tmp != NULL) {
      http->target = (char*)malloc(sizeof(char)*(strlen(tmp)+1));
      strcpy(http->target, tmp);
      free(tmp);
    }
  } else if(!strcmp(token, "DELETE")) {
    http->http_performative = HTTP_DELETE;
  } else if(!strcmp(token, "TRACE")) {
    http->http_performative = HTTP_TRACE;
  } else if(!strcmp(token, "OPTIONS")) {
    http->http_performative = HTTP_OPTIONS;
  } else if(!strcmp(token, "CONNECT")) {
    http->http_performative = HTTP_CONNECT;
  } else if(!strncmp(token, "HTTP/", 5)) {
    /* This is a response message */
    http->http_performative = HTTP_RESPONSE;
    /* We expect a status code */
    free(token);
    cur = http_GetToken(cur, &token);
    sscanf(token, "%d", &http->response_code);
    /* And now a status string */
    len = strstr(cur, "\r\n") - cur + 1;
    http->response_string = (char*)malloc(
        sizeof(char) * (len +1) );
    strncpy(
        http->response_string,
        cur,
        len );
    http->response_string[len] = '\0';
  } else {
    /* FIXME */
    /* Illegal performative */
    http->http_performative = HTTP_PERFORMATIVE_UNDEF;
  }
  free(token);
  cur = string;
  while(*cur != '\0') {
    if(*cur == '\n') {
      while (*cur == '\n' || *cur == '\r' || *cur == ' ')
        cur++;
      break;
    }
    cur++;
  }
  return cur;
}

const char*
http_GetToken(const char* string, char** token)
{
  const char* cur;
  const char* begin;
  char* itr;
  
  cur = string;
  /* See if it's an empty line */
  if (string[0] == '\r' && string[1] == '\n') {
    *token = NULL;
    return NULL;
  }
  /* Get rid of initial whitespace */
  while(*cur == ' ' || *cur == '\t' || *cur == '\r' || *cur == '\n') cur++;

  begin = cur;
  while(*cur != '\0') {
    cur++;
    if (*cur == ' ' || *cur == '\t' || *cur == '\r' || *cur == '\n')
      break;
  }
  cur--;
  *token = (char*)malloc(cur - begin + 4*sizeof(char));
  itr = *token;
  while (begin <= cur) {
    *itr = *begin;
    itr++;
    begin++;
  }
  *itr='\0';
  return cur+1;
}

int mtp_http_ParseResponse(struct mtp_http_s* http, const char* string)
{
  return 0;
}

int 
mtp_http_ComposeMessage(message_p message)
{
  char* http_header;
  char* tmp;
  char buf[20];
  if (message->isHTTP) {
    /* message reports that it already is an http message. No need to continue. */
    return 0;
  }

  http_header = (char*)malloc
    (
     sizeof(char) * (1400 + strlen(message->to_address))
    );
  http_header[0] = '\0';
  strcat(http_header, "POST /");
  strcat(http_header, message->target);
  strcat(http_header, " HTTP/1.0\r\n");
  strcat(http_header, "User-Agent: MobileC/");
  strcat(http_header, PACKAGE_VERSION);
  strcat(http_header, "\r\n");
  strcat(http_header, "Host: ");
  strcat(http_header, message->to_address);
  strcat(http_header, "\r\n");
  strcat(http_header, "Content-Type: text/plain\r\n");
  strcat(http_header, "Connection: Close\r\n");
  strcat(http_header, "Content-Length: ");
  sprintf(buf, "%d", strlen(message->message_body) + 1);
  strcat(http_header, buf);
  strcat(http_header, "\r\n\r\n");

  tmp = (char*)malloc
    (
     sizeof(char) * 
     (strlen(http_header) + strlen(message->message_body) + 1)
    );
  tmp[0] = '\0';
  strcpy(tmp, http_header);
  strcat(tmp, message->message_body);
  free(message->message_body);
  message->message_body = tmp;
  free(http_header);
  return MC_SUCCESS;
}

struct message_s* 
mtp_http_CreateMessage(
    mtp_http_t* mtp_http,
    char* hostname,
    int port)
{
  int i;
  int num;
  char buf[30];
  char boundary[30];
  message_t* message;
  dynstring_t* http_header;
  dynstring_t* http_body;
  http_header = dynstring_New();
  http_body = dynstring_New();
  dynstring_Append(http_header, "POST /");
  dynstring_Append(http_header, mtp_http->target);
  dynstring_Append(http_header, " HTTP/1.1\r\n");
  dynstring_Append(http_header, "User-Agent: MobileC/");
  dynstring_Append(http_header, PACKAGE_VERSION);
  dynstring_Append(http_header, "\r\n");
  dynstring_Append(http_header, "Host: ");
  dynstring_Append(http_header, mtp_http->host);
  dynstring_Append(http_header, ":");
  sprintf(buf, "%d", port);
  dynstring_Append(http_header, buf);
  dynstring_Append(http_header, "\r\n");
  dynstring_Append(http_header, "Cache-Control: no-cache\r\n");
  dynstring_Append(http_header, "Mime-Version: 1.0\r\n");

  /* The next part of the message will be different depending on 
   * how many message parts we have.  */
  if(mtp_http->message_parts == 1) {
    dynstring_Append(http_header, "Content-Type: text/plain\r\n");
    dynstring_Append(http_header, "Content-Length: ");
    sprintf(buf, "%d", strlen((char*)mtp_http->content[0].data));
    dynstring_Append(http_header, buf);
    dynstring_Append(http_header, "\r\n\r\n");
    dynstring_Append(http_header, (char*)mtp_http->content[0].data);
  } else {
    /* We need to generate a random boundary. */
    srand(time(NULL));
    strcpy(boundary, "--");
    for(i = 2; i < 26; i++) {
      num = rand() % 36;
      if(num < 10) {
        boundary[i] = (char)(48 + num);
      } else {
        boundary[i] = (char)( (num-10)+65);
      }
    }
    boundary[i] = '\0';
    /* FIXME: A randomly generated boundary should now be stored in boundary. Should
     * we check to see if there is a match within the body? The chances of collision are
     * infinitessimal. */
    dynstring_Append(http_body, "This is not part of the MIME multipart encoded message.\r\n");
    for(i = 0; i<mtp_http->message_parts; i++) {
      dynstring_Append(http_body, boundary);
      dynstring_Append(http_body, "\r\nContent-Type: ");
      dynstring_Append(http_body, mtp_http->content[i].content_type);
      dynstring_Append(http_body, "\r\n\r\n");
      dynstring_Append(http_body, (char*)mtp_http->content[i].data);
      dynstring_Append(http_body, "\r\n");
    }
    /* Append one last boundary */
    dynstring_Append(http_body, boundary);
    dynstring_Append(http_body, "--");
    dynstring_Append(http_body, "\r\n\r\n");

    /* Set the content length in the header */
    dynstring_Append(http_header, "Content-Length: ");
    sprintf(buf, "%d", http_body->len-2 );
    dynstring_Append(http_header, buf);
    dynstring_Append(http_header, "\r\n");
    dynstring_Append(http_header, "Content-Type: multipart/mixed ; boundary=\"");
    dynstring_Append(http_header, boundary+2); /* Omit leading "--" */
    dynstring_Append(http_header, "\"\r\n");

  }
  dynstring_Append(http_header, "Connection: Close\r\n\r\n");
  message = message_New();
  message->to_address = (char*)malloc(
      sizeof(char) * (strlen(hostname)+15) );
  sprintf(message->to_address, "%s:%d", hostname, port);
  message->message_body = (char*) malloc( 
      sizeof(char) * (http_header->len + http_body->len + 1));
  strcpy(message->message_body, http_header->message);
  strcat(message->message_body, http_body->message);
  dynstring_Destroy(http_header);
  dynstring_Destroy(http_body);
  message->isHTTP = 1;
  return message;
}

