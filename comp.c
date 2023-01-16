//
// Created by zr on 23-1-13.
//
#include "comp.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

static int comp_codec_encode(comp_codec_t*, comp_bitstream_t*, comp_bitstream_t*);
static int comp_codec_decode(comp_codec_t*, comp_bitstream_t*, comp_bitstream_t*);
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

int comp_codec_encode(comp_codec_t* codec, comp_bitstream_t* in, comp_bitstream_t* out)
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

int comp_codec_decode(comp_codec_t* codec, comp_bitstream_t* in, comp_bitstream_t* out)
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

static comp_str_t filename(const char* filepath)
{
    const char* ptr = strrchr(filepath, '/');
    if(!ptr)
        return comp_str_new(filepath);
    return comp_str_new_len(ptr + 1, strlen(ptr + 1));
}

static int comp_compress_file(comp_compressor_t* c, comp_str_t filename,
                              comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    comp_bitstream_write_char(out_stream, COMP_FILE_MARKER);
    comp_bitstream_write_char(out_stream, (char) comp_str_len(filename));
    comp_bitstream_write(out_stream, filename, comp_str_len(filename));
    return c->codec->encode(c->codec, in_stream, out_stream);
}

static void comp_compress(comp_compressor_t* c, const char* in_path, const char* out_path)
{
    struct stat st;
    if(stat(in_path, &st) != 0)
    {
        printf("%s isn't a file or directory\n", in_path);
        return;
    }
    FILE* out = fopen(out_path, "wb");
    comp_bitstream_t* out_stream = comp_bitstream_init(out);
    if(!out_stream) return;
    comp_bitstream_write_short(out_stream, COMP_START_MARKER);
    if(!S_ISDIR(st.st_mode))
    {
        FILE* in = fopen(in_path, "rb");
        comp_bitstream_t* in_stream = comp_bitstream_init(in);
        if(!in_stream)
        {
            comp_bitstream_destroy(out_stream);
            return;
        }
        comp_str_t name = filename(in_path);
        comp_compress_file(c, name, in_stream, out_stream);
        comp_str_free(name);
    }
    else
    {

    }
}

static void comp_decompress(comp_compressor_t* c, const char* in_path)
{

}