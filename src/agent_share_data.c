#include <string.h>
#include <stdlib.h>
#include "include/agent_share_data.h"

agent_share_data_p agent_share_data_New()
{
  agent_share_data_p asd;
  asd = (agent_share_data_p)malloc(sizeof(agent_share_data_t));
  memset(asd, 0, sizeof(agent_share_data_t));
  return asd;
}

agent_share_data_p agent_share_data_Copy(agent_share_data_p agent_share_data)
{
  agent_share_data_p asd;
  asd = (agent_share_data_p)malloc(sizeof(agent_share_data_t));
  asd->size = agent_share_data->size;
  asd->name = strdup(agent_share_data->name);
  asd->data = malloc(agent_share_data->size);
  memcpy(asd->data, agent_share_data->data, agent_share_data->size);
  return asd;
}

int agent_share_data_Destroy(agent_share_data_p agent_share_data)
{
  if(agent_share_data->name)
    free(agent_share_data->name);
  if(agent_share_data->data)
    free(agent_share_data->data);
  return 0;
}

int agent_share_data_CmpName(const char* key, agent_share_data_t* asd)
{
  if(asd->name == NULL) {
    return -1;
  } 
  return strcmp(key, asd->name);
}
