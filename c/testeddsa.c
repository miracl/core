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

/* test driver and function exerciser for EDDSA API Functions */

// Test Vectors from RFC8032

// Observe that NIST standard does not support Ed25519ctx

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "randapi.h"
#include "eddsa_Ed25519.h"
#include "eddsa_Ed448.h"

// prehash message
static void PH_Ed25519(octet *M,octet *HM) {
    hash512 sh512;
    HASH512_init(&sh512);
    for (int i=0;i<M->len;i++ )
        HASH512_process(&sh512,M->val[i]);
    HASH512_hash(&sh512,HM->val);
    HM->len=64;
}

static void PH_Ed448(octet *M,octet *HM) {
    sha3 SHA3;
    SHA3_init(&SHA3,SHAKE256);
    for (int i=0;i<M->len;i++) 
        SHA3_process(&SHA3,M->val[i]);
    SHA3_shake(&SHA3,HM->val,64); 
    HM->len=64;
}

int eddsa_Ed25519(csprng *RNG)
{
    char d[EGS_Ed25519], q[EFS_Ed25519], m[256], sig[2*EFS_Ed25519], hm[64];
    octet D = {0, sizeof(d), d};
    octet Q = {0, sizeof(q), q};
    octet M = {0, sizeof(m), m};
    octet SIG = {0, sizeof(sig), sig};
    octet HM={0,sizeof(hm),hm};
    bool prehash=false;
    bool verify;

// 32 bytes
//    OCT_fromHex(&D,(char *)"9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60");
    OCT_fromHex(&D,(char *)"c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7");
//    OCT_fromHex(&D,(char *)"833fe62409237b9d62ec77587520911e9a759cec1d19755b7da901b96dca3d42");

    EDDSA_Ed25519_KEY_PAIR_GENERATE(NULL, &D, &Q);
    printf("Secret key= 0x");
    OCT_output(&D);
    printf("Public key= 0x");
    OCT_output(&Q);
    printf("\n");

    M.len=2; M.val[0]=0xaf; M.val[1]=0x82;
//    M.len=3; M.val[0]=0x61; M.val[1]=0x62;  M.val[2]=0x63;

    if (prehash) {
        PH_Ed25519(&M,&HM);
        EDDSA_Ed25519_SIGNATURE(true,&D, NULL,&HM, &SIG);
    } else {
        EDDSA_Ed25519_SIGNATURE(false,&D, NULL,&M, &SIG);
    }
    printf("Signature= 0x");
    OCT_output(&SIG);


    if (prehash) {
        PH_Ed25519(&M,&HM);
        verify=EDDSA_Ed25519_VERIFY(true,&Q, NULL ,&HM, &SIG);
    } else {
        verify=EDDSA_Ed25519_VERIFY(false,&Q, NULL ,&M, &SIG);
    }
    if (verify)
        printf("Signature is verified\n");
    else
        printf("Signature is NOT verified\n");

    return 1;
}

int eddsa_Ed448(csprng *RNG)
{
    char d[EGS_Ed448+1],  q[EFS_Ed448 + 1], m[256], sig[2*EFS_Ed448+2], ctx[256], hm[64];
    octet D = {0, sizeof(d), d};
    octet Q = {0, sizeof(q), q};
    octet M = {0, sizeof(m), m};
    octet SIG = {0, sizeof(sig), sig};
    octet CTX = {0, sizeof(ctx), ctx};
    octet HM={0,sizeof(hm),hm};
    bool prehash=false;
    bool verify;

// 57 bytes
//    OCT_fromHex(&D,(char *)"6c82a562cb808d10d632be89c8513ebf6c929f34ddfa8c9f63c9960ef6e348a3528c8a3fcc2f044e39a3fc5b94492f8f032e7549a20098f95b");
    OCT_fromHex(&D,(char *)"c4eab05d357007c632f3dbb48489924d552b08fe0c353a0d4a1f00acda2c463afbea67c5e8d2877c5e3bc397a659949ef8021e954e0a12274e");
//    OCT_fromHex(&D,(char *)"833fe62409237b9d62ec77587520911e9a759cec1d19755b7da901b96dca3d42ef7822e0d5104127dc05d6dbefde69e3ab2cec7c867c6e2c49");


    EDDSA_Ed448_KEY_PAIR_GENERATE(NULL, &D, &Q);
    printf("Secret key= 0x");
    OCT_output(&D);
    printf("Public key= 0x");
    OCT_output(&Q);
    printf("\n"); 

    M.len=1; M.val[0]=0x03; 
//    M.len=3; M.val[0]=0x61; M.val[1]=0x62;  M.val[2]=0x63;
    CTX.len=3; CTX.val[0]=0x66; CTX.val[1]=0x6F; CTX.val[2]=0x6f;


    if (prehash) {
        PH_Ed448(&M,&HM);
        EDDSA_Ed448_SIGNATURE(true,&D, NULL,&HM, &SIG);
    } else {
        EDDSA_Ed448_SIGNATURE(false,&D, NULL,&M, &SIG);
    }

    printf("Signature= 0x");
    OCT_output(&SIG);

    if (prehash) {
        PH_Ed448(&M,&HM);
        verify=EDDSA_Ed448_VERIFY(true,&Q, NULL ,&HM, &SIG);
    } else {
        verify=EDDSA_Ed448_VERIFY(false,&Q, NULL ,&M, &SIG);
    }
    if (verify)
        printf("Signature is verified\n");
    else
        printf("Signature is NOT verified\n");

    return 1;
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

    printf("\nTesting EDDSA protocols for curve Ed25519\n");
    eddsa_Ed25519(&RNG);
    printf("\nTesting EDDSA protocols for curve Ed448\n");
    eddsa_Ed448(&RNG);

    KILL_CSPRNG(&RNG);
}

