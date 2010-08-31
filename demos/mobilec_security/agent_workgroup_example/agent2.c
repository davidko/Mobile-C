#include <stdio.h>

int main()
{
  printf("agent2 reporting for duty.\n");
  while(1) {
    printf("agent2 running 2 second task...\n");
    sleep(2);
    printf("agent2 done performing task.\n");
  }
  return 0;
}
