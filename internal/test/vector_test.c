//
// Created by zr on 23-1-14.
//
#include "../vector.h"
#include <stdio.h>

int int_less(const void* a, const void* b)
{
    return *(const int*)a < *(const int*)b;
}

struct my_type
{
    char a;
    int b;
};

int my_type_less(const void* a, const void* b)
{
    const struct my_type* t1 = a;
    const struct my_type* t2 = b;
    if(t1->a < t2->a)
        return 1;
    if(t1->a == t2->a)
        return t1->b < t2->b;
    return 0;
}

int main()
{
    int a[] = {2, 3, 7, 1, 5, 9, 4, 8, 100, 31, 24};
    comp_vec_t* v = comp_vec_init(10);
//    for(int i = 0; i < 11; i++)
//        comp_vec_push_back(v, &a[i]);
//    comp_vec_sort(v, 0, comp_vec_len(v) - 1, int_less);
//    for(int i = 0; i < comp_vec_len(v); i++)
//        printf("%d ", *(int*)comp_vec_get(v, i));
//    printf("\n");
//    comp_vec_clear(v);
    struct my_type b[] = {{3, 7}, {2, 1},
            {3, 6}, {1, 9},
            {7, 4}, {6, 2},
            {6, 8}, {10, 4}};
    for(int i = 0; i < 8; i++)
        comp_vec_push_back(v, &b[i]);
    for(int i = 0; i < comp_vec_len(v); i++)
        printf("{%d, %d} ", ((struct my_type*)comp_vec_get(v, i))->a, ((struct my_type*)comp_vec_get(v, i))->b);
    printf("\n");
    comp_vec_sort(v, 0, comp_vec_len(v) - 1, my_type_less);
    for(int i = 0; i < comp_vec_len(v); i++)
        printf("{%d, %d} ", ((struct my_type*)comp_vec_get(v, i))->a, ((struct my_type*)comp_vec_get(v, i))->b);
    printf("\n");
    return 0;
}