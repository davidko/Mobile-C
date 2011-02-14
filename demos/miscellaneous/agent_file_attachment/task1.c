/* File: miscellaneous/task1.c */

int main()
{
  printf("Hello. Now attaching file...\n");
  mc_AgentAttachFile(mc_current_agent, "data", "data.png");
  return 0;
}
