#include "wbsm4.h"

int main()
{
    unsigned char IN[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    unsigned char key[16] = {0};
    unsigned char EX_IN[16];
    unsigned char EX_OUT[16];
    unsigned char OUT[16];
    unsigned char OOUT[16];
    uint32_t x0, x1, x2, x3;

    wbsm4_gen(key);
    // external encodings
    x0 = GET32(IN);
    x1 = GET32(IN + 4);
    x2 = GET32(IN + 8);
    x3 = GET32(IN + 12);
    x0 = affineU32(SE[0], x0);
    x1 = affineU32(SE[1], x1);
    x2 = affineU32(SE[2], x2);
    x3 = affineU32(SE[3], x3);
    PUT32(x0, EX_IN);
    PUT32(x1, EX_IN + 4);
    PUT32(x2, EX_IN + 8);
    PUT32(x3, EX_IN + 12);
    // encryption
    wbsm4_encrypt(EX_IN, EX_OUT);
    // external encodings
    x0 = GET32(EX_OUT);
    x1 = GET32(EX_OUT + 4);
    x2 = GET32(EX_OUT + 8);
    x3 = GET32(EX_OUT + 12);
    x0 = affineU32(FE[0], x0);
    x1 = affineU32(FE[1], x1);
    x2 = affineU32(FE[2], x2);
    x3 = affineU32(FE[3], x3);
    PUT32(x0, OUT);
    PUT32(x1, OUT + 4);
    PUT32(x2, OUT + 8);
    PUT32(x3, OUT + 12);
    printstate(OUT);
    
    sm4_context ctx;
    sm4_setkey_enc(&ctx, key);
    sm4_crypt_ecb(&ctx, 1, 16, IN, OOUT);
    printstate(OOUT);
    return 0;
}