#include "WBMatrix.h"
#include "sm4.h"

#define GET32(pc)  (\
((uint32_t)(pc)[0] << 24) ^\
((uint32_t)(pc)[1] << 16) ^\
((uint32_t)(pc)[2] <<  8) ^\
((uint32_t)(pc)[3]))

#define PUT32(st, ct)\
(ct)[0] = (uint8_t)((st) >> 24);\
(ct)[1] = (uint8_t)((st) >> 16);\
(ct)[2] = (uint8_t)((st) >>  8);\
(ct)[3] = (uint8_t)(st)

#define  SHL(x,n) (((x) & 0xFFFFFFFF) << n)
#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - n)))

Aff32 G[32][3]; //Xi+k
Aff32 H[32]; //Xi
Aff32 I[24]; //part 2
Aff32 C[8]; //data after L
Aff32 D[4]; //mask after L
Aff32 J[8]; //0-3 part1,4-7 part3 mask
Aff32 SE[4];
Aff32 FE[4];
M32 N;//for higher security,each round N can be different,and there needs additional affine transformation in Part1

uint8_t Table_1[2][4][256];
uint8_t Table_2[6][4][256][256];
uint32_t Table_3[24][4][256];
uint8_t M[8][256];
uint8_t Phi[8][256];

void printstate(unsigned char * in);
void generatePermutation(uint8_t* permutation, uint8_t* inverse);
void wbsm4_gen(uint8_t *key);
void wbsm4_encrypt(unsigned char IN[], unsigned char OUT[], unsigned char share[]);