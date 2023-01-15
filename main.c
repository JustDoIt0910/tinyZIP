#include "huffman.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    comp_huffman_ctx_t* huff = comp_huffman_init();
    if(!huff) return 0;
    if(argc < 2)
        return 0;
    FILE* in = fopen(argv[1], "rb");
//    FILE* out = fopen("huff_out", "wb");
//    huff->huffman_encode(huff, in, out);

    huff->huffman_decode(huff, in, NULL);
    comp_huffman_free(huff);
    return 0;
}
