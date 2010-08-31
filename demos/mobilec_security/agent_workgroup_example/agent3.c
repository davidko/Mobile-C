int main()
{
  printf("agent3 has arrived with no workgroup code.\n");
  printf("agent3 here. Attempting to kill agent1...\n");
  mc_DeleteAgent("agent1");
  printf("agent3 here. Attempting to kill agent2...\n");
  mc_DeleteAgent("agent2");
  printf("agent3 done. Exitting...\n");
  return 0;
}
