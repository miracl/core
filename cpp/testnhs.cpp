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

/* test driver and function exerciser for NewHope Functions */

// See https://eprint.iacr.org/2016/1157 (Alkim, Ducas, Popplemann and Schwabe)

/* gcc -O2 testnhs.c amcl.a -o testnhs.exe */

#include <stdio.h>
#include <time.h>
#include "newhope.h"

using namespace amcl;

int main()
{
	int i,ii,j,err;
    unsigned long ran;
	char raw[100];
	csprng SRNG,CRNG;
	RAND_clean(&SRNG); RAND_clean(&CRNG);
	char s[1792],sb[1824],uc[2176],keyA[32],keyB[32];

	octet S= {0,sizeof(s),s};
	octet SB= {0,sizeof(sb),sb};
	octet UC= {0,sizeof(uc),uc};
	octet KEYA={0,sizeof(keyA),keyA};
	octet KEYB={0,sizeof(keyB),keyB};

// initialise random number generators
    time((time_t *)&ran);

    raw[0]=ran;
    raw[1]=ran>>8;
    raw[2]=ran>>16;
    raw[3]=ran>>24;

	for (i=4;i<100;i++) raw[i]=i+1;
	RAND_seed(&SRNG,100,raw);
										for (i=4;i<100;i++) raw[i]=i+2;
										RAND_seed(&CRNG,100,raw);
// NewHope Simple key exchange

	NHS_SERVER_1(&SRNG,&SB,&S);
										NHS_CLIENT(&CRNG,&SB,&UC,&KEYB);
	NHS_SERVER_2(&S,&UC,&KEYA);

    printf("Alice key= 0x");
    OCT_output(&KEYA);
										printf("Bob's key= 0x");
										OCT_output(&KEYB);

	return 0;
}


