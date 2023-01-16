//
// Created by zr on 23-1-13.
//
#include "comp.h"
#include <stdlib.h>

static void comp_codec_encode(struct comp_codec_s, FILE*, FILE*);
static void comp_codec_decode(struct comp_codec_s, FILE*, FILE*);
static void comp_compress(struct comp_compressor_s*, const char*, const char*);
static void comp_decompress(struct comp_compressor_s*, const char*);

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

void comp_codec_encode(struct comp_codec_s codec, FILE* in, FILE* out)
{

}

void comp_codec_decode(struct comp_codec_s codec, FILE* in, FILE* out)
{

}

static void comp_compress(struct comp_compressor_s* c, const char* in_path, const char* out_path)
{

}

static void comp_decompress(struct comp_compressor_s* c, const char* in_path)
{

}