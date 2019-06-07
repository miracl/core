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

//
// Program to convert from Big to AMCL BIG format
// cl /O2 bigtobig.cpp big.cpp miracl.lib
// g++ -O2 bigtobig.cpp big.cpp miracl.a -o bigtobig
//
//

#include <iostream>
#include "zzn.h"

using namespace std;

Miracl precision=100;

// Code to parse formula in command line
// This code isn't mine, but its public domain
// Shamefully I forget the source
//
// NOTE: It may be necessary on some platforms to change the operators * and #
//

#if defined(unix)
#define TIMES '.'
#define RAISE '^'
#else
#define TIMES '*'
#define RAISE '#'
#endif

Big tt;
static char *ss;

void eval_power (Big& oldn,Big& n,char op)
{
        if (op) n=pow(oldn,toint(n));    // power(oldn,size(n),n,n);
}

void eval_product (Big& oldn,Big& n,char op)
{
        switch (op)
        {
        case TIMES:
                n*=oldn; 
                break;
        case '/':
                n=oldn/n;
                break;
        case '%':
                n=oldn%n;
        }
}

void eval_sum (Big& oldn,Big& n,char op)
{
        switch (op)
        {
        case '+':
                n+=oldn;
                break;
        case '-':
                n=oldn-n;
        }
}

void eval (void)
{
        Big oldn[3];
        Big n;
        int i;
        char oldop[3];
        char op;
        char minus;
        for (i=0;i<3;i++)
        {
            oldop[i]=0;
        }
LOOP:
        while (*ss==' ')
        ss++;
        if (*ss=='-')    /* Unary minus */
        {
        ss++;
        minus=1;
        }
        else
        minus=0;
        while (*ss==' ')
        ss++;
        if (*ss=='(' || *ss=='[' || *ss=='{')    /* Number is subexpression */
        {
        ss++;
        eval ();
        n=tt;
        }
        else            /* Number is decimal value */
        {
        for (i=0;ss[i]>='0' && ss[i]<='9';i++)
                ;
        if (!i)         /* No digits found */
        {
                cout <<  "Error - invalid number" << endl;
                exit (20);
        }
        op=ss[i];
        ss[i]=0;
        n=atoi(ss);
        ss+=i;
        *ss=op;
        }
        if (minus) n=-n;
        do
        op=*ss++;
        while (op==' ');
        if (op==0 || op==')' || op==']' || op=='}')
        {
        eval_power (oldn[2],n,oldop[2]);
        eval_product (oldn[1],n,oldop[1]);
        eval_sum (oldn[0],n,oldop[0]);
        tt=n;
        return;
        }
        else
        {
        if (op==RAISE)
        {
                eval_power (oldn[2],n,oldop[2]);
                oldn[2]=n;
                oldop[2]=RAISE;
        }
        else
        {
                if (op==TIMES || op=='/' || op=='%')
                {
                eval_power (oldn[2],n,oldop[2]);
                oldop[2]=0;
                eval_product (oldn[1],n,oldop[1]);
                oldn[1]=n;
                oldop[1]=op;
                }
                else
                {
                if (op=='+' || op=='-')
                {
                        eval_power (oldn[2],n,oldop[2]);
                        oldop[2]=0;
                        eval_product (oldn[1],n,oldop[1]);
                        oldop[1]=0;
                        eval_sum (oldn[0],n,oldop[0]);
                        oldn[0]=n;
                        oldop[0]=op;
                }
                else    /* Error - invalid operator */
                {
                        cout <<  "Error - invalid operator" << endl;
                        exit (20);
                }
                }
        }
        }
        goto LOOP;
}

void output(int w,Big t,Big m)
{
	Big y=t;

	for (int i=0;i<w;i++)
	{
		cout << "0x" << y%m ;
		if (i<w-1) cout << ",";
		y/=m;
	}
	cout << endl;
}

int main(int argc, char **argv)
{
    int i,ip,chunk,basebits;
    Big n,m;
    BOOL gotP,gotA,gotB;
    int Base;
    miracl *mip=&precision;
    argv++; argc--;
    if (argc<1)
    {
        cout << "Program converts from Big to BIG" << endl;
        cout << "bigtobig <big number> <chunk>> <basebits>" << endl;
        cout << "OR" << endl;
        cout << "bigtobig <formula for big number> <chunk>> <basebits>" << endl;
#if defined(unix)
        cout << "e.g. bigtobig -f 2^255-19 32 29" << endl;
#else
        cout << "e.g. bigtobig -f 2#255-19 32 29" << endl;
#endif
        cout << "To input Big number in Hex, precede with -h" << endl;
        return 0;
    }

    ip=0;
    gprime(1000);
    gotP=gotA=gotB=FALSE;
    n=0;
    Base=10;
    while (ip<argc)
    { 
        if (!gotP && strcmp(argv[ip],"-f")==0)
        {
            ip++;
            if (!gotP && ip<argc)
            {

                ss=argv[ip++];
                tt=0;
                eval();
                n=tt;
                gotP=TRUE;
                continue;
            }
            else
            {
                cout << "Error in command line" << endl;
                return 0;
            }
        }
 

        if (strcmp(argv[ip],"-h")==0)
        {
            ip++;
            Base=16;
            continue;
        }
   
        if (!gotP)
        {
            mip->IOBASE=Base;
            n=argv[ip++];
            mip->IOBASE=10;
            gotP=TRUE;
            continue;
        }
        if (!gotA) 
        {
            mip->IOBASE=Base;
            chunk=atoi(argv[ip++]);
            gotA=TRUE;
            continue;
        }
        if (!gotB) 
        {
            mip->IOBASE=Base;
            basebits=atoi(argv[ip++]);
            gotB=TRUE;
            continue;
        }
        cout << "Error in command line" << endl;
        return 0;
    }
    if (!gotP || !gotA || !gotB)
    {
        cout << "Error in command line" << endl;
        return 0;
    }

	mip->IOBASE=16;

	m=pow((Big)2,basebits);

	output(1+bits(n)/basebits,n,m);

    return 0;
}

