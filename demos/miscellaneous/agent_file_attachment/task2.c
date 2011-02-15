/* File: miscellaneous/task2.c */

int main()
{
  char** files;
  int num_files;
  int i;
  int status;
  printf("Hello. Now retrieving file...\n");

  mc_AgentListFiles(mc_current_agent, 0, &files, &num_files);
  printf("%d saved files:\n", num_files);
  for(i = 0; i < num_files; i++) {
    printf("%s\n", files[i]);
  }
  status = mc_AgentRetrieveFile(mc_current_agent, 0, "data", "data_retrieved.png");
  if(status){
    printf("Error retrieving file.\n");
  }
  return 0;
}
