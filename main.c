#include "huffman.h"
#include "comp.h"
#include <stdio.h>

extern int encode(comp_huffman_ctx_t* huff, FILE* in, FILE* out);

int main(int argc, char* argv[]) {
    comp_huffman_ctx_t* huff = comp_huffman_init();
    if(!huff) return 0;
    if(argc < 2)
        return 0;
//    FILE* in = fopen(argv[1], "rb");
//    FILE* out = fopen("huff_out", "wb");
//    huff->huffman_encode(huff, in, out);
    FILE* in = fopen(argv[1], "rb");
    FILE* out = fopen("minesweeper_dec", "wb");
    huff->huffman_decode(huff, in, out);
    comp_huffman_free(huff);
    return 0;
}
