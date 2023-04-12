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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "eddsa_ED25519.h"
#include "randapi.h"

#if CHUNK==32 || CHUNK==64
#include "eddsa_GOLDILOCKS.h"
#endif

using namespace core;

int eddsa_ED25519(csprng *RNG)
{
    using namespace ED25519;

    char s[EGS_ED25519], w[EFS_ED25519];
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};


    OCT_fromHex(&S,(char *)"9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60");

    EDDSA_KEY_PAIR_GENERATE(NULL, 256, &S, &W);
    printf("Secret key= 0x");
    OCT_output(&S);
    printf("Public key= 0x");
    OCT_output(&W);
    printf("\n");
    return 1;
}

#if CHUNK==32 || CHUNK==64
int eddsa_GOLDILOCKS(csprng *RNG)
{
    using namespace GOLDILOCKS;

    char s[EGS_GOLDILOCKS+1],  w[EFS_GOLDILOCKS + 1];
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};

    OCT_fromHex(&S,(char *)"6c82a562cb808d10d632be89c8513ebf6c929f34ddfa8c9f63c9960ef6e348a3528c8a3fcc2f044e39a3fc5b94492f8f032e7549a20098f95b");

    EDDSA_KEY_PAIR_GENERATE(NULL, 456, &S, &W);
    printf("Secret key= 0x");
    OCT_output(&S);
    printf("Public key= 0x");
    OCT_output(&W);
    printf("\n"); 

    return 1;
}
#endif

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
    for (i = 0; i < 100; i++) RAW.val[i] = i;

    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG

    printf("Testing EDDSA protocols for curve ED25519\n");
    eddsa_ED25519(&RNG);

#if CHUNK!=16
    printf("Testing EDDSA protocols for curve GOLDILOCKS\n");
    eddsa_GOLDILOCKS(&RNG);
#endif
    KILL_CSPRNG(&RNG);
}

