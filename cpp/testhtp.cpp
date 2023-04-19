/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* test driver and function exerciser for HTP API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "core.h"
#include "randapi.h"
#include "ecp_NIST256.h"
#include "ecp_Ed25519.h"
#include "ecp_C25519.h"
#include "ecp_Ed448.h"
#include "ecp_SECP256K1.h"
#include "ecp_BLS12381.h"
#include "ecp2_BLS12381.h"

using namespace core;

#define CEIL(a,b) (((a)-1)/(b)+1)

/* Select curves 1,2,3,7,17,29 */

/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */

static void hash_to_field_Ed25519(int hash,int hlen,F25519::FP *u,octet *DST,octet *M, int ctr)
{
    using namespace Ed25519;
    using namespace Ed25519_BIG;
    using namespace Ed25519_FP;
    
    int i,j,L,k,m;
    BIG q,w,r;
    DBIG dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_rcopy(q, Modulus);
    k=BIG_nbits(q);
    BIG_rcopy(r, CURVE_Order);
    m=BIG_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_dmod(w,dx,q);
        FP_nres(&u[i],w);
    }
}

int htp_Ed25519(char *mess)
{
    using namespace Ed25519;
    using namespace Ed25519_BIG;
    using namespace Ed25519_FP;
 
    int res=0;
    BIG r;
    FP u[2];
    ECP P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    BIG_rcopy(r, CURVE_Order);
    printf("Random oracle - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_");

    hash_to_field_Ed25519(MC_SHA2, SHA512,u,&DST,&MSG,2);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_output(&u[1]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_output(&P);
    ECP_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_output(&P1);
    ECP_add(&P,&P1);
    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_");

    hash_to_field_Ed25519(MC_SHA2, SHA512,u,&DST,&MSG,1);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q= "); ECP_output(&P);

    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");

    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");
    
    return res;
}


static void hash_to_field_C25519(int hash,int hlen,F25519::FP *u,octet *DST,octet *M, int ctr)
{
    using namespace C25519;
    using namespace C25519_BIG;
    using namespace C25519_FP;
    
    int i,j,L,k,m;
    BIG q,w,r;
    DBIG dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_rcopy(q, Modulus);
    k=BIG_nbits(q);
    BIG_rcopy(r, CURVE_Order);
    m=BIG_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_dmod(w,dx,q);
        FP_nres(&u[i],w);
    }
}

int htp_C25519(char *mess)
{
    using namespace C25519;
    using namespace C25519_BIG;
    using namespace C25519_FP;
 
    int res=0;
    BIG r;
    FP u[2];
    ECP P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};
    
    OCT_jstring(&MSG,mess);

    printf("Non-Uniform\n");
    OCT_clear(&DST);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-curve25519_XMD:SHA-512_ELL2_NU_");

    hash_to_field_C25519(MC_SHA2, SHA512,u,&DST,&MSG,1);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q= "); ECP_output(&P);

    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");

    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");
    
    return res;
}

/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_NIST256(int hash,int hlen,NIST256::FP *u,octet *DST,octet *M, int ctr)
{
    using namespace NIST256;
    using namespace NIST256_BIG;
    using namespace NIST256_FP;
    
    int i,j,L,k,m;
    BIG q,w,r;
    DBIG dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_rcopy(q, Modulus);
    k=BIG_nbits(q);
    BIG_rcopy(r, CURVE_Order);
    m=BIG_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_dmod(w,dx,q);
        FP_nres(&u[i],w);
    }
}

int htp_NIST256(char *mess)
{
    using namespace NIST256;
    using namespace NIST256_BIG;
    using namespace NIST256_FP;
 
    int res=0;
    BIG r;
    FP u[2];
    ECP P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    BIG_rcopy(r, CURVE_Order);
    printf("Random oracle - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_RO_");

    hash_to_field_NIST256(MC_SHA2,HASH_TYPE_NIST256,u,&DST,&MSG,2);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_output(&u[1]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_output(&P);
    ECP_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_output(&P1);
    ECP_add(&P,&P1);
    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_NU_");
    
    hash_to_field_NIST256(MC_SHA2,HASH_TYPE_NIST256,u,&DST,&MSG,1);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q= "); ECP_output(&P);
    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");
    return res;
}


/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_Ed448(int hash,int hlen,Ed448::FP *u,octet *DST,octet *M, int ctr)
{
    using namespace Ed448;
    using namespace Ed448_BIG;
    using namespace Ed448_FP;
    
    int i,j,L,k,m;
    BIG q,w,r;
    DBIG dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_rcopy(q, Modulus);
    k=BIG_nbits(q);
    BIG_rcopy(r, CURVE_Order);
    m=BIG_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_dmod(w,dx,q);
        FP_nres(&u[i],w);
    }
}

int htp_Ed448(char *mess)
{
    using namespace Ed448;
    using namespace Ed448_BIG;
    using namespace Ed448_FP;
 
    int res=0;
    BIG r;
    FP u[2];
    ECP P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    BIG_rcopy(r, CURVE_Order);
    printf("Random oracle - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_RO_");

    hash_to_field_Ed448(MC_SHA2,HASH_TYPE_Ed448,u,&DST,&MSG,2);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_output(&u[1]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_output(&P);
    ECP_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_output(&P1);
    ECP_add(&P,&P1);
    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_NU_");

    hash_to_field_Ed448(MC_SHA2,HASH_TYPE_Ed448,u,&DST,&MSG,1);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q= "); ECP_output(&P);

    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");

    return res;
}

/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_SECP256K1(int hash,int hlen,SECP256K1::FP *u,octet *DST,octet *M, int ctr)
{
    using namespace SECP256K1;
    using namespace SECP256K1_BIG;
    using namespace SECP256K1_FP;
    
    int i,j,L,k,m;
    BIG q,w,r;
    DBIG dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_rcopy(q, Modulus);
    k=BIG_nbits(q);
    BIG_rcopy(r, CURVE_Order);
    m=BIG_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_dmod(w,dx,q);
        FP_nres(&u[i],w);
    }
}

int htp_SECP256K1(char *mess)
{
    using namespace SECP256K1;
    using namespace SECP256K1_BIG;
    using namespace SECP256K1_FP;
 
    int res=0;
    BIG r;
    FP u[2];
    ECP P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    BIG_rcopy(r, CURVE_Order);
    printf("Random oracle - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_RO_");
    hash_to_field_SECP256K1(MC_SHA2,HASH_TYPE_SECP256K1,u,&DST,&MSG,2);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_output(&u[1]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_output(&P);
    ECP_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_output(&P1);
    ECP_add(&P,&P1);
    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_NU_");
    hash_to_field_SECP256K1(MC_SHA2,HASH_TYPE_SECP256K1,u,&DST,&MSG,1);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q= "); ECP_output(&P);

    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");

    return res;
}


/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_BLS12381(int hash,int hlen,BLS12381::FP *u,octet *DST,octet *M, int ctr)
{
    using namespace BLS12381;
    using namespace BLS12381_BIG;
    using namespace BLS12381_FP;
    
    int i,j,L,k,m;
    BIG q,w,r;
    DBIG dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_rcopy(q, Modulus);
    k=BIG_nbits(q);
    BIG_rcopy(r, CURVE_Order);
    m=BIG_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    //printf("L= %d\n",L);
    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_dmod(w,dx,q);
        FP_nres(&u[i],w);
    }
}

int htp_BLS12381(char *mess)
{
    using namespace BLS12381;
    using namespace BLS12381_BIG;
    using namespace BLS12381_FP;
 
    int res=0;
    BIG r;
    FP u[2];
    ECP P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    BIG_rcopy(r, CURVE_Order);
    printf("Random oracle - message= %s\n",mess);
    OCT_jstring(&MSG,mess);


    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_RO_");
    hash_to_field_BLS12381(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,2);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_output(&u[1]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_output(&P);

//exit(0);

    ECP_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_output(&P1);
    ECP_add(&P,&P1);
    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_NU_");
    hash_to_field_BLS12381(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,1);
    printf("u[0]= "); FP_output(&u[0]); printf("\n");
    ECP_map2point(&P,&u[0]);
    printf("Q= "); ECP_output(&P);

    ECP_cfp(&P);
    ECP_affine(&P);
    printf("P= "); ECP_output(&P); printf("\n");
    ECP_mul(&P,r);
    if (!ECP_isinf(&P))
        printf("Hashing failure\n");

    return res;
}

/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_BLS12381_G2(int hash,int hlen,BLS12381::FP2 *u,octet *DST,octet *M, int ctr)
{
    using namespace BLS12381;
    using namespace BLS12381_BIG;
    
    int i,j,L,k,m;
    BIG q,w1,w2,r;
    DBIG dx;
    char okm[1024],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_rcopy(q, Modulus);
    k=BIG_nbits(q);
    BIG_rcopy(r, CURVE_Order);
    m=BIG_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    //printf("L= %d\n",L);
    XMD_Expand(hash,hlen,&OKM,2*L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[2*i*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_dmod(w1,dx,q);
        
        for (j=0;j<L;j++)
            fd[j]=OKM.val[(2*i+1)*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_dmod(w2,dx,q);

        FP2_from_BIGs(&u[i],w1,w2);
    }
}

int htp_BLS12381_G2(char *mess)
{
    using namespace BLS12381;
    using namespace BLS12381_BIG;

    int res=0;
    BIG r;
    FP2 u[2];
    ECP2 P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    BIG_rcopy(r, CURVE_Order);
    printf("Random oracle - message= %s\n",mess);
    OCT_jstring(&MSG,mess);


    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_RO_");
    hash_to_field_BLS12381_G2(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,2);
    printf("u[0]= "); FP2_output(&u[0]); printf("\n");
    printf("u[1]= "); FP2_output(&u[1]); printf("\n");
    ECP2_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP2_output(&P);
    ECP2_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP2_output(&P1);
    ECP2_add(&P,&P1);
    ECP2_cfp(&P);
    ECP2_affine(&P);
    printf("P= "); ECP2_output(&P); printf("\n");
    ECP2_mul(&P,r);
    if (!ECP2_isinf(&P))
        printf("Hashing failure\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_NU_");
    hash_to_field_BLS12381_G2(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,1);
    printf("u[0]= "); FP2_output(&u[0]); printf("\n");
    ECP2_map2point(&P,&u[0]);
    printf("Q= "); ECP2_output(&P);

    ECP2_cfp(&P);
    ECP2_affine(&P);
    printf("P= "); ECP2_output(&P); printf("\n");
    ECP2_mul(&P,r);
    if (!ECP2_isinf(&P))
        printf("Hashing failure\n");

    return res;
}

int main()
{
    int i, res;
    unsigned long ran;

    char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNG;                // Crypto Strong RNG

    time((time_t *)&ran);

    RAW.len = 100;              // fake random seed source
    RAW.val[0] = ran;
    RAW.val[1] = ran >> 8;
    RAW.val[2] = ran >> 16;
    RAW.val[3] = ran >> 24;
    for (i = 4; i < 100; i++) RAW.val[i] = i;

    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG

    printf("%d bit build", CHUNK);

    printf("\nTesting HTP for curve Ed25519\n");
    htp_Ed25519((char *)"");
    htp_Ed25519((char *)"abc");
    htp_Ed25519((char *)"abcdef0123456789");
    htp_Ed25519((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_Ed25519((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    printf("\nTesting HTP for curve C25519\n");
    htp_C25519((char *)"");
    htp_C25519((char *)"abc");
    htp_C25519((char *)"abcdef0123456789");
    htp_C25519((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_C25519((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");


    printf("\nTesting HTP for curve NIST256\n");
    htp_NIST256((char *)"");
    htp_NIST256((char *)"abc");
    htp_NIST256((char *)"abcdef0123456789");
    htp_NIST256((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_NIST256((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    printf("\nTesting HTP for curve Ed448\n");
    htp_Ed448((char *)"");
    htp_Ed448((char *)"abc");
    htp_Ed448((char *)"abcdef0123456789");
    htp_Ed448((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_Ed448((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    printf("\nTesting HTP for curve SECP256K1\n");
    htp_SECP256K1((char *)"");
    htp_SECP256K1((char *)"abc");
    htp_SECP256K1((char *)"abcdef0123456789");
    htp_SECP256K1((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_SECP256K1((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    printf("\nTesting HTP for curve BLS12381_G1\n");
    htp_BLS12381((char *)"");
    htp_BLS12381((char *)"abc");
    htp_BLS12381((char *)"abcdef0123456789");
    htp_BLS12381((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_BLS12381((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    printf("\nTesting HTP for curve BLS12381_G2\n");
    htp_BLS12381_G2((char *)"");
    htp_BLS12381_G2((char *)"abc");
    htp_BLS12381_G2((char *)"abcdef0123456789");
    htp_BLS12381_G2((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_BLS12381_G2((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");


    KILL_CSPRNG(&RNG);
}


