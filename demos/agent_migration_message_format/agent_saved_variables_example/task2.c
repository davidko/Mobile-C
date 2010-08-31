#include <stdio.h>
int retvar;
int main()
{
  const int *i;
  i = (int*)mc_AgentVariableRetrieve(mc_current_agent, "savevar", 0);
  if (i==NULL) {
    printf("Variable 'savevar' not found.\n");
  } else {
    printf("Variable 'savevar' has value %d.\n", *i);
  }
  retvar = *i*2;
  return 0;
}
