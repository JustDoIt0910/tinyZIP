//
// Created by zr on 23-1-21.
//
#include "lzw.h"
#include "marker.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int encode(comp_lzw_ctx_t*, comp_bitstream_t*, comp_bitstream_t*);
static int decode(comp_lzw_ctx_t*, comp_bitstream_t*, comp_bitstream_t*);
static void lzw_ctx_cleanup(comp_lzw_ctx_t*);

comp_lzw_ctx_t* comp_lzw_init(comp_progress_bar* bar)
{
    comp_lzw_ctx_t* lzw = (comp_lzw_ctx_t*) malloc(sizeof(comp_lzw_ctx_t));
    if(!lzw) return NULL;
    lzw->tire = NULL;
    lzw->bar = bar;
    lzw_ctx_cleanup(lzw);
    lzw->lzw_encode = encode;
    lzw->lzw_decode = decode;
    return lzw;
}

void comp_lzw_free(comp_lzw_ctx_t* lzw)
{
    comp_tire_free(lzw->tire);
    free(lzw);
}

static void lzw_ctx_cleanup(comp_lzw_ctx_t* lzw)
{
    comp_tire_free(lzw->tire);
    lzw->tire = NULL;
    comp_str_t s = comp_str_empty();
    for (int i = 0; i < LZW_MAX_SYMBOL; i++)
    {
        s = comp_str_append_char(s, (char) i);
        lzw->tire = comp_tire_put(lzw->tire, s, i);
        comp_str_clear(s);
    }
    comp_str_free(s);
}

int encode(comp_lzw_ctx_t* lzw, comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    comp_bitstream_write_char(out_stream, LZW_HEADER_MARKER);
    char lookahead;
    comp_bitstream_read_char(in_stream, &lookahead);
    if(comp_bitstream_eof(in_stream))
        goto end;
    comp_bar_add(lzw->bar, 1);
    comp_str_t lst_prefix = comp_str_empty();
    lst_prefix = comp_str_append_char(lst_prefix, lookahead);
    comp_tire_t* node, *root;
    int flag = 0; int i = LZW_MAX_SYMBOL + 1;
    while(1)
    {
        u_int16_t code;
        root = lzw->tire;
        while((node = comp_tire_get(root, lst_prefix)) != NULL)
        {
            comp_bitstream_read_char(in_stream, &lookahead);
            comp_bar_add(lzw->bar, 1);
            if(comp_bitstream_eof(in_stream))
            {
                flag = 1;
                code = node->value;
                break;
            }
            lst_prefix = comp_str_append_char(lst_prefix, lookahead);
            code = node->value;
            root = node;
        }
        comp_bitstream_write_nbit(out_stream, code, LZW_CODE_WIDTH);
        if(flag) break;
        if(i < LZW_CODE_NUM)
            comp_tire_put(lzw->tire, lst_prefix, i++);
        comp_str_clear(lst_prefix);
        comp_str_append_char(lst_prefix, lookahead);
    }
    comp_str_free(lst_prefix);
end:
    comp_bitstream_write_nbit(out_stream, LZW_TERMINATE_CODE, LZW_CODE_WIDTH);
    comp_bitstream_flush(out_stream);
    lzw_ctx_cleanup(lzw);
    return 0;
}

int decode(comp_lzw_ctx_t* lzw, comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    char h; int i;
    size_t remain = 0;
    comp_bitstream_read_char(in_stream, &h);
    if((u_char) h != LZW_HEADER_MARKER)
        return -1;
    comp_bar_add(lzw->bar, 1);
    int code;
    comp_bitstream_read_nbit(in_stream, &code, LZW_CODE_WIDTH);
    remain += LZW_CODE_WIDTH % 8;
    comp_bar_add(lzw->bar, 1);
    if((u_int32_t) code == LZW_TERMINATE_CODE)
        goto end;
    comp_str_t code_tbl[LZW_CODE_NUM];
    memset(code_tbl, 0, sizeof(comp_str_t) * LZW_CODE_NUM);
    for (i = 0; i < LZW_MAX_SYMBOL; i++)
    {
        code_tbl[i] = comp_str_empty();
        comp_str_append_char(code_tbl[i], (char) i);
    }
    i++;
    comp_str_t val = code_tbl[code];
    while(1)
    {
        comp_bitstream_write(out_stream, val, comp_str_len(val));
        comp_bitstream_read_nbit(in_stream, &code, LZW_CODE_WIDTH);
        comp_bar_add(lzw->bar, 1);
        remain += LZW_CODE_WIDTH % 8;
        if(remain % 8 == 0)
            comp_bar_add(lzw->bar, 1);
        if((u_int32_t) code == LZW_TERMINATE_CODE)
            break;
        comp_str_t s = code_tbl[code];
        if(i == code)
        {
            assert(!s);
            s = comp_str_new_len(val, comp_str_len(val));
            s = comp_str_append_char(s, comp_str_at(val,  0));
            code_tbl[code] = s;
            val = s;
            i++;
            continue;
        }
        if(i < LZW_CODE_NUM)
        {
            code_tbl[i] = comp_str_new_len(val, comp_str_len(val));
            comp_str_append_char(code_tbl[i++], comp_str_at(s, 0));
        }
        val = s;
    }
end:
    remain = remain % 8;
    assert(remain == 0 || remain == 4);
    comp_bitstream_read_nbit(in_stream, NULL, remain);
    if(remain == 4)
        comp_bar_add(lzw->bar, 1);
    return 0;
}