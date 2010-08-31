/* file: multi_task_example/task1.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

double results_task1;

int main()
{
    FILE * fptr;
    char line[1024];

    printf("TASK 1\n");
    printf("\nThis is the mobile agent 3.\n\n");
    printf("Reading data file...\n");
    if((fptr = fopen("ChDataFile_iel2", "r")) == NULL) 
    {
      printf("Error: could not open file 'ChDataFile_iel2'.\n");
      exit(EXIT_FAILURE);
    }

    fgets(line, sizeof(line), fptr);
    results_task1 = atof(line);

    fclose(fptr);
    printf("I am leaving to go to the next host.\n");

    return 0;
}
