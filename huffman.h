//
// Created by zr on 23-1-13.
//

#ifndef COMPRESS_HUFFMAN_H
#define COMPRESS_HUFFMAN_H
#include "internal/str.h"
#include "internal/vector.h"
#include "internal/bitstream.h"
#include <stdio.h>
#include <sys/types.h>

#define HUFFMAN_MAX_SYMBOL 256
#define HUFFMAN_DEBUG(fmt, ...)             \
    printf("%s:%d ", __FILE__, __LINE__),   \
    printf(fmt, __VA_ARGS__), printf("\n")

struct comp_huffman_node_s
{
    u_char c;
    u_int32_t freq;
    int is_leaf;
    struct comp_huffman_node_s* left;
    struct comp_huffman_node_s* right;
};

struct comp_huffman_symbol_s
{
    u_char symbol;
    size_t symbol_code_len;
};

typedef struct comp_huffman_node_s comp_huffman_node_t;
typedef struct comp_huffman_symbol_s comp_huffman_symbol_t;

struct comp_huffman_ctx_s;
typedef int (*comp_huffman_encode_f)(struct comp_huffman_ctx_s*, comp_bitstream_t*, comp_bitstream_t*);
typedef int (*comp_huffman_decode_f)(struct comp_huffman_ctx_s*, comp_bitstream_t*, comp_bitstream_t*);

struct comp_huffman_ctx_s
{
    u_int32_t freq[256];
    comp_str_t symbol_code_table[256];
    comp_vec_t* symbols;
    comp_huffman_node_t* root;
    u_char padding;
    u_int32_t content_len;
    int disable;
    comp_huffman_encode_f huffman_encode;
    comp_huffman_decode_f huffman_decode;
};

#define HUFFMAN_GET_SYMBOL(index) \
(((comp_huffman_symbol_t*)(comp_vec_get(huff->symbols, index)))->symbol)
#define HUFFMAN_GET_SYMBOL_LEN(index) \
(((comp_huffman_symbol_t*)(comp_vec_get(huff->symbols, index)))->symbol_code_len)

typedef struct comp_huffman_ctx_s comp_huffman_ctx_t;

comp_huffman_ctx_t* comp_huffman_init();
void comp_huffman_free(comp_huffman_ctx_t*);

#endif //COMPRESS_HUFFMAN_H
