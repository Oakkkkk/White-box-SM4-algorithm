#include "wbsm4.h"
//#include <time.h>

int main()
{
    unsigned char In[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    unsigned char key[16] = {0};
    unsigned char EX_IN[16];
    unsigned char EX_OUT[16];
    unsigned char Out[16];
    unsigned char OOUT[16];
    unsigned char share[16];
    uint32_t x0, x1, x2, x3;
    uint32_t s0, s1, s2, s3;

    /*clock_t begin, end;
    double duration;
    begin = clock();
    end = clock();
    duration = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f\n", duration);*/

    wbsm4_gen(key);

    // external encodings
    x0 = GET32(In);
    x1 = GET32(In + 4);
    x2 = GET32(In + 8);
    x3 = GET32(In + 12);
    x0 = affineU32(SE[0], x0);
    x1 = affineU32(SE[1], x1);
    x2 = affineU32(SE[2], x2);
    x3 = affineU32(SE[3], x3);
    PUT32(x0, EX_IN);
    PUT32(x1, EX_IN + 4);
    PUT32(x2, EX_IN + 8);
    PUT32(x3, EX_IN + 12);
    // encryption
    //begin = clock();
    wbsm4_encrypt(EX_IN, EX_OUT, share);
    /*end = clock();
    duration = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f\n", duration);*/

    // external encodings
    x0 = GET32(EX_OUT);
    x1 = GET32(EX_OUT + 4);
    x2 = GET32(EX_OUT + 8);
    x3 = GET32(EX_OUT + 12);
    s0 = GET32(share);
    s1 = GET32(share + 4);
    s2 = GET32(share + 8);
    s3 = GET32(share + 12);
    x0 = affineU32(FE[0], x0);
    x1 = affineU32(FE[1], x1);
    x2 = affineU32(FE[2], x2);
    x3 = affineU32(FE[3], x3);
    PUT32(x0 ^ s0, Out);
    PUT32(x1 ^ s1, Out + 4);
    PUT32(x2 ^ s2, Out + 8);
    PUT32(x3 ^ s3, Out + 12);
    printstate(Out);
    
    //FILE* fp;
    //errno_t err;
    //if ((err = fopen_s(&fp, "table.txt", "w")) != 0)  
    //{
    //    printf("无法打开此文件\n");            
    //    exit(0);                               
    //}
    ////保存Aff32 M[3];
    //fprintf_s(fp, "%s", "static Aff32 M[3] =\n{\n\t");
    //for (int i = 0; i < 3; i++)
    //{
    //    fprintf_s(fp, "{");
    //    for (int j = 0; j < 32; j++)
    //    {
    //        fprintf_s(fp, "0x%.8x,", M[0][i].Mat.M[j]);
    //    }
    //    fprintf_s(fp, "0x%.8x},\n\t\t", M[0][i].Vec.V);
    //}
    //fprintf(fp, "};\n");
    ////Aff32 C;
    //fprintf_s(fp, "%s", "static Aff32 C =\n{\n\t");
    //for (int i = 0; i < 32; i++)
    //{
    //    fprintf_s(fp, "0x%.8x,", C[0].Mat.M[i]);
    //}
    //fprintf_s(fp, "0x%.8x};\n", C[0].Vec.V);
    ////Aff32 D;
    //fprintf_s(fp, "%s", "static Aff32 D =\n{\n\t");
    //for (int i = 0; i < 32; i++)
    //{
    //    fprintf_s(fp, "0x%.8x,", D[0].Mat.M[i]);   
    //}
    //fprintf_s(fp, "0x%.8x};\n", D[0].Vec.V);
    ////Aff32 D2;
    //fprintf_s(fp, "%s", "static Aff32 D2 =\n{\n\t");
    //for (int i = 0; i < 32; i++)
    //{
    //    fprintf_s(fp, "0x%.8x,", D2[0].Mat.M[i]);    
    //}
    //fprintf_s(fp, "0x%.8x};\n", D2[0].Vec.V);
    ////Aff32 SE[4];
    //fprintf_s(fp, "%s", "static Aff32 SE[4] =\n{\n\t");
    //for (int r = 0; r < 4; r++)
    //{
    //    fprintf_s(fp, "{");
    //    for (int i = 0; i < 32; i++)
    //    {
    //        fprintf_s(fp, "0x%.8x,", SE[r].Mat.M[i]);
    //    }
    //    fprintf_s(fp, "0x%.8x", SE[r].Vec.V);
    //    fprintf(fp, "}%s\n", (r != 3) ? ", " : "");
    //}
    //fprintf_s(fp, "};\n");
    ////uint8_t Table_1[4][256];
    //fprintf_s(fp, "%s", "static uint8_t Table_1[4][256] =\n{\n\t");
    //for (int r = 0; r < 4; r++)
    //{
    //    fprintf_s(fp, "{");
    //    for (int i = 0; i < 256; i++)
    //    {
    //        fprintf_s(fp, "0x%.2x,", Table_1[r][i]);
    //    }
    //    fprintf(fp, "}%s\n", (r != 3) ? ", " : "");
    //}
    //fprintf_s(fp, "};\n");
    ////uint8_t EMask[256];
    //fprintf_s(fp, "%s", "static uint8_t EMask[256] =\n{\n\t");
    //for (int i = 0; i < 256; i++)
    //{
    //    fprintf_s(fp, "0x%.2x,", EMask[i]);
    //}
    //fprintf_s(fp, "};\n");
    ////uint8_t RE[256];
    //fprintf_s(fp, "%s", "static uint8_t RE[256] =\n{\n\t");
    //for (int i = 0; i < 256; i++)
    //{
    //    fprintf_s(fp, "0x%.2x,", RE[i]);
    //}
    //fprintf_s(fp, "};\n");
   
    sm4_context ctx;
    sm4_setkey_enc(&ctx, key);
    sm4_crypt_ecb(&ctx, 1, 16, In, OOUT);
    printstate(OOUT);
    return 0;
}