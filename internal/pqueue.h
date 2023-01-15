//
// Created by zr on 23-1-13.
//

#ifndef COMPRESS_PQUEUE_H
#define COMPRESS_PQUEUE_H
#include <stddef.h>

typedef int (*comp_pqueue_pri_cmp)(const void*, const void*);

struct comp_pqueue_s
{
    size_t size;
    size_t cap;
    size_t step;
    void** pq;
    comp_pqueue_pri_cmp pri_cmp;
};

typedef struct comp_pqueue_s comp_pqueue_t;

comp_pqueue_t* comp_pqueue_init(size_t, comp_pqueue_pri_cmp);
int comp_pqueue_insert(comp_pqueue_t*, void*);
void* comp_pqueue_top(comp_pqueue_t*);
void* comp_pqueue_pop(comp_pqueue_t*);
void comp_pqueue_destroy(comp_pqueue_t*);
size_t comp_pqueue_size(comp_pqueue_t*);

#endif //COMPRESS_PQUEUE_H
