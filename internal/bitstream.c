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
            int bit = (((unsigned char)ch) >> (8 - i)) & 1;
            if(comp_bitstream_write_bit(s, bit) < 0)
                return -1;
        }
        return 0;
    }
    return -1;
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