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
#include "randapi.h"


int hpke_C25519()
{
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
    OCT_fromHex(&seedE,(char *)"7268600d403fce431561aef583ee1613527cff655c1343f29812e66706df3234");
    OCT_fromHex(&seedR,(char *)"6db9df30aa07dd42ee5e8181afdb977e538f5e1fec8a06223f33f7013e525037");
    DeriveKeyPair_C25519(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair_C25519(config_id,&skR,&pkR,&seedR);

    HPKE_C25519_Encap(config_id,&skE,&Z,&pkE,&pkR);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_Decap(config_id,&skR,&Z,&pkE,&pkR);
    printf("Decapsulated Z= "); OCT_output(&Z);


    HPKE_C25519_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET);  

    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 1
    mode=1;
    printf("\nMode 1\n");
    OCT_fromHex(&seedE,(char *)"78628c354e46f3e169bd231be7b2ff1c77aa302460a26dbfa15515684c00130b");
    OCT_fromHex(&seedR,(char *)"d4a09d09f575fef425905d2ab396c1449141463f698f8efdb7accfaff8995098");
    DeriveKeyPair_C25519(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair_C25519(config_id,&skR,&pkR,&seedR);
    
    HPKE_C25519_Encap(config_id,&skE,&Z,&pkE,&pkR);
    
    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_Decap(config_id,&skR,&Z,&pkE,&pkR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,&psk,&pskID);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET);    

    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 2
    mode=2;
    printf("\nMode 2\n");
    OCT_fromHex(&seedE,(char *)"6e6d8f200ea2fb20c30b003a8b4f433d2f4ed4c2658d5bc8ce2fef718059c9f7");
    OCT_fromHex(&seedR,(char *)"f1d4a30a4cef8d6d4e3b016e6fd3799ea057db4f345472ed302a67ce1c20cdec");
    OCT_fromHex(&seedS,(char *)"94b020ce91d73fca4649006c7e7329a67b40c55e9e93cc907d282bbbff386f58");
    DeriveKeyPair_C25519(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair_C25519(config_id,&skR,&pkR,&seedR);
    DeriveKeyPair_C25519(config_id,&skS,&pkS,&seedS);

    HPKE_C25519_AuthEncap(config_id,&skE,&skS,&Z,&pkE,&pkR,&pkS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_AuthDecap(config_id,&skR,&Z,&pkE,&pkR,&pkS);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET);  
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 3
    mode=3;
    printf("\nMode 3\n");

    OCT_fromHex(&seedE,(char *)"4303619085a20ebcf18edd22782952b8a7161e1dbae6e46e143a52a96127cf84");
    OCT_fromHex(&seedR,(char *)"4b16221f3b269a88e207270b5e1de28cb01f847841b344b8314d6a622fe5ee90");
    OCT_fromHex(&seedS,(char *)"62f77dcf5df0dd7eac54eac9f654f426d4161ec850cc65c54f8b65d2e0b4e345");
    DeriveKeyPair_C25519(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair_C25519(config_id,&skR,&pkR,&seedR);
    DeriveKeyPair_C25519(config_id,&skS,&pkS,&seedS);

    HPKE_C25519_AuthEncap(config_id,&skE,&skS,&Z,&pkE,&pkR,&pkS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_AuthDecap(config_id,&skR,&Z,&pkE,&pkR,&pkS);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,&psk,&pskID);

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
    int res=0;
    int config_id=0xB12; // 10|11|00011000 = 2, 2, 18
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
    OCT_fromHex(&seedE,(char *)"7f06ab8215105fc46aceeb2e3dc5028b44364f960426eb0d8e4026c2f8b5d7e7a986688f1591abf5ab753c357a5d6f0440414b4ed4ede71317772ac98d9239f70904");
    OCT_fromHex(&seedR,(char *)"2ad954bbe39b7122529f7dde780bff626cd97f850d0784a432784e69d86eccaade43b6c10a8ffdb94bf943c6da479db137914ec835a7e715e36e45e29b587bab3bf1");

    DeriveKeyPair_NIST521(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair_NIST521(config_id,&skR,&pkR,&seedR);

    HPKE_NIST521_Encap(config_id,&skE,&Z,&pkE,&pkR);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_Decap(config_id,&skR,&Z,&pkE,&pkR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET); 
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 1
    mode=1;
    printf("\nMode 1\n");

    OCT_fromHex(&seedE,(char *)"f3ebfa9a69a924e672114fcd9e06fa9559e937f7eccce4181a2b506df53dbe514be12f094bb28e01de19dd345b4f7ede5ad7eaa6b9c3019592ec68eaae9a14732ce0");
    OCT_fromHex(&seedR,(char *)"a2a2458705e278e574f835effecd18232f8a4c459e7550a09d44348ae5d3b1ea9d95c51995e657ad6f7cae659f5e186126a471c017f8f5e41da9eba74d4e0473e179");
    DeriveKeyPair_NIST521(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair_NIST521(config_id,&skR,&pkR,&seedR);
    
    HPKE_NIST521_Encap(config_id,&skE,&Z,&pkE,&pkR);
    
    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_Decap(config_id,&skR,&Z,&pkE,&pkR);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,&psk,&pskID);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET); 
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 2
    mode=2;
    printf("\nMode 2\n");

    OCT_fromHex(&seedE,(char *)"fe1c589c2a05893895a537f38c7cb4300b5a7e8fef3d6ccb8f07a498029c61e90262e009dc254c7f6235f9c6b2fd6aeff0a714db131b09258c16e217b7bd2aa619b0");
    OCT_fromHex(&seedR,(char *)"8feea0438481fc0ecd470d6adfcda334a759c6b8650452c5a5dd9b2dd2cc9be33d2bb7ee64605fc07ab4664a58bb9a8de80defe510b6c97d2daf85b92cd4bb0a66bf");
    OCT_fromHex(&seedS,(char *)"2f66a68b85ef04822b054ef521838c00c64f8b6226935593b69e13a1a2461a4f1a74c10c836e87eed150c0db85d4e4f506cbb746149befac6f5c07dc48a615ef92db");
    DeriveKeyPair_NIST521(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair_NIST521(config_id,&skR,&pkR,&seedR);
    DeriveKeyPair_NIST521(config_id,&skS,&pkS,&seedS);

    HPKE_NIST521_AuthEncap(config_id,&skE,&skS,&Z,&pkE,&pkR,&pkS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_AuthDecap(config_id,&skR,&Z,&pkE,&pkR,&pkS);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,NULL,NULL);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET); 
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);

// Mode 3
    mode=3;
    printf("\nMode 3\n");

    OCT_fromHex(&seedE,(char *)"54272797b1fbc128a6967ff1fd606e0c67868f7762ce1421439cbc9e90ce1b28d566e6c2acbce712e48eebf236696eb680849d6873e9959395b2931975d61d38bd6c");
    OCT_fromHex(&seedR,(char *)"3db434a8bc25b27eb0c590dc64997ab1378a99f52b2cb5a5a5b2fa540888f6c0f09794c654f4468524e040e6b4eca2c9dcf229f908b9d318f960cc9e9baa92c5eee6");
    OCT_fromHex(&seedS,(char *)"65d523d9b37e1273eb25ad0527d3a7bd33f67208dd1666d9904c6bc04969ae5831a8b849e7ff642581f2c3e56be84609600d3c6bbdaded3f6989c37d2892b1e978d5");
    DeriveKeyPair_NIST521(config_id,&skE,&pkE,&seedE);
    DeriveKeyPair_NIST521(config_id,&skR,&pkR,&seedR);
    DeriveKeyPair_NIST521(config_id,&skS,&pkS,&seedS);

    HPKE_NIST521_AuthEncap(config_id,&skE,&skS,&Z,&pkE,&pkR,&pkS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_AuthDecap(config_id,&skR,&Z,&pkE,&pkR,&pkS);
    printf("Decapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_KeySchedule(config_id,&KEY,&NONCE,&EXPORTER_SECRET,mode,&Z,&INFO,&psk,&pskID);

    printf("Key=   "); OCT_output(&KEY);
    printf("Nonce= "); OCT_output(&NONCE);
    printf("Exporter Secret= "); OCT_output(&EXPORTER_SECRET); 
    
    AES_GCM_ENCRYPT(&KEY,&NONCE,&AAD,&PLAIN,&CIPHER,&TAG);

    printf("Cipher= "); OCT_output(&CIPHER);
    printf("Tag= "); OCT_output(&TAG);
}

int main()
{
    printf("\n%d bit build", CHUNK);

    printf("\nTesting HPKE for curve C25519\n");
    hpke_C25519();

    printf("\nTesting HPKE for curve NIST521\n");
    hpke_NIST521();
}


