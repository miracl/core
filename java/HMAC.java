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

/*
 * Implementation of the Secure Hashing Algorithm (SHA-256)
 *
 * Generates a 256 bit message digest. It should be impossible to come
 * come up with two messages that hash to the same value ("collision free").
 *
 * For use with byte-oriented messages only.
 */

package org.miracl.core;

import org.miracl.core.HASH256;
import org.miracl.core.HASH384;
import org.miracl.core.HASH512;
import org.miracl.core.SHA3;

public final class HMAC {

    public static final int MC_SHA2 = 2;
    public static final int MC_SHA3 = 3;

    /* Convert Integer to n-byte array */
    public static byte[] inttoBytes(int n, int len) {
        int i;
        byte[] b = new byte[len];

        for (i = 0; i < len; i++) b[i] = 0;
        i = len;
        while (n > 0 && i > 0) {
            i--;
            b[i] = (byte)(n & 0xff);
            n /= 256;
        }
        return b;
    }

    public static byte[] GPhashit(int hash, int sha, int pad, int zpad, byte[] A, int n, byte[] B) {
        byte[] R = null;

        if (hash == MC_SHA2)
        {
            if (sha == 32) {
                HASH256 H = new HASH256();
                for (int i=0;i<zpad;i++) H.process(0);
                if (A != null) H.process_array(A); 
                if (n >= 0) H.process_num(n);
                if (B != null) H.process_array(B);
                R = H.hash();
            }
            if (sha == 48) {
                HASH384 H = new HASH384();
                for (int i=0;i<zpad;i++) H.process(0);
                if (A != null) H.process_array(A); 
                if (n >= 0) H.process_num(n);
                if (B != null) H.process_array(B);
                R = H.hash();
            }
            if (sha == 64) {
                HASH512 H = new HASH512();
                for (int i=0;i<zpad;i++) H.process(0);
                if (A != null) H.process_array(A); 
                if (n >= 0) H.process_num(n);
                if (B != null) H.process_array(B);
                R = H.hash();
            }
        }
        if (hash == MC_SHA3)
        {
            SHA3 H = new SHA3(sha);
            for (int i=0;i<zpad;i++) H.process(0);
            if (A != null) H.process_array(A); 
            if (n >= 0) H.process_num(n);
            if (B != null) H.process_array(B);
            R = H.hash();
        }
        if (R == null) return null;

        if (pad == 0) return R;
        /* If pad>0 output is truncated or padded to pad bytes */
        byte[] W = new byte[pad];
        if (pad <= sha) {
            for (int i = 0; i < pad; i++) W[i] = R[i];
        } else {
            for (int i = 0; i < sha; i++) W[i + pad - sha] = R[i];
            for (int i = 0; i < pad - sha; i++) W[i] = 0;
        }
        return W;
    }

    public static byte[] SPhashit(int hash, int hlen, byte[] A)
    {
        return GPhashit(hash, hlen, 0, 0, A, -1, null);
    }

    public static byte[] KDF2(int hash, int sha, byte[] Z, byte[] P, int olen) {
        /* NOTE: the parameter olen is the length of the output k in bytes */
        int hlen = sha;
        byte[] K = new byte[olen];
        byte[] B;
        int counter, cthreshold, k = 0;

        for (int i = 0; i < K.length; i++) K[i] = 0;

        cthreshold = olen / hlen; if (olen % hlen != 0) cthreshold++;

        for (counter = 1; counter <= cthreshold; counter++) {
            B = GPhashit(hash,sha, 0, 0, Z, counter, P);
            if (k + hlen > olen) for (int i = 0; i < olen % hlen; i++) K[k++] = B[i];
            else for (int i = 0; i < hlen; i++) K[k++] = B[i];
        }

        return K;
    }

    /* Password based Key Derivation Function */
    /* Input password p, salt s, and repeat count */
    /* Output key of length olen */
    public static byte[] PBKDF2(int hash, int sha, byte[] Pass, byte[] Salt, int rep, int olen) {
        int i, j, k, len, d, opt;
        d = olen / sha; if (olen % sha != 0) d++;
        byte[] F = new byte[sha];
        byte[] U = new byte[sha];
        byte[] S = new byte[Salt.length + 4];

        byte[] K = new byte[d * sha];
        opt = 0;

        for (i = 1; i <= d; i++) {
            for (j = 0; j < Salt.length; j++) S[j] = Salt[j];
            byte[] N = inttoBytes(i, 4);
            for (j = 0; j < 4; j++) S[Salt.length + j] = N[j];

            HMAC1(hash, sha, F, sha, S, Pass);

            for (j = 0; j < sha; j++) U[j] = F[j];
            for (j = 2; j <= rep; j++) {
                HMAC1(hash, sha, U, sha, U, Pass);
                for (k = 0; k < sha; k++) F[k] ^= U[k];
            }
            for (j = 0; j < sha; j++) K[opt++] = F[j];
        }
        byte[] key = new byte[olen];
        for (i = 0; i < olen; i++) key[i] = K[i];
        return key;
    }

    private static int blksize(int hash,int sha)
    {
        int b=0;
        if (hash == MC_SHA2)
        {
            b=64;
            if (sha > 32)
                b=128;
        }
        if (hash == MC_SHA3)
        {
            b=200-2*sha;
        }
        return b;
    }

    /* Calculate HMAC of m using key k. HMAC is tag of length olen */
    public static int HMAC1(int hash, int sha, byte[] tag, int olen, byte[] K, byte[] M ) {
        /* Input is from an octet m        *
        * olen is requested output length in bytes. k is the key  *
        * The output is the calculated tag */
        
        int b=blksize(hash,sha);
        if (b==0) return 0;

        byte[] B;
        byte[] K0 = new byte[b];
        
        for (int i = 0; i < b; i++) K0[i] = 0;

        if (K.length > b) {
            B = SPhashit(hash,sha,K);
            for (int i = 0; i < sha; i++) K0[i] = B[i];
        } else
            for (int i = 0; i < K.length; i++ ) K0[i] = K[i];

        for (int i = 0; i < b; i++) K0[i] ^= 0x36;
        B = GPhashit(hash, sha, 0, 0, K0, -1, M);

        for (int i = 0; i < b; i++) K0[i] ^= 0x6a;
        B = GPhashit(hash, sha, olen, 0, K0, -1, B);

        for (int i = 0; i < olen; i++) tag[i] = B[i];

        return 1;
    }


    public static byte[] HKDF_Extract(int hash, int hlen, byte[] SALT, byte[] IKM)  { 
        byte[] PRK = new byte[hlen];
	    if (SALT == null) {
		    byte[] H = new byte[hlen];
		    for (int i = 0; i < hlen; i++) H[i]=0;
		    HMAC1(hash,hlen,PRK,hlen,H,IKM);
	    } else {
		    HMAC1(hash,hlen,PRK,hlen,SALT,IKM);
	    }
        return PRK;
    }

    public static byte[] HKDF_Expand(int hash, int hlen, int olen, byte[] PRK, byte[] INFO) { 
	    int i,j,k,m,n = olen/hlen;
	    int flen = olen%hlen;

	    byte[] OKM = new byte[olen];
	    byte[] T = new byte[1+INFO.length];
	    byte[] K = new byte[hlen];

        k=m=0;
	    for (i=1;i<=n;i++) {
		    for (j = 0; j < INFO.length; j++)
			    T[k++] = INFO[j];
		    T[k++]=(byte)i;
		    HMAC1(hash,hlen,K,hlen,PRK,T);
		    k=0;
            if (i==1) T = new byte[INFO.length+1+hlen]; // resize T
		    for (j = 0; j < hlen; j++) {
			    OKM[m++] = K[j];
			    T[k++] = K[j];
		    }
	    }
	    if (flen > 0) {
		    for (j = 0; j < INFO.length; j++) 
			    T[k++] = INFO[j];
		    T[k++] = (byte)(n+1);
		    HMAC1(hash,hlen,K,flen,PRK,T);
		    for (j = 0; j < flen; j++) 
			    OKM[m++] = K[j];
		    
	    }
	    return OKM;
    } 

    static int ceil(int a,int b) {
        return (((a)-1)/(b)+1);
    }

    public static byte[] XOF_Expand(int hlen,int olen,byte[] DST,byte[] MSG) {
        byte[] OKM = new byte[olen];
        SHA3 H = new SHA3(hlen);
        for (int i=0;i<MSG.length;i++ )
            H.process(MSG[i]);
        H.process(olen/256);
        H.process(olen%256);

        for (int i=0;i<DST.length;i++ )
            H.process(DST[i]);
        H.process(DST.length);

        H.shake(OKM,olen);
        return OKM;
    }

    public static byte[] XMD_Expand(int hash,int hlen,int olen,byte[] DST,byte[] MSG) {
        byte[] OKM = new byte[olen];
        byte[] H1 = new byte[hlen];
        byte[] TMP = new byte[DST.length+4];
        byte[] TMP2 = new byte[DST.length+2];

        int ell=ceil(olen,hlen);
        int blk=blksize(hash,hlen);
        TMP[0]=(byte)(olen/256);
        TMP[1]=(byte)(olen%256);
        TMP[2]=(byte)0;
        for (int j=0;j<DST.length;j++)
            TMP[3+j]=DST[j];
        TMP[3+DST.length]=(byte)DST.length;

        byte[] H0=GPhashit(hash, hlen, 0, blk, MSG, -1, TMP);

        int k=0;
        for (int j=0;j<hlen;j++)
            H1[j]=0;
        
        for (int i=1;i<=ell;i++)
        {
            for (int j=0;j<hlen;j++)
                H1[j]^=H0[j];
            TMP2[0]=(byte)i;
            for (int j=0;j<DST.length;j++)
                TMP2[1+j]=DST[j];
            TMP2[1+DST.length]=(byte)DST.length;
            H1=GPhashit(hash, hlen, 0, 0, H1, -1, TMP2);
            for (int j=0;j<hlen && k<olen;j++)
                OKM[k++]=H1[j];
        }
        
        return OKM;
    }

/* Mask Generation Function */

	private static void MGF1(int sha,byte[] Z,int olen,byte[] K)
	{
		int hlen=sha;
		byte[] B;

		int counter,cthreshold,k=0;

		for (int i=0;i<K.length;i++) K[i]=0;

		cthreshold=olen/hlen; if (olen%hlen!=0) cthreshold++;
		for (counter=0;counter<cthreshold;counter++)
		{
			//B=hashit(sha,Z,counter);
            B=GPhashit(MC_SHA2,sha,0,0,Z,counter,null);
			if (k+hlen>olen) for (int i=0;i<olen%hlen;i++) K[k++]=B[i];
			else for (int i=0;i<hlen;i++) K[k++]=B[i];
		}	
	}

	private static void MGF1XOR(int sha,byte[] Z,int olen, byte[] K)
	{
		int hlen=sha;
    //    int olen=K.length;
		byte[] B;

		int counter,cthreshold,k=0;

		cthreshold=olen/hlen; if (olen%hlen!=0) cthreshold++;
		for (counter=0;counter<cthreshold;counter++)
		{
			//B=hashit(sha,Z,counter);
            B=GPhashit(MC_SHA2,sha,0,0,Z,counter,null);
			if (k+hlen>olen) for (int i=0;i<olen%hlen;i++) K[k++]^=B[i];
			else for (int i=0;i<hlen;i++) K[k++]^=B[i];
		}	
	}

/*
	public static void printBinary(byte[] array)
	{
		int i;
		for (i=0;i<array.length;i++)
		{
			System.out.printf("%02x", array[i]);
		}
		System.out.println();
	}  

*/

/* SHAXXX identifier strings */
	private static final byte[] SHA256ID={0x30,0x31,0x30,0x0d,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20};
	private static final byte[] SHA384ID={0x30,0x41,0x30,0x0d,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30};
	private static final byte[] SHA512ID={0x30,0x51,0x30,0x0d,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40};

/* PKCS 1.5 padding of a message to be signed */

	public static boolean PKCS15(int sha,byte[] m,byte[] w,int RFS)
	{
		int olen=RFS;
		int i,hlen=sha;
		int idlen=19; 

		if (olen<idlen+hlen+10) return false;
		//byte[] H=hashit(sha,m,-1);
        byte[] H=SPhashit(MC_SHA2,sha,m);

		for (i=0;i<w.length;i++) w[i]=0;
		i=0;
		w[i++]=0;
		w[i++]=1;
		for (int j=0;j<olen-idlen-hlen-3;j++)
			w[i++]=(byte)0xff;
		w[i++]=0;


		if (hlen==32) for (int j=0;j<idlen;j++) w[i++]=SHA256ID[j];
		if (hlen==48) for (int j=0;j<idlen;j++) w[i++]=SHA384ID[j];
		if (hlen==64) for (int j=0;j<idlen;j++) w[i++]=SHA512ID[j];

		for (int j=0;j<hlen;j++)
			w[i++]=H[j];

		return true;
	}

/* SHAXXX identifier strings */
	private static final byte[] SHA256IDb={0x30,0x2f,0x30,0x0b,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x04,0x20};
	private static final byte[] SHA384IDb={0x30,0x3f,0x30,0x0b,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x04,0x30};
	private static final byte[] SHA512IDb={0x30,0x4f,0x30,0x0b,0x06,0x09,0x60,(byte)0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x04,0x40};

/* Alternate PKCS 1.5 padding of a message to be signed */

	public static boolean PKCS15b(int sha,byte[] m,byte[] w,int RFS)
	{
		int olen=RFS;
		int i,hlen=sha;
		int idlen=17; 

		if (olen<idlen+hlen+10) return false;
		//byte[] H=hashit(sha,m,-1);
        byte[] H=SPhashit(MC_SHA2,sha,m);

		for (i=0;i<w.length;i++) w[i]=0;
		i=0;
		w[i++]=0;
		w[i++]=1;
		for (int j=0;j<olen-idlen-hlen-3;j++)
			w[i++]=(byte)0xff;
		w[i++]=0;


		if (hlen==32) for (int j=0;j<idlen;j++) w[i++]=SHA256IDb[j];
		if (hlen==48) for (int j=0;j<idlen;j++) w[i++]=SHA384IDb[j];
		if (hlen==64) for (int j=0;j<idlen;j++) w[i++]=SHA512IDb[j];

		for (int j=0;j<hlen;j++)
			w[i++]=H[j];

		return true;
	}

    public static byte[] PSS_ENCODE(int sha,byte[] m,RAND rng,int RFS)
    {
        int emlen=RFS;
        int embits=8*emlen-1;

        int hlen=sha;
		byte[] SALT=new byte[hlen];
		for (int i=0;i<hlen;i++) SALT[i]=(byte)(i+1);//(byte)rng.getByte();
        byte mask=(byte)((0xff)>>(8*emlen-embits));
        byte[] H = SPhashit(MC_SHA2,sha,m);

        if (emlen < hlen + hlen +  2) return new byte[0]; 
        
        byte[] MD=new byte[8+hlen+hlen];
        for (int i=0;i<8;i++)
            MD[i]=0;
        for (int i=0;i<hlen;i++)
            MD[8+i]=H[i];
        for (int i=0;i<hlen;i++)
            MD[8+hlen+i]=SALT[i];

//System.out.print("MD= 0x"); printBinary(MD);

        H=SPhashit(MC_SHA2,sha,MD);
		byte[] f=new byte[RFS];
        for (int i=0;i<emlen-hlen-hlen-2;i++)
            f[i]=0;
        f[emlen-hlen-hlen-2]=0x01;
        for (int i=0;i<hlen;i++)
            f[emlen+i-hlen-hlen-1]=SALT[i];
//System.out.print("f= 0x"); printBinary(f);
        MGF1XOR(sha,H,emlen-hlen-1,f);
        f[0]&=mask;
        for (int i=0;i<hlen;i++)
            f[emlen+i-hlen-1]=H[i];
        f[emlen-1]=(byte)0xbc;
        return f;
    }

    public static boolean PSS_VERIFY(int sha,byte[] m,byte[] f)
    {
        int emlen=f.length;
        int embits=8*emlen-1;
        int hlen=sha;
		byte[] SALT=new byte[hlen];
        byte mask=(byte)((0xff)>>(8*emlen-embits));

        byte[] HMASK=SPhashit(MC_SHA2,sha,m);
        if (emlen < hlen + hlen +  2) return false; 
        if (f[emlen-1]!=(byte)0xbc) return false;
        if ((f[0]&(~mask))!=0) return false;

        byte [] DB=new byte[emlen-hlen-1];
        for (int i=0;i<emlen-hlen-1;i++)
            DB[i]=f[i];
        byte [] H=new byte[hlen];
        for (int i=0;i<hlen;i++)
            H[i]=f[emlen+i-hlen-1];

        MGF1XOR(sha,H,emlen-hlen-1,DB);
        DB[0]&=mask;

        byte k=0;
        for (int i=0;i<emlen-hlen-hlen-2;i++)
            k|=DB[i];
        if (k!=0) return false;
        if (DB[emlen-hlen-hlen-2]!=0x01) return false;

        for (int i=0;i<hlen;i++)
            SALT[i]=DB[emlen+i-hlen-hlen-1];

        byte[] MD=new byte[8+hlen+hlen];
        for (int i=0;i<8;i++)
            MD[i]=0;
        for (int i=0;i<hlen;i++)
            MD[8+i]=HMASK[i];
        for (int i=0;i<hlen;i++)
            MD[8+hlen+i]=SALT[i];
        HMASK=SPhashit(MC_SHA2,sha,MD);

        k=0;
        for (int i=0;i<hlen;i++)
            k|=(H[i]-HMASK[i]);
        if (k!=0) return false;
        return true;
    }

	/* OAEP Message Encoding for Encryption */
	public static byte[] OAEP_ENCODE(int sha,byte[] m,RAND rng,byte[] p,int RFS)
	{ 
		int i,slen,olen=RFS-1;
		int mlen=m.length;
		int hlen,seedlen;
		byte[] f=new byte[RFS];

		hlen=sha;
		byte[] SEED=new byte[hlen];
		seedlen=hlen;

		if (mlen>olen-hlen-seedlen-1) return new byte[0]; 

		byte[] DBMASK=new byte[olen-seedlen];

		//byte[] h=hashit(sha,p,-1);
        byte[] h = SPhashit(MC_SHA2,sha,p);

		for (i=0;i<hlen;i++) f[i]=h[i];

		slen=olen-mlen-hlen-seedlen-1;      

		for (i=0;i<slen;i++) f[hlen+i]=0;
		f[hlen+slen]=1;
		for (i=0;i<mlen;i++) f[hlen+slen+1+i]=m[i];

		for (i=0;i<seedlen;i++) SEED[i]=(byte)rng.getByte();

		MGF1(sha,SEED,olen-seedlen,DBMASK);

		for (i=0;i<olen-seedlen;i++) DBMASK[i]^=f[i];

		MGF1(sha,DBMASK,seedlen,f);

		for (i=0;i<seedlen;i++) f[i]^=SEED[i];

		for (i=0;i<olen-seedlen;i++) f[i+seedlen]=DBMASK[i];

		/* pad to length RFS */
		int d=1;
		for (i=RFS-1;i>=d;i--)
			f[i]=f[i-d];
		for (i=d-1;i>=0;i--)
			f[i]=0;

		return f;
	}

	/* OAEP Message Decoding for Decryption */
	public static byte[] OAEP_DECODE(int sha,byte[] p,byte[] f,int RFS)
	{
		int x,t;
		boolean comp;
		int i,k,olen=RFS-1;
		int hlen,seedlen;

		hlen=sha;
		byte[] SEED=new byte[hlen];
		seedlen=hlen;
		byte[] CHASH=new byte[hlen];
	
		if (olen<seedlen+hlen+1) return new byte[0];
		byte[] DBMASK=new byte[olen-seedlen];
		for (i=0;i<olen-seedlen;i++) DBMASK[i]=0;

		if (f.length<RFS)
		{
			int d=RFS-f.length;
			for (i=RFS-1;i>=d;i--)
				f[i]=f[i-d];
			for (i=d-1;i>=0;i--)
				f[i]=0;

		}

		//byte[] h=hashit(sha,p,-1);
        byte[] h = SPhashit(MC_SHA2,sha,p);
		for (i=0;i<hlen;i++) CHASH[i]=h[i];

		x=f[0];

		for (i=seedlen;i<olen;i++)
			DBMASK[i-seedlen]=f[i+1]; 

		MGF1(sha,DBMASK,seedlen,SEED);
		for (i=0;i<seedlen;i++) SEED[i]^=f[i+1];
		MGF1(sha,SEED,olen-seedlen,f);
		for (i=0;i<olen-seedlen;i++) DBMASK[i]^=f[i];

		comp=true;
		for (i=0;i<hlen;i++)
		{
			if (CHASH[i]!=DBMASK[i]) comp=false;
		}

		for (i=0;i<olen-seedlen-hlen;i++)
			DBMASK[i]=DBMASK[i+hlen];

		for (i=0;i<hlen;i++)
			SEED[i]=CHASH[i]=0;
		
		for (k=0;;k++)
		{
			if (k>=olen-seedlen-hlen) return new byte[0];
			if (DBMASK[k]!=0) break;
		}

		t=DBMASK[k];
		if (!comp || x!=0 || t!=0x01) 
		{
			for (i=0;i<olen-seedlen;i++) DBMASK[i]=0;
			return new byte[0];
		}

		byte[] r=new byte[olen-seedlen-hlen-k-1];

		for (i=0;i<olen-seedlen-hlen-k-1;i++)
			r[i]=DBMASK[i+k+1];
	
		for (i=0;i<olen-seedlen;i++) DBMASK[i]=0;

		return r;
	}


/*    

//java org/miracl/core/HMAC.java org/miracl/core/SHA3.java ...
//javac org/miracl/core/HMAC org/miracl/core/SHA3 ....

    public static void main(String[] args) {
        byte[] msg = "abc".getBytes();
        byte[] dst = "P256_XMD:SHA-256_SSWU_RO_TESTGEN".getBytes();
                 
        byte[] okm=HMAC.XMD_Expand(HMAC.MC_SHA2,32,48,dst,msg);
        //byte[] okm=HMAC.XOF_Expand(SHA3.SHAKE128,48,dst,msg);
        for (int i = 0; i < 48; i++) System.out.format("%02x", okm[i]);

        System.out.println("");
    }    */

}






/*
        byte[] ikm=new byte[22];
        byte[] salt=new byte[13];
        byte[] info=new byte[10];
     
        for (i=0;i<22;i++) ikm[i]=0x0b;
        for (i=0;i<13;i++) salt[i]=(byte)i;
        for (i=0;i<10;i++) info[i]=(byte)(0xf0+(byte)i);

        byte[] prk=HMAC.HKDF_Extract(HMAC.MC_SHA2,32,salt,ikm);
        System.out.printf("PRK= ");
        for (i=0;i<prk.length ;i++ )
            System.out.printf("%02x",prk[i]);

        byte[] okm=HMAC.HKDF_Expand(HMAC.MC_SHA2,32,42,prk,info);
        System.out.printf("\nOKM= ");
        for (i=0;i<okm.length ;i++ )
            System.out.printf("%02x",okm[i]);
        System.out.println();

*/


