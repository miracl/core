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

/* test driver and function exerciser for HPKE API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hpke_C25519.h"
#include "hpke_NIST521.h"

using namespace core;

int hpke_C25519()
{
    using namespace C25519;

    int res=0;
    int config_id=0x520; // 01|01|00100000 = 1, 1, 32
    int i,mode;
    char info[32];

    char seede[EGS_C25519];
    char seedr[EGS_C25519];
    char seeds[EGS_C25519];
    octet seedE = {0, sizeof(seede), seede};
    octet seedR = {0, sizeof(seedr), seedr};
    octet seedS = {0, sizeof(seeds), seeds};

    char skr[EGS_C25519];
    char sks[EGS_C25519];
    char ske[EGS_C25519];
    char psks[32];
    char pskid[32];
    char pkr[EFS_C25519];
    char pks[EFS_C25519];
    char pke[EFS_C25519];
    char z[72];
    char plain[80];
    char cipher[80];
    char aad[32];
    char tag[32];
    char exporter_secret[128];
    
    octet INFO = {0, sizeof(info), info};
    octet skR = {0, sizeof(skr), skr};
    octet skS = {0, sizeof(sks), sks};
    octet skE = {0, sizeof(ske), ske};
    octet psk = {0, sizeof(psks), psks};
    octet pskID = {0,sizeof(pskid), pskid};
    octet pkR = {0, sizeof(pkr), pkr};
    octet pkS = {0, sizeof(pks), pks};
    octet pkE = {0, sizeof(pke), pke};
    octet Z = {0, sizeof(z), z};
    octet PLAIN = {0,sizeof(plain),plain};
    octet CIPHER = {0,sizeof(cipher),cipher};
    octet AAD = {0,sizeof(aad),aad};
    octet TAG = {0,sizeof(tag),tag};
    octet EXPORTER_SECRET = {0,sizeof(exporter_secret),exporter_secret};      
              
    OCT_fromHex(&INFO,(char *)"4f6465206f6e2061204772656369616e2055726e");
    OCT_fromHex(&psk,(char *)"0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82");
    OCT_fromHex(&pskID,(char *)"456e6e796e20447572696e206172616e204d6f726961");
    OCT_fromHex(&PLAIN,(char *)"4265617574792069732074727574682c20747275746820626561757479");
    OCT_fromHex(&AAD,(char *)"436f756e742d30");

    char key[32];
    char nonce[12];
    octet KEY = {0, sizeof(key), key};
    octet NONCE = {0, sizeof(nonce), nonce};

// Mode 0

    mode=0;
    printf("\nMode 0\n");

    OCT_fromHex(&seedE,(char *)"6305de86b3cec022fae6f2f2d2951f0f90c8662112124fd62f17e0a99bdbd08e");
    OCT_fromHex(&seedR,(char *)"6d9014e4609687b0a3670a22f2a14eac5ae6ad8c0beb62fb3ecb13dc8ebf5e06");

    DeriveKeyPair(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair(config_id,&skR,&pkR,&seedR);

    HPKE_Encap(config_id,&skE,&Z,&pkE,&pkR);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Secret= "); OCT_output(&Z);

    HPKE_Decap(config_id,&skR,&Z,&pkE,&pkR);
    printf("Decapsulated Secret= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,NULL,NULL);
    
    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET);  

    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 1
    mode=1;
    printf("\nMode 1\n");
    OCT_fromHex(&seedE,(char *)"abd63dfd2fb9ccf8789cae5a6aff91e07f9f5925c27f005e702bf956b0000a85");
    OCT_fromHex(&seedR,(char *)"654e8b44e8e29fc75f3beadf7f28dc065e38a53c1a731e15f2d46fd6130574da");
    DeriveKeyPair(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair(config_id,&skR,&pkR,&seedR);
    
    HPKE_Encap(config_id,&skE,&Z,&pkE,&pkR);
    
    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_Decap(config_id,&skR,&Z,&pkE,&pkR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,&psk,&pskID);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET);    

    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 2
    mode=2;
    printf("\nMode 2\n");
    OCT_fromHex(&seedE,(char *)"42d691088397246b00e9d9ce8f5406a317433558dc28132e02618970005d02fd");
    OCT_fromHex(&seedR,(char *)"fb953f486ef5f7a1ceddff40bffe02b857c8af9d611966e417a24d6efa7c9d1c");
    OCT_fromHex(&seedS,(char *)"131aa907c85b05726e7a058b064bf29cb2cb72a2afbffbd8076a884291f3143e");
    DeriveKeyPair(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair(config_id,&skR,&pkR,&seedR);
    DeriveKeyPair(config_id,&skS,&pkS,&seedS);


    HPKE_AuthEncap(config_id,&skE,&skS,&Z,&pkE,&pkR,&pkS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_AuthDecap(config_id,&skR,&Z,&pkE,&pkR,&pkS);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET);  
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 3
    mode=3;
    printf("\nMode 3\n");

    OCT_fromHex(&seedE,(char *)"0c3a14fe896a7166f4d5e2a21c903b69f9ca71783290ca10f1b8c5eb258149be");
    OCT_fromHex(&seedR,(char *)"b759021868fba28c1ddd509eaac450a896ab0f5edffead0a019fecb574950d64");
    OCT_fromHex(&seedS,(char *)"2e7219703b6659698e4c2d141d13e0092df7039212db9c97e347c7d2c0aee239");
    DeriveKeyPair(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair(config_id,&skR,&pkR,&seedR);
    DeriveKeyPair(config_id,&skS,&pkS,&seedS);

    HPKE_AuthEncap(config_id,&skE,&skS,&Z,&pkE,&pkR,&pkS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_AuthDecap(config_id,&skR,&Z,&pkE,&pkR,&pkS);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,&psk,&pskID);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET);  
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

    return res;
}

int hpke_NIST521()
{
    using namespace NIST521;

    int res=0;
    int config_id=0xB12; // 10|11|00010010 = 2, 3, 18
    int i,mode;
    char info[64];

    char seede[EGS_NIST521];
    char seedr[EGS_NIST521];
    char seeds[EGS_NIST521];
    octet seedE = {0, sizeof(seede), seede};
    octet seedR = {0, sizeof(seedr), seedr};
    octet seedS = {0, sizeof(seeds), seeds};

    char skr[EGS_NIST521];
    char sks[EGS_NIST521];
    char ske[EGS_NIST521];
    char psks[64];
    char pskid[64];
    char pkr[2*EFS_NIST521+1];
    char pks[2*EFS_NIST521+1];
    char pke[2*EFS_NIST521+1];
    char z[256];
    char plain[80];
    char cipher[80];
    char aad[32];
    char tag[32];
    char exporter_secret[128];
    
    octet INFO = {0, sizeof(info), info};
    octet skR = {0, sizeof(skr), skr};
    octet skS = {0, sizeof(sks), sks};
    octet skE = {0, sizeof(ske), ske};
    octet psk = {0, sizeof(psks), psks};
    octet pskID = {0,sizeof(pskid), pskid};
    octet pkR = {0, sizeof(pkr), pkr};
    octet pkS = {0, sizeof(pks), pks};
    octet pkE = {0, sizeof(pke), pke};
    octet Z = {0, sizeof(z), z};
    octet PLAIN = {0,sizeof(plain),plain};
    octet CIPHER = {0,sizeof(cipher),cipher};
    octet AAD = {0,sizeof(aad),aad};
    octet TAG = {0,sizeof(tag),tag};
    octet EXPORTER_SECRET = {0,sizeof(exporter_secret),exporter_secret};      

    OCT_fromHex(&INFO,(char *)"4f6465206f6e2061204772656369616e2055726e");
    OCT_fromHex(&psk,(char *)"0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82");
    OCT_fromHex(&pskID,(char *)"456e6e796e20447572696e206172616e204d6f726961");
    OCT_fromHex(&PLAIN,(char *)"4265617574792069732074727574682c20747275746820626561757479");
    OCT_fromHex(&AAD,(char *)"436f756e742d30");

    char key[64];
    char nonce[24];
    octet KEY = {0, sizeof(key), key};
    octet NONCE = {0, sizeof(nonce), nonce};

// Mode 0

    mode=0;
    printf("\nMode 0\n");

    OCT_fromHex(&seedE,(char *)"ea39fec1941c5f516e19533f40d415c65fde023c10c559f3845e71ffccea478101573d069cc67874d5b2aba6a22eb51cdd689836b7e9cabbb4469c57947db7316fa7");
    OCT_fromHex(&seedR,(char *)"8249fd42416aba5b0d51dcd3548d774ae172148cbba1519107c5d84a160225441a9c018fdf3b9ffc2c41c1c62e29208d5165a59e7f14fe93b4f911cbbebda1904391");
   
    DeriveKeyPair(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair(config_id,&skR,&pkR,&seedR);

    HPKE_Encap(config_id,&skE,&Z,&pkE,&pkR);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_Decap(config_id,&skR,&Z,&pkE,&pkR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET); 
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 1
    mode=1;
    printf("\nMode 1\n");

    OCT_fromHex(&seedE,(char *)"b563555965facaa37a5e754cf2e50193953e1d527e61637e521df1868354799258f0d15d8807750cea08e9b6a358d2440ab06725861b237fb69973cf41e802434433");
    OCT_fromHex(&seedR,(char *)"2be954cd856c0c548ec6de490c821be20c4fc9610b5dba1831c698045fa01dbceb49f8324bae26e5d1ee62db3246d65492812e2c539b96bc580d46c247304adfc55e");
    DeriveKeyPair(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair(config_id,&skR,&pkR,&seedR);
    
    HPKE_Encap(config_id,&skE,&Z,&pkE,&pkR);
    
    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_Decap(config_id,&skR,&Z,&pkE,&pkR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,&psk,&pskID);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET); 
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 2
    mode=2;
    printf("\nMode 2\n");

    OCT_fromHex(&seedE,(char *)"c9621b9ef899275dc970606a2b0806fe860f62d539f3ee618a9409009b8ae154bc4acf495dd9fa8f850c4dca82b923b42270d7b16ed343c7e86e3036c88d0d7d77ee");
    OCT_fromHex(&seedR,(char *)"ef805c20cda1fa06e06cfb968ad68c748a3dd94337f7357ca0060a382a84fa5de6df3e3216f886957694547264d5bc63450cfdcc4d2b33fc8ebf8d7c708f8b5e4bb5");
    OCT_fromHex(&seedS,(char *)"d8779e14425887ebb21b1952b1a0b77842830aef910724b082807dfebc8ec309b4969da762369e77834593970215b85510c9a0347ff14c8583aae7c9c2208275b740");
    DeriveKeyPair(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair(config_id,&skR,&pkR,&seedR);
    DeriveKeyPair(config_id,&skS,&pkS,&seedS);
 
    HPKE_AuthEncap(config_id,&skE,&skS,&Z,&pkE,&pkR,&pkS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_AuthDecap(config_id,&skR,&Z,&pkE,&pkR,&pkS);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET); 
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 3
    mode=3;
    printf("\nMode 3\n");

    OCT_fromHex(&seedE,(char *)"d7537fd470c0beece615e26dd109922460292e973127cb4e22da16c0756fc336224e07dbecdf36edd144ebcd82aece3db52f814a33a639b5e7c964b27f6e3195cd73");
    OCT_fromHex(&seedR,(char *)"f0858f5e1865db4fe45dc3274bcd273a29088d80f9203a16ec1210e3d81dd50a99f15c427d547fea55593e2ef834beb5f80c536fdd2881a8943c05488a371a3c988d");
    OCT_fromHex(&seedS,(char *)"1e8d0026273feb61537181872e03ed2c7756f0ed1c4bb9ecd159614c2afdcaacc5fcf70f6d30d7ea6760c98a1ce1138a82497eb72461ca5da50c8729d431de53857f");
    DeriveKeyPair(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair(config_id,&skR,&pkR,&seedR);
    DeriveKeyPair(config_id,&skS,&pkS,&seedS);

    HPKE_AuthEncap(config_id,&skE,&skS,&Z,&pkE,&pkR,&pkS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_AuthDecap(config_id,&skR,&Z,&pkE,&pkR,&pkS);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,&psk,&pskID);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET); 
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

    return res;
}

int main()
{
    printf("\n%d bit build", CHUNK);

    printf("\nTesting HPKE for curve C25519\n");
    hpke_C25519();

    printf("\nTesting HPKE for curve NIST521\n");
    hpke_NIST521();
}


