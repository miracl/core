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

/* Test and benchmark elliptic curve and RSA functions
    First build core.a from build_ec batch file
    gcc -O3 benchtest_ec.c core.a -o benchtest_ec.exe
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "eddsa_ZZZ.h"

#if CURVETYPE_ZZZ == EDWARDS

using namespace XXX;

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
    int n=8*EGS_ZZZ-lg+1;
    BIG_mod2m(r,n);
    BIG_zero(t); BIG_inc(t,1); BIG_shl(t,n);
    BIG_add(r,r,t);
    c=BIG_lastbits(r,lg);
    BIG_dec(r,c);
//    printf("lg= %d n=%d\n",lg,n);
}

static void H(octet *S,char *digest)
{
#if CURVE_SECURITY_ZZZ <= 128  // for ed25519?
//printf("%d-bit security\n",S->len);
    hash512 sh512;
    HASH512_init(&sh512);
    for (int i=0;i<S->len;i++ )
        HASH512_process(&sh512,S->val[i]);
    HASH512_hash(&sh512,digest);
#else                          // for ed448?
///printf("%d-bit security\n",S->len);
    sha3 SHA3;
    SHA3_init(&SHA3,SHAKE256);
    for (int i=0;i<S->len;i++) 
    {
        //printf("%x ",S->val[i]);
        SHA3_process(&SHA3,S->val[i]);
    }
    //printf("\n");
    SHA3_shake(&SHA3,digest,2*S->len);  // should be twice this
#endif
}

/* Calculate a public/private EC GF(p) key pair. W=S.G mod EC(p),
 * where S is the secret key and W is the public key
 * and G is fixed generator.
 * If RNG is a cryptographically strong RNG 
 * If RNG==NULL, S is generated externally 
 * b is length of S in bits */
int ZZZ::EDDSA_KEY_PAIR_GENERATE(csprng *RNG,int b,octet* S, octet *W)
{
    BIG r, x, y, s;
    ECP G;
    char digest[128];
    int res = 0;
    int index=0;
    b/=8;

    if (b>MODBYTES_XXX) index=1;
    ECP_generator(&G);

    if (RNG != NULL)
        OCT_rand(S, RNG, b); // create private key

    H(S,digest);

// reverse bytes for little endian
    for (int i = 0; i < b/2; i++) { 
        char ch = digest[i]; 
        digest[i] = digest[b - i - 1]; 
        digest[b - i - 1] = ch; 
    } 

    BIG_fromBytes(s,&digest[index]);
    RFC7748(s);
    ECP_mul(&G, s);

    ECP_get(x, y, &G);
    W->val[0]=0;
    BIG_toBytes(&W->val[index],y);
    W->len=b;

    W->val[0]|=BIG_parity(x)<<7;

    OCT_reverse(W);

    return res;
}

#endif
