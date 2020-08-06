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
    OCT_fromHex(&psk,(char *)"5db3b80a81cb63ca59470c83414ef70a");
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
    OCT_fromHex(&seedE,(char *)"a77ae3e14cc2ec9e603a9049423d48e66a5e3139e896e95cf19919430657adc7");
    OCT_fromHex(&seedR,(char *)"1289f0db1d8f68d0c531b5e53a40911a2a2347059355d7c267717033fef2b08c");
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
    OCT_fromHex(&seedE,(char *)"0fa1407ccee05de0cceb2f2d2381d2df0602dbd43be90eefd288ce4ad0b3ba32");
    OCT_fromHex(&seedR,(char *)"326ee379f778718e6cb343f55668fbb9d0098ba0503cd4414a8f1ce252605c39");
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
    OCT_fromHex(&seedE,(char *)"02900cb4856b5f222293a9bd7bda2f1f81c562dc3002336ad1c39f6572402b7d");
    OCT_fromHex(&seedR,(char *)"518df90f0f5044ce653180c700e4902d37a7ba1cd23482a76e18b300fecaac4e");
    OCT_fromHex(&seedS,(char *)"262a05ad0c08030cdbbaafc03d64f33b95bf8089f216c62ac39b72064a4b4dcb");
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

    OCT_fromHex(&seedE,(char *)"c1d1028243a951dbf6469025f3a1304407b08fb932104e61c7aab42ab4f1995c");
    OCT_fromHex(&seedR,(char *)"02a965d8f53bbdcc11cc618d4f31f69277500b75959ca97fd533058315511d1b");
    OCT_fromHex(&seedS,(char *)"e9c09a3e50073935e75d3846007a26088a93ebf58ad0bb30ad6c42a9d4d2419e");
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
    OCT_fromHex(&psk,(char *)"5db3b80a81cb63ca59470c83414ef70a");
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
    OCT_fromHex(&seedE,(char *)"1ed3768f499b5b3c2beda3166528b649d4b117a0bd450f0e9e19815c2597d1777ac67ea367415fb28c8819c94b383a0a8a15c9f03b4835330e3e6c8bc8319202e473");
    OCT_fromHex(&seedR,(char *)"62641514bccd2858f3d6513305288d6ca0e443f00a86eb33ccd519d1803aebc5d07bbad0e1013ce61d9c9d713b3c90c8e79a1af01d6c69750f67cbbd1d9d4afeedfa");
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

    OCT_fromHex(&seedE,(char *)"64463def238f309f1e9d1f28c15dc126cffa4ded911a4c527eeb71ba593847fb405756239d2c694ce4effa3996cafb5cc0b3736dd988deb7289210ec92bf6b339302");
    OCT_fromHex(&seedR,(char *)"41b782c18c14986c9d7a636152f13677aeddf479c1c7791ea46e0ebbe35ca9dd524c23d730eef443741d7d965415833d6c549c8c1b31ad05f2b9a88f916b2930528e");
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

    OCT_fromHex(&seedE,(char *)"81dc51e31ef8e9b33fefcdf00bd3b0ff585b941fe76cf39a86c269e2f53be7edb3db0be1a58b6cb8d8e6020fe8a2018c59d47cacb35b2b8c61bd4155438b5eda5c0d");
    OCT_fromHex(&seedR,(char *)"54af23ea93c8fc34deb6a7cd70e657ea8990fc4e9a18656d5764b62f7a33a9e0212adeae1585ad2ef28688c1b558866c1975973c4dff08955c1f9fd7939b10b5fbfc");
    OCT_fromHex(&seedS,(char *)"b65599d814192278ab826ef197a61b77db50f40495f77502dfaa03acd1f3565a3cefebd59de2328ece0638c90d8a89f9ca58f2850e39e9a4c9c339290d66da12fdf0");
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

    OCT_fromHex(&seedE,(char *)"dc1fda9b21a1af6925ecf9ad79d2422f698b4168587c7908b36f5f58352181b9506554d8d8c9427e0dd2cfda25f0eabf58e9f5597e1b76ac12c799fe96e3cc03dc59");
    OCT_fromHex(&seedR,(char *)"46592c2b171b8cdcce89601fab103f63ed43badadcf9df62a928ae3b7fa91f269eff3485f6401c374e19a8bb988005626b9c26d39795282b1095bcc4f62a67255e15");
    OCT_fromHex(&seedS,(char *)"d02446c344c10cd162486caa69aa1156ac3066e0fd668fa7faaf13bdbc944edbc0cd68ee36e4c30ecc36c2c5ab0978473eb1b5dcfff27985c9328877e85fd48b657d");
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


