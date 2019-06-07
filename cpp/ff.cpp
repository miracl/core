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

/* AMCL basic functions for Large Finite Field support */

#include "ff_WWW.h"

using namespace XXX; 

namespace WWW {
	static void FF_dsucopy(BIG x[],BIG y[],int);
	static void FF_dscopy(BIG x[],BIG y[],int);
	static void FF_sducopy(BIG x[],BIG y[],int);
	static void FF_shrw(BIG a[],int);
	static void FF_shlw(BIG a[],int);
	static void FF_radd(BIG z[],int,BIG x[],int,BIG y[],int,int);
	static void FF_rinc(BIG z[],int,BIG y[],int,int);
	static void FF_rdec(BIG z[],int,BIG y[],int,int);
	static void FF_rnorm(BIG z[],int,int);
	static void FF_cswap(BIG a[],BIG b[],int,int);
	static void FF_karmul(BIG z[],int,BIG x[],int,BIG y[],int,BIG t[],int,int);
	static void FF_karsqr(BIG z[],int,BIG x[],int,BIG t[],int,int);
	static void FF_karmul_lower(BIG z[],int,BIG x[],int,BIG y[],int,BIG t[],int,int);
	static void FF_karmul_upper(BIG z[],BIG x[],BIG y[],BIG t[],int);
	static void FF_lmul(BIG z[],BIG x[],BIG y[],int);
	static void FF_reduce(BIG r[],BIG T[],BIG N[],BIG ND[],int);
	static void FF_nres(BIG a[],BIG m[],int);
	static void FF_redc(BIG a[],BIG m[],BIG ND[],int);
	static void FF_invmod2m(BIG U[],BIG a[],int);
	static void FF_modmul(BIG z[],BIG x[],BIG y[],BIG p[],BIG ND[],int);
	static void FF_modsqr(BIG z[],BIG x[],BIG p[],BIG ND[],int);
}

/* Arazi and Qi inversion mod 256 */
static int invmod256(int a)
{
    int U,t1,t2,b,c;
    t1=0;
    c=(a>>1)&1;
    t1+=c;
    t1&=1;
    t1=2-t1;
    t1<<=1;
    U=t1+1;

// i=2
    b=a&3;
    t1=U*b;
    t1>>=2;
    c=(a>>2)&3;
    t2=(U*c)&3;
    t1+=t2;
    t1*=U;
    t1&=3;
    t1=4-t1;
    t1<<=2;
    U+=t1;

// i=4
    b=a&15;
    t1=U*b;
    t1>>=4;
    c=(a>>4)&15;
    t2=(U*c)&15;
    t1+=t2;
    t1*=U;
    t1&=15;
    t1=16-t1;
    t1<<=4;
    U+=t1;

    return U;
}

/* a=1/a mod 2^BIGBITS_XXX. This is very fast! */
void XXX::BIG_invmod2m(BIG a)
{
    int i;
    BIG U,t1,b,c;
    BIG_zero(U);
    BIG_inc(U,invmod256(BIG_lastbits(a,8)));
    for (i=8; i<BIGBITS_XXX; i<<=1)
    {
		BIG_norm(U);
        BIG_copy(b,a);
        BIG_mod2m(b,i);   // bottom i bits of a

        BIG_smul(t1,U,b);
        BIG_shr(t1,i); // top i bits of U*b

        BIG_copy(c,a);
        BIG_shr(c,i);
        BIG_mod2m(c,i); // top i bits of a

        BIG_smul(b,U,c);
        BIG_mod2m(b,i);  // bottom i bits of U*c

        BIG_add(t1,t1,b);
		BIG_norm(t1);
        BIG_smul(b,t1,U);
        BIG_copy(t1,b);  // (t1+b)*U
        BIG_mod2m(t1,i);				// bottom i bits of (t1+b)*U

        BIG_one(b);
        BIG_shl(b,i);
        BIG_sub(t1,b,t1);
        BIG_norm(t1);

        BIG_shl(t1,i);

        BIG_add(U,U,t1);
    }
    BIG_copy(a,U);
    BIG_norm(a);
    BIG_mod2m(a,BIGBITS_XXX);
}

/* x=y */
void WWW::FF_copy(BIG x[],BIG y[],int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_copy(x[i],y[i]);
}

/* x=y<<n */
static void WWW::FF_dsucopy(BIG x[],BIG y[],int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        BIG_copy(x[n+i],y[i]);
        BIG_zero(x[i]);
    }
}

/* x=y */
static void WWW::FF_dscopy(BIG x[],BIG y[],int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        BIG_copy(x[i],y[i]);
        BIG_zero(x[n+i]);
    }
}

/* x=y>>n */
static void WWW::FF_sducopy(BIG x[],BIG y[],int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_copy(x[i],y[n+i]);
}

/* set to zero */
void WWW::FF_zero(BIG x[],int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_zero(x[i]);
}

/* test equals 0 */
int WWW::FF_iszilch(BIG x[],int n)
{
    int i;
    for (i=0; i<n; i++)
        if (!BIG_iszilch(x[i])) return 0;
    return 1;
}

/* shift right by BIGBITS_XXX-bit words */
static void WWW::FF_shrw(BIG a[],int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        BIG_copy(a[i],a[i+n]);
        BIG_zero(a[i+n]);
    }
}

/* shift left by BIGBITS_XXX-bit words */
static void WWW::FF_shlw(BIG a[],int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        BIG_copy(a[i+n],a[i]);
        BIG_zero(a[i]);
    }
}

/* extract last bit */
int WWW::FF_parity(BIG x[])
{
    return BIG_parity(x[0]);
}

/* extract last m bits */
int WWW::FF_lastbits(BIG x[],int m)
{
    return BIG_lastbits(x[0],m);
}

/* x=1 */
void WWW::FF_one(BIG x[],int n)
{
    int i;
    BIG_one(x[0]);
    for (i=1; i<n; i++)
        BIG_zero(x[i]);
}

/* x=m, where m is 32-bit int */
void WWW::FF_init(BIG x[],sign32 m,int n)
{
    int i;
    BIG_zero(x[0]);
#if CHUNK<64
    x[0][0]=(chunk)(m&BMASK_XXX);
    x[0][1]=(chunk)(m>>BASEBITS_XXX);
#else
    x[0][0]=(chunk)m;
#endif
    for (i=1; i<n; i++)
        BIG_zero(x[i]);
}

/* compare x and y - must be normalised */
int WWW::FF_comp(BIG x[],BIG y[],int n)
{
    int i,j;
    for (i=n-1; i>=0; i--)
    {
        j=BIG_comp(x[i],y[i]);
        if (j!=0) return j;
    }
    return 0;
}

/* recursive add */
static void WWW::FF_radd(BIG z[],int zp,BIG x[],int xp,BIG y[],int yp,int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_add(z[zp+i],x[xp+i],y[yp+i]);
}

/* recursive inc */
static void WWW::FF_rinc(BIG z[],int zp,BIG y[],int yp,int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_add(z[zp+i],z[zp+i],y[yp+i]);
}

/* recursive dec */
static void WWW::FF_rdec(BIG z[],int zp,BIG y[],int yp,int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_sub(z[zp+i],z[zp+i],y[yp+i]);
}

/* simple add */
void WWW::FF_add(BIG z[],BIG x[],BIG y[],int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_add(z[i],x[i],y[i]);
}

/* simple sub */
void WWW::FF_sub(BIG z[],BIG x[],BIG y[],int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_sub(z[i],x[i],y[i]);
}

/* increment/decrement by a small integer */
void WWW::FF_inc(BIG x[],int m,int n)
{
    BIG_inc(x[0],m);
    FF_norm(x,n);
}

void WWW::FF_dec(BIG x[],int m,int n)
{
    BIG_dec(x[0],m);
    FF_norm(x,n);
}

/* normalise - but hold any overflow in top part unless n<0 */
static void WWW::FF_rnorm(BIG z[],int zp,int n)
{
    int i,trunc=0;
    chunk carry;
    if (n<0)
    {
        /* -v n signals to do truncation */
        n=-n;
        trunc=1;
    }
    for (i=0; i<n-1; i++)
    {
        carry=BIG_norm(z[zp+i]);

        z[zp+i][NLEN_XXX-1]^=carry<<P_TBITS_WWW; /* remove it */
        z[zp+i+1][0]+=carry;
    }
    carry=BIG_norm(z[zp+n-1]);
    if (trunc) z[zp+n-1][NLEN_XXX-1]^=carry<<P_TBITS_WWW;
}

void WWW::FF_norm(BIG z[],int n)
{
    FF_rnorm(z,0,n);
}

/* shift left by one bit */
void WWW::FF_shl(BIG x[],int n)
{
    int i;
    int carry,delay_carry=0;
    for (i=0; i<n-1; i++)
    {
        carry=BIG_fshl(x[i],1);
        x[i][0]|=delay_carry;
        x[i][NLEN_XXX-1]^=(chunk)carry<<P_TBITS_WWW;
        delay_carry=carry;
    }
    BIG_fshl(x[n-1],1);
    x[n-1][0]|=delay_carry;
}

/* shift right by one bit */
void WWW::FF_shr(BIG x[],int n)
{
    int i;
    int carry;
    for (i=n-1; i>0; i--)
    {
        carry=BIG_fshr(x[i],1);
        x[i-1][NLEN_XXX-1]|=(chunk)carry<<P_TBITS_WWW;
    }
    BIG_fshr(x[0],1);
}

void WWW::FF_output(BIG x[],int n)
{
    int i;
    FF_norm(x,n);
    for (i=n-1; i>=0; i--)
    {
        BIG_output(x[i]);
        printf(" ");
    }
}

void WWW::FF_rawoutput(BIG x[],int n)
{
    int i;
    for (i=n-1; i>=0; i--)
    {
        BIG_rawoutput(x[i]);
        printf(" ");
    }
}

/* Convert FFs to/from octet strings */
void WWW::FF_toOctet(octet *w,BIG x[],int n)
{
    int i;
    w->len=n*MODBYTES_XXX;
    for (i=0; i<n; i++)
    {
        BIG_toBytes(&(w->val[(n-i-1)*MODBYTES_XXX]),x[i]);
    }
}

void WWW::FF_fromOctet(BIG x[],octet *w,int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        BIG_fromBytes(x[i],&(w->val[(n-i-1)*MODBYTES_XXX]));
    }
}

/* in-place swapping using xor - side channel resistant */
static void WWW::FF_cswap(BIG a[],BIG b[],int d,int n)
{
    int i;
    for (i=0; i<n; i++)
        BIG_cswap(a[i],b[i],d);
    return;
}

/* z=x*y, t is workspace */
static void WWW::FF_karmul(BIG z[],int zp,BIG x[],int xp,BIG y[],int yp,BIG t[],int tp,int n)
{
    int nd2;
    if (n==1)
    {
		BIG_norm(x[xp]);
		BIG_norm(y[yp]);
        BIG_mul(t[tp],x[xp],y[yp]);
        BIG_split(z[zp+1],z[zp],t[tp],BIGBITS_XXX);
        return;
    }

    nd2=n/2;
    FF_radd(z,zp,x,xp,x,xp+nd2,nd2);
    FF_rnorm(z,zp,nd2);  /* needs this if recursion level too deep */

    FF_radd(z,zp+nd2,y,yp,y,yp+nd2,nd2);
    FF_rnorm(z,zp+nd2,nd2);
    FF_karmul(t,tp,z,zp,z,zp+nd2,t,tp+n,nd2);
    FF_karmul(z,zp,x,xp,y,yp,t,tp+n,nd2);
    FF_karmul(z,zp+n,x,xp+nd2,y,yp+nd2,t,tp+n,nd2);
    FF_rdec(t,tp,z,zp,n);
    FF_rdec(t,tp,z,zp+n,n);
    FF_rinc(z,zp+nd2,t,tp,n);
    FF_rnorm(z,zp,2*n);
}

static void WWW::FF_karsqr(BIG z[],int zp,BIG x[],int xp,BIG t[],int tp,int n)
{
    int nd2;
    if (n==1)
    {
		BIG_norm(x[xp]);
        BIG_sqr(t[tp],x[xp]);
        BIG_split(z[zp+1],z[zp],t[tp],BIGBITS_XXX);
        return;
    }
    nd2=n/2;
    FF_karsqr(z,zp,x,xp,t,tp+n,nd2);
    FF_karsqr(z,zp+n,x,xp+nd2,t,tp+n,nd2);
    FF_karmul(t,tp,x,xp,x,xp+nd2,t,tp+n,nd2);
    FF_rinc(z,zp+nd2,t,tp,n);
    FF_rinc(z,zp+nd2,t,tp,n);

    FF_rnorm(z,zp+nd2,n);  /* was FF_rnorm(z,zp,2*n)  */
}

static void WWW::FF_karmul_lower(BIG z[],int zp,BIG x[],int xp,BIG y[],int yp,BIG t[],int tp,int n)
{
    /* Calculates Least Significant bottom half of x*y */
    int nd2;
    if (n==1)
    {
        /* only calculate bottom half of product */
		BIG_norm(x[xp]);
		BIG_norm(y[yp]);
        BIG_smul(z[zp],x[xp],y[yp]);
        return;
    }
    nd2=n/2;
    FF_karmul(z,zp,x,xp,y,yp,t,tp+n,nd2);
    FF_karmul_lower(t,tp,x,xp+nd2,y,yp,t,tp+n,nd2);
    FF_rinc(z,zp+nd2,t,tp,nd2);
    FF_karmul_lower(t,tp,x,xp,y,yp+nd2,t,tp+n,nd2);
    FF_rinc(z,zp+nd2,t,tp,nd2);
    FF_rnorm(z,zp+nd2,-nd2);  /* truncate it */
}

static void WWW::FF_karmul_upper(BIG z[],BIG x[],BIG y[],BIG t[],int n)
{
    /* Calculates Most Significant upper half of x*y, given lower part */
    int nd2;

    nd2=n/2;
    FF_radd(z,n,x,0,x,nd2,nd2);
    FF_radd(z,n+nd2,y,0,y,nd2,nd2);
    FF_rnorm(z,n,nd2);
    FF_rnorm(z,n+nd2,nd2);

    FF_karmul(t,0,z,n+nd2,z,n,t,n,nd2);  /* t = (a0+a1)(b0+b1) */
    FF_karmul(z,n,x,nd2,y,nd2,t,n,nd2); /* z[n]= a1*b1 */
    /* z[0-nd2]=l(a0b0) z[nd2-n]= h(a0b0)+l(t)-l(a0b0)-l(a1b1) */
    FF_rdec(t,0,z,n,n);              /* t=t-a1b1  */
    FF_rinc(z,nd2,z,0,nd2);   /* z[nd2-n]+=l(a0b0) = h(a0b0)+l(t)-l(a1b1)  */
    FF_rdec(z,nd2,t,0,nd2);   /* z[nd2-n]=h(a0b0)+l(t)-l(a1b1)-l(t-a1b1)=h(a0b0) */
    FF_rnorm(z,0,-n);					/* a0b0 now in z - truncate it */
    FF_rdec(t,0,z,0,n);         /* (a0+a1)(b0+b1) - a0b0 */
    FF_rinc(z,nd2,t,0,n);

    FF_rnorm(z,nd2,n);
}

/* z=x*y */
void WWW::FF_mul(BIG z[],BIG x[],BIG y[],int n)
{
#ifndef C99
    BIG t[2*FFLEN_WWW];
#else
    BIG t[2*n];
#endif
    FF_karmul(z,0,x,0,y,0,t,0,n);
}

/* return low part of product */
static void WWW::FF_lmul(BIG z[],BIG x[],BIG y[],int n)
{
#ifndef C99
    BIG t[2*FFLEN_WWW];
#else
    BIG t[2*n];
#endif
    FF_karmul_lower(z,0,x,0,y,0,t,0,n);
}

/* Set b=b mod c */
void WWW::FF_mod(BIG b[],BIG c[],int n)
{
    int k=0;

    FF_norm(b,n);
    if (FF_comp(b,c,n)<0)
        return;
    do
    {
        FF_shl(c,n);
        k++;
    }
    while (FF_comp(b,c,n)>=0);

    while (k>0)
    {
        FF_shr(c,n);
        if (FF_comp(b,c,n)>=0)
        {
            FF_sub(b,b,c,n);
            FF_norm(b,n);
        }
        k--;
    }
}

/* z=x^2 */
void WWW::FF_sqr(BIG z[],BIG x[],int n)
{
#ifndef C99
    BIG t[2*FFLEN_WWW];
#else
    BIG t[2*n];
#endif
    FF_karsqr(z,0,x,0,t,0,n);
}

/* r=t mod modulus, N is modulus, ND is Montgomery Constant */
static void WWW::FF_reduce(BIG r[],BIG T[],BIG N[],BIG ND[],int n)
{
    /* fast karatsuba Montgomery reduction */
#ifndef C99
    BIG t[2*FFLEN_WWW];
    BIG m[FFLEN_WWW];
#else
    BIG t[2*n];
    BIG m[n];
#endif
    WWW::FF_sducopy(r,T,n);  /* keep top half of T */
    FF_karmul_lower(m,0,T,0,ND,0,t,0,n);  /* m=T.(1/N) mod R */

    FF_karmul_upper(T,N,m,t,n);  /* T=mN */
    FF_sducopy(m,T,n);

    FF_add(r,r,N,n);
    FF_sub(r,r,m,n);
    FF_norm(r,n);
}


/* Set r=a mod b */
/* a is of length - 2*n */
/* r,b is of length - n */
void WWW::FF_dmod(BIG r[],BIG a[],BIG b[],int n)
{
    int k;
#ifndef C99
    BIG m[2*FFLEN_WWW];
    BIG x[2*FFLEN_WWW];
#else
    BIG m[2*n];
    BIG x[2*n];
#endif
    FF_copy(x,a,2*n);
    FF_norm(x,2*n);
    FF_dsucopy(m,b,n);
    k=BIGBITS_XXX*n;

    while (FF_comp(x,m,2*n)>=0)
    {
        FF_sub(x,x,m,2*n);
        FF_norm(x,2*n);
    }

    while (k>0)
    {
        FF_shr(m,2*n);

        if (FF_comp(x,m,2*n)>=0)
        {
            FF_sub(x,x,m,2*n);
            FF_norm(x,2*n);
        }

        k--;
    }
    FF_copy(r,x,n);
    FF_mod(r,b,n);
}

/* Set r=1/a mod p. Binary method - a<p on entry */

void WWW::FF_invmodp(BIG r[],BIG a[],BIG p[],int n)
{
#ifndef C99
    BIG u[FFLEN_WWW],v[FFLEN_WWW],x1[FFLEN_WWW],x2[FFLEN_WWW],t[FFLEN_WWW],one[FFLEN_WWW];
#else
    BIG u[n],v[n],x1[n],x2[n],t[n],one[n];
#endif
    FF_copy(u,a,n);
    FF_copy(v,p,n);
    FF_one(one,n);
    FF_copy(x1,one,n);
    FF_zero(x2,n);

// reduce n in here as well!
    while (FF_comp(u,one,n)!=0 && FF_comp(v,one,n)!=0)
    {
        while (FF_parity(u)==0)
        {
            FF_shr(u,n);
            if (FF_parity(x1)!=0)
            {
                FF_add(x1,p,x1,n);
                FF_norm(x1,n);
            }
            FF_shr(x1,n);
        }
        while (FF_parity(v)==0)
        {
            FF_shr(v,n);
            if (FF_parity(x2)!=0)
            {
                FF_add(x2,p,x2,n);
                FF_norm(x2,n);
            }
            FF_shr(x2,n);
        }
        if (FF_comp(u,v,n)>=0)
        {

            FF_sub(u,u,v,n);
            FF_norm(u,n);
            if (FF_comp(x1,x2,n)>=0) FF_sub(x1,x1,x2,n);
            else
            {
                FF_sub(t,p,x2,n);
                FF_add(x1,x1,t,n);
            }
            FF_norm(x1,n);
        }
        else
        {
            FF_sub(v,v,u,n);
            FF_norm(v,n);
            if (FF_comp(x2,x1,n)>=0) FF_sub(x2,x2,x1,n);
            else
            {
                FF_sub(t,p,x1,n);
                FF_add(x2,x2,t,n);
            }
            FF_norm(x2,n);
        }
    }
    if (FF_comp(u,one,n)==0)
        FF_copy(r,x1,n);
    else
        FF_copy(r,x2,n);
}

/* nesidue mod m */
static void WWW::FF_nres(BIG a[],BIG m[],int n)
{
#ifndef C99
    BIG d[2*FFLEN_WWW];
#else
    BIG d[2*n];
#endif
	if (n==1)
	{
		BIG_dscopy(d[0],a[0]);
		BIG_dshl(d[0],NLEN_XXX*BASEBITS_XXX);
		BIG_dmod(a[0],d[0],m[0]);
	}
	else
	{
		FF_dsucopy(d,a,n);
		FF_dmod(a,d,m,n);
	}
}

static void WWW::FF_redc(BIG a[],BIG m[],BIG ND[],int n)
{
#ifndef C99
    BIG d[2*FFLEN_WWW];
#else
    BIG d[2*n];
#endif
	if (n==1)
	{
		BIG_dzero(d[0]);
		BIG_dscopy(d[0],a[0]);
		BIG_monty(a[0],m[0],((chunk)1<<BASEBITS_XXX)-ND[0][0],d[0]);
	}
	else
	{
		FF_mod(a,m,n);
		FF_dscopy(d,a,n);
		FF_reduce(a,d,m,ND,n);
		FF_mod(a,m,n);
	}
}

/* U=1/a mod 2^m - Arazi & Qi */
static void WWW::FF_invmod2m(BIG U[],BIG a[],int n)
{
    int i;
#ifndef C99
    BIG t1[FFLEN_WWW],b[FFLEN_WWW],c[FFLEN_WWW];
#else
    BIG t1[2*n],b[n],c[n];
#endif

    FF_zero(U,n);
    FF_zero(b,n);
    FF_zero(c,n);
    FF_zero(t1,2*n);

    BIG_copy(U[0],a[0]);
    BIG_invmod2m(U[0]);
    for (i=1; i<n; i<<=1)
    {
        FF_copy(b,a,i);
        FF_mul(t1,U,b,i);
        FF_shrw(t1,i); // top half to bottom half, top half=0

        FF_copy(c,a,2*i);
        FF_shrw(c,i); // top half of c
        FF_lmul(b,U,c,i); // should set top half of b=0
        FF_add(t1,t1,b,i);
        FF_norm(t1,2*i);
        FF_lmul(b,t1,U,i);
        FF_copy(t1,b,i);
        FF_one(b,i);
        FF_shlw(b,i);
        FF_sub(t1,b,t1,2*i);
        FF_norm(t1,2*i);
        FF_shlw(t1,i);
        FF_add(U,U,t1,2*i);
    }

    FF_norm(U,n);
}

void WWW::FF_random(BIG x[],csprng *rng,int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        BIG_random(x[i],rng);
    }
    /* make sure top bit is 1 */
    while (BIG_nbits(x[n-1])<MODBYTES_XXX*8) BIG_random(x[n-1],rng);
}

/* generate random x mod p */
void WWW::FF_randomnum(BIG x[],BIG p[],csprng *rng,int n)
{
    int i;
#ifndef C99
    BIG d[2*FFLEN_WWW];
#else
    BIG d[2*n];
#endif
    for (i=0; i<2*n; i++)
    {
        BIG_random(d[i],rng);
    }
    FF_dmod(x,d,p,n);
}

static void WWW::FF_modmul(BIG z[],BIG x[],BIG y[],BIG p[],BIG ND[],int n)
{
#ifndef C99
    BIG d[2*FFLEN_WWW];
#else
    BIG d[2*n];
#endif
    chunk ex=P_EXCESS_WWW(x[n-1]);
    chunk ey=P_EXCESS_WWW(y[n-1]);
#ifdef dchunk
    if ((dchunk)(ex+1)*(ey+1)>(dchunk)P_FEXCESS_WWW)
#else
    if ((ex+1)>P_FEXCESS_WWW/(ey+1))
#endif
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it %d %d\n",ex,ey);
#endif
        FF_mod(x,p,n);
    }

	if (n==1)
	{
		BIG_mul(d[0],x[0],y[0]);
		BIG_monty(z[0],p[0],((chunk)1<<BASEBITS_XXX)-ND[0][0],d[0]);
	}
	else
	{
		FF_mul(d,x,y,n);
		FF_reduce(z,d,p,ND,n);
	}
}

static void WWW::FF_modsqr(BIG z[],BIG x[],BIG p[],BIG ND[],int n)
{
#ifndef C99
    BIG d[2*FFLEN_WWW];
#else
    BIG d[2*n];
#endif
    chunk ex=P_EXCESS_WWW(x[n-1]);
#ifdef dchunk
    if ((dchunk)(ex+1)*(ex+1)>(dchunk)P_FEXCESS_WWW)
#else
    if ((ex+1)>P_FEXCESS_WWW/(ex+1))
#endif
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it %d\n",ex);
#endif
        FF_mod(x,p,n);
    }
	if (n==1)
	{
		BIG_sqr(d[0],x[0]);
		BIG_monty(z[0],p[0],((chunk)1<<BASEBITS_XXX)-ND[0][0],d[0]);
	}
	else
	{
		FF_sqr(d,x,n);
		FF_reduce(z,d,p,ND,n);
	}
}

/* r=x^e mod p using side-channel resistant Montgomery Ladder, for large e */
void WWW::FF_skpow(BIG r[],BIG x[],BIG e[],BIG p[],int n)
{
    int i,b;
#ifndef C99
    BIG R0[FFLEN_WWW],R1[FFLEN_WWW],ND[FFLEN_WWW];
#else
    BIG R0[n],R1[n],ND[n];
#endif
    FF_invmod2m(ND,p,n);

    FF_one(R0,n);
    FF_copy(R1,x,n);
    FF_nres(R0,p,n);
    FF_nres(R1,p,n);

    for (i=8*MODBYTES_XXX*n-1; i>=0; i--)
    {
        b=BIG_bit(e[i/BIGBITS_XXX],i%BIGBITS_XXX);
        FF_modmul(r,R0,R1,p,ND,n);

        FF_cswap(R0,R1,b,n);
        FF_modsqr(R0,R0,p,ND,n);

        FF_copy(R1,r,n);
        FF_cswap(R0,R1,b,n);
    }
    FF_copy(r,R0,n);
    FF_redc(r,p,ND,n);
}

/* r=x^e mod p using side-channel resistant Montgomery Ladder, for short e */
void WWW::FF_skspow(BIG r[],BIG x[],BIG e,BIG p[],int n)
{
    int i,b;
#ifndef C99
    BIG R0[FFLEN_WWW],R1[FFLEN_WWW],ND[FFLEN_WWW];
#else
    BIG R0[n],R1[n],ND[n];
#endif
    FF_invmod2m(ND,p,n);
    FF_one(R0,n);
    FF_copy(R1,x,n);
    FF_nres(R0,p,n);
    FF_nres(R1,p,n);
    for (i=8*MODBYTES_XXX-1; i>=0; i--)
    {
        b=BIG_bit(e,i);
        FF_modmul(r,R0,R1,p,ND,n);
        FF_cswap(R0,R1,b,n);
        FF_modsqr(R0,R0,p,ND,n);
        FF_copy(R1,r,n);
        FF_cswap(R0,R1,b,n);
    }
    FF_copy(r,R0,n);
    FF_redc(r,p,ND,n);
}

/* raise to an integer power - right-to-left method */
void WWW::FF_power(BIG r[],BIG x[],int e,BIG p[],int n)
{
    int f=1;
#ifndef C99
    BIG w[FFLEN_WWW],ND[FFLEN_WWW];
#else
    BIG w[n],ND[n];
#endif
    FF_invmod2m(ND,p,n);

    FF_copy(w,x,n);
    FF_nres(w,p,n);

    if (e==2)
    {
        FF_modsqr(r,w,p,ND,n);
    }
    else for (;;)
        {
            if (e%2==1)
            {
                if (f) FF_copy(r,w,n);
                else FF_modmul(r,r,w,p,ND,n);
                f=0;
            }
            e>>=1;
            if (e==0) break;
            FF_modsqr(w,w,p,ND,n);
        }

    FF_redc(r,p,ND,n);
}

/* r=x^e mod p, faster but not side channel resistant */
void WWW::FF_pow(BIG r[],BIG x[],BIG e[],BIG p[],int n)
{
    int i,b;
#ifndef C99
    BIG w[FFLEN_WWW],ND[FFLEN_WWW];
#else
    BIG w[n],ND[n];
#endif
    FF_invmod2m(ND,p,n);

    FF_copy(w,x,n);
    FF_one(r,n);
    FF_nres(r,p,n);
    FF_nres(w,p,n);

    for (i=8*MODBYTES_XXX*n-1; i>=0; i--)
    {
        FF_modsqr(r,r,p,ND,n);
        b=BIG_bit(e[i/BIGBITS_XXX],i%BIGBITS_XXX);
        if (b==1) FF_modmul(r,r,w,p,ND,n);
    }
    FF_redc(r,p,ND,n);
}

/* double exponentiation r=x^e.y^f mod p */
void WWW::FF_pow2(BIG r[],BIG x[],BIG e,BIG y[],BIG f,BIG p[],int n)
{
    int i,eb,fb;
#ifndef C99
    BIG xn[FFLEN_WWW],yn[FFLEN_WWW],xy[FFLEN_WWW],ND[FFLEN_WWW];
#else
    BIG xn[n],yn[n],xy[n],ND[n];
#endif

    FF_invmod2m(ND,p,n);

    FF_copy(xn,x,n);
    FF_copy(yn,y,n);
    FF_nres(xn,p,n);
    FF_nres(yn,p,n);
    FF_modmul(xy,xn,yn,p,ND,n);
    FF_one(r,n);
    FF_nres(r,p,n);

    for (i=8*MODBYTES_XXX-1; i>=0; i--)
    {
        eb=BIG_bit(e,i);
        fb=BIG_bit(f,i);
        FF_modsqr(r,r,p,ND,n);
        if (eb==1)
        {
            if (fb==1) FF_modmul(r,r,xy,p,ND,n);
            else FF_modmul(r,r,xn,p,ND,n);
        }
        else
        {
            if (fb==1) FF_modmul(r,r,yn,p,ND,n);
        }
    }
    FF_redc(r,p,ND,n);
}

static sign32 igcd(sign32 x,sign32 y)
{
    /* integer GCD, returns GCD of x and y */
    sign32 r;
    if (y==0) return x;
    while ((r=x%y)!=0)
        x=y,y=r;
    return y;
}

/* quick and dirty check for common factor with s */
int WWW::FF_cfactor(BIG w[],sign32 s,int n)
{
    int r;
    sign32 g;
#ifndef C99
    BIG x[FFLEN_WWW],y[FFLEN_WWW];
#else
    BIG x[n],y[n];
#endif
    FF_init(y,s,n);
    FF_copy(x,w,n);
    FF_norm(x,n);

    do
    {
        FF_sub(x,x,y,n);
        FF_norm(x,n);
        while (!FF_iszilch(x,n) && FF_parity(x)==0) FF_shr(x,n);
    }
    while (FF_comp(x,y,n)>0);
#if CHUNK<32
    g=x[0][0]+((sign32)(x[0][1])<<BASEBITS_XXX);
#else
    g=(sign32)x[0][0];
#endif
    r=igcd(s,g);
    if (r>1) return 1;
    return 0;
}

/* Miller-Rabin test for primality. Slow. */
int WWW::FF_prime(BIG p[],csprng *rng,int n)
{
    int i,j,loop,s=0;
#ifndef C99
    BIG d[FFLEN_WWW],x[FFLEN_WWW],unity[FFLEN_WWW],nm1[FFLEN_WWW];
#else
    BIG d[n],x[n],unity[n],nm1[n];
#endif
    sign32 sf=4849845;/* 3*5*.. *19 */

    FF_norm(p,n);

    if (FF_cfactor(p,sf,n)) return 0;

    FF_one(unity,n);
    FF_sub(nm1,p,unity,n);
    FF_norm(nm1,n);
    FF_copy(d,nm1,n);
    while (FF_parity(d)==0)
    {
        FF_shr(d,n);
        s++;
    }
    if (s==0) return 0;

    for (i=0; i<10; i++)
    {
        FF_randomnum(x,p,rng,n);
        FF_pow(x,x,d,p,n);
        if (FF_comp(x,unity,n)==0 || FF_comp(x,nm1,n)==0) continue;
        loop=0;
        for (j=1; j<s; j++)
        {
            FF_power(x,x,2,p,n);
            if (FF_comp(x,unity,n)==0) return 0;
            if (FF_comp(x,nm1,n)==0 )
            {
                loop=1;
                break;
            }
        }
        if (loop) continue;
        return 0;
    }

    return 1;
}

