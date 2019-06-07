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

/* test driver and function exerciser for BLS Signature API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ArduinoECCX08.h>
#include <bls_XXX.h>
#include <randapi.h>

using namespace amcl;

csprng RNG;                // Crypto Strong RNG 

static char message[]="This is a test message";

int bls_XXX()
{
	using namespace XXX;

	int res;
	char s[BGS_XXX],w[4*BFS_XXX],sig[BFS_XXX+1];
	octet S= {0,sizeof(s),s};
	octet W= {0,sizeof(w),w};
	octet SIG= {0,sizeof(sig),sig};	

	Serial.println("Generating key pair");
	BLS_KEY_PAIR_GENERATE(&RNG,&S,&W);

	Serial.print("Private key= 0x");
	OCT_output(&S);
	Serial.print("Public key= 0x");
	OCT_output(&W);

	Serial.println("Signing");
	BLS_SIGN(&SIG,message,&S);
	Serial.print("Signature= 0x");
	OCT_output(&SIG);

	//message[7]='f'; // change the message
	Serial.println("Verifying");
	res=BLS_VERIFY(&SIG,message,&W);
	if (res==BLS_OK) 
	{
		Serial.println("Signature is OK");
		return 1;
	}
	else 
	{
		Serial.println("Signature is *NOT* OK");
		return 0;
	}
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
	Serial.println("Testing BLS signature for curve XXX");
	if (!bls_XXX())
	{
		Serial.println("BLS failed");
		while (1) ;
	}
}


