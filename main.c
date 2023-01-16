#include "comp.h"

int main(int argc, char* argv[]) {
    comp_compressor_t* c = comp_compressor_init(COMP_CODEC_HUFFMAN);
    if(!c) return 0;
    if(argc < 3) return 0;
    c->compress(c, argv[1], argv[2]);
    comp_compressor_free(c);
    return 0;
}
