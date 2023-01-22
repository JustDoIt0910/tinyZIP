//
// Created by zr on 23-1-21.
//
#include "lzw.h"
#include "marker.h"
#include <stdlib.h>

static void encode(comp_lzw_ctx_t*, comp_bitstream_t*, comp_bitstream_t*);
static int decode(comp_lzw_ctx_t*, comp_bitstream_t*, comp_bitstream_t*);

comp_lzw_ctx_t* comp_lzw_init(comp_progress_bar* bar)
{
    comp_lzw_ctx_t* lzw = (comp_lzw_ctx_t*) malloc(sizeof(comp_lzw_ctx_t));
    if(!lzw) return NULL;
    lzw->tire = NULL;
    lzw->bar = bar;
    comp_str_t s = comp_str_empty();
    for (int i = 0; i < LZW_MAX_SYMBOL; ++i)
    {
        s = comp_str_append_char(s, (char) i);
        lzw->tire = comp_tire_put(lzw->tire, s, i);
        comp_str_clear(s);
    }
    comp_str_free(s);
    lzw->lzw_encode = encode;
    lzw->lzw_decode = decode;
    return lzw;
}

void comp_lzw_free(comp_lzw_ctx_t* lzw)
{
    comp_tire_free(lzw->tire);
    free(lzw);
}

void encode(comp_lzw_ctx_t* lzw, comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    comp_bitstream_write_char(out_stream, LZW_HEADER_MARKER);
    char lookahead;
    comp_bitstream_read_char(in_stream, &lookahead);
    if(comp_bitstream_eof(in_stream))
        return;
    comp_str_t lst_prefix = comp_str_empty();
    lst_prefix = comp_str_append_char(lst_prefix, lookahead);
    comp_tire_t* node;
    int flag = 0; int i = LZW_MAX_SYMBOL + 1;
    while(1)
    {
        u_int16_t code;
        while((node = comp_tire_get(lzw->tire, lst_prefix)) != NULL)
        {
            comp_bitstream_read_char(in_stream, &lookahead);
            if(comp_bitstream_eof(in_stream))
            {
                flag = 1;
                code = node->value;
                break;
            }
            lst_prefix = comp_str_append_char(lst_prefix, lookahead);
            code = node->value;
        }
        comp_bitstream_write_nbit(out_stream, code, LZW_CODE_WIDTH);
        if(flag) break;
        if(i < LZW_CODE_NUM)
            comp_tire_put(lzw->tire, lst_prefix, i++);
        comp_str_clear(lst_prefix);
        comp_str_append_char(lst_prefix, lookahead);
    }
    comp_bitstream_write_nbit(out_stream, LZW_MAX_SYMBOL, LZW_CODE_WIDTH);
    comp_bitstream_flush(out_stream);
}

int decode(comp_lzw_ctx_t* lzw, comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    return 0;
}