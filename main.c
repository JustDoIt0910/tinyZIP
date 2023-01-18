#include "comp.h"
#include <string.h>

void usage()
{
    printf("Usage: compress [-c compress] [-d decompress] input_file [output_file]\n");
}

void default_output_filename(const char* input, char* output)
{
    const char* ptr = strrchr(input, '.');
    size_t n = ptr ? ptr - input : strlen(input);
    memcpy(output, input, n);
    strcat(output, ".tz");
}

int main(int argc, char* argv[]) {
    comp_compressor_t* c = comp_compressor_init(COMP_CODEC_HUFFMAN);
    if(!c) return 0;
    if(argc < 2)
    {
        usage();
        comp_compressor_free(c);
        return 0;
    }
    if(!strcmp(argv[1], "-c"))
    {
        if(argc < 3)
        {
            usage();
            comp_compressor_free(c);
            return 0;
        }
        if(argc == 3)
        {
            char output[100] = {0};
            default_output_filename(argv[2], output);
            c->compress(c, argv[2], output);
        }
        else
            c->compress(c, argv[2], argv[3]);
    }
    else if(!strcmp(argv[1], "-d"))
    {
        if(argc < 3)
        {
            usage();
            comp_compressor_free(c);
            return 0;
        }
        c->decompress(c, argv[2]);
    }
    else
        usage();
    comp_compressor_free(c);
    return 0;
}
