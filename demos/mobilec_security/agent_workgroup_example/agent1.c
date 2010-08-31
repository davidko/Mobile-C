#include <stdio.h>

int main()
{
  printf("agent1 reporting for duty.\n");
  while(1) {
    printf("agent1 running 2 second task...\n");
    sleep(2);
    printf("agent1 done performing task.\n");
  }
  return 0;
}
