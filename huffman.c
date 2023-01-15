//
// Created by zr on 23-1-13.
//
#include "huffman.h"
#include "internal/pqueue.h"
#include "internal/bitstream.h"
#include <stdlib.h>
#include <string.h>

static int encode(comp_huffman_t* huff, FILE* in, FILE* out);
static int decode(comp_huffman_t* huff, FILE* in, FILE* out);

static inline int huffman_node_pri_cmp(const void* a, const void* b)
{
    return ((const comp_huffman_node_t*) a)->freq > ((const comp_huffman_node_t*) b)->freq;
}

static inline int canonical_symbol_cmp(const void* a, const void* b)
{
    const comp_huffman_symbol_t* s1 = a;
    const comp_huffman_symbol_t* s2 = b;
    if(s1->symbol_code_len < s2->symbol_code_len)
        return 1;
    if(s1->symbol_code_len == s2->symbol_code_len)
        return s1->symbol < s2->symbol;
    return 0;
}

static comp_huffman_node_t* huffman_node_new(unsigned char c, int freq,
                                             comp_huffman_node_t* lchild, comp_huffman_node_t* rchild)
{
    comp_huffman_node_t* node = (comp_huffman_node_t*) malloc(sizeof(comp_huffman_node_t));
    if(!node) return NULL;
    node->c = c;
    node->freq = freq;
    node->left = lchild;
    node->right = rchild;
    node->is_leaf = (lchild == NULL && rchild == NULL);
    return node;
}

static comp_huffman_symbol_t* huffman_symbol_new(unsigned char sym, size_t code_len)
{
    comp_huffman_symbol_t* huff_symbol = (comp_huffman_symbol_t*) malloc(sizeof(comp_huffman_symbol_t));
    if(!huff_symbol) return NULL;
    huff_symbol->symbol_code_len = code_len;
    huff_symbol->symbol = sym;
    return huff_symbol;
}

comp_huffman_t* comp_huffman_init()
{
    comp_huffman_t* huff = (comp_huffman_t*) malloc(sizeof(comp_huffman_t));
    if(!huff) return NULL;
    memset(huff->freq, 0, HUFFMAN_MAX_SYMBOL);
    for(int i = 0; i < HUFFMAN_MAX_SYMBOL; i++)
        huff->symbol_code_table[i] = comp_str_empty();
    huff->symbols = comp_vec_init(64);
    huff->root = NULL;
    huff->huffman_encode = encode;
    huff->huffman_decode = decode;
    return huff;
}

static int huffman_build_tree(comp_huffman_t* huff)
{
    comp_pqueue_t* pq = comp_pqueue_init(64, huffman_node_pri_cmp);
    if(!pq) return -1;
    for(int c = 0; c < HUFFMAN_MAX_SYMBOL; c++)
        if(huff->freq[c] > 0)
        {
            comp_huffman_node_t* node = huffman_node_new((unsigned char)c, huff->freq[c],
                                                         NULL, NULL);
            if(!node)
            {
                comp_pqueue_destroy(pq);
                return -1;
            }
            comp_pqueue_insert(pq, node);
        }
    while(comp_pqueue_size(pq) > 1)
    {
        comp_huffman_node_t* x = comp_pqueue_pop(pq);
        comp_huffman_node_t* y = comp_pqueue_pop(pq);
        comp_huffman_node_t* parent = huffman_node_new(0, x->freq + y->freq, x, y);
        comp_pqueue_insert(pq, parent);
    }
    huff->root = comp_pqueue_pop(pq);
    comp_pqueue_destroy(pq);
    return 0;
}

static void get_code_len(comp_huffman_t* huff, comp_huffman_node_t* root, size_t code_len)
{
    if(root->is_leaf)
    {
        comp_huffman_symbol_t* huff_symbol = huffman_symbol_new(root->c, code_len);
        comp_vec_push_back(huff->symbols, huff_symbol);
        return;
    }
    get_code_len(huff, root->left, code_len + 1);
    get_code_len(huff, root->right, code_len + 1);
}

static void huffman_assign_code(comp_huffman_t* huff, int code, comp_huffman_symbol_t* sym)
{
    comp_str_t code_str = comp_str_parse_int(code, 2);
    comp_str_t symbol_code = huff->symbol_code_table[sym->symbol];
    size_t code_str_len = comp_str_len(code_str);
    if(sym->symbol_code_len == code_str_len)
        symbol_code = comp_str_assign(symbol_code, code_str);
    else
    {
        for(int i = 0; i < sym->symbol_code_len - code_str_len; i++)
            symbol_code = comp_str_append_char(symbol_code, '0');
        symbol_code = comp_str_append_str(symbol_code, code_str);
    }
    huff->symbol_code_table[sym->symbol] = symbol_code;
    comp_str_free(code_str);
}

static void huffman_build_code(comp_huffman_t* huff)
{
    get_code_len(huff, huff->root, 0);
    comp_vec_sort(huff->symbols, 0, comp_vec_len(huff->symbols) - 1, canonical_symbol_cmp);
    int cnt = 0;
    int code = 0; int pre_min_code = 0;
    size_t pre_code_len = 1;
    for(int i = 0; i < comp_vec_len(huff->symbols); i++)
    {
        comp_huffman_symbol_t* sym = comp_vec_get(huff->symbols, i);
        if(sym->symbol_code_len == pre_code_len)
        {
            huffman_assign_code(huff, code, sym);
            code++;
            cnt++;
        }
        else
        {
            size_t gap = sym->symbol_code_len - pre_code_len;
            code = pre_min_code = (pre_min_code + cnt) << gap;
            huffman_assign_code(huff, code, sym);
            code++;
            pre_code_len = sym->symbol_code_len;
            cnt = 1;
        }
    }
}

void huffman_write_header(comp_huffman_t* huff, comp_bitstream_t* in_stream)
{
    comp_bitstream_write_char(in_stream, HUFFMAN_HEADER_MARKER);
    
}

int encode(comp_huffman_t* huff, FILE* in, FILE* out)
{
    comp_bitstream_t* in_stream = comp_bitstream_init(in);
    char c;
    while(1)
    {
        comp_bitstream_read_char(in_stream, &c);
        if(comp_bitstream_eof(in_stream))
            break;
        huff->freq[(unsigned char) c]++;
    }
    huffman_build_tree(huff);
    huffman_build_code(huff);
#ifdef DEBUG
    for(int i = 0; i < comp_vec_len(huff->symbols); i++)
    {
        comp_huffman_symbol_t* sym = comp_vec_get(huff->symbols, i);
        HUFFMAN_DEBUG("%c: %s", sym->symbol, huff->symbol_code_table[sym->symbol]);
    }
#endif
    return 0;
}

int decode(comp_huffman_t* huff, FILE* in, FILE* out)
{
    return 0;
}

void comp_huffman_free(comp_huffman_t* huff)
{
    for(int i = 0; i < 256; i++)
        comp_str_free(huff->symbol_code_table[i]);
    comp_vec_free(huff->symbols);
    free(huff);
}