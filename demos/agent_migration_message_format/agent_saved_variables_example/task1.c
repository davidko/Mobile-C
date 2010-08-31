#include <stdio.h>
#include <math.h>
int savevar;
int another_savevar;
int array_savevar[10];
int main()
{
  int i;
  printf("Hello World!\n");
  printf("This is mobagent1 from the agency at port 5050.\n");
  printf("I am performing the task on the agency at port 5051 now.\n");
  printf("%f\n", hypot(1,2)); 
  savevar = 10;
  another_savevar = 20;
  mc_AgentVariableSave(mc_current_agent, "savevar");
  mc_AgentVariableSave(mc_current_agent, "another_savevar");
  for(i = 0; i < 10; i++) {
    array_savevar[i] = i*3;
  }
  mc_AgentVariableSave(mc_current_agent, "array_savevar");
  return 0;
}
