/* SVN FILE INFO
 * $Revision: 563 $ : Last Committed Revision
 * $Date: 2010-08-31 11:59:15 -0700 (Tue, 31 Aug 2010) $ : Last Committed Date */
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

#include <string.h>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include "config.h"
#else
#include <winsock2.h>
#include "winconfig.h"
#endif
#if HAVE_LIBBLUETOOTH
#ifdef _WIN32
#include <Ws2bth.h>
#else
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#endif
#endif
#include <mxml.h>
#include "include/libmc.h"
#include "include/agent.h"
#include "include/mc_platform.h"
#include "include/message.h"
#include "include/mtp_http.h"
#include "include/xml_compose.h"
#include "include/xml_helper.h"
#include "include/xml_parser.h"
#include "include/macros.h"

#include "security/asm_node.h"

#define SOCKET_INPUT_SIZE 4096

message_p
message_New(void)
{
  message_p message;
  message = (message_p)malloc(sizeof(message_t));
  CHECK_NULL(message, exit(0););
  message->addr = NULL;
  message->connect_id = 0;
  message->message_id = 0;
  message->isHTTP = 0;
  message->message_type = (enum message_type_e)0;
  message->http_type = (enum http_performative_e)0;
  message->xml_root = NULL;
  message->xml_payload = NULL;
  message->message_body = NULL;
  message->update_name = NULL;
  message->update_num = 0;
  message->from_address = NULL;
  message->to_address = NULL;
  message->target = NULL;
  message->agent_xml_flag = 0;
  return message;
}

message_p
message_Copy(message_p src)
{
  fprintf(stderr, "FIXME: message_Copy() is not implemented yet. %s:%d\n",
      __FILE__, __LINE__);
  return NULL;
}

int
message_InitializeFromAgent(
    mc_platform_p mc_platform,
    message_p message,
    agent_p agent)
{
  struct hostent* host;

  char* buf;
  char* destination_host;
  char* destination_port_str;
#ifndef _WIN32
  char* save_ptr; /* For re-entrant strtok_r */
#endif
  int destination_port;

  message->message_id = rand();
  message->message_type = MOBILE_AGENT;

  message->xml_root = agent_xml_compose(agent);
  /* If agent_xml_compose fails, that is a fatal error, since
   * 'agent' is gauranteed to be a valid agent. */
  CHECK_NULL(message->xml_root, exit(0););
  message->message_body = mxmlSaveAllocString( 
      message->xml_root,
      MXML_NO_CALLBACK );

  message->update_name = NULL;

  message->from_address =
    (char*)malloc(sizeof(char) * (strlen(mc_platform->hostname) + 10));
  sprintf(
      message->from_address,
      "%s:%d",
      mc_platform->hostname,
      mc_platform->port );
  if (
      agent->datastate->task_progress >=
      agent->datastate->number_of_tasks
     )
  {
    message->to_address = 
      (char*)malloc
      (
       sizeof(char) * 
       (
        strlen(agent->home) + 1
       )
      );
    CHECK_NULL(message->to_address, exit(0););
    strcpy
      (
       message->to_address,
       agent->home
      );
  } else {
    message->to_address = 
      (char*) malloc
      ( 
       sizeof(char) * 
       (
        strlen
        (
         agent->datastate->tasks[ agent->datastate->task_progress ]
         ->server_name 
        )
        +1
       )
      );
    CHECK_NULL( message->to_address, mc_platform->err = MC_ERR_MEMORY; return MC_ERR_MEMORY;);
    strcpy(
        message->to_address,
        agent->datastate->tasks[ agent->datastate->task_progress ]->server_name 
        );
  }
  message->agent_xml_flag = 0;
  message->target = strdup("ams");
	message->sending_agent_name = strdup(agent->name);
  /* Set up message->addr */
  if(mc_platform->bluetooth == 0) {
    buf = (char*)malloc
      (
       sizeof(char) * 
       (strlen(message->to_address)+1)
      );
    CHECK_NULL(buf, exit(0););
    strcpy(buf, message->to_address);
    destination_host = strtok_r(buf, ":", &save_ptr);
    destination_port_str = strtok_r(NULL, ":", &save_ptr);
    destination_port = atoi(destination_port_str);
    message->addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    if ((host = gethostbyname(destination_host))) {
      memcpy(&(message->addr->sin_addr), host->h_addr, host->h_length);
      message->addr->sin_port = htons(destination_port);
    } else {
      WARN("Host not found.");
    }
    free(buf);
  }
  return MC_SUCCESS;
}

int
message_InitializeFromConnection(
    mc_platform_p mc_platform,
    message_p message,
    connection_p connection)
{
  int i = 1;
  int n;
  char *message_string;
  char *buffer;

  message->addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
  CHECK_NULL(message->addr, exit(0););
  *(message->addr) = connection->addr;

  message->connect_id = connection->connect_id;

  message->message_id = rand();

  message->to_address = NULL;
  message->from_address = NULL;
  message->target = NULL;

  buffer = (char*) malloc(sizeof(char) * (SOCKET_INPUT_SIZE + 1));
  CHECK_NULL(buffer, exit(0););
  message_string = (char*) malloc(sizeof(char) * (SOCKET_INPUT_SIZE + 1));
  CHECK_NULL(message_string, exit(0););
  message_string[0] = '\0';
  buffer[0] = '\0';

  /* Receive the message */
  while(1) {
#ifndef _WIN32
    /*n = recvfrom(connection->clientfd,
        (void *) buffer,
        (size_t) sizeof(char)*SOCKET_INPUT_SIZE,
        0,
        (struct sockaddr *) 0,
        (socklen_t *) 0); */
		n = recv(connection->clientfd, (void*)buffer, (size_t) sizeof(char)*SOCKET_INPUT_SIZE, MSG_WAITALL);
#else
    n = recvfrom(connection->clientfd,
        buffer,
        (size_t) sizeof(char)*SOCKET_INPUT_SIZE,
        0,
        (struct sockaddr *) 0,
        0);
#endif
    if (n < 0) {
      free(buffer);
			SOCKET_ERROR();
      return MC_ERR_CONNECT;
    } 
    else if (n == 0) {
      free(buffer);
      break;
    } else {
      buffer[n] = '\0';
      i++;
      strcat(message_string, buffer);
      message_string = (char*)realloc
        (
         message_string, 
         sizeof(char) * (SOCKET_INPUT_SIZE+1) * i
        );
      CHECK_NULL(message_string, exit(0););
      buffer[0] = '\0';
    }
  }
  message->message_body = (char*)malloc
    (
     sizeof(char) * 
     (strlen(message_string) + 1)
    );
  CHECK_NULL(message->message_body, exit(0););
  strcpy(message->message_body, message_string);
  free(message_string);
  message->xml_root = mxmlLoadString
    (
     NULL, 
     message->message_body,
     MXML_NO_CALLBACK
    );
  if (message_xml_parse(message)) {
    fprintf(stderr, "Error parsing message at %s:%d.\n",
        __FILE__, __LINE__);
    message_Destroy(message);
    return MC_ERR_PARSE;
  }
  return MC_SUCCESS;  
}

int http_to_hostport(const char* http_str, char** host, int* port, char** target)
{
  /* We want to convert the string "http://somehost.com:5050/acc" to a 
   * host: somehost.com
   * port: 5050
   * target: acc */
  const char* tmp;
  if(strncmp(http_str, "http://", 7)) {
    return MC_ERR_PARSE;
  }
  http_str += 7;
  tmp = strchr(http_str, (int)':');
  if (tmp == NULL) return MC_ERR_PARSE;

  /* Get the host */
  *host = (char*)malloc(sizeof(char) * 
      (tmp - http_str + 1) );
  strncpy(*host, http_str, tmp - http_str);
  (*host)[tmp-http_str] = '\0';

  /* Get the port */
  tmp++;
  sscanf(tmp, "%d", port);

  /* Get the target */
  tmp = strchr(tmp, (int)'/');
  tmp++;
  *target = (char*)malloc(sizeof(char) * 
      (strlen(tmp)+1) );
  strcpy(*target, tmp);

  return 0;
}

int
message_InitializeFromString(
    mc_platform_p mc_platform,
    message_p message,
    const char* string,
    const char* destination_host,
    int destination_port,
    const char* target)
{
  char* destination;
  struct hostent* host;

  message->connect_id = 0;
  message->message_id = rand();

  message->message_type = MOBILE_AGENT;

  message->xml_root = NULL;

  message->message_body = 
    (char*)malloc( sizeof(char) * (strlen(string)+1));
  CHECK_NULL(message->message_body, 
      mc_platform->err = MC_ERR_MEMORY;
      return MC_ERR_MEMORY; );
  strcpy(message->message_body, string);

  message->update_name = NULL;

  if(destination_host != NULL) {
    destination = (char*)malloc(sizeof(char)*(strlen(destination_host) + 10));
    CHECK_NULL(destination,
        mc_platform->err = MC_ERR_MEMORY;
        return MC_ERR_MEMORY; );
    sprintf(destination, "%s:%d", 
        destination_host,
        destination_port
        );
    message->to_address = destination;
  } else {
    message->to_address = NULL;
  }

  /* DEBUG */
  //message->to_address = destination;
  message->from_address = (char*)malloc(
      sizeof(char) * (strlen(mc_platform->hostname)+10));
  sprintf(message->from_address,
      "%s:%d",
      mc_platform->hostname,
      mc_platform->port );
  message->target = (char*)malloc(sizeof(char) * 
      (strlen(target)+1));
  strcpy(message->target, target);

  /* Set up message->addr */
  message->addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
  if (destination_host != NULL && strlen(destination_host)!= 0) {
    if((host = gethostbyname(destination_host)))
    {
      memcpy(&(message->addr->sin_addr), host->h_addr, host->h_length);
      message->addr->sin_port = htons(destination_port);
    } else {
      fprintf(stderr, "Warning: Host not found: %s:%d  %s:%d",
          destination_host, destination_port, __FILE__, __LINE__ );
    }
  }
  message->xml_root = mxmlLoadString
    (
     NULL, 
     message->message_body,
     MXML_NO_CALLBACK
    );
  if (message_xml_parse(message)) {
    fprintf(stderr, "Error parsing message at %s:%d.\n",
        __FILE__, __LINE__);
    return MC_ERR_PARSE;
  }

  return MC_SUCCESS;
}

  int
message_Destroy(message_p message)
{
  if (message == NULL) {
    return MC_SUCCESS;
  }
  /* We may not want to delete this here,
   * in case an agent needs this data. */
  if(message->xml_root != NULL && message->agent_xml_flag == 0) {
    mxmlDelete(message->xml_root);
  }

  if(message->addr) {
    free(message->addr);
    message->addr = NULL;
  }
  if(message->message_body != NULL) {
    free(message->message_body);
    message->message_body = NULL;
  }
  if(message->update_name != NULL) {
    free(message->update_name);
  }
  if(message->from_address != NULL) {
    free(message->from_address);
  }
  if(message->to_address != NULL) {
    free(message->to_address);
  }
  if(message->target != NULL) {
    free(message->target);
  }
  if(message->sending_agent_name != NULL) {
    free(message->sending_agent_name);
  }

  free(message);
  message = NULL;
  return MC_SUCCESS;
}

// authenticating receiver and send message
int
auth_rece_send_msg(int sockfd, char *hostname, char *message, char *privkey, char *known_host_filename){

  int ret=-1;
  unsigned char passphrase[35], aes_key[35];
  int nonce, mode;
  char privatekey[1215];
  char peer_pubkey[300];
  char plaintext[135];
  FILE *fd;

  /* temporary files  */
  char *infile; //= "msg";     // message
  char *outfile; //= "en-msg"; // encrypted message
  memset(passphrase ,  '\0', 35);
  memset(privatekey ,  '\0', 1215);
  memset(plaintext  ,  '\0', 135);
  memset(aes_key, '\0', 35);
  //memset(infile, '\0', 20);
  //memset(outfile, '\0', 20);
  //memset(buffer, '\0', 10);

#ifndef _WIN32   
  int fd1;
  infile = (char *)malloc(sizeof(char)*20);    
  strcpy(infile, "msgXXXXXX");
  fd1 = mkstemp(infile);
  if (fd1 == -1) {
    fprintf(stderr, "Could not create temporary file:%s. %s:%d\n",
        infile, __FILE__, __LINE__ );
    exit(EXIT_FAILURE);
  } 
  close(fd1);

  outfile = (char *)malloc(sizeof(char)*20);
  strcpy(outfile, "en-msgXXXXXX");
  fd1 = mkstemp(outfile);
  if (fd1 == -1) {
    fprintf(stderr, "Could not create temporary file:%s. %s:%d\n",
        outfile, __FILE__, __LINE__ );
    exit(EXIT_FAILURE);
  }
  close(fd1);
  /*  
      tempfile = (char *)malloc(sizeof(char)*20);
      strcpy(tempfile, "tempXXXXXX");
      fd1 = mkstemp(tempfile);
      if(fd == -1){
      printf("Could not create temporary file \n");
      exit(EXIT_FAILURE);
      }
      close(fd1);
      */
#else
  infile  = _tempnam(".", "msg");
  outfile = _tempnam(".", "en-msg");
#endif

  /* srand(time(NULL));
     num = rand();
     sprintf(infile , "msg%d", num);
     sprintf(outfile, "en-msg%d", num);
     */     
  strcpy(privatekey, privkey);
  //printf("\n ################################### \n");	
  //printf("Authenticating %s \n", hostname);	
  //printf("Known host file lookup for peer public key ... \n");
  // Known host file lookup for peer public key 
  if (read_known_host_file(peer_pubkey, hostname, known_host_filename) == -1 ){
    printf("Client: %s 's Public key not found in know host file\n", hostname);
    if( remove(infile) ) printf("message.c : remove error 4");
    if( remove(outfile) ) printf("message.c : remove error 5");
#ifndef _WIN32
    //close(sockfd);
#else
    //closesocket(sockfd);
#endif
  }else{
    if ((ret=initiate_migration_process(sockfd, &nonce, peer_pubkey, privatekey, aes_key)) != 1){
      if (ret == -1)
        printf("Client: Connected peer is not authenticated \n");
      if (ret == -2)
        printf("Client: Unable to get authentication from oither peer \n");
    }else{
      //printf("Successfully Authenticate %s \n", hostname); 
      // printf("AES key is exchanged. \n");

      // writing message (that is to be send) in a file.
      if( (fd = fopen(infile,"w")) == NULL){
        printf("Unable to write message in a file \n");
        if( remove(infile) ) printf("message.c : remove error 1");
      }else{
        //printf("message length = %d \n", strlen(message) );	 
        fwrite (message , 1 , strlen(message) , fd );
        fclose(fd);  
        // Encrypt message file
        mode = 0; // Encryption
        if (aes_en_de(mode, infile, outfile, aes_key, &nonce, 0) != 1){
          printf("Client: AES Encryption Failed \n");
          if( remove(infile) ) printf("message.c : remove error 2"); 
          if( remove(outfile) ) printf("message.c : remove error 3");
        }else{	
          if( remove(infile) ) printf("message.c : remove error 4");
          if (send_AES_en_MA(sockfd, &nonce, outfile, peer_pubkey) != 1  ){
            printf("Client: Error while sending mobile agent \n");
            if( remove(outfile) ) printf("message.c : remove error 5");
          }else{
            if( remove(outfile) ) printf("message.c : remove error 6");
            //printf(" Successfully send the message \n");
            ret = 2;	
          }
        }
      }
    }
  }
  // printf("------------------------------ \n");  
  //  free(infile);
  //  free(outfile);
  //  free(tempfile);
  return ret;
}

#define MSG_THREADS 40
int message_Send(mc_platform_t* mc_platform, message_p message, char *privatekey)
{
	THREAD_T msg_thread;
	message_send_arg_t* arg;
#ifndef _WIN32
	pthread_attr_t attr;
	pthread_attr_init(&attr);
#else
	int stack_size = 0;
#endif

	arg = (message_send_arg_t*)malloc(sizeof(message_send_arg_t));
	arg->mc_platform = mc_platform;
	arg->message = message;
	arg->privatekey = privatekey;

	MUTEX_LOCK(&mc_platform->acc->msg_thread_lock);
	while(mc_platform->acc->num_msg_threads >= MSG_THREADS) {
		COND_WAIT(&mc_platform->acc->msg_thread_cond, &mc_platform->acc->msg_thread_lock);
	}
	mc_platform->acc->num_msg_threads++;
	MUTEX_UNLOCK(&mc_platform->acc->msg_thread_lock);
	/* DEBUG */
	THREAD_CREATE(&msg_thread, message_send_Thread, arg);
	THREAD_DETACH(msg_thread);
  return 0;
}

#ifdef _WIN32
typedef struct bdaddr_s {
  UINT8 b[6];
} bdaddr_t;

void baswap(bdaddr_t *dst, const bdaddr_t *src)
{
	register unsigned char *d = (unsigned char *) dst;
	register const unsigned char *s = (const unsigned char *) src;
	register int i;

	for (i = 0; i < 6; i++)
		d[i] = s[5-i];
}

int str2ba(const char *str, bdaddr_t *ba)
{
	UINT8 b[6];
	const char *ptr = str;
	int i;

	for (i = 0; i < 6; i++) {
		b[i] = (UINT8) strtol(ptr, NULL, 16);
		if (i != 5 && !(ptr = strchr(ptr, ':')))
			ptr = ":00:00:00:00:00";
		ptr++;
	}

	baswap(ba, (bdaddr_t *) b);

	return 0;
}
#endif

#define MSG_THREAD_EXIT() \
	free(arg); \
	message_Destroy(message); \
  if(myaddrinfo) { \
    freeaddrinfo(myaddrinfo); \
  } \
	MUTEX_LOCK(&mc_platform->acc->msg_thread_lock); \
	mc_platform->acc->num_msg_threads--; \
	COND_SIGNAL(&mc_platform->acc->msg_thread_cond); \
	MUTEX_UNLOCK(&mc_platform->acc->msg_thread_lock); \
	THREAD_EXIT()

#ifndef _WIN32
  void*
message_send_Thread(void* arg)
#else
  DWORD WINAPI
message_send_Thread( LPVOID arg )
#endif
{
	mc_platform_t* mc_platform;
	message_p message;
	char* privatekey;
  char *buffer;
  mtp_http_t* mtp_http;
  int n;
#ifndef _WIN32
  int skt;
  struct sockaddr_in sktin;
  struct addrinfo* myaddrinfo;
  struct addrinfo hint_addrinfo;
#else
  SOCKET skt;
  SOCKADDR_IN sktin;
  struct hostent host;
  struct hostent* host_result;
#endif
#if HAVE_LIBBLUETOOTH
#ifdef _WIN32
  SOCKADDR_BTH btsktin;
#else
  struct sockaddr_rc btsktin;
#endif
#endif
  char *buf;
  char *hostname;
#ifndef _WIN32
  char *saveptr; /* For reentrant strtok_r */
#endif
  int port;

	int num_tries = 0;
	const int max_num_tries = 100;
	int errnum;

	dynstring_t* message_string;
#ifdef NEW_SECURITY
  int ret;
#endif

	mc_platform = ((message_send_arg_t*)arg)->mc_platform;
	message = ((message_send_arg_t*)arg)->message;
	privatekey = ((message_send_arg_t*)arg)->privatekey;
		
  /* Compose the http message */
  if (
      mtp_http_ComposeMessage(
        message
        )
     )
  {
		fprintf(stderr, "Compose Message Error. %s:%d\n", __FILE__, __LINE__);
    MSG_THREAD_EXIT();
  }

  if(mc_platform->bluetooth) {
#if HAVE_LIBBLUETOOTH
#ifndef _WIN32
    if((skt = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) 
    { 
      fprintf(stderr, "Error - can't create socket\n");
      MSG_THREAD_EXIT();
    }
    btsktin.rc_family = AF_BLUETOOTH;
    buf = (char*)malloc(sizeof(char)*(strlen(message->to_address)+1));
    strcpy(buf, message->to_address);
    hostname = strtok_r(buf, " ", &saveptr);
    sscanf( strtok_r(NULL, " ", &saveptr), "%d", &port );
    btsktin.rc_channel = (uint8_t) port;
    str2ba( hostname, &btsktin.rc_bdaddr );
#else
    if((skt = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM)) < 0) 
    { 
      fprintf(stderr, "Error - can't create socket\n");
      MSG_THREAD_EXIT();
    }
    btsktin.addressFamily = AF_BTH;
    buf = (char*)malloc(sizeof(char)*(strlen(message->to_address)+1));
    strcpy(buf, message->to_address);
    hostname = strtok_r(buf, " ", &saveptr);
    sscanf( strtok_r(NULL, " ", &saveptr), "%d", &port );
    btsktin.port =  port;
    str2ba( hostname, (bdaddr_t*)&btsktin.btAddr );
#endif
#endif
  } else {
    /* We need to split up the address into a hostname and port. */
    buf = (char*)malloc(sizeof(char)*(strlen(message->to_address)+1));
    strcpy(buf, message->to_address);
    hostname = strtok_r(buf, ":", &saveptr);
    sscanf( strtok_r(NULL, ":", &saveptr), "%d", &port );

    if((skt = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
    { 
      fprintf(stderr, "Error - can't create socket\n");
      MSG_THREAD_EXIT();
    }

    memset(&sktin, 0, sizeof(sktin));
    sktin.sin_family = PF_INET;
    sktin.sin_port = htons(port);
  }

  if (mc_platform->bluetooth == 0) {
#ifndef _WIN32
    //if(gethostbyname_r(hostname, &host, hostbuf, hostbuf_len, &host_result, &errnum)) 
    memset(&hint_addrinfo, 0, sizeof(hint_addrinfo));
    hint_addrinfo.ai_family = AF_INET;
    if( (errnum = getaddrinfo(hostname, NULL, &hint_addrinfo, &myaddrinfo)) == 0 )
#else
      if(host_result = gethostbyname(hostname)) 
#endif
      {
#ifdef _WIN32
        host = *host_result;
        memcpy(&sktin.sin_addr, host.h_addr, host.h_length);
#else
        ((struct sockaddr_in*)myaddrinfo->ai_addr)->sin_port = htons(port);
#endif
      }
      else if((sktin.sin_addr.s_addr = inet_addr(hostname)) < 0) 
      {
        fprintf(stderr, "Error - can't get host entry for %s\n", hostname);
        free(buf);
        MSG_THREAD_EXIT();
      } 
  }

#ifndef _WIN32
  if (mc_platform->bluetooth) {
#if HAVE_LIBBLUETOOTH
    errnum = connect(skt, (struct sockaddr *)&btsktin, sizeof(btsktin));
#endif
  } else {
    errnum = connect(skt, myaddrinfo->ai_addr, sizeof(struct sockaddr));
  }
#else
  if (mc_platform->bluetooth) {
#if HAVE_LIBBLUETOOTH
    errnum = connect(skt, (struct sockaddr *) &btsktin, sizeof(btsktin));
#endif
  } else {
    errnum = connect(skt, (struct sockaddr *) &sktin, sizeof(sktin));
  }
#endif
  num_tries = 0;
  while(
		(num_tries < max_num_tries) && (errnum < 0)
		) {
		printf("ERROR Socket Connect failed... errno:%s\n", strerror(errno));
#ifndef _WIN32
		usleep(100000);
#else
		Sleep(100);
#endif
		num_tries++;
#ifndef _WIN32
    if (mc_platform->bluetooth) {
#if HAVE_LIBBLUETOOTH
      errnum = connect(skt, (struct sockaddr *)&btsktin, sizeof(btsktin));
#endif
    } else {
      errnum = connect(skt, myaddrinfo->ai_addr, sizeof(struct sockaddr));
    }
#else
    if (mc_platform->bluetooth) {
#if HAVE_LIBBLUETOOTH
      errnum = connect(skt, (struct sockaddr *) &btsktin, sizeof(btsktin));
#endif
    } else {
      errnum = connect(skt, (struct sockaddr *) &sktin, sizeof(sktin));
    }
#endif
  }
	if (num_tries == max_num_tries) {
    fprintf(stderr, "Error - can't connect to %s:%d\n",
        hostname,
        port
        );
    free(buf);
    MSG_THREAD_EXIT();
	}

#ifdef NEW_SECURITY 
  /*  authenticate the receiver of message */
  ret = auth_rece_send_msg(skt, hostname, message->message_body, privatekey, mc_platform->agency->known_host_filename);
  if( ret == 1  ){
    printf("Successfull Authenticate and but send of MA is failed \n");
#ifndef _WIN32
    if(close(skt) < 0) {
			SOCKET_ERROR();
		}
#else
    closesocket(skt);
#endif
    free(buf);
		fprintf(stderr, "Security Error. %s:%d\n", __FILE__, __LINE__);
    MSG_THREAD_EXIT();
  }else if(ret != 2){
#ifndef _WIN32
    if(close(skt) < 0) {
			SOCKET_ERROR();
		}
#else
    closesocket(skt);
#endif
    free(buf);
		fprintf(stderr, "Security Error. %s:%d\n", __FILE__, __LINE__);
    MSG_THREAD_EXIT();
  }else if(ret == 2) // successfully authenticate and send
    //printf("successfully authenticate and send \n");        
#else
    if(send(skt, message->message_body, strlen(message->message_body), 0) < 0) {
      perror("send");
      printf("cannot write to socket %s:%d\n", __FILE__, __LINE__);
    }
    else
#endif /* NEW_SECURITY */
    {
      // Now we should receive an HTTP response 
      buffer = (char*) malloc(sizeof(char) * (SOCKET_INPUT_SIZE + 2));
      CHECK_NULL(buffer, exit(0););
      mtp_http = mtp_http_New();
			message_string = dynstring_New();
			while(1) {
#ifndef _WIN32
/*				n = recvfrom(skt,
						(void *) buffer,
						(size_t) sizeof(char)*SOCKET_INPUT_SIZE,
						0,
						(struct sockaddr *) 0,
						(socklen_t *) 0);
*/
				n = recv(skt, (void*)buffer, (size_t) sizeof(char)*SOCKET_INPUT_SIZE, MSG_WAITALL);
#else
				n = recvfrom(skt,
						buffer,
						(size_t) sizeof(char)*SOCKET_INPUT_SIZE,
						0,
						(struct sockaddr *) 0,
						0);
#endif
				if (n<0) {
					/* There was an error receiving the response. */
					SOCKET_ERROR();
					free(buffer);
					MSG_THREAD_EXIT();
				}
				else if (n==0) {
					break;
				}
				else
				{
					dynstring_Append(message_string, buffer);
				}
			}
      if( mtp_http_Parse(mtp_http, message_string->message) ) {
        fprintf(stderr, "http parsing error: Response expected. %s:%d\n",
            __FILE__, __LINE__);
        fprintf(stderr, "Received message was:\n%s\n", message_string->message);
      }
      if (mtp_http->response_code != 200) {
        fprintf(stderr, "Warning: remote http server responded: %d %s\n",
            mtp_http->response_code, mtp_http->response_string );
      }

      mtp_http_Destroy(mtp_http);
    }

#ifndef _WIN32
  if(close(skt) <0) {
		SOCKET_ERROR();
	}
#else
  closesocket(skt);
#endif
  free(buf);
  free(buffer);
	dynstring_Destroy(message_string);
  MSG_THREAD_EXIT();
}

#undef MSG_THREAD_EXIT
