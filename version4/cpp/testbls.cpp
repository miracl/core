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
#include <time.h>
#include "bls_BN254.h"

#if CHUNK==32 || CHUNK==64
#include "bls_BLS383.h"
#include "bls192_BLS24.h"
#include "bls256_BLS48.h"
#endif

#include "randapi.h"

using namespace amcl;

static char message[]="This is a test message";

int bls_BN254(csprng *RNG)
{
	using namespace BN254;

	int res;
    char s[BGS_BN254],w[4*BFS_BN254],sig[BFS_BN254+1];
	octet S= {0,sizeof(s),s};
    octet W= {0,sizeof(w),w};
	octet SIG= {0,sizeof(sig),sig};	

	BLS_INIT();

	BLS_KEY_PAIR_GENERATE(RNG,&S,&W);

	printf("Private key= 0x");
    OCT_output(&S);
	printf("Public key= 0x");
    OCT_output(&W);

	BLS_SIGN(&SIG,message,&S);
	printf("Signature= 0x");
    OCT_output(&SIG);

	//message[7]='f'; // change the message

	res=BLS_VERIFY(&SIG,message,&W);
	if (res==BLS_OK) printf("Signature is OK\n");
	else printf("Signature is *NOT* OK\n");
	return res;
}

#if CHUNK==32 || CHUNK==64

int bls_BLS383(csprng *RNG)
{
	using namespace BLS383;

	int res;
    char s[BGS_BLS383],w[4*BFS_BLS383],sig[BFS_BLS383+1];
	octet S= {0,sizeof(s),s};
    octet W= {0,sizeof(w),w};
	octet SIG= {0,sizeof(sig),sig};	

	BLS_INIT();

	BLS_KEY_PAIR_GENERATE(RNG,&S,&W);

	printf("Private key= 0x");
    OCT_output(&S);
	printf("Public key= 0x");
    OCT_output(&W);

	BLS_SIGN(&SIG,message,&S);
	printf("Signature= 0x");
    OCT_output(&SIG);

	//message[7]='f'; // change the message

	res=BLS_VERIFY(&SIG,message,&W);
	if (res==BLS_OK) printf("Signature is OK\n");
	else printf("Signature is *NOT* OK\n");
	return res;
}

int bls_BLS24(csprng *RNG)
{
	using namespace BLS24;

	int res;
    char s[BGS_BLS24],w[8*BFS_BLS24],sig[BFS_BLS24+1];
	octet S= {0,sizeof(s),s};
    octet W= {0,sizeof(w),w};
	octet SIG= {0,sizeof(sig),sig};	

	BLS_INIT();

	BLS_KEY_PAIR_GENERATE(RNG,&S,&W);

	printf("Private key= 0x");
    OCT_output(&S);
	printf("Public key= 0x");
    OCT_output(&W);

	BLS_SIGN(&SIG,message,&S);
	printf("Signature= 0x");
    OCT_output(&SIG);

	//message[7]='f'; // change the message

	res=BLS_VERIFY(&SIG,message,&W);
	if (res==BLS_OK) printf("Signature is OK\n");
	else printf("Signature is *NOT* OK\n");
	return res;
}

int bls_BLS48(csprng *RNG)
{
	using namespace BLS48;

	int res;
    char s[BGS_BLS48],w[16*BFS_BLS48],sig[BFS_BLS48+1];
	octet S= {0,sizeof(s),s};
    octet W= {0,sizeof(w),w};
	octet SIG= {0,sizeof(sig),sig};	

	BLS_INIT();

	BLS_KEY_PAIR_GENERATE(RNG,&S,&W);

	printf("Private key= 0x");
    OCT_output(&S);
	printf("Public key= 0x");
    OCT_output(&W);

	BLS_SIGN(&SIG,message,&S);
	printf("Signature= 0x");
    OCT_output(&SIG);

	//message[7]='f'; // change the message

	res=BLS_VERIFY(&SIG,message,&W);
	if (res==BLS_OK) printf("Signature is OK\n");
	else printf("Signature is *NOT* OK\n");
	return res;
}

#endif

int main()
{
    int i,res;
    unsigned long ran;

	char raw[100];
    octet RAW= {0,sizeof(raw),raw};
    csprng RNG;                // Crypto Strong RNG 

    time((time_t *)&ran);

    RAW.len=100;				// fake random seed source 
    RAW.val[0]=ran;
    RAW.val[1]=ran>>8;
    RAW.val[2]=ran>>16;
    RAW.val[3]=ran>>24;
    for (i=0; i<100; i++) RAW.val[i]=i;

    CREATE_CSPRNG(&RNG,&RAW);   // initialise strong RNG 

	printf("%d bit build\n",CHUNK);

	printf("Testing BLS signature for curve BN254\n");
	bls_BN254(&RNG);

#if CHUNK!=16
	printf("\nTesting BLS signature for curve BLS383\n");
	bls_BLS383(&RNG);

	printf("\nTesting BLS signature for curve BLS24\n");
	bls_BLS24(&RNG);

	printf("\nTesting BLS signature for curve BLS48\n");
	bls_BLS48(&RNG);
#endif

	KILL_CSPRNG(&RNG);
}


