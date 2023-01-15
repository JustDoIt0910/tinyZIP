//
// Created by zr on 23-1-14.
//
#include "../str.h"
#include <stdio.h>

int main()
{
//    comp_str_t str = comp_str_new("test");
    comp_str_t str = comp_str_empty();
    comp_str_debug(str);
    str = comp_str_append_char(str, 'a');
    str = comp_str_append_char(str, 'b');
    str = comp_str_append_char(str, 'c');
    comp_str_debug(str);
    str = comp_str_append_str(str, "this is s simple test of dynamic string");
    comp_str_debug(str);
    str = comp_str_assign(str, "assign");
    comp_str_debug(str);
    printf("%zu\n", comp_str_len(str));
    comp_str_free(str);

    comp_str_t s = comp_str_parse_int(100, 16);
    comp_str_debug(s);
    comp_str_free(s);
}