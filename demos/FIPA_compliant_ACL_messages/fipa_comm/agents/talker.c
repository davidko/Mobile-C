#include <stdio.h>
#include <mobilec.h>

int main() {
    fipa_acl_message_p acl = mc_AclNew();

    printf("%s: Started at %s\n", mc_agent_name, mc_agent_address);

    mc_AclSetPerformative(acl, FIPA_REQUEST);
    mc_AclSetSender(acl, mc_agent_name, mc_agent_address);
    mc_AclSetContent(acl, "SOMETHING");
    mc_AclAddReceiver(acl, "listen", "http://127.0.0.1:8866/acc");
    mc_AclSend(acl);
    mc_AclDestroy(acl);

    acl = mc_AclWaitRetrieve(mc_current_agent);
    printf("%s: Got reply from \n\t%s@%s\n\t'%s'\n", 
      mc_agent_name, acl->sender->name, acl->sender->addresses->urls[0]->str, acl->content->content);
    mc_AclDestroy(acl);

    acl = mc_AclNew();
    mc_AclSetPerformative(acl, FIPA_REQUEST);
    mc_AclSetSender(acl, mc_agent_name, mc_agent_address);
    mc_AclSetContent(acl, "DATA");
    mc_AclAddReceiver(acl, "listen", "http://127.0.0.1:8866/acc");
    mc_AclSend(acl);
    mc_AclDestroy(acl);

    acl = mc_AclWaitRetrieve(mc_current_agent);
    printf("%s: Got reply from \n\t%s@%s\n\t'%s'\n", 
      mc_agent_name, acl->sender->name, acl->sender->addresses->urls[0]->str, acl->content->content);

    mc_AclDestroy(acl);

    acl = mc_AclNew();
    mc_AclSetPerformative(acl, FIPA_INFORM);
    mc_AclSetSender(acl, mc_agent_name, mc_agent_address);
    mc_AclAddReceiver(acl, "listen", "http://127.0.0.1:8866/acc");
    mc_AclSend(acl);
    mc_AclDestroy(acl);

    acl = mc_AclWaitRetrieve(mc_current_agent);
    printf("%s: Got reply from \n\t%s@%s\n\t'%s'\n", 
      mc_agent_name, acl->sender->name, acl->sender->addresses->urls[0]->str, acl->content->content);
    mc_AclDestroy(acl);

    return 0;
}
