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
    OCT_fromHex(&seedE,(char *)"1dacee520c81ade608f4fa3e5ccae0ecedcc7880e3fc6f3e5afd2e4af8396571");
    OCT_fromHex(&seedR,(char *)"0a3367dadc97e200074936b5adedcd5680f30672d1ec7158fdfcb795040ec909");
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
    OCT_fromHex(&seedE,(char *)"7f5b697a87fb11d733e31265410c80b72525637e1b872f9ede00d507c0c534d7");
    OCT_fromHex(&seedR,(char *)"89a6ce4b7b8eb12eebff3864615a2539e3e64fa2d4ab525c18c7bc451a2b5e40");
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
    OCT_fromHex(&seedE,(char *)"f7faa81395a22af1071b5273f9f94db0609ab9a7f4eca54834ea408d97294cc2");
    OCT_fromHex(&seedR,(char *)"a10d107753f85bf7128768693e53129fb44a281ea4849a86231632db48dbb28c");
    OCT_fromHex(&seedS,(char *)"e1257ce84dd98bca7847d75661b658cd2cc6c4efbf9e989a40f390720b57d409");
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

    OCT_fromHex(&seedE,(char *)"e98713f7e24e7025fb7abddfff317d865ad5f4c3b6be22cda48dbcdde6151a68");
    OCT_fromHex(&seedR,(char *)"b281654cdb2cd6a9670c3035eaa702736a951585a938381198f7d29085ca7f3a");
    OCT_fromHex(&seedS,(char *)"ea95c4d3c5e7868af29fc0cb0841485dead8d54b57ee1b8ed31a153295454835");
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
    OCT_fromHex(&seedE,(char *)"bb8f0c6c0d81a0a54d6084e3d7d8001875ce2b4831400075246ddeabc3f62d065d91fa8f2ca0cfee3a94f1fa6f43f53a2e39241ccd5ad9f0d889aba103e78286ca17");
    OCT_fromHex(&seedR,(char *)"28b1403adcf3d2bba7d7c733df4ef8ab9f4c2cf92c840beb7de3fd139611a95be850589a49825450c033b2e1f6cb66127c83827d5464fca1e427c2bb31bf18e857e2");

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

    OCT_fromHex(&seedE,(char *)"bd12d21f5a36063ec0242aa26f22144aa7d24c3cc2e6a1805868250d8e9ce6fb2a04f8852cd6ec3241249fcf262b8f245d179636fcfbf275a75539cfc756a4dd3f53");
    OCT_fromHex(&seedR,(char *)"e9a016952866c33da399e64ecd629cf32826285698726f2c22debd4799b8fac112ac1a546ce39452406396a389fb10f51036c7022dc1cdde177b54d005731fb79ce4");
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

    OCT_fromHex(&seedE,(char *)"0e5ec115c172744e793ef333abfd5d9838fd31eef595f0b77a2f5623404aa42f0e77198c2ac78defeae75cc79cf5dc5c7587b6eda733e48f3b17d5e3496482f19cdb");
    OCT_fromHex(&seedR,(char *)"3f3c12492eee6f1f8029a89002b6539ea9754b48412c14d2fe94368cf1fa473f0171cdb1117aaa447ad6914bfb794ead14a9cd22cd83caa22bd905bd7d614d401ddc");
    OCT_fromHex(&seedS,(char *)"02d1581b120d0415bda1fb2cbe6a3c81edf28209f0fc1460745ad9018747c31e6d42a36ed1734214312311db80bb3f88c2da6ea7624cf2915d32f2993ebeb92c2934");
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

    OCT_fromHex(&seedE,(char *)"c393a0a0cab3313f0526d72b8bef55b26a156493263fa1a653069ed5f2f3637f371b15704778c89a6e01c3fd958dfee5f85613e2d4b2eb17a152810690d8252df44b");
    OCT_fromHex(&seedR,(char *)"e7fa07c9938a3d9d883d256abd7fb5777ed7c4195a8d244c0e682fd1a08fdcce58ab1aa7ba31b1fa120f03d9df438509a1e31992506dbc196ac4489079b3b64bea9b");
    OCT_fromHex(&seedS,(char *)"403e8d5ff0155c71232a887f542df221c385956eaca3acceffdc4681b99c7e9613f90ba03b0c03c0c57a90b2dd31effc6278ab6dc059f3e5389dff6495bda12b4da4");
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


