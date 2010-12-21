/* SVN FILE INFO
 * $Revision: 561 $ : Last Committed Revision
 * $Date: 2010-08-30 15:31:37 -0700 (Mon, 30 Aug 2010) $ : Last Committed Date */
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

#ifndef _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <pthread.h>
#include "config.h"
#if HAVE_LIBBLUETOOTH
#ifdef _WIN32
#include <Ws2bth.h>
#else
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#endif
#endif

#else
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include "winconfig.h"
#endif

#include <stdlib.h>
#include "include/acc.h"
#include "include/connection.h"
#include "include/data_structures.h"
#include "include/macros.h"
#include "include/mc_error.h"
#include "include/mc_platform.h"
#include "include/message.h"
#include "include/mtp_http.h"
#include "include/xml_parser.h"
#include "include/fipa_acl_envelope.h"

#define BACKLOG 200

acc_p
acc_Initialize(struct mc_platform_s* mc_platform)
{
  acc_p acc;
  acc = (acc_p)malloc(sizeof(acc_t));
  acc->mc_platform = mc_platform;

  acc->waiting = 0;
  acc->waiting_lock = (MUTEX_T*)malloc(sizeof(MUTEX_T));
  MUTEX_INIT(acc->waiting_lock);
  acc->waiting_cond = (COND_T*)malloc(sizeof(COND_T));
  COND_INIT(acc->waiting_cond);

	/* Initialize conn_thread stuff */
	MUTEX_INIT(&acc->conn_thread_lock);
	COND_INIT(&acc->conn_thread_cond);
	acc->num_conn_threads = 0;

	/* Initialize msg_thread stuff */
	MUTEX_INIT(&acc->msg_thread_lock);
	COND_INIT(&acc->msg_thread_cond);
	acc->num_msg_threads = 0;
  return acc;
}

  int 
acc_Destroy(acc_p acc)
{
  if(acc == NULL) {
    return MC_SUCCESS;
  }
  free(acc);
  acc = NULL;
  return MC_SUCCESS;
}

#ifndef _WIN32
  void*
acc_MessageHandlerThread(void* arg)
#else
  DWORD WINAPI
acc_MessageHandlerThread(LPVOID arg)
#endif
{
  mc_platform_p mc_platform = (mc_platform_p)arg;
  message_p message;
  agent_p agent;
  int mobile_agent_counter = 1;
  char* tmpstr;
  char* origname;
  int i;

  while(1) 
  {
    MUTEX_LOCK(mc_platform->message_queue->lock);
    MUTEX_LOCK(mc_platform->quit_lock);
    while(mc_platform->message_queue->size == 0 && !mc_platform->quit) {
      MUTEX_UNLOCK(mc_platform->quit_lock);
      COND_WAIT(
         mc_platform->message_queue->cond,
         mc_platform->message_queue->lock );
      MUTEX_LOCK(mc_platform->quit_lock);
    }
    if (mc_platform->message_queue->size == 0 && mc_platform->quit)
    {
			MUTEX_LOCK(&mc_platform->acc->msg_thread_lock);
			while(mc_platform->acc->num_msg_threads > 0) {
				COND_WAIT(
						&mc_platform->acc->msg_thread_cond,
						&mc_platform->acc->msg_thread_lock
						);
			}
			MUTEX_UNLOCK(&mc_platform->acc->msg_thread_lock);
      MUTEX_UNLOCK(mc_platform->quit_lock);
      MUTEX_UNLOCK(mc_platform->message_queue->lock);
      THREAD_EXIT();
    }

    MUTEX_UNLOCK(mc_platform->quit_lock);
    MUTEX_UNLOCK(mc_platform->message_queue->lock);
    message = message_queue_Pop(mc_platform->message_queue);
    if (message == NULL) {
      printf("POP ERROR\n");
      continue;
    }
    /* Send MC Signal */
    MUTEX_LOCK(mc_platform->MC_signal_lock);
    mc_platform->MC_signal = MC_RECV_MESSAGE;
    COND_BROADCAST(mc_platform->MC_signal_cond);
    MUTEX_UNLOCK(mc_platform->MC_signal_lock);
    MUTEX_LOCK(mc_platform->giant_lock);
    while(mc_platform->giant == 0) {
      COND_WAIT (
          mc_platform->giant_cond,
          mc_platform->giant_lock);
    }
    MUTEX_UNLOCK(mc_platform->giant_lock);
    /* See if the destination is here. If it is, process the message.
     * Otherwise, send the message to it's destination */
    if(message->to_address == NULL) {
      /* Process message. Create the agent if it is an agent, or
       * process the ACL if it's an ACL message. */
      switch(message->message_type) {
        case MOBILE_AGENT:
          agent = agent_Initialize(
              mc_platform,
              message,
              mobile_agent_counter);
          if (agent != NULL) {
            /* We need to make sure there are no agent-name collisions. */
            i = 1;
            if(agent_queue_SearchName(mc_platform->agent_queue, agent->name)) {
              origname = agent->name;
              while(agent_queue_SearchName(mc_platform->agent_queue, agent->name)) {
                /* An agent with a matching name was found! For now, let us just
                 * rename the agent. */
                tmpstr = (char*)malloc(sizeof(char) * strlen(origname) + 7);
                sprintf(tmpstr, "%s_%04d", origname, i);
                agent->name = tmpstr;
                i++;
              }
              fprintf(stderr, "Warning: Agent '%s' has been renamed to '%s'.\n",
                  origname, agent->name);
              free(origname);
            }
            mobile_agent_counter++;
            agent_queue_Add(
                mc_platform->agent_queue,
                agent);
          } else {
						fprintf(stderr, "agent_Initialize() failed. %s:%d\n", __FILE__, __LINE__);
					}
          message_Destroy(message);
          /* Send MC_Signal */
          MUTEX_LOCK(mc_platform->MC_signal_lock);
          mc_platform->MC_signal = MC_RECV_AGENT;
          COND_BROADCAST(mc_platform->MC_signal_cond);
          MUTEX_UNLOCK(mc_platform->MC_signal_lock);
          MUTEX_LOCK(mc_platform->giant_lock);
          while(mc_platform->giant == 0) {
            COND_WAIT(mc_platform->giant_cond,
                mc_platform->giant_lock);
          }
          MUTEX_UNLOCK(mc_platform->giant_lock);
          /* Set the ams to run */
          MUTEX_LOCK(mc_platform->ams->runflag_lock);
          mc_platform->ams->run = 1;
          COND_BROADCAST(mc_platform->ams->runflag_cond);
          MUTEX_UNLOCK(mc_platform->ams->runflag_lock);
          break;
        case FIPA_ACL:
          /* This case should never happen now, due to a reorganization
           * of MobileC. This enum will be deprecated in a future release. */
          break;
        case RETURN_MSG:
          /* Add the persistent neutral agent. */
          agent = agent_Initialize(
              mc_platform,
              message,
              mobile_agent_counter);
          if (agent != NULL) {
            MUTEX_LOCK(agent->lock);
            agent->datastate->persistent = 1;
            agent->agent_status = MC_AGENT_NEUTRAL;
            MUTEX_UNLOCK(agent->lock);
            mobile_agent_counter++;
            agent_queue_Add(
                mc_platform->agent_queue,
                agent);
          }
          message_Destroy(message);
          /* Send MC_Signal */
          MUTEX_LOCK(mc_platform->MC_signal_lock);
          mc_platform->MC_signal = MC_RECV_RETURN;
          COND_BROADCAST(mc_platform->MC_signal_cond);
          MUTEX_UNLOCK(mc_platform->MC_signal_lock);
          MUTEX_LOCK(mc_platform->giant_lock);
          while(mc_platform->giant == 0) {
            COND_WAIT(
                mc_platform->giant_cond,
                mc_platform->giant_lock);
          }
          MUTEX_UNLOCK(mc_platform->giant_lock);
          /* Set the ams to run */
          MUTEX_LOCK(mc_platform->ams->runflag_lock);
          mc_platform->ams->run = 1;
          COND_BROADCAST(mc_platform->ams->runflag_cond);
          MUTEX_UNLOCK(mc_platform->ams->runflag_lock);
          break;
        case RELAY:
        case REQUEST:
        case SUBSCRIBE:
        case CANCEL:
        case N_UNDRSTD:
        case QUER_IF:
        case QUER_REF:
        case AGENT_UPDATE:
          fprintf(stderr, "FIXME: Message type %d not processable.%s:%d\n",
              message->message_type, __FILE__, __LINE__ );
          message_Destroy(message);
          break;
        default:
          fprintf(stderr, "Unknown message type:%d %s:%d\n",
              message->message_type, __FILE__, __LINE__);
          message_Destroy(message);
      }
    } else {
      message_Send
        (
         mc_platform, message, mc_platform -> private_key 
        );
    }
  }
  THREAD_EXIT();
}


#define CONN_THREADS 40
#ifndef _WIN32
  void*
acc_Thread(void* arg)
#else
  DWORD WINAPI
acc_Thread( LPVOID arg )
#endif
{
  connection_p connection;
  mc_platform_p mc_platform = (mc_platform_p)arg;

	connection_thread_arg_t* connection_thread_arg;

	acc_t* acc = mc_platform->acc;

#ifndef _WIN32
	pthread_attr_t attr;
	pthread_attr_init(&attr);
#else
	int stack_size = 0;
#endif
	THREAD_T conn_thread;

  /* We want to watch the connectlist for incoming connections, initialize 
   * them into messages, and add the messages to the appropriate queue. */
  while(1) {
    connection = NULL;
    MUTEX_LOCK(mc_platform->connection_queue->lock);
    MUTEX_LOCK(mc_platform->quit_lock);
    while (
			(mc_platform->connection_queue->size == 0 ) && !mc_platform->quit) {
      MUTEX_UNLOCK(mc_platform->quit_lock);
      COND_WAIT(
          mc_platform->connection_queue->cond,
          mc_platform->connection_queue->lock
          );
      MUTEX_LOCK(mc_platform->quit_lock);
    }
    if 
      (
       mc_platform->connection_queue->size == 0 &&
       mc_platform->quit 
      )
      {
				MUTEX_LOCK(&mc_platform->acc->conn_thread_lock);
				while(mc_platform->acc->num_conn_threads > 0) {
					COND_WAIT(
						&mc_platform->acc->conn_thread_cond,
						&mc_platform->acc->conn_thread_lock
						);
				}
				MUTEX_UNLOCK(&mc_platform->acc->conn_thread_lock);
        MUTEX_UNLOCK(mc_platform->quit_lock);
        MUTEX_UNLOCK(mc_platform->connection_queue->lock);
        THREAD_EXIT();
      }
    MUTEX_UNLOCK(mc_platform->quit_lock);
    MUTEX_UNLOCK(mc_platform->connection_queue->lock);
    /* Send MC Signal */
    MUTEX_LOCK(mc_platform->MC_signal_lock);
    mc_platform->MC_signal = MC_RECV_CONNECTION;
    COND_BROADCAST(mc_platform->MC_signal_cond);
    MUTEX_UNLOCK(mc_platform->MC_signal_lock);

    /* Check the giant lock to make sure we can continue */
    MUTEX_LOCK(mc_platform->giant_lock);
    while (mc_platform->giant == 0) {
      COND_WAIT(
          mc_platform->giant_cond,
          mc_platform->giant_lock
          );
    }
    MUTEX_UNLOCK(mc_platform->giant_lock);

    /* Continue with normal operation */
    connection = connection_queue_Pop(mc_platform->connection_queue);
		connection_thread_arg = (connection_thread_arg_t*)malloc(sizeof(connection_thread_arg_t));
		connection_thread_arg->mc_platform = mc_platform;
		connection_thread_arg->connection = connection;
		MUTEX_LOCK(&acc->conn_thread_lock);
		while(acc->num_conn_threads > CONN_THREADS) {
			COND_WAIT(&acc->conn_thread_cond, &acc->conn_thread_lock);
		}
		acc->num_conn_threads++;
		MUTEX_UNLOCK(&acc->conn_thread_lock);
		THREAD_CREATE(&conn_thread, acc_connection_Thread, connection_thread_arg);
		THREAD_DETACH(conn_thread);
  }
}


#define CONNECT_THREAD_EXIT() \
	free(arg); \
	MUTEX_LOCK(&acc->conn_thread_lock); \
	acc->num_conn_threads--; \
	COND_SIGNAL(&acc->conn_thread_cond); \
	MUTEX_UNLOCK(&acc->conn_thread_lock); \
	THREAD_EXIT();

#ifndef _WIN32
  void*
acc_connection_Thread(void* arg)
#else
  DWORD WINAPI
acc_connection_Thread( LPVOID arg )
#endif
{
  mtp_http_p mtp_http;
	acc_t* acc;
	mc_platform_t* mc_platform;
	connection_t* connection;
  message_p message;
  fipa_acl_envelope_p fipa_envelope;
	int err;
	int i, j;
  agent_t* agent;
  fipa_message_string_p fipa_message_string;
  fipa_acl_message_p fipa_message;

	mc_platform = ((connection_thread_arg_t*)arg)->mc_platform;
	acc = ((connection_thread_arg_t*)arg)->mc_platform->acc;
	connection = ((connection_thread_arg_t*)arg)->connection;
	mtp_http = mtp_http_New();
	if ( mtp_http_InitializeFromConnection(mtp_http, connection, mc_platform->private_key ) )
	{
		fprintf(stderr, "mtp_http_InitializeFromConnection failed. %s:%d\n", __FILE__, __LINE__);
		connection_Destroy(connection);
		mtp_http_Destroy(mtp_http);
		CONNECT_THREAD_EXIT();
	}

	switch(mtp_http->http_performative)
	{
		case HTTP_POST:
		case HTTP_PUT:
			/* See if the incoming message is going to any of our services. */
			/* See if it is a message for the AMS */
			if(
					!strcmp(mtp_http->target, "/ams") ||
					!strcmp( strrchr(mtp_http->target, (int)'/'), "/ams" )
				) {
				message = message_New();
				/*message->message_parts = mtp_http->message_parts;*/
				message->message_body = (char*)malloc
					(
					 sizeof(char) * 
					 (strlen((char*)mtp_http->content->data)+1)
					);
				strcpy(message->message_body, (char*)mtp_http->content->data);
				message->xml_root = mxmlLoadString
					(
					 NULL,
					 message->message_body,
					 MXML_NO_CALLBACK
					);
				if (message->xml_root == NULL) {
					fprintf(stderr, "xml loadstring error. %s:%d\n", __FILE__, __LINE__);
				}
				if(message_xml_parse(message)) {
					fprintf(stderr, "Error parsing message. %s:%d\n",
							__FILE__,__LINE__);
					message_Destroy(message);
					mtp_http_Destroy(mtp_http);
					CONNECT_THREAD_EXIT();
				}
				mtp_http_Destroy(mtp_http);
				break;
			} else if 
				( 
				 !strcmp(mtp_http->target, "/acc") ||
				 !strcmp( strrchr(mtp_http->target, (int)'/'), "/acc")
				) {
					/* Make sure there are two parts to the html message.
					 * We expect to receive an xml envelope with a message attached. */
					if (mtp_http->message_parts != 2) {
						fprintf(stderr, "Error parsing message. %s:%d\n",
								__FILE__,__LINE__);
						mtp_http_Destroy(mtp_http);
						CONNECT_THREAD_EXIT();
					}
					/* Now we need to parse the envelope */
					fipa_envelope = fipa_acl_envelope_New();
					err = fipa_envelope_Parse(fipa_envelope, (char*)mtp_http->content[0].data);
					if (err) {
						fprintf(stderr, "Error parsing message. %s:%d\n",
								__FILE__, __LINE__);
						fipa_acl_envelope_Destroy(fipa_envelope);
						mtp_http_Destroy(mtp_http);
						CONNECT_THREAD_EXIT();
					}
					/* Now we need to check the envelope receivers and put copies of the message into
					 * each receiving agent's mailbox. */
					for(i = 0; i < fipa_envelope->num_params; i++) {
						char* portstr;
						for(j = 0; j < fipa_envelope->params[i]->to->num; j++) {
							agent = agent_queue_SearchName(
									mc_platform->agent_queue,
									fipa_envelope->params[i]->to->fipa_agent_identifiers[j]->name
									);
							if (agent != NULL) {
                // we do the following incase we have
                //   sda@localhost:5050
                //   sda@localhost:5051
                //   sda@localhost:5052
                // we don't want to post 3 times since agent_queue_SearchName
                //   will always return true
                // FIXME: A better way may be to make sure the port numbers match
                // 
                // we assume the addres is http://###.###.###.###:PORT/acc
                // we get the second ":" and then let atoi() handle the rest
                portstr = fipa_envelope->params[i]->to->fipa_agent_identifiers[j]->addresses->urls[0]->str;
                portstr = strstr(portstr, ":")+1;
                portstr = strstr(portstr, ":")+1;
                //agent->home_port is never set???
                if(agent->mc_platform->port != atoi(portstr)) continue;
 
								/* Parse, copy, and deliver the message to the agent. */
								fipa_message_string = fipa_message_string_New();
								fipa_message_string->message = strdup((char*)mtp_http->content[1].data);
								fipa_message_string->parse = fipa_message_string->message;
								fipa_message = fipa_acl_message_New();
								err = fipa_acl_Parse(fipa_message, fipa_message_string);
								if (err) {
									fipa_message_string_Destroy(fipa_message_string);
									fipa_acl_message_Destroy(fipa_message);
									fipa_acl_envelope_Destroy(fipa_envelope);
									mtp_http_Destroy(mtp_http);
									CONNECT_THREAD_EXIT();
								}
								agent_mailbox_Post( agent->mailbox, fipa_message);
								fipa_message_string_Destroy(fipa_message_string);
							}
						}
					}
					fipa_acl_envelope_Destroy(fipa_envelope);
					mtp_http_Destroy(mtp_http);
					CONNECT_THREAD_EXIT();
				}
			else {
				/* FIXME: We don't support other recipients yet */
				fprintf(stderr, "Unsupported. %s:%d\n", __FILE__, __LINE__);
				mtp_http_Destroy(mtp_http);
			}
		default:
			fprintf(stderr, "unsupported http performative. %s:%d\n",
					__FILE__, __LINE__);
	}

	/* If we get here, then we have an incoming message for the ams */
	connection_Destroy(connection);
	switch(message->message_type) {
		case RELAY:
		case REQUEST:
		case SUBSCRIBE:
		case CANCEL:
		case N_UNDRSTD:
		case MOBILE_AGENT:
		case QUER_IF:
		case QUER_REF:
		case AGENT_UPDATE:
		case RETURN_MSG:
		case FIPA_ACL:
			message_queue_Add(mc_platform->message_queue, message);
			break;
		default:
			fprintf(stderr, "Unknown message type:%d. Rejecting message.%s:%d\n",
					message->message_type,
					__FILE__, __LINE__ );
			free(message);
			break;
	}
	CONNECT_THREAD_EXIT();
}
#undef CONNECT_THREAD_EXIT


  void
acc_Start(mc_platform_p mc_platform)
{
  acc_p acc = mc_platform->acc;
#ifndef _WIN32
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  if(mc_platform->stack_size[MC_THREAD_ACC] != -1) {
  pthread_attr_setstacksize
    (
     &attr,
     mc_platform->stack_size[MC_THREAD_ACC]
    );
  }
#else
  int stack_size;
  if (mc_platform->stack_size[MC_THREAD_ACC] < 1) {
    /* In windows, 0 is default, not min */
    stack_size = mc_platform->stack_size[MC_THREAD_ACC]+1; 
  } else {
    stack_size = mc_platform->stack_size[MC_THREAD_ACC];
  }
#endif
  THREAD_CREATE
    ( 
     &acc->thread,
     acc_Thread,
     mc_platform 
    );
  THREAD_CREATE
    ( 
     &acc->message_handler_thread,
     acc_MessageHandlerThread,
     mc_platform
    );
  THREAD_CREATE
    (
     &acc->listen_thread,
     listen_Thread,
     mc_platform 
    );
  THREAD_CREATE
    (
     &acc->udplisten_thread,
     udplisten_Thread,
     mc_platform 
    );
}

int
auth_conn_rece_key(int sockfd, char *peer_name, int *nonce, unsigned char *aes_key, char *privkey, char* known_host_filename){
	int ret = -1;
	//char passphrase[35];
	char privatekey[1210]; 
	char peer_pubkey[300];
	char plaintext[135];
	
	//memset(passphrase, '\0', 35);
	memset(privatekey, '\0', 1210);
	//memset(peer_pubkey,'\0', 300);
	memset(plaintext,  '\0', 135);
	memset(aes_key, '\0', 35);

	strcpy(privatekey, privkey);
	//printf("Authenticating %s \n", peer_name);
	
  /* Known host file lookup for peer public key */  
  if (read_known_host_file(peer_pubkey, peer_name, known_host_filename) == -1 ){
     printf("Server: %s 's Public key not found in known host file\n",peer_name);

  }else{     
     if ( (ret=reply_migration_process(sockfd, nonce, peer_pubkey, privatekey, aes_key)) != 1){
        if (ret == -1)
            printf("Server: Connected peer is not authenticated \n");
        if (ret == -2)
            printf("Server: Unable to get authentication from other peer \n");
	 }else{ 
		 if(ret == 2)
		     ret = 2; // incase of Acl message in unencrypted form
		 else
		     ret = 1;		
      }
  }
 return ret;
}


#ifndef _WIN32
  void*
listen_Thread(void* arg)
#else
  DWORD WINAPI
listen_Thread( LPVOID arg )
#endif
{
#ifndef _WIN32
  int connectionsockfd; 
  int sockfd;
  struct sockaddr_in sktin;
  struct sockaddr_in peer_addr;
#if HAVE_LIBBLUETOOTH
#ifdef _WIN32
  SOCKADDR_BTH loc_addr = { 0 }, rem_addr = { 0 };
#else
  struct sockaddr_rc loc_addr = { 0 };
#endif
#endif
#else
  SOCKET connectionsockfd;
  SOCKET sockfd;
  struct sockaddr_in sktin;
  struct sockaddr_in peer_addr;

  struct hostent *remoteHost;
    struct in_addr addr;

#endif

#ifdef NEW_SECURITY
  int ret;
  int nonce;
  unsigned char aes_key[36];
#endif

  char peer_name[45];	
  connection_p connection;
  u_long connection_number;
  int connectionlen;
#ifndef _WIN32
  int yes = 1;
#else
  bool yes = true;
#endif
  mc_platform_p mc_platform = (mc_platform_p)arg;

  /* basic initialization */
  connection_number = 0;


  if (mc_platform->bluetooth == 0) {
    connectionlen = sizeof(struct sockaddr_in);
  } else {
#if HAVE_LIBBLUETOOTH
#ifdef _WIN32
    connectionlen = sizeof(SOCKADDR_BTH);
#else
    connectionlen = sizeof(struct sockaddr_rc);
#endif
#else
    fprintf(stderr, "ERROR: This copy of Mobile-C was not compiled with Bluetooth support, but\n"
     "bluetooth support was requested in the Mobile-C options. Please re-compile \n"
     "Mobile-C with Bluetooth support.\n");
    exit(-1);
#endif
  }

  /* Set up the socket */
  if(mc_platform->bluetooth) {
#if HAVE_LIBBLUETOOTH
#ifndef _WIN32
    sockfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
#else
    sockfd = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM); 
#endif
#endif
  } else {
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
  }
	if (sockfd < 0) {
		SOCKET_ERROR();
	}

  mc_platform->sockfd = sockfd;
#ifndef _WIN32
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
#else
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(int));
#endif

  if(mc_platform->bluetooth) {
#if HAVE_LIBBLUETOOTH
#ifndef _WIN32
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) mc_platform->agency->portno;
#else
    loc_addr.addressFamily= AF_BTH;
    loc_addr.btAddr = 0;
    loc_addr.port = mc_platform->agency->portno;
#endif
    if(bind(sockfd, (struct sockaddr *)&loc_addr, sizeof(loc_addr))) {
      fprintf(stderr, "error binding to bluetooth socket. %s:%d\n", 
        strerror(errno), errno);
      exit(1);
    }
#endif
  } else {
    sktin.sin_family = AF_INET;
    sktin.sin_port = htons(mc_platform->port);
    sktin.sin_addr.s_addr = INADDR_ANY;
    memset(sktin.sin_zero, '\0', sizeof sktin.sin_zero);
    while(bind(sockfd, (struct sockaddr *)&sktin, sizeof(struct sockaddr))
        == -1) {
      fprintf(stderr, "bind() error. %s:%d\n",
          __FILE__, __LINE__ );
#ifndef _WIN32
      sleep(1);
#else
      Sleep(1000);
#endif
    }
  }
  listen(sockfd, BACKLOG);

  /* this while loop runs continuously creating connections */
  while(1)
  {
    /* Set waiting flag */
    MUTEX_LOCK(mc_platform->acc->waiting_lock);
    mc_platform->acc->waiting = 1;
    COND_BROADCAST(mc_platform->acc->waiting_cond);
    MUTEX_UNLOCK(mc_platform->acc->waiting_lock);
#ifdef _WIN32
    if(mc_platform->bluetooth) {
        fprintf(stderr, "Warning: Windows Mobile-C Bluetooth Agencies only support sending agents.\nReceiving agents is unsupported. Disabling ACC listen thread...\n");
        while(1) {
            Sleep(10000);
        }
    }
#endif
    if(mc_platform->bluetooth == 0) {
#ifndef _WIN32
      if((connectionsockfd = accept(sockfd, 
              (struct sockaddr *)&peer_addr, 
              (socklen_t *)&connectionlen)) < 0)
#else
        if((connectionsockfd = accept(sockfd,
                (struct sockaddr *)&peer_addr,
                (int*)&connectionlen)) == INVALID_SOCKET)
#endif
        {
          fprintf(stderr, "ListenThread: accept error \n");
#ifdef _WIN32
          printf("Error number: %d\n", WSAGetLastError() );
#endif
          continue;
        }
    } else {
#if HAVE_LIBBLUETOOTH
#ifndef _WIN32
      if((connectionsockfd = accept(sockfd, 
              (struct sockaddr *)&peer_addr, 
              (socklen_t *)&connectionlen)) < 0)
#else
        if((connectionsockfd = accept(sockfd,
                (struct sockaddr *)&rem_addr,
                (int*)&connectionlen)) == INVALID_SOCKET)
#endif
        {
          fprintf(stderr, "ListenThread: accept error \n");
#ifdef _WIN32
          printf("Error number: %d\n", WSAGetLastError() );
#endif
          continue;
        }
#endif
    }
    if(connectionsockfd > 0) 
    {
      /* See if the quit flag is enabled. If it is, we should terminate instead
       * of accepting more connections. */
      if(mc_platform->quit) {
        break;
      }

      /* Acquire host name of connected   */
#ifndef _WIN32
      getnameinfo((const struct sockaddr*)&peer_addr, sizeof(peer_addr),
          peer_name, sizeof(peer_name), NULL, 0, 0);
#else
      addr.s_addr = inet_addr( inet_ntoa(peer_addr.sin_addr) );
      if (addr.s_addr == INADDR_NONE) 
        printf("The IPv4 address entered must be a legal address\n");
      else
        remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);

      memset(peer_name, '\0', 45 );
      strcpy(peer_name, remoteHost->h_name);

#endif
      //printf("peer_name = %s \n", peer_name);

#ifdef NEW_SECURITY
      /* Authentication Process for Received Connection  */
      ret = auth_conn_rece_key(connectionsockfd, peer_name, &nonce, aes_key, mc_platform->private_key, mc_platform->agency->known_host_filename);
      if( ret == 2 || ret == 1){
        //printf("Successfully Authenticate %s \n", peer_name);
        // Adding connection in connection_queue 
#endif /* NEW_SECURITY */
        /* Set waiting flag */
        MUTEX_LOCK(mc_platform->acc->waiting_lock);
        mc_platform->acc->waiting = 0;
        COND_BROADCAST(mc_platform->acc->waiting_cond);
        MUTEX_UNLOCK(mc_platform->acc->waiting_lock);
        /* create a new connection and insert it into the connection linked 
         * list */
        connection = connection_New();
        connection->connect_id = rand();
        connection->remote_hostname = NULL;
        connection->addr = peer_addr;
        connection->serverfd = sockfd;
        connection->clientfd = connectionsockfd;
#ifdef NEW_SECURITY
        connection->nonce = nonce;
        connection->AES_key = aes_key;
#endif /* NEW_SECURITY */

        /* add the connection to list and increment the number of connections */
        connection_queue_Add(mc_platform->connection_queue, connection);
#ifdef NEW_SECURITY
      }else{ 
        printf("Unable to authenticate %s \n", peer_name);
      }
#endif /* NEW_SECURITY */
    }
  }

#ifndef _WIN32
  if(close(sockfd) != 0) 
#else
  if(closesocket(sockfd) != 0) 
#endif
  {
    fprintf(stderr, "Socket close failed: %d\n", errno);
  } else {
    printf("close success.\n");
  }

  /* Free the current thread */
  THREAD_EXIT();
}

#define BUFLEN 512
#define UDPPORT 8866
#ifndef _WIN32
  void*
udplisten_Thread(void* arg)
#else
  DWORD WINAPI
udplisten_Thread( LPVOID arg )
#endif
{
  mc_platform_p mc_platform = (mc_platform_p)arg;
	struct sockaddr_in si_me, si_remote;
	int s;
#ifndef _WIN32
  unsigned int slen = sizeof(si_remote);
#else
  int slen = sizeof(si_remote);
#endif
	char buf[BUFLEN];
	char mc_req_string[] = "Mobile-C Agency Information Request";

	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1 ) {
		perror("Socket error.");	
		exit(1);
	}

	memset((char*) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(UDPPORT);
	si_me.sin_addr.s_addr = htons(INADDR_ANY); 

	if(bind(s, (const struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
		/* Some other agency is already on this host listening. Just disable the udp thread for now. */
		return NULL;
	}
	while(1) {
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_remote, &slen)==-1){
			perror("recvfrom");
			continue;
		}
		/* Received a udp packet. Let's inspect the contents. */
		/* It should read :
		   Mobile-C Agency Information Request
			 Version <Sending-Agency's-Version>
		*/

    if(strncmp(buf, mc_req_string, strlen(mc_req_string)) == 0) {
			sprintf(buf, "Mobile-C Version %s\n%s:%d\n", 
			PACKAGE_VERSION, mc_platform->hostname, mc_platform->port);
			if(sendto(s, buf, strlen(buf)+1, 0, (const struct sockaddr*)&si_remote, slen) == -1) {
				perror("sendto");
			}
		} else {
			sprintf(buf, "Message not understood.");
			if(sendto(s, buf, strlen(buf)+1, 0, (const struct sockaddr*)&si_remote, slen) == -1) {
				perror("sendto");
			}
		}
	}
}
#undef BUFLEN
#undef UDPPORT
