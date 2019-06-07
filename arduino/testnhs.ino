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
#include <ArduinoECCX08.h>
#include <newhope.h>
#include <randapi.h>

using namespace amcl;

csprng SRNG,CRNG;

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
    CREATE_CSPRNG(&SRNG,&RAW);   // initialise strong RNG 

    for (i=0; i<100; i++) RAW.val[i]=ECCX08.random(256);
    CREATE_CSPRNG(&CRNG,&RAW);   // initialise strong RNG 
}

void loop()
{
	int i,ii,j,err;
	char s[1792],sb[1824],uc[2176],keyA[32],keyB[32];

	octet S= {0,sizeof(s),s};
	octet SB= {0,sizeof(sb),sb};
	octet UC= {0,sizeof(uc),uc};
	octet KEYA={0,sizeof(keyA),keyA};
	octet KEYB={0,sizeof(keyB),keyB};

// NewHope Simple key exchange

	NHS_SERVER_1(&SRNG,&SB,&S);
										NHS_CLIENT(&CRNG,&SB,&UC,&KEYB);
	NHS_SERVER_2(&S,&UC,&KEYA);

	Serial.print("Alice key= 0x");
	OCT_output(&KEYA);
										Serial.print("Bob's key= 0x");
										OCT_output(&KEYB);
}


