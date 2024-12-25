#include "wbsm4.h"

int main()
{
    //unsigned char IN[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    //unsigned char IN[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    //unsigned char IN[16] = {0x72,0xEB,0xA3,0x03,0x99,0x47,0xE1,0x70,0x92,0xE9,0x22,0xD7,0xCD,0xA3,0x8E,0xA0};
    //unsigned char IN[16] ={0x9F,0x1F,0x7B,0xFF,0x6F,0x55,0x11,0x38,0x4D,0x94,0x30,0x53,0x1E,0x53,0x8F,0xD3};
    unsigned char IN[16] = {0xA2,0xD8,0xC3,0xD2,0x9E,0xE4,0xE2,0xE4,0x9F,0xB8,0x89,0x74,0xAD,0x5E,0xE9,0x45};
    unsigned char key[16] = { 0 };
    //unsigned char key[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    unsigned char EX_IN[16];
    unsigned char EX_OUT[16];
    unsigned char OUT[16];
    unsigned char OOUT[16];
    uint32_t x0, x1, x2, x3;
    //create
    gen_table();
    //wbkey_gen(key);//dwbsm4 encryption
      wbdkey_gen(key);//dwbsm4 decryption
    FILE *fp;
    fp = fopen("C:/example.txt", "w");
    fprintf(fp,"uint32_t Mask[32]={");
    for(int i=0;i<32;i++){
        fprintf(fp,"%lld",Mask[i]);
        if(i!=31)fprintf(fp,",");
    }
    fprintf(fp,"};\n");
    fprintf(fp,"uint32_t re_mask[4]={");
        for(int i=0;i<4;i++){
            fprintf(fp,"%lld",re_mask[i]);
            if(i!=3)fprintf(fp,",");
        }
    fprintf(fp,"};\n");
        fprintf(fp,"Aff32 SE[4]={");
        for(int i=0;i<4;i++)
        {
            fprintf(fp,"{");
                fprintf(fp,"{");
                    for(int j=0;j<32;j++)
                    {
                        fprintf(fp,"%lld",SE[i].Mat.M[j]);
                        if(j!=31)fprintf(fp,",");
                    }
                fprintf(fp,"}");
                fprintf(fp,",");
                fprintf(fp,"%lld",SE[i].Vec.V);
            fprintf(fp,"}");
            if(i!=3)fprintf(fp,",");

        }
        fprintf(fp,"};\n");

    fprintf(fp,"Aff32 FE[4]={");;
    for(int i=0;i<4;i++)
    {
        fprintf(fp,"{");
        fprintf(fp,"{");
        for(int j=0;j<32;j++)
        {
            fprintf(fp,"%lld",FE[i].Mat.M[j]);
            if(j!=31)fprintf(fp,",");
        }
        fprintf(fp,"}");
        fprintf(fp,",");
        fprintf(fp,"%lld",FE[i].Vec.V);
        fprintf(fp,"}");
        if(i!=3)fprintf(fp,",");

    }
    fprintf(fp,"};\n");

    fprintf(fp,"Aff8 Eij[36][4]={");
        for(int i=0;i<36;i++)
        {
            fprintf(fp,"{");
                for(int j=0;j<4;j++)
                {
                    fprintf(fp,"{");
                        fprintf(fp,"{");
                            for(int k=0;k<8;k++)
                            {
                                fprintf(fp,"%lld",Eij[i][j].Mat.M[k]);
                                if(k!=7)fprintf(fp,",");
                            }
                        fprintf(fp,"}");
                    fprintf(fp,",");
                        fprintf(fp,"%lld", Eij[i][j].Vec.V);
                    fprintf(fp,"}");
                    if(j!=3)fprintf(fp,",");
                }
            fprintf(fp,"}");
            if(i!=35)fprintf(fp,",");
        }
    fprintf(fp,"};\n");


    fprintf(fp,"Aff8 Eij_inv[36][4]={");
    for(int i=0;i<36;i++)
    {
        fprintf(fp,"{");
        for(int j=0;j<4;j++)
        {
            fprintf(fp,"{");
            fprintf(fp,"{");
            for(int k=0;k<8;k++)
            {
                fprintf(fp,"%lld",Eij_inv[i][j].Mat.M[k]);
                if(k!=7)fprintf(fp,",");
            }
            fprintf(fp,"}");
            fprintf(fp,",");
            fprintf(fp,"%lld", Eij_inv[i][j].Vec.V);
            fprintf(fp,"}");
            if(j!=3)fprintf(fp,",");
        }
        fprintf(fp,"}");
        if(i!=35)fprintf(fp,",");
    }
    fprintf(fp,"};\n");

    fprintf(fp,"Aff32 Ei_inv[36]={");
        for(int i=0;i<36;i++)
        {
            fprintf(fp,"{");
                fprintf(fp,"{");
                    for(int j=0;j<32;j++)
                    {
                        fprintf(fp,"%lld",Ei_inv[i].Mat.M[j]);
                        if(j!=31)fprintf(fp,",");
                    }
                fprintf(fp,"}");
            fprintf(fp,",");
            fprintf(fp,"%lld",Ei_inv[i].Vec.V);
            fprintf(fp,"}");
           if(i!=35)fprintf(fp,",");
        }
    fprintf(fp,"};\n");

    fprintf(fp,"Aff8 Aij[32][4]={");
        for(int i=0;i<32;i++){
            fprintf(fp,"{");
                for(int j=0;j<4;j++)
                {
                    fprintf(fp,"{");
                    fprintf(fp,"{");
                        for(int k=0;k<8;k++)
                        {
                            fprintf(fp,"%lld",Aij[i][j].Mat.M[k]);
                            if(k!=7)fprintf(fp,",");
                        }
                    fprintf(fp,"}");
                    fprintf(fp,",");
                    fprintf(fp,"%lld",Aij[i][j].Vec.V);
                    fprintf(fp,"}");
                    if(j!=3)fprintf(fp,",");
                }
            fprintf(fp,"}");
            if(i!=31)fprintf(fp,",");
        }
    fprintf(fp,"};\n");

    fprintf(fp,"Aff8 Aij_inv[32][4]={");
    for(int i=0;i<32;i++){
        fprintf(fp,"{");
        for(int j=0;j<4;j++)
        {
            fprintf(fp,"{");
            fprintf(fp,"{");
            for(int k=0;k<8;k++)
            {
                fprintf(fp,"%lld",Aij_inv[i][j].Mat.M[k]);
                if(k!=7)fprintf(fp,",");
            }
            fprintf(fp,"}");
            fprintf(fp,",");
            fprintf(fp,"%lld",Aij_inv[i][j].Vec.V);
            fprintf(fp,"}");
            if(j!=3)fprintf(fp,",");
        }
        fprintf(fp,"}");
        if(i!=31)fprintf(fp,",");
    }
    fprintf(fp,"};\n");
    fprintf(fp,"Aff32 Q[36]={");
        for(int i=0;i<36;i++){
            fprintf(fp,"{");
                fprintf(fp,"{");
                    for(int j=0;j<32;j++){
                        fprintf(fp,"%lld",Q[i].Mat.M[j]);
                        if(j!=31)fprintf(fp,",");
                    }
                fprintf(fp,"}");
            fprintf(fp,",");
            fprintf(fp,"%lld",Q[i].Vec);
            fprintf(fp,"}");
            if(i!=35)fprintf(fp,",");
        }
    fprintf(fp,"};\n");

    fprintf(fp,"Aff32  Q_inv[36]={");
    for(int i=0;i<36;i++){
        fprintf(fp,"{");
        fprintf(fp,"{");
        for(int j=0;j<32;j++){
            fprintf(fp,"%lld", Q_inv[i].Mat.M[j]);
            if(j!=31)fprintf(fp,",");
        }
        fprintf(fp,"}");
        fprintf(fp,",");
        fprintf(fp,"%lld", Q_inv[i].Vec);
        fprintf(fp,"}");
        if(i!=35)fprintf(fp,",");
    }
    fprintf(fp,"};\n");
    fprintf(fp,"Aff32 M[32][3]={");
        for(int i=0;i<32;i++)
        {
            fprintf(fp,"{");
                for(int j=0;j<3;j++){
                    fprintf(fp,"{");
                    fprintf(fp,"{");
                        for(int k=0;k<32;k++)
                        {
                            fprintf(fp,"%lld",M[i][j].Mat.M[k]);
                            if(k!=31)fprintf(fp,",");
                        }
                    fprintf(fp,"}");
                        fprintf(fp,",");
                    fprintf(fp,"%lld",M[i][j].Vec.V);
                    fprintf(fp,"}");
                    if(j!=2)fprintf(fp,",");
                }
            fprintf(fp,"}");
            if(i!=31)fprintf(fp,",");
        }
    fprintf(fp,"};\n");


    //uint8_t Out[32][4][256];
    //uint8_t In[32][4][256];


    fprintf(fp,"Aff32 C[32]={");
        for(int i=0;i<32;i++)
        {
            fprintf(fp,"{");
            fprintf(fp,"{");
                for(int j=0;j<32;j++)
                {
                    fprintf(fp,"%lld",C[i].Mat.M[j]);
                    if(j!=31)fprintf(fp,",");
                }
            fprintf(fp,"}");
            fprintf(fp,",");
            fprintf(fp,"%lld",C[i].Vec.V);
            fprintf(fp,"}");
            if(i!=31)fprintf(fp,",");
        }
    fprintf(fp,"};\n");

    fprintf(fp,"uint8_t Out[32][4][256]={");
        for(int i=0;i<32;i++){
            fprintf(fp,"{");
                for(int j=0;j<4;j++)
                {
                    fprintf(fp,"{");
                        for(int k=0;k<256;k++)
                        {
                            fprintf(fp,"%lld",Out[i][j][k]);
                            if(k!=255)fprintf(fp,",");
                        }
                    fprintf(fp,"}");
                    if(j!=3)fprintf(fp,",");
                }
            fprintf(fp,"}");
            if(i!=31)fprintf(fp,",");
        }
    fprintf(fp,"};\n");

    fprintf(fp,"uint8_t In[32][4][256]={");
    for(int i=0;i<32;i++){
        fprintf(fp,"{");
        for(int j=0;j<4;j++)
        {
            fprintf(fp,"{");
            for(int k=0;k<256;k++)
            {
                fprintf(fp,"%lld", In[i][j][k]);
                if(k!=255)fprintf(fp,",");
            }
            fprintf(fp,"}");
            if(j!=3)fprintf(fp,",");
        }
        fprintf(fp,"}");
        if(i!=31)fprintf(fp,",");
    }
    fprintf(fp,"};\n");
    fprintf(fp,"uint_t wbkey[32]={");
        for(int i=0;i<32;i++)
        {fprintf(fp,"%lld",wbkey[i]);
        if(i!=31)fprintf(fp,",");};

    fprintf(fp,"};\n");
    fprintf(fp,"uint8_t TypeI[32][4][256][256]={");
    for(int i=0;i<32;i++)
    {
        fprintf(fp,"{");
            for(int j=0;j<4;j++){
                fprintf(fp,"{");
                for(int k=0;k<256;k++)
                {
                    fprintf(fp,"{");
                    for(int m=0;m<256;m++)
                    {
                        fprintf(fp,"%lld",TypeI[i][j][k][m]);
                        if(m!=255)fprintf(fp,",");
                    }
                    fprintf(fp,"}");
                    if(k!=255)fprintf(fp,",");
                }
                fprintf(fp,"}");
                if(j!=4)fprintf(fp,",");
            }
        fprintf(fp,"}");
        if(i!=31)fprintf(fp,",");
    }
    fprintf(fp,"};\n");
    fprintf(fp,"uint32_t TypeII[32][4][256]={");
    for(int i=0;i<32;i++)
    {
        fprintf(fp,"{");
        for(int j=0;j<4;j++){
            fprintf(fp,"{");
                for(int m=0;m<256;m++)
                {
                    fprintf(fp,"%lld",TypeI[i][j][m]);
                    if(m!=255)fprintf(fp,",");
                }
            fprintf(fp,"}");
            if(j!=4)fprintf(fp,",");
        }
        fprintf(fp,"}");
        if(i!=31)fprintf(fp,",");
    }
    fprintf(fp,"};\n");

//    uint8_t TypeI[32][4][256][256];
//    uint32_t TypeII[32][4][256];
////wbkey
//    uint_t T_wbkey[32];


    printf("key:");
    printstate(key);
    printf("plaintext:");
    printstate(IN);

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

////standard aes input
    sm4_context ctx;
    //sm4_setkey_enc(&ctx, key);//standard enc
    sm4_setkey_dec(&ctx,key);   //standard dec
    sm4_crypt_ecb(&ctx, 1, 16, IN, OOUT);
    printf("Standard sm4 decryption results:");
    printstate(OOUT);
////standard aes output
    // encryption

    encrypt(EX_IN, EX_OUT);

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
    printf("White-box decryption results:");
    printstate(OUT);




    return 0;
}