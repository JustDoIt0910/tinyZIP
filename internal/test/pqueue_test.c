//
// Created by zr on 23-1-13.
//
#include "../pqueue.h"
#include <stdio.h>
#include <stdlib.h>

int less(const void* a, const void* b)
{
    return *(const int*)a > *(const int*)b;
}

int main()
{
    comp_pqueue_t* pq = comp_pqueue_init(10, less);
    if(!pq)
        return 0;
    int arr[] = {1, 1, 5, 2, 1, 2};
    for(int i = 0; i < 6; i++)
        comp_pqueue_insert(pq, &arr[i]);
//    for(int i = 0; i < 12; i++)
//        printf("%d ", *(int*) comp_pqueue_pop(pq));
    for(int i = 0; i < 2; i++)
    {
        int* x = comp_pqueue_pop(pq);
        int* y = comp_pqueue_pop(pq);
        int* z = malloc(sizeof(int));
        *z = *x + *y;
        comp_pqueue_insert(pq, z);
    }
    size_t sz = comp_pqueue_size(pq);
//    for(int i = 0; i < sz; i++)
//        printf("%d ", *(int*) pq->pq[i + 1]);
    for(int i = 0; i < sz; i++)
        printf("%d ", *(int*) comp_pqueue_pop(pq));
    comp_pqueue_destroy(pq);
}