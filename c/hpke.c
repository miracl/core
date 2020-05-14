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
#define FIELD EFS_ZZZ 
#define POINT (2*EFS_ZZZ+1)
#define MAX_HASH HASH_TYPE_ZZZ
#define MAX_LABEL 20                // may need adjustment

static void LabeledExtract(octet *PRK,octet *SALT,char *label,octet *IKM)
{
    char likm[8+MAX_LABEL+POINT];
    octet LIKM={0,sizeof(likm),likm};
    OCT_jstring(&LIKM,(char *)"RFCXXXX ");
    OCT_jstring(&LIKM,label);
    if (IKM!=NULL)
        OCT_joctet(&LIKM,IKM);
    HKDF_Extract(MC_SHA2,HASH_TYPE_ZZZ,PRK,SALT,&LIKM);
}

static void LabeledExpand(octet *OKM,octet *PRK,char *label,octet *INFO,int L)
{
    char linfo[10+MAX_LABEL+3*POINT];
    octet LINFO={0,sizeof(linfo),linfo};
    OCT_jint(&LINFO,L,2);    
    OCT_jstring(&LINFO,(char *)"RFCXXXX ");
    OCT_jstring(&LINFO,label);
    OCT_joctet(&LINFO,INFO);
    HKDF_Expand(MC_SHA2,HASH_TYPE_ZZZ,OKM,L,PRK,&LINFO);
}

static void ExtractAndExpand(octet *OKM,octet *DH,octet *CONTEXT)
{
    char zeros[MAX_HASH];
    octet ZEROS={0,sizeof(zeros),zeros};
    char prk[MAX_HASH];
    octet PRK={0,sizeof(prk),prk};
    OCT_jbyte(&ZEROS,0,HASH_TYPE_ZZZ);
    LabeledExtract(&PRK,&ZEROS,(char *)"dh",DH);
    LabeledExpand(OKM, &PRK,(char *)"prk",CONTEXT,HASH_TYPE_ZZZ);  
}

void HPKE_ZZZ_Encap(int config_id,octet *SKE,octet *Z,octet *pkE,octet *pkR)
{
    int res,kem;
    char ske[GROUP];
    octet skE={0,sizeof(ske),ske};
    char dh[FIELD];
    octet DH={0,sizeof(dh),dh};
    char kemcontext[2*POINT];
    octet KEMCONTEXT={0,sizeof(kemcontext),kemcontext};
    kem=config_id&255;

    OCT_copy(&skE,SKE);
    if (kem==32 || kem==33) {
        OCT_reverse(&skE);
        if (kem==32)
        {
            skE.val[GROUP-1]&=248;  
            skE.val[0]&=127;
            skE.val[0]|=64;
        } else {
            skE.val[GROUP-1]&=252;
            skE.val[0]|=128;
        }
    }
    
    res=ECP_ZZZ_KEY_PAIR_GENERATE(NULL, &skE, pkE);
    if (kem==32 || kem==33)
    {
        OCT_reverse(pkR);
    }
    res=ECP_ZZZ_SVDP_DH(&skE, pkR, &DH);
    if (kem==32 || kem==33) {
        OCT_reverse(pkR);
        OCT_reverse(pkE);
        OCT_reverse(&DH);
    }
    OCT_copy(&KEMCONTEXT,pkE);
    OCT_joctet(&KEMCONTEXT,pkR);

    ExtractAndExpand(Z,&DH,&KEMCONTEXT);
}

void HPKE_ZZZ_Decap(int config_id,octet *Z,octet *pkE,octet *SKR)
{
    int res,kem;
    char skr[GROUP];
    octet skR={0,sizeof(skr),skr};
    char dh[FIELD];
    octet DH={0,sizeof(dh),dh};
    kem=config_id&255;
    char pkr[POINT];
    octet pkR={0,sizeof(pkr),pkr};
    char kemcontext[2*POINT];
    octet KEMCONTEXT={0,sizeof(kemcontext),kemcontext};

    OCT_copy(&skR,SKR);

    if (kem==32 || kem==33) {
        OCT_reverse(&skR);
        OCT_reverse(pkE);
        if (kem==32)
        {
            skR.val[GROUP-1]&=248;
            skR.val[0]&=127;
            skR.val[0]|=64;
        } else {
            skR.val[GROUP-1]&=252;
            skR.val[0]|=128;
        }       
    }
    ECP_ZZZ_SVDP_DH(&skR, pkE, &DH);
    if (kem==32 || kem==33) {
        OCT_reverse(pkE);
        OCT_reverse(&DH);
    }
    ECP_ZZZ_KEY_PAIR_GENERATE(NULL, &skR, &pkR);
    if (kem==32 || kem==33) {
        OCT_reverse(&pkR);
    }
    OCT_copy(&KEMCONTEXT,pkE);
    OCT_joctet(&KEMCONTEXT,&pkR);

    ExtractAndExpand(Z,&DH,&KEMCONTEXT);
}

void HPKE_ZZZ_AuthEncap(int config_id,octet *SKE,octet *Z,octet *pkE,octet *pkR,octet *SKS)
{
    int res,kem;

    char dh[2*FIELD];
    octet DH={0,sizeof(dh),dh};
    char dh1[FIELD];
    octet DH1={0,sizeof(dh1),dh1};
    char pks[POINT];
    octet pkS={0,sizeof(pks),pks};
    char ske[GROUP];
    octet skE={0,sizeof(ske),ske};
    char sks[GROUP];
    octet skS={0,sizeof(sks),sks};
    char kemcontext[3*POINT];
    octet KEMCONTEXT={0,sizeof(kemcontext),kemcontext};

    kem=config_id&255;

    OCT_copy(&skE,SKE);
    OCT_copy(&skS,SKS);
    if (kem==32 || kem==33) {
        OCT_reverse(&skE);
        OCT_reverse(&skS);
        if (kem==32)
        {
            skE.val[GROUP-1]&=248;
            skE.val[0]&=127;
            skE.val[0]|=64;
            skS.val[GROUP-1]&=248;
            skS.val[0]&=127;
            skS.val[0]|=64;
        } else {
            skE.val[GROUP-1]&=252;
            skE.val[0]|=128;
            skS.val[GROUP-1]&=252;
            skS.val[0]|=128;
        }
    }

    ECP_ZZZ_KEY_PAIR_GENERATE(NULL, &skE, pkE);

    if (kem==32 || kem==33)
        OCT_reverse(pkR);

    ECP_ZZZ_SVDP_DH(&skE, pkR, &DH);
    ECP_ZZZ_SVDP_DH(&skS, pkR, &DH1);

    if (kem==32 || kem==33) {
        OCT_reverse(&DH);
        OCT_reverse(&DH1);
    }

    OCT_joctet(&DH,&DH1);

    ECP_ZZZ_KEY_PAIR_GENERATE(NULL, &skS, &pkS);  
    
    if (kem==32 || kem==33) {
        OCT_reverse(pkR);
        OCT_reverse(pkE);
        OCT_reverse(&pkS);
    }

    OCT_copy(&KEMCONTEXT,pkE);
    OCT_joctet(&KEMCONTEXT,pkR);
    OCT_joctet(&KEMCONTEXT,&pkS);

    ExtractAndExpand(Z,&DH,&KEMCONTEXT);
}

void HPKE_ZZZ_AuthDecap(int config_id,octet *Z,octet *pkE,octet *SKR,octet *pkS)
{
    int res,kem;
    char skr[GROUP];
    octet skR={0,sizeof(skr),skr};

    char dh[2*FIELD];
    octet DH={0,sizeof(dh),dh};
    char dh1[FIELD];
    octet DH1={0,sizeof(dh1),dh1};
    char pkr[POINT];
    octet pkR={0,sizeof(pkr),pkr};
    char kemcontext[3*POINT];
    octet KEMCONTEXT={0,sizeof(kemcontext),kemcontext};


    OCT_copy(&skR,SKR);
    kem=config_id&255;

    if (kem==32 || kem==33) {
        OCT_reverse(&skR);
        OCT_reverse(pkE);
        OCT_reverse(pkS);
        if (kem==32)
        {
            skR.val[GROUP-1]&=248;
            skR.val[0]&=127;
            skR.val[0]|=64;
        } else {
            skR.val[GROUP-1]&=252;
            skR.val[0]|=128;
        }
    }

    ECP_ZZZ_SVDP_DH(&skR, pkE, &DH);
    ECP_ZZZ_SVDP_DH(&skR, pkS, &DH1);
    if (kem==32 || kem==33) {
        OCT_reverse(&DH);
        OCT_reverse(&DH1);
    }
    OCT_joctet(&DH,&DH1);

    ECP_ZZZ_KEY_PAIR_GENERATE(NULL, &skR, &pkR); 

    if (kem==32 || kem==33) {
        OCT_reverse(pkE);
        OCT_reverse(&pkR);
        OCT_reverse(pkS);
    }

    OCT_copy(&KEMCONTEXT,pkE);
    OCT_joctet(&KEMCONTEXT,&pkR);
    OCT_joctet(&KEMCONTEXT,pkS);

    ExtractAndExpand(Z,&DH,&KEMCONTEXT);
}

void HPKE_ZZZ_KeySchedule(int config_id,octet *key,octet *nonce,octet *exp_secret,int mode,octet *Z,octet *info,octet *psk,octet *pskID)
{
    char context[7+2*MAX_HASH];
    octet CONTEXT={0,sizeof(context),context};

    char h[MAX_HASH];
    octet H={0,sizeof(h),h};
    char secret_h[MAX_HASH];
    octet secret={0,sizeof(secret_h),secret_h};
    char zeros[MAX_HASH];
    octet ZEROS={0,sizeof(zeros),zeros};

    int kem_id=config_id&255;
    int kdf_id=(config_id>>8)&3;
    int aead_id=(config_id>>10)&3;

    OCT_jint(&CONTEXT,kem_id,2);
    OCT_jint(&CONTEXT,kdf_id,2);
    OCT_jint(&CONTEXT,aead_id,2);
    OCT_jint(&CONTEXT,mode,1);


    OCT_jbyte(&ZEROS,0,HASH_TYPE_ZZZ);
    LabeledExtract(&H,&ZEROS,(char *)"pskID_hash",pskID);
    OCT_joctet(&CONTEXT,&H);
    LabeledExtract(&H,&ZEROS,(char *)"info",info);
    OCT_joctet(&CONTEXT,&H);
  
//printf("context= "); OCT_output(&CONTEXT);

    if (psk==NULL)
        LabeledExtract(&H,&ZEROS,(char *)"psk_hash",&ZEROS);
    else
        LabeledExtract(&H,&ZEROS,(char *)"psk_hash",psk);
    LabeledExtract(&secret,&H,(char *)"zz",Z);

//printf("secret== "); OCT_output(&secret);

    LabeledExpand(key,&secret,(char *)"key",&CONTEXT,AESKEY_ZZZ);
    LabeledExpand(nonce,&secret,(char *)"nonce",&CONTEXT,12);
    if (exp_secret!=NULL)
        LabeledExpand(exp_secret,&secret,(char *)"exp",&CONTEXT,HASH_TYPE_ZZZ);
}
