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

/* AMCL basic functions for BIG type */
/* SU=m, SU is Stack Usage */

#include "big_XXX.h"

/* test a=0? */
int BIG_XXX_iszilch(BIG_XXX a)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        if (a[i]!=0) return 0;
    return 1;
}

/* test a=1? */
int BIG_XXX_isunity(BIG_XXX a)
{
    int i;
    for(i=1; i<NLEN_XXX; i++)
        if (a[i]!=0) return 0;
    if (a[0]!=1) return 0;
    return 1;
}

/* test a=0? */
int BIG_XXX_diszilch(DBIG_XXX a)
{
    int i;
    for (i=0; i<DNLEN_XXX; i++)
        if (a[i]!=0) return 0;
    return 1;
}

/* SU= 56 */
/* output a */
void BIG_XXX_output(BIG_XXX a)
{
    BIG_XXX b;
    int i,len;
    len=BIG_XXX_nbits(a);
    if (len%4==0) len/=4;
    else
    {
        len/=4;
        len++;
    }
    if (len<MODBYTES_XXX*2) len=MODBYTES_XXX*2;

    for (i=len-1; i>=0; i--)
    {
        BIG_XXX_copy(b,a);
        BIG_XXX_shr(b,i*4);
        printf("%01x",(unsigned int) b[0]&15);
    }
}

/* SU= 16 */
void BIG_XXX_rawoutput(BIG_XXX a)
{
    int i;
    printf("(");
    for (i=0; i<NLEN_XXX-1; i++)
#if CHUNK==64
        printf("%"PRIxMAX",",(uintmax_t) a[i]);
    printf("%"PRIxMAX")",(uintmax_t) a[NLEN_XXX-1]);
#else
        printf("%x,",(unsigned int) a[i]);
    printf("%x)",(unsigned int) a[NLEN_XXX-1]);
#endif
}

/* Swap a and b if d=1 */
void BIG_XXX_cswap(BIG_XXX a,BIG_XXX b,int d)
{
    int i;
    chunk t,c=d;
    c=~(c-1);
#ifdef DEBUG_NORM
    for (i=0; i<NLEN_XXX+2; i++)
#else
    for (i=0; i<NLEN_XXX; i++)
#endif
    {
        t=c&(a[i]^b[i]);
        a[i]^=t;
        b[i]^=t;
    }
}

/* Move b to a if d=1 */
void BIG_XXX_cmove(BIG_XXX f,BIG_XXX g,int d)
{
    int i;
    chunk b=(chunk)-d;
#ifdef DEBUG_NORM
    for (i=0; i<NLEN_XXX+2; i++)
#else
    for (i=0; i<NLEN_XXX; i++)
#endif
    {
        f[i]^=(f[i]^g[i])&b;
    }
}

/* Move g to f if d=1 */
void BIG_XXX_dcmove(DBIG_XXX f,DBIG_XXX g,int d)
{
    int i;
    chunk b=(chunk)-d;
#ifdef DEBUG_NORM
    for (i=0; i<DNLEN_XXX+2; i++)
#else
    for (i=0; i<DNLEN_XXX; i++)
#endif
    {
        f[i]^=(f[i]^g[i])&b;
    }
}

/* convert BIG to/from bytes */
/* SU= 64 */
void BIG_XXX_toBytes(char *b,BIG_XXX a)
{
    int i;
    BIG_XXX c;
    BIG_XXX_copy(c,a);
	BIG_XXX_norm(c);
    for (i=MODBYTES_XXX-1; i>=0; i--)
    {
        b[i]=c[0]&0xff;
        BIG_XXX_fshr(c,8);
    }
}

/* SU= 16 */
void BIG_XXX_fromBytes(BIG_XXX a,char *b)
{
    int i;
    BIG_XXX_zero(a);
    for (i=0; i<MODBYTES_XXX; i++)
    {
        BIG_XXX_fshl(a,8);
        a[0]+=(int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[MPV_XXX]=1;
    a[MNV_XXX]=0;
#endif
}

void BIG_XXX_fromBytesLen(BIG_XXX a,char *b,int s)
{
    int i,len=s;
    BIG_XXX_zero(a);

    if (len>MODBYTES_XXX) len=MODBYTES_XXX;
    for (i=0; i<len; i++)
    {
        BIG_XXX_fshl(a,8);
        a[0]+=(int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[MPV_XXX]=1;
    a[MNV_XXX]=0;
#endif
}



/* SU= 88 */
void BIG_XXX_doutput(DBIG_XXX a)
{
    DBIG_XXX b;
    int i,len;
    BIG_XXX_dnorm(a);
    len=BIG_XXX_dnbits(a);
    if (len%4==0) len/=4;
    else
    {
        len/=4;
        len++;
    }

    for (i=len-1; i>=0; i--)
    {
        BIG_XXX_dcopy(b,a);
        BIG_XXX_dshr(b,i*4);
        printf("%01x",(unsigned int) b[0]&15);
    }
}


void BIG_XXX_drawoutput(DBIG_XXX a)
{
    int i;
    printf("(");
    for (i=0; i<DNLEN_XXX-1; i++)
#if CHUNK==64
        printf("%"PRIxMAX",",(uintmax_t) a[i]);
    printf("%"PRIxMAX")",(uintmax_t) a[DNLEN_XXX-1]);
#else
        printf("%x,",(unsigned int) a[i]);
    printf("%x)",(unsigned int) a[DNLEN_XXX-1]);
#endif
}

/* Copy b=a */
void BIG_XXX_copy(BIG_XXX b,BIG_XXX a)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        b[i]=a[i];
#ifdef DEBUG_NORM
    b[MPV_XXX]=a[MPV_XXX];
    b[MNV_XXX]=a[MNV_XXX];
#endif
}

/* Copy from ROM b=a */
void BIG_XXX_rcopy(BIG_XXX b,const BIG_XXX a)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        b[i]=a[i];
#ifdef DEBUG_NORM
    b[MPV_XXX]=1;
    b[MNV_XXX]=0;
#endif
}

/* double length DBIG copy b=a */
void BIG_XXX_dcopy(DBIG_XXX b,DBIG_XXX a)
{
    int i;
    for (i=0; i<DNLEN_XXX; i++)
        b[i]=a[i];
#ifdef DEBUG_NORM
    b[DMPV_XXX]=a[DMPV_XXX];
    b[DMNV_XXX]=a[DMNV_XXX];
#endif
}

/* Copy BIG to bottom half of DBIG */
void BIG_XXX_dscopy(DBIG_XXX b,BIG_XXX a)
{
    int i;
    for (i=0; i<NLEN_XXX-1; i++)
        b[i]=a[i];

    b[NLEN_XXX-1]=a[NLEN_XXX-1]&BMASK_XXX; /* top word normalized */
    b[NLEN_XXX]=a[NLEN_XXX-1]>>BASEBITS_XXX;

    for (i=NLEN_XXX+1; i<DNLEN_XXX; i++) b[i]=0;
#ifdef DEBUG_NORM
    b[DMPV_XXX]=a[MPV_XXX];
    b[DMNV_XXX]=a[MNV_XXX];
#endif
}

/* Copy BIG to top half of DBIG */
void BIG_XXX_dsucopy(DBIG_XXX b,BIG_XXX a)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        b[i]=0;
    for (i=NLEN_XXX; i<DNLEN_XXX; i++)
        b[i]=a[i-NLEN_XXX];
#ifdef DEBUG_NORM
    b[DMPV_XXX]=a[MPV_XXX];
    b[DMNV_XXX]=a[MNV_XXX];
#endif
}

/* Copy bottom half of DBIG to BIG */
void BIG_XXX_sdcopy(BIG_XXX b,DBIG_XXX a)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        b[i]=a[i];
#ifdef DEBUG_NORM
    b[MPV_XXX]=a[DMPV_XXX];
    b[MNV_XXX]=a[DMNV_XXX];
#endif
}

/* Copy top half of DBIG to BIG */
void BIG_XXX_sducopy(BIG_XXX b,DBIG_XXX a)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        b[i]=a[i+NLEN_XXX];
#ifdef DEBUG_NORM
    b[MPV_XXX]=a[DMPV_XXX];
    b[MNV_XXX]=a[DMNV_XXX];

#endif
}

/* Set a=0 */
void BIG_XXX_zero(BIG_XXX a)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        a[i]=0;
#ifdef DEBUG_NORM
    a[MPV_XXX]=a[MNV_XXX]=0;
#endif
}

void BIG_XXX_dzero(DBIG_XXX a)
{
    int i;
    for (i=0; i<DNLEN_XXX; i++)
        a[i]=0;
#ifdef DEBUG_NORM
    a[DMPV_XXX]=a[DMNV_XXX]=0;
#endif
}

/* set a=1 */
void BIG_XXX_one(BIG_XXX a)
{
    int i;
    a[0]=1;
    for (i=1; i<NLEN_XXX; i++)
        a[i]=0;
#ifdef DEBUG_NORM
    a[MPV_XXX]=1;
    a[MNV_XXX]=0;
#endif
}



/* Set c=a+b */
/* SU= 8 */
void BIG_XXX_add(BIG_XXX c,BIG_XXX a,BIG_XXX b)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        c[i]=a[i]+b[i];
#ifdef DEBUG_NORM
    c[MPV_XXX]=a[MPV_XXX]+b[MPV_XXX];
    c[MNV_XXX]=a[MNV_XXX]+b[MNV_XXX];
    if (c[MPV_XXX]>NEXCESS_XXX)  printf("add problem - positive digit overflow %d\n",c[MPV_XXX]);
    if (c[MNV_XXX]>NEXCESS_XXX)  printf("add problem - negative digit overflow %d\n",c[MNV_XXX]);

#endif
}

/* Set c=a or b */
void BIG_XXX_or(BIG_XXX c,BIG_XXX a,BIG_XXX b)
{
    int i;
	BIG_XXX_norm(a);
	BIG_XXX_norm(b);
    for (i=0; i<NLEN_XXX; i++)
        c[i]=a[i]|b[i];
#ifdef DEBUG_NORM
    c[MPV_XXX]=1;
    c[MNV_XXX]=0;
#endif
}


/* Set c=c+d */
void BIG_XXX_inc(BIG_XXX c,int d)
{
    BIG_XXX_norm(c);
    c[0]+=(chunk)d;
#ifdef DEBUG_NORM
    c[MPV_XXX]+=1;
#endif
}

/* Set c=a-b */
/* SU= 8 */
void BIG_XXX_sub(BIG_XXX c,BIG_XXX a,BIG_XXX b)
{
    int i;
    for (i=0; i<NLEN_XXX; i++)
        c[i]=a[i]-b[i];
#ifdef DEBUG_NORM
    c[MPV_XXX]=a[MPV_XXX]+b[MNV_XXX];
    c[MNV_XXX]=a[MNV_XXX]+b[MPV_XXX];
    if (c[MPV_XXX]>NEXCESS_XXX)  printf("sub problem - positive digit overflow %d\n",c[MPV_XXX]);
    if (c[MNV_XXX]>NEXCESS_XXX)  printf("sub problem - negative digit overflow %d\n",c[MNV_XXX]);

#endif
}

/* SU= 8 */

void BIG_XXX_dsub(DBIG_XXX c,DBIG_XXX a,DBIG_XXX b)
{
    int i;
    for (i=0; i<DNLEN_XXX; i++)
        c[i]=a[i]-b[i];
#ifdef DEBUG_NORM
    c[DMPV_XXX]=a[DMPV_XXX]+b[DMNV_XXX];
    c[DMNV_XXX]=a[DMNV_XXX]+b[DMPV_XXX];
    if (c[DMPV_XXX]>NEXCESS_XXX)  printf("double sub problem - positive digit overflow %d\n",c[DMPV_XXX]);
    if (c[DMNV_XXX]>NEXCESS_XXX)  printf("double sub problem - negative digit overflow %d\n",c[DMNV_XXX]);
#endif
}

void BIG_XXX_dadd(DBIG_XXX c,DBIG_XXX a,DBIG_XXX b)
{
    int i;
    for (i=0; i<DNLEN_XXX; i++)
        c[i]=a[i]+b[i];
#ifdef DEBUG_NORM
    c[DMPV_XXX]=a[DMPV_XXX]+b[DMNV_XXX];
    c[DMNV_XXX]=a[DMNV_XXX]+b[DMPV_XXX];
    if (c[DMPV_XXX]>NEXCESS_XXX)  printf("double add problem - positive digit overflow %d\n",c[DMPV_XXX]);
    if (c[DMNV_XXX]>NEXCESS_XXX)  printf("double add problem - negative digit overflow %d\n",c[DMNV_XXX]);
#endif
}

/* Set c=c-1 */
void BIG_XXX_dec(BIG_XXX c,int d)
{
    BIG_XXX_norm(c);
    c[0]-=(chunk)d;
#ifdef DEBUG_NORM
    c[MNV_XXX]+=1;
#endif
}

/* multiplication r=a*c by c<=NEXCESS_XXX */
void BIG_XXX_imul(BIG_XXX r,BIG_XXX a,int c)
{
    int i;
    for (i=0; i<NLEN_XXX; i++) r[i]=a[i]*c;
#ifdef DEBUG_NORM
    r[MPV_XXX]=a[MPV_XXX]*c;
    r[MNV_XXX]=a[MNV_XXX]*c;
    if (r[MPV_XXX]>NEXCESS_XXX)  printf("int mul problem - positive digit overflow %d\n",r[MPV_XXX]);
    if (r[MNV_XXX]>NEXCESS_XXX)  printf("int mul problem - negative digit overflow %d\n",r[MNV_XXX]);

#endif
}

/* multiplication r=a*c by larger integer - c<=FEXCESS */
/* SU= 24 */
chunk BIG_XXX_pmul(BIG_XXX r,BIG_XXX a,int c)
{
    int i;
    chunk ak,carry=0;
    for (i=0; i<NLEN_XXX; i++)
    {
        ak=a[i];
        r[i]=0;
        carry=muladd_XXX(ak,(chunk)c,carry,&r[i]);
    }
#ifdef DEBUG_NORM
    r[MPV_XXX]=1;
    r[MNV_XXX]=0;
#endif
    return carry;
}

/* r/=3 */
/* SU= 16 */
int BIG_XXX_div3(BIG_XXX r)
{
    int i;
    chunk ak,base,carry=0;
    BIG_XXX_norm(r);
    base=((chunk)1<<BASEBITS_XXX);
    for (i=NLEN_XXX-1; i>=0; i--)
    {
        ak=(carry*base+r[i]);
        r[i]=ak/3;
        carry=ak%3;
    }
    return (int)carry;
}

/* multiplication c=a*b by even larger integer b>FEXCESS, resulting in DBIG */
/* SU= 24 */
void BIG_XXX_pxmul(DBIG_XXX c,BIG_XXX a,int b)
{
    int j;
    chunk carry;
    BIG_XXX_dzero(c);
    carry=0;
    for (j=0; j<NLEN_XXX; j++)
        carry=muladd_XXX(a[j],(chunk)b,carry,&c[j]);
    c[NLEN_XXX]=carry;
#ifdef DEBUG_NORM
    c[DMPV_XXX]=1;
    c[DMNV_XXX]=0;
#endif
}

/* .. if you know the result will fit in a BIG, c must be distinct from a and b */
/* SU= 40 */
void BIG_XXX_smul(BIG_XXX c,BIG_XXX a,BIG_XXX b)
{
    int i,j;
    chunk carry;

    BIG_XXX_zero(c);
    for (i=0; i<NLEN_XXX; i++)
    {
        carry=0;
        for (j=0; j<NLEN_XXX; j++)
        {
            if (i+j<NLEN_XXX)
                carry=muladd_XXX(a[i],b[j],carry,&c[i+j]);
        }
    }
#ifdef DEBUG_NORM
    c[MPV_XXX]=1;
    c[MNV_XXX]=0;
#endif

}

/* Set c=a*b */
/* SU= 72 */
void BIG_XXX_mul(DBIG_XXX c,BIG_XXX a,BIG_XXX b)
{
    int i;
#ifdef dchunk
    dchunk t,co;
    dchunk s;
    dchunk d[NLEN_XXX];
    int k;
#endif

#ifdef DEBUG_NORM
    if ((a[MPV_XXX]!=1 && a[MPV_XXX]!=0) || a[MNV_XXX]!=0) printf("First input to mul not normed\n");
    if ((b[MPV_XXX]!=1 && b[MPV_XXX]!=0) || b[MNV_XXX]!=0) printf("Second input to mul not normed\n");
#endif

    /* Faster to Combafy it.. Let the compiler unroll the loops! */

#ifdef COMBA

    /* faster psuedo-Karatsuba method */
#ifdef UNWOUND

    /* Insert output of faster.c here */

#else
    for (i=0; i<NLEN_XXX; i++)
        d[i]=(dchunk)a[i]*b[i];

    s=d[0];
    t=s;
    c[0]=(chunk)t&BMASK_XXX;
    co=t>>BASEBITS_XXX;

    for (k=1; k<NLEN_XXX; k++)
    {
        s+=d[k];
        t=co+s;
        for (i=k; i>=1+k/2; i--) t+=(dchunk)(a[i]-a[k-i])*(b[k-i]-b[i]);
        c[k]=(chunk)t&BMASK_XXX;
        co=t>>BASEBITS_XXX;
    }
    for (k=NLEN_XXX; k<2*NLEN_XXX-1; k++)
    {
        s-=d[k-NLEN_XXX];
        t=co+s;
        for (i=NLEN_XXX-1; i>=1+k/2; i--) t+=(dchunk)(a[i]-a[k-i])*(b[k-i]-b[i]);
        c[k]=(chunk)t&BMASK_XXX;
        co=t>>BASEBITS_XXX;
    }
    c[2*NLEN_XXX-1]=(chunk)co;

#endif

#else
    int j;
    chunk carry;
    BIG_XXX_dzero(c);
    for (i=0; i<NLEN_XXX; i++)
    {
        carry=0;
        for (j=0; j<NLEN_XXX; j++)
            carry=muladd_XXX(a[i],b[j],carry,&c[i+j]);

        c[NLEN_XXX+i]=carry;
    }

#endif

#ifdef DEBUG_NORM
    c[DMPV_XXX]=1;
    c[DMNV_XXX]=0;
#endif
}

/* Set c=a*a */
/* SU= 80 */
void BIG_XXX_sqr(DBIG_XXX c,BIG_XXX a)
{
    int i,j;
#ifdef dchunk
    dchunk t,co;
#endif

#ifdef DEBUG_NORM
    if ((a[MPV_XXX]!=1 && a[MPV_XXX]!=0) || a[MNV_XXX]!=0) printf("Input to sqr not normed\n");
#endif
    /* Note 2*a[i] in loop below and extra addition */

#ifdef COMBA

#ifdef UNWOUND

    /* Insert output of faster.c here */

#else


    t=(dchunk)a[0]*a[0];
    c[0]=(chunk)t&BMASK_XXX;
    co=t>>BASEBITS_XXX;

    for (j=1; j<NLEN_XXX-1; )
    {
        t=(dchunk)a[j]*a[0];
        for (i=1; i<(j+1)/2; i++)
        {
            t+=(dchunk)a[j-i]*a[i];
        }
        t+=t;
        t+=co;
        c[j]=(chunk)t&BMASK_XXX;
        co=t>>BASEBITS_XXX;
        j++;
        t=(dchunk)a[j]*a[0];
        for (i=1; i<(j+1)/2; i++)
        {
            t+=(dchunk)a[j-i]*a[i];
        }
        t+=t;
        t+=co;
        t+=(dchunk)a[j/2]*a[j/2];
        c[j]=(chunk)t&BMASK_XXX;
        co=t>>BASEBITS_XXX;
        j++;
    }

    for (j=NLEN_XXX-1+NLEN_XXX%2; j<DNLEN_XXX-3; )
    {
        t=(dchunk)a[NLEN_XXX-1]*a[j-NLEN_XXX+1];
        for (i=j-NLEN_XXX+2; i<(j+1)/2; i++)
        {
            t+=(dchunk)a[j-i]*a[i];
        }
        t+=t;
        t+=co;
        c[j]=(chunk)t&BMASK_XXX;
        co=t>>BASEBITS_XXX;
        j++;
        t=(dchunk)a[NLEN_XXX-1]*a[j-NLEN_XXX+1];
        for (i=j-NLEN_XXX+2; i<(j+1)/2; i++)
        {
            t+=(dchunk)a[j-i]*a[i];
        }
        t+=t;
        t+=co;
        t+=(dchunk)a[j/2]*a[j/2];
        c[j]=(chunk)t&BMASK_XXX;
        co=t>>BASEBITS_XXX;
        j++;
    }

    t=(dchunk)a[NLEN_XXX-2]*a[NLEN_XXX-1];
    t+=t;
    t+=co;
    c[DNLEN_XXX-3]=(chunk)t&BMASK_XXX;
    co=t>>BASEBITS_XXX;

    t=(dchunk)a[NLEN_XXX-1]*a[NLEN_XXX-1]+co;
    c[DNLEN_XXX-2]=(chunk)t&BMASK_XXX;
    co=t>>BASEBITS_XXX;
    c[DNLEN_XXX-1]=(chunk)co;


#endif

#else
    chunk carry;
    BIG_XXX_dzero(c);
    for (i=0; i<NLEN_XXX; i++)
    {
        carry=0;
        for (j=i+1; j<NLEN_XXX; j++)
            carry=muladd_XXX(a[i],a[j],carry,&c[i+j]);
        c[NLEN_XXX+i]=carry;
    }

    for (i=0; i<DNLEN_XXX; i++) c[i]*=2;

    for (i=0; i<NLEN_XXX; i++)
        c[2*i+1]+=muladd_XXX(a[i],a[i],0,&c[2*i]);

    BIG_XXX_dnorm(c);
#endif


#ifdef DEBUG_NORM
    c[DMPV_XXX]=1;
    c[DMNV_XXX]=0;
#endif

}

/* Montgomery reduction */
void BIG_XXX_monty(BIG_XXX a,BIG_XXX md,chunk MC,DBIG_XXX d)
{
    int i,k;

#ifdef dchunk
    dchunk t,c,s;
    dchunk dd[NLEN_XXX];
    chunk v[NLEN_XXX];
#endif

#ifdef COMBA

#ifdef UNWOUND

    /* Insert output of faster.c here */

#else

    t=d[0];
    v[0]=((chunk)t*MC)&BMASK_XXX;
    t+=(dchunk)v[0]*md[0];
    c=(t>>BASEBITS_XXX)+d[1];
    s=0;

    for (k=1; k<NLEN_XXX; k++)
    {
        t=c+s+(dchunk)v[0]*md[k];
        for (i=k-1; i>k/2; i--) t+=(dchunk)(v[k-i]-v[i])*(md[i]-md[k-i]);
        v[k]=((chunk)t*MC)&BMASK_XXX;
        t+=(dchunk)v[k]*md[0];
        c=(t>>BASEBITS_XXX)+d[k+1];
        dd[k]=(dchunk)v[k]*md[k];
        s+=dd[k];
    }
    for (k=NLEN_XXX; k<2*NLEN_XXX-1; k++)
    {
        t=c+s;
        for (i=NLEN_XXX-1; i>=1+k/2; i--) t+=(dchunk)(v[k-i]-v[i])*(md[i]-md[k-i]);
        a[k-NLEN_XXX]=(chunk)t&BMASK_XXX;
        c=(t>>BASEBITS_XXX)+d[k+1];
        s-=dd[k-NLEN_XXX+1];
    }
    a[NLEN_XXX-1]=(chunk)c&BMASK_XXX;

#endif



#else
    int j;
    chunk m,carry;
    for (i=0; i<NLEN_XXX; i++)
    {
        if (MC==-1) m=(-d[i])&BMASK_XXX;
        else
        {
            if (MC==1) m=d[i];
            else m=(MC*d[i])&BMASK_XXX;
        }
        carry=0;
        for (j=0; j<NLEN_XXX; j++)
            carry=muladd_XXX(m,md[j],carry,&d[i+j]);
        d[NLEN_XXX+i]+=carry;
    }
    BIG_XXX_sducopy(a,d);
    BIG_XXX_norm(a);

#endif

#ifdef DEBUG_NORM
    a[MPV_XXX]=1;
    a[MNV_XXX]=0;
#endif
}

/* General shift left of a by n bits */
/* a MUST be normalised */
/* SU= 32 */
void BIG_XXX_shl(BIG_XXX a,int k)
{
    int i;
    int n=k%BASEBITS_XXX;
    int m=k/BASEBITS_XXX;

    a[NLEN_XXX-1]=((a[NLEN_XXX-1-m]<<n));
    if (NLEN_XXX>=m+2) a[NLEN_XXX-1]|=(a[NLEN_XXX-m-2]>>(BASEBITS_XXX-n));

    for (i=NLEN_XXX-2; i>m; i--)
        a[i]=((a[i-m]<<n)&BMASK_XXX)|(a[i-m-1]>>(BASEBITS_XXX-n));
    a[m]=(a[0]<<n)&BMASK_XXX;
    for (i=0; i<m; i++) a[i]=0;

}

/* Fast shift left of a by n bits, where n less than a word, Return excess (but store it as well) */
/* a MUST be normalised */
/* SU= 16 */
int BIG_XXX_fshl(BIG_XXX a,int n)
{
    int i;

    a[NLEN_XXX-1]=((a[NLEN_XXX-1]<<n))|(a[NLEN_XXX-2]>>(BASEBITS_XXX-n)); /* top word not masked */
    for (i=NLEN_XXX-2; i>0; i--)
        a[i]=((a[i]<<n)&BMASK_XXX)|(a[i-1]>>(BASEBITS_XXX-n));
    a[0]=(a[0]<<n)&BMASK_XXX;

    return (int)(a[NLEN_XXX-1]>>((8*MODBYTES_XXX)%BASEBITS_XXX)); /* return excess - only used in ff.c */
}

/* double length left shift of a by k bits - k can be > BASEBITS , a MUST be normalised */
/* SU= 32 */
void BIG_XXX_dshl(DBIG_XXX a,int k)
{
    int i;
    int n=k%BASEBITS_XXX;
    int m=k/BASEBITS_XXX;

    a[DNLEN_XXX-1]=((a[DNLEN_XXX-1-m]<<n))|(a[DNLEN_XXX-m-2]>>(BASEBITS_XXX-n));

    for (i=DNLEN_XXX-2; i>m; i--)
        a[i]=((a[i-m]<<n)&BMASK_XXX)|(a[i-m-1]>>(BASEBITS_XXX-n));
    a[m]=(a[0]<<n)&BMASK_XXX;
    for (i=0; i<m; i++) a[i]=0;

}

/* General shift right of a by k bits */
/* a MUST be normalised */
/* SU= 32 */
void BIG_XXX_shr(BIG_XXX a,int k)
{
    int i;
    int n=k%BASEBITS_XXX;
    int m=k/BASEBITS_XXX;
    for (i=0; i<NLEN_XXX-m-1; i++)
        a[i]=(a[m+i]>>n)|((a[m+i+1]<<(BASEBITS_XXX-n))&BMASK_XXX);
    if (NLEN_XXX>m)  a[NLEN_XXX-m-1]=a[NLEN_XXX-1]>>n;
    for (i=NLEN_XXX-m; i<NLEN_XXX; i++) a[i]=0;

}

/* Fast combined shift, subtract and norm. Return sign of result */
int BIG_XXX_ssn(BIG_XXX r,BIG_XXX a,BIG_XXX m)
{
	int i,n=NLEN_XXX-1;
	chunk carry;
	m[0]=(m[0]>>1)|((m[1]<<(BASEBITS_XXX-1))&BMASK_XXX);
	r[0]=a[0]-m[0];
    carry=r[0]>>BASEBITS_XXX;
    r[0]&=BMASK_XXX;
    
	for (i=1;i<n;i++)
	{
		m[i]=(m[i]>>1)|((m[i+1]<<(BASEBITS_XXX-1))&BMASK_XXX);
		r[i]=a[i]-m[i]+carry;
		carry=r[i]>>BASEBITS_XXX;
		r[i]&=BMASK_XXX;
	}
	
	m[n]>>=1;
	r[n]=a[n]-m[n]+carry;
#ifdef DEBUG_NORM
    r[MPV_XXX]=1;
    r[MNV_XXX]=0;
#endif
	return ((r[n]>>(CHUNK-1))&1);
}

/* Faster shift right of a by k bits. Return shifted out part */
/* a MUST be normalised */
/* SU= 16 */
int BIG_XXX_fshr(BIG_XXX a,int k)
{
    int i;
    chunk r=a[0]&(((chunk)1<<k)-1); /* shifted out part */
    for (i=0; i<NLEN_XXX-1; i++)
        a[i]=(a[i]>>k)|((a[i+1]<<(BASEBITS_XXX-k))&BMASK_XXX);
    a[NLEN_XXX-1]=a[NLEN_XXX-1]>>k;
    return (int)r;
}

/* double length right shift of a by k bits - can be > BASEBITS */
/* SU= 32 */
void BIG_XXX_dshr(DBIG_XXX a,int k)
{
    int i;
    int n=k%BASEBITS_XXX;
    int m=k/BASEBITS_XXX;
    for (i=0; i<DNLEN_XXX-m-1; i++)
        a[i]=(a[m+i]>>n)|((a[m+i+1]<<(BASEBITS_XXX-n))&BMASK_XXX);
    a[DNLEN_XXX-m-1]=a[DNLEN_XXX-1]>>n;
    for (i=DNLEN_XXX-m; i<DNLEN_XXX; i++ ) a[i]=0;
}

/* Split DBIG d into two BIGs t|b. Split happens at n bits, where n falls into NLEN word */
/* d MUST be normalised */
/* SU= 24 */
chunk BIG_XXX_split(BIG_XXX t,BIG_XXX b,DBIG_XXX d,int n)
{
    int i;
    chunk nw,carry=0;
    int m=n%BASEBITS_XXX;

    if (m==0)
    {
        for (i=0; i<NLEN_XXX; i++) b[i]=d[i];
        if (t!=b)
        {
            for (i=NLEN_XXX; i<2*NLEN_XXX; i++) t[i-NLEN_XXX]=d[i];
            carry=t[NLEN_XXX-1]>>BASEBITS_XXX;
            t[NLEN_XXX-1]=t[NLEN_XXX-1]&BMASK_XXX; /* top word normalized */
        }
        return carry;
    }

    for (i=0; i<NLEN_XXX-1; i++) b[i]=d[i];

    b[NLEN_XXX-1]=d[NLEN_XXX-1]&(((chunk)1<<m)-1);

    if (t!=b)
    {
        carry=(d[DNLEN_XXX-1]<<(BASEBITS_XXX-m));
        for (i=DNLEN_XXX-2; i>=NLEN_XXX-1; i--)
        {
            nw=(d[i]>>m)|carry;
            carry=(d[i]<<(BASEBITS_XXX-m))&BMASK_XXX;
            t[i-NLEN_XXX+1]=nw;
        }
    }
#ifdef DEBUG_NORM
    t[MPV_XXX]=1;
    t[MNV_XXX]=0;
    b[MPV_XXX]=1;
    b[MNV_XXX]=0;
#endif
    return carry;
}

/* you gotta keep the sign of carry! Look - no branching! */
/* Note that sign bit is needed to disambiguate between +ve and -ve values */
/* normalise BIG - force all digits < 2^BASEBITS */
chunk BIG_XXX_norm(BIG_XXX a)
{
    int i;
    chunk d,carry=0;
    for (i=0; i<NLEN_XXX-1; i++)
    {
        d=a[i]+carry;
        a[i]=d&BMASK_XXX;
        carry=d>>BASEBITS_XXX;
    }
    a[NLEN_XXX-1]=(a[NLEN_XXX-1]+carry);

#ifdef DEBUG_NORM
    a[MPV_XXX]=1;
    a[MNV_XXX]=0;
#endif
    return (a[NLEN_XXX-1]>>((8*MODBYTES_XXX)%BASEBITS_XXX));  /* only used in ff.c */
}

void BIG_XXX_dnorm(DBIG_XXX a)
{
    int i;
    chunk d,carry=0;
    for (i=0; i<DNLEN_XXX-1; i++)
    {
        d=a[i]+carry;
        a[i]=d&BMASK_XXX;
        carry=d>>BASEBITS_XXX;
    }
    a[DNLEN_XXX-1]=(a[DNLEN_XXX-1]+carry);
#ifdef DEBUG_NORM
    a[DMPV_XXX]=1;
    a[DMNV_XXX]=0;
#endif
}

/* Compare a and b. Return 1 for a>b, -1 for a<b, 0 for a==b */
/* a and b MUST be normalised before call */
int BIG_XXX_comp(BIG_XXX a,BIG_XXX b)
{
    int i;
    for (i=NLEN_XXX-1; i>=0; i--)
    {
        if (a[i]==b[i]) continue;
        if (a[i]>b[i]) return 1;
        else  return -1;
    }
    return 0;
}

int BIG_XXX_dcomp(DBIG_XXX a,DBIG_XXX b)
{
    int i;
    for (i=DNLEN_XXX-1; i>=0; i--)
    {
        if (a[i]==b[i]) continue;
        if (a[i]>b[i]) return 1;
        else  return -1;
    }
    return 0;
}

/* return number of bits in a */
/* SU= 8 */
int BIG_XXX_nbits(BIG_XXX a)
{
    int bts,k=NLEN_XXX-1;
	BIG_XXX t;
    chunk c;
	BIG_XXX_copy(t,a);
    BIG_XXX_norm(t);
    while (k>=0 && t[k]==0) k--;
    if (k<0) return 0;
    bts=BASEBITS_XXX*k;
    c=t[k];
    while (c!=0)
    {
        c/=2;
        bts++;
    }
    return bts;
}

/* SU= 8, Calculate number of bits in a DBIG - output normalised */
int BIG_XXX_dnbits(DBIG_XXX a)
{
    int bts,k=DNLEN_XXX-1;
	DBIG_XXX t;
    chunk c;
	BIG_XXX_dcopy(t,a);
    BIG_XXX_dnorm(t);
    while (k>=0 && t[k]==0) k--;
    if (k<0) return 0;
    bts=BASEBITS_XXX*k;
    c=t[k];
    while (c!=0)
    {
        c/=2;
        bts++;
    }
    return bts;
}


/* Set b=b mod c */
/* SU= 16 */
void BIG_XXX_mod(BIG_XXX b,BIG_XXX c1)
{
    int k=0;
    BIG_XXX r; /**/
	BIG_XXX c;
	BIG_XXX_copy(c,c1);

    BIG_XXX_norm(b);
    if (BIG_XXX_comp(b,c)<0)
        return;
    do
    {
        BIG_XXX_fshl(c,1);
        k++;
    }
    while (BIG_XXX_comp(b,c)>=0);

    while (k>0)
    {
        BIG_XXX_fshr(c,1);

// constant time...
        BIG_XXX_sub(r,b,c);
        BIG_XXX_norm(r);
        BIG_XXX_cmove(b,r,1-((r[NLEN_XXX-1]>>(CHUNK-1))&1));
        k--;
    }
}

/* Set a=b mod c, b is destroyed. Slow but rarely used. */
/* SU= 96 */
void BIG_XXX_dmod(BIG_XXX a,DBIG_XXX b,BIG_XXX c)
{
    int k=0;
    DBIG_XXX m,r;
    BIG_XXX_dnorm(b);
    BIG_XXX_dscopy(m,c);

    if (BIG_XXX_dcomp(b,m)<0)
    {
        BIG_XXX_sdcopy(a,b);
        return;
    }

    do
    {
        BIG_XXX_dshl(m,1);
        k++;
    }
    while (BIG_XXX_dcomp(b,m)>=0);

    while (k>0)
    {
        BIG_XXX_dshr(m,1);
// constant time...
        BIG_XXX_dsub(r,b,m);
        BIG_XXX_dnorm(r);
        BIG_XXX_dcmove(b,r,1-((r[DNLEN_XXX-1]>>(CHUNK-1))&1));

        k--;
    }
    BIG_XXX_sdcopy(a,b);
}

/* Set a=b/c,  b is destroyed. Slow but rarely used. */
/* SU= 136 */

void BIG_XXX_ddiv(BIG_XXX a,DBIG_XXX b,BIG_XXX c)
{
    int d,k=0;
    DBIG_XXX m,dr;
    BIG_XXX e,r;
    BIG_XXX_dnorm(b);
    BIG_XXX_dscopy(m,c);

    BIG_XXX_zero(a);
    BIG_XXX_zero(e);
    BIG_XXX_inc(e,1);

    while (BIG_XXX_dcomp(b,m)>=0)
    {
        BIG_XXX_fshl(e,1);
        BIG_XXX_dshl(m,1);
        k++;
    }

    while (k>0)
    {
        BIG_XXX_dshr(m,1);
        BIG_XXX_fshr(e,1);

        BIG_XXX_dsub(dr,b,m);
        BIG_XXX_dnorm(dr);
        d=1-((dr[DNLEN_XXX-1]>>(CHUNK-1))&1);
        BIG_XXX_dcmove(b,dr,d);

        BIG_XXX_add(r,a,e);
        BIG_XXX_norm(r);
        BIG_XXX_cmove(a,r,d);

        k--;
    }
}

/* SU= 136 */

void BIG_XXX_sdiv(BIG_XXX a,BIG_XXX c)
{
    int d,k=0;
    BIG_XXX m,e,b,r;
    BIG_XXX_norm(a);
    BIG_XXX_copy(b,a);
    BIG_XXX_copy(m,c);

    BIG_XXX_zero(a);
    BIG_XXX_zero(e);
    BIG_XXX_inc(e,1);

    while (BIG_XXX_comp(b,m)>=0)
    {
        BIG_XXX_fshl(e,1);
        BIG_XXX_fshl(m,1);
        k++;
    }

    while (k>0)
    {
        BIG_XXX_fshr(m,1);
        BIG_XXX_fshr(e,1);

        BIG_XXX_sub(r,b,m);
        BIG_XXX_norm(r);
        d=1-((r[NLEN_XXX-1]>>(CHUNK-1))&1);
        BIG_XXX_cmove(b,r,d);

        BIG_XXX_add(r,a,e);
        BIG_XXX_norm(r);
        BIG_XXX_cmove(a,r,d);
        k--;
    }
}

/* return LSB of a */
int BIG_XXX_parity(BIG_XXX a)
{
    return a[0]%2;
}

/* return n-th bit of a */
/* SU= 16 */
int BIG_XXX_bit(BIG_XXX a,int n)
{
    if (a[n/BASEBITS_XXX]&((chunk)1<<(n%BASEBITS_XXX))) return 1;
    else return 0;
}

/* return last n bits of a, where n is small < BASEBITS */
/* SU= 16 */
int BIG_XXX_lastbits(BIG_XXX a,int n)
{
    int msk=(1<<n)-1;
    BIG_XXX_norm(a);
    return ((int)a[0])&msk;
}

/* get 8*MODBYTES size random number */
void BIG_XXX_random(BIG_XXX m,csprng *rng)
{
    int i,b,j=0,r=0;
    int len=8*MODBYTES_XXX;

    BIG_XXX_zero(m);
    /* generate random BIG */
    for (i=0; i<len; i++)
    {
        if (j==0) r=RAND_byte(rng);
        else r>>=1;
        b=r&1;
        BIG_XXX_shl(m,1);
        m[0]+=b;
        j++;
        j&=7;
    }

#ifdef DEBUG_NORM
    m[MPV_XXX]=1;
    m[MNV_XXX]=0;
#endif
}

/* get random BIG from rng, modulo q. Done one bit at a time, so its portable */

void BIG_XXX_randomnum(BIG_XXX m,BIG_XXX q,csprng *rng)
{
    int i,b,j=0,r=0;
    DBIG_XXX d;
    BIG_XXX_dzero(d);
    /* generate random DBIG */
    for (i=0; i<2*BIG_XXX_nbits(q); i++)
    {
        if (j==0) r=RAND_byte(rng);
        else r>>=1;
        b=r&1;
        BIG_XXX_dshl(d,1);
        d[0]+=b;
        j++;
        j&=7;
    }
    /* reduce modulo a BIG. Removes bias */
    BIG_XXX_dmod(m,d,q);
#ifdef DEBUG_NORM
    m[MPV_XXX]=1;
    m[MNV_XXX]=0;
#endif
}

/* create randum BIG less than r and less than trunc bits */
void BIG_XXX_randtrunc(BIG_XXX s,BIG_XXX r,int trunc,csprng *rng)
{
	BIG_XXX_randomnum(s,r,rng);
	if (BIG_XXX_nbits(r)>trunc)
		  BIG_XXX_mod2m(s,trunc);
}


/* Set r=a*b mod m */
/* SU= 96 */
void BIG_XXX_modmul(BIG_XXX r,BIG_XXX a1,BIG_XXX b1,BIG_XXX m)
{
    DBIG_XXX d;
	BIG_XXX a,b;
	BIG_XXX_copy(a,a1);
	BIG_XXX_copy(b,b1);
    BIG_XXX_mod(a,m);
    BIG_XXX_mod(b,m);

    BIG_XXX_mul(d,a,b);
    BIG_XXX_dmod(r,d,m);
}

/* Set a=a*a mod m */
/* SU= 88 */
void BIG_XXX_modsqr(BIG_XXX r,BIG_XXX a1,BIG_XXX m)
{
    DBIG_XXX d;
	BIG_XXX a;
	BIG_XXX_copy(a,a1);
    BIG_XXX_mod(a,m);
    BIG_XXX_sqr(d,a);
    BIG_XXX_dmod(r,d,m);
}

/* Set r=-a mod m */
/* SU= 16 */
void BIG_XXX_modneg(BIG_XXX r,BIG_XXX a1,BIG_XXX m)
{
	BIG_XXX a;
	BIG_XXX_copy(a,a1);
    BIG_XXX_mod(a,m);
    BIG_XXX_sub(r,m,a);
	BIG_XXX_mod(r,m);
}

/* Set a=a/b mod m */
/* SU= 136 */
void BIG_XXX_moddiv(BIG_XXX r,BIG_XXX a1,BIG_XXX b1,BIG_XXX m)
{
    DBIG_XXX d;
    BIG_XXX z;
	BIG_XXX a,b;
	BIG_XXX_copy(a,a1);
	BIG_XXX_copy(b,b1);

    BIG_XXX_mod(a,m);
    BIG_XXX_invmodp(z,b,m);

    BIG_XXX_mul(d,a,z);
    BIG_XXX_dmod(r,d,m);
}

/* Get jacobi Symbol (a/p). Returns 0, 1 or -1 */
/* SU= 216 */
int BIG_XXX_jacobi(BIG_XXX a,BIG_XXX p)
{
    int n8,k,m=0;
    BIG_XXX t,x,n,zilch,one;
    BIG_XXX_one(one);
    BIG_XXX_zero(zilch);
    if (BIG_XXX_parity(p)==0 || BIG_XXX_comp(a,zilch)==0 || BIG_XXX_comp(p,one)<=0) return 0;
    BIG_XXX_norm(a);
    BIG_XXX_copy(x,a);
    BIG_XXX_copy(n,p);
    BIG_XXX_mod(x,p);

    while (BIG_XXX_comp(n,one)>0)
    {
        if (BIG_XXX_comp(x,zilch)==0) return 0;
        n8=BIG_XXX_lastbits(n,3);
        k=0;
        while (BIG_XXX_parity(x)==0)
        {
            k++;
            BIG_XXX_shr(x,1);
        }
        if (k%2==1) m+=(n8*n8-1)/8;
        m+=(n8-1)*(BIG_XXX_lastbits(x,2)-1)/4;
        BIG_XXX_copy(t,n);

        BIG_XXX_mod(t,x);
        BIG_XXX_copy(n,x);
        BIG_XXX_copy(x,t);
        m%=2;

    }
    if (m==0) return 1;
    else return -1;
}

/* Set r=1/a mod p. Binary method */
/* SU= 240 */
void BIG_XXX_invmodp(BIG_XXX r,BIG_XXX a,BIG_XXX p)
{
    BIG_XXX u,v,x1,x2,t,one;
    BIG_XXX_mod(a,p);
    BIG_XXX_copy(u,a);
    BIG_XXX_copy(v,p);
    BIG_XXX_one(one);
    BIG_XXX_copy(x1,one);
    BIG_XXX_zero(x2);

    while (BIG_XXX_comp(u,one)!=0 && BIG_XXX_comp(v,one)!=0)
    {
        while (BIG_XXX_parity(u)==0)
        {
            BIG_XXX_fshr(u,1);
            if (BIG_XXX_parity(x1)!=0)
            {
                BIG_XXX_add(x1,p,x1);
                BIG_XXX_norm(x1);
            }
            BIG_XXX_fshr(x1,1);
        }
        while (BIG_XXX_parity(v)==0)
        {
            BIG_XXX_fshr(v,1);
            if (BIG_XXX_parity(x2)!=0)
            {
                BIG_XXX_add(x2,p,x2);
                BIG_XXX_norm(x2);
            }
            BIG_XXX_fshr(x2,1);
        }
        if (BIG_XXX_comp(u,v)>=0)
        {
            BIG_XXX_sub(u,u,v);
            BIG_XXX_norm(u);
            if (BIG_XXX_comp(x1,x2)>=0) BIG_XXX_sub(x1,x1,x2);
            else
            {
                BIG_XXX_sub(t,p,x2);
                BIG_XXX_add(x1,x1,t);
            }
            BIG_XXX_norm(x1);
        }
        else
        {
            BIG_XXX_sub(v,v,u);
            BIG_XXX_norm(v);
            if (BIG_XXX_comp(x2,x1)>=0) BIG_XXX_sub(x2,x2,x1);
            else
            {
                BIG_XXX_sub(t,p,x1);
                BIG_XXX_add(x2,x2,t);
            }
            BIG_XXX_norm(x2);
        }
    }
    if (BIG_XXX_comp(u,one)==0)
        BIG_XXX_copy(r,x1);
    else
        BIG_XXX_copy(r,x2);
}

/* set x = x mod 2^m */
void BIG_XXX_mod2m(BIG_XXX x,int m)
{
    int i,wd,bt;
    chunk msk;
    BIG_XXX_norm(x);

    wd=m/BASEBITS_XXX;
    bt=m%BASEBITS_XXX;
    msk=((chunk)1<<bt)-1;
    x[wd]&=msk;
    for (i=wd+1; i<NLEN_XXX; i++) x[i]=0;
}

// new
/* Convert to DBIG number from byte array of given length */
void BIG_XXX_dfromBytesLen(DBIG_XXX a,char *b,int s)
{
    int i,len=s;
    BIG_XXX_dzero(a);

    for (i=0; i<len; i++)
    {
        BIG_XXX_dshl(a,8);
        a[0]+=(int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[DMPV_XXX]=1;
    a[DMNV_XXX]=0;
#endif
}
