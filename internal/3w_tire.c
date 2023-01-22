//
// Created by zr on 23-1-21.
//
#include "3w_tire.h"
#include <stdlib.h>

static comp_tire_t* get(comp_tire_t* t,  comp_str_t s, int d)
{
    if(!t) return NULL;
    u_char c = comp_str_at(s, d);
    if(c < t->c) return get(t->left, s, d);
    else if(c > t->c) return get(t->right, s, d);
    else if(d < comp_str_len(s) - 1) return get(t->mid, s, d + 1);
    else return t;
}

comp_tire_t* comp_tire_get(comp_tire_t* t, comp_str_t s)
{
    return get(t, s, 0);
}

comp_tire_t* put(comp_tire_t* t, comp_str_t s, TIRE_VALUE_TYPE v, int d)
{
    u_char c = comp_str_at(s, d);
    if(!t)
    {
        t = (comp_tire_t*) malloc(sizeof(comp_tire_t));
        t->left = t->right = t->mid = NULL;
        t->c = c;
    }
    if(c < t->c) t->left = put(t->left, s, v, d);
    else if(c > t->c) t->right = put(t->right, s, v, d);
    else if(d < comp_str_len(s) - 1) t->mid = put(t->mid, s, v, d + 1);
    else t->value = v;
    return t;
}

comp_tire_t* comp_tire_put(comp_tire_t* t, comp_str_t s, TIRE_VALUE_TYPE v)
{
    return put(t, s, v, 0);
}

void comp_tire_free(comp_tire_t* root)
{
    if(!root) return;
    comp_tire_free(root->left);
    comp_tire_free(root->mid);
    comp_tire_free(root->right);
    free(root);
}