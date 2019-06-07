/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/* Boneh-Lynn-Shacham signature 128-bit API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bls_ZZZ.h"

using namespace XXX;
using namespace YYY;

#ifndef AMCL_ARDUINO
static FP4 G2_TAB[G2_TABLE_ZZZ];  // space for precomputation on fixed G2 parameter
#endif

/* hash a message to an ECP point, using SHA3 */
static void bls_hashit(ECP *P,char *m)
{
	int i;
    sha3 hs;
	char h[MODBYTES_XXX];
    octet HM= {0,sizeof(h),h};
	SHA3_init(&hs,SHAKE256);
    for (i=0;m[i]!=0;i++) SHA3_process(&hs,m[i]);
    SHA3_shake(&hs,HM.val,MODBYTES_XXX);
	HM.len=MODBYTES_XXX;
	ECP_mapit(P,&HM);
}

#ifdef AMCL_ARDUINO
void ZZZ::BLS_INIT()
{
}
#else
void ZZZ::BLS_INIT()
{
	ECP2 G;
	ECP2_generator(&G);
	PAIR_precomp(G2_TAB,&G);   // should be done just once on start-up
}
#endif

/* generate key pair, private key S, public key W */

int ZZZ::BLS_KEY_PAIR_GENERATE(csprng *RNG,octet* S,octet *W)
{
	ECP2 G;
	BIG s,q;
    BIG_rcopy(q,CURVE_Order);
	ECP2_generator(&G);
	BIG_randtrunc(s,q,2*CURVE_SECURITY_ZZZ,RNG);
    BIG_toBytes(S->val,s);
    S->len=MODBYTES_XXX;
    PAIR_G2mul(&G,s);
	ECP2_toOctet(W,&G);
	return BLS_OK;
}

/* Sign message m using private key S to produce signature SIG */

int ZZZ::BLS_SIGN(octet *SIG,char *m,octet *S)
{
	BIG s;
	ECP D;
	bls_hashit(&D,m);
	BIG_fromBytes(s,S->val);
	PAIR_G1mul(&D,s);
	ECP_toOctet(SIG,&D,true); /* compress output */
	return BLS_OK;
}

/* Verify signature given message m, the signature SIG, and the public key W */
int ZZZ::BLS_VERIFY(octet *SIG,char *m,octet *W)
{
	FP12 v;
	ECP2 G,PK;
	ECP D,HM;
	bls_hashit(&HM,m);
	ECP_fromOctet(&D,SIG);
	ECP2_fromOctet(&PK,W);
	ECP_neg(&D);

// Use multi-pairing mechanism and precomputation on G2
#ifndef AMCL_ARDUINO
	FP12 r[ATE_BITS_ZZZ];  // space for accumulated line functions

	PAIR_initmp(r);
	PAIR_another_pc(r,G2_TAB,&D);
	PAIR_another(r,&PK,&HM);
	PAIR_miller(&v,r);
#else
//.. or alternatively just
    ECP2_generator(&G);
    PAIR_double_ate(&v,&G,&D,&PK,&HM);
#endif
	PAIR_fexp(&v);

    if (FP12_isunity(&v)) return BLS_OK;
	return BLS_FAIL;
}

