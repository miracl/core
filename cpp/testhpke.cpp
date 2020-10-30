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

    OCT_fromHex(&seedE,(char *)"591c66abd531b9c8287cf76ac053efba38d61e994e7f656c30dab6723a8af9ce");
    OCT_fromHex(&seedR,(char *)"8a219e9a42233826f165d2c1036399fa84cfb3bcb93872bc49287dfbe6f1fec9");

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
    OCT_fromHex(&seedE,(char *)"f39542a6e45dc0e6225de671455fea9d918d4fa241c7fb301895a138ce7c2f52");
    OCT_fromHex(&seedR,(char *)"5aa0335739cb3dbafce48d5d9cccd7d499b0727b445522e1408aba8171aabfae");
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
    OCT_fromHex(&seedE,(char *)"0b119a7a132355f3c2a0ad03e10e252d7292fcde480e726d03a81f127d02c050");
    OCT_fromHex(&seedR,(char *)"874ba77ded181cd1cb4f2b4f37386a5b2b86c84c57d2a6d1b93a05fb5edd69f0");
    OCT_fromHex(&seedS,(char *)"2e369ba803dcee17dba926e4252b8dd9a47417a35bcf90f0e55e4af85c83153b");
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

    OCT_fromHex(&seedE,(char *)"96360b18f7822f85f4a0a1dd200d00ab82ad0032b05a1f34cb120a0cbda4865a");
    OCT_fromHex(&seedR,(char *)"48710faa5c8b8b2a4447c9ccf1cc232fb49f6c0fd04b151949757e15b414eea8");
    OCT_fromHex(&seedS,(char *)"dd413f8d2d4b195a7cd0793453d04660203a51db65a57d16c2a720e6d44e526c");
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

    OCT_fromHex(&seedE,(char *)"3fdb7eab3a6a99de8abbcb507be5704ae6a8994008b8a8e6b63fbd97fa8619c66bd8665c22079939f3f63f978c5806802b22ba5bb396da9cf252ee67068bf57461bf");
    OCT_fromHex(&seedR,(char *)"2e99ac709379c7eb15ca068253bbae4dd6297c2397f47a89b8cb3ef4e83f235f83cb1ce3d2f754c47431ff0d0d8d2c429a7b6768d9524c3be60b9fb7749c49cb816b");
   
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

    OCT_fromHex(&seedE,(char *)"ae300665d34d5ab7c0508a94a741ba2cb285966106ba9cefbe1f9c24c3eb626108d0c9ccc8291d90c50c6d04ac181ccd8efc2cc52383eb205637a84d2be5362bf247");
    OCT_fromHex(&seedR,(char *)"dbbda5e5a54ee85cfe076e4746ceb971bed338a7fe0b625d6c7c28b3c82c8128258906a52543655ecb62889e9dbe8feec747d06e4216f88ca4adba200179e0e75276");
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

    OCT_fromHex(&seedE,(char *)"11c0c7337b294452826e14a7f6c9e7981a03c467a08f47a8b478b37f3e9c90266898e3c3f8e84235a6a2837269c84b355d7f5ca133085172a08f00c3857da8a1410b");
    OCT_fromHex(&seedR,(char *)"bb56b3452a80ad82b2d48c19ce76194a198eefdb67040959bc9e479db0682a4b5b46d7d020df66864d374b25deb5927144e3f08f2f9eacdd5f54b8b5c65d91ee211f");
    OCT_fromHex(&seedS,(char *)"bab663b9c05f680f401a494ae8c8714fd95cbcd56a01e9e8194b4b3da863a5e8313d4916dc58f6d3aaa2dafe420ae81b2a6c0075223afc6b13f3734a26ca30da5e38");
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

    OCT_fromHex(&seedE,(char *)"4ebb461a5c46330d6de3a40d19ac499cc206733cf1a4fb3ba922d976aa1c45848668f04a3b5a4845a0d1c83755967d8914a9824fbb8823d161c16c93c51636e1ad89");
    OCT_fromHex(&seedR,(char *)"1ae2f1008c46c7a6e9275b1e29c906475c6bc019b1dfc38cbce68c5233de9d33ba93fe9d7b9ea5beb04f4adc5a3b72238f6e3d904d29eb0680ea240103d3335a3c47");
    OCT_fromHex(&seedS,(char *)"e0f2ada4f2a900fded767dc9868119ee3e4767afac667a780b68e5e2b4d7d363dbf02717ab314369c45f34dcec3de384a65e8453a971ad0353a507f34dc1d5d9b8f5");
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


