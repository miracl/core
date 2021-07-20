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

/* Boneh-Lynn-Shacham signature 128-bit API */

/* Loosely (for now) following https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-bls-signature-02 */

// Minimal-signature-size variant

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bls_ZZZ.h"

using namespace XXX;
using namespace YYY;

#ifndef CORE_ARDUINO
static FP4 G2_TAB[G2_TABLE_ZZZ];  // space for precomputation on fixed G2 parameter
#endif

#define CEIL(a,b) (((a)-1)/(b)+1)

/* output u[i] \in F_p */
/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field(int hash,int hlen,FP *u,octet *DST,octet *M, int ctr)
{
    int i,j,L,nbq;
    BIG q,w;
    DBIG dx;
    char okm[256],fd[128];
    octet OKM = {0,sizeof(okm),okm};

    BIG_rcopy(q, Modulus);
    nbq=BIG_nbits(q);
    L=CEIL(nbq+CURVE_SECURITY_ZZZ,8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_dfromBytesLen(dx,fd,L);
        BIG_ctdmod(w,dx,q,8*L-nbq);
        FP_nres(&u[i],w);
    }
}

/* hash a message to an ECP point, using SHA2, random oracle method */
static void BLS_HASH_TO_POINT(ECP *P, octet *M)
{
    FP u[2];
    ECP P1;
    char dst[50];
    octet DST = {0,sizeof(dst),dst};

    OCT_jstring(&DST,(char *)"BLS_SIG_ZZZG1_XMD:SHA-256_SVDW_RO_NUL_");
    hash_to_field(MC_SHA2,HASH_TYPE_ZZZ,u,&DST,M,2);

    ECP_map2point(P,&u[0]);
    ECP_map2point(&P1,&u[1]);

    ECP_add(P,&P1);
    ECP_cfp(P);
    ECP_affine(P);
}

#ifdef CORE_ARDUINO
int ZZZ::BLS_INIT()
{
    return BLS_OK;
}
#else
int ZZZ::BLS_INIT()
{
    ECP2 G;
    if (!ECP2_generator(&G)) return BLS_FAIL;
    PAIR_precomp(G2_TAB, &G);  // should be done just once on start-up
    return BLS_OK;
}
#endif

/* generate key pair, private key S, public key W */

int ZZZ::BLS_KEY_PAIR_GENERATE(octet *IKM, octet* S, octet *W)
{
    int nbr,L;
    BIG r,s;
    DBIG dx;
    ECP2 G;
    char salt[20],prk[HASH_TYPE_ZZZ],okm[128],aikm[65],len[2];
    octet SALT = {0,sizeof(salt),salt};
    octet PRK = {0,sizeof(prk),prk};
    octet OKM = {0,sizeof(okm),okm};
    octet AIKM = {0,sizeof(aikm),aikm};
    octet LEN = {0,sizeof(len),len};

    OCT_copy(&AIKM,IKM);
    OCT_jbyte(&AIKM,0,1);

    BIG_rcopy(r, CURVE_Order);
    nbr=BIG_nbits(r);
    L=CEIL(3*CEIL(nbr,8),2);
    OCT_jint(&LEN,L,2);

    if (!ECP2_generator(&G)) return BLS_FAIL;

    OCT_jstring(&SALT,(char *)"BLS-SIG-KEYGEN-SALT-");
    HKDF_Extract(MC_SHA2,HASH_TYPE_ZZZ,&PRK,&SALT,&AIKM);
    HKDF_Expand(MC_SHA2,HASH_TYPE_ZZZ,&OKM,L,&PRK,&LEN);

    BIG_dfromBytesLen(dx,OKM.val,L);
    BIG_ctdmod(s,dx,r,8*L-nbr);
    BIG_toBytes(S->val, s);
    S->len = MODBYTES_XXX;

// SkToPk

    PAIR_G2mul(&G, s);
    ECP2_toOctet(W, &G, true);
    return BLS_OK;
}


/* Sign message M using private key S to produce signature SIG */

int ZZZ::BLS_CORE_SIGN(octet *SIG, octet *M, octet *S)
{
    BIG s;
    ECP D;
    BLS_HASH_TO_POINT(&D, M);
    BIG_fromBytes(s, S->val);
    PAIR_G1mul(&D, s);
    ECP_toOctet(SIG, &D, true); /* compress output */
    return BLS_OK;
}

/* Verify signature given message M, the signature SIG, and the public key W */
int ZZZ::BLS_CORE_VERIFY(octet *SIG, octet *M, octet *W)
{
    FP12 v;
    ECP2 G, PK;
    ECP D, HM;
    BLS_HASH_TO_POINT(&HM, M);

    ECP_fromOctet(&D, SIG);
	if (!PAIR_G1member(&D)) return BLS_FAIL;
    ECP_neg(&D);

    ECP2_fromOctet(&PK, W);
	if (!PAIR_G2member(&PK)) return BLS_FAIL;

// Use multi-pairing mechanism and precomputation on G2
#ifndef CORE_ARDUINO
    FP12 r[ATE_BITS_ZZZ];  // space for accumulated line functions

    PAIR_initmp(r);
    PAIR_another_pc(r, G2_TAB, &D);
    PAIR_another(r, &PK, &HM);
    PAIR_miller(&v, r);
#else
//.. or alternatively just
    if (!ECP2_generator(&G)) return BLS_FAIL;
    PAIR_double_ate(&v, &G, &D, &PK, &HM);
#endif
    PAIR_fexp(&v);

    if (FP12_isunity(&v)) return BLS_OK;
    return BLS_FAIL;
}

