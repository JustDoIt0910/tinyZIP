//
// Created by zr on 23-1-13.
//
#include "pqueue.h"
#include <stdlib.h>

static size_t left_child_index(size_t i) { return i << 1; }

static size_t right_child_index(size_t i) { return (i << 1) + 1; }

static size_t parent_index(size_t i) { return i >> 1; }

comp_pqueue_t* comp_pqueue_init(size_t cap, comp_pqueue_pri_cmp cmp)
{
    comp_pqueue_t* q = (comp_pqueue_t*)malloc(sizeof(comp_pqueue_t));
    if(!q)
        return NULL;
    q->size = 0;
    q->cap = q->step = cap;
    q->pri_cmp = cmp;
    q->pq = (void**) malloc((cap + 1) * sizeof(void*));
    if(!q->pq)
    {
        free(q);
        return NULL;
    }
    return q;
}

static size_t max_pri_child(comp_pqueue_t* q, size_t i)
{
    size_t left = left_child_index(i);
    if(left > q->size)
        return 0;
    size_t right = right_child_index(i);
    if(right <= q->size && q->pri_cmp(q->pq[left], q->pq[right]))
        return right;
    return left;
}

static void pqueue_item_swim(comp_pqueue_t* q, size_t i)
{
    size_t cur = i;
    size_t parent = parent_index(i);
    void* moving_node = q->pq[i];
    for(; cur > 1 && q->pri_cmp(q->pq[parent], moving_node);
        cur = parent, parent = parent_index(cur))
        q->pq[cur] = q->pq[parent];
    q->pq[cur] = moving_node;
}

static void pqueue_item_sink(comp_pqueue_t* q, size_t i)
{
    size_t cur = i;
    size_t child;
    void* moving_node = q->pq[i];
    for(; (child = max_pri_child(q, cur)) && q->pri_cmp(moving_node, q->pq[child]); cur = child)
        q->pq[cur] = q->pq[child];
    q->pq[cur] = moving_node;
}

int comp_pqueue_insert(comp_pqueue_t* q, void* v)
{
    if(!q)
        return -1;
    if(q->size == q->cap)
    {
        size_t new_cap = q->cap + q->step;
        void** tmp = (void**) realloc(q->pq, (new_cap + 1) * sizeof(void*));
        if(!tmp)
            return -1;
        q->pq = tmp;
        q->cap = new_cap;
    }
    q->pq[++q->size] = v;
    pqueue_item_swim(q, q->size);
    return 0;
}

void* comp_pqueue_top(comp_pqueue_t* q)
{
    if(!q)
        return NULL;
    return q->pq[1];
}

void* comp_pqueue_pop(comp_pqueue_t* q)
{
    if(!q)
        return NULL;
    void* top = q->pq[1];
    q->pq[1] = q->pq[q->size--];
    pqueue_item_sink(q, 1);
    return top;
}

void comp_pqueue_destroy(comp_pqueue_t* q)
{
    if(!q) return;
    if(q->pq)
        free(q->pq);
    free(q);
}

size_t comp_pqueue_size(comp_pqueue_t* q)
{
    return q->size;
}
