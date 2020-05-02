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

/* Hybrid Public Key Encryption */

/* Following https://datatracker.ietf.org/doc/draft-irtf-cfrg-hpke/?include_text=1 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hpke_ZZZ.h"

void HPKE_ZZZ_Encap(int config_id,csprng *RNG,octet *SKE,octet *Z,octet *pkE,octet *pkR)
{
    int res,kem;
    char ske[EGS_ZZZ];
    octet skE={0,sizeof(ske),ske};

    kem=config_id&7;
    if (RNG==NULL)
    {
        OCT_copy(&skE,SKE);
        if (kem==2 || kem==4) {
            OCT_reverse(&skE);
            if (kem==2)
            {
                skE.val[EGS_ZZZ-1]&=248;  // x2EGS_ZZZ-119 scalar processing
                skE.val[0]&=127;
                skE.val[0]|=64;
            } else {
                skE.val[EGS_ZZZ-1]&=252;
                skE.val[0]|=128;
            }
        }
    }

    res=ECP_ZZZ_KEY_PAIR_GENERATE(RNG, &skE, pkE);
    if (kem==2 || kem==4)
        OCT_reverse(pkR);

    res=ECP_ZZZ_SVDP_DH(&skE, pkR, Z);
    if (kem==2 || kem==4) {
        OCT_reverse(pkR);
        OCT_reverse(pkE);
        OCT_reverse(Z);
    }
}

void HPKE_ZZZ_Decap(int config_id,octet *Z,octet *pkE,octet *SKR)
{
    int res,kem;
    char skr[EGS_ZZZ];
    octet skR={0,sizeof(skr),skr};
    kem=config_id&7;
    OCT_copy(&skR,SKR);

    if (kem==2 || kem==4) {
        OCT_reverse(&skR);
        OCT_reverse(pkE);
        if (kem==2)
        {
            skR.val[EGS_ZZZ-1]&=248;
            skR.val[0]&=127;
            skR.val[0]|=64;
        } else {
            skR.val[EGS_ZZZ-1]&=252;
            skR.val[0]|=128;
        }
    }
    ECP_ZZZ_SVDP_DH(&skR, pkE, Z);
    if (kem==2 || kem==4) {
        OCT_reverse(pkE);
        OCT_reverse(Z);
    }
}

void HPKE_ZZZ_AuthEncap(int config_id,csprng *RNG,octet *SKE,octet *Z,octet *pkE,octet *pkR,octet *SKI)
{
    int res,kem;
    char ske[EGS_ZZZ];
    octet skE={0,sizeof(ske),ske};
    char ski[EGS_ZZZ];
    octet skI={0,sizeof(ski),ski};
    char nz[EFS_ZZZ];
    octet NZ={0,sizeof(nz),nz};
    kem=config_id&7;

    if (RNG==NULL)
    {
        OCT_copy(&skE,SKE);
        OCT_copy(&skI,SKI);
        if (kem==2 || kem==4) {
            OCT_reverse(&skE);
            OCT_reverse(&skI);
            if (kem==2)
            {
                skE.val[EGS_ZZZ-1]&=248;
                skE.val[0]&=127;
                skE.val[0]|=64;
                skI.val[EGS_ZZZ-1]&=248;
                skI.val[0]&=127;
                skI.val[0]|=64;
            } else {
                skE.val[EGS_ZZZ-1]&=252;
                skE.val[0]|=128;
                skI.val[EGS_ZZZ-1]&=252;
                skI.val[0]|=128;
            }
        }
    }

    ECP_ZZZ_KEY_PAIR_GENERATE(RNG, &skE, pkE);
    if (kem==2 || kem==4)
        OCT_reverse(pkR);

    ECP_ZZZ_SVDP_DH(&skE, pkR, Z);
    ECP_ZZZ_SVDP_DH(&skI, pkR, &NZ);
    if (kem==2 || kem==4) {
        OCT_reverse(pkR);
        OCT_reverse(pkE);
        OCT_reverse(Z);
        OCT_reverse(&NZ);
    }
    OCT_joctet(Z,&NZ);
}

void HPKE_ZZZ_AuthDecap(int config_id,octet *Z,octet *pkE,octet *SKR,octet *pkI)
{
    int res,kem;
    char skr[EGS_ZZZ];
    octet skR={0,sizeof(skr),skr};
    char nz[EFS_ZZZ];
    octet NZ={0,sizeof(nz),nz};
    OCT_copy(&skR,SKR);
    kem=config_id&7;

    if (kem==2 || kem==4) {
        OCT_reverse(&skR);
        OCT_reverse(pkE);
        OCT_reverse(pkI);
        if (kem==2)
        {
            skR.val[EGS_ZZZ-1]&=248;
            skR.val[0]&=127;
            skR.val[0]|=64;
        } else {
            skR.val[EGS_ZZZ-1]&=252;
            skR.val[0]|=128;
        }
    }

    ECP_ZZZ_SVDP_DH(&skR, pkE, Z);
    ECP_ZZZ_SVDP_DH(&skR, pkI, &NZ);
    if (kem==2 || kem==4) {
        OCT_reverse(pkE);
        OCT_reverse(pkI);
        OCT_reverse(Z);
        OCT_reverse(&NZ);
    }
    OCT_joctet(Z,&NZ);
}

void HPKE_ZZZ_KeySchedule(int config_id,octet *key,octet *nonce,int mode,octet *pkR,octet *Z,octet *pkE,octet *info,octet *psk,octet *pskID,octet *pkI)
{
    char context[550];
    octet CONTEXT={0,sizeof(context),context};
    char h[64];
    octet H={0,sizeof(h),h};
    char secret[64];
    octet SECRET={0,sizeof(secret),secret};
    char full_context[550];
    octet FULL_CONTEXT={0,sizeof(full_context),full_context};

    int kem_id=config_id&7;
    int kdf_id=(config_id>>3)&3;
    int aead_id=(config_id>>5)&3;

    OCT_jint(&CONTEXT,mode,1);
    OCT_jint(&CONTEXT,kem_id,2);
    OCT_jint(&CONTEXT,kdf_id,2);
    OCT_jint(&CONTEXT,aead_id,2);
    OCT_joctet(&CONTEXT,pkE);
    OCT_joctet(&CONTEXT,pkR);
    if (pkI==NULL)
        OCT_jbyte(&CONTEXT,0,pkR->max);
    else
        OCT_joctet(&CONTEXT,pkI);

    SPhash(MC_SHA2,HASH_TYPE_ZZZ,&H,pskID);
    OCT_joctet(&CONTEXT,&H);
    SPhash(MC_SHA2,HASH_TYPE_ZZZ,&H,info);
    OCT_joctet(&CONTEXT,&H);

    //printf("Context= "); OCT_output(&CONTEXT);
    HKDF_Extract(MC_SHA2,HASH_TYPE_ZZZ,&SECRET,psk,Z);
    //printf("Secret= "); OCT_output(&SECRET);

    OCT_jbytes(&FULL_CONTEXT,(char *)"hpke key",8); OCT_joctet(&FULL_CONTEXT,&CONTEXT);
    HKDF_Expand(MC_SHA2,HASH_TYPE_ZZZ,key,AESKEY_ZZZ,&SECRET,&FULL_CONTEXT);
    OCT_clear(&FULL_CONTEXT);
    OCT_jbytes(&FULL_CONTEXT,(char *)"hpke nonce",10); OCT_joctet(&FULL_CONTEXT,&CONTEXT);
    HKDF_Expand(MC_SHA2,HASH_TYPE_ZZZ,nonce,12,&SECRET,&FULL_CONTEXT);
}
