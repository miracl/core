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

/* Utility to recommend best choice for BASEBITS 

(MINGW build)

g++ -O2 check.cpp big.cpp miracl.a -o check.exe */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "big.h"

using namespace std;

Miracl precision=20;

// calculate maximum excess given base, length in words, length in bits
int maxxes(int b,int n,int m)
{
	int mx=n*b-m;
	int tp=m%b;
	if (mx >= tp)
		mx=tp-1;
	return mx;
}

int main(int argc, char *argv[])
{
	int p,w,b,n,s,t,e,ex,mx;
	Big lhs,rhs;

	argc--; argv++;

    if (argc!=2)
    {
       printf("Bad arguments\n");
       printf("check wordlength modulus-length\n");
	   printf("Wordlength can be 16, 32 or 64 (or 26 for Javascript)\n");
       printf("Example:\n");
       printf("check 32 256\n");
	   printf("Outputs choices for BASEBITS, number of words per Big, and number of spare bits\n");
	   printf("Normally choose for minimum words per Big, and maximum excess bits\n");
	   printf("Ideally >=5 excess bits for Edwards curves\n");
	   printf("Ideally >=10 excess bits for Weierstrass curves\n");
	   printf("Ideally >=23 excess bits for pairings\n");
       exit(0);
    }

    n=atoi(argv[0]);
	p=atoi(argv[1]); // Number of bits in prime modulus

	if (n!=16 && n!=26 && n!=32 && n!=64)
	{
		printf("wordlength must be 16, 32 or 64 (or 26 for Javascript)\n");
		return 0;
	}

	rhs=pow((Big)2,2*n-1);
	e=3;            // need at least 3 bits to allow adds and subtracts without normalisation
	if (n==26)
	{
		rhs*=2;	// no sign bit to worry about in Javascript
		e=1;	// not an issue for Javascript
	}

	for (b=n-e;b>=n-8;b--)
	{
		if (n==64 && b%2!=0) continue; // insist on even values for 64-bit builds 
		w=p/b; if (p%b!=0) w++;
		s=w*b-p;

		lhs=(w+2)*pow((Big)2,2*b);  // sum of products plus carry plus one for redc

		if (lhs>=rhs)    {printf("Stability violation for BASEBITS= %d\n",b); continue;}

		mx=s;
		if (mx>(n-4)/2)
			mx=(n-4)/2;

// At least 4 bits spare needed for field excess.  
		if (mx<4) {printf("Not enough Fp excess for BASEBITS= %d\n",b); continue;}
// At least 2 spare bits needed for FF excess 
		t=b*(1+(p-1)/b) - 8*(1+(p-1)/8);
		if (t<2 || t>=b) {printf("Not enough FF excess for BASEBITS= %d\n",b);}

		printf("Solution for BASEBITS= %d, Words Per Big=%d, Max excess= %d\n",b,w,mx);
	}
	
	return 0;
}
