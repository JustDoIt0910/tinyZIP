//
// Created by zr on 23-1-18.
//
#include "../bar.h"
#include <unistd.h>

int main()
{
    comp_progress_bar* bar = comp_bar_init(comp_str_new("test"), 100);
    int cnt = 100;
    while (cnt-- > 0)
    {
        comp_bar_add(bar, 1);
        usleep(1000 * 500);
    }
    comp_bar_free(bar);
    return 0;
}