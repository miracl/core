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

/* Kyber API implementation. Constant time where it matters. Spends nearly all of its time running SHA3. Small.

   M.Scott 22/11/2021
*/

#include "kyber.h"

// parameters for each security level
const int PARAMS_512[6]={2,3,2,10,4,32};
const int PARAMS_768[6]={3,2,2,10,4,32};
const int PARAMS_1024[6]={4,2,2,11,5,32};

/* Start of public domain reference implementation code - taken from https://github.com/pq-crystals/kyber */

const sign16 zetas[128] = {
  -1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
   -171,   622,  1577,   182,   962, -1202, -1474,  1468,
    573, -1325,   264,   383,  -829,  1458, -1602,  -130,
   -681,  1017,   732,   608, -1542,   411,  -205, -1571,
   1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
    516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
   -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
   -398,   961, -1508,  -725,   448, -1065,   677, -1275,
  -1103,   430,   555,   843, -1251,   871,  1550,   105,
    422,   587,   177,  -235,  -291,  -460,  1574,  1653,
   -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
  -1590,   644,  -872,   349,   418,   329,  -156,   -75,
    817,  1097,   603,   610,  1322, -1285, -1465,   384,
  -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
  -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
   -108,  -308,   996,   991,   958, -1460,  1522,  1628
};

static int16_t montgomery_reduce(int32_t a)
{
  int16_t t;

  t = (int16_t)a*KY_QINV;
  t = (a - (int32_t)t*KY_PRIME) >> 16;
  return t;
}

static int16_t barrett_reduce(int16_t a) {
  int16_t t;
  const int16_t v = ((1<<26) + KY_PRIME/2)/KY_PRIME;

  t  = ((int32_t)v*a + (1<<25)) >> 26;
  t *= KY_PRIME;
  return a - t;
}

static sign16 fqmul(sign16 a, sign16 b) {
  return montgomery_reduce((sign32)a*b);
}

static void ntt(int16_t r[256]) {
  unsigned int len, start, j, k;
  int16_t t, zeta;

  k = 1;
  for(len = 128; len >= 2; len >>= 1) {
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas[k++];
      for(j = start; j < start + len; j++) {
        t = fqmul(zeta, r[j + len]);
        r[j + len] = r[j] - t;
        r[j] = r[j] + t;
      }
    }
  }
}

static void invntt(int16_t r[256]) {
  unsigned int start, len, j, k;
  int16_t t, zeta;
  const int16_t f = 1441; // mont^2/128

  k = 127;
  for(len = 2; len <= 128; len <<= 1) {
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas[k--];
      for(j = start; j < start + len; j++) {
        t = r[j];
        r[j] = barrett_reduce(t + r[j + len]);
        r[j + len] = r[j + len] - t;
        r[j + len] = fqmul(zeta, r[j + len]);
      }
    }
  }

  for(j = 0; j < 256; j++)
    r[j] = fqmul(r[j], f);
}

static void basemul(sign16 r[2], const sign16 a[2], const sign16 b[2], sign16 zeta) {
    r[0]  = fqmul(a[1], b[1]);
    r[0]  = fqmul(r[0], zeta);
    r[0] += fqmul(a[0], b[0]);
    r[1]  = fqmul(a[0], b[1]);
    r[1] += fqmul(a[1], b[0]);
}

static void poly_reduce(sign16 *r)
{
    int i;
    for(i=0;i<KY_DEGREE;i++)
        r[i] = barrett_reduce(r[i]);
}

static void poly_ntt(sign16 *r)
{
    ntt(r);
    poly_reduce(r);
}

static void poly_invntt(sign16 *r)
{
    invntt(r);
}

// Note r must be distinct from a and b
static void poly_mul(sign16 *r, const sign16 *a, const sign16 *b)
{
    int i;
    for(i = 0; i < KY_DEGREE/4; i++) {
        basemul(&r[4*i], &a[4*i], &b[4*i], zetas[64 + i]);
        basemul(&r[4*i + 2], &a[4*i + 2], &b[4*i + 2], -zetas[64 + i]);
    }
}

static void poly_tomont(sign16 *r)
{
    int i;
    const sign16 f = KY_ONE;
    for(i=0;i<KY_DEGREE;i++)
        r[i] = montgomery_reduce((sign32)r[i]*f);
}

/* End of public domain reference code use */

// copy polynomial
static void poly_copy(sign16 *p1, sign16 *p2)
{
    int i;
    for (i = 0; i < KY_DEGREE; i++)
        p1[i] = p2[i];
}

// zero polynomial
static void poly_zero(sign16 *p1)
{
    int i;
    for (i = 0; i < KY_DEGREE; i++)
        p1[i] = 0;
}

// add polynomials
static void poly_add(sign16 *p1, sign16 *p2, sign16 *p3)
{
    int i;
    for (i = 0; i < KY_DEGREE; i++)
        p1[i] = (p2[i] + p3[i]);
}

// subtract polynomials
static void poly_sub(sign16 *p1, sign16 *p2, sign16 *p3)
{
    int i;
    for (i = 0; i < KY_DEGREE; i++)
        p1[i] = (p2[i] - p3[i]);
}

// Generate A[i][j] from rho
static void ExpandAij(byte rho[32],sign16 Aij[],int i,int j)
{
    int m;
    sha3 sh;
    SHA3_init(&sh, SHAKE128);
    byte buff[640];  // should be plenty (?)
    for (m=0;m<32;m++)
        SHA3_process(&sh,rho[m]);
    SHA3_process(&sh,j&0xff);
    SHA3_process(&sh,i&0xff);
    SHA3_shake(&sh,(char *)buff,640);
	i=j=0;
	while (j<KY_DEGREE)
	{
		int d1=buff[i]+256*(buff[i+1]&0x0F);
		int d2=buff[i+1]/16+16*buff[i+2];
		if (d1<KY_PRIME)
			Aij[j++]=d1;
		if (d2<KY_PRIME && j<KY_DEGREE)
			Aij[j++]=d2;
		i+=3;
	}
}

// get n-th bit from byte array
static int getbit(byte b[],int n)
{
	int wd=n/8;
	int bt=n%8;
	return (b[wd]>>bt)&1;
}

// centered binomial distribution
static void CBD(byte bts[],int eta,sign16 f[KY_DEGREE])
{
	int a,b;
	for (int i=0;i<KY_DEGREE;i++)
	{
		a=b=0;
		for (int j=0;j<eta;j++)
		{
			a+=getbit(bts,2*i*eta+j);
			b+=getbit(bts,2*i*eta+eta+j);
		}
		f[i]=a-b; 
	}
}

// extract ab bits into word from dense byte stream
static sign16 nextword(int ab,byte t[],int *ptr, int *bts)
{
    int mptr=*ptr;
    int mbts=*bts;
    sign16 r=t[mptr]>>mbts;
    sign16 mask=(1<<ab)-1;
    sign16 w;
    int i=0;
    int gotbits=8-mbts; // bits left in current byte
    while (gotbits<ab)
    {
        i++;
        w=(sign16)t[mptr+i];
        r|=w<<gotbits;
        gotbits+=8;
    }
    mbts+=ab;
    while (mbts>=8)
    {
        mbts-=8;
        mptr++;
    }
    w=r&mask;
    *ptr=mptr;
    *bts=mbts;
    return w;  
}

// array t has ab active bits per word
// extract bytes from array of words
// if max!=0 then -max<=t[i]<=+max
static byte nextbyte16(int ab,sign16 t[],int *ptr, int *bts)
{
    int mptr=*ptr;
    int mbts=*bts;
    sign16 r,w;
    int left=ab-mbts; // number of bits left in this word
    int i=0;

    w=t[mptr]; w+=(w>>15)&KY_PRIME;
    r=w>>mbts;
    while (left<8)
    {
        i++;
        w=t[mptr+i]; w+=(w>>15)&KY_PRIME;
        r|=w<<left;
        left+=ab;
    }

    mbts+=8;
    while (mbts>=ab)
    {
        mbts-=ab;
        mptr++;
    }
    *ptr=mptr;
    *bts=mbts;
    return (byte)r&0xff;        
}

// encode polynomial vector of length len with coefficients of length 2^L, into packed bytes
static int encode(sign16 t[],int len,int L,byte pack[])
{
    int ptr,bts,n;
    ptr=bts=0;
    for (n=0;n<len*(KY_DEGREE*L)/8;n++ ) {
        pack[n]=nextbyte16(L,t,&ptr,&bts);
    }
    return n;
}

static byte chk_encode(sign16 t[],int len,int L,byte pack[])
{
    int ptr,bts,n;
    byte m,diff=0;
    ptr=bts=0;
    for (n=0;n<len*(KY_DEGREE*L)/8;n++ ) {
        m=nextbyte16(L,t,&ptr,&bts);
        diff|=(m^pack[n]);
    }    
    return diff;
}

// decode packed bytes into polynomial vector of length len, with coefficients of length 2^L
static void decode(byte pack[],int L,sign16 t[],int len)
{
    int ptr,bts,i;
    ptr=bts=0;
    for (i=0;i<len*KY_DEGREE;i++ )
        t[i]=nextword(L,pack,&ptr,&bts);
}

// Bernsteins safe division by 0xD01
static int32_t safediv(int32_t x)
{
    int32_t qpart,q=0;    

    qpart=(int32_t)(((int64_t)x*645083)>>31);
    x-=qpart*0xD01; q += qpart;

    qpart=(int32_t)(((int64_t)x*645083)>>31)+1;
    x-=qpart*0xD01; q += qpart+(x>>31);

    return q;
}

// compress polynomial coefficents in place, for polynomial vector of length len
static void compress(sign16 t[],int len,int d)
{
	sign32 twod=(1<<d);
	for (int i=0;i<len*KY_DEGREE;i++)
    {
        t[i]+=(t[i]>>15)&KY_PRIME;
        t[i]= (sign16)(safediv(twod*t[i]+KY_PRIME/2)&(twod-1));
    }
}

// decompress polynomial coefficents in place, for polynomial vector of length len
static void decompress(sign16 t[],int len,int d)
{
	int twod1=(1<<(d-1));
	for (int i=0;i<len*KY_DEGREE;i++)
		t[i]=(KY_PRIME*t[i]+twod1)>>d;
}

// ********************* Kyber API ******************************

// input entropy, output key pair
void KYBER_CPA_keypair(const int *params,byte *tau,octet *sk,octet *pk)
{
    int i,j,k,row;
    sha3 sh;
    byte rho[32];
	byte sigma[33];
	byte buff[256];

    int ck=params[0];
    sign16 r[KY_DEGREE];
    sign16 w[KY_DEGREE];
    sign16 Aij[KY_DEGREE]; 
#ifdef USE_VLAS
    sign16 s[ck*KY_DEGREE];
    sign16 e[ck*KY_DEGREE];
    sign16 p[ck*KY_DEGREE];
#else
    sign16 s[KY_MAXK*KY_DEGREE];
    sign16 e[KY_MAXK*KY_DEGREE];
    sign16 p[KY_MAXK*KY_DEGREE];
#endif

    int eta1=params[1];
    int public_key_size=32+ck*(KY_DEGREE*3)/2;
    int secret_cpa_key_size=ck*(KY_DEGREE*3)/2;

    SHA3_init(&sh,SHA3_HASH512);
   
    for (i=0;i<32;i++)
        SHA3_process(&sh,tau[i]); 
	SHA3_hash(&sh,(char *)buff);
	for (i=0;i<32;i++)
	{
		rho[i]=buff[i];
		sigma[i]=buff[i+32];
	}

	sigma[32]=0;   // N
// create s
	for (i=0;i<ck;i++)
	{
		SHA3_init(&sh,SHAKE256);
		for (j=0;j<33;j++)
			SHA3_process(&sh,sigma[j]); 
		SHA3_shake(&sh,(char *)buff,64*eta1);
		CBD(buff,eta1,&s[i*KY_DEGREE]);
		sigma[32]+=1;
	}

// create e
	for (i=0;i<ck;i++)
	{
		SHA3_init(&sh,SHAKE256);
		for (j=0;j<33;j++)
			SHA3_process(&sh,sigma[j]); 
		SHA3_shake(&sh,(char *)buff,64*eta1);
		CBD(buff,eta1,&e[i*KY_DEGREE]);
		sigma[32]+=1;
	}

    for (k=0;k<ck;k++)
    {
        row=KY_DEGREE*k;
        poly_ntt(&s[row]);
        poly_ntt(&e[row]);
    }

    for (i=0;i<ck;i++)
    {
        row=KY_DEGREE*i;
        ExpandAij(rho,Aij,i,0);
        poly_mul(r,Aij,s);
        for (j=1;j<ck;j++)
        {
            ExpandAij(rho,Aij,i,j);
            poly_mul(w,&s[j*KY_DEGREE],Aij);
            poly_add(r,r,w);
        }
        poly_reduce(r);
        poly_tomont(r);
        poly_add(&p[row],r,&e[row]);
        poly_reduce(&p[row]);
    }

    encode(s,ck,12,(byte *)sk->val);
    sk->len=secret_cpa_key_size;
    encode(p,ck,12,(byte *)pk->val);
    pk->len=public_key_size;
    for (i=0;i<32;i++)
        pk->val[public_key_size-32+i]=rho[i];
}

// provide 64 random bytes, output secret and public keys
void KYBER_CCA_keypair(const int *params,byte *randbytes64,octet *sk,octet *pk)
{
    int i;
    sha3 sh;
    byte h[32];

    KYBER_CPA_keypair(params,randbytes64,sk,pk);
    OCT_joctet(sk,pk);

    SHA3_init(&sh,SHA3_HASH256);
    for (i=0;i<pk->len;i++)
        SHA3_process(&sh,(byte)pk->val[i]);
    SHA3_hash(&sh,(char *)h);
    OCT_jbytes(sk,(char *)h,32);
    OCT_jbytes(sk,(char *)&randbytes64[32],32);
}

// Given input of entropy, public key and shared secret is an input, outputs ciphertext
void KYBER_CPA_base_encrypt(const int *params,byte *coins,octet *pk,octet *ss,sign16 *u, sign16* v)
{
    int i,row,j,len;
    sha3 sh;
    byte sigma[33];
	byte buff[256];
    byte rho[32];
    int ck=params[0];

    sign16 r[KY_DEGREE];
    sign16 w[KY_DEGREE];
    sign16 Aij[KY_DEGREE]; 
#ifdef USE_VLAS
    sign16 q[ck*KY_DEGREE];
    sign16 p[ck*KY_DEGREE];
#else
    sign16 q[KY_MAXK*KY_DEGREE];
    sign16 p[KY_MAXK*KY_DEGREE];
#endif

    int eta1=params[1];
    int eta2=params[2];
    int du=params[3];
    int dv=params[4];
    int public_key_size=32+ck*(KY_DEGREE*3)/2;

    for (i=0;i<32;i++)
        sigma[i]=coins[i];//i+6; //RAND_byte(RNG);
	sigma[32]=0;

    for (i=0;i<32;i++)
        rho[i]=pk->val[pk->len-32+i];

// create q
	for (i=0;i<ck;i++)
	{
		SHA3_init(&sh,SHAKE256);
		for (j=0;j<33;j++)
			SHA3_process(&sh,sigma[j]); 
		SHA3_shake(&sh,(char *)buff,64*eta1);
		CBD(buff,eta1,&q[i*KY_DEGREE]);
		sigma[32]+=1;
	}

// create e1
	for (i=0;i<ck;i++)
	{
		SHA3_init(&sh,SHAKE256);
		for (j=0;j<33;j++)
			SHA3_process(&sh,sigma[j]); 
		SHA3_shake(&sh,(char *)buff,64*eta2);
		CBD(buff,eta1,&u[i*KY_DEGREE]);          // e1
		sigma[32]+=1;
	}

    for (i=0;i<ck;i++)
    {
        row=KY_DEGREE*i;
        poly_ntt(&q[row]);
    }

    for (i=0;i<ck;i++)
    {
        row=KY_DEGREE*i;
        ExpandAij(rho,Aij,0,i);
        poly_mul(r,Aij,q);
        for (j=1;j<ck;j++)
        {
            ExpandAij(rho,Aij,j,i);
            poly_mul(w,&q[j*KY_DEGREE],Aij);
            poly_add(r,r,w);
        }
        poly_reduce(r);
        poly_invntt(r);
        poly_add(&u[row],&u[row],r);
        poly_reduce(&u[row]);
    }

    decode((byte *)pk->val,12,p,ck);

    poly_mul(v,p,q);
    for (i=1;i<ck;i++)
    {
        row=KY_DEGREE*i;
        poly_mul(r,&p[row],&q[row]);
        poly_add(v,v,r);
    }
    poly_invntt(v);

// create e2
    SHA3_init(&sh,SHAKE256);
	for (j=0;j<33;j++)
		SHA3_process(&sh,sigma[j]); 
	SHA3_shake(&sh,(char *)buff,64*eta2);
	CBD(buff,eta1,w);  // e2

    poly_add(v,v,w);
    
    decode((byte *)ss->val,1,r,1);
    decompress(r,1,1);
   
    poly_add(v,v,r);
    poly_reduce(v);

    compress(u,ck,du);
    compress(v,1,dv);
}

// Given input of entropy, public key and shared secret is an input, outputs ciphertext
static void KYBER_CPA_encrypt(const int *params,byte *coins,octet *pk,octet *ss,octet *ct)
{
    int ck=params[0];
    sign16 v[KY_DEGREE]; 
#ifdef USE_VLAS
    sign16 u[ck*KY_DEGREE];
#else
    sign16 u[KY_MAXK*KY_DEGREE];
#endif

    int du=params[3];
    int dv=params[4];
    int ciphertext_size=(du*ck+dv)*KY_DEGREE/8;
    KYBER_CPA_base_encrypt(params,coins,pk,ss,u,v);
    encode(u,ck,du,(byte *)ct->val);
    encode(v,1,dv,(byte *)&ct->val[ciphertext_size-(dv*KY_DEGREE/8)]);
    ct->len=ciphertext_size;
}

// Re-encrypt and check that ct is OK (if so return is zero)
static byte KYBER_CPA_check_encrypt(const int *params,byte *coins,octet *pk,octet *ss,octet *ct)
{
    int ck=params[0];
    sign16 v[KY_DEGREE]; 
#ifdef USE_VLAS
    sign16 u[ck*KY_DEGREE];
#else
    sign16 u[KY_MAXK*KY_DEGREE];
#endif

    int du=params[3];
    int dv=params[4];
    int ciphertext_size=(du*ck+dv)*KY_DEGREE/8;
    byte d1,d2;
    KYBER_CPA_base_encrypt(params,coins,pk,ss,u,v);
    d1=chk_encode(u,ck,du,(byte *)ct->val);
    d2=chk_encode(v,1,dv,(byte *)&ct->val[ciphertext_size-(dv*KY_DEGREE/8)]);
    if ((d1|d2)==0)
        return 0;
    else
        return 0xff;
}

// Given entropy and public key, outputs 32-byte shared secret and ciphertext
void KYBER_CCA_encrypt(const int *params,byte *randbytes32,octet *pk,octet *ss,octet *ct)
{
    int i;
    sha3 sh;
    byte h[32],hm[32],g[64],coins[32];
    octet HM={32,sizeof(hm),(char *)hm};
    int ck=params[0];
    int du=params[3];
    int dv=params[4];
    int shared_secret_size=params[5];


    SHA3_init(&sh,SHA3_HASH256);               // H(m)
    for (i=0;i<32;i++)
        SHA3_process(&sh,randbytes32[i]);
    SHA3_hash(&sh,(char *)hm);

    SHA3_init(&sh,SHA3_HASH256);               // H(pk)
    for (i=0;i<pk->len;i++)
        SHA3_process(&sh,(byte)pk->val[i]);
    SHA3_hash(&sh,(char *)h);

    SHA3_init(&sh,SHA3_HASH512);               // Kb,r = G(H(m)|H(pk)
    for (i=0;i<32;i++)
        SHA3_process(&sh,hm[i]);
    for (i=0;i<32;i++)
        SHA3_process(&sh,h[i]);
    SHA3_hash(&sh,(char *)g);

    for (i=0;i<32;i++)
        coins[i]=g[i+32];
    KYBER_CPA_encrypt(params,coins,pk,&HM,ct);
    
    SHA3_init(&sh,SHA3_HASH256);              // H(ct)
    for (i=0;i<ct->len;i++)
        SHA3_process(&sh,(byte)ct->val[i]);
    SHA3_hash(&sh,(char *)h);

    SHA3_init(&sh,SHAKE256);                  // K=KDF(Kb|H(ct))
    for (i=0;i<32;i++)
        SHA3_process(&sh,g[i]);
    for (i=0;i<32;i++)
        SHA3_process(&sh,h[i]);

    SHA3_shake(&sh,ss->val,shared_secret_size); // could be any length?
    ss->len=shared_secret_size;
}

// Input secret key and ciphertext, outputs shared 32-byte secret
void KYBER_CPA_decrypt(const int *params,octet *sk,octet *ct,octet *ss)
{
	int i,j,row;
    int ck=params[0];
	sign16 w[KY_DEGREE];
    sign16 v[KY_DEGREE];
    sign16 r[KY_DEGREE];
#ifdef USE_VLAS
    sign16 u[ck*KY_DEGREE];
    sign16 s[ck*KY_DEGREE];
#else
    sign16 u[KY_MAXK*KY_DEGREE];
    sign16 s[KY_MAXK*KY_DEGREE];
#endif
    int du=params[3];
    int dv=params[4];
    int shared_secret_size=params[5];

    decode((byte *)ct->val,du,u,ck);
    decode((byte *)&ct->val[du*ck*KY_DEGREE/8],dv,v,1);
    decompress(u,ck,du);
    decompress(v,1,dv);
    decode((byte *)sk->val,12,s,ck);

    poly_ntt(u);
    poly_mul(w,u,s);
    for (i=1;i<ck;i++)
    {
        row=KY_DEGREE*i;
        poly_ntt(&u[row]);
        poly_mul(r,&u[row],&s[row]);
        poly_add(w,w,r);
    }
    poly_reduce(w);
    poly_invntt(w);
    poly_sub(v,v,w);
    compress(v,1,1);
    encode(v,1,1,(byte *)ss->val);
    ss->len=shared_secret_size;    
}

void KYBER_CCA_decrypt(const int *params,octet *sk,octet *ct,octet *ss)
{ 
    int i,olen,same;
    sha3 sh;
    byte h[32],z[32],m[32],coins[32],g[64],mask;
    octet M={32,sizeof(m),(char *)m};
    int ck=params[0];
    int du=params[3];
    int dv=params[4];
    int secret_cpa_key_size=ck*(KY_DEGREE*3)/2;
    int public_key_size=32+ck*(KY_DEGREE*3)/2;
    int shared_secret_size=params[5];
    octet PK={public_key_size,public_key_size,&sk->val[secret_cpa_key_size]}; // public key is here

    olen=sk->len;
    sk->len=secret_cpa_key_size;  // chop off CPA secret
    for (i=0;i<32;i++)
        h[i]=sk->val[secret_cpa_key_size+public_key_size+i];
    for (i=0;i<32;i++)
        z[i]=sk->val[secret_cpa_key_size+public_key_size+32+i];

    KYBER_CPA_decrypt(params,sk,ct,&M);

    SHA3_init(&sh,SHA3_HASH512);               // Kb,r = G(H(m)|H(pk)
    for (i=0;i<32;i++)
        SHA3_process(&sh,m[i]);
    for (i=0;i<32;i++)
        SHA3_process(&sh,h[i]);
    SHA3_hash(&sh,(char *)g);

    for (i=0;i<32;i++)
        coins[i]=g[i+32];
    mask=KYBER_CPA_check_encrypt(params,coins,&PK,&M,ct);       // encrypt again with public key - FO transform CPA->CCA 

    for (i=0;i<32;i++)
        g[i]^=(g[i]^z[i])&mask;               // substitute z for Kb on failure

    SHA3_init(&sh,SHA3_HASH256);              // H(ct)
    for (i=0;i<ct->len;i++)
        SHA3_process(&sh,(byte)ct->val[i]);
    SHA3_hash(&sh,(char *)h);

    SHA3_init(&sh,SHAKE256);                  // K=KDF(Kb|H(ct))
    for (i=0;i<32;i++)
        SHA3_process(&sh,g[i]);
    for (i=0;i<32;i++)
        SHA3_process(&sh,h[i]);
    
    SHA3_shake(&sh,ss->val,shared_secret_size); // could be any length?
    ss->len=shared_secret_size;
    sk->len=olen; // restore length
}

// ********************* Kyber API ******************************

void KYBER512_keypair(byte *r64,octet *SK,octet *PK)
{
    KYBER_CCA_keypair(PARAMS_512,r64,SK,PK);
}

void KYBER768_keypair(byte *r64,octet *SK,octet *PK)
{
    KYBER_CCA_keypair(PARAMS_768,r64,SK,PK);
}

void KYBER1024_keypair(byte *r64,octet *SK,octet *PK)
{
    KYBER_CCA_keypair(PARAMS_1024,r64,SK,PK);
}

void KYBER512_encrypt(byte *r32,octet *PK,octet *SS,octet *CT)
{
    KYBER_CCA_encrypt(PARAMS_512,r32,PK,SS,CT);
}

void KYBER768_encrypt(byte *r32,octet *PK,octet *SS,octet *CT)
{
    KYBER_CCA_encrypt(PARAMS_768,r32,PK,SS,CT);
}

void KYBER1024_encrypt(byte *r32,octet *PK,octet *SS,octet *CT)
{
    KYBER_CCA_encrypt(PARAMS_1024,r32,PK,SS,CT);
}

void KYBER512_decrypt(octet *SK,octet *CT,octet *SS)
{
    KYBER_CCA_decrypt(PARAMS_512,SK,CT,SS);
}

void KYBER768_decrypt(octet *SK,octet *CT,octet *SS)
{
    KYBER_CCA_decrypt(PARAMS_768,SK,CT,SS);
}

void KYBER1024_decrypt(octet *SK,octet *CT,octet *SS)
{
    KYBER_CCA_decrypt(PARAMS_1024,SK,CT,SS);
}

