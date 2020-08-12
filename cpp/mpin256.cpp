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

/* MPIN 256-bit API Functions */

/* Version 3.0  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpin256_ZZZ.h"

using namespace XXX;
using namespace YYY;

#define ROUNDUP(a,b) ((a)-1)/(b)+1

void ZZZ::MPIN_ENCODE_TO_CURVE(octet *DST,octet *ID,octet *HCID)
{
    int j,k,m,L;
    char okm[512],fd[256];
    BIG q,r,w;
    ECP P;
    FP u;
    DBIG dx;
    octet OKM = {0,sizeof(okm),okm};
    BIG_rcopy(q,Modulus);
    k=BIG_nbits(q);
    BIG_rcopy(r, CURVE_Order);
    m=BIG_nbits(r);
    L=ROUNDUP(k+ROUNDUP(m,2),8);
    XMD_Expand(MC_SHA2,HASH_TYPE_ZZZ,&OKM,L,DST,ID);
    for (j=0;j<L;j++)
        fd[j]=OKM.val[j];
        
    BIG_dfromBytesLen(dx,fd,L);
    BIG_dmod(w,dx,q);
    FP_nres(&u,w);
    ECP_map2point(&P,&u);
    ECP_cfp(&P);
    ECP_affine(&P);
    ECP_toOctet(HCID, &P, false);
}

/* create random secret S */
int ZZZ::MPIN_RANDOM_GENERATE(csprng *RNG, octet* S)
{
    BIG r, s;

    BIG_rcopy(r, CURVE_Order);
    BIG_randtrunc(s, r, 2 * CURVE_SECURITY_ZZZ, RNG);
    BIG_toBytes(S->val, s);
    S->len = MODBYTES_XXX;
    return 0;
}

/* Extract PIN from TOKEN for identity CID */
int ZZZ::MPIN_EXTRACT_PIN(octet *CID, int pin, octet *TOKEN)
{
    ECP P, R;
    int res = 0;
    pin %= MAXPIN;
    if (!ECP_fromOctet(&P, TOKEN))  res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_fromOctet(&R,CID)) res=MPIN_INVALID_POINT;
    }
    if (res==0)
    {
        ECP_pinmul(&R, pin, PBLEN);
        ECP_sub(&P, &R);
        ECP_toOctet(TOKEN, &P, false);
    }
    return res;
}

/* Implement step 2 on client side of MPin protocol - SEC=-(x+y)*SEC */
int ZZZ::MPIN_CLIENT_2(octet *X, octet *Y, octet *SEC)
{
    BIG px, py, r;
    ECP P;
    int res = 0;
    BIG_rcopy(r, CURVE_Order);
    if (!ECP_fromOctet(&P, SEC)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        BIG_fromBytes(px, X->val);
        BIG_fromBytes(py, Y->val);
        BIG_add(px, px, py);
        BIG_mod(px, r);
        PAIR_G1mul(&P, px);
        ECP_neg(&P);
        ECP_toOctet(SEC, &P, false);
    }
    return res;
}

/* Client secret CST=s*IDHTC where IDHTC is client ID hashed to a curve point, and s is the master secret */
int ZZZ::MPIN_GET_CLIENT_SECRET(octet *S, octet *IDHTC, octet *CST)
{
    ECP P;
    BIG s;
    int res = 0;
    BIG_fromBytes(s, S->val);
    if (!ECP_fromOctet(&P, IDHTC)) res = MPIN_INVALID_POINT;
    if (res==0)
    {
        PAIR_G1mul(&P, s);
        ECP_toOctet(CST, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Implement step 1 on client side of MPin protocol */
int ZZZ::MPIN_CLIENT_1(octet *CID, csprng *RNG, octet *X, int pin, octet *TOKEN, octet *SEC, octet *xID)
{
    BIG r, x;
    ECP P, T, W;
    int res = 0;
    BIG_rcopy(r, CURVE_Order);
    if (RNG != NULL)
    {
        BIG_randtrunc(x, r, 2 * CURVE_SECURITY_ZZZ, RNG);
        X->len = MODBYTES_XXX;
        BIG_toBytes(X->val, x);
    }
    else
        BIG_fromBytes(x, X->val);

    if (!ECP_fromOctet(&P,CID)) res=MPIN_INVALID_POINT;

    if (res==0)
    {    
        if (!ECP_fromOctet(&T, TOKEN)) res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        pin %= MAXPIN;
        ECP_copy(&W, &P);               // W=H(ID)
        ECP_pinmul(&W, pin, PBLEN);         // W=alpha.H(ID)
        ECP_add(&T, &W);                // T=Token+alpha.H(ID) = s.H(ID)
        PAIR_G1mul(&P, x);              // P=x.H(ID)
        ECP_toOctet(xID, &P, false); // xID
    }
    if (res == 0)
    {
        ECP_toOctet(SEC, &T, false); // V
    }
    return res;
}

/* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
int ZZZ::MPIN_GET_SERVER_SECRET(octet *S, octet *SST)
{
    BIG r, s;
    ECP8 Q;
    int res = 0;
    BIG_rcopy(r, CURVE_Order);
    ECP8_generator(&Q);
    if (res == 0)
    {
        BIG_fromBytes(s, S->val);
        PAIR_G2mul(&Q, s);
        ECP8_toOctet(SST, &Q, false);
    }
    return res;
}

/* Implement M-Pin on server side */
int ZZZ::MPIN_SERVER(octet *HID, octet *Y, octet *SST, octet *xID, octet *mSEC)
{
    BIG y;
    FP48 g;
    ECP8 Q, sQ;
    ECP P, R;
    int res = 0;
    ECP8_generator(&Q);

    if (!ECP8_fromOctet(&sQ, SST)) res = MPIN_INVALID_POINT;

    if (res == 0)
    {
        if (!ECP_fromOctet(&R, xID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        BIG_fromBytes(y, Y->val);
        if (!ECP_fromOctet(&P, HID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        PAIR_G1mul(&P, y); // y(A+AT)
        ECP_add(&P, &R); // x(A+AT)+y(A+T)
        if (!ECP_fromOctet(&R, mSEC))  res = MPIN_INVALID_POINT; // V
    }
    if (res == 0)
    {
        PAIR_double_ate(&g, &Q, &R, &sQ, &P);
        PAIR_fexp(&g);
        if (!FP48_isunity(&g))
        {
            res = MPIN_BAD_PIN;
        }
    }
    return res;
}

