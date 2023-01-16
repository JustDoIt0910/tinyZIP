//
// Created by zr on 23-1-13.
//
#include "comp.h"
#include <stdlib.h>

static int comp_codec_encode(comp_codec_t*, FILE*, FILE*);
static int comp_codec_decode(comp_codec_t*, FILE*, FILE*);
static void comp_compress(comp_compressor_t*, const char*, const char*);
static void comp_decompress(comp_compressor_t*, const char*);

static comp_huffman_codec_t* huffman_codec_new()
{
    comp_huffman_codec_t* codec = (comp_huffman_codec_t*) malloc(sizeof(comp_huffman_codec_t));
    if(!codec) return NULL;
    CODEC_PARENT_INIT(codec, COMP_CODEC_HUFFMAN, comp_codec_encode, comp_codec_decode);
    codec->huffman_ctx = comp_huffman_init();
    if(!codec->huffman_ctx)
    {
        free(codec);
        return NULL;
    }
    return codec;
}

static comp_lzw_codec_t* lzw_codec_new()
{
    comp_lzw_codec_t* codec = (comp_lzw_codec_t*) malloc(sizeof(comp_lzw_codec_t));
    if(!codec) return NULL;
    CODEC_PARENT_INIT(codec, COMP_CODEC_LZW, comp_codec_encode, comp_codec_decode);
    return codec;
}

comp_codec_t* comp_codec_init(comp_codec_type type)
{
    comp_codec_t* codec = NULL;
    switch (type)
    {
        case COMP_CODEC_HUFFMAN:
            codec = (comp_codec_t*) huffman_codec_new();
            break;
        case COMP_CODEC_LZW:
            codec = (comp_codec_t*) lzw_codec_new();
            break;
        default:
            break;
    }
    return codec;
}

void comp_codec_free(comp_codec_t* codec)
{
    switch (codec->type)
    {
        case COMP_CODEC_HUFFMAN:
            comp_huffman_free(((comp_huffman_codec_t*) codec)->huffman_ctx);
            break;
        default:
            break;
    }
    free(codec);
}

comp_compressor_t* comp_compressor_init(comp_codec_type type)
{
    comp_compressor_t* c = (comp_compressor_t*) malloc(sizeof(comp_compressor_t));
    if(!c) return NULL;
    c->codec = comp_codec_init(type);
    if(!c->codec) return NULL;
    c->compress = comp_compress;
    c->decompress = comp_decompress;
    return c;
}

void comp_compressor_free(comp_compressor_t* c)
{
    if(!c) return;
    comp_codec_free(c->codec);
    free(c);
}

int comp_codec_encode(comp_codec_t* codec, FILE* in, FILE* out)
{
    if(codec->type == COMP_CODEC_HUFFMAN)
    {
        comp_huffman_codec_t* huffman_codec = (comp_huffman_codec_t*) codec;
        comp_huffman_ctx_t* ctx = huffman_codec->huffman_ctx;
        return ctx->huffman_encode(ctx, in, out);
    }
    else if(codec->type == COMP_CODEC_LZW)
    {
        comp_lzw_codec_t* lzw_codec = (comp_lzw_codec_t*) codec;
        // TODO lzw encode
        return -1;
    }
    return -1;
}

int comp_codec_decode(comp_codec_t* codec, FILE* in, FILE* out)
{
    if(codec->type == COMP_CODEC_HUFFMAN)
    {
        comp_huffman_codec_t* huffman_codec = (comp_huffman_codec_t*) codec;
        comp_huffman_ctx_t* ctx = huffman_codec->huffman_ctx;
        return ctx->huffman_decode(ctx, in, out);
    }
    else if(codec->type == COMP_CODEC_LZW)
    {
        comp_lzw_codec_t* lzw_codec = (comp_lzw_codec_t*) codec;
        // TODO lzw decode
        return -1;
    }
    return -1;
}

static void comp_compress(comp_compressor_t* c, const char* in_path, const char* out_path)
{

}

static void comp_decompress(comp_compressor_t* c, const char* in_path)
{

}