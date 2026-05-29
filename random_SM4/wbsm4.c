#include "wbsm4.h"
#include "sbox.h"

Aff32 P[36];
Aff32 P_inv[36];//Xi+k

Aff8 E_ij[32][4];
Aff8 E_ij_inv[32][4];
Aff32 E_i[32];//for test
Aff32 E_i_inv[32];

Aff8 E_1_ij[8][4];
Aff8 E_1_ij_inv[8][4];
Aff32 E_1_i[8];
Aff32 E_1_i_inv[8];

M8 Nj[4];
M8 Nj_inv[4];
M32 N; //mask after L
M32 N_inv;

Aff8 R[4];
Aff8 R_inv[4];
Aff32 R_i[4];//Mask

Aff32 Q[28];
Aff32 Q_inv[28];

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

void generatePermutation(uint8_t* permutation, uint8_t* inverse)
{
    int i, j;
    uint8_t temp;
    for (i = 0; i < 256; i++)
    {
        permutation[i] = i;
    }
    for (i = 0; i < 255; i++)
    {
        j = cus_random() % (256 - i);
        temp = permutation[i];
        permutation[i] = permutation[i + j];
        permutation[i + j] = temp;
    }
    for (i = 0; i < 256; i++)
    {
        inverse[permutation[i]] = i;
    }
}

void wbsm4_gen(uint8_t *key)
{
    int i, j, x, y;
    V8 tempV8;
    uint8_t temp8, temp_2;
    
    sm4_context ctx;
    sm4_setkey_enc(&ctx, key);
    InitRandom(((unsigned int)time(NULL)));

    //affine P[36]
    for (i = 0; i < 36; i++) 
    {
        genaffinepairM32(&P[i], &P_inv[i]);
    }

    //affine E_i_inv[32]
    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < 4; j++)
        {
            genaffinepairM8(&E_ij[i][j], &E_ij_inv[i][j]);
        }
        // combine 4 E8 to 1 E32
        affinecomM8to32(E_ij_inv[i][0], E_ij_inv[i][1], E_ij_inv[i][2], E_ij_inv[i][3], &E_i_inv[i]);
        affinecomM8to32(E_ij[i][0], E_ij[i][1], E_ij[i][2], E_ij[i][3], &E_i[i]);
    }
    
    //affine E_1_i_inv[8],E_1_i[8]
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 4; j++)
        {
            genaffinepairM8(&E_1_ij[i][j], &E_1_ij_inv[i][j]);
        }
        affinecomM8to32(E_1_ij[i][0], E_1_ij[i][1], E_1_ij[i][2], E_1_ij[i][3], &E_1_i[i]);
        affinecomM8to32(E_1_ij_inv[i][0], E_1_ij_inv[i][1], E_1_ij_inv[i][2], E_1_ij_inv[i][3], &E_1_i_inv[i]);
    } 

    //affine R_inv[4],R_i[4]
    for (i = 0; i < 4; i++)
    {
        genaffinepairM8(&R[i], &R_inv[i]);
        affinecomM8to32(R[i], R[i], R[i], R[i], &R_i[i]);
    }

    //M8 Nj,Nj_inv,M32 N
    for (i = 0; i < 4; i++)
    {
        genMatpairM8(&Nj[i], &Nj_inv[i]);
    }
    MatrixcomM8to32(Nj[0], Nj[1], Nj[2], Nj[3], &N);
    MatrixcomM8to32(Nj_inv[0], Nj_inv[1], Nj_inv[2], Nj_inv[3], &N_inv);

    //Aff32 Q,Q_inv
    for (i = 0; i < 24; i++)
    {
        genaffinepairM32(&Q[i], &Q_inv[i]);
    }

    //affine G,H
    for (i = 0; i < 32; i++)
    {
        affinemixM32(E_i_inv[i], P_inv[i + 1], &G[i][0]);
        affinemixM32(E_i_inv[i], P_inv[i + 2], &G[i][1]);
        affinemixM32(E_i_inv[i], P_inv[i + 3], &G[i][2]);
        
        affinemixM32(P[i + 4], P_inv[i], &H[i]);
    }

    //affine I
    for (i = 0; i < 24; i++)
    {
        affinemixM32(P[i + 8], Q_inv[i], &I[i]);
    }

    //affine C,D
    for (i = 0; i < 8; i++)
    {
        MatMulMatM32(L_matrix, E_1_i[i].Mat, &C[i].Mat);
        MatMulVecM32(L_matrix, E_1_i[i].Vec, &C[i].Vec);
        if (i < 4)
        {
            affinemixM32(P[i + 4], C[i], &C[i]);
            MatMulMatM32(L_matrix, R_i[i].Mat, &D[i].Mat);
            MatMulVecM32(L_matrix, R_i[i].Vec, &D[i].Vec);
            MatMulMatM32(N_inv, D[i].Mat, &D[i].Mat);
            MatMulVecM32(N_inv, D[i].Vec, &D[i].Vec);
        }
        else
        {
            affinemixM32(P[i + 28], C[i], &C[i]);
        }
    }

    //affine J
    for (i = 0; i < 8; i++)
    {
        if (i < 4)
        {
            MatMulMatM32(E_i_inv[i + 4].Mat, N, &J[i].Mat);
            J[i].Vec.V = 0;
        }
        else
        {
            MatMulMatM32(P[i + 4].Mat, N, &J[i].Mat);
            J[i].Vec = P[i + 4].Vec;
        }    
    }

    //constant of H,I,C
    for (i = 0; i < 32; i++)
    {
        if (i < 4)
        {
            uint32_t temp_u32 = cus_random();
            C[i].Vec.V ^= temp_u32;
            H[i].Vec.V ^= P[i + 4].Vec.V ^ temp_u32;
        }
        else if (i > 7 && i < 28)
        {
            uint32_t temp_u32 = cus_random();
            I[i - 4].Vec.V ^= temp_u32;
            H[i].Vec.V ^= P[i + 4].Vec.V ^ temp_u32;
        }
        else if (i > 27)
        {
            uint32_t temp_u32 = cus_random();
            C[i - 24].Vec.V ^= temp_u32;
            H[i].Vec.V ^= P[i + 4].Vec.V ^ temp_u32;
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

    //M
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 256; j++)
        {
            randV8(&tempV8);
            if (i < 4)
            {
                M[i][j] = affineU8(R_inv[i], tempV8.V);
            }
            else
            {
                M[i][j] = tempV8.V;
            }  
        }
    }
    
    //Phi
    for (i = 0; i < 8; i++)
    {
        uint8_t Phi_inv[256];
        for (j = 0; j < 256; j++)
        {
            generatePermutation(Phi[i], Phi_inv);
        }
    }                   

    //Table_1
    for (i = 0; i < 4; i++)
    {
        for (x = 0; x < 256; x++)
        {
            temp8 = affineU8(E_ij[0][i], x);
            temp8 = SBOX[temp8 ^ ((ctx.sk[0] >> (24 - 8 * i)) & 0xff)];
            switch (i) 
            {
            case 0:temp8 ^= affineU8(R[0], M[0][x]); break;//
            case 1:temp8 ^= affineU8(R[0], M[0][Phi[0][x]]); break;//
            case 2:temp8 ^= affineU8(R[0], M[0][Phi[0][Phi[0][x]]]); break;//
            case 3:temp8 ^= affineU8(R[0], M[0][Phi[0][Phi[0][Phi[0][x]]]]); break;//
            default:break;
            }
            temp8 = affineU8(E_1_ij_inv[0][i], temp8);
            Table_1[0][i][x] = temp8;                
        }
    }
    for (i = 0; i < 4; i++)
    {
        for (x = 0; x < 256; x++)
        {
            temp8 = affineU8(E_ij[28][i], x);
            temp8 = SBOX[temp8 ^ ((ctx.sk[28] >> (24 - 8 * i)) & 0xff)];
            switch (i)
            {
            case 0:temp8 ^= M[4][x]; break;//
            case 1:temp8 ^= M[4][Phi[4][x]]; break;//
            case 2:temp8 ^= M[4][Phi[4][Phi[4][x]]]; break;//
            case 3:temp8 ^= M[4][Phi[4][Phi[4][Phi[4][x]]]]; break;//
            default:break;
            }
            temp8 = affineU8(E_1_ij_inv[4][i], temp8);
            Table_1[1][i][x] = temp8;
        }
    }

    //Table_2
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 256; x++)
            {                
                for (y = 0; y < 256; y++)
                {
                    temp8 = affineU8(E_ij[i + 1][j], x);
                    temp_2 = MatMulNumM8(Nj[j], y);
                    temp8 = SBOX[temp8 ^ temp_2 ^ ((ctx.sk[i + 1] >> (24 - 8 * j)) & 0xff)];
                    switch (j)
                    {
                    case 0:temp8 ^= affineU8(R[i + 1], M[i + 1][x]); break;//
                    case 1:temp8 ^= affineU8(R[i + 1], M[i + 1][Phi[i + 1][x]]); break;//
                    case 2:temp8 ^= affineU8(R[i + 1], M[i + 1][Phi[i + 1][Phi[i + 1][x]]]); break;//
                    case 3:temp8 ^= affineU8(R[i + 1], M[i + 1][Phi[i + 1][Phi[i + 1][Phi[i + 1][x]]]]); break;//
                    default:break;
                    }
                    temp8 = affineU8(E_1_ij_inv[i + 1][j], temp8);
                    Table_2[i][j][x][y] = temp8;
                }
            }
        }
    }
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 256; x++)
            {
                for (y = 0; y < 256; y++)
                {
                    temp8 = affineU8(E_ij[i + 29][j], x);
                    temp_2 = y;
                    temp8 = SBOX[temp8 ^ temp_2 ^ ((ctx.sk[i + 29] >> (24 - 8 * j)) & 0xff)];
                    switch (j)
                    {
                    case 0:temp8 ^= M[i + 5][x]; break;//
                    case 1:temp8 ^= M[i + 5][Phi[i + 5][x]]; break;//
                    case 2:temp8 ^= M[i + 5][Phi[i + 5][Phi[i + 5][x]]]; break;//
                    case 3:temp8 ^= M[i + 5][Phi[i + 5][Phi[i + 5][Phi[i + 5][x]]]]; break;//
                    default:break;
                    }
                    temp8 = affineU8(E_1_ij_inv[i + 5][j], temp8);
                    Table_2[i + 3][j][x][y] = temp8;
                }
            }
        }
    }
    
    //Table_3
    for (i = 0; i < 24; i++)
    {
        //combine QL
        M32 QL;
        MatMulMatM32(Q[i].Mat, L_matrix, &QL);

        uint32_t Q_constant[3] = { 0 };
        for (j = 0; j < 3; j++)
        {
            Q_constant[j] = cus_random();
        }

        for (x = 0; x < 256; x++)
        {
            for (j = 0; j < 4; j++)
            {
                uint8_t temp_u8 = affineU8(E_ij[i + 4][j], x);
                temp_u8 = SBOX[temp_u8 ^ ((ctx.sk[i + 4] >> (24 - j * 8)) & 0xff)];
                uint32_t temp_32 = temp_u8 << (24 - j * 8);
                Table_3[i][j][x] = MatMulNumM32(QL, temp_32);
            }
            for (j = 0; j < 3; j++)
            {
                Table_3[i][j][x] ^= Q_constant[j];
            }
            Table_3[i][3][x] ^= Q[i].Vec.V ^ Q_constant[0] ^ Q_constant[1] ^ Q_constant[2];
        }        
    }
}

void wbsm4_encrypt(unsigned char IN[], unsigned char OUT[], unsigned char share[])
{
    int i;
    uint32_t x0, x1, x2, x3, x4, x4_1;
    uint32_t mask[8] = { 0 };
    uint32_t temp;
    uint32_t xt0, xt1, xt2, xt3;

    x0 = GET32(IN);
    x1 = GET32(IN + 4);
    x2 = GET32(IN + 8);
    x3 = GET32(IN + 12);
    
    //round 0-3,test right
    for (i = 0; i < 4; i++)
    {
        xt1 = affineU32(G[i][0], x1);
        xt2 = affineU32(G[i][1], x2);
        xt3 = affineU32(G[i][2], x3);
        x4 = xt1 ^ xt2 ^ xt3;

        switch (i)
        {
        case 0:temp = 0; break;
        case 1:temp = mask[0]; break;
        case 2:temp = mask[0] ^ mask[1]; break;
        case 3:temp = mask[0] ^ mask[1] ^ mask[2]; break;
        }//part 1

        if (i == 0)
        {
            x4_1 = ((Table_1[0][0][x4 >> 24 & 0xff]) << 24) | ((Table_1[0][1][x4 >> 16 & 0xff]) << 16) | ((Table_1[0][2][x4 >> 8 & 0xff]) << 8) | (Table_1[0][3][x4 & 0xff]);
        }
        else
        {
            x4_1 = (Table_2[i - 1][0][x4 >> 24 & 0xff][temp >> 24 & 0xff] << 24) | (Table_2[i - 1][1][x4 >> 16 & 0xff][temp >> 16 & 0xff] << 16) | (Table_2[i - 1][2][x4 >> 8 & 0xff][temp >> 8 & 0xff] << 8) | (Table_2[i - 1][3][x4 & 0xff][temp & 0xff]);
        }
        temp = (M[i][x4 >> 24 & 0xff] << 24) | (M[i][Phi[i][x4 >> 16 & 0xff]] << 16) | (M[i][Phi[i][Phi[i][x4 >> 8 & 0xff]]] << 8) | M[i][Phi[i][Phi[i][Phi[i][x4 & 0xff]]]];
        x4_1 = affineU32(C[i], x4_1);
        mask[i] = affineU32(D[i], temp);//part 2

        xt0 = affineU32(H[i], x0);
        x4 = xt0 ^ x4_1;//part3
        
        x0 = x1;
        x1 = x2;
        x2 = x3;
        x3 = x4;
    }

    //round 4-27,test right
    for (i = 0; i < 24; i++)
    {
        xt1 = affineU32(G[i + 4][0], x1);
        xt2 = affineU32(G[i + 4][1], x2);
        xt3 = affineU32(G[i + 4][2], x3);
        x4 = xt1 ^ xt2 ^ xt3;

        switch (i)
        {
        case 0:temp = mask[1] ^ mask[2] ^ mask[3]; break;
        case 1:temp = mask[2] ^ mask[3]; break;
        case 2:temp = mask[3]; break;
        default :temp = 0; break;
        }
        if (i < 3)
        {
            temp = affineU32(J[i], temp);
        }
        x4 = x4 ^ temp;//part 1

        x4 = Table_3[i][0][(x4 >> 24) & 0xff] ^ Table_3[i][1][(x4 >> 16) & 0xff] ^ Table_3[i][2][(x4 >> 8) & 0xff] ^ Table_3[i][3][x4 & 0xff];  

        xt0 = affineU32(H[i + 4], x0);
        if (i < 4)
        {
            temp = affineU32(J[i + 4], mask[i]);
            xt0 ^= temp;
        }
        x4 = affineU32(I[i], x4); 
        x4 = xt0 ^ x4;//part3

        x0 = x1;
        x1 = x2;
        x2 = x3;
        x3 = x4;
    }

    //round 28-31,test right
    for (i = 0; i < 4; i++)
    {
        xt1 = affineU32(G[i + 28][0], x1);
        xt2 = affineU32(G[i + 28][1], x2);
        xt3 = affineU32(G[i + 28][2], x3);
        x4 = xt1 ^ xt2 ^ xt3;

        switch (i)
        {
        case 0:temp = 0; break;
        case 1:temp = mask[4]; break;
        case 2:temp = mask[4] ^ mask[5]; break;
        case 3:temp = mask[4] ^ mask[5] ^ mask[6]; break;
        }//part 1

        if (i == 0)
        {
            x4_1 = ((Table_1[1][0][x4 >> 24 & 0xff]) << 24) | ((Table_1[1][1][x4 >> 16 & 0xff]) << 16) | ((Table_1[1][2][x4 >> 8 & 0xff]) << 8) | (Table_1[1][3][x4 & 0xff]);
        }
        else
        {
            x4_1 = (Table_2[i + 2][0][x4 >> 24 & 0xff][temp >> 24 & 0xff] << 24) | (Table_2[i + 2][1][x4 >> 16 & 0xff][temp >> 16 & 0xff] << 16) | (Table_2[i + 2][2][x4 >> 8 & 0xff][temp >> 8 & 0xff] << 8) | (Table_2[i + 2][3][x4 & 0xff][temp & 0xff]);
        }
        temp = (M[i + 4][x4 >> 24 & 0xff] << 24) | (M[i + 4][Phi[i + 4][x4 >> 16 & 0xff]] << 16) | (M[i + 4][Phi[i + 4][Phi[i + 4][x4 >> 8 & 0xff]]] << 8) | M[i + 4][Phi[i + 4][Phi[i + 4][Phi[i + 4][x4 & 0xff]]]];
        x4_1 = affineU32(C[i + 4], x4_1);
        mask[i + 4] = MatMulNumM32(L_matrix, temp);//part 2

        xt0 = affineU32(H[i + 28], x0);
        x4 = xt0 ^ x4_1;//part3      

        x0 = x1;
        x1 = x2;
        x2 = x3;
        x3 = x4;
    }

    PUT32(x3, OUT);
    PUT32(x2, OUT + 4);
    PUT32(x1, OUT + 8);
    PUT32(x0, OUT + 12);

    PUT32(mask[7], share);
    PUT32(mask[6], share + 4);
    PUT32(mask[5], share + 8);
    PUT32(mask[4], share + 12);
}