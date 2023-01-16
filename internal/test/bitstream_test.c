//
// Created by zr on 23-1-13.
//
#include "../bitstream.h"

int main() {
//    FILE* fp = fopen("test", "rb+");
//    comp_bitstream_t* bs = comp_bitstream_init(fp);
//    if(!bs)
//        return 0;
//    int bit;
//    while(1)
//    {
//        comp_bitstream_read_bit(bs, &bit);
//        if(comp_bitstream_eof(bs))
//            break;
//        printf("%d", bit);
//    }
//    comp_bitstream_destroy(bs);

//    FILE* fp = fopen("test2", "wb+");
//    comp_bitstream_t* bs = comp_bitstream_init(fp);
//    if(!bs)
//        return 0;
//    comp_bitstream_write_bit(bs, 1);
//    comp_bitstream_write_bit(bs, 1);
//    comp_bitstream_write_bit(bs, 1);
//    comp_bitstream_write_bit(bs, 1);
//    comp_bitstream_write_bit(bs, 0);
//    comp_bitstream_write_bit(bs, 1);
//    comp_bitstream_write_char(bs, 'a');
//    comp_bitstream_destroy(bs);

//    FILE* fp = fopen("test3", "rb+");
//    comp_bitstream_t* bs = comp_bitstream_init(fp);
//    if(!bs)
//       return 0;
//    char ch;
//    for(int i = 0; i < 10; i++)
//    {
//        comp_bitstream_read_char(bs, &ch);
//        printf("%x ", ch);
//    }

//    FILE* fp = fopen("test", "wb+");
//    comp_bitstream_t* bs = comp_bitstream_init(fp);
//    u_int32_t i = 0xFFFFFFFF;
//    comp_bitstream_write_int(bs, (int) i);
//
//    comp_bitstream_reset(bs);
//    int x;
//    comp_bitstream_read_int(bs, &x);
//    printf("%u", (u_int32_t)x);

    FILE* fp = fopen("test", "wb+");
    comp_bitstream_t* bs = comp_bitstream_init(fp);
    u_int16_t i = 0x9100;
    comp_bitstream_write_short(bs, (short) i);

    comp_bitstream_reset(bs);
    short x;
    comp_bitstream_read_short(bs, &x);
    printf("%x", (u_int16_t)x);

    return 0;
}