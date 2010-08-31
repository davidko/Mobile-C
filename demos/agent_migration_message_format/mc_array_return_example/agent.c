#include <stdio.h>
int a[2][3][2];
int main()
{
    int i, j, k, l;
    k = 0;
    printf("\nThis is a mobile agent from local_port 5050.\n");
    printf("I am performing the task on the agency at local_port 5051 now.\n");
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            for(l = 0; l < 2; l++) {
                a[i][j][l] = k;
                k++;
                printf("%d ", i+j);
            }
        }
    }
    
    return 0;
}
