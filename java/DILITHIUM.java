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

/* Dilithium API implementation. Constant time where it matters. Spends nearly all of its time running SHA3. Small.

The Matrix A is calculated on-the-fly to keep memory requirement minimal
Note that 
1. A precalculated A can be included in the public key, for use by signature and verification (which blows up public key size)
2. Precalculating A for signature calculation means that the A does not have to re-calculated for each attempt to find a good signature

Might be simpler to wait for hardware support for SHA3 before attempting further optimization!

   M.Scott 13/10/2022
*/

package org.miracl.core;

public final class DILITHIUM {

//q= 8380417
    public static final int DL_LGN = 8;
    public static final int DL_DEGREE = (1 << DL_LGN);
    public static final int DL_PRIME = 0x7fe001;
    public static final int DL_D = 13;
    public static final int DL_TD = (23-DL_D);

    public static final int DL_ONE = 0x3FFE00;    // R mod Q
    public static final int DL_COMBO = 0xA3FA;    // ONE*inv mod Q
    public static final int DL_R2MODP = 0x2419FF; // R^2 mod Q
    public static final int DL_ND = 0xFC7FDFFF;   // 1/(R-Q) mod R

    public static final int DL_MAXLG = 19;
    public static final int DL_MAXK = 8;     // could reduce these if not using highest security
    public static final int DL_MAXL = 7;
    public static final int DL_YBYTES = (((DL_MAXLG+1)*DL_DEGREE)/8);

    public static final int SK_SIZE_2 = (32*3+DL_DEGREE*(4*13+4*3+4*3)/8);
    public static final int PK_SIZE_2 = ((4*DL_DEGREE*DL_TD)/8+32);
    public static final int SIG_SIZE_2 = ((DL_DEGREE*4*(17+1))/8+80+4+32);

    public static final int SK_SIZE_3 = (32*3+DL_DEGREE*(6*13+5*4+6*4)/8);
    public static final int PK_SIZE_3 = ((6*DL_DEGREE*DL_TD)/8+32);
    public static final int SIG_SIZE_3 = ((DL_DEGREE*5*(19+1))/8+55+6+32);

    public static final int SK_SIZE_5 = (32*3+DL_DEGREE*(8*13+7*3+8*3)/8);
    public static final int PK_SIZE_5 = ((8*DL_DEGREE*DL_TD)/8+32);
    public static final int SIG_SIZE_5 = ((DL_DEGREE*7*(19+1))/8+75+8+32);

// parameters for each security level
// tau,gamma1,gamma2,K,L,eta,lg(2*eta+1),omega
	public static final int[] PARAMS_2 = {39,17,88,4,4,2,3,80};
	public static final int[] PARAMS_3 = {49,19,32,6,5,4,4,55};
	public static final int[] PARAMS_5 = {60,19,32,8,7,2,3,75};

	static final int[] roots =  {0x3ffe00,0x64f7,0x581103,0x77f504,0x39e44,0x740119,0x728129,0x71e24,0x1bde2b,0x23e92b,0x7a64ae,0x5ff480,0x2f9a75,0x53db0a,0x2f7a49,0x28e527,0x299658,0xfa070,0x6f65a5,0x36b788,0x777d91,0x6ecaa1,0x27f968,0x5fb37c,0x5f8dd7,0x44fae8,0x6a84f8,0x4ddc99,0x1ad035,0x7f9423,0x3d3201,0x445c5,0x294a67,0x17620,0x2ef4cd,0x35dec5,0x668504,0x49102d,0x5927d5,0x3bbeaf,0x44f586,0x516e7d,0x368a96,0x541e42,0x360400,0x7b4a4e,0x23d69c,0x77a55e,0x65f23e,0x66cad7,0x357e1e,0x458f5a,0x35843f,0x5f3618,0x67745d,0x38738c,0xc63a8,0x81b9a,0xe8f76,0x3b3853,0x3b8534,0x58dc31,0x1f9d54,0x552f2e,0x43e6e6,0x688c82,0x47c1d0,0x51781a,0x69b65e,0x3509ee,0x2135c7,0x67afbc,0x6caf76,0x1d9772,0x419073,0x709cf7,0x4f3281,0x4fb2af,0x4870e1,0x1efca,0x3410f2,0x70de86,0x20c638,0x296e9f,0x5297a4,0x47844c,0x799a6e,0x5a140a,0x75a283,0x6d2114,0x7f863c,0x6be9f8,0x7a0bde,0x1495d4,0x1c4563,0x6a0c63,0x4cdbea,0x40af0,0x7c417,0x2f4588,0xad00,0x6f16bf,0xdcd44,0x3c675a,0x470bcb,0x7fbe7f,0x193948,0x4e49c1,0x24756c,0x7ca7e0,0xb98a1,0x6bc809,0x2e46c,0x49a809,0x3036c2,0x639ff7,0x5b1c94,0x7d2ae1,0x141305,0x147792,0x139e25,0x67b0e1,0x737945,0x69e803,0x51cea3,0x44a79d,0x488058,0x3a97d9,0x1fea93,0x33ff5a,0x2358d4,0x3a41f8,0x4cdf73,0x223dfb,0x5a8ba0,0x498423,0x412f5,0x252587,0x6d04f1,0x359b5d,0x4a28a1,0x4682fd,0x6d9b57,0x4f25df,0xdbe5e,0x1c5e1a,0xde0e6,0xc7f5a,0x78f83,0x67428b,0x7f3705,0x77e6fd,0x75e022,0x503af7,0x1f0084,0x30ef86,0x49997e,0x77dcd7,0x742593,0x4901c3,0x53919,0x4610c,0x5aad42,0x3eb01b,0x3472e7,0x4ce03c,0x1a7cc7,0x31924,0x2b5ee5,0x291199,0x585a3b,0x134d71,0x3de11c,0x130984,0x25f051,0x185a46,0x466519,0x1314be,0x283891,0x49bb91,0x52308a,0x1c853f,0x1d0b4b,0x6fd6a7,0x6b88bf,0x12e11b,0x4d3e3f,0x6a0d30,0x78fde5,0x1406c7,0x327283,0x61ed6f,0x6c5954,0x1d4099,0x590579,0x6ae5ae,0x16e405,0xbdbe7,0x221de8,0x33f8cf,0x779935,0x54aa0d,0x665ff9,0x63b158,0x58711c,0x470c13,0x910d8,0x463e20,0x612659,0x251d8b,0x2573b7,0x7d5c90,0x1ddd98,0x336898,0x2d4bb,0x6d73a8,0x4f4cbf,0x27c1c,0x18aa08,0x2dfd71,0xc5ca5,0x19379a,0x478168,0x646c3e,0x51813d,0x35c539,0x3b0115,0x41dc0,0x21c4f7,0x70fbf5,0x1a35e7,0x7340e,0x795d46,0x1a4cd0,0x645caf,0x1d2668,0x666e99,0x6f0634,0x7be5db,0x455fdc,0x530765,0x5dc1b0,0x7973de,0x5cfd0a,0x2cc93,0x70f806,0x189c2a,0x49c5aa,0x776a51,0x3bcf2c,0x7f234f,0x6b16e0,0x3c15ca,0x155e68,0x72f6b7,0x1e29ce};
	static final int[] iroots = {0x3ffe00,0x7f7b0a,0x7eafd,0x27cefe,0x78c1dd,0xd5ed8,0xbdee8,0x7c41bd,0x56fada,0x5065b8,0x2c04f7,0x50458c,0x1feb81,0x57b53,0x5bf6d6,0x6401d6,0x7b9a3c,0x42ae00,0x4bde,0x650fcc,0x320368,0x155b09,0x3ae519,0x20522a,0x202c85,0x57e699,0x111560,0x86270,0x492879,0x107a5c,0x703f91,0x5649a9,0x2ab0d3,0x6042ad,0x2703d0,0x445acd,0x44a7ae,0x71508b,0x77c467,0x737c59,0x476c75,0x186ba4,0x20a9e9,0x4a5bc2,0x3a50a7,0x4a61e3,0x19152a,0x19edc3,0x83aa3,0x5c0965,0x495b3,0x49dc01,0x2bc1bf,0x49556b,0x2e7184,0x3aea7b,0x442152,0x26b82c,0x36cfd4,0x195afd,0x4a013c,0x50eb34,0x7e69e1,0x56959a,0x454828,0x375fa9,0x3b3864,0x2e115e,0x15f7fe,0xc66bc,0x182f20,0x6c41dc,0x6b686f,0x6bccfc,0x2b520,0x24c36d,0x1c400a,0x4fa93f,0x3637f8,0x7cfb95,0x1417f8,0x744760,0x33821,0x5b6a95,0x319640,0x66a6b9,0x2182,0x38d436,0x4378a7,0x7212bd,0x10c942,0x7f3301,0x509a79,0x781bea,0x7bd511,0x330417,0x15d39e,0x639a9e,0x6b4a2d,0x5d423,0x13f609,0x59c5,0x12beed,0xa3d7e,0x25cbf7,0x64593,0x385bb5,0x2d485d,0x567162,0x5f19c9,0xf017b,0x4bcf0f,0x7df037,0x376f20,0x302d52,0x30ad80,0xf430a,0x3e4f8e,0x62488f,0x13308b,0x183045,0x5eaa3a,0x4ad613,0x1629a3,0x2e67e7,0x381e31,0x17537f,0x3bf91b,0x61b633,0xce94a,0x6a8199,0x43ca37,0x14c921,0xbcb2,0x4410d5,0x875b0,0x361a57,0x6743d7,0xee7fb,0x7d136e,0x22e2f7,0x66c23,0x221e51,0x2cd89c,0x3a8025,0x3fa26,0x10d9cd,0x197168,0x62b999,0x1b8352,0x659331,0x682bb,0x78abf3,0x65aa1a,0xee40c,0x5e1b0a,0x7bc241,0x44deec,0x4a1ac8,0x2e5ec4,0x1b73c3,0x385e99,0x66a867,0x73835c,0x51e290,0x6735f9,0x7d63e5,0x309342,0x126c59,0x7d0b46,0x4c7769,0x620269,0x28371,0x5a6c4a,0x5ac276,0x1eb9a8,0x39a1e1,0x76cf29,0x38d3ee,0x276ee5,0x1c2ea9,0x198008,0x2b35f4,0x846cc,0x4be732,0x5dc219,0x74041a,0x68fbfc,0x14fa53,0x26da88,0x629f68,0x1386ad,0x1df292,0x4d6d7e,0x6bd93a,0x6e21c,0x15d2d1,0x32a1c2,0x6cfee6,0x145742,0x10095a,0x62d4b6,0x635ac2,0x2daf77,0x362470,0x57a770,0x6ccb43,0x397ae8,0x6785bb,0x59efb0,0x6cd67d,0x41fee5,0x6c9290,0x2785c6,0x56ce68,0x54811c,0x7cc6dd,0x65633a,0x32ffc5,0x4b6d1a,0x412fe6,0x2532bf,0x7b7ef5,0x7aa6e8,0x36de3e,0xbba6e,0x8032a,0x364683,0x4ef07b,0x60df7d,0x2fa50a,0x9ffdf,0x7f904,0xa8fc,0x189d76,0x78507e,0x7360a7,0x71ff1b,0x6381e7,0x7221a3,0x30ba22,0x1244aa,0x395d04,0x35b760,0x4a44a4,0x12db10,0x5aba7a,0x7bcd0c,0x365bde,0x255461,0x5da206,0x33008e,0x459e09,0x5c872d,0x4be0a7,0x5ff56e};

    static int round(int a, int b) {
        return (a + b / 2) / b;
    }

    /* constant time absolute vaue */
    static int nabs(int x) {
        int mask = (x >> 31);
        return (x + mask) ^ mask;
    }

    /* Montgomery stuff */

    static int redc(long T) {
        long m = (T * DL_ND) & 0xffffffffL;
        return (int)((m * DL_PRIME + T) >>> WL);
    }

    static int nres(int x) {
        return redc((long)x * DL_R2MODP);
    }

    static int modmul(int a, int b) {
        return redc((long)a * b);
    }

// make all elements +ve
    static void poly_pos(int[] p) {
        for (int i=0;i<DL_DEGREE;i++)
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
// note that ntt() and intt() are not mutually inverse

/* NTT code */
/* Cooley-Tukey NTT */
/* Excess of 2 allowed on input - coefficients must be < 2*PRIME */

    static void ntt(int[] x)
    {
        int m, i, j, start, len = DL_DEGREE / 2;
        int S, V, q = DL_PRIME;

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
public static final int NTTL=1; // maybe should be 2?

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
                     if (NTTL>1) {
				        if (m<NTTL && j<k+lim)   // This should be unwound for timings
				        { // need to knock back excesses. Never used if NTTL=1.
				    	    U=modmul(x[j],DL_ONE);  
				    	    V=modmul(x[j+t],DL_ONE); 
				        } else {
                            U = x[j];
                            V = x[j + t];
                        }
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

    static void nres_it(int[] p) {
        for (int i = 0; i < DL_DEGREE; i++)
            p[i] = nres(p[i]);
    }

    static void redc_it(int[] p) {
        for (int i = 0; i < DL_DEGREE; i++)
            p[i] = redc(p[i]);
    }
// copy polynomial
    static void poly_copy(int[] p1, int[] p2) {
        for (int i = 0; i < DL_DEGREE; i++)
            p1[i] = p2[i];
    }

// copy from small polynomial
    static void poly_scopy(int[] p1, byte[] p2) {
        for (int i = 0; i < DL_DEGREE; i++)
            p1[i] = (int)p2[i];
    }

// copy from medium polynomial
    static void poly_mcopy(int[] p1, short[] p2) {
        for (int i = 0; i < DL_DEGREE; i++)
            p1[i] = (int)p2[i];
    }

    static void poly_zero(int[] p1) {
        for (int i = 0; i < DL_DEGREE; i++)
            p1[i] = 0;
    }

    static void poly_negate(int[] p1,int[] p2) {
        for (int i = 0; i < DL_DEGREE; i++)
            p1[i] = DL_PRIME-p2[i];
    }

    static void poly_mul(int[] p1, int[] p2, int[] p3) {
        for (int i = 0; i < DL_DEGREE; i++)
            p1[i] = modmul(p2[i], p3[i]);
    }

    static void poly_add(int[] p1, int[] p2, int[] p3) {
        for (int i = 0; i < DL_DEGREE; i++)
            p1[i] = (p2[i] + p3[i]);
    }

    static void poly_sub(int[] p1, int[] p2, int[] p3) {
        for (int i = 0; i < DL_DEGREE; i++)
            p1[i] = (p2[i] + DL_PRIME - p3[i]);
    }
/* reduces inputs that are already < 2q */
    static void poly_soft_reduce(int[] poly) {
        int e;
        for (int i = 0; i < DL_DEGREE; i++)
        {
            e = poly[i] - DL_PRIME;
            poly[i] = e + ((e >> 31)&DL_PRIME);
        }
    }

/* fully reduces modulo q */
    static void poly_hard_reduce(sign32 *poly) {
        int e;
        for (int i = 0; i < DL_DEGREE; i++)
        {
            e = modmul(poly[i], DL_ONE); // reduces to < 2q
            e = e - DL_PRIME;
            poly[i] = e + ((e >> 31)&DL_PRIME); // finishes it off
        }
    }

// Generate A[i][j] from rho
    static void ExpandAij(byte[] rho,int[] Aij,int i,int j)
    {
		SHA3 sh = new SHA3(SHA3.SHAKE128);
        int m,n;
        int b0,b1,b2;
        int cf;
        byte buff[840];  // should be plenty
        for (m=0;m<32;m++)
            sh.process(rho[m]);
        sh.process(j&0xff);
        sh.process(i&0xff);
        sh.shake(buff,840);
        m=n=0;
        while (m<DL_DEGREE)
        {
            int b0=(int)(buff[n++])&0xff; 
            int b1=(int)(buff[n++])&0xff; 
            int b2=(int)(buff[n++])&0xff; 
            cf=((b2&0x7F)<<16)+(b1<<8)+b0;
            if (cf>=DL_PRIME) continue;
            Aij[m++]=cf;
        }
    }

// array t has ab active bits per word
// extract bytes from array of words
// if max!=0 then -max<=t[i]<=+max
    static byte nextbyte32(int ab,int max,int[] t,int[] position)
    {
	    int ptr=position[0]; // index in array
	    int bts=position[1]; // bit index in word
        int r,w;
        int left=ab-bts; // number of bits left in this word
        int i=0;
        int k=ptr%256;
        w=t[k];
        if (max!=0)
            w=max-w;
        r=w>>bts;
        while (left<8)
        {
            i++;
            w=t[k+i];
            if (max!=0)
                w=max-w;
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
        return (byte)r&0xff;        
    }

// array t has ab active bits per word
// extract dense bytes from array of words
// if max!=0 then -max<=t[i]<=+max
    static byte nextbyte16(int ab,int max,short[] t,int[] position)
    {
		int ptr=position[0]; // index in array
		int bts=position[1]; // bit index in word
        int r,w;
        int left=ab-bts; // number of bits left in this word
        int i=0;
        int k=ptr%256;
        w=t[k];
        if (max!=0)
            w=max-w;
        r=w>>bts;
        while (left<8)
        {
            i++;
            w=t[k+i];
            if (max!=0)
                w=max-w;
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
        return (byte)r&0xff;        
    }

// array t has ab active bits per word
// extract dense bytes from array of words
// if max!=0 then -max<=t[i]<=+max
    static byte nextbyte8(int ab,int max,byte[] t,int[] position)
    {
		int ptr=position[0]; // index in array
		int bts=position[1]; // bit index in word
        int r,w;
        int left=ab-bts; // number of bits left in this word
        int i=0;
        int k=ptr%256;
        w=t[k];
        if (max!=0)
            w=max-w;
        r=w>>bts;
        while (left<8)
        {
            i++;
            w=t[k+i];
            if (max!=0)
                w=max-w;
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
        return (byte)r&0xff;        
    }

// extract ab bits into word from dense byte stream
    static int nextword(const int ab,int max,byte[] t,int[] position)
    {
		int ptr=position[0]; // index in array
		int bts=position[1]; // bit index in word
        int r=((int)t[ptr]&0xff)>>bts;
        int mask=(1<<ab)-1;
        int w;
        int i=0;
        int gotbits=8-bts; // bits left in current byte
        while (gotbits<ab)
        {
            i++;
            w=(int)t[ptr+i])&0xff;
            r|=w<<gotbits;
            gotbits+=8;
        }
        bts+=ab;
        while (bts>=8)
        {
            bts-=8;
            ptr++;
        }
        w=r&mask;
        if (max!=0)
            w=max-w;
		position[0]=ptr;
		position[1]=bts;
        return w;  
    }

// pack public key
    static int pack_pk(int[] params,byte[] pk,byte[] rho,short[][] t1)
    {
		int[] pos=new int[2];
		pos[0]=pos[1]=0;
        int i,j,n=0;
        int ck=params[3];
        for (i=0;i<32;i++)
            pk[i]=rho[i];
        n=32; ptr=bts=0;
        for (j=0;j<ck;j++)
        {
            for (i=0;i<(DL_DEGREE*DL_TD)/8;i++ )
                pk[n++]=nextbyte16(DL_TD,0,t1[j],pos);
        }
        return n;
    }

// unpack public key
    static void unpack_pk(int[] params,byte[] rho,short[] t1,byte[] pk)
    {
		int[] pos=new int[2];
		pos[0]=pos[1]=0;
        int i,j,ck=params[3];
        for (i=0;i<32;i++)
            rho[i]=pk[i];
        bts=0; ptr=32;
        for (j=0;j<ck;j++)
        {
            for (i=0;i<DL_DEGREE;i++ )
                t1[j][i]=(sign16)nextword(DL_TD,0,pk,pos);
        }
    }
// secret key of size 32*3+DEGREE*(K*D+L*LG2ETA1+K*LG2ETA1)/8
    static int pack_sk(int[] params,byte[] sk,byte[] rho,byte[] bK,byte[] tr,byte[][] s1,byte[][] s2,short[][] t0)
    {
        int ptr,bts,i,j,n=32;
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
		int[] pos=new int[2];
		pos[0]=pos[1]=0;
        for (j=0;j<el;j++)
        {
            for (i=0;i<(DL_DEGREE*lg2eta1)/8;i++)
                sk[n++]=nextbyte8(lg2eta1,eta,s1[j],pos);
        }
        pos[0]=pos[1]=0;
        for (j=0;j<ck;j++)
        {
            for (i=0;i<(DL_DEGREE*lg2eta1)/8;i++)
                sk[n++]=nextbyte8(lg2eta1,eta,s2[j],pos);
        }
        pos[0]=pos[1]=0;
        for (j=0;j<ck;j++)
        {
            for (i=0;i<(DL_DEGREE*DL_D)/8;i++)
                sk[n++]=nextbyte16(DL_D,(1<<(DL_D-1)),t0[j],pos);
        }
        return n;
    }

    static void unpack_sk(int[] params,byte[] rho,byte[] bK,byte[] tr,byte[] s1,byte[] s2,short[] t0,byte[] sk)
    {
        int i,j,n=32;
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
		int[] pos=new int[2];
		pos[0]=0; pos[1]=n;
        for (j=0;j<el;j++)
        {
            for (i=0;i<DL_DEGREE;i++ )
                s1[j][i]=(byte)nextword(lg2eta1,eta,sk,pos);
        }
        for (j=0;j<ck;j++)
        {
            for (i=0;i<DL_DEGREE;i++ )
                s2[j][i]=(byte)nextword(lg2eta1,eta,sk,pos);
        }
        for (j=0;j<ck;j++)
        {
            for (i=0;i<DL_DEGREE;i++ )
                t0[j][i]=(short)nextword(DL_D,(1<<(DL_D-1)),sk,pos);
        }

    }

// pack signature - changes z 
    static int pack_sig(int[] params,byte[] sig,int[][] z,byte[] ct,byte[] h)
    {
        int lg=params[1];
        int gamma1=1<<lg;
        int ck=params[3];
        int el=params[4];
        int omega=params[7];
        int i,j,m,n;
        int t;

        for (i=0;i<32;i++)
            sig[i]=ct[i];   
        n=32;
		int[] pos=new int[2];
		pos[0]=pos[1]=0;
//pre-process z
        for (i=0;i<el;i++)
        {
            row=DL_DEGREE*i;
            for (m=0;m<DL_DEGREE;m++)
            {
                t=z[i][m];
                if (t>DL_PRIME/2) t-=DL_PRIME;
                t=gamma1-t;
                z[row+m]=t;
            }
        }
        for (j=0;j<el;i++)
        {
            for (i=0;i<(DL_DEGREE*(lg+1))/8;i++) {
                sig[n++]=nextbyte32(lg+1,0,z[j],pos);
        }
        for (i=0;i<omega+ck;i++)
            sig[n++]=h[i];
        return n;
    }

    static void unpack_sig(int[] params,int[][] z,byte[] ct,byte[] h,byte[] sig)
    {
        int lg=params[1];
        int gamma1=1<<lg;
        int ck=params[3];
        int el=params[4];
        int omega=params[7];

        int i,j;
        int t;
        int m=32+(el*DL_DEGREE*(lg+1))/8;
        for (i=0;i<32;i++)
            ct[i]=sig[i];

		int[] pos=new int[2];
		pos[0]=0; pos[1]=32;
        for (j=0;j<el;j++)
        {
            for (i=0;i<DL_DEGREE;i++) {
                t=nextword(lg+1,0,sig,pos);
                t=gamma1-t;
                if (t<0) t+=DL_PRIME;
                z[j][i]=t;
            }
        }
        for (i=0;i<omega+ck;i++)
            h[i]=sig[m++];    
    }

}