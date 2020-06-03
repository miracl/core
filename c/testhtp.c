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
#include "ecp_ED25519.h"
#include "ecp_GOLDILOCKS.h"
#include "ecp_SECP256K1.h"
#include "ecp_BLS12381.h"
#include "ecp2_BLS12381.h"

#define RO

#define CEIL(a,b) (((a)-1)/(b)+1)

/* WORK IN PROGRESS - PLEASE IGNORE */
/* Select curves 1,3,7,17,29 */

/* IMPORTANT - this must be edited if curve is changed */

#if CHUNK==16

#define BIG_ED BIG_256_13
#define DBIG_ED DBIG_256_13
#define BIG_ED_rcopy BIG_256_13_rcopy
#define BIG_ED_nbits BIG_256_13_nbits
#define BIG_ED_dmod BIG_256_13_dmod
#define BIG_ED_dfromBytesLen BIG_256_13_dfromBytesLen

#endif

#if CHUNK==32

#define BIG_ED BIG_256_29
#define DBIG_ED DBIG_256_29
#define BIG_ED_rcopy BIG_256_29_rcopy
#define BIG_ED_nbits BIG_256_29_nbits
#define BIG_ED_dmod BIG_256_29_dmod
#define BIG_ED_dfromBytesLen BIG_256_29_dfromBytesLen

#define BIG_NT BIG_256_28
#define DBIG_NT DBIG_256_28
#define BIG_NT_rcopy BIG_256_28_rcopy
#define BIG_NT_nbits BIG_256_28_nbits
#define BIG_NT_dmod BIG_256_28_dmod
#define BIG_NT_dfromBytesLen BIG_256_28_dfromBytesLen

#define BIG_SP BIG_256_28
#define DBIG_SP DBIG_256_28
#define BIG_SP_rcopy BIG_256_28_rcopy
#define BIG_SP_nbits BIG_256_28_nbits
#define BIG_SP_dmod BIG_256_28_dmod
#define BIG_SP_dfromBytesLen BIG_256_28_dfromBytesLen

#define BIG_GL BIG_448_29
#define DBIG_GL DBIG_448_29
#define BIG_GL_rcopy BIG_448_29_rcopy
#define BIG_GL_nbits BIG_448_29_nbits
#define BIG_GL_dmod BIG_448_29_dmod
#define BIG_GL_dfromBytesLen BIG_448_29_dfromBytesLen

#define BIG_BLS12 BIG_384_29
#define DBIG_BLS12 DBIG_384_29
#define BIG_BLS12_rcopy BIG_384_29_rcopy
#define BIG_BLS12_nbits BIG_384_29_nbits
#define BIG_BLS12_dmod BIG_384_29_dmod
#define BIG_BLS12_dfromBytesLen BIG_384_29_dfromBytesLen

#endif

#if CHUNK==64

#define BIG_ED BIG_256_56
#define DBIG_ED DBIG_256_56
#define BIG_ED_rcopy BIG_256_56_rcopy
#define BIG_ED_nbits BIG_256_56_nbits
#define BIG_ED_dmod BIG_256_56_dmod
#define BIG_ED_dfromBytesLen BIG_256_56_dfromBytesLen

#define BIG_NT BIG_256_56
#define DBIG_NT DBIG_256_56
#define BIG_NT_rcopy BIG_256_56_rcopy
#define BIG_NT_nbits BIG_256_56_nbits
#define BIG_NT_dmod BIG_256_56_dmod
#define BIG_NT_dfromBytesLen BIG_256_56_dfromBytesLen

#define BIG_SP BIG_256_56
#define DBIG_SP DBIG_256_56
#define BIG_SP_rcopy BIG_256_56_rcopy
#define BIG_SP_nbits BIG_256_56_nbits
#define BIG_SP_dmod BIG_256_56_dmod
#define BIG_SP_dfromBytesLen BIG_256_56_dfromBytesLen

#define BIG_GL BIG_448_58
#define DBIG_GL DBIG_448_58
#define BIG_GL_rcopy BIG_448_58_rcopy
#define BIG_GL_nbits BIG_448_58_nbits
#define BIG_GL_dmod BIG_448_58_dmod
#define BIG_GL_dfromBytesLen BIG_448_58_dfromBytesLen

#define BIG_BLS12 BIG_384_58
#define DBIG_BLS12 DBIG_384_58
#define BIG_BLS12_rcopy BIG_384_58_rcopy
#define BIG_BLS12_dmod BIG_384_58_dmod
#define BIG_BLS12_nbits BIG_384_58_nbits
#define BIG_BLS12_dfromBytesLen BIG_384_58_dfromBytesLen

#endif


/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_ED25519(int hash,int hlen,FP_F25519 *u,octet *DST,octet *M, int ctr)
{
    int i,j,L,k,m;
    BIG_ED q,w,r;
    DBIG_ED dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_ED_rcopy(q, Modulus_F25519);
    k=BIG_ED_nbits(q);
    BIG_ED_rcopy(r, CURVE_Order_ED25519);
    m=BIG_ED_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_ED_dfromBytesLen(dx,fd,L);
        BIG_ED_dmod(w,dx,q);
        FP_F25519_nres(&u[i],w);
    }
}

int htp_ED25519(char *mess)
{
    int res=0;
    FP_F25519 u[2];
    ECP_ED25519 P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    printf("Random Access - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_");
    hash_to_field_ED25519(MC_SHA2,/*HASH_TYPE_ED25519*/ SHA512,u,&DST,&MSG,2);
    printf("u[0]= "); FP_F25519_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_F25519_output(&u[1]); printf("\n");
    ECP_ED25519_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_ED25519_output(&P);
    ECP_ED25519_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_ED25519_output(&P1);
    ECP_ED25519_add(&P,&P1);
    ECP_ED25519_cfp(&P);
    ECP_ED25519_affine(&P);
    printf("P= "); ECP_ED25519_output(&P); printf("\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_");
    hash_to_field_ED25519(MC_SHA2,/*HASH_TYPE_ED25519*/ SHA512,u,&DST,&MSG,1);
    printf("u[0]= "); FP_F25519_output(&u[0]); printf("\n");
    ECP_ED25519_map2point(&P,&u[0]);
    printf("Q= "); ECP_ED25519_output(&P);
    ECP_ED25519_cfp(&P);
    ECP_ED25519_affine(&P);
    printf("P= "); ECP_ED25519_output(&P); printf("\n");
    return res;
}

/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_NIST256(int hash,int hlen,FP_NIST256 *u,octet *DST,octet *M, int ctr)
{
    
    int i,j,L,k,m;
    BIG_NT q,w,r;
    DBIG_NT dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_NT_rcopy(q, Modulus_NIST256);
    k=BIG_NT_nbits(q);
    BIG_NT_rcopy(r, CURVE_Order_NIST256);
    m=BIG_NT_nbits(r);
    L=CEIL(k+CEIL(m,2),8);
    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_NT_dfromBytesLen(dx,fd,L);
        BIG_NT_dmod(w,dx,q);
        FP_NIST256_nres(&u[i],w);
    }
}

int htp_NIST256(char *mess)
{
 
    int res=0;
    FP_NIST256 u[2];
    ECP_NIST256 P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    printf("Random Access - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_RO_");
    hash_to_field_NIST256(MC_SHA2,HASH_TYPE_NIST256,u,&DST,&MSG,2);
    printf("u[0]= "); FP_NIST256_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_NIST256_output(&u[1]); printf("\n");
    ECP_NIST256_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_NIST256_output(&P);
    ECP_NIST256_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_NIST256_output(&P1);
    ECP_NIST256_add(&P,&P1);
    ECP_NIST256_cfp(&P);
    ECP_NIST256_affine(&P);
    printf("P= "); ECP_NIST256_output(&P); printf("\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_NU_");
    hash_to_field_NIST256(MC_SHA2,HASH_TYPE_NIST256,u,&DST,&MSG,1);
    printf("u[0]= "); FP_NIST256_output(&u[0]); printf("\n");
    ECP_NIST256_map2point(&P,&u[0]);
    printf("Q= "); ECP_NIST256_output(&P);
    ECP_NIST256_cfp(&P);
    ECP_NIST256_affine(&P);
    printf("P= "); ECP_NIST256_output(&P); printf("\n");
    return res;
}


/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_GOLDILOCKS(int hash,int hlen,FP_GOLDILOCKS *u,octet *DST,octet *M, int ctr)
{

    int i,j,L,k,m;
    BIG_GL q,w,r;
    DBIG_GL dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_GL_rcopy(q, Modulus_GOLDILOCKS);
    k=BIG_GL_nbits(q);
    BIG_GL_rcopy(r, CURVE_Order_GOLDILOCKS);
    m=BIG_GL_nbits(r);
    L=CEIL(k+CEIL(m,2),8);
    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_GL_dfromBytesLen(dx,fd,L);
        BIG_GL_dmod(w,dx,q);
        FP_GOLDILOCKS_nres(&u[i],w);
    }
}

int htp_GOLDILOCKS(char *mess)
{
 
    int res=0;
    FP_GOLDILOCKS u[2];
    ECP_GOLDILOCKS P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    printf("Random Access - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_RO_");
    hash_to_field_GOLDILOCKS(MC_SHA2,HASH_TYPE_GOLDILOCKS,u,&DST,&MSG,2);
    printf("u[0]= "); FP_GOLDILOCKS_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_GOLDILOCKS_output(&u[1]); printf("\n");
    ECP_GOLDILOCKS_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_GOLDILOCKS_output(&P);
    ECP_GOLDILOCKS_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_GOLDILOCKS_output(&P1);
    ECP_GOLDILOCKS_add(&P,&P1);
    ECP_GOLDILOCKS_cfp(&P);
    ECP_GOLDILOCKS_affine(&P);
    printf("+P= "); ECP_GOLDILOCKS_output(&P); printf("\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_NU_");
    hash_to_field_GOLDILOCKS(MC_SHA2,HASH_TYPE_GOLDILOCKS,u,&DST,&MSG,1);
    printf("u[0]= "); FP_GOLDILOCKS_output(&u[0]); printf("\n");
    ECP_GOLDILOCKS_map2point(&P,&u[0]);
    printf("Q= "); ECP_GOLDILOCKS_output(&P);
    ECP_GOLDILOCKS_cfp(&P);
    ECP_GOLDILOCKS_affine(&P);
    printf("+P= "); ECP_GOLDILOCKS_output(&P); printf("\n");
    return res;
}

/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_SECP256K1(int hash,int hlen,FP_SECP256K1 *u,octet *DST,octet *M, int ctr)
{
    
    int i,j,L,k,m;
    BIG_SP q,w,r;
    DBIG_SP dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_SP_rcopy(q, Modulus_SECP256K1);
    k=BIG_SP_nbits(q);
    BIG_SP_rcopy(r, CURVE_Order_SECP256K1);
    m=BIG_SP_nbits(r);
    L=CEIL(k+CEIL(m,2),8);
    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_SP_dfromBytesLen(dx,fd,L);
        BIG_SP_dmod(w,dx,q);
        FP_SECP256K1_nres(&u[i],w);
    }
}

int htp_SECP256K1(char *mess)
{
    int res=0;
    FP_SECP256K1 u[2];
    ECP_SECP256K1 P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    printf("Random Access - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SVDW_RO_");
    hash_to_field_SECP256K1(MC_SHA2,HASH_TYPE_SECP256K1,u,&DST,&MSG,2);
    printf("u[0]= "); FP_SECP256K1_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_SECP256K1_output(&u[1]); printf("\n");
    ECP_SECP256K1_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_SECP256K1_output(&P);
    ECP_SECP256K1_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_SECP256K1_output(&P1);
    ECP_SECP256K1_add(&P,&P1);
    ECP_SECP256K1_cfp(&P);
    ECP_SECP256K1_affine(&P);
    printf("P= "); ECP_SECP256K1_output(&P); printf("\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SVDW_NU_");
    hash_to_field_SECP256K1(MC_SHA2,HASH_TYPE_SECP256K1,u,&DST,&MSG,1);
    printf("u[0]= "); FP_SECP256K1_output(&u[0]); printf("\n");
    ECP_SECP256K1_map2point(&P,&u[0]);
    printf("Q= "); ECP_SECP256K1_output(&P);
    ECP_SECP256K1_cfp(&P);
    ECP_SECP256K1_affine(&P);
    printf("P= "); ECP_SECP256K1_output(&P); printf("\n");
    return res;
}


/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_BLS12381(int hash,int hlen,FP_BLS12381 *u,octet *DST,octet *M, int ctr)
{
    int i,j,L,k,m;
    BIG_BLS12 q,w,r;
    DBIG_BLS12 dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_BLS12_rcopy(q, Modulus_BLS12381);
    k=BIG_BLS12_nbits(q);
    BIG_BLS12_rcopy(r, CURVE_Order_BLS12381);
    m=BIG_BLS12_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    //printf("L= %d\n",L);
    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_BLS12_dfromBytesLen(dx,fd,L);
        BIG_BLS12_dmod(w,dx,q);
        FP_BLS12381_nres(&u[i],w);
    }
}

int htp_BLS12381(char *mess)
{
 
    int res=0;
    FP_BLS12381 u[2];
    ECP_BLS12381 P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    printf("Random Access - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SVDW_RO_");
    hash_to_field_BLS12381(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,2);
    printf("u[0]= "); FP_BLS12381_output(&u[0]); printf("\n");
    printf("u[1]= "); FP_BLS12381_output(&u[1]); printf("\n");
    ECP_BLS12381_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP_BLS12381_output(&P);
    ECP_BLS12381_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP_BLS12381_output(&P1);
    ECP_BLS12381_add(&P,&P1);
    ECP_BLS12381_cfp(&P);
    ECP_BLS12381_affine(&P);
    printf("P= "); ECP_BLS12381_output(&P); printf("\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SVDW_NU_");
    hash_to_field_BLS12381(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,1);
    printf("u[0]= "); FP_BLS12381_output(&u[0]); printf("\n");
    ECP_BLS12381_map2point(&P,&u[0]);
    printf("Q= "); ECP_BLS12381_output(&P);
    ECP_BLS12381_cfp(&P);
    ECP_BLS12381_affine(&P);
    printf("P= "); ECP_BLS12381_output(&P); printf("\n");
    return res;
}

/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_BLS12381_G2(int hash,int hlen,FP2_BLS12381 *u,octet *DST,octet *M, int ctr)
{
    int i,j,L,k,m;
    BIG_BLS12 q,w1,w2,r;
    DBIG_BLS12 dx;
    char okm[1024],fd[256];
    octet OKM = {0,sizeof(okm),okm};

    BIG_BLS12_rcopy(q, Modulus_BLS12381);
    k=BIG_BLS12_nbits(q);
    BIG_BLS12_rcopy(r, CURVE_Order_BLS12381);
    m=BIG_BLS12_nbits(r);
    L=CEIL(k+CEIL(m,2),8);

    //printf("L= %d\n",L);
    XMD_Expand(hash,hlen,&OKM,2*L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[2*i*L+j];
        
        BIG_BLS12_dfromBytesLen(dx,fd,L);
        BIG_BLS12_dmod(w1,dx,q);

        for (j=0;j<L;j++)
            fd[j]=OKM.val[(2*i+1)*L+j];
        
        BIG_BLS12_dfromBytesLen(dx,fd,L);
        BIG_BLS12_dmod(w2,dx,q);

        FP2_BLS12381_from_BIGs(&u[i],w1,w2);

    }
}

int htp_BLS12381_G2(char *mess)
{
 
    int res=0;
    FP2_BLS12381 u[2];
    ECP2_BLS12381 P,P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};

    printf("Random Access - message= %s\n",mess);
    OCT_jstring(&MSG,mess);

    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SVDW_RO_");
    hash_to_field_BLS12381_G2(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,2);
    printf("u[0]= "); FP2_BLS12381_output(&u[0]); printf("\n");
    printf("u[1]= "); FP2_BLS12381_output(&u[1]); printf("\n");
    ECP2_BLS12381_map2point(&P,&u[0]);
    printf("Q[0]= "); ECP2_BLS12381_output(&P);
    ECP2_BLS12381_map2point(&P1,&u[1]);
    printf("Q[1]= "); ECP2_BLS12381_output(&P1);
    ECP2_BLS12381_add(&P,&P1);
    ECP2_BLS12381_cfp(&P);
    ECP2_BLS12381_affine(&P);
    printf("P= "); ECP2_BLS12381_output(&P); printf("\n");

    printf("Non-Uniform\n");
    OCT_clear(&DST);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SVDW_NU_");
    hash_to_field_BLS12381_G2(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,1);
    printf("u[0]= "); FP2_BLS12381_output(&u[0]); printf("\n");
    ECP2_BLS12381_map2point(&P,&u[0]);
    printf("Q= "); ECP2_BLS12381_output(&P);
    ECP2_BLS12381_cfp(&P);
    ECP2_BLS12381_affine(&P);
    printf("P= "); ECP2_BLS12381_output(&P); printf("\n");
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

    printf("\nTesting HTP for curve ED25519\n");
    htp_ED25519((char *)"");
    htp_ED25519((char *)"abc");
    htp_ED25519((char *)"abcdef0123456789");
    htp_ED25519((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_ED25519((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");


    printf("\nTesting HTP for curve NIST256\n");
    htp_NIST256((char *)"");
    htp_NIST256((char *)"abc");
    htp_NIST256((char *)"abcdef0123456789");
    htp_NIST256((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_NIST256((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    printf("\nTesting HTP for curve GOLDILOCKS\n");
    htp_GOLDILOCKS((char *)"");
    htp_GOLDILOCKS((char *)"abc");
    htp_GOLDILOCKS((char *)"abcdef0123456789");
    htp_GOLDILOCKS((char *)"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_GOLDILOCKS((char *)"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

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


