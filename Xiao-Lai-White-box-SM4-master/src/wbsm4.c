#include "wbsm4.h"
#include "sbox.h"

M32 L_matrix = {
    .M[0] = 0xA0202080, 
    .M[1] = 0x50101040, 
    .M[2] = 0x28080820, 
    .M[3] = 0x14040410,
    .M[4] = 0xA020208, 
    .M[5] = 0x5010104, 
    .M[6] = 0x2808082, 
    .M[7] = 0x1404041, 
    .M[8] = 0x80A02020, 
    .M[9] = 0x40501010, 
    .M[10] = 0x20280808, 
    .M[11] = 0x10140404, 
    .M[12] = 0x80A0202, 
    .M[13] = 0x4050101, 
    .M[14] = 0x82028080, 
    .M[15] = 0x41014040, 
    .M[16] = 0x2080A020, 
    .M[17] = 0x10405010, 
    .M[18] = 0x8202808, 
    .M[19] = 0x4101404, 
    .M[20] = 0x2080A02, 
    .M[21] = 0x1040501, 
    .M[22] = 0x80820280, 
    .M[23] = 0x40410140, 
    .M[24] = 0x202080A0, 
    .M[25] = 0x10104050, 
    .M[26] = 0x8082028, 
    .M[27] = 0x4041014, 
    .M[28] = 0x202080A, 
    .M[29] = 0x1010405, 
    .M[30] = 0x80808202, 
    .M[31] = 0x40404101
};

void printstate(unsigned char * in)
{
    int i;
    for(i = 0; i < 16; i++) 
    {
        printf("%.2X", in[i]);
    }
    printf("\n");
}

void wbsm4_gen(uint8_t *key)
{
    int i, j, x;
    Aff32 P[36];
    Aff32 P_inv[36];
    Aff8 Eij[32][4];
    Aff8 Eij_inv[32][4];
    Aff32 Ei_inv[32];
    Aff32 Q[32];
    Aff32 Q_inv[32];

    sm4_context ctx;
    sm4_setkey_enc(&ctx, key);
    InitRandom(((unsigned int)time(NULL)));

    for (i = 0; i < 36; i++) 
    {
        //affine P
        genaffinepairM32(&P[i], &P_inv[i]);
    }

    for (i = 0; i < 32; i++) 
    {
        //affine E
        for (j = 0; j < 4; j++) 
        {
            genaffinepairM8(&Eij[i][j], &Eij_inv[i][j]);
        }

        // combine 4 E8 to 1 E32
        affinecomM8to32(Eij_inv[i][0], Eij_inv[i][1], Eij_inv[i][2], Eij_inv[i][3], &Ei_inv[i]);

        //affine M
        affinemixM32(Ei_inv[i], P_inv[i + 1], &M[i][0]);
        affinemixM32(Ei_inv[i], P_inv[i + 2], &M[i][1]);
        affinemixM32(Ei_inv[i], P_inv[i + 3], &M[i][2]);

        //affine Q
        genaffinepairM32(&Q[i], &Q_inv[i]);

        //affine C D, C for Xi0, D for T(Xi1+Xi2+Xi3+rk)
        affinemixM32(P[i + 4], P_inv[i], &C[i]);
        affinemixM32(P[i + 4], Q_inv[i], &D[i]);
        uint32_t temp_u32 = cus_random();
        C[i].Vec.V ^= temp_u32;
        D[i].Vec.V ^= P[i + 4].Vec.V ^ temp_u32;
    }

    for (i = 0; i < 32; i++)
    {
        //combine QL
        M32 QL;
        MatMulMatM32(Q[i].Mat, L_matrix, &QL);

        uint32_t Q_constant[3] = {0};
        for(j = 0; j < 3; j++)
        {
            Q_constant[j] = cus_random();
        }

        for (x = 0; x < 256; x++) 
        {
            for (j = 0; j < 4; j++) 
            {
                uint8_t temp_u8 = affineU8(Eij[i][j], x);
                temp_u8 = SBOX[temp_u8 ^ ((ctx.sk[i] >> (24 - j * 8)) & 0xff)];
                uint32_t temp_32 = temp_u8 << (24 - j * 8);
                Table[i][j][x] = MatMulNumM32(QL, temp_32);
            }
            for(j = 0; j < 3; j++)
            {
                Table[i][j][x] ^= Q_constant[j];
            }
            Table[i][3][x] ^=  Q[i].Vec.V ^ Q_constant[0] ^ Q_constant[1] ^ Q_constant[2];
        }
    }

    //external encoding
    for (i = 0; i < 4; i++) 
    {
        SE[i].Mat = P[i].Mat;
        SE[i].Vec = P[i].Vec;

        FE[i].Mat = P_inv[35 - i].Mat;
        FE[i].Vec = P_inv[35 - i].Vec;
    }
}

void wbsm4_encrypt(unsigned char IN[], unsigned char OUT[])
{
    int i;
    uint32_t x0, x1, x2, x3, x4;
    uint32_t xt0, xt1, xt2, xt3, xt4;
    
    x0 = GET32(IN);
    x1 = GET32(IN + 4);
    x2 = GET32(IN + 8);
    x3 = GET32(IN + 12);

    for(i = 0; i < 32; i++)
    {
        xt1 = affineU32(M[i][0], x1);
        xt2 = affineU32(M[i][1], x2);
        xt3 = affineU32(M[i][2], x3);
        x4 = xt1 ^ xt2 ^ xt3;
        x4 = Table[i][0][(x4 >> 24) & 0xff] ^ Table[i][1][(x4 >> 16) & 0xff] ^ Table[i][2][(x4 >> 8) & 0xff] ^ Table[i][3][x4 & 0xff];
        xt0 = affineU32(C[i], x0);
        xt4 = affineU32(D[i], x4);
        x4 = xt0 ^ xt4;
        
        x0 = x1;
        x1 = x2;
        x2 = x3;
        x3 = x4;
    }

    PUT32(x3, OUT);
    PUT32(x2, OUT + 4);
    PUT32(x1, OUT + 8);
    PUT32(x0, OUT + 12);
}