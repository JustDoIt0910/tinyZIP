//
// Created by zr on 23-1-13.
//

#ifndef COMPRESS_COMP_H
#define COMPRESS_COMP_H
#include <stdio.h>
#include "huffman.h"


struct comp_codec_s;
typedef int (*comp_encode_f) (struct comp_codec_s*, comp_bitstream_t*, comp_bitstream_t*);
typedef int (*comp_decode_f) (struct comp_codec_s*, comp_bitstream_t*, comp_bitstream_t*);

typedef enum comp_codec_type
{ COMP_CODEC_HUFFMAN, COMP_CODEC_LZW } comp_codec_type;

struct comp_codec_s
{
    comp_codec_type type;
    comp_encode_f encode;
    comp_decode_f decode;
};

struct comp_huffman_codec_s
{
    struct comp_codec_s p;
    comp_huffman_ctx_t* huffman_ctx;
};

struct comp_lzw_codec_s
{
    struct comp_codec_s p;
    // TODO implement lzw codec
};

typedef struct comp_codec_s comp_codec_t;
typedef struct comp_huffman_codec_s comp_huffman_codec_t;
typedef struct comp_lzw_codec_s comp_lzw_codec_t;

#define CODEC_PARENT_INIT(codec, _type, encode_f, decode_f) \
        (codec)->p.type = (_type);                          \
        (codec)->p.encode = (encode_f);                     \
        (codec)->p.decode = (decode_f)                      \

struct comp_compressor_s;
typedef void (*comp_compress_f) (struct comp_compressor_s*, const char*, const char*);
typedef void (*comp_decompress_f) (struct comp_compressor_s*, const char*);

typedef enum comp_parse_state
{
    COMP_PARSE_START,
    COMP_PARSE_FILE,
    COMP_PARSE_DIR,
    COMP_PARSE_STOP,
    COMP_PARSE_FAIL
} comp_parse_state;

struct comp_compressor_s
{
    comp_codec_t* codec;
    comp_parse_state state;             // for decompression
    comp_str_t cur_decompress_dir;      // for decompression
    comp_vec_t* decompress_dir_stack;   // for decompression
    comp_compress_f compress;
    comp_decompress_f decompress;
};

typedef struct comp_compressor_s comp_compressor_t;

comp_codec_t* comp_codec_init(comp_codec_type);
void comp_codec_free(comp_codec_t*);
comp_compressor_t* comp_compressor_init(comp_codec_type);
void comp_compressor_free(comp_compressor_t*);

#endif //COMPRESS_COMP_H
