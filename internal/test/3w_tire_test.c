//
// Created by zr on 23-1-21.
//
#include "../3w_tire.h"
#include <stdio.h>

int main()
{
    comp_tire_t* root = NULL;
    comp_str_t s1 = comp_str_new("test");
    comp_str_t s2 = comp_str_new("tire");
    comp_str_t s3 = comp_str_new("abcdef");
    comp_str_t s4 = comp_str_new("abcxyz");
    root = comp_tire_put(root, s1, 10);
    root = comp_tire_put(root, s2, 3);
    root = comp_tire_put(root, s3, 7);
    root = comp_tire_put(root, s4, 2);


    comp_tire_t* t = comp_tire_get(root, s1);
    if(t) printf("found s1, value = %u\n", t->value);
    t = comp_tire_get(root, s2);
    if(t) printf("found s2, value = %u\n", t->value);
    t = comp_tire_get(root, s3);
    if(t) printf("found s3, value = %u\n", t->value);
    t = comp_tire_get(root, s4);
    if(t) printf("found s4, value = %u\n", t->value);
    s4 = comp_str_assign(s4, "none");
    t = comp_tire_get(root, s4);
    if(!t) printf("s4 not found\n");

    comp_str_free(s1);
    comp_str_free(s2);
    comp_str_free(s3);
    comp_str_free(s4);
    comp_tire_free(root);
    return 0;
}