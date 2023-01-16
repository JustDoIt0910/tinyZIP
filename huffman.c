//
// Created by zr on 23-1-13.
//
#include "huffman.h"
#include "internal/pqueue.h"
#include "internal/bitstream.h"
#include <stdlib.h>
#include <string.h>

static int encode(comp_huffman_ctx_t* huff, comp_bitstream_t* in, comp_bitstream_t* out);
static int decode(comp_huffman_ctx_t* huff, comp_bitstream_t* in, comp_bitstream_t* out);

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

static comp_huffman_node_t* huffman_node_new(u_char c, u_int32_t freq, int is_leaf,
                                             comp_huffman_node_t* lchild, comp_huffman_node_t* rchild)
{
    comp_huffman_node_t* node = (comp_huffman_node_t*) malloc(sizeof(comp_huffman_node_t));
    if(!node) return NULL;
    node->c = c;
    node->freq = freq;
    node->left = lchild;
    node->right = rchild;
    node->is_leaf = is_leaf;
    return node;
}

static comp_huffman_symbol_t* huffman_symbol_new(u_char sym, size_t code_len)
{
    comp_huffman_symbol_t* huff_symbol = (comp_huffman_symbol_t*) malloc(sizeof(comp_huffman_symbol_t));
    if(!huff_symbol) return NULL;
    huff_symbol->symbol_code_len = code_len;
    huff_symbol->symbol = sym;
    return huff_symbol;
}

comp_huffman_ctx_t* comp_huffman_init()
{
    comp_huffman_ctx_t* huff = (comp_huffman_ctx_t*) malloc(sizeof(comp_huffman_ctx_t));
    if(!huff) return NULL;
    memset(huff->freq, 0, HUFFMAN_MAX_SYMBOL);
    for(int i = 0; i < HUFFMAN_MAX_SYMBOL; i++)
        huff->symbol_code_table[i] = comp_str_empty();
    huff->symbols = comp_vec_init(64);
    huff->root = NULL;
    huff->padding = 0;
    huff->content_len = 0;
    huff->huffman_encode = encode;
    huff->huffman_decode = decode;
    return huff;
}

static int huffman_build_tree(comp_huffman_ctx_t* huff)
{
    comp_pqueue_t* pq = comp_pqueue_init(64, huffman_node_pri_cmp);
    if(!pq) return -1;
    for(int c = 0; c < HUFFMAN_MAX_SYMBOL; c++)
        if(huff->freq[c] > 0)
        {
            comp_huffman_node_t* node = huffman_node_new((u_char)c, huff->freq[c], 1, NULL, NULL);
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
        comp_huffman_node_t* parent = huffman_node_new(0, x->freq + y->freq, 0, x, y);
        comp_pqueue_insert(pq, parent);
    }
    huff->root = comp_pqueue_pop(pq);
    comp_pqueue_destroy(pq);
    return 0;
}

static void get_code_len(comp_huffman_ctx_t* huff, comp_huffman_node_t* root, size_t code_len)
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

static void huffman_assign_code(comp_huffman_ctx_t* huff, int code, comp_huffman_symbol_t* sym)
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

static void huffman_build_code(comp_huffman_ctx_t* huff)
{
    get_code_len(huff, huff->root, 0);
    comp_vec_sort(huff->symbols, 0, comp_vec_len(huff->symbols) - 1, canonical_symbol_cmp);
    int cnt = 0;
    int code = 0; int pre_min_code = 0;
    size_t pre_code_len = 1;
    u_char remain = 0;
    for(int i = 0; i < comp_vec_len(huff->symbols); i++)
    {
        comp_huffman_symbol_t* sym = comp_vec_get(huff->symbols, i);
        remain += (sym->symbol_code_len * huff->freq[sym->symbol]) % 8;
        huff->content_len += huff->freq[sym->symbol];
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
    huff->padding = 8 - remain % 8;
}

void huffman_write_header(comp_huffman_ctx_t* huff, size_t header_len, comp_bitstream_t* out_stream)
{
    comp_bitstream_write_char(out_stream, HUFFMAN_HEADER_MARKER);
    u_char header_len_high = (header_len >> 8) & 0xFF;
    u_char header_len_low = header_len & 0xFF;
    comp_bitstream_write_char(out_stream, (char) header_len_high);
    comp_bitstream_write_char(out_stream, (char) header_len_low);
    comp_bitstream_write_int(out_stream, (int) huff->content_len);
    char num[17] = {0};
    for(int i = 0; i < comp_vec_len(huff->symbols); i++)
        num[HUFFMAN_GET_SYMBOL_LEN(i)]++;
    comp_bitstream_write(out_stream, num + 1, 16);
    for(int i = 0; i < comp_vec_len(huff->symbols); i++)
        comp_bitstream_write_char(out_stream, HUFFMAN_GET_SYMBOL(i));
#ifdef DEBUG
    HUFFMAN_DEBUG("padding = %d", huff->padding);
#endif
    comp_bitstream_write_char(out_stream, (char) huff->padding);
}

static void huffman_free_tree(comp_huffman_node_t* root)
{
    if(root->is_leaf)
    {
        free(root);
        return;
    }
    huffman_free_tree(root->left);
    huffman_free_tree(root->right);
    free(root);
}

static void huffman_ctx_cleanup(comp_huffman_ctx_t* huff)
{
    memset(huff->freq, 0, HUFFMAN_MAX_SYMBOL);
    for(int i = 0; i < HUFFMAN_MAX_SYMBOL; i++)
        comp_str_clear(huff->symbol_code_table[i]);
    for(int i = 0; i < comp_vec_len(huff->symbols); i++)
        free(comp_vec_get(huff->symbols, i));
    comp_vec_clear(huff->symbols);
    if(huff->root)
        huffman_free_tree(huff->root);
    huff->padding = 0;
    huff->content_len = 0;
    huff->root = NULL;
}

static void huffman_encode_content(comp_huffman_ctx_t* huff, comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    for(int i = 0; i < huff->padding; i++)
        comp_bitstream_write_bit(out_stream, 0);
    char input;
    while(1)
    {
        comp_bitstream_read_char(in_stream, &input);
        if(comp_bitstream_eof(in_stream))
            break;
        comp_str_t code = huff->symbol_code_table[(u_char) input];
        for(int i = 0; i < comp_str_len(code); i++)
            comp_bitstream_write_bit(out_stream, comp_str_at(code, i) - '0');
    }
    comp_bitstream_flush(out_stream);
}

int encode(comp_huffman_ctx_t* huff, comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    if(!in_stream || !out_stream) return -1;
    char c;
    // 2 bytes header_len + 4 bytes content_len + 16 bytes symbol num + 1 byte padding_len
    size_t huffman_header_len = 2 + 4 + 16 + 1;
    while(1)
    {
        comp_bitstream_read_char(in_stream, &c);
        if(comp_bitstream_eof(in_stream))
            break;
        if(huff->freq[(u_char) c]++ == 0)
            huffman_header_len++;
    }
    huffman_build_tree(huff);
    huffman_build_code(huff);
#ifdef DEBUG
    for(int i = 0; i < comp_vec_len(huff->symbols); i++)
    {
        comp_huffman_symbol_t* sym = comp_vec_get(huff->symbols, i);
        HUFFMAN_DEBUG("%x: %s", sym->symbol, huff->symbol_code_table[sym->symbol]);
    }
#endif
    huffman_write_header(huff, huffman_header_len, out_stream);
    comp_bitstream_reset(in_stream);
    huffman_encode_content(huff, in_stream, out_stream);
    huffman_ctx_cleanup(huff);
    return 0;
}

static int huffman_read_header(comp_huffman_ctx_t* huff, comp_bitstream_t* in_stream)
{
    char input;
    comp_bitstream_read_char(in_stream, &input);
    if(input != HUFFMAN_HEADER_MARKER)
        return -1;
    char hdr_high, hdr_low;
    comp_bitstream_read_char(in_stream, &hdr_high);
    comp_bitstream_read_char(in_stream, &hdr_low);
    size_t huffman_hdr_len = (u_char)hdr_high << 8 | (u_char)hdr_low;
    huffman_hdr_len -= 2;
    int content_len;
    comp_bitstream_read_int(in_stream, &content_len);
    huff->content_len = content_len;
    huffman_hdr_len -= 4;
    char num[17] = {0};
    for(int i = 1; i <= 16; i++)
        if(comp_bitstream_read_char(in_stream, num + i) < 0)
            return -1;
    huffman_hdr_len -= 16;
    for(int i = 1; i <= 16; i++)
        for(int j = 0; j < num[i]; j++)
        {
            comp_bitstream_read_char(in_stream, &input);
            comp_huffman_symbol_t* symbol = huffman_symbol_new(input, i);
            comp_vec_push_back(huff->symbols, symbol);
            huffman_hdr_len -= 1;
        }
    comp_bitstream_read_char(in_stream, &input);
    huff->padding = input;
    huffman_hdr_len -= 1;
    return huffman_hdr_len == 0 ? 0 : -1;
}

static void huffman_decode_content(comp_huffman_ctx_t* huff, comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    for(int i = 0; i < huff->padding; i++)
        comp_bitstream_read_bit(in_stream, NULL);
    int bit;
    comp_huffman_node_t* huff_node = huff->root;
    while(1)
    {
        comp_bitstream_read_bit(in_stream, &bit);
        if(comp_bitstream_eof(in_stream))
            break;
        if(!bit) huff_node = huff_node->left;
        else huff_node = huff_node->right;
        if(huff_node->is_leaf)
        {

            comp_bitstream_write_char(out_stream, (char) huff_node->c);
            huff_node = huff->root;
        }
    }
    comp_bitstream_flush(out_stream);
}

static int assign_symbol(comp_huffman_node_t* root, comp_huffman_symbol_t* sym, size_t len)
{
    if(len == sym->symbol_code_len)
    {
        if(!root->left)
        {
            root->left = huffman_node_new(sym->symbol, 0, 1, NULL, NULL);
            return 0;
        }
        if(!root->right)
        {
            root->right = huffman_node_new(sym->symbol, 0, 1, NULL, NULL);
            return 0;
        }
        return -1;
    }
    if(!root->left)
        root->left = huffman_node_new(0, 0, 0, NULL, NULL);
    int assigned = -1;
    if(!root->left->is_leaf)
        assigned = assign_symbol(root->left, sym, len + 1);
    if(assigned < 0)
    {
        if(!root->right)
            root->right = huffman_node_new(0, 0, 0, NULL, NULL);
        if(!root->right->is_leaf)
            return assign_symbol(root->right, sym, len + 1);
        return -1;
    }
    return 0;
}

static int huffman_rebuild_tree(comp_huffman_ctx_t* huff)
{
    huff->root = huffman_node_new(0, 0, 0, NULL, NULL);
    for(int i = 0; i < comp_vec_len(huff->symbols); i++)
        if(assign_symbol(huff->root, (comp_huffman_symbol_t*) comp_vec_get(huff->symbols, i), 1) < 0)
            return -1;
    return 0;
}

// for debugging
static void print(comp_huffman_node_t* root, comp_str_t code)
{
    if(root->is_leaf)
    {
        printf("%x ==> %s\n", root->c, code);
        comp_str_free(code);
        return;
    }
    comp_str_t left_code = comp_str_new(code);
    comp_str_t right_code = comp_str_new(code);
    left_code = comp_str_append_char(left_code, '0');
    right_code = comp_str_append_char(right_code, '1');
    comp_str_free(code);
    print(root->left, left_code);
    print(root->right, right_code);
}
// for debugging
void huffman_print(comp_huffman_ctx_t* huff)
{
    print(huff->root, comp_str_empty());
}

int decode(comp_huffman_ctx_t* huff, comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    if(!in_stream || !out_stream) return -1;
    int err = 0;
    if(huffman_read_header(huff, in_stream) < 0)
    {
        err = 1;
        goto end;
    }
    if(huffman_rebuild_tree(huff) < 0)
    {
        HUFFMAN_DEBUG("%s", "rebuild huffman tree fail");
        err = 1;
        goto end;
    }
#ifdef DEBUG
    huffman_print(huff);
#endif
    huffman_decode_content(huff, in_stream, out_stream);

end:
    huffman_ctx_cleanup(huff);
    return err == 0 ? 0 : -1;
}

void comp_huffman_free(comp_huffman_ctx_t* huff)
{
    for(int i = 0; i < 256; i++)
        comp_str_free(huff->symbol_code_table[i]);
    comp_vec_free(huff->symbols);
    free(huff);
}