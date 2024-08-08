#include <stdio.h>
//#include "ff_malloc.c"
#include "bf_malloc.c"


int main(int argc, char *argv[])
{
    int *arr[15];
    for (int i = 0; i < 10; i++)
    {
        arr[i] = (int *)bf_malloc(sizeof(union header) * (i + 1));
    }
    for (int i = 3; i < 5; i++)
    {
        free(arr[i]);
    }
    for (int i = 7; i < 9; i++)
    {
        free(arr[i]);
    }
    for (int i = 10; i < 15; i++)
    {
        arr[i] = (int *)bf_malloc(sizeof(union header) * (i/2));
    }
    for (int i = 0; i < 3; i++)
    {
        free(arr[i]);
    }
    for (int i = 5; i < 7; i++)
    {
        free(arr[i]);
    }
    for (int i = 9; i < 15; i++)
    {
        free(arr[i]);
    }
    return 0;
}