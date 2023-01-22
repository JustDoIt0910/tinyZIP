//
// Created by zr on 23-1-13.
//

#ifndef COMPRESS_BITSTREAM_H
#define COMPRESS_BITSTREAM_H
#include <stdio.h>
#include <sys/types.h>

struct comp_bitstream_s
{
    FILE* fp;
    u_char in_buf;
    u_char out_buf;
    int in_buf_remain;
    int out_buf_remain;
    int eof;
    int closed;
};

typedef struct comp_bitstream_s comp_bitstream_t;

comp_bitstream_t* comp_bitstream_init(FILE*);
void comp_bitstream_destroy(comp_bitstream_t*);
int comp_bitstream_write_bit(comp_bitstream_t*, int);
int comp_bitstream_write_char(comp_bitstream_t*, char);
int comp_bitstream_write_short(comp_bitstream_t*, short);
int comp_bitstream_write_int(comp_bitstream_t*, int);
int comp_bitstream_write(comp_bitstream_t*, const char*, size_t);
int comp_bitstream_write_nbit(comp_bitstream_t*, int, size_t);
int comp_bitstream_write_str(comp_bitstream_t*, const char*);
int comp_bitstream_flush(comp_bitstream_t*);
int comp_bitstream_read_bit(comp_bitstream_t*, int*);
int comp_bitstream_read_char(comp_bitstream_t*, char*);
int comp_bitstream_read_short(comp_bitstream_t*, short*);
int comp_bitstream_read_int(comp_bitstream_t*, int*);
int comp_bitstream_read_nbit(comp_bitstream_t*, int*, size_t);
void comp_bitstream_close(comp_bitstream_t*);
int comp_bitstream_eof(comp_bitstream_t*);
void comp_bitstream_reset(comp_bitstream_t*);

#endif //COMPRESS_BITSTREAM_H
