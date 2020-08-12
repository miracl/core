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

/* MPIN 128-bit API Functions */

/* Version 3.0 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpin_ZZZ.h"

#define ROUNDUP(a,b) ((a)-1)/(b)+1

void MPIN_ZZZ_ENCODE_TO_CURVE(octet *DST,octet *ID,octet *HCID)
{
    int j,k,m,L;
    char okm[512],fd[256];
    BIG_XXX q,r,w;
    ECP_ZZZ P;
    FP_YYY u;
    DBIG_XXX dx;
    octet OKM = {0,sizeof(okm),okm};
    BIG_XXX_rcopy(q,Modulus_YYY);
    k=BIG_XXX_nbits(q);
    BIG_XXX_rcopy(r, CURVE_Order_ZZZ);
    m=BIG_XXX_nbits(r);
    L=ROUNDUP(k+ROUNDUP(m,2),8);
    XMD_Expand(MC_SHA2,HASH_TYPE_ZZZ,&OKM,L,DST,ID);
    for (j=0;j<L;j++)
        fd[j]=OKM.val[j];
        
    BIG_XXX_dfromBytesLen(dx,fd,L);
    BIG_XXX_dmod(w,dx,q);
    FP_YYY_nres(&u,w);
    ECP_ZZZ_map2point(&P,&u);
    ECP_ZZZ_cfp(&P);
    ECP_ZZZ_affine(&P);
    ECP_ZZZ_toOctet(HCID, &P, false);
}

/* create random secret S */
int MPIN_ZZZ_RANDOM_GENERATE(csprng *RNG, octet* S)
{
    BIG_XXX r, s;
    BIG_XXX_rcopy(r, CURVE_Order_ZZZ);
    BIG_XXX_randtrunc(s, r, 2 * CURVE_SECURITY_ZZZ, RNG);
    BIG_XXX_toBytes(S->val, s);
    S->len = MODBYTES_XXX;
    return 0;
}

/* Extract PIN from TOKEN for identity CID */
int MPIN_ZZZ_EXTRACT_PIN(octet *CID, int pin, octet *TOKEN)
{
    ECP_ZZZ P, R;
    int res = 0;
    pin %= MAXPIN;

    if (!ECP_ZZZ_fromOctet(&P, TOKEN))  res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_ZZZ_fromOctet(&R,CID)) res=MPIN_INVALID_POINT;
    }
    if (res==0)
    {
        ECP_ZZZ_pinmul(&R, pin, PBLEN);
        ECP_ZZZ_sub(&P, &R);
        ECP_ZZZ_toOctet(TOKEN, &P, false);
    }
    return res;
}

/* Implement step 2 on client side of MPin protocol - SEC=-(x+y)*SEC */
int MPIN_ZZZ_CLIENT_2(octet *X, octet *Y, octet *SEC)
{
    BIG_XXX px, py, r;
    ECP_ZZZ P;
    int res = 0;
    BIG_XXX_rcopy(r, CURVE_Order_ZZZ);
    if (!ECP_ZZZ_fromOctet(&P, SEC)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        BIG_XXX_fromBytes(px, X->val);
        BIG_XXX_fromBytes(py, Y->val);
        BIG_XXX_add(px, px, py);
        BIG_XXX_mod(px, r);
        PAIR_ZZZ_G1mul(&P, px);
        ECP_ZZZ_neg(&P);
        ECP_ZZZ_toOctet(SEC, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Client secret CST=s*IDHTC where IDHTC is client ID hashed to a curve point, and s is the master secret */
int MPIN_ZZZ_GET_CLIENT_SECRET(octet *S, octet *IDHTC, octet *CST)
{
    ECP_ZZZ P;
    BIG_XXX s;
    int res = 0;
    BIG_XXX_fromBytes(s, S->val);
    if (!ECP_ZZZ_fromOctet(&P, IDHTC)) res = MPIN_INVALID_POINT;
  
    if (res==0)
    {
        PAIR_ZZZ_G1mul(&P, s);
        ECP_ZZZ_toOctet(CST, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Implement step 1 on client side of MPin protocol */
int MPIN_ZZZ_CLIENT_1(octet *CID, csprng *RNG, octet *X, int pin, octet *TOKEN, octet *SEC, octet *xID)
{
    BIG_XXX r, x;
    ECP_ZZZ P, T, W;
    int res = 0;

    BIG_XXX_rcopy(r, CURVE_Order_ZZZ);
    if (RNG != NULL)
    {
        BIG_XXX_randtrunc(x, r, 2 * CURVE_SECURITY_ZZZ, RNG);
        X->len = MODBYTES_XXX;
        BIG_XXX_toBytes(X->val, x);
    }
    else
        BIG_XXX_fromBytes(x, X->val);

    if (!ECP_ZZZ_fromOctet(&P,CID)) res=MPIN_INVALID_POINT;

    if (res==0)
    {
        if (!ECP_ZZZ_fromOctet(&T, TOKEN)) res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        pin %= MAXPIN;

        ECP_ZZZ_copy(&W, &P);               // W=H(ID)
        ECP_ZZZ_pinmul(&W, pin, PBLEN);     // W=alpha.H(ID)
        ECP_ZZZ_add(&T, &W);                // T=Token+alpha.H(ID) = s.H(ID)
        PAIR_ZZZ_G1mul(&P, x);              // P=x.H(ID)
        ECP_ZZZ_toOctet(xID, &P, false);    // xID     /* change to TRUE for point compression */
    }

    if (res == 0)
    {
        ECP_ZZZ_toOctet(SEC, &T, false); // V    /* change to TRUE for point compression */
    }
    return res;
}

/* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
int MPIN_ZZZ_GET_SERVER_SECRET(octet *S, octet *SST)
{
    BIG_XXX r, s;
    ECP2_ZZZ Q;
    int res = 0;
    BIG_XXX_rcopy(r, CURVE_Order_ZZZ);
    ECP2_ZZZ_generator(&Q);
    if (res == 0)
    {
        BIG_XXX_fromBytes(s, S->val);
        PAIR_ZZZ_G2mul(&Q, s);
        ECP2_ZZZ_toOctet(SST, &Q, false);
    }
    return res;
}

/* Implement M-Pin on server side */
int MPIN_ZZZ_SERVER(octet *HID, octet *Y, octet *SST, octet *xID, octet *mSEC)
{
    BIG_XXX y;
    FP12_YYY g;
    ECP2_ZZZ Q, sQ;
    ECP_ZZZ P, R;
    int res = 0;

    ECP2_ZZZ_generator(&Q);
    if (!ECP2_ZZZ_fromOctet(&sQ, SST)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_ZZZ_fromOctet(&R, xID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        BIG_XXX_fromBytes(y, Y->val);
        if (!ECP_ZZZ_fromOctet(&P, HID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        PAIR_ZZZ_G1mul(&P, y); 
        ECP_ZZZ_add(&P, &R); 
        if (!ECP_ZZZ_fromOctet(&R, mSEC))  res = MPIN_INVALID_POINT; // V
    }
    if (res == 0)
    {
        PAIR_ZZZ_double_ate(&g, &Q, &R, &sQ, &P);
        PAIR_ZZZ_fexp(&g);

        if (!FP12_YYY_isunity(&g))
        {
            res = MPIN_BAD_PIN;
        }
    }
    return res;
}

