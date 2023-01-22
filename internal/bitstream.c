//
// Created by zr on 23-1-13.
//
#include "bitstream.h"
#include <stdlib.h>

comp_bitstream_t* comp_bitstream_init(FILE* fp)
{
    if(!fp)
        return NULL;
    comp_bitstream_t* s = (comp_bitstream_t*) malloc(sizeof(comp_bitstream_t));
    if(!s)
        return s;
    s->fp = fp;
    s->in_buf = s->out_buf = 0;
    s->in_buf_remain = s->out_buf_remain = 0;
    s->eof = s->closed = 0;
    return s;
}

static int clear_out_buf(comp_bitstream_t* s)
{
    if(s->out_buf_remain == 0)
        return 0;
    s->out_buf <<= (8 - s->out_buf_remain);
    if(fwrite(&s->out_buf, sizeof(char), 1, s->fp))
    {
        s->out_buf = 0;
        s->out_buf_remain = 0;
        return 0;
    }
    return -1;
}

void comp_bitstream_destroy(comp_bitstream_t* s)
{
    if(!s) return;
    if(!s->closed)
        comp_bitstream_close(s);
    free(s);
}

int comp_bitstream_write_bit(comp_bitstream_t* s, int bit)
{
    if(bit < 0 || bit > 1)
        return -1;
    s->out_buf = (s->out_buf << 1) | bit;
    if(++s->out_buf_remain == 8)
        return clear_out_buf(s);
    return 0;
}

int comp_bitstream_write_char(comp_bitstream_t* s, char ch)
{
    if(s->out_buf_remain == 0)
    {
        if(fwrite(&ch, sizeof(char), 1, s->fp))
            return 0;
    }
    else
    {
        for(int i = 1; i <= 8; i++)
        {
            int bit = (((u_char)ch) >> (8 - i)) & 1;
            if(comp_bitstream_write_bit(s, bit) < 0)
                return -1;
        }
        return 0;
    }
    return -1;
}

int comp_bitstream_write_short(comp_bitstream_t* s, short st)
{
    u_char c1 = (st >> 8) & 0xFF;
    u_char c2 = st & 0xFF;
    if(comp_bitstream_write_char(s, (char) c1) < 0) return -1;
    if(comp_bitstream_write_char(s, (char) c2) < 0) return -1;
    return 0;
}

int comp_bitstream_write_int(comp_bitstream_t* s, int i)
{
    u_char c1 = (i >> 24) & 0xFF;
    u_char c2 = (i >> 16) & 0xFF;
    u_char c3 = (i >> 8) & 0xFF;
    u_char c4 = i & 0xFF;
    if(comp_bitstream_write_char(s, (char) c1) < 0) return -1;
    if(comp_bitstream_write_char(s, (char) c2) < 0) return -1;
    if(comp_bitstream_write_char(s, (char) c3) < 0) return -1;
    if(comp_bitstream_write_char(s, (char) c4) < 0) return -1;
    return 0;
}

int comp_bitstream_write_nbit(comp_bitstream_t* s, int i, size_t len)
{
    int bit;
    for(int n = (int) len - 1; n >= 0; n--)
    {
        bit = (i >> n) & 1;
        if(comp_bitstream_write_bit(s, bit) < 0)
            return -1;
    }
    return 0;
}

int comp_bitstream_write(comp_bitstream_t* s, const char* data, size_t len)
{
    for(size_t i = 0; i < len; i++)
        if(comp_bitstream_write_char(s, *(data + i)) < 0)
            return -1;
    return 0;
}

int comp_bitstream_flush(comp_bitstream_t* s)
{
    if(clear_out_buf(s) < 0)
        return -1;
    fflush(s->fp);
    return 0;
}

static void fill_in_buf(comp_bitstream_t* s)
{
    if(fread(&s->in_buf, sizeof(char), 1, s->fp) == 1)
    {
        s->in_buf_remain = 8;
        return;
    }
    s->eof = 1;
    s->in_buf = s->in_buf_remain = 0;
}

int comp_bitstream_read_bit(comp_bitstream_t* s, int* bit)
{
    if(s->in_buf_remain == 0)
        fill_in_buf(s);
    if(s->eof)
        return -1;
    s->in_buf_remain--;
    if(bit)
        *bit = (s->in_buf >> s->in_buf_remain) & 1;
    return 0;
}

int comp_bitstream_read_char(comp_bitstream_t* s, char* ch)
{
    if(s->eof)
        return -1;
    if(s->in_buf_remain == 8)
    {
        *ch = (char) s->in_buf;
        fill_in_buf(s);
        return 0;
    }
    unsigned c = s->in_buf;
    c <<= (8 - s->in_buf_remain);
    int remain = s->in_buf_remain;
    fill_in_buf(s);
    if(s->eof)
    {
        *ch = (char) c;
        return -1;
    }
    s->in_buf_remain = remain;
    c |= (s->in_buf >> s->in_buf_remain);
    *ch = (char) c;
    return 0;
}

int comp_bitstream_read_short(comp_bitstream_t* s, short* st)
{
    char c1, c2; short x = 0;
    if(comp_bitstream_read_char(s, &c1) < 0) return -1;
    if(comp_bitstream_read_char(s, &c2) < 0) return -1;
    x = (short) (x | (c1 & 0xFF));
    x = (short) ((x << 8) | (c2 & 0xFF));
    *st = x;
    return 0;
}

int comp_bitstream_read_int(comp_bitstream_t* s, int* i)
{
    char c1, c2, c3, c4; int x = 0;
    if(comp_bitstream_read_char(s, &c1) < 0) return -1;
    if(comp_bitstream_read_char(s, &c2) < 0) return -1;
    if(comp_bitstream_read_char(s, &c3) < 0) return -1;
    if(comp_bitstream_read_char(s, &c4) < 0) return -1;
    x |= c1;
    x = (x << 8) | (c2 & 0xFF);
    x = (x << 8) | (c3 & 0xFF);
    x = (x << 8) | (c4 & 0xFF);
    *i = x;
    return 0;
}

void comp_bitstream_close(comp_bitstream_t* s)
{
    comp_bitstream_flush(s);
    fclose(s->fp);
    s->closed = 1;
}

int comp_bitstream_eof(comp_bitstream_t* s)
{
    return s->eof;
}

void comp_bitstream_reset(comp_bitstream_t* s)
{
    rewind(s->fp);
    s->in_buf = s->out_buf = 0;
    s->in_buf_remain = s->out_buf_remain = 0;
    s->eof = 0;
}