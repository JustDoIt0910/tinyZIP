//
// Created by zr on 23-1-22.
//
#include "../lzw.h"
#include <stdio.h>

int main()
{
    comp_bitstream_t* in = comp_bitstream_init(fopen("test", "rb"));
    comp_bitstream_t* out = comp_bitstream_init(fopen("test_out", "wb"));
    comp_lzw_ctx_t* lzw = comp_lzw_init(NULL);
    lzw->lzw_encode(lzw, in, out);
    comp_bitstream_destroy(in);
    comp_bitstream_destroy(out);
}