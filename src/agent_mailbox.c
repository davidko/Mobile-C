/* SVN FILE INFO
 * $Revision: 316 $ : Last Committed Revision
 * $Date: 2009-04-29 16:40:54 -0700 (Wed, 29 Apr 2009) $ : Last Committed Date */
#ifndef _WIN32
#include "config.h"
#else
#include "winconfig.h"
#endif

#include "include/data_structures.h"

agent_mailbox_p agent_mailbox_New(void)
{
  agent_mailbox_p mailbox;
  mailbox = (agent_mailbox_p)malloc(sizeof(agent_mailbox_t));
  memset(mailbox, 0, sizeof(agent_mailbox_t));
  mailbox->mail_queue = mail_queue_New();
  return mailbox;
}

agent_mailbox_p agent_mailbox_Copy(agent_mailbox_p src)
{
  agent_mailbox_p tmp;
  tmp = agent_mailbox_New();
  tmp->mail_queue = mail_queue_Copy(src->mail_queue);
  return tmp;
}

int agent_mailbox_Destroy(agent_mailbox_t* mailbox)
{
  if (mailbox == NULL) return 0;
  mail_queue_Destroy(mailbox->mail_queue);
  free(mailbox);

  return 0;
}

int agent_mailbox_Post(agent_mailbox_p mailbox, fipa_acl_message_t* message)
{
  mail_queue_Add(mailbox->mail_queue, message);
  return 0;
}

fipa_acl_message_t* agent_mailbox_Retrieve(agent_mailbox_p mailbox)
{
  return mail_queue_Pop(mailbox->mail_queue);
}

fipa_acl_message_t* agent_mailbox_WaitRetrieve(agent_mailbox_p mailbox)
{
  MUTEX_LOCK(mailbox->mail_queue->lock);
  while ( mailbox->mail_queue->size == 0 ) {
    COND_WAIT(mailbox->mail_queue->cond, mailbox->mail_queue->lock);
  }
  MUTEX_UNLOCK(mailbox->mail_queue->lock);
  return agent_mailbox_Retrieve(mailbox);
}

