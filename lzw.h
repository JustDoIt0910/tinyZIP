//
// Created by zr on 23-1-21.
//
#ifndef COMPRESS_LZW_H
#define COMPRESS_LZW_H
#include "internal/bitstream.h"
#include "internal/3w_tire.h"
#include "bar.h"

#define LZW_MAX_SYMBOL 256
#define LZW_CODE_WIDTH 12
#define LZW_CODE_NUM (1 << LZW_CODE_WIDTH)
#define LZW_TERMINATE_CODE 256

struct comp_lzw_ctx_s;
typedef int (*comp_lzw_encode_f) (struct comp_lzw_ctx_s*, comp_bitstream_t*, comp_bitstream_t*);
typedef int (*comp_lzw_decode_f) (struct comp_lzw_ctx_s*, comp_bitstream_t*, comp_bitstream_t*);

struct comp_lzw_ctx_s
{
    comp_tire_t* tire; // for encoding
    comp_progress_bar* bar;
    comp_lzw_encode_f lzw_encode;
    comp_lzw_decode_f lzw_decode;
};

typedef struct comp_lzw_ctx_s comp_lzw_ctx_t;

comp_lzw_ctx_t* comp_lzw_init(comp_progress_bar*);
void comp_lzw_free(comp_lzw_ctx_t*);

#endif //COMPRESS_LZW_H
