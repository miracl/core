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

#define GROUP EGS_ZZZ
#define POINT (2*EFS_ZZZ+1)
#define MAX_HASH HASH_TYPE_ZZZ
#define MAX_LABEL 20                // may need adjustment

static void LabeledExtract(octet *PRK,octet *SALT,octet *SUITE_ID,char *label,octet *IKM)
{
    char likm[8+MAX_LABEL+2*POINT];
    octet LIKM={0,sizeof(likm),likm};
    OCT_jstring(&LIKM,(char *)"HPKE-v1");
    OCT_joctet(&LIKM,SUITE_ID);
    OCT_jstring(&LIKM,label);
    if (IKM!=NULL)
        OCT_joctet(&LIKM,IKM);
    HKDF_Extract(MC_SHA2,HASH_TYPE_ZZZ,PRK,SALT,&LIKM);
}

static void LabeledExpand(octet *OKM,octet *PRK,octet *SUITE_ID,char *label,octet *INFO,int L)
{
    char linfo[10+MAX_LABEL+3*POINT];
    octet LINFO={0,sizeof(linfo),linfo};
    OCT_jint(&LINFO,L,2);    
    OCT_jstring(&LINFO,(char *)"HPKE-v1");
    OCT_joctet(&LINFO,SUITE_ID);
    OCT_jstring(&LINFO,label);
    if (INFO!=NULL)
        OCT_joctet(&LINFO,INFO);
    HKDF_Expand(MC_SHA2,HASH_TYPE_ZZZ,OKM,L,PRK,&LINFO);
}

static void ExtractAndExpand(int config_id,octet *OKM,octet *DH,octet *CONTEXT)
{
    char prk[MAX_HASH];
    octet PRK={0,sizeof(prk),prk};
    char suite_id[10];
    octet SUITE_ID={0,sizeof(suite_id),suite_id};

    int kem_id=config_id&255;
    OCT_jstring(&SUITE_ID,(char *)"KEM");
    OCT_jint(&SUITE_ID,kem_id,2);

    LabeledExtract(&PRK,NULL,&SUITE_ID,(char *)"eae_prk",DH);
    LabeledExpand(OKM, &PRK,&SUITE_ID,(char *)"shared_secret",CONTEXT,HASH_TYPE_ZZZ);  
}

int DeriveKeyPair_ZZZ(int config_id,octet *SK,octet *PK,octet *SEED)
{
    int counter=0;
    char prk[MAX_HASH];
    octet PRK={0,sizeof(prk),prk};
    char suite_id[10];
    octet SUITE_ID={0,sizeof(suite_id),suite_id};
    char info[2];
    octet INFO={0,sizeof(info),info};
    int bit_mask,kem=config_id&255;
    OCT_jstring(&SUITE_ID,(char *)"KEM");
    OCT_jint(&SUITE_ID,kem,2);
    LabeledExtract(&PRK,NULL,&SUITE_ID,(char *)"dkp_prk",SEED);
    if (kem==32 || kem==33)
    { // RFC7748
        LabeledExpand(SK,&PRK,&SUITE_ID,(char *)"sk",NULL,GROUP);
        OCT_reverse(SK);
        if (kem==32)
        {
            SK->val[GROUP-1]&=248;  
            SK->val[0]&=127;
            SK->val[0]|=64;
        } else {
            SK->val[GROUP-1]&=252;
            SK->val[0]|=128;
        }

    } else {
        if (kem==18) bit_mask=1;
        else bit_mask=0xFF;
        OCT_clear(SK);
        while (!ECP_ZZZ_IN_RANGE(SK) && counter<256)
        {
            OCT_empty(&INFO);
            OCT_jbyte(&INFO,counter,1);
            LabeledExpand(SK,&PRK,&SUITE_ID,(char *)"candidate",&INFO,GROUP);
            SK->val[0]&=bit_mask;
            counter++;
        }
    }

    ECP_ZZZ_KEY_PAIR_GENERATE(NULL, SK, PK);
    if (kem==32 || kem==33)
        OCT_reverse(PK);
 
    if (counter<256) return 1;
    else return 0;
}

void HPKE_ZZZ_Encap(int config_id,octet *skE,octet *Z,octet *pkE,octet *pkR)
{
    int res,kem;
    char dh[POINT];
    octet DH={0,sizeof(dh),dh};
    char kemcontext[2*POINT];
    octet KEMCONTEXT={0,sizeof(kemcontext),kemcontext};
    kem=config_id&255;

    if (kem==32 || kem==33) 
    {
        OCT_reverse(pkR);
        res=ECP_ZZZ_SVDP_DH(skE, pkR, &DH, 0);
        OCT_reverse(pkR);
        OCT_reverse(&DH);
    } else {
        res=ECP_ZZZ_SVDP_DH(skE, pkR, &DH, 0);
    }

    OCT_copy(&KEMCONTEXT,pkE);
    OCT_joctet(&KEMCONTEXT,pkR);
    ExtractAndExpand(config_id,Z,&DH,&KEMCONTEXT);
}

void HPKE_ZZZ_Decap(int config_id,octet *skR,octet *Z,octet *pkE,octet *pkR)
{
    int res,kem;
    char dh[POINT];
    octet DH={0,sizeof(dh),dh};
    char kemcontext[2*POINT];
    octet KEMCONTEXT={0,sizeof(kemcontext),kemcontext};
    kem=config_id&255;

    if (kem==32 || kem==33)
    {
        OCT_reverse(pkE);
        ECP_ZZZ_SVDP_DH(skR, pkE, &DH, 0);
        OCT_reverse(pkE);
        OCT_reverse(&DH);
    } else {
        ECP_ZZZ_SVDP_DH(skR, pkE, &DH, 0);
    }

    OCT_copy(&KEMCONTEXT,pkE);
    OCT_joctet(&KEMCONTEXT,pkR);

    ExtractAndExpand(config_id,Z,&DH,&KEMCONTEXT);
}

void HPKE_ZZZ_AuthEncap(int config_id,octet *skE,octet *skS,octet *Z,octet *pkE,octet *pkR,octet *pkS)
{
    int res,kem;
    char dh[2*POINT];
    octet DH={0,sizeof(dh),dh};
    char dh1[POINT];
    octet DH1={0,sizeof(dh1),dh1};
    char kemcontext[3*POINT];
    octet KEMCONTEXT={0,sizeof(kemcontext),kemcontext};

    kem=config_id&255;

    if (kem==32 || kem==33)
    {
        OCT_reverse(pkR);
        ECP_ZZZ_SVDP_DH(skE, pkR, &DH,0);
        ECP_ZZZ_SVDP_DH(skS, pkR, &DH1,0);
        OCT_reverse(pkR);
        OCT_reverse(&DH);
        OCT_reverse(&DH1);
    } else {
        ECP_ZZZ_SVDP_DH(skE, pkR, &DH, 0);
        ECP_ZZZ_SVDP_DH(skS, pkR, &DH1,0);
    }

    OCT_joctet(&DH,&DH1);
    OCT_copy(&KEMCONTEXT,pkE);
    OCT_joctet(&KEMCONTEXT,pkR);
    OCT_joctet(&KEMCONTEXT,pkS);

    ExtractAndExpand(config_id,Z,&DH,&KEMCONTEXT);
}

void HPKE_ZZZ_AuthDecap(int config_id,octet *skR,octet *Z,octet *pkE,octet *pkR,octet *pkS)
{
    int res,kem;
    char dh[2*POINT];
    octet DH={0,sizeof(dh),dh};
    char dh1[POINT];
    octet DH1={0,sizeof(dh1),dh1};
    char kemcontext[3*POINT];
    octet KEMCONTEXT={0,sizeof(kemcontext),kemcontext};

    kem=config_id&255;

    if (kem==32 || kem==33) {
        OCT_reverse(pkE);
        OCT_reverse(pkS);
        ECP_ZZZ_SVDP_DH(skR, pkE, &DH,0);
        ECP_ZZZ_SVDP_DH(skR, pkS, &DH1,0);
        OCT_reverse(pkE);
        OCT_reverse(pkS);
        OCT_reverse(&DH);
        OCT_reverse(&DH1);
    } else {
        ECP_ZZZ_SVDP_DH(skR, pkE, &DH, 0);
        ECP_ZZZ_SVDP_DH(skR, pkS, &DH1, 0);
    }
    OCT_joctet(&DH,&DH1);

    OCT_copy(&KEMCONTEXT,pkE);
    OCT_joctet(&KEMCONTEXT,pkR);
    OCT_joctet(&KEMCONTEXT,pkS);

    ExtractAndExpand(config_id,Z,&DH,&KEMCONTEXT);
}

void HPKE_ZZZ_KeySchedule(int config_id,octet *key,octet *nonce,octet *exp_secret,int mode,octet *Z,octet *info,octet *psk,octet *pskID)
{
    char context[1+2*MAX_HASH];
    octet CONTEXT={0,sizeof(context),context};

    char h[MAX_HASH];
    octet H={0,sizeof(h),h};
    char secret_h[MAX_HASH];
    octet secret={0,sizeof(secret_h),secret_h};

    int kem_id=config_id&255;
    int kdf_id=(config_id>>8)&3;
    int aead_id=(config_id>>10)&3;

    char suite_id[10];
    octet SUITE_ID={0,sizeof(suite_id),suite_id};
    OCT_jstring(&SUITE_ID,(char *)"HPKE");
    OCT_jint(&SUITE_ID,kem_id,2);
    OCT_jint(&SUITE_ID,kdf_id,2);
    OCT_jint(&SUITE_ID,aead_id,2);

    OCT_jint(&CONTEXT,mode,1);

    LabeledExtract(&H,NULL,&SUITE_ID,(char *)"psk_id_hash",pskID);
    OCT_joctet(&CONTEXT,&H);
    LabeledExtract(&H,NULL,&SUITE_ID,(char *)"info_hash",info);
    OCT_joctet(&CONTEXT,&H);

    LabeledExtract(&secret,Z,&SUITE_ID,(char *)"secret",psk);
    //LabeledExtract(&H,NULL,&SUITE_ID,(char *)"psk_hash",psk);
    //LabeledExtract(&secret,&H,&SUITE_ID,(char *)"secret",Z);

    LabeledExpand(key,&secret,&SUITE_ID,(char *)"key",&CONTEXT,AESKEY_ZZZ);
    LabeledExpand(nonce,&secret,&SUITE_ID,(char *)"base_nonce",&CONTEXT,12);
    if (exp_secret!=NULL)
        LabeledExpand(exp_secret,&secret,&SUITE_ID,(char *)"exp",&CONTEXT,HASH_TYPE_ZZZ);
}
