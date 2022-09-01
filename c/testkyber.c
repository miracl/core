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

/* test driver and function exerciser for Kyber functions */
/* g++ -O2 testkyber.cpp core.a -o testkyber */

#include <stdio.h>
#include <time.h>
#include "kyber.h"

#define LOOPS 100

int main() {
    int i,j;
    unsigned long ran;
    bool result;
    char raw[100];
    csprng RNG;
    char sk[KYBER_SECRET_CPA_SIZE_768], pk[KYBER_PUBLIC_SIZE_768],ct[KYBER_CIPHERTEXT_SIZE_768],skc[KYBER_SECRET_CCA_SIZE_768],ss[KYBER_SHARED_SECRET_768];
    octet SKC = {0, sizeof(skc), skc};
    octet SK = {0, sizeof(sk), sk};
    octet PK = {0, sizeof(pk), pk};
    octet CT = {0, sizeof(ct), ct};
    octet SS = {0, sizeof(ss), ss};

    byte tau[32],r64[64],r32[32];
    byte coins[32];

    time((time_t *)&ran);

    raw[0] = ran;
    raw[1] = ran >> 8;
    raw[2] = ran >> 16;
    raw[3] = ran >> 24;

    for (i = 0; i < 100; i++) raw[i] = i + 1; 
    RAND_seed(&RNG, 100, raw);
       
    printf("Testing Kyber Encryption\n");

    for (j=0;j<LOOPS;j++) {

        for (i=0;i<64;i++)
            r64[i]=RAND_byte(&RNG); 
  
        KYBER768_keypair(r64,&SKC,&PK);

//printf("Public key= ");
//for (i=0;i<PK.len;i++)
//    printf("%02x",(byte)PK.val[i]);
//printf("\n");
    
//printf("Secret key= ");
//for (i=0;i<SKC.len;i++)
//    printf("%02x",(byte)SKC.val[i]);
//printf("\n");


        for (i=0;i<32;i++)
            r32[i]=RAND_byte(&RNG); 

		KYBER768_encrypt(r32,&PK,&SS,&CT); 

//printf("Ciphertext= ");
//for (i=0;i<CT.len;i++)
//    printf("%02x",(byte)CT.val[i]);
//printf("\n");

        printf("j= %d\n",j);
        for (i=0;i<32;i++)
            printf("%02x",(byte)SS.val[i]);
        printf("\n");

		KYBER768_decrypt(&SKC,&CT,&SS);
        
        for (i=0;i<32;i++)
            printf("%02x",(byte)SS.val[i]);
        printf("\n");
    }

    return 0;
} 


