//
// Created by zr on 23-1-14.
//
#ifndef COMPRESS_VECTOR_H
#define COMPRESS_VECTOR_H
#include <stddef.h>

struct comp_vec_s
{
    size_t len;
    size_t cap;
    void** data;
};

typedef struct comp_vec_s comp_vec_t;
typedef int (*comp_vec_cmp_f) (const void*, const void*);

comp_vec_t* comp_vec_init(size_t);
void comp_vec_free(comp_vec_t*);
void* comp_vec_get(comp_vec_t*, size_t);
size_t comp_vec_len(comp_vec_t*);
void comp_vec_push_back(comp_vec_t*, void*);
void* comp_vec_pop_back(comp_vec_t*);
void* comp_vec_front(comp_vec_t*);
void* comp_vec_back(comp_vec_t*);
int comp_vec_empty(comp_vec_t*);
void comp_vec_sort(comp_vec_t*, int, int, comp_vec_cmp_f);
void comp_vec_clear(comp_vec_t*);

#endif //COMPRESS_VECTOR_H
