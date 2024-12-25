#include "wbsm4.h"
#include "sbox.h"

#define  SHL(x,n) (((x) & 0xFFFFFFFF) << n)
#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - n)))

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
        printf("%.2X", in[i]);   //16进制输出，大写
    }
    printf("\n");
}

//generate 8 bit non-linear code,tested successfully
void non_linear8(uint8_t Out[32][4][256], uint8_t In[32][4][256])
{
    srand((unsigned int)time(NULL));
    for (int r = 0; r < 32; r++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 256; j++)
            {
                Out[r][i][j] = j;
            }
        }
    }

    //random Out
    for (int r = 0; r < 32; r++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 256; j++)
            {
                int randNum = rand() % 256;
                //cout << randNum <<endl;
                int t = Out[r][i][j];
                Out[r][i][j] = Out[r][i][randNum];
                Out[r][i][randNum] = t;

            }
        }
    }

    //In
    for (int r = 0; r < 32; r++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 256; j++)
            {
                In[r][i][Out[r][i][j]] = j;
            }

        }
    }

    return;
}

void split32to8_mat(Aff32 MB, Aff8 MB_ij[4][4])
{   
    for (int r = 0; r < 32; r++) {
        uint32_t m = MB.Mat.M[r];
        MB_ij[r / 8][0].Mat.M[r % 8] = (m >> 24) & 0xff;
        MB_ij[r / 8][1].Mat.M[r % 8] = (m >> 16) & 0xff;
        MB_ij[r / 8][2].Mat.M[r % 8] = (m >> 8) & 0xff;
        MB_ij[r / 8][3].Mat.M[r % 8] = (m >> 0) & 0xff;
    }
}

void gen_table()
{
    int i, j, k, x, m, r;
    uint8_t table_in_x[32][4][256];
    V32 rmask, mask;
    mask.V = 0;
    rmask.V = 0;

    //affine Q
    for (i = 0; i < 36; i++)
    {
        genaffinepairM32(&Q[i], &Q_inv[i]);
    }

    //affine Eij,Aij,Ei_inv,Ai
    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < 4; j++)
        {
            genaffinepairM8(&Eij[i][j], &Eij_inv[i][j]);   
            genaffinepairM8(&Aij[i][j], &Aij_inv[i][j]);
        }
        affinecomM8to32(Eij_inv[i][0], Eij_inv[i][1], Eij_inv[i][2], Eij_inv[i][3], &Ei_inv[i]);   
        affinemixM32(Ei_inv[i], Q_inv[i + 1], &M[i][0]);
        affinemixM32(Ei_inv[i], Q_inv[i + 2], &M[i][1]);
        affinemixM32(Ei_inv[i], Q_inv[i + 3], &M[i][2]);
    }

    //affine C
    for (i = 0; i < 32; i++)
    {
        affinemixM32(Q[i + 4], Q_inv[i], &C[i]); //x0
    }

    //external SE,FE
    for (i = 0; i < 4; i++)
    {
        SE[i].Mat = Q[i].Mat;
        SE[i].Vec = Q[i].Vec;

        FE[i].Mat = Q_inv[35 - i].Mat;
        FE[i].Vec = Q_inv[35 - i].Vec;
    }

    //non-linear
    non_linear8(Out, In);

    //mask
    for (r = 0; r < 32; r++)
    {
        Mask[r] = cus_random();
    }


    //remove x's affine e
    for (r = 0; r < 32; r++)
    {
        for (i = 0; i < 4; i++)
        {
            for (x = 0; x < 256; x++)
            {
                table_in_x[r][i][x] = affineU8(Eij[r][i], x);
            }
        }
    }
    
    //round 1-31:remove mask
    for (r = 1; r < 32; r++)
    {
        for (i = 0; i < 4; i++)
        {
            for (x = 0; x < 256; x++)
            {
                for (m = 0; m < r; m++)
                {
                    if ((m + 1) % 4 == 0)
                    {
                        continue;
                    }
                    else
                    {
                        uint8_t y = table_in_x[r][i][x];
                        mask.V = Mask[r - 1 - m];
                        MatMulVecM32(L_matrix, mask, &rmask);
                        uint8_t temp = y ^ (rmask.V >> (24 - 8 * i) & 0xff);
                        table_in_x[r][i][x] = temp;
                    }
                }
            }
        }
    }

    //gen_table TypeI
    for (r = 0; r < 32; r++)
    {
        for (i = 0; i < 4; i++)
        {
            for (x = 0; x < 256; x++)
            {
                for (k = 0; k < 256; k++)
                {
                    int tk = In[r][i][k];
                    tk = affineU8(Eij[r][i], tk);
                    uint8_t temp = SBOX[tk ^ table_in_x[r][i][x]];
                    temp = temp ^ (Mask[r] >> (24 - 8 * i) & 0xff); //add mask
                    TypeI[r][i][x][k] = affineU8(Aij_inv[r][i], temp);  //add affine A
                }
            }
        }
    }

    //gen_table TypeII
    for (r = 0; r < 32; r++)
    {
        for (i = 0; i < 4; i++)
        {
            for (x = 0; x < 256; x++)
            {
                uint8_t temp8 = affineU8(Aij[r][i], x);
                uint32_t temp32 = temp8 << (24 - 8 * i);
                temp32 = temp32 ^ (ROTL(temp32, 2)) ^ (ROTL(temp32, 10)) ^ (ROTL(temp32, 18)) ^ (ROTL(temp32, 24));
                TypeII[r][i][x] = affineU32(Q[r + 4], temp32);
            }
        }
    }

    //remove output mask
    for (r = 28; r < 32; r++)
    {
        uint32_t temp = 0;
        V32 temp_mask;
        for (m = 0; m < 8; m++)
        {
            temp ^= Mask[r - m * 4];    
        }
        mask.V = temp;
        MatMulVecM32(L_matrix, mask, &rmask);
        MatMulVecM32(Q[r + 4].Mat, rmask, &temp_mask);
        re_mask[r - 28] = temp_mask.V;
    }
    
}

void wbkey_gen(uint8_t* key)
{
    sm4_context ctx;
    sm4_setkey_enc(&ctx, key);
    //sm4_setkey_dec(&ctx,key);
    int r, i;
    int temp[4];

    for (r = 0; r < 32; r++)
    {
        for (i = 0; i < 4; i++)
        {
            uint8_t m = ctx.sk[r] >> (24 - 8 * i) & 0xff;
            temp[i] = affineU8(Eij_inv[r][i], m);
            temp[i] = Out[r][i][temp[i]];
        }
        wbkey[r] = temp[0] << 24 | temp[1] << 16 | temp[2] << 8 | temp[3];
    }
    //printf("0x%0.8x,\n", ctx.sk[1]);
}

void wbdkey_gen(uint8_t* key)
{
    sm4_context ctx;
    sm4_setkey_enc(&ctx, key);
    //sm4_setkey_dec(&ctx,key);
    int r, i;
    int temp[4];

    for (r = 0; r < 32; r++)
    {
        for (i = 0; i < 4; i++)
        {
            uint8_t m = ctx.sk[r] >> (24 - 8 * i) & 0xff;
            temp[i] = affineU8(Eij_inv[r][i], m);
            temp[i] = Out[r][i][temp[i]];
        }
        wbkey[r] = temp[0] << 24 | temp[1] << 16 | temp[2] << 8 | temp[3];
    }


    //printf("0x%0.8x,\n", ctx.sk[1]);
}

void encrypt(unsigned char IN[], unsigned char OUT[])
{
    int i;
    uint32_t x0, x1, x2, x3, x4;
    uint32_t xt0, xt1, xt2, xt3, xt4;

    x0 = GET32(IN);
    x1 = GET32(IN + 4);
    x2 = GET32(IN + 8);
    x3 = GET32(IN + 12);

    for (i = 0; i < 32; i++)
    {
        xt1 = affineU32(M[i][0], x1);
        xt2 = affineU32(M[i][1], x2);
        xt3 = affineU32(M[i][2], x3);
        x4 = xt1 ^ xt2 ^ xt3;
        //x4 = TypeI[i][0][(x4 >> 24) & 0xff][(wbkey[i] >> 24) & 0xff] << 24 | TypeI[i][1][(x4 >> 16) & 0xff][(wbkey[i] >> 16) & 0xff] << 16 | TypeI[i][2][(x4 >> 8) & 0xff][(wbkey[i] >> 8) & 0xff] << 8 | TypeI[i][3][x4 & 0xff][wbkey[i] & 0xff];
        x4 = TypeI[i][0][(x4 >> 24) & 0xff][(wbkey[i] >> 24) & 0xff] << 24 | TypeI[i][1][(x4 >> 16) & 0xff][(wbkey[i] >> 16) & 0xff] << 16 | TypeI[i][2][(x4 >> 8) & 0xff][(wbkey[i] >> 8) & 0xff] << 8 | TypeI[i][3][x4 & 0xff][wbkey[i] & 0xff];

        xt0 = affineU32(C[i], x0);
        xt4 = TypeII[i][0][(x4 >> 24) & 0xff] ^ TypeII[i][1][(x4 >> 16) & 0xff] ^ TypeII[i][2][(x4 >> 8) & 0xff] ^ TypeII[i][3][x4 & 0xff];
        x4 = xt0 ^ xt4;

        x0 = x1;
        x1 = x2;
        x2 = x3;
        x3 = x4;
    }

    x0 ^= re_mask[0];
    x1 ^= re_mask[1];
    x2 ^= re_mask[2];
    x3 ^= re_mask[3];

    PUT32(x3, OUT);
    PUT32(x2, OUT + 4);
    PUT32(x1, OUT + 8);
    PUT32(x0, OUT + 12);
}

void decrypt(unsigned char IN[], unsigned char OUT[])
{
    int i;
    uint32_t x0, x1, x2, x3, x4;
    uint32_t xt0, xt1, xt2, xt3, xt4;

    x0 = GET32(IN);
    x1 = GET32(IN + 4);
    x2 = GET32(IN + 8);
    x3 = GET32(IN + 12);

    for (i = 0; i < 32; i++)
    {
        xt1 = affineU32(M[i][0], x1);
        xt2 = affineU32(M[i][1], x2);
        xt3 = affineU32(M[i][2], x3);
        x4 = xt1 ^ xt2 ^ xt3;
        //x4 = TypeI[i][0][(x4 >> 24) & 0xff][(wbkey[i] >> 24) & 0xff] << 24 | TypeI[i][1][(x4 >> 16) & 0xff][(wbkey[i] >> 16) & 0xff] << 16 | TypeI[i][2][(x4 >> 8) & 0xff][(wbkey[i] >> 8) & 0xff] << 8 | TypeI[i][3][x4 & 0xff][wbkey[i] & 0xff];
        x4 = TypeI[i][0][(x4 >> 24) & 0xff][(wbkey[31-i] >> 24) & 0xff] << 24 | TypeI[i][1][(x4 >> 16) & 0xff][(wbkey[31-i] >> 16) & 0xff] << 16 | TypeI[i][2][(x4 >> 8) & 0xff][(wbkey[31-i] >> 8) & 0xff] << 8 | TypeI[31-i][3][x4 & 0xff][wbkey[31-i] & 0xff];

        xt0 = affineU32(C[i], x0);
        xt4 = TypeII[i][0][(x4 >> 24) & 0xff] ^ TypeII[i][1][(x4 >> 16) & 0xff] ^ TypeII[i][2][(x4 >> 8) & 0xff] ^ TypeII[i][3][x4 & 0xff];
        x4 = xt0 ^ xt4;

        x0 = x1;
        x1 = x2;
        x2 = x3;
        x3 = x4;
    }

    x0 ^= re_mask[0];
    x1 ^= re_mask[1];
    x2 ^= re_mask[2];
    x3 ^= re_mask[3];

    PUT32(x3, OUT);
    PUT32(x2, OUT + 4);
    PUT32(x1, OUT + 8);
    PUT32(x0, OUT + 12);
}