//
// Created by zr on 23-1-18.
//
#include "bar.h"
#include <stdlib.h>
#include <stdio.h>

static char r[] = {'-', '\\', '|', '/'};

comp_progress_bar* comp_bar_init(comp_str_t title, size_t total)
{
    comp_progress_bar* bar = (comp_progress_bar*) malloc(sizeof(comp_progress_bar));
    if(!bar) return NULL;
    bar->title = comp_str_new(title);
    bar->total = total;
    bar->complete = bar->progress = 0;
    bar->first = 1;
    return bar;
}

void comp_bar_free(comp_progress_bar* bar)
{
    if(!bar) return;
    comp_str_free(bar->title);
    free(bar);
}

void comp_bar_set_title(comp_progress_bar* bar, const char* title)
{
    bar->title = comp_str_assign(bar->title, title);
    comp_bar_update(bar);
}

void comp_bar_set_total(comp_progress_bar* bar, size_t total)
{
    bar->total = total;
}

void comp_bar_update(comp_progress_bar* bar)
{
    if(bar->first)
        bar->first = 0;
    else
    {
        printf("\033[1A");
        printf("\r");
        printf("\033[K");
    }
    printf("%s\n", bar->title);
    int cnt = MAX_BAR_WIDTH * bar->progress / 100;
    printf("\033[K");
    printf("[");
    for(int i = 0; i < cnt; i++)
        printf("%c", i == cnt - 1 ? '>' : '=');
    for (int i = 0; i < MAX_BAR_WIDTH - cnt; i++)
        printf(" ");
    printf("]");
    printf("[%zu%%] %c", bar->progress, r[bar->progress % 4]);
    fflush(stdout);
}

void comp_bar_add(comp_progress_bar* bar, size_t delta)
{
    bar->complete += delta;
    u_int32_t p = bar->complete * 100 / bar->total;
    if(p > bar->progress)
    {
        bar->progress = p;
        comp_bar_update(bar);
    }
}