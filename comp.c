//
// Created by zr on 23-1-13.
//
#include "comp.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include "marker.h"

static int comp_codec_encode(comp_codec_t*, comp_bitstream_t*, comp_bitstream_t*);
static int comp_codec_decode(comp_codec_t*, comp_bitstream_t*, comp_bitstream_t*);
static void comp_compress(comp_compressor_t*, const char*, const char*);
static void comp_decompress(comp_compressor_t*, const char*);

static comp_huffman_codec_t* huffman_codec_new(comp_progress_bar* bar)
{
    comp_huffman_codec_t* codec = (comp_huffman_codec_t*) malloc(sizeof(comp_huffman_codec_t));
    if(!codec) return NULL;
    CODEC_PARENT_INIT(codec, COMP_CODEC_HUFFMAN, comp_codec_encode, comp_codec_decode);
    codec->huffman_ctx = comp_huffman_init(bar);
    if(!codec->huffman_ctx)
    {
        free(codec);
        return NULL;
    }
    return codec;
}

static comp_lzw_codec_t* lzw_codec_new(comp_progress_bar* bar)
{
    comp_lzw_codec_t* codec = (comp_lzw_codec_t*) malloc(sizeof(comp_lzw_codec_t));
    if(!codec) return NULL;
    CODEC_PARENT_INIT(codec, COMP_CODEC_LZW, comp_codec_encode, comp_codec_decode);
    return codec;
}

comp_codec_t* comp_codec_init(comp_codec_type type, comp_progress_bar* bar)
{
    comp_codec_t* codec = NULL;
    switch (type)
    {
        case COMP_CODEC_HUFFMAN:
            codec = (comp_codec_t*) huffman_codec_new(bar);
            break;
        case COMP_CODEC_LZW:
            codec = (comp_codec_t*) lzw_codec_new(bar);
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
    c->bar = comp_bar_init("", 0);
    c->codec = comp_codec_init(type, c->bar);
    if(!c->codec) return NULL;
    c->state = COMP_PARSE_STOP;
    c->cur_decompress_dir = comp_str_empty();
    c->decompress_dir_stack = comp_vec_init(10);
    c->compress = comp_compress;
    c->decompress = comp_decompress;
    return c;
}

void comp_compressor_free(comp_compressor_t* c)
{
    if(!c) return;
    comp_codec_free(c->codec);
    comp_str_free(c->cur_decompress_dir);
    comp_vec_free(c->decompress_dir_stack);
    comp_bar_free(c->bar);
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

static comp_str_t basename(const char* path)
{
    const char* ptr = strrchr(path, '/');
    if(!ptr)
        return comp_str_new(path);
    return comp_str_new_len(ptr + 1, strlen(ptr + 1));
}

static size_t get_dir_size(comp_str_t path)
{
    DIR* dir = opendir(path);
    struct dirent* entry;
    size_t sz = 0;
    struct stat st;
    while((entry = readdir(dir)) != NULL)
    {
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        comp_str_t new_path = comp_str_new(path);
        new_path = comp_str_append_char(new_path, '/');
        new_path = comp_str_append_str(new_path, entry->d_name);
        if(entry->d_type == DT_REG)
        {
            stat(new_path, &st);
            sz += st.st_size;
        }
        else if(entry->d_type == DT_DIR)
            sz += get_dir_size(new_path);
        comp_str_free(new_path);
    }
    return sz;
}

static int comp_compress_file(comp_compressor_t* c, comp_str_t filename,
                              comp_bitstream_t* in_stream, comp_bitstream_t* out_stream)
{
    comp_bitstream_write_char(out_stream, COMP_FILE_MARKER);
    comp_bitstream_write_char(out_stream, (char) comp_str_len(filename));
    comp_bitstream_write(out_stream, filename, comp_str_len(filename));
    return c->codec->encode(c->codec, in_stream, out_stream);
}

static int comp_compress_dir(comp_compressor_t* c, comp_str_t dir_path, comp_bitstream_t* out_stream)
{
    comp_bitstream_write_char(out_stream, COMP_DIR_MARKER);
    comp_str_t dirname = basename(dir_path);
    comp_bitstream_write_char(out_stream, (char) comp_str_len(dirname));
    comp_bitstream_write(out_stream, dirname, comp_str_len(dirname));
    comp_str_free(dirname);
    DIR* dir = opendir(dir_path);
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL)
    {
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if(entry->d_type == DT_REG)
        {
            comp_str_t file_path = comp_str_new(dir_path);
            file_path = comp_str_append_char(file_path, '/');
            file_path = comp_str_append_str(file_path, entry->d_name);
#ifdef DEBUG
            printf("compress %s  ", file_path);
#else
            comp_bar_set_title(c->bar, file_path);
#endif
            comp_bitstream_t* in_stream = comp_bitstream_init(fopen(file_path, "rb"));
            comp_str_free(file_path);
            if(!in_stream)
                continue;
            comp_str_t filename = comp_str_new(entry->d_name);
            if(comp_compress_file(c, filename, in_stream, out_stream) < 0)
            {
#ifdef DEBUG
                printf("fail.\n");
#endif
                comp_bitstream_destroy(in_stream);
                comp_str_free(filename);
                return -1;
            }
#ifdef DEBUG
            printf("done.\n");
#endif
            comp_bitstream_destroy(in_stream);
            comp_str_free(filename);
        }
        else if(entry->d_type == DT_DIR)
        {
            comp_str_t new_dir_path = comp_str_new(dir_path);
            new_dir_path = comp_str_append_char(new_dir_path, '/');
            new_dir_path = comp_str_append_str(new_dir_path, entry->d_name);
            if(comp_compress_dir(c, new_dir_path, out_stream) < 0)
            {
                comp_str_free(new_dir_path);
                return -1;
            }
            comp_str_free(new_dir_path);
        }
    }
    comp_bitstream_write_char(out_stream, COMP_DIR_MARKER);
    comp_bitstream_write_char(out_stream, 0);
    return 0;
}

static void comp_compress(comp_compressor_t* c, const char* in_path, const char* out_path)
{
    struct stat st;
    if(stat(in_path, &st) != 0)
    {
        printf("%s isn't a file or directory\n", in_path);
        return;
    }
    size_t sz;
    FILE* out = fopen(out_path, "wb");
    comp_bitstream_t* out_stream = comp_bitstream_init(out);
    if(!out_stream) return;
    comp_bitstream_write_short(out_stream, COMP_START_MARKER);
    if(!S_ISDIR(st.st_mode))
    {
        sz = st.st_size;
        comp_bar_set_total(c->bar, sz);
        FILE* in = fopen(in_path, "rb");
        comp_bitstream_t* in_stream = comp_bitstream_init(in);
        if(!in_stream)
        {
            comp_bitstream_destroy(out_stream);
            return;
        }
#ifndef DEBUG
        comp_bar_set_title(c->bar,in_path);
#else
        printf("compress %s  ", in_path);
#endif
        comp_str_t name = basename(in_path);
#ifndef DEBUG
        comp_compress_file(c, name, in_stream, out_stream);
#else
        if(comp_compress_file(c, name, in_stream, out_stream) < 0)
            printf("fail.\n");
        else printf("done.\n");
#endif
        comp_str_free(name);
        comp_bitstream_destroy(in_stream);
    }
    else
    {
        comp_str_t path = comp_str_new(in_path);
        sz = get_dir_size(path);
        comp_bar_set_total(c->bar, sz);
        comp_compress_dir(c, path, out_stream);
        comp_str_free(path);
    }
    comp_bitstream_destroy(out_stream);
    printf("\n");
}

static int comp_decompress_file(comp_compressor_t* c, comp_bitstream_t* in_stream)
{
    char name_len, input;
    comp_bitstream_read_char(in_stream, &name_len);
    comp_bar_add(c->bar, 1);
    comp_str_t filepath = comp_str_new(c->cur_decompress_dir);
    for(int i = 0; i < (u_char) name_len; i++)
    {
        comp_bitstream_read_char(in_stream, &input);
        filepath = comp_str_append_char(filepath, input);
    }
    comp_bar_add(c->bar, name_len);
#ifdef DEBUG
    printf("decompress %s  ", filepath);
#else
    comp_bar_set_title(c->bar, filepath);
#endif
    FILE* out = fopen(filepath, "wb");
    comp_bitstream_t* out_stream = comp_bitstream_init(out);
    int err;
    if(!out_stream)
    {
        err = -1;
        goto end;
    }
    err = c->codec->decode(c->codec, in_stream, out_stream);
end:
#ifdef DEBUG
    printf(err == -1 ? "fail.\n" : "done.\n");
#endif
    comp_bitstream_destroy(out_stream);
    comp_str_free(filepath);
    return err;
}

static int comp_decompress_dir(comp_compressor_t* c, comp_bitstream_t* in_stream)
{
    char name_len, input;
    comp_bitstream_read_char(in_stream, &name_len);
    comp_bar_add(c->bar, 1);
    if(name_len == 0)
    {
        comp_str_t parent_dir = comp_vec_pop_back(c->decompress_dir_stack);
        c->cur_decompress_dir = comp_str_assign(c->cur_decompress_dir, parent_dir);
        comp_str_free(parent_dir);
    }
    else
    {
        comp_str_t dir_path = comp_str_new(c->cur_decompress_dir);
        for(int i = 0; i < (u_char) name_len; i++)
        {
            comp_bitstream_read_char(in_stream, &input);
            dir_path = comp_str_append_char(dir_path, input);
        }
        comp_bar_add(c->bar, name_len);
        struct stat st;
        if(stat(dir_path, &st) == 0)
            return -1;
        mkdir(dir_path, S_IRWXU);
        dir_path = comp_str_append_char(dir_path, '/');
        comp_str_t parent_dir = comp_str_new(c->cur_decompress_dir);
        comp_vec_push_back(c->decompress_dir_stack, parent_dir);
        c->cur_decompress_dir = comp_str_assign(c->cur_decompress_dir, dir_path);
        comp_str_free(dir_path);
    }
    return 0;
}

static void comp_decompress(comp_compressor_t* c, const char* in_path)
{
    FILE* in = fopen(in_path, "rb");
    if(!in)
    {
        printf("%s: file doesn't exist\n", in_path);
        return;
    }
    struct stat st;
    stat(in_path, &st);
    comp_bar_set_total(c->bar, st.st_size);
    comp_bitstream_t* in_stream = comp_bitstream_init(in);
    if(!in_stream) return;
    short start_marker; char marker;
    do
    {
        switch (c->state)
        {
            case COMP_PARSE_STOP:
                comp_bitstream_read_short(in_stream, &start_marker);
                comp_bar_add(c->bar, 2);
                if((u_int16_t) start_marker == COMP_START_MARKER)
                    c->state = COMP_PARSE_START;
                break;
            case COMP_PARSE_START:
                comp_bitstream_read_char(in_stream, &marker);
                if(comp_bitstream_eof(in_stream))
                {
                    c->state = COMP_PARSE_STOP;
                    break;
                }
                comp_bar_add(c->bar, 1);
                if((u_char) marker == COMP_FILE_MARKER)
                    c->state = COMP_PARSE_FILE;
                else if((u_char) marker == COMP_DIR_MARKER)
                    c->state = COMP_PARSE_DIR;
                else c->state = COMP_PARSE_FAIL;
                break;
            case COMP_PARSE_FILE:
                if(comp_decompress_file(c, in_stream) < 0)
                    c->state = COMP_PARSE_FAIL;
                else c->state = COMP_PARSE_START;
                break;
            case COMP_PARSE_DIR:
                if(comp_decompress_dir(c, in_stream) < 0)
                    c->state = COMP_PARSE_FAIL;
                else c->state = COMP_PARSE_START;
                break;
            default:
                break;
        }
    } while (c->state != COMP_PARSE_STOP && c->state != COMP_PARSE_FAIL);
    comp_bitstream_destroy(in_stream);
    printf("\n");
}