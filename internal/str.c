//
// Created by zr on 23-1-14.
//
#include "str.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

comp_str_t comp_str_new_len(const char* data, size_t len)
{
    size_t hdr_len = sizeof(comp_ds_t);
    comp_ds_t* hdr = (comp_ds_t*) malloc(hdr_len + len + 1);
    if(!hdr)
        return NULL;
    char* s = (char*)hdr + hdr_len;
    memset(s, 0, len + 1);
    if(data && len)
        memcpy(s, data, len);
    hdr->len = len;
    hdr->alloc = hdr_len + len + 1;
    return s;
}

comp_str_t comp_str_new(const char* data)
{
    size_t len = strlen(data);
    return comp_str_new_len(data, len);
}

comp_str_t comp_str_empty()
{
    return comp_str_new_len("", 0);
}

size_t comp_str_len(comp_str_t s)
{
    comp_ds_t* hdr = COMP_DS_HDR(s);
    return hdr->len;
}

static comp_str_t comp_str_grow(comp_str_t s, size_t len)
{
    size_t hdr_len = sizeof(comp_ds_t);
    size_t new_alloc = 2 * len + hdr_len + 1;
    comp_ds_t* hdr = COMP_DS_HDR(s);
    comp_ds_t* tmp = (comp_ds_t*) realloc(hdr, new_alloc);
    if(!tmp)
        return NULL;
    hdr = tmp;
    hdr->alloc = new_alloc;
    s = (char*)hdr + hdr_len;
    return s;
}

comp_str_t comp_str_append_char(comp_str_t s, char c)
{
    if(!s)
        return NULL;
    comp_ds_t* hdr = COMP_DS_HDR(s);
    hdr->buf[hdr->len] = c;
    size_t hdr_len = sizeof(comp_ds_t);
    size_t new_len = hdr->len + 1;
    if(hdr_len + new_len + 1 > hdr->alloc)
    {
        comp_str_t tmp = comp_str_grow(s, new_len);
        if(!tmp)
            return s;
        s = tmp;
    }
    hdr = COMP_DS_HDR(s);
    hdr->len += 1;
    hdr->buf[hdr->len] = 0;
    return s;
}

comp_str_t comp_str_append_str(comp_str_t s, const char* str)
{
    if(!s)
        return NULL;
    comp_ds_t* hdr = COMP_DS_HDR(s);
    size_t hdr_len = sizeof(comp_ds_t);
    size_t new_len = hdr->len + strlen(str);
    if(hdr_len + new_len + 1 > hdr->alloc)
    {
        comp_str_t tmp = comp_str_grow(s, new_len);
        if(!tmp)
            return s;
        s = tmp;
    }
    hdr = COMP_DS_HDR(s);
    strcpy(hdr->buf + hdr->len, str);
    hdr->len = new_len;
    hdr->buf[hdr->len] = 0;
    return s;
}

void comp_str_free(comp_str_t s)
{
    if(!s)
        return;
    comp_ds_t* hdr = COMP_DS_HDR(s);
    free(hdr);
}

void comp_str_debug(comp_str_t s)
{
    comp_ds_t* hdr = COMP_DS_HDR(s);
    printf("addr = %p len = %zu alloc = %zu\n", s, hdr->len, hdr->alloc);
    printf("content = %s\n", s);
}

void comp_str_clear(comp_str_t s)
{
    comp_ds_t* hdr = COMP_DS_HDR(s);
    memset(s, 0, hdr->len);
    hdr->len = 0;
}

comp_str_t comp_str_assign(comp_str_t s, const char* str)
{
    comp_str_clear(s);
    return comp_str_append_str(s, str);
}

comp_str_t comp_str_parse_int(int v, int base)
{
    if(base < 2 || base > 16)
        return NULL;
    comp_str_t str = comp_str_empty();
    if(!str) return NULL;
    static char tbl[] = "0123456789ABCDEF";
    char tmp[50] = {0};
    int s = v; int i = 0;
    if(s < 0)
    {
        s = -s;
        str = comp_str_append_char(str, '-');
    }
    if(s == 0)
    {
        str = comp_str_append_char(str, '0');
        return str;
    }
    while(s > 0)
    {
        tmp[i++] = tbl[s % base];
        s /= base;
    }
    for(int j = i - 1; j >= 0; j--)
        str = comp_str_append_char(str, tmp[j]);
    return str;
}

char comp_str_at(comp_str_t s, size_t index)
{
    comp_ds_t* hdr = COMP_DS_HDR(s);
    if(index >= hdr->len)
        return -1;
    return s[index];
}

comp_str_t comp_str_substr(comp_str_t s, size_t start, size_t len)
{
    if(start >= comp_str_len(s) || start + len > comp_str_len(s))
        return NULL;
    comp_str_t sub = comp_str_new_len(s, len);
    return sub;
}