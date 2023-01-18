//
// Created by zr on 23-1-18.
//

#ifndef COMPRESS_BAR_H
#define COMPRESS_BAR_H
#include <sys/types.h>
#include "internal/str.h"

#define MAX_BAR_WIDTH 50

struct comp_progress_bar
{
    size_t total;
    size_t complete;
    size_t progress;
    int first;
    comp_str_t title;
};

typedef struct comp_progress_bar comp_progress_bar;

comp_progress_bar* comp_bar_init(comp_str_t, size_t);
void comp_bar_free(comp_progress_bar*);
void comp_bar_set_title(comp_progress_bar*, const char*);
void comp_bar_set_total(comp_progress_bar*, size_t);
void comp_bar_update(comp_progress_bar*);
void comp_bar_add(comp_progress_bar*, size_t);

#endif //COMPRESS_BAR_H
