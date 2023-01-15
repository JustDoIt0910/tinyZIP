//
// Created by zr on 23-1-14.
//

#ifndef COMPRESS_STR_H
#define COMPRESS_STR_H
#include <stddef.h>

#define COMP_DS_HDR(s) (comp_ds_t*)((char*)(s) - sizeof(comp_ds_t))

struct __attribute__((__packed__)) comp_ds_s
{
    size_t len;
    size_t alloc;
    char buf[];
};

typedef struct comp_ds_s comp_ds_t;
typedef char* comp_str_t;

comp_str_t comp_str_new_len(const char*, size_t);
comp_str_t comp_str_new(const char*);
comp_str_t comp_str_empty();
size_t comp_str_len(comp_str_t);
comp_str_t comp_str_append_char(comp_str_t, char);
comp_str_t comp_str_append_str(comp_str_t, const char*);
void comp_str_clear(comp_str_t);
comp_str_t comp_str_assign(comp_str_t, const char*);
void comp_str_free(comp_str_t);
void comp_str_debug(comp_str_t);
comp_str_t comp_str_parse_int(int, int);

#endif //COMPRESS_STR_H
