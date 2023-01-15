#include "huffman.h"
#include <stdio.h>

int main() {
    comp_huffman_t* huff = comp_huffman_init();
    if(!huff) return 0;
    FILE* in = fopen("huff2", "rb+");
    huff->huffman_encode(huff, in, NULL);
    return 0;
}
