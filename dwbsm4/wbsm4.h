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

//mask
uint32_t Mask[32];
uint32_t re_mask[4];

//external code
Aff32 SE[4];
Aff32 FE[4];

//affine code
Aff8 Eij[36][4];
Aff8 Eij_inv[36][4];
Aff32 Ei_inv[36];
Aff8 Aij[32][4];
Aff8 Aij_inv[32][4];
Aff32 Q[36];
Aff32 Q_inv[36];
Aff32 M[32][3];
Aff32 C[32];

//non-linear code
uint8_t Out[32][4][256];
uint8_t In[32][4][256];

//table
uint8_t TypeI[32][4][256][256];
uint32_t TypeII[32][4][256];

//wbkey
uint32_t wbkey[32];

void printstate(unsigned char* in);
void non_linear8(uint8_t Out[32][4][256], uint8_t In[32][4][256]);
void split32to8_mat(Aff32 MB, Aff8 MB_ij[4][4]);
void gen_table();
void wbkey_gen(uint8_t* key);
void wbdkey_gen(uint8_t* key);
void encrypt(unsigned char IN[], unsigned char OUT[]);