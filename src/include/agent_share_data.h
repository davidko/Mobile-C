#ifndef _AGENT_SHARE_DATA_H_
#define _AGENT_SHARE_DATA_H_

typedef struct agent_share_data_s {
  size_t size;
  char* name;
  void* data;
} agent_share_data_t;

typedef agent_share_data_t* agent_share_data_p;

agent_share_data_p agent_share_data_New();

agent_share_data_p agent_share_data_Copy(agent_share_data_p agent_share_data);

int agent_share_data_Destroy(agent_share_data_p agent_share_data);

int agent_share_data_CmpName(const char* key, agent_share_data_t* asd);
#endif
