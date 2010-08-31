int main()
{
  printf("agent4 has arrived with workgroup code \"12345abCD\".\n");
  printf("agent4 here. Attempting to kill agent1...\n");
  mc_DeleteAgentWG(mc_current_agent, "agent1");
  printf("agent4 here. Attempting to kill agent2...\n");
  mc_DeleteAgentWG(mc_current_agent, "agent2");
  printf("agent4 done. Exiting...\n");
  return 0;
}
