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

// EDDSA API functions

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-5.pdf

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "eddsa_ZZZ.h"

#if CURVETYPE_ZZZ == EDWARDS

using namespace XXX;
using namespace YYY;

// Process a random BIG r by RFC7748 (for Montgomery & Edwards curves only)
static void RFC7748(BIG r)
{
    int c,lg=0;
    BIG t;
    c=ZZZ::CURVE_Cof_I;
    while (c!=1)
    {
        lg++;
        c/=2;
    }
    int n=8*MODBYTES_XXX-lg+1;
    BIG_mod2m(r,n);
    BIG_zero(t); BIG_inc(t,1); BIG_shl(t,n);
    BIG_add(r,r,t);
    c=BIG_lastbits(r,lg);
    BIG_dec(r,c);
//    printf("lg= %d n=%d\n",lg,n);
}

// reverse first n bytes of buff - for little endian
static void reverse(int n,char *buff) {
    for (int i = 0; i < n/2; i++) { 
        char ch = buff[i]; 
        buff[i] = buff[n - i - 1]; 
        buff[n - i - 1] = ch; 
    } 
}

// dom2 - context still needs to be appended
static void dom2(bool ph,int cl,octet *DOM) {
    OCT_jstring(DOM,(char *)"SigZZZ no ZZZ collisions");
    if (ph)
        OCT_jbyte(DOM,1,1);
    else 
        OCT_jbyte(DOM,0,1);
    OCT_jbyte(DOM,cl,1); // context length
}

// dom4 - context still needs to be appended
static void dom4(bool ph,int cl,octet *DOM) {
    OCT_jstring(DOM,(char *)"SigZZZ");
    if (ph)
        OCT_jbyte(DOM,1,1);
    else 
        OCT_jbyte(DOM,0,1);
    OCT_jbyte(DOM,cl,1); // context length
}

static void H(octet *S,char *digest)
{
#if CURVE_SECURITY_ZZZ <= 128  // for ed25519?
    hash512 sh512;
    HASH512_init(&sh512);
    for (int i=0;i<S->len;i++ )
        HASH512_process(&sh512,S->val[i]);
    HASH512_hash(&sh512,digest);
#else                          // for ed448?
    sha3 SHA3;
    SHA3_init(&SHA3,SHAKE256);
    for (int i=0;i<S->len;i++) 
        SHA3_process(&SHA3,S->val[i]);
    SHA3_shake(&SHA3,digest,2*S->len); 
#endif
}

static int H2(bool ph,octet *context,octet *R,octet *Q,octet *M,DBIG dr)
{
    char h[128];
    int b=Q->len;
    char dom[64];
    octet DOM={0,sizeof(dom),dom};
    int cl;
    if (context==NULL) cl=0;
    else cl=context->len&0xff;
#if CURVE_SECURITY_ZZZ <= 128  // for ed25519?
    hash512 sh512;
    HASH512_init(&sh512);
    if (ph || cl>0)
    {                   // if not prehash and no context, omit dom2()
        dom2(ph,cl,&DOM);
        for (int i=0;i<DOM.len;i++)
            HASH512_process(&sh512,DOM.val[i]);
        for (int i=0;i<cl;i++)
            HASH512_process(&sh512,context->val[i]);
    }
    for (int i=0;i<b;i++ )
        HASH512_process(&sh512,R->val[i]);
    for (int i=0;i<b;i++ )
        HASH512_process(&sh512,Q->val[i]);
    for (int i=0;i<M->len;i++ )
        HASH512_process(&sh512,M->val[i]);
    HASH512_hash(&sh512,h);
#else                          // for ed448?
    dom4(ph,cl,&DOM);
    sha3 SHA3;
    SHA3_init(&SHA3,SHAKE256);
    for (int i=0;i<DOM.len;i++)
        SHA3_process(&SHA3,DOM.val[i]);
    for (int i=0;i<cl;i++)
        SHA3_process(&SHA3,context->val[i]);
    for (int i=0;i<b;i++) 
        SHA3_process(&SHA3,R->val[i]);
    for (int i=0;i<b;i++) 
        SHA3_process(&SHA3,Q->val[i]);
    for (int i=0;i<M->len;i++) 
        SHA3_process(&SHA3,M->val[i]);
    SHA3_shake(&SHA3,h,2*b);
#endif
    reverse(2*b,h);
    BIG_dfromBytesLen(dr,h,2*b);
    return 2*b;
}

static void getR(bool ph,int b,char *digest,octet *context,octet *M,DBIG dr) {
    char h[128];
    char dom[64];
    octet DOM={0,sizeof(dom),dom};
    int cl;
    if (context==NULL) cl=0;
    else cl=context->len&0xff;

#if CURVE_SECURITY_ZZZ <= 128  // for ed25519?
    hash512 sh512;
    HASH512_init(&sh512);
    if (ph || cl>0)
    {                   // if not prehash and no context, omit dom2()
        dom2(ph,cl,&DOM);
        for (int i=0;i<DOM.len;i++)
            HASH512_process(&sh512,DOM.val[i]);
        for (int i=0;i<cl;i++)
            HASH512_process(&sh512,context->val[i]);
    }
    for (int i=b;i<2*b;i++ )
        HASH512_process(&sh512,digest[i]);
    for (int i=0;i<M->len;i++)
        HASH512_process(&sh512,M->val[i]);
    HASH512_hash(&sh512,h);

#else                       // for ed448?
    dom4(ph,cl,&DOM);
    sha3 SHA3;
    SHA3_init(&SHA3,SHAKE256);
    for (int i=0;i<DOM.len;i++)
        SHA3_process(&SHA3,DOM.val[i]);
    for (int i=0;i<cl;i++)
        SHA3_process(&SHA3,context->val[i]);
    for (int i=b;i<2*b;i++ )
        SHA3_process(&SHA3,digest[i]);
    for (int i=0;i<M->len;i++) 
        SHA3_process(&SHA3,M->val[i]);
    SHA3_shake(&SHA3,h,2*b); 
#endif

    reverse(2*b,h);
    BIG_dfromBytesLen(dr,h,2*b);
}
// encode integer (little endian)
static int encode_int(XXX::BIG x,char *ei) {
    int b,index=0;

    if (8*MODBYTES_XXX==MBITS_YYY) index=1; // extra byte needed for compression        
    b=MODBYTES_XXX+index;

    ei[0]=0;
    BIG_toBytes(&ei[index],x);
    reverse(b,ei);
    return b;
}

// encode point
static void encode(ZZZ::ECP *P,octet *W) {
    BIG x,y;
    int b,index=0;

    if (8*MODBYTES_XXX==MBITS_YYY) index=1; // extra byte needed for compression        
    b=MODBYTES_XXX+index;

    ECP_get(x, y, P);
    b=encode_int(y,W->val);
    W->val[b-1]|=BIG_parity(x)<<7;
    W->len=b;
}

// decode integer (little endian)
static int decode_int(bool strip_sign,char *ei,XXX::BIG x) {
    int b,index=0;
    int sign=0;
    char r[MODBYTES_XXX+1];

    if (8*MODBYTES_XXX==MBITS_YYY) index=1; // extra byte needed for compression        
    b=MODBYTES_XXX+index;

    for (int i=0;i<b;i++)
        r[i]=ei[i];
  
    reverse(b,r);
    if ((r[0]&0x80)!=0)
        sign=1;
    if (strip_sign)
        r[0]&=0x7f;

    if (r[0]==0) // remove leading zero
        BIG_fromBytesLen(x,&r[1],b-1);
    else
        BIG_fromBytesLen(x,&r[0],b);
    return sign;
}

// decode compressed point
static void decode(octet *W,ZZZ::ECP *P) {
    BIG X,Y;
    FP x,d,t,one,hint;
    int sign=decode_int(true,W->val,Y); // LSB of x
    FP_nres(&x,Y); 
    FP_sqr(&x,&x);
    FP_copy(&d,&x); FP_one(&one);
    FP_sub(&x,&x,&one);
    FP_norm(&x);
    FP_rcopy(&t, ZZZ::CURVE_B);

    FP_mul(&d,&d,&t);
#if CURVE_A_ZZZ == 1
    FP_sub(&d,&d,&one);
#else
    FP_add(&d,&d,&one);
#endif  
    FP_norm(&d);
// inverse square root trick for sqrt(x/d)
    FP_sqr(&t,&x);
    FP_mul(&x,&x,&t);
    FP_mul(&x,&x,&d);
    if (!FP_qr(&x,&hint))
    {
        ECP_inf(P);
        return;
    }
    FP_sqrt(&d,&x,&hint);
    FP_inv(&x,&x,&hint);
    FP_mul(&x,&x,&d);
    FP_mul(&x,&x,&t);

    FP_reduce(&x);
    FP_redc(X,&x);
    if (BIG_parity(X)!=sign)
        FP_neg(&x,&x);
    FP_norm(&x);
    FP_redc(X,&x);
    ECP_set(P,X,Y);
}

/* Calculate a public/private EC GF(p) key pair. Q=D.G mod EC(p),
 * where D is the secret key and Q is the public key
 * and G is fixed generator.
 * RNG is a cryptographically strong RNG 
 * If RNG==NULL, D is generated externally 
 */
int ZZZ::EDDSA_KEY_PAIR_GENERATE(csprng *RNG,octet* D,octet *Q)
{
    BIG r, x, y, s;
    ECP G;
    char digest[128];
    int res = EDDSA_OK;
    int b,index=0;

    if (8*MODBYTES_XXX==MBITS_YYY) index=1; // extra byte needed for compression        
    b=MODBYTES_XXX+index;

    ECP_generator(&G);

    if (RNG != NULL)
        OCT_rand(D, RNG, b); // create random private key

    H(D,digest);

// reverse bytes for little endian
    reverse(b,digest);
    BIG_fromBytes(s,&digest[index]);
    RFC7748(s);
    ECP_mul(&G, s);

    encode(&G,Q);
    return res;
}

// Generate a signature using key pair (D,Q) on message M
// Set ph=true if message has already been pre-hashed
// if ph=false, then context should be NULL for ed25519. However RFC8032 mode ed25519ctx is supported by supplying a non-NULL or non-empty context
int ZZZ::EDDSA_SIGNATURE(bool ph,octet *D, octet *context,octet *M,octet *SIG)
{
    DBIG dr;
    BIG s,sr,sd,q;
    ECP R;
    char digest[128];
    char pq[MODBYTES_XXX+1];
    octet Q={0,sizeof(pq),pq};
    H(D,digest);   // hash of private key
    int res = EDDSA_OK;
    int b,index=0;
    if (8*MODBYTES_XXX==MBITS_YYY) index=1; // extra byte needed for compression        
    b=MODBYTES_XXX+index;

    EDDSA_KEY_PAIR_GENERATE(NULL,D,&Q);

    BIG_rcopy(q, CURVE_Order);
    ECP_generator(&R);

    if (D->len!=Q.len || D->len!=b)
        res=EDDSA_INVALID_PUBLIC_KEY;
    if (res==EDDSA_OK)
    {
        getR(ph,b,digest,context,M,dr);
        BIG_dmod(sr,dr,q);
        ECP_mul(&R,sr);
        encode(&R,SIG);
// reverse bytes for little endian        
        reverse(b,digest);
        BIG_fromBytes(s,&digest[index]);
        RFC7748(s);
        H2(ph,context,SIG,&Q,M,dr);
        BIG_dmod(sd,dr,q);
        BIG_modmul(s,s,sd,q);
        BIG_modadd(s,s,sr,q);
        encode_int(s,&SIG->val[b]);
        SIG->len=2*b;
    }
    return res;
}

// verify a signature using public key Q
// same context (if any) as used for signature
bool ZZZ::EDDSA_VERIFY(bool ph,octet *Q,octet *context,octet *M,octet *SIG) 
{
    DBIG du;
    BIG q,t,su;
    ECP R,QD,G;
    int res = EDDSA_OK;
    bool dr,dq;
    int lg=0;
    int c,b,index=0;
    if (8*MODBYTES_XXX==MBITS_YYY) index=1; // extra byte needed for compression        
    b=MODBYTES_XXX+index;

    c=ZZZ::CURVE_Cof_I;
    while (c!=1)
    {
        lg++;
        c/=2;
    }
    BIG_rcopy(q, CURVE_Order);

    decode(SIG,&R);
    if (ECP_isinf(&R)) return false;
    decode_int(false,&SIG->val[b],t);
    if (BIG_comp(t,q)>=0) return false;
    decode(Q,&QD);
    if (ECP_isinf(&QD)) return false;

    H2(ph,context,SIG,Q,M,du);
    BIG_dmod(su,du,q);
    ECP_generator(&G);
    for (int i=0;i<lg;i++)
    { // use cofactor 2^c
        ECP_dbl(&G); ECP_dbl(&QD); ECP_dbl(&R);
    }

    ECP_neg(&QD);
    ECP_mul2(&G,&QD,t,su);

    if (!ECP_equals(&G,&R)) return false;

    return true;
}

#endif
