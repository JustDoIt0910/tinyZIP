//
// Created by zr on 23-1-14.
//
#include "vector.h"
#include <stdlib.h>

comp_vec_t* comp_vec_init(size_t size)
{
    comp_vec_t* v = (comp_vec_t*) malloc(sizeof(comp_vec_t));
    if(!v) return NULL;
    v->data = (void*) malloc(sizeof(void*) * size);
    if(!v->data)
    {
        free(v);
        return NULL;
    }
    v->len = 0;
    v->cap = size;
    return v;
}

void comp_vec_free(comp_vec_t* v)
{
    if(!v) return;
    if(v->data)
        free(v->data);
    free(v);
}

void* comp_vec_get(comp_vec_t* v, size_t i)
{
    if(!v || i >= v->len)
        return NULL;
    return v->data[i];
}

size_t comp_vec_len(comp_vec_t* v)
{
    if(!v) return 0;
    return v->len;
}

void comp_vec_push_back(comp_vec_t* v, void* p)
{
    if(v->len >= v->cap)
    {
        size_t new_cap = 2 * v->len;
        void* tmp = realloc(v->data, new_cap * sizeof(void*));
        if(!tmp)
            return;
        v->data = tmp;
        v->cap = new_cap;
    }
    v->data[v->len++] = p;
}

void* comp_vec_pop_back(comp_vec_t* v)
{
    return v->data[--v->len];
}

void* comp_vec_front(comp_vec_t* v)
{
    return v->data[0];
}

void* comp_vec_back(comp_vec_t* v)
{
    return v->data[v->len - 1];
}

int comp_vec_empty(comp_vec_t* v)
{
    return v->len == 0;
}

static inline void vec_swap(comp_vec_t* v, size_t i, size_t j)
{
    void* t = v->data[i];
    v->data[i] = v->data[j];
    v->data[j] = t;
}

static int vec_quicksort_part(comp_vec_t* v, int l, int r, comp_vec_cmp_f cmp)
{
    void* t = v->data[l];
    int i = l;
    int j = r + 1;
    while (1)
    {
        while(cmp(v->data[++i], t))
            if(i == r) break;
        while(cmp(t, v->data[--j]))
            if(j == l) break;
        if(i >= j) break;
        vec_swap(v, i, j);
    }
    vec_swap(v, l, j);
    return j;
}

void comp_vec_sort(comp_vec_t* v, int l, int r, comp_vec_cmp_f cmp)
{
    if(l >= r) return;
    int p = vec_quicksort_part(v, l, r, cmp);
    comp_vec_sort(v, l, p - 1, cmp);
    comp_vec_sort(v, p + 1, r, cmp);
}

void comp_vec_clear(comp_vec_t* v)
{
    v->len = 0;
}