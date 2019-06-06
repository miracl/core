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

/* Timing Pairing-Based Crypto */
/* Only suitable for curves of embedding degree 12 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ArduinoECCX08.h>
#include <pair_XXX.h>
#include <randapi.h>

using namespace amcl;
using namespace XXX;
//using namespace XXX_FP;
using namespace XXX_BIG;


csprng RNG;                // Crypto Strong RNG 
ECP2 Q;
ECP G;
BIG r;
int count;

void setup()
{
	int i;     
	ECP P;
	ECP2 W;
	FP12 g,w;
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
	Serial.println("Timing XXX PBC");

	ECP_generator(&G);
	ECP2_generator(&Q);
	BIG_rcopy(r,CURVE_Order);

	ECP_copy(&P,&G);
	PAIR_G1mul(&P,r);

	if (!ECP_isinf(&P))
	{
		Serial.println("FAILURE in G1 - rG!=O");
		while (1)  delay(1000);
		
	}

	ECP2_copy(&W,&Q);
	PAIR_G2mul(&W,r);

	if (!ECP_isinf(&P))
	{
		Serial.println("FAILURE in G2 - rQ!=O");
		while (1)  delay(1000);
		
	}

	PAIR_ate(&w,&Q,&G);
	PAIR_fexp(&w);

	FP12_copy(&g,&w);
	PAIR_GTpow(&g,r);

	if (!FP12_isunity(&g))
	{
		Serial.println("FAILURE in GT - g^r!=1");
		while (1)  delay(1000);
		
	}

	count=0;
}

void loop()
{
	ECP P;
	ECP2 W;
	FP12 g,w;
	BIG s;
	BIG_randtrunc(s,r,2*CURVE_SECURITY_XXX,&RNG);
	Serial.println("Start G1 point multiplication");
	ECP_copy(&P,&G);
	PAIR_G1mul(&P,s);
	Serial.println("Finish G1 point multiplication");
	ECP2_copy(&W,&Q);
	PAIR_G2mul(&W,s);
	Serial.println("Finish G2 point multiplication");
	PAIR_ate(&w,&Q,&P);
	Serial.println("Finish Ate pairing");
	PAIR_fexp(&w);
	Serial.println("Finish final exponentiation");
	FP12_copy(&g,&w);
	PAIR_GTpow(&g,s);
	Serial.println("Finish GT exponentiation");
	count++;
	if (count>1)
	{
		while(1) delay(1000);
	}
}


