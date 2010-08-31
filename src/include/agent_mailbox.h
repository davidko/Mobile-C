/* SVN FILE INFO
 * $Revision: 174 $ : Last Committed Revision
 * $Date: 2008-06-24 10:50:29 -0700 (Tue, 24 Jun 2008) $ : Last Committed Date */

#ifndef _AGENT_MAILBOX_H_
#define _AGENT_MAILBOX_H_

typedef struct agent_mailbox_s
{
  struct mail_queue_s* mail_queue;
} agent_mailbox_t;
typedef agent_mailbox_t* agent_mailbox_p;

agent_mailbox_p agent_mailbox_New(void);

agent_mailbox_p agent_mailbox_Copy(agent_mailbox_p src);

int agent_mailbox_Destroy(agent_mailbox_t* mailbox);

int agent_mailbox_Post(agent_mailbox_p mailbox, fipa_acl_message_t* message);

fipa_acl_message_t* agent_mailbox_Retrieve(agent_mailbox_p mailbox);

fipa_acl_message_t* agent_mailbox_WaitRetrieve(agent_mailbox_p mailbox);
#endif
