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
    OCT_fromHex(&skR,(char *)"d3c8ca6516cd4cc75f66210c5a49d05381bfbfc0de090c19432d778ea4599829");
    OCT_fromHex(&skE,(char *)"b9d453d3ec0dbe59fa4a193bde3e4ea17f80c9b2fa69f2f3e029120303b86885"); 
    OCT_fromHex(&pkR,(char *)"10b2fc2332b75206d2c791c3db1094dfd298b6508138ce98fec2c0c7a4dbc408"); 

    HPKE_C25519_Encap(config_id,&skE,&Z,&pkE,&pkR);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_Decap(config_id,&Z,&pkE,&skR);
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
    OCT_fromHex(&skR,(char *)"1f68f688b24c27f825012d40efa4fb33e033d717d569047a702c3ef5a64bde64");
    OCT_fromHex(&skE,(char *)"8ad4455d6eda442a9731ac224c9f8a468f489c77e3871cde9ebdd12e9027bad7");
    OCT_fromHex(&pkR,(char *)"6f61735fee57c59ce6489d80a11d77b7b2f9e9fddc3cb0bff0cf5a982ce7f344");
    
    HPKE_C25519_Encap(config_id,&skE,&Z,&pkE,&pkR);
    
    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_Decap(config_id,&Z,&pkE,&skR);
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
    OCT_fromHex(&skR,(char *)"41ee53399ebae982cd067fefa138b6b401cea1a7ef428fe9d6bc90e903a88f6e");
    OCT_fromHex(&skS,(char *)"55799581f14ec785dbaa11857700adf78f842a51b5ae6a4b4e5a4d99c66e5793");
    OCT_fromHex(&skE,(char *)"367299c0da446bfa8f3b41382c58b1b77fdadb5cf056d1fe94d6ab0b8741a184");
    OCT_fromHex(&pkR,(char *)"777b10479021ffb3d21ff7ad0d7ff1a27220f6203e729826a71dc1dd7f77ed27");
    OCT_fromHex(&pkS,(char *)"2bba172f178cf852e8670e574fdcdd62d8dcfa063548d3424d84f3e403f4e64c");

    HPKE_C25519_AuthEncap(config_id,&skE,&Z,&pkE,&pkR,&skS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_AuthDecap(config_id,&Z,&pkE,&skR,&pkS);
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

    OCT_fromHex(&skR,(char *)"2fe8ef0b6fdf5f8da2b43472ca05bc324f7e6aabd9e2b65b2dfa5b892f832a20");
    OCT_fromHex(&skS,(char *)"aede8f90d017a796c2dc73f3674168837a0ca0afaf3577b6aeeb784dc0b31c49");
    OCT_fromHex(&skE,(char *)"84a883acd803f41ea16ec23e81ebe3af3cff34fa3c6b50616d67511404d3daa0");
    OCT_fromHex(&pkR,(char *)"09171abdb6c6c8833791cb29357e39c2dbd5d6a7c1ed726f77bbdca5eddf397a");
    OCT_fromHex(&pkS,(char *)"92bf662e7ae5fda99130c32334f556803b00a419bb726386017c1fe217fb0e3d");

    HPKE_C25519_AuthEncap(config_id,&skE,&Z,&pkE,&pkR,&skS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_C25519_AuthDecap(config_id,&Z,&pkE,&skR,&pkS);
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
    OCT_fromHex(&skR,(char *)"01b3e33a2cf926ef496fff7e86a3743c23a9797c3bf99af8366ce22b3b0940fff1f1934e4d0bd539e130bdc3ff5991d37bdf45fc73529fe9a2e00e138d376610c26a");
    OCT_fromHex(&skE,(char *)"00252116681dacf4e707e9881aff5496942a36e74185347df21ab9d647dfe2a910e325b027ac3114335c038589d86a6a1a665498a5a6ea687ad5ddbd4258f273e2e7");
    OCT_fromHex(&pkR,(char *)"0400ec315d49eee4579d51967cb9cf2a848d918059769f530f0fcfb92342bbf00b561a55dc58fae5f8ef0fcf53e86514600b09787dc886afbbd682feb5cd3d1b3e671800be3c0358bf880ba2435eada8a1d5ea3585ea920c29ae39a5cb035de057721b8fc07d0666d46a6be634b257427404a19c7ebface8fa37da857bbef6fe04622d9627");

    HPKE_NIST521_Encap(config_id,&skE,&Z,&pkE,&pkR);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_Decap(config_id,&Z,&pkE,&skR);
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

    OCT_fromHex(&skR,(char *)"017ee8d2bb7dc4766d91596bc1fa741d03ac78c729258e9118c65a2b21d9fcb4249b3854049a384c8c3c18387ae4d94b57e11407359d62cb5de0e7c2eeded7c5ee32");
    OCT_fromHex(&skE,(char *)"009c521217c269a1c94169f6bcd3b399903ea6d385b38d51eac1fa07a4aac06bc144613ba531e3377a817715d987fd63229e3adcaa1e585e3fe75ea8288a486ae3a8");
    OCT_fromHex(&pkR,(char *)"04009070038197aaa0221dc91ad4c638ce2e28ba901d9cf588f4dcec4ae243507f19bf14c835d442aa26a9ee6d14f69777ae92c6f3a80f29bdd24ec38972a23ce0e89a00f81d57db61fb8f530c25fb9dd226bf57de3c914254baf5c57c2de46cded50e051531b0d5e7ea57c26a4ab80bba58e1b862110ad67b505e8f1011e00ad3f23a0d43");
    
    HPKE_NIST521_Encap(config_id,&skE,&Z,&pkE,&pkR);
    
    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_Decap(config_id,&Z,&pkE,&skR);
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

    OCT_fromHex(&skR,(char *)"00e4a39e9808aec7cf16708aeb13eaa70fd0bcb453534233836f3207d88c61af1c23a4a993687973b42d178c1789bee1656f01d463e86cad6758c79f817f680e26bf");
    OCT_fromHex(&skS,(char *)"01dd6f1ea81fbcb28da541d801edfa89e5648cc809b42cbe8b503caf6d7c5026a8bd9ea058951b8b9eee10e095a1865ef26ef247a3e62c25062f3877ff0627711eca");
    OCT_fromHex(&skE,(char *)"0179c8ecb90db0cec33df6cf79fda902d4facfd55a1e26216e176769d149df8da12d46f35f07c7cabd5613dc47826fc5b778f24a50381c4ed0a9b93cee969f32f735");
    OCT_fromHex(&pkR,(char *)"0400bd881c923366876ec86f5110ec695d2da0cd8b35e64adbd5a9b13fc6daa9daac5b38277b6cd791fd6b47deab09815492426f373ead722b2ea7257b9e110681ec1001800806a1c5f656bb354333c001d20a7639e5749fb3bc1a209f00e512752bceffa60e771926ce9d6972bb3abeaf6d314b09d3c0ae69452dd61db2b84a21d70402dc");
    OCT_fromHex(&pkS,(char *)"040162d338cc04cf8f7aa40bf50efdef65fd17d561a22e387e833fa71408a9a3a7e44900964544c58ce2e3e444a4622fafdaa0ce72d39266b6aae61d01879783bd5d910063ec49234436b959148f8220e583774298e5dbd28be96be0c4f754ac93a10306c72495a84472d08376547d5fec4da477d316c4f6810cd97a9e97e792a06745fbc8");

    HPKE_NIST521_AuthEncap(config_id,&skE,&Z,&pkE,&pkR,&skS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_AuthDecap(config_id,&Z,&pkE,&skR,&pkS);
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

    OCT_fromHex(&skR,(char *)"00dcca4c7aad3a047a5d0c579dce56d45a6ef7446055ef19d4295dff1b8d1586f95e09052f733c5d1c9fa8bb7eab289cc3c5bb23e55559606a7b509d56bd660a63d0");
    OCT_fromHex(&skS,(char *)"002a28ddb89ecbb9af391e31185cdaf7a4683c9bf3f950866b73e65cae7f769a849a792b23c65266cda967208a6f7d741ce06b9a5b206a1c670a36a02c0b2d62bbc5");
    OCT_fromHex(&skE,(char *)"00a69a328f366433eb173d1cd308627e6733d7b52d88745eb8bd63a4d4959d6ff82389a1ead33fa9816afcb348532d78d41e1444f9f1aa945cdfc4f4bb98c8514722");
    OCT_fromHex(&pkR,(char *)"0400475fd0db79dacda40f43684ccc8a3397689135f422388905a95f7fc4357fad0ef3dec49c98a5aaa722457e4ee51d4eca72845b2d0580a8e77ab5086c3a9c6edede00c5bfa1b71a8c8ba3672952e5ceebc0446df94c96cc0051faa671158d5c9c86a05797c3ca6ccc19173b09bf2fe9b8e052f4ffecc871df03dc1da15b00cb1bfaa003");
    OCT_fromHex(&pkS,(char *)"040031d4c9313147b1b3e76ae4721a1fd12b8ff0096f5b4301e062e52d59d5cbebf1826b424e4ce29ebf726fd67425451506f3a38fe9b5d38c967ef4f03d9b4ae55e4901449a96bc684065b5e10e7852b43b7fbba72e3ecd3daf740c1b32289194beed37aa233c70ef8ae5cccd2ff65cbce812135e7dc5771b37506f1671f98dede8641dd8");

    HPKE_NIST521_AuthEncap(config_id,&skE,&Z,&pkE,&pkR,&skS);

    printf("pkE= "); OCT_output(&pkE); 
    printf("Encapsulated Z= "); OCT_output(&Z);

    HPKE_NIST521_AuthDecap(config_id,&Z,&pkE,&skR,&pkS);
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


