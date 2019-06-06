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

/* Timing ECC Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ArduinoECCX08.h>
#include <ecp_XXX.h>
#include <randapi.h>

using namespace amcl;
using namespace XXX;
using namespace XXX_BIG;

csprng RNG;                // Crypto Strong RNG 
ECP G;
BIG r;
int count;

void setup()
{
	int i;  
	ECP P;
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

	Serial.println("Testing/Timing XXX ECC");

#if CURVETYPE_XXX==WEIERSTRASS
	Serial.println("Weierstrass parameterization");
#endif
#if CURVETYPE_XXX==EDWARDS
	Serial.println("Edwards parameterization");
#endif
#if CURVETYPE_XXX==MONTGOMERY
	Serial.println("Montgomery parameterization");
#endif

#if MODTYPE_YYY == PSEUDO_MERSENNE
	Serial.println("Pseudo-Mersenne Modulus");
#endif

#if MODTYPE_YYY == GENERALISED_MERSENNE
	Serial.println("Generalised-Mersenne Modulus");
#endif

#if MODTYPE_YYY == MONTGOMERY_FRIENDLY
	Serial.println("Montgomery Friendly Modulus");
#endif

#if MODTYPE_YYY == NOT_SPECIAL
	Serial.println("Not special Modulus");
#endif

	ECP_generator(&G);
	BIG_rcopy(r,CURVE_Order);
	ECP_copy(&P,&G);
	ECP_mul(&P,r);

	if (!ECP_isinf(&P))
	{
		Serial.println("FAILURE - rG!=O");
		while (1)  delay(1000);
		
	}
	count=0;
}

void loop()
{
	ECP P;
	BIG s;

	BIG_randtrunc(s,r,2*CURVE_SECURITY_XXX,&RNG);
	ECP_copy(&P,&G);
	Serial.println("Start ECC point multiplication");

	ECP_mul(&P,s);

	Serial.println("Stop ECC point multiplication");
	count++;
	if (count>6)
	{
		while(1) delay(1000);
	}
}

