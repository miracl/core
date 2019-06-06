/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/* test driver and function exerciser for ECDH/ECIES/ECDSA API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ArduinoECCX08.h>
#include <ecdh_XXX.h>
#include <randapi.h>

using namespace amcl;

csprng RNG;                // Crypto Strong RNG 

int ecdh_XXX()
{
    using namespace XXX;

    int i,res;
    unsigned long ran;
    char *pp=(char *)"M0ng00se";
    // These octets are automatically protected against buffer overflow attacks 
    // Note salt must be big enough to include an appended word 
    // Note ECIES ciphertext C must be big enough to include at least 1 appended block 
    // Recall EFS_XXX is field size in bytes. So EFS_XXX=32 for 256-bit curve 
    char s0[2*EGS_XXX],s1[EGS_XXX],w0[2*EFS_XXX+1],w1[2*EFS_XXX+1],z0[EFS_XXX],z1[EFS_XXX],key[AESKEY_XXX],salt[40],pw[40];
    octet S0= {0,sizeof(s0),s0};
    octet S1= {0,sizeof(s1),s1};
    octet W0= {0,sizeof(w0),w0};
    octet W1= {0,sizeof(w1),w1};
    octet Z0= {0,sizeof(z0),z0};
    octet Z1= {0,sizeof(z1),z1};
    octet KEY= {0,sizeof(key),key};
    octet SALT= {0,sizeof(salt),salt};
    octet PW= {0,sizeof(pw),pw};

    SALT.len=8;
    for (i=0; i<8; i++) SALT.val[i]=i+1; // set Salt

    Serial.print("Alice's Passphrase= ");
    Serial.println(pp);

    OCT_empty(&PW);
    OCT_jstring(&PW,pp);   // set Password from string

    // private key S0 of size EGS_XXX bytes derived from Password and Salt 

    PBKDF2(HASH_TYPE_XXX,&PW,&SALT,1000,EGS_XXX,&S0);

    Serial.print("Alices private key= 0x");
    OCT_output(&S0);

    // Generate Key pair S/W 

    ECP_KEY_PAIR_GENERATE(NULL,&S0,&W0);
    Serial.print("Alices public key= 0x");
    OCT_output(&W0);

    res=ECP_PUBLIC_KEY_VALIDATE(&W0);
    if (res!=0)
    {
        Serial.println("ECP Public Key is invalid!");
        return 0;
    }

    // Random private key for other party 
    ECP_KEY_PAIR_GENERATE(&RNG,&S1,&W1);
    res=ECP_PUBLIC_KEY_VALIDATE(&W1);
    if (res!=0)
    {
        Serial.println("ECP Public Key is invalid!");
        return 0;
    }
    Serial.print("Servers private key= 0x");
    OCT_output(&S1);
    Serial.print("Servers public key= 0x");
    OCT_output(&W1);

    // Calculate common key using DH - IEEE 1363 method 

    ECP_SVDP_DH(&S0,&W1,&Z0);
    ECP_SVDP_DH(&S1,&W0,&Z1);

    if (!OCT_comp(&Z0,&Z1))
    {
        Serial.println("*** ECPSVDP-DH Failed");
        return 0;
    }

    KDF2(HASH_TYPE_XXX,&Z0,NULL,AESKEY_XXX,&KEY);

    Serial.print("Alice's DH Key=  0x");
    OCT_output(&KEY);
    Serial.print("Servers DH Key=  0x");
    OCT_output(&KEY);

    char ds[EGS_XXX],p1[30],p2[30],v[2*EFS_XXX+1],m[32],c[64],t[32],cs[EGS_XXX];
    octet DS= {0,sizeof(ds),ds};
    octet CS= {0,sizeof(cs),cs};
    octet P1= {0,sizeof(p1),p1};
    octet P2= {0,sizeof(p2),p2};
    octet V= {0,sizeof(v),v};
    octet M= {0,sizeof(m),m};
    octet C= {0,sizeof(c),c};
    octet T= {0,sizeof(t),t};

    Serial.println("Testing ECIES");

    P1.len=3;
    P1.val[0]=0x0;
    P1.val[1]=0x1;
    P1.val[2]=0x2;
    P2.len=4;
    P2.val[0]=0x0;
    P2.val[1]=0x1;
    P2.val[2]=0x2;
    P2.val[3]=0x3;

    M.len=17;
    for (i=0; i<=16; i++) M.val[i]=i;

    ECP_ECIES_ENCRYPT(HASH_TYPE_XXX,&P1,&P2,&RNG,&W1,&M,12,&V,&C,&T);

    Serial.println("Ciphertext= ");
    Serial.print("V= 0x");
    OCT_output(&V);
    Serial.print("C= 0x");
    OCT_output(&C);
    Serial.print("T= 0x");
    OCT_output(&T);

    if (!ECP_ECIES_DECRYPT(HASH_TYPE_XXX,&P1,&P2,&V,&C,&T,&S1,&M))
    {
        Serial.println("*** ECIES Decryption Failed");
        return 0;
    }
    else Serial.println("Decryption succeeded");

    Serial.print("Message is 0x");
    OCT_output(&M);


    Serial.println("Testing ECDSA");

    if (ECP_SP_DSA(HASH_TYPE_XXX,&RNG,NULL,&S0,&M,&CS,&DS)!=0)
    {
        Serial.println("***ECDSA Signature Failed");
        return 0;
    }

    Serial.print("Signature C = 0x");
    OCT_output(&CS);
    Serial.print("Signature D = 0x");
    OCT_output(&DS);

    if (ECP_VP_DSA(HASH_TYPE_XXX,&W0,&M,&CS,&DS)!=0)
    {
        Serial.println("***ECDSA Verification Failed");
        return 0;
    }
    else 
    {
      Serial.println("ECDSA Signature/Verification succeeded");
    }

    return 1;
}



void setup()
{
    int i;     
    Serial.begin(115200);
    while (!Serial);

    if (!ECCX08.begin()) {
        Serial.println("Failed to communicate with ECC508/ECC608!");
        while (1);
    }
    char raw[100];
    octet RAW= {0,sizeof(raw),raw};
    RAW.len=100;	
    for (i=0; i<100; i++) RAW.val[i]=ECCX08.random(256);

    CREATE_CSPRNG(&RNG,&RAW);   // initialise strong RNG 
}



void loop()
{
    Serial.println("\nTesting ECDH protocols for curve XXX\n");
    if (!ecdh_XXX())
    {
	Serial.println("ECC failed");
	while (1) ;
    }
}

