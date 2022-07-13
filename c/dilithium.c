/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Dilithium API implementation. Constant time where it matters. Slow (spends nearly all of its time running SHA3) but small.

Note that the Matrix A is calculated on-the-fly to keep memory requirement minimal
But this makes all stages much slower
Note that 
1. Matrix A can just be generated randomly for Key generation (without using SHA3 which is very slow)
2. A precalculated A can be included in the public key, for use by signature and verification (which blows up public key size)
3. Precalculating A for signature calculation means that the A does not have to re-calculated for each attempt to find a good signature

Might be simpler to wait for hardware support for SHA3!

   M.Scott 30/09/2021
*/

#include "dilithium.h"

#define round(a,b) (((a)+((b)/2))/(b))

// parameters for each security level
// tau,gamma1,gamma2,K,L,eta,lg(2*eta+1),omega
const int PARAMS_2[8]={39,17,88,4,4,2,3,80};
const int PARAMS_3[8]={49,19,32,6,5,4,4,55};
const int PARAMS_5[8]={60,19,32,8,7,2,3,75};

const sign32 roots[] = {0x3ffe00,0x64f7,0x581103,0x77f504,0x39e44,0x740119,0x728129,0x71e24,0x1bde2b,0x23e92b,0x7a64ae,0x5ff480,0x2f9a75,0x53db0a,0x2f7a49,0x28e527,0x299658,0xfa070,0x6f65a5,0x36b788,0x777d91,0x6ecaa1,0x27f968,0x5fb37c,0x5f8dd7,0x44fae8,0x6a84f8,0x4ddc99,0x1ad035,0x7f9423,0x3d3201,0x445c5,0x294a67,0x17620,0x2ef4cd,0x35dec5,0x668504,0x49102d,0x5927d5,0x3bbeaf,0x44f586,0x516e7d,0x368a96,0x541e42,0x360400,0x7b4a4e,0x23d69c,0x77a55e,0x65f23e,0x66cad7,0x357e1e,0x458f5a,0x35843f,0x5f3618,0x67745d,0x38738c,0xc63a8,0x81b9a,0xe8f76,0x3b3853,0x3b8534,0x58dc31,0x1f9d54,0x552f2e,0x43e6e6,0x688c82,0x47c1d0,0x51781a,0x69b65e,0x3509ee,0x2135c7,0x67afbc,0x6caf76,0x1d9772,0x419073,0x709cf7,0x4f3281,0x4fb2af,0x4870e1,0x1efca,0x3410f2,0x70de86,0x20c638,0x296e9f,0x5297a4,0x47844c,0x799a6e,0x5a140a,0x75a283,0x6d2114,0x7f863c,0x6be9f8,0x7a0bde,0x1495d4,0x1c4563,0x6a0c63,0x4cdbea,0x40af0,0x7c417,0x2f4588,0xad00,0x6f16bf,0xdcd44,0x3c675a,0x470bcb,0x7fbe7f,0x193948,0x4e49c1,0x24756c,0x7ca7e0,0xb98a1,0x6bc809,0x2e46c,0x49a809,0x3036c2,0x639ff7,0x5b1c94,0x7d2ae1,0x141305,0x147792,0x139e25,0x67b0e1,0x737945,0x69e803,0x51cea3,0x44a79d,0x488058,0x3a97d9,0x1fea93,0x33ff5a,0x2358d4,0x3a41f8,0x4cdf73,0x223dfb,0x5a8ba0,0x498423,0x412f5,0x252587,0x6d04f1,0x359b5d,0x4a28a1,0x4682fd,0x6d9b57,0x4f25df,0xdbe5e,0x1c5e1a,0xde0e6,0xc7f5a,0x78f83,0x67428b,0x7f3705,0x77e6fd,0x75e022,0x503af7,0x1f0084,0x30ef86,0x49997e,0x77dcd7,0x742593,0x4901c3,0x53919,0x4610c,0x5aad42,0x3eb01b,0x3472e7,0x4ce03c,0x1a7cc7,0x31924,0x2b5ee5,0x291199,0x585a3b,0x134d71,0x3de11c,0x130984,0x25f051,0x185a46,0x466519,0x1314be,0x283891,0x49bb91,0x52308a,0x1c853f,0x1d0b4b,0x6fd6a7,0x6b88bf,0x12e11b,0x4d3e3f,0x6a0d30,0x78fde5,0x1406c7,0x327283,0x61ed6f,0x6c5954,0x1d4099,0x590579,0x6ae5ae,0x16e405,0xbdbe7,0x221de8,0x33f8cf,0x779935,0x54aa0d,0x665ff9,0x63b158,0x58711c,0x470c13,0x910d8,0x463e20,0x612659,0x251d8b,0x2573b7,0x7d5c90,0x1ddd98,0x336898,0x2d4bb,0x6d73a8,0x4f4cbf,0x27c1c,0x18aa08,0x2dfd71,0xc5ca5,0x19379a,0x478168,0x646c3e,0x51813d,0x35c539,0x3b0115,0x41dc0,0x21c4f7,0x70fbf5,0x1a35e7,0x7340e,0x795d46,0x1a4cd0,0x645caf,0x1d2668,0x666e99,0x6f0634,0x7be5db,0x455fdc,0x530765,0x5dc1b0,0x7973de,0x5cfd0a,0x2cc93,0x70f806,0x189c2a,0x49c5aa,0x776a51,0x3bcf2c,0x7f234f,0x6b16e0,0x3c15ca,0x155e68,0x72f6b7,0x1e29ce};
const sign32 iroots[] = {0x3ffe00,0x7f7b0a,0x7eafd,0x27cefe,0x78c1dd,0xd5ed8,0xbdee8,0x7c41bd,0x56fada,0x5065b8,0x2c04f7,0x50458c,0x1feb81,0x57b53,0x5bf6d6,0x6401d6,0x7b9a3c,0x42ae00,0x4bde,0x650fcc,0x320368,0x155b09,0x3ae519,0x20522a,0x202c85,0x57e699,0x111560,0x86270,0x492879,0x107a5c,0x703f91,0x5649a9,0x2ab0d3,0x6042ad,0x2703d0,0x445acd,0x44a7ae,0x71508b,0x77c467,0x737c59,0x476c75,0x186ba4,0x20a9e9,0x4a5bc2,0x3a50a7,0x4a61e3,0x19152a,0x19edc3,0x83aa3,0x5c0965,0x495b3,0x49dc01,0x2bc1bf,0x49556b,0x2e7184,0x3aea7b,0x442152,0x26b82c,0x36cfd4,0x195afd,0x4a013c,0x50eb34,0x7e69e1,0x56959a,0x454828,0x375fa9,0x3b3864,0x2e115e,0x15f7fe,0xc66bc,0x182f20,0x6c41dc,0x6b686f,0x6bccfc,0x2b520,0x24c36d,0x1c400a,0x4fa93f,0x3637f8,0x7cfb95,0x1417f8,0x744760,0x33821,0x5b6a95,0x319640,0x66a6b9,0x2182,0x38d436,0x4378a7,0x7212bd,0x10c942,0x7f3301,0x509a79,0x781bea,0x7bd511,0x330417,0x15d39e,0x639a9e,0x6b4a2d,0x5d423,0x13f609,0x59c5,0x12beed,0xa3d7e,0x25cbf7,0x64593,0x385bb5,0x2d485d,0x567162,0x5f19c9,0xf017b,0x4bcf0f,0x7df037,0x376f20,0x302d52,0x30ad80,0xf430a,0x3e4f8e,0x62488f,0x13308b,0x183045,0x5eaa3a,0x4ad613,0x1629a3,0x2e67e7,0x381e31,0x17537f,0x3bf91b,0x61b633,0xce94a,0x6a8199,0x43ca37,0x14c921,0xbcb2,0x4410d5,0x875b0,0x361a57,0x6743d7,0xee7fb,0x7d136e,0x22e2f7,0x66c23,0x221e51,0x2cd89c,0x3a8025,0x3fa26,0x10d9cd,0x197168,0x62b999,0x1b8352,0x659331,0x682bb,0x78abf3,0x65aa1a,0xee40c,0x5e1b0a,0x7bc241,0x44deec,0x4a1ac8,0x2e5ec4,0x1b73c3,0x385e99,0x66a867,0x73835c,0x51e290,0x6735f9,0x7d63e5,0x309342,0x126c59,0x7d0b46,0x4c7769,0x620269,0x28371,0x5a6c4a,0x5ac276,0x1eb9a8,0x39a1e1,0x76cf29,0x38d3ee,0x276ee5,0x1c2ea9,0x198008,0x2b35f4,0x846cc,0x4be732,0x5dc219,0x74041a,0x68fbfc,0x14fa53,0x26da88,0x629f68,0x1386ad,0x1df292,0x4d6d7e,0x6bd93a,0x6e21c,0x15d2d1,0x32a1c2,0x6cfee6,0x145742,0x10095a,0x62d4b6,0x635ac2,0x2daf77,0x362470,0x57a770,0x6ccb43,0x397ae8,0x6785bb,0x59efb0,0x6cd67d,0x41fee5,0x6c9290,0x2785c6,0x56ce68,0x54811c,0x7cc6dd,0x65633a,0x32ffc5,0x4b6d1a,0x412fe6,0x2532bf,0x7b7ef5,0x7aa6e8,0x36de3e,0xbba6e,0x8032a,0x364683,0x4ef07b,0x60df7d,0x2fa50a,0x9ffdf,0x7f904,0xa8fc,0x189d76,0x78507e,0x7360a7,0x71ff1b,0x6381e7,0x7221a3,0x30ba22,0x1244aa,0x395d04,0x35b760,0x4a44a4,0x12db10,0x5aba7a,0x7bcd0c,0x365bde,0x255461,0x5da206,0x33008e,0x459e09,0x5c872d,0x4be0a7,0x5ff56e};

/* Montgomery stuff */

static sign32 redc(unsign64 T)
{
    unsign32 m = (unsign32)T * (unsign32)DL_ND;
    return ((unsign64)m * DL_PRIME + T) >> 32;
}

static sign32 nres(unsign32 x)
{
    return redc((unsign64)x * DL_R2MODP);
}

static sign32 modmul(unsign32 a, unsign32 b)
{
    return redc((unsign64)a * b);
}

// make all elements +ve
static void poly_pos(sign32 *p)
{
    int i;
    for (i=0;i<DL_DEGREE;i++)
        p[i]+=(p[i]>>31)&DL_PRIME; 
}

// NTT code

// Important!
// nres(x); ntt(x)
// nres(y); ntt(y)
// z=x*y
// intt(z);
// redc(z);

// is equivalent to (note that nres() and redc() cancel out)

// ntt(x);
// nres(y); ntt(y);
// z=x*y
// intt(z)

// is equivalent to

// ntt(x)
// ntt(y)
// z=x*y
// intt(z)
// nres(z)

// In all cases z ends up in normal (non-Montgomery) form!
// So the conversion to Montgomery form can be "pushed" through the calculation.

// Here intt(z) <- intt(z);nres(z); 
// Combining is more efficient

/* NTT code */
/* Cooley-Tukey NTT */
/* Excess of 2 allowed on input - coefficients must be < 2*PRIME */

static void ntt(sign32 *x)
{
    int m, i, j, start, len = DL_DEGREE / 2;
    sign32 S, V, q = DL_PRIME;

    /* Make positive */
    poly_pos(x);
    m = 1;
    while (m < DL_DEGREE)
    {
        start = 0;
        for (i = 0; i < m; i++)
        {
            S = roots[m + i];
            for (j = start; j < start + len; j++)
            {
                V = modmul(x[j + len], S);
                x[j + len] = x[j] + 2 * q - V;
                x[j] = x[j] + V;
            }
            start += 2 * len;
        }
        len /= 2;
        m *= 2;
    }
}

/* Gentleman-Sande INTT */
/* Excess of 2 allowed on input - coefficients must be < 2*PRIME */
/* Output fully reduced */
#define NTTL 2 // maybe could be 1?

static void intt(sign32 *x)
{
    int m, i, j, k, n,lim,t = 1;
    sign32 S, U, V, W, q = DL_PRIME;

    m = DL_DEGREE/2;
    n=DL_LGN;
    while (m >= 1)
    {
		lim=NTTL>>n;
		n--;
        k = 0;
        for (i = 0; i < m; i++)
        {
            S = iroots[m + i];
            for (j = k; j < k + t; j++)
            {
				if (m<NTTL && j<k+lim)   // This should be unwound for timings
				{ // need to knock back excesses. Never used if NTTL=1.
					U=modmul(x[j],DL_ONE);  
					V=modmul(x[j+t],DL_ONE); 
				} else {
                    U = x[j];
                    V = x[j + t];
                }
                x[j] = U + V;
                W = U + (DL_DEGREE/NTTL) * q - V; 
                x[j + t] = modmul(W, S); 
            }
            k += 2 * t;
        }
        t *= 2;
        m /= 2;
    }

    for (j = 0; j < DL_DEGREE; j++)
    { // fully reduce, nres combined with 1/DEGREE
        x[j]=  modmul(x[j],DL_COMBO);
        x[j] -= q;
        x[j] += (x[j] >> 31)&q;
    } 
}

static void nres_it(sign32 *p)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p[i] = nres(p[i]);
}

static void redc_it(sign32 *p)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p[i] = redc(p[i]);
}

// copy polynomial
static void poly_copy(sign32 *p1, sign32 *p2)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p1[i] = p2[i];
}

// copy from small polynomial
static void poly_scopy(sign32 *p1, sign8 *p2)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p1[i] = (sign32)p2[i];
}

// copy from medium polynomial
static void poly_mcopy(sign32 *p1, sign16 *p2)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p1[i] = (sign32)p2[i];
}

static void poly_zero(sign32 *p1)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p1[i] = 0;
}

static void poly_negate(sign32 *p1,sign32 *p2)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p1[i] = DL_PRIME-p2[i];
}

static void poly_mul(sign32 *p1, sign32 *p2, sign32 *p3)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p1[i] = modmul(p2[i], p3[i]);
}

static void poly_add(sign32 *p1, sign32 *p2, sign32 *p3)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p1[i] = (p2[i] + p3[i]);
}

static void poly_sub(sign32 *p1, sign32 *p2, sign32 *p3)
{
    int i;
    for (i = 0; i < DL_DEGREE; i++)
        p1[i] = (p2[i] + DL_PRIME - p3[i]);
}

/* reduces inputs that are already < 2q */
static void poly_soft_reduce(sign32 *poly)
{
    int i;
    sign32 e;
    for (i = 0; i < DL_DEGREE; i++)
    {
        e = poly[i] - DL_PRIME;
        poly[i] = e + ((e >> 31)&DL_PRIME);
    }
}

/* fully reduces modulo q */
static void poly_hard_reduce(sign32 *poly)
{
    int i;
    sign32 e;
    for (i = 0; i < DL_DEGREE; i++)
    {
        e = modmul(poly[i], DL_ONE); // reduces to < 2q
        e = e - DL_PRIME;
        poly[i] = e + ((e >> 31)&DL_PRIME); // finishes it off
    }
}

// Generate A[i][j] from rho
static void ExpandAij(byte rho[32],sign32 Aij[],int i,int j)
{
    sha3 sh;
    int m,n;
    unsign32 b0,b1,b2;
    sign32 cf;
    SHA3_init(&sh, SHAKE128);
    byte buff[4*DL_DEGREE];  // should be plenty
    for (m=0;m<32;m++)
        SHA3_process(&sh,rho[m]);
    SHA3_process(&sh,j&0xff);
    SHA3_process(&sh,i&0xff);
    SHA3_shake(&sh,(char *)buff,4*DL_DEGREE);
    m=n=0;
    while (m<DL_DEGREE)
    {
        b0=(unsign32)buff[n++]; b1=(unsign32)buff[n++]; b2=(unsign32)buff[n++]; 
        cf=((b2&0x7F)<<16)+(b1<<8)+b0;
        if (cf>=DL_PRIME) continue;
        Aij[m++]=cf;
    }
}

// array t has ab active bits per word
// extract bytes from array of words
// if max!=0 then -max<=t[i]<=+max
static byte nextbyte32(int ab,int max,sign32 t[],int *ptr, int *bts)
{
    sign32 r,w;
    int left=ab-(*bts); // number of bits left in this word
    int i=0;
    w=t[*ptr];
    if (max!=0)
        w=max-w;
    r=w>>(*bts);
    while (left<8)
    {
        i++;
        w=t[(*ptr)+i];
        if (max!=0)
            w=max-w;
        r|=w<<left;
        left+=ab;
    }

    *bts+=8;
    while ((*bts)>=ab)
    {
        *bts-=ab;
        (*ptr)++;
    }
    return (byte)r&0xff;        
}

// array t has ab active bits per word
// extract dense bytes from array of words
// if max!=0 then -max<=t[i]<=+max
static byte nextbyte16(int ab,int max,sign16 t[],int *ptr, int *bts)
{
    sign32 r,w;
    int left=ab-(*bts); // number of bits left in this word
    int i=0;
    w=t[*ptr];
    if (max!=0)
        w=max-w;
    r=w>>(*bts);
    while (left<8)
    {
        i++;
        w=t[(*ptr)+i];
        if (max!=0)
            w=max-w;
        r|=w<<left;
        left+=ab;
    }

    *bts+=8;
    while ((*bts)>=ab)
    {
        *bts-=ab;
        (*ptr)++;
    }
    return (byte)r&0xff;        
}

// array t has ab active bits per word
// extract dense bytes from array of words
// if max!=0 then -max<=t[i]<=+max
static byte nextbyte8(int ab,int max,sign8 t[],int *ptr, int *bts)
{
    sign32 r,w;
    int left=ab-(*bts); // number of bits left in this word
    int i=0;
    w=t[*ptr];
    if (max!=0)
        w=max-w;
    r=w>>(*bts);
    while (left<8)
    {
        i++;
        w=t[(*ptr)+i];
        if (max!=0)
            w=max-w;
        r|=w<<left;
        left+=ab;
    }

    *bts+=8;
    while ((*bts)>=ab)
    {
        *bts-=ab;
        (*ptr)++;
    }
    return (byte)r&0xff;        
}

// extract ab bits into word from dense byte stream
static sign32 nextword(int ab,int max,byte t[],int *ptr, int *bts)
{
    sign32 r=t[*ptr]>>(*bts);
    sign32 mask=(1<<ab)-1;
    sign32 w;
    int i=0;
    int gotbits=8-(*bts); // bits left in current byte
    while (gotbits<ab)
    {
        i++;
        w=(sign32)t[(*ptr)+i];
        r|=w<<gotbits;
        gotbits+=8;
    }
    *bts+=ab;
    while ((*bts)>=8)
    {
        *bts-=8;
        (*ptr)++;
    }
    w=r&mask;
    if (max!=0)
     w=max-w;
    return w;  
}

// pack public key
static int pack_pk(const int *params,byte pk[],byte rho[32],sign16 t1[])
{
    int ptr,bts,i,n=0;
    int ck=params[3];
    for (i=0;i<32;i++)
        pk[i]=rho[i];
    n=32; ptr=bts=0;
    for (i=0;i<(ck*DL_DEGREE*DL_TD)/8;i++ )
        pk[n++]=nextbyte16(DL_TD,0,t1,&ptr,&bts);
    return n;
}

// unpack public key
static void unpack_pk(const int *params,byte rho[32],sign16 t1[],byte pk[])
{
    int ptr,bts,i;
    int ck=params[3];
    for (i=0;i<32;i++)
        rho[i]=pk[i];
    ptr=bts=0;
    for (i=0;i<ck*DL_DEGREE;i++ )
        t1[i]=(sign16)nextword(DL_TD,0,&pk[32],&ptr,&bts);
}

// secret key of size 32*3+DEGREE*(K*D+L*LG2ETA1+K*LG2ETA1)/8
static int pack_sk(const int *params,byte sk[],byte rho[32],byte bK[32],byte tr[32],sign8 s1[],sign8 s2[],sign16 t0[])
{
    int ptr,bts,i,n=32;
    int ck=params[3];
    int el=params[4];
    int eta=params[5];
    int lg2eta1=params[6];

    for (i=0;i<32;i++)
        sk[i]=rho[i];
    for (i=0;i<32;i++)
        sk[n++]=bK[i];
    for (i=0;i<32;i++)
        sk[n++]=tr[i];
    ptr=bts=0;
    for (i=0;i<(el*DL_DEGREE*lg2eta1)/8;i++)
        sk[n++]=nextbyte8(lg2eta1,eta,s1,&ptr,&bts);
    ptr=bts=0;
    for (i=0;i<(ck*DL_DEGREE*lg2eta1)/8;i++)
        sk[n++]=nextbyte8(lg2eta1,eta,s2,&ptr,&bts);
    ptr=bts=0;
    for (i=0;i<(ck*DL_DEGREE*DL_D)/8;i++)
        sk[n++]=nextbyte16(DL_D,(1<<(DL_D-1)),t0,&ptr,&bts);
    return n;
}

static void unpack_sk(const int *params,byte rho[32],byte bK[32],byte tr[32],sign8 s1[],sign8 s2[],sign16 t0[],byte sk[])
{
    int ptr,bts,i,n=32;
    int ck=params[3];
    int el=params[4];
    int eta=params[5];
    int lg2eta1=params[6];

    for (i=0;i<32;i++)
        rho[i]=sk[i];
    for (i=0;i<32;i++)
        bK[i]=sk[n++];
    for (i=0;i<32;i++)
        tr[i]=sk[n++];
    ptr=bts=0; 
    for (i=0;i<el*DL_DEGREE;i++ )
        s1[i]=(sign8)nextword(lg2eta1,eta,&sk[n],&ptr,&bts);
    n+=ptr;
    ptr=bts=0; 
    for (i=0;i<ck*DL_DEGREE;i++ )
        s2[i]=(sign8)nextword(lg2eta1,eta,&sk[n],&ptr,&bts);
    n+=ptr;
    ptr=bts=0;
    for (i=0;i<ck*DL_DEGREE;i++ )
        t0[i]=(sign16)nextword(DL_D,(1<<(DL_D-1)),&sk[n],&ptr,&bts);
}

// pack signature - changes z 
static int pack_sig(const int *params,byte sig[],sign32 z[],byte ct[],byte h[])
{
    int lg=params[1];
    int gamma1=1<<lg;
    int ck=params[3];
    int el=params[4];
    int omega=params[7];
    int ptr,bts,i,m,n,row,k=(el*DL_DEGREE*(lg+1))/8;
    sign32 t;

    for (i=0;i<32;i++)
      sig[i]=ct[i];   
    n=32;
    ptr=bts=0;
//pre-process z
    for (i=0;i<el;i++)
    {
        row=DL_DEGREE*i;
        for (m=0;m<DL_DEGREE;m++)
        {
            t=z[row+m];
            if (t>DL_PRIME/2) t-=DL_PRIME;
            t=gamma1-t;
            z[row+m]=t;
        }
    }
    for (i=0;i<k;i++) {
        sig[n++]=nextbyte32(lg+1,0,z,&ptr,&bts);
    }
    for (i=0;i<omega+ck;i++)
        sig[n++]=h[i];
    return n;
}

static void unpack_sig(const int *params,sign32 z[],byte ct[],byte h[],byte sig[])
{
    int lg=params[1];
    int gamma1=1<<lg;
    int ck=params[3];
    int el=params[4];
    int omega=params[7];

    int ptr,bts,i,n=el*DL_DEGREE;
    sign32 t;
    int m=32+(el*DL_DEGREE*(lg+1))/8;
    for (i=0;i<32;i++)
        ct[i]=sig[i];

    ptr=bts=0;
    for (i=0;i<n;i++) {
        t=nextword(lg+1,0,&sig[32],&ptr,&bts);
        t=gamma1-t;
        if (t<0) t+=DL_PRIME;
        z[i]=t;
    }

    for (i=0;i<omega+ck;i++)
        h[i]=sig[m++];    
}

// rejection sampling in range -ETA to +ETA
static void sample_Sn(const int *params,byte rhod[64],sign8 s[],int n)
{
    int m,ptr,bts;
    int eta=params[5];
    int lg2eta1=params[6];
    sha3 sh;
    byte buff[272];
    SHA3_init(&sh, SHAKE256);
    for (m=0;m<64;m++)
        SHA3_process(&sh,rhod[m]);
    SHA3_process(&sh,n&0xff);
    SHA3_process(&sh,(n>>8)&0xff);
    SHA3_shake(&sh,(char *)buff,272);
    ptr=bts=0;
    for (m=0;m<DL_DEGREE;m++)
    {
        do
        {
            s[m]=nextword(lg2eta1,0,buff,&ptr,&bts);
        }
        while (s[m]>2*eta);
        s[m]=eta-s[m];
    }
}

// uniform random sampling
static void sample_Y(const int *params,int k,byte rhod[64],sign32 y[])
{
    int i,j,m,ki,row,ptr,bts;   // 2^n-1
    int lg=params[1];
    int gamma1=1<<lg;
    int el=params[4];
    sha3 sh;
    sign32 w,t;
    byte buff[DL_YBYTES];

    for (i=0;i<el;i++)
    {
        row=DL_DEGREE*i;
        SHA3_init(&sh, SHAKE256);
        for (j=0;j<64;j++)
            SHA3_process(&sh,rhod[j]);
        ki=k+i;
        SHA3_process(&sh,ki&0xff);
        SHA3_process(&sh,ki>>8);
        SHA3_shake(&sh,(char *)buff,DL_YBYTES);

        ptr=bts=0;
        for (m=0;m<DL_DEGREE;m++)
        {  // take in LG+1 bits at a time
            w=nextword(lg+1,0,buff,&ptr,&bts);  // 20 bits
            w=gamma1-w;
            t=w>>31;
            y[row+m]=w+(DL_PRIME&t);
        }
    }
}

// CRH(rho,t1)
static void CRH1(const int *params,byte H[32],byte rho[32],sign16 t1[])
{
    int i;
    int ptr,bts;
    int ck=params[3];
    sha3 sh;
    SHA3_init(&sh, SHAKE256);
    for (i=0;i<32;i++)
        SHA3_process(&sh,rho[i]);
    ptr=bts=0;
    for (i=0;i<(ck*DL_DEGREE*DL_TD)/8;i++)
            SHA3_process(&sh,nextbyte16(DL_TD,0,t1,&ptr,&bts));
    SHA3_shake(&sh,(char *)H,32);
}

// CRH(tr,M)
static void CRH2(byte H[64],byte tr[32],byte mess[],int mlen)
{
    int i;
    sha3 sh;
    SHA3_init(&sh, SHAKE256);
    for (i=0;i<32;i++)
        SHA3_process(&sh,tr[i]);
    for (i=0;i<mlen;i++)
        SHA3_process(&sh,mess[i]);
    SHA3_shake(&sh,(char *)H,64);
}

// CRH(K,mu)
static void CRH3(byte H[64],byte bK[32],byte mu[64])
{
    int i;
    sha3 sh;
    SHA3_init(&sh, SHAKE256);
    for (i=0;i<32;i++)
        SHA3_process(&sh,bK[i]);
    for (i=0;i<64;i++)
        SHA3_process(&sh,mu[i]);
    SHA3_shake(&sh,(char *)H,64);
}

// H(mu,w1)
static void H4(const int *params,byte CT[32],byte mu[64],sign8 w1[])
{
    int i;
    int ptr,bts;
    int ck=params[3];
    int dv=params[2];
    sha3 sh;
    int w1b=4;
    if (dv==88) w1b=6;
    SHA3_init(&sh, SHAKE256);
    for (i=0;i<64;i++)
        SHA3_process(&sh,mu[i]);
    ptr=bts=0;
    for (i=0;i<(ck*DL_DEGREE*w1b)/8;i++)
        SHA3_process(&sh,nextbyte8(w1b,0,w1,&ptr,&bts));
    SHA3_shake(&sh,(char *)CT,32);
}

static void SampleInBall(const int *params,byte ct[32],sign32 c[DL_DEGREE])
{
    int i,j,k,n,b,sn;
    int tau=params[0];
    byte signs[8],buff[136];
    sha3 sh;
    SHA3_init(&sh, SHAKE256);
    for (i=0;i<32;i++)
        SHA3_process(&sh,ct[i]);
    SHA3_shake(&sh,(char *)buff,136);
    for (i=0;i<8;i++)
        signs[i]=buff[i];
    k=8; 
    b=0;
    poly_zero(c);
    sn=signs[0]; n=1;
    for (i=DL_DEGREE-tau;i<DL_DEGREE;i++)
    {
        do
        {
            j=buff[k++];
        } while (j>i);
        c[i]=c[j];
        c[j]=1-2*((sign32)sn&1);
        sn>>=1; b++;
        if (b==8) {
            sn=signs[n++]; b=0;
        }
    }
}

static sign16 p2r(sign32 *r0)
{
    sign32 d=(1<<DL_D);
    sign32 r1;
    r1=(*r0+d/2-1)>>DL_D;  
    *r0-=(r1 << DL_D);
    return (sign16)r1;
}

static void Power2Round(sign32 t[],sign16 t0[],sign16 t1[])
{
    int m;
    sign32 w;
    for (m=0;m<DL_DEGREE;m++)
    {   
        w=t[m];
        t1[m]=p2r(&w);
        t0[m]=(sign16)w;
    }
}

// ALPHA = (Q-1)/16 - borrowed from dilithium ref implementation
static sign32 decompose_lo(const int *params,sign32 a) {
    int gamma2,dv=params[2];
    sign32 a0,a1;
    a1  = (a + 127) >> 7;

    if (dv==32)
    {
        a1  = (a1*1025 + (1 << 21)) >> 22;
        a1 &= 15;
        gamma2=(DL_PRIME-1)/32;
    } else { // 88
        a1  = (a1*11275 + (1 << 23)) >> 24;
        a1 ^= ((43 - a1) >> 31) & a1;
        gamma2=(DL_PRIME-1)/88;
    }

    a0  = a - a1*2*gamma2;  // (Q-1)/2R=alpha
    a0 -= (((DL_PRIME-1)/2 - a0) >> 31) & DL_PRIME;
    a0 += (a0>>31)&DL_PRIME;
    return a0;
}

// ALPHA = (Q-1)/16
static sign8 decompose_hi(const int *params,sign32 a) {
    int dv=params[2];
    sign32 a1;
    a1  = (a + 127) >> 7;
    if (dv==32) {
        a1  = (a1*1025 + (1 << 21)) >> 22;
        a1 &= 15;
    } else {
        a1  = (a1*11275 + (1 << 23)) >> 24;
        a1 ^= ((43 - a1) >> 31) & a1;
    }
    return (sign8)a1;
}

static void lobits(const int *params,sign32 r0[],sign32 r[])
{
    int m;
    for (m=0;m<DL_DEGREE;m++)
            r0[m]=decompose_lo(params,r[m]);
}

static void hibits(const int *params,sign8 r1[],sign32 r[])
{
    int m;
    for (m=0;m<DL_DEGREE;m++)
        r1[m]=(sign32)decompose_hi(params,r[m]);
}    


// before h initialised to zeros, hptr=0
// after new hptr returned and h[OMEGA+i]= hptr
static int MakePartialHint(const int *params,byte h[],int hptr,sign32 z[],sign32 r[])
{
    int m;
    sign8 a0,a1;
    sign32 rz;
    int omega=params[7];
    for (m=0;m<DL_DEGREE;m++)
    {
        a0=decompose_hi(params,r[m]);
        rz=r[m]+z[m];
        rz-=DL_PRIME;
        rz=rz+((rz>>31)&DL_PRIME);
        a1=decompose_hi(params,rz);
        if (a0!=a1) {
            if (hptr>=omega) return omega+1;
            h[hptr++]=m&0xff;
        }
    }
    return hptr;
}

static int UsePartialHint(const int *params,sign8 r[],byte h[],int hptr,int i,sign32 w[])
{
    int dv=(sign8)params[2];
    int omega=params[7];
    sign8 a1,md=dv/2;
    sign32 a0;
    int m;
    for (m=0;m<DL_DEGREE;m++)
    {
        a1=decompose_hi(params,w[m]);
        if (m==h[hptr] && hptr<h[omega+i])
        {
            hptr++;
            a0=decompose_lo(params,w[m]);
            if (a0<=DL_PRIME/2) {
                a1++;
                if (a1>=md) a1-=md;
            } else {
                a1--;
                if (a1<0) a1+=md;
            }
        }
        r[m]=a1;
    }
    return hptr;
}

static sign32 infinity_norm(sign32 w[])
{
    int m;
    sign32 az,n=0;
    for (m=0;m<DL_DEGREE;m++)
    {
        az=w[m];
        if (az>DL_PRIME/2) az=DL_PRIME-az;
        if (az>n) n=az;    
    }
    return n;
}

// Dilithium API

static void keypair(const int *params,byte *tau,octet *sk,octet *pk)
{
    int i,row,j;
    sha3 sh;
    byte buff[128];
    byte rho[32];
    byte rhod[64];
    byte bK[32];
    byte tr[32];          // 320 bytes
    sign32 Aij[DL_DEGREE];     // 1024 bytes
    sign32 w[DL_DEGREE]; // work space  1024 bytes
    sign32 r[DL_DEGREE]; // work space  1024 bytes total = 12352

    int ck=params[3];
    int el=params[4];

#ifdef USE_VLAS
    sign8 s1[el*DL_DEGREE];     // 1280 bytes
    sign8 s2[ck*DL_DEGREE];     // 1536 bytes
    sign16 t0[ck*DL_DEGREE];    // 3072 bytes
    sign16 t1[ck*DL_DEGREE];    // 3072 bytes
#else    
    sign8 s1[DL_MAXL*DL_DEGREE];     // 1280 bytes
    sign8 s2[DL_MAXK*DL_DEGREE];     // 1536 bytes
    sign16 t0[DL_MAXK*DL_DEGREE];    // 3072 bytes
    sign16 t1[DL_MAXK*DL_DEGREE];    // 3072 bytes
#endif

    SHA3_init(&sh, SHAKE256);
  
    for (i=0;i<32;i++)
        SHA3_process(&sh,tau[i]); 
    SHA3_shake(&sh,(char *)buff,128);
    for (i=0;i<32;i++)
    {
        rho[i]=buff[i];
        bK[i]=buff[i+96];
    }
    for (i=0;i<64;i++)
        rhod[i]=buff[32+i];

    for (i=0;i<el;i++)
    {
        row=DL_DEGREE*i;
        sample_Sn(params,rhod,&s1[row],i);
    }

    for (i=0;i<ck;i++)
    {
        row=DL_DEGREE*i;
        sample_Sn(params,rhod,&s2[row],el+i);
        poly_zero(r);
        for (j=0;j<el;j++)
        {
            poly_scopy(w,&s1[j*DL_DEGREE]);
            ntt(w);  
            ExpandAij(rho,Aij,i,j);  // This is bottleneck
            poly_mul(w,w,Aij);
            poly_add(r,r,w);
            //poly_soft_reduce(r);  // be lazy
        }
        poly_hard_reduce(r);  
        intt(r);
        poly_scopy(w,&s2[row]);
        poly_pos(w);
        poly_add(r,r,w);
        poly_soft_reduce(r);
        Power2Round(r,&t0[row],&t1[row]);
    }

    CRH1(params,tr,rho,t1);

    pk->len=pack_pk(params,(byte *)pk->val,rho,t1);
    sk->len=pack_sk(params,(byte *)sk->val,rho,bK,tr,s1,s2,t0);
}

static int signature(const int *params,octet *sk,octet *M,octet *sig)
{
    int i,k,nh,fk,row,j;
    bool badone;
    byte rho[32];
    byte bK[32];
    byte ct[32];
    byte tr[32];
    byte mu[64];
    byte rhod[64];   // 288 bytes
    byte hint[100]; // 61 bytes

    sign32 c[DL_DEGREE];  // 1024 bytes
    sign32 w[DL_DEGREE];  // work space 1024 bytes
    sign32 r[DL_DEGREE];  // work space 1024 bytes total= 21673 bytes
    int ck=params[3];
    int el=params[4];
    //sign32 Aij[DL_DEGREE]; // 1024 bytes
#ifdef USE_VLAS
    sign8 s1[el*DL_DEGREE]; // 1280 bytes
    sign8 s2[ck*DL_DEGREE]; // 1536 bytes
    sign16 t0[ck*DL_DEGREE]; // 3072 bytes
    sign32 y[el*DL_DEGREE];  // 5120 bytes
    sign32 Ay[ck*DL_DEGREE]; // 6144 bytes
    sign8 w1[ck*DL_DEGREE]; // 1280 bytes
#else
    sign8 s1[DL_MAXL*DL_DEGREE]; // 1280 bytes
    sign8 s2[DL_MAXK*DL_DEGREE]; // 1536 bytes
    sign16 t0[DL_MAXK*DL_DEGREE]; // 3072 bytes
    sign32 y[DL_MAXL*DL_DEGREE];  // 5120 bytes
    sign32 Ay[DL_MAXK*DL_DEGREE]; // 6144 bytes
    sign8 w1[DL_MAXK*DL_DEGREE]; // 1280 bytes
#endif
    int tau=params[0];
    int lg=params[1];
    int gamma1=(sign32)(1<<lg);
    int dv=(sign32)params[2];
    int gamma2=(DL_PRIME-1)/dv;
    int eta=params[5];
    int beta=(sign32)(tau*eta);
    int omega=params[7];

    unpack_sk(params,rho,bK,tr,s1,s2,t0,(byte *)sk->val);

// signature
    CRH2(mu,tr,(byte*)M->val,M->len);
    CRH3(rhod,bK,mu);

    for (k=0; ;k++ )
    {
        fk=k*el;
        sample_Y(params,fk,rhod,y);

// NTT y
        for (i=0;i<el;i++)
        {
            row=DL_DEGREE*i;
            ntt(&y[row]);
        }

// Calculate Ay 
        for (i=0;i<ck;i++)
        {
            row=DL_DEGREE*i;
            poly_zero(r);
            for (j=0;j<el;j++)
            { // Note: no NTTs in here
                poly_copy(w,&y[j*DL_DEGREE]);
                ExpandAij(rho,c,i,j);  // This is bottleneck // re-use c for Aij 
                poly_mul(w,w,c);
                poly_add(r,r,w);
                //poly_soft_reduce(r);  // be lazy
            }
            poly_hard_reduce(r);
            intt(r);
            poly_copy(&Ay[row],r);
// Calculate w1
            hibits(params,&w1[row],&Ay[row]);
        }

// Calculate c
        H4(params,ct,mu,w1);
        SampleInBall(params,ct,c);

        badone=false;
// Calculate z=y+c.s1
        ntt(c);
        for (i=0;i<el;i++)
        {
            row=DL_DEGREE*i;
            poly_scopy(w,&s1[row]);
            ntt(w);
            poly_mul(w,w,c);
            nres_it(w);
            poly_add(&y[row],&y[row],w);  // re-use y for z 
            redc_it(&y[row]);  // unNTT y
            intt(&y[row]);
            poly_soft_reduce(&y[row]);
            if (infinity_norm(&y[row])>=gamma1-beta)
            {
                badone=true;
                break;
            }
        }

        if (badone) continue;
 
// Calculate Ay=w-c.s2 and r0=lobits(w-c.s2)
        nh=0;
        for (i=0;i<omega+ck;i++)
            hint[i]=0;
        for (i=0;i<ck;i++)
        {
            row=DL_DEGREE*i;
            poly_scopy(w,&s2[row]);
            ntt(w);
            poly_mul(w,w,c);
            
            intt(w);
            poly_sub(&Ay[row],&Ay[row],w);
            poly_soft_reduce(&Ay[row]);   //Ay=w-cs2
            lobits(params,w,&Ay[row]);    // r0
            if (infinity_norm(w)>=gamma2-beta)
            {
                badone=true;
                break;
            }
            poly_mcopy(w,&t0[row]);
            ntt(w);
            poly_mul(w,w,c);
            
            intt(w);
            poly_negate(r,w);  // -ct0
            if (infinity_norm(r)>=gamma2)
            {
                badone=true;
                break;
            }
            poly_sub(&Ay[row],&Ay[row],r);
            poly_soft_reduce(&Ay[row]);

            nh=MakePartialHint(params,hint,nh,r,&Ay[row]);
            if (nh>omega)
            {
                badone=true;
                break;
            }
            hint[omega+i]=nh;
        }
        if (badone) continue;
        break;
    }
    sig->len=pack_sig(params,(byte *)sig->val,y,ct,hint);
    return k+1;      
}

static bool verify(const int *params,octet *pk,octet *M,octet *sig)
{
    int i,row,j,m,hints;
    byte rho[32];
    byte mu[64];
    byte ct[32];
    byte cct[32];
    byte tr[32];         // 192 bytes
    byte hint[100];  // 61 bytes

    sign32 c[DL_DEGREE];    // 1024 bytes
    sign32 w[DL_DEGREE]; // work space // 1024 bytes
    sign32 r[DL_DEGREE]; // work space // 1024 bytes total=14077 bytes
    sign32 Aij[DL_DEGREE];  // 1024 bytes

    int ck=params[3];
    int el=params[4];

#ifdef USE_VLAS
    sign32 z[el*DL_DEGREE];  // 5120 bytes
    sign16 t1[ck*DL_DEGREE]; // 3072 bytes
    sign8 w1d[ck*DL_DEGREE]; // 1536 bytes 
#else
    sign32 z[DL_MAXL*DL_DEGREE];  // 5120 bytes
    sign16 t1[DL_MAXK*DL_DEGREE]; // 3072 bytes
    sign8 w1d[DL_MAXK*DL_DEGREE]; // 1536 bytes 
#endif

    int tau=params[0];
    int lg=params[1];
    int gamma1=(sign32)(1<<lg);
    int eta=params[5];
    int beta=(sign32)(tau*eta);
    int omega=params[7];

// unpack public key and signature
    unpack_pk(params,rho,t1,(byte *)pk->val);
    unpack_sig(params,z,ct,hint,(byte *)sig->val);

    for (i=0;i<el;i++)
    {
        row=DL_DEGREE*i;
        if (infinity_norm(&z[row])>=gamma1-beta)
            return false;
        ntt(&z[row]); // convert to ntt form
    }

    CRH1(params,tr,rho,t1);
    CRH2(mu,tr,(byte *)M->val,M->len);
    SampleInBall(params,ct,c);
    ntt(c);

// Calculate Az
    hints=0;
    for (i=0;i<ck;i++)
    {
        row=DL_DEGREE*i;
        poly_zero(r);
        for (j=0;j<el;j++)
        { // Note: no NTTs in here
            poly_copy(w,&z[j*DL_DEGREE]);
            ExpandAij(rho,Aij,i,j);    // This is bottleneck
            poly_mul(w,w,Aij);
            poly_add(r,r,w);
            //poly_soft_reduce(r);  // be lazy
        }
        poly_hard_reduce(r);

// Calculate Az-ct1.2^d
        for (m=0;m<DL_DEGREE;m++)
            w[m]=(sign32)(t1[row+m])<<DL_D;
        ntt(w);
        poly_mul(w,w,c);
        poly_sub(r,r,w);
        intt(r);

        hints=UsePartialHint(params,&w1d[row],hint,hints,i,r);
        if (hints>omega) return false;
    }

    H4(params,cct,mu,w1d);

    for (i=0;i<32;i++)
        if (ct[i]!=cct[i])
            return false;
    return true;
}

void DLTHM_keypair_2(byte *tau,octet *sk,octet *pk)
{
    keypair(PARAMS_2,tau,sk,pk);
}

int DLTHM_signature_2(octet *sk,octet *M,octet *sig)
{
    return signature(PARAMS_2,sk,M,sig);
}

bool DLTHM_verify_2(octet *pk,octet *M,octet *sig)
{
    return verify(PARAMS_2,pk,M,sig);
}


void DLTHM_keypair_3(byte *tau,octet *sk,octet *pk)
{
    keypair(PARAMS_3,tau,sk,pk);
}

int DLTHM_signature_3(octet *sk,octet *M,octet *sig)
{
    return signature(PARAMS_3,sk,M,sig);
}

bool DLTHM_verify_3(octet *pk,octet *M,octet *sig)
{
    return verify(PARAMS_3,pk,M,sig);
}

void DLTHM_keypair_5(byte *tau,octet *sk,octet *pk)
{
    keypair(PARAMS_5,tau,sk,pk);
}

int DLTHM_signature_5(octet *sk,octet *M,octet *sig)
{
    return signature(PARAMS_5,sk,M,sig);
}

bool DLTHM_verify_5(octet *pk,octet *M,octet *sig)
{
    return verify(PARAMS_5,pk,M,sig);
}
