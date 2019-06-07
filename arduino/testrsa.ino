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
#include <rsa_XXX.h>
#include <randapi.h>

using namespace amcl;

csprng RNG;                // Crypto Strong RNG 

int rsa_XXX()
{
    using namespace XXX;

    int i,res=1;
    unsigned long ran;
    char m[RFS_XXX],ml[RFS_XXX],c[RFS_XXX],e[RFS_XXX],s[RFS_XXX];
    rsa_public_key pub;
    rsa_private_key priv;
    
    octet M= {0,sizeof(m),m};
    octet ML= {0,sizeof(ml),ml};
    octet C= {0,sizeof(c),c};
    octet E= {0,sizeof(e),e};
    octet S= {0,sizeof(s),s};

    Serial.println("Generating public/private key pair");
    RSA_KEY_PAIR(&RNG,65537,&priv,&pub,NULL,NULL);

    Serial.println("Encrypting test string");
    OCT_jstring(&M,(char *)"Hello World\n");

    OAEP_ENCODE(HASH_TYPE_RSA_XXX,&M,&RNG,NULL,&E); // OAEP encode message m to e  

    RSA_ENCRYPT(&pub,&E,&C);     // encrypt encoded message 
    Serial.print("Ciphertext= ");
    OCT_output(&C);

    Serial.println("Decrypting test string");
    RSA_DECRYPT(&priv,&C,&ML);   // ... and then decrypt it 
	Serial.println("Decrypted");
    OAEP_DECODE(HASH_TYPE_RSA_XXX,NULL,&ML);    // decode it 
	Serial.println("Decoded");
    OCT_output_string(&ML);

    Serial.println("Signing message");
    PKCS15(HASH_TYPE_RSA_XXX,&M,&C);

    RSA_DECRYPT(&priv,&C,&S); // create signature in S 

    Serial.print("Signature= ");
    OCT_output(&S);

    RSA_ENCRYPT(&pub,&S,&ML);

    if (OCT_comp(&C,&ML)) Serial.println("Signature is valid");
    else 
    {
        Serial.println("Signature is INVALID");
	res=0;
    }
    RSA_PRIVATE_KEY_KILL(&priv);

    OCT_clear(&M);
    OCT_clear(&ML);   // clean up afterwards 
    OCT_clear(&C);
    OCT_clear(&E);

    return res;
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
	Serial.println("\nTesting RSA protocols");
	if (!rsa_XXX())
	{
		Serial.println("RSA failed");
		while (1) ;
	}
}

