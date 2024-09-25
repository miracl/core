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

/* Kyber API */

// Now conforms to new ML_KEM standard
// See https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.203.pdf
//

package org.miracl.core;

public final class KYBER {

	public static final int KY_LGN = 8;
	public static final int KY_DEGREE = (1 << KY_LGN);
	public static final int KY_PRIME = 0xD01;

	public static final int KY_ONE = 0x549;		// R mod Q
	public static final int KY_QINV = 62209;   // q^(-1) mod 2^16

	public static final int SECRET_CPA_SIZE_512 = (2*(KY_DEGREE*3)/2);
	public static final int PUBLIC_SIZE_512 = (32+2*(KY_DEGREE*3)/2);
	public static final int CIPHERTEXT_SIZE_512 = ((10*2+4)*KY_DEGREE/8);
	public static final int SECRET_CCA_SIZE_512 = (SECRET_CPA_SIZE_512+PUBLIC_SIZE_512+64);
	public static final int SHARED_SECRET_512 = 32;

	public static final int SECRET_CPA_SIZE_768 = (3*(KY_DEGREE*3)/2);
	public static final int PUBLIC_SIZE_768 = (32+3*(KY_DEGREE*3)/2);
	public static final int CIPHERTEXT_SIZE_768 = ((10*3+4)*KY_DEGREE/8);
	public static final int SECRET_CCA_SIZE_768 = (SECRET_CPA_SIZE_768+PUBLIC_SIZE_768+64);
	public static final int SHARED_SECRET_768 = 32;

	public static final int SECRET_CPA_SIZE_1024 = (4*(KY_DEGREE*3)/2);
	public static final int PUBLIC_SIZE_1024 = (32+4*(KY_DEGREE*3)/2);
	public static final int CIPHERTEXT_SIZE_1024 = ((11*4+5)*KY_DEGREE/8);
	public static final int SECRET_CCA_SIZE_1024 = (SECRET_CPA_SIZE_1024+PUBLIC_SIZE_1024+64);
	public static final int SHARED_SECRET_1024 = 32;

	public static final int KY_MAXK = 4;

// parameters for each security level
// K,eta1,eta2,du,dv,shared secret
	public static final int[] PARAMS_512 = {2,3,2,10,4,32};
	public static final int[] PARAMS_768 = {3,2,2,10,4,32};
	public static final int[] PARAMS_1024 = {4,2,2,11,5,32};

/* Translated from public domain reference implementation code - taken from https://github.com/pq-crystals/kyber */

	static final short[] zetas = {
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

	static short montgomery_reduce(int a)
	{
		short t;
		t = (short)(a*KY_QINV);
		t = (short)((a - (int)t*KY_PRIME) >> 16);
		return t;
	}

	static short barrett_reduce(short a) {
		short t;
		short v = ((1<<26) + KY_PRIME/2)/KY_PRIME;
		int vv=(int)v;
		int aa=(int)a;

		t  = (short)((vv*aa + 0x2000000) >> 26);
		t *= (short)KY_PRIME;
		return (short)(a - t);
	}

	static short fqmul(short a, short b) {
		return montgomery_reduce((int)a*b);
	}

	static void ntt(short[] r) {
		int len, start, j, k;
		short t, zeta;

		k = 1;
		for(len = 128; len >= 2; len >>= 1) {
			for(start = 0; start < 256; start = j + len) {
				zeta = zetas[k++];
				for(j = start; j < start + len; j++) {
					t = fqmul(zeta, r[j + len]);
					r[j + len] = (short)(r[j] - t);
					r[j] = (short)(r[j] + t);
				}
			}
		}
	}

	static void invntt(short[] r) {
		int start, len, j, k;
		short t, zeta;
		short f = 1441; // mont^2/128

		k = 127;
		for(len = 2; len <= 128; len <<= 1) {
			for(start = 0; start < 256; start = j + len) {
				zeta = zetas[k--];
				for(j = start; j < start + len; j++) {
					t = r[j];
					r[j] = barrett_reduce((short)(t + r[j + len]));
					r[j + len] = (short)(r[j + len] - t);
					r[j + len] = fqmul(zeta, r[j + len]);
				}
			}
		}

		for(j = 0; j < 256; j++)
			r[j] = fqmul(r[j], f);
	}

	static void basemul(int index,short r[], short a[], short b[], short zeta) {
		int i=index;
		int j=index+1;
		r[i]  = fqmul(a[j], b[j]);
		r[i]  = fqmul(r[i], zeta);
		r[i] += fqmul(a[i], b[i]);
		r[j]  = fqmul(a[i], b[j]);
		r[j] += fqmul(a[j], b[i]);
	}

	static void poly_reduce(short[] r)
	{
		int i;
		for(i=0;i<KY_DEGREE;i++)
			r[i] = barrett_reduce(r[i]);
	}

	static void poly_ntt(short[] r)
	{
		ntt(r);
		poly_reduce(r);
	}

	static void poly_invntt(short[] r)
	{
		invntt(r);
	}

// Note r must be distinct from a and b
	static void poly_mul(short[] r, short[] a, short[] b)
	{
		for(int i = 0; i < KY_DEGREE/4; i++) {
			basemul(4*i,r,a,b,zetas[64+i]);
			basemul(4*i+2,r,a,b,(short)-zetas[64+i]);
		}
	}

	static void poly_tomont(short[] r)
	{
		int i;
		short f = KY_ONE;
		for(i=0;i<KY_DEGREE;i++)
			r[i] = montgomery_reduce((int)r[i]*f);
	}

/* End of public domain reference code use */

// copy polynomial
	static void poly_copy(short[] p1, short[] p2)
	{
		int i;
		for (i = 0; i < KY_DEGREE; i++)
			p1[i] = p2[i];
	}	

// zero polynomial
	static void poly_zero(short[] p1)
	{
		int i;
		for (i = 0; i < KY_DEGREE; i++)
			p1[i] = 0;
	}

// add polynomials
	static void poly_add(short[] p1, short[] p2, short[] p3)
	{
		int i;
		for (i = 0; i < KY_DEGREE; i++)
			p1[i] = (short)(p2[i] + p3[i]);
	}

// subtract polynomials
	static void poly_sub(short[] p1, short[] p2, short[] p3)
	{
		int i;
		for (i = 0; i < KY_DEGREE; i++)
			p1[i] = (short)(p2[i] - p3[i]);
	}

// Generate A[i][j] from rho
	static void ExpandAij(byte[] rho,short[] Aij,int i,int j)
	{
		int m;
		SHA3 sh = new SHA3(SHA3.SHAKE128);
		byte[] buff = new byte[640]; // should be plenty (?)
		for (m=0;m<32;m++)
			sh.process(rho[m]);

		sh.process(j&0xff);
		sh.process(i&0xff);
		sh.shake(buff,640);
		i=j=0;
		while (j<KY_DEGREE)
		{
			int t1=(int)(buff[i])&0xff;  // convert byte to unsigned
			int t2=(int)(buff[i+1])&0xff;
			int t3=(int)(buff[i+2])&0xff;
			int d1=(t1+256*(t2&0x0f));
			int d2=(t2/16+16*t3);
			if (d1<KY_PRIME)
				Aij[j++]=(short)d1;
			if (d2<KY_PRIME && j<KY_DEGREE)
				Aij[j++]=(short)d2;
			i+=3;
		}
	}

// get n-th bit from byte array
	static int getbit(byte[] b,int n)
	{
		int wd=n/8;
		int bt=n%8;
		return (b[wd]>>bt)&1;
	}

// centered binomial distribution
	static void CBD(byte[] bts,int eta,short[] f)
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
			f[i]=(short)(a-b); 
		}
	}

    static short caste(byte x) {
        return (short)((int)x&0xff);
    }

// extract ab bits into word from dense byte stream
	static short nextword(int ab,byte[] t,int[] position)
	{
		int ptr=position[0]; // index in array
		int bts=position[1]; // bit index in byte
		short r=(short)(     ((int)t[ptr]&0xff)    >>bts);
		short mask=(short)((1<<ab)-1);
		short w;
		int i=0;

		int gotbits=8-bts; // bits left in current byte
		while (gotbits<ab)
		{
			i++;
			w=(short)(((int)t[ptr+i])&0xff);
			r|=w<<gotbits;
			gotbits+=8;
		}
		bts+=ab;
		while (bts>=8)
		{
			bts-=8;
			ptr++;
		}
		w=(short)(r&mask);
		position[0]=ptr;
		position[1]=bts;
		return w;  
	}

// array t has ab active bits per word
// extract bytes from array of words
// if max!=0 then -max<=t[i]<=+max
	static byte nextbyte16(int ab,short[] t,int[] position)
	{
		int ptr=position[0]; // index in array
		int bts=position[1]; // bit index in word
		short r,w;
		int left=ab-bts; // number of bits left in this word
		int i=0;
		int k=ptr%256;

		w=t[k]; w+=(w>>15)&KY_PRIME;
		r=(short)(w>>bts);
		while (left<8)
		{
			i++;
			w=t[k+i]; w+=(w>>15)&KY_PRIME;
			r|=w<<left;
			left+=ab;
		}

		bts+=8;
		while (bts>=ab)
		{
			bts-=ab;
			ptr++;
		}
		position[0]=ptr;
		position[1]=bts;
		return (byte)(r&0xff);        
	}

// encode polynomial vector with coefficients of length L, into packed bytes
// pos indicates current position in vector t
// pptr indicates which vector in the matrix
	static void encode(short[] t,int pos[],int L,byte[] pack,int pptr)
	{
		int n;
		int k=(KY_DEGREE*L)/8;  // compressed length
		for (n=0;n<k;n++ ) {
			pack[n+pptr*k]=nextbyte16(L,t,pos);
		}
	}

	static byte chk_encode(short[] t,int pos[],int L,byte[] pack,int pptr)
	{
		int n;
		int k=(KY_DEGREE*L)/8;
		byte m,diff=0;
		for (n=0;n<k;n++ ) {
			m=nextbyte16(L,t,pos);
			diff|=(m^pack[n+pptr*k]);
		}    
		return diff;
	}

// decode packed bytes into polynomial vector, with coefficients of length L
// pos indicates current position in byte array pack
	static void decode(byte[] pack,int L,short[] t,int pos[])
	{
		int i;
		for (i=0;i<KY_DEGREE;i++ )
			t[i]=nextword(L,pack,pos);
	}

// Bernsteins safe division by 0xD01
	static int safediv(int x)
    {
        int qpart,q=0;    
 
        qpart=(int)(((long)x*645083)>>31);
        x-=qpart*0xD01; q += qpart;

        qpart=(int)(((long)x*645083)>>31)+1;
        x-=qpart*0xD01; q += qpart+(x>>31);

        return q;
    }

// compress polynomial coefficents in place, for polynomial vector of length len
	static void compress(short[] t,int d)
	{
		int twod=(1<<d);
		int ti;
		for (int i=0;i<KY_DEGREE;i++)
		{
			t[i]+=(t[i]>>15)&KY_PRIME;
			t[i]= (short)(safediv((twod*t[i]+KY_PRIME/2))&(twod-1));
		}
	}

// decompress polynomial coefficents in place, for polynomial vector of length len
	static void decompress(short[] t,int d)
	{
		int twod1=(1<<(d-1));
		for (int i=0;i<KY_DEGREE;i++)
			t[i]=(short)((KY_PRIME*t[i]+twod1)>>d);
	}


// input entropy, output key pair
	static void CPA_keypair(int[] params,byte[] tau,byte[] sk,byte[] pk)
	{
		int i,j,k;
		SHA3 sh = new SHA3(SHA3.HASH512);
		byte[] rho = new byte[32]; 
		byte[] sigma = new byte[33]; 
		byte[] buff = new byte[256]; 

		int ck=params[0];
		short[] r = new short[KY_DEGREE];
		short[] w = new short[KY_DEGREE];
		short[] Aij = new short[KY_DEGREE];

		short[][] s = new short[ck][KY_DEGREE];
		short[][] e = new short[ck][KY_DEGREE];
		short[][] p = new short[ck][KY_DEGREE];

		int eta1=params[1];
		int public_key_size=32+ck*(KY_DEGREE*3)/2;
		int secret_cpa_key_size=ck*(KY_DEGREE*3)/2;
   
		for (i=0;i<32;i++)
			sh.process(tau[i]); 

        sh.process((byte)ck);

		byte[] bf = sh.hash();
		for (i=0;i<32;i++)
		{
			rho[i]=bf[i];
			sigma[i]=bf[i+32];
		}
		sigma[32]=0;   // N

// create s
		for (i=0;i<ck;i++)
		{
			sh=new SHA3(SHA3.SHAKE256);
			for (j=0;j<33;j++)
				sh.process(sigma[j]); 
			sh.shake(buff,64*eta1);
			CBD(buff,eta1,s[i]);
			sigma[32]+=1;
		}

// create e
		for (i=0;i<ck;i++)
		{
			sh=new SHA3(SHA3.SHAKE256);
			for (j=0;j<33;j++)
				sh.process(sigma[j]); 
			sh.shake(buff,64*eta1);
			CBD(buff,eta1,e[i]);
			sigma[32]+=1;
		}

		for (k=0;k<ck;k++)
		{
			poly_ntt(s[k]);
			poly_ntt(e[k]);
		}

		for (i=0;i<ck;i++)
		{
			ExpandAij(rho,Aij,i,0);
			poly_mul(r,Aij,s[0]);

			for (j=1;j<ck;j++)
			{
				ExpandAij(rho,Aij,i,j);
				poly_mul(w,s[j],Aij);
				poly_add(r,r,w);
			}
			poly_reduce(r);
			poly_tomont(r);
			poly_add(p[i],r,e[i]);
			poly_reduce(p[i]);
		}

		int[] pos=new int[2];
		pos[0]=pos[1]=0;
		for (i=0;i<ck;i++)
			encode(s[i],pos,12,sk,i);
		pos[0]=pos[1]=0;
		for (i=0;i<ck;i++)
			encode(p[i],pos,12,pk,i);
		for (i=0;i<32;i++)
			pk[public_key_size-32+i]=rho[i];
	}

// input 64 random bytes, output secret and public keys
	static void CCA_keypair(int[] params,byte[] randbytes64,byte[] sk,byte[] pk)
	{
		SHA3 sh = new SHA3(SHA3.HASH256);
		int i;
		int sks=(params[0]*(KY_DEGREE*3)/2);
		int pks=(32+params[0]*(KY_DEGREE*3)/2);

		CPA_keypair(params,randbytes64,sk,pk);
		for (i=0;i<pks;i++)
			sk[sks+i]=pk[i];

		for (i=0;i<pks;i++)
			sh.process(pk[i]);
		byte[] h=sh.hash();
		for (i=0;i<32;i++)
			sk[sks+pks+i]=h[i];
		for (i=0;i<32;i++)
			sk[sks+pks+32+i]=randbytes64[32+i];
	}

	static void CPA_base_encrypt(int[] params,byte[] coins,byte[] pk,byte[] ss,short[][] u,short[] v)
	{
		int i,j;
		byte[] sigma=new byte[33];
		byte[] buff=new byte[256];
		byte[] rho=new byte[32];

		int ck=params[0];
		short[] r=new short[KY_DEGREE];
		short[] w=new short[KY_DEGREE];
		short[] Aij=new short[KY_DEGREE];
		short[][] q=new short[ck][KY_DEGREE];
		short[][] p=new short[ck][KY_DEGREE];

		int eta1=params[1];
		int eta2=params[2];
		int du=params[3];
		int dv=params[4];
		int public_key_size=32+ck*(KY_DEGREE*3)/2;

		for (i=0;i<32;i++)
			sigma[i]=coins[i];//i+6; //RAND_byte(RNG);
		sigma[32]=0;

		for (i=0;i<32;i++)
			rho[i]=pk[public_key_size-32+i];

// create q
		for (i=0;i<ck;i++)
		{
			SHA3 sh = new SHA3(SHA3.SHAKE256);
			for (j=0;j<33;j++)
				sh.process(sigma[j]); 
			sh.shake(buff,64*eta1);
			CBD(buff,eta1,q[i]);
			sigma[32]+=1;
		}
// create e1
		for (i=0;i<ck;i++)
		{
			SHA3 sh = new SHA3(SHA3.SHAKE256);
			for (j=0;j<33;j++)
				sh.process(sigma[j]); 
			sh.shake(buff,64*eta2);
			CBD(buff,eta1,u[i]);          // e1
			sigma[32]+=1;
		}
		for (i=0;i<ck;i++)
			poly_ntt(q[i]);
	
		for (i=0;i<ck;i++)
		{
			ExpandAij(rho,Aij,0,i);
			poly_mul(r,Aij,q[0]);
			for (j=1;j<ck;j++)
			{
				ExpandAij(rho,Aij,j,i);
				poly_mul(w,q[j],Aij);
				poly_add(r,r,w);
			}
			poly_reduce(r);
			poly_invntt(r);
			poly_add(u[i],u[i],r);
			poly_reduce(u[i]);
		}

		int[] pos=new int[2];
		pos[0]=pos[1]=0;
		for (i=0;i<ck;i++)
			decode(pk,12,p[i],pos);	
	
		poly_mul(v,p[0],q[0]);

		for (i=1;i<ck;i++)
		{
			poly_mul(r,p[i],q[i]);
			poly_add(v,v,r);
		}    
		poly_invntt(v);        

// create e2
		SHA3 sh = new SHA3(SHA3.SHAKE256);
		for (j=0;j<33;j++)
			sh.process(sigma[j]); 
		sh.shake(buff,64*eta2);
		CBD(buff,eta1,w);  // e2

		poly_add(v,v,w);
		pos[0]=pos[1]=0;
		decode(ss,1,r,pos);
		decompress(r,1);

		poly_add(v,v,r);
		poly_reduce(v);

		for (i=0;i<ck;i++)
			compress(u[i],du);
		compress(v,dv);		
	}

// Given input of entropy, public key and shared secret is an input, outputs ciphertext
	static void CPA_encrypt(int[] params,byte[] coins,byte[] pk,byte[] ss,byte[] ct)
	{
		int i,ck=params[0];
		short[] v=new short[KY_DEGREE];
		short[][] u=new short[ck][KY_DEGREE];
		int du=params[3];
		int dv=params[4];
		int ciphertext_size=(du*ck+dv)*KY_DEGREE/8;

		CPA_base_encrypt(params,coins,pk,ss,u,v);
		int[] pos=new int[2];
		pos[0]=pos[1]=0;
		for (i=0;i<ck;i++)
			encode(u[i],pos,du,ct,i);

        int len=(dv*KY_DEGREE/8); // last part of CT
        byte[] last=new byte[len];

		encode(v,pos,dv,last,0);
        for (i=0;i<len;i++)
            ct[ciphertext_size-len+i]=last[i];
	}

// Re-encrypt and check that ct is OK (if so return is zero)
	static byte CPA_check_encrypt(int[] params,byte[] coins,byte[] pk,byte[] ss,byte[] ct)
	{
		int i,ck=params[0];
		short[] v=new short[KY_DEGREE];
		short[][] u=new short[ck][KY_DEGREE];
		int du=params[3];
		int dv=params[4];
		int ciphertext_size=(du*ck+dv)*KY_DEGREE/8;
		byte d1,d2;
		
		CPA_base_encrypt(params,coins,pk,ss,u,v);
		int[] pos=new int[2];
		pos[0]=pos[1]=0;
		d1=0;
		for (i=0;i<ck;i++)
			d1|=chk_encode(u[i],pos,du,ct,i);

        int len=(dv*KY_DEGREE/8); // last part of CT
        byte[] last=new byte[len];
        for (i=0;i<len;i++)
            last[i]=ct[ciphertext_size-len+i];

		d2=chk_encode(v,pos,dv,last,0);

		if ((d1|d2)==0)
			return 0;
		else
			return (byte)0xff;
	}

	static void CCA_encrypt(int[] params,byte[] randbytes32,byte[] pk,byte[] ss,byte[] ct)
	{
		int i;
		byte[] coins = new byte[32];

		int ck=params[0];
		int du=params[3];
		int dv=params[4];
		int shared_secret_size=params[5];
		int public_key_size=32+ck*(KY_DEGREE*3)/2;
		int ciphertext_size=(du*ck+dv)*KY_DEGREE/8;

        byte[] hm=new byte[32];
        for (i=0;i<32;i++)
            hm[i]=randbytes32[i];
    
		//SHA3 sh = new SHA3(SHA3.HASH256);
		//for (i=0;i<32;i++)
		//	sh.process(randbytes32[i]);
		//byte[] hm= sh.hash();

		SHA3 sh = new SHA3(SHA3.HASH256);
		for (i=0;i<public_key_size;i++)
			sh.process(pk[i]);
		byte[] h= sh.hash();

		sh = new SHA3(SHA3.HASH512);
		for (i=0;i<32;i++)
			sh.process(hm[i]);
		for (i=0;i<32;i++)
			sh.process(h[i]);
		byte[] g= sh.hash();

		for (i=0;i<32;i++)
			coins[i]=g[i+32];

		CPA_encrypt(params,coins,pk,hm,ct);

        for (i=0;i<32;i++) 
            ss[i]=g[i];

		//sh = new SHA3(SHA3.HASH256);
		//for (i=0;i<ciphertext_size;i++)
		//	sh.process(ct[i]);
		//h= sh.hash();

		//sh = new SHA3(SHA3.SHAKE256);
		//for (i=0;i<32;i++)
		//	sh.process(g[i]);
		//for (i=0;i<32;i++)
		//	sh.process(h[i]);

		//sh.shake(ss,shared_secret_size);
	}

	static void CPA_decrypt(int[] params,byte[] SK,byte[] CT,byte[] SS)
	{
		int i,j;
		int ck=params[0];
		short[] w=new short [KY_DEGREE];
		short[] v=new short [KY_DEGREE];
		short[] r=new short [KY_DEGREE];

		short[][] u=new short[ck][KY_DEGREE];
		short[][] s=new short[ck][KY_DEGREE];

		int du=params[3];
		int dv=params[4];
		int shared_secret_size=params[5];

		int[] pos=new int[2];
		pos[0]=pos[1]=0;
		for (i=0;i<ck;i++)
			decode(CT,du,u[i],pos);
		decode(CT,dv,v,pos);
		for (i=0;i<ck;i++)
			decompress(u[i],du);
		decompress(v,dv);
		pos[0]=pos[1]=0;
		for (i=0;i<ck;i++)
			decode(SK,12,s[i],pos);

		poly_ntt(u[0]);
		poly_mul(w,u[0],s[0]);
		for (i=1;i<ck;i++)
		{
			poly_ntt(u[i]);
			poly_mul(r,u[i],s[i]);
			poly_add(w,w,r);
		}
		poly_reduce(w);
		poly_invntt(w);
		poly_sub(v,v,w);
		compress(v,1);
		pos[0]=pos[1]=0;
		encode(v,pos,1,SS,0); 
	}

	static void CCA_decrypt(int[] params,byte[] SK,byte[] CT,byte[] SS)
	{
		int i,olen,same;
		int ck=params[0];
		int du=params[3];
		int dv=params[4];
		int secret_cpa_key_size=ck*(KY_DEGREE*3)/2;
		int public_key_size=32+ck*(KY_DEGREE*3)/2;
		int shared_secret_size=params[5];
		int ciphertext_size=(du*ck+dv)*KY_DEGREE/8;

		byte[] h=new byte[32];
		byte[] z=new byte[32];
		byte[] m=new byte[32];
		byte[] coins=new byte[32];
		byte[] PK=new byte[public_key_size];

		for (i=0;i<public_key_size;i++)
			PK[i]=SK[secret_cpa_key_size+i];

		for (i=0;i<32;i++)
			h[i]=SK[secret_cpa_key_size+public_key_size+i];
		for (i=0;i<32;i++)
			z[i]=SK[secret_cpa_key_size+public_key_size+32+i];

		CPA_decrypt(params,SK,CT,m);

		SHA3 sh = new SHA3(SHA3.HASH512);
		for (i=0;i<32;i++)
			sh.process(m[i]);
		for (i=0;i<32;i++)
			sh.process(h[i]);
		byte[] g= sh.hash();

		for (i=0;i<32;i++)
			coins[i]=g[i+32];

        sh = new SHA3(SHA3.SHAKE256);
        for (i=0;i<32;i++)
            sh.process(z[i]);
        for (i=0;i<ciphertext_size;i++)
            sh.process(CT[i]);
        sh.shake(SS,shared_secret_size);

		byte mask=CPA_check_encrypt(params,coins,PK,m,CT);

        for (i=0;i<32;i++)
            SS[i]^=(SS[i]^g[i])&(~mask);

		//for (i=0;i<32;i++)
		//	g[i]^=(g[i]^z[i])&mask;               // substitute z for Kb on failure

		//sh = new SHA3(SHA3.HASH256);
		//for (i=0;i<ciphertext_size;i++)
		//	sh.process(CT[i]);
		//h=sh.hash();

		//sh = new SHA3(SHA3.SHAKE256);
		//for (i=0;i<32;i++)
		//	sh.process(g[i]);
		//for (i=0;i<32;i++)
		//	sh.process(h[i]);
		//sh.shake(SS,shared_secret_size);
	}

	public static void keypair512(byte[] r64,byte[] SK,byte[] PK)
	{
		CCA_keypair(PARAMS_512,r64,SK,PK);
	}

	public static void encrypt512(byte r32[],byte[] PK,byte[] SS,byte[] CT)
	{
		CCA_encrypt(PARAMS_512,r32,PK,SS,CT);
	}

	public static void decrypt512(byte[] SK,byte[] CT,byte[] SS)
	{
		CCA_decrypt(PARAMS_512,SK,CT,SS);
	}


	public static void keypair768(byte[] r64,byte[] SK,byte[] PK)
	{
		CCA_keypair(PARAMS_768,r64,SK,PK);
	}

	public static void encrypt768(byte r32[],byte[] PK,byte[] SS,byte[] CT)
	{
		CCA_encrypt(PARAMS_768,r32,PK,SS,CT);
	}

	public static void decrypt768(byte[] SK,byte[] CT,byte[] SS)
	{
		CCA_decrypt(PARAMS_768,SK,CT,SS);
	}

	public static void keypair1024(byte[] r64,byte[] SK,byte[] PK)
	{
		CCA_keypair(PARAMS_1024,r64,SK,PK);
	}

	public static void encrypt1024(byte r32[],byte[] PK,byte[] SS,byte[] CT)
	{
		CCA_encrypt(PARAMS_1024,r32,PK,SS,CT);
	}

	public static void decrypt1024(byte[] SK,byte[] CT,byte[] SS)
	{
		CCA_decrypt(PARAMS_1024,SK,CT,SS);
	}
}
