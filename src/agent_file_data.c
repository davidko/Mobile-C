#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/agent_file_data.h"
#include <b64/cencode.h>

agent_file_data_p agent_file_data_New(void)
{
  agent_file_data_p afd;
  afd = (agent_file_data_p)malloc(sizeof(agent_file_data_t));
  afd->name = NULL;
  afd->data = NULL;
  return afd;
}

agent_file_data_p agent_file_data_NewWithData(const char* name, const char* data)
{
  agent_file_data_p afd;
  afd = agent_file_data_New();
  afd->name = strdup(name);
  afd->data = strdup(data);
  return afd;
}

agent_file_data_p agent_file_data_Copy(agent_file_data_p src)
{
  agent_file_data_p afd;
  afd = (agent_file_data_p)malloc(sizeof(agent_file_data_t));
  if(src->name) {
    afd->name = strdup(src->name);
  } else {
    afd->name = NULL;
  }
  if(src->data) {
    strcpy(afd->data, src->data);
  } else {
    afd->data = NULL;
  }
  return afd;
}

agent_file_data_p
agent_file_data_InitializeFromFilename(const char* filename)
{
  FILE* fp;
  long lSize;
  size_t result;
  char* buffer;
  agent_file_data_p afd;
  base64_encodestate estate;
  int codeSize;

  fp = fopen(filename, "rb");
  if(fp == NULL) {
    return NULL;
  }

  // Get file size
  fseek(fp, 0, SEEK_END);
  lSize = ftell(fp);
  rewind(fp);

  /* Allocate memory */
  buffer = (char*)malloc(lSize * sizeof(char));
  result = fread(buffer, 1, lSize, fp);
  if(result != lSize) {
    free(buffer);
    return NULL;
  }

  /* Allocate data structure */
  base64_init_encodestate(&estate);
  afd = (agent_file_data_p)malloc(sizeof(agent_file_data_t));
  afd->name = strdup(filename);
  afd->data = (char*)malloc(lSize*2*sizeof(char));
  codeSize = base64_encode_block(buffer, lSize, afd->data, &estate);
  base64_encode_blockend(afd->data + codeSize, &estate);
  fclose(fp);
  return afd;
}

agent_file_data_p
agent_file_data_Encode(const char* name, void* data, int size)
{
  return NULL;
}

int agent_file_data_Destroy(agent_file_data_p agent_file_data)
{
  if(agent_file_data->name) {
    free(agent_file_data->name);
    agent_file_data->name = NULL;
  }
  if(agent_file_data->data) {
    free(agent_file_data->data);
  }
  free(agent_file_data);
  return 0;
}
