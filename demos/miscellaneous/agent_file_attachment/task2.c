/* File: miscellaneous/task2.c */

int main()
{
  char** files;
  int num_files;
  int i;
  printf("Hello. Now retrieving file...\n");

  mc_AgentListFiles(mc_current_agent, 0, &files, &num_files);
  printf("%d saved files:\n", num_files);
  for(i = 0; i < num_files; i++) {
    printf("%s\n", files[i]);
  }
  mc_AgentRetrieveFile(mc_current_agent, 0, "data", "data_retrieved.png");
  return 0;
}
