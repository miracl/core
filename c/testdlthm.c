/*
 * Copyright (c) 2012-2021 MIRACL UK Ltd.
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

/* test driver and function exerciser for Dilithium functions */

// See https://eprint.iacr.org/2017/633 (Ducas, Lepoint, Lyubashevsky, Schwabe, Seiler, Stehle)

/* gcc -O2 testdlthm.c core.a -o testdlthm */

#include <stdio.h>
#include <time.h>
#include "dilithium.h"

#define LOOPS 100

int main() {
    int i,j,attempts;
    int tats=0;
    unsigned long ran;
    bool result;
    char raw[100];
    byte tau[32];
    csprng RNG;
    char sk[DL_SK_SIZE_3], pk[DL_PK_SIZE_3], sig[DL_SIG_SIZE_3],m[128];
    octet SK = {0, sizeof(sk), sk};
    octet PK = {0, sizeof(pk), pk};
    octet SIG = {0, sizeof(sig), sig};
    octet M = {0, sizeof(m), m};

    time((time_t *)&ran);

    raw[0] = ran;
    raw[1] = ran >> 8;
    raw[2] = ran >> 16;
    raw[3] = ran >> 24;

    for (i = 0; i < 100; i++) raw[i] = i + 1; // change to i = 4 to randomize
    RAND_seed(&RNG, 100, raw);

    printf("Testing Dilithium signature\n");

    for (i=0;i<LOOPS;i++) {

        OCT_clear(&M);
        OCT_jstring(&M,(char *)"Hello World");

        for (j=0;j<32;j++)
            tau[j]=RAND_byte(&RNG);

        DLTHM_keypair_3(tau,&SK,&PK);

        printf("private key %d bits\n",8*SK.len);
        printf("public key %d bits\n",8*PK.len);

        attempts=DLTHM_signature_3(&SK,&M,&SIG);
        tats+=attempts;
        printf("Signature %d bits created on attempt %d\n",8*SIG.len,attempts);

        result=DLTHM_verify_3(&PK,&M,&SIG);
        if (result) {
            printf("Signature is verified\n");
        } else {
            printf("Signature is NOT verified\n"); 
            break;
        }
    }
    if (LOOPS>1)
        printf("Average= %d\n",tats/LOOPS);
    return 0;
} 

