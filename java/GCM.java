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
 * Implementation of the AES-GCM Encryption/Authentication
 *
 * Some restrictions..
 * 1. Only for use with AES
 * 2. Returned tag is always 128-bits. Truncate at your own risk.
 * 3. The order of function calls must follow some rules
 *
 * Typical sequence of calls..
 * 1. call GCM_init
 * 2. call GCM_add_header any number of times, as long as length of header is multiple of 16 bytes (block size)
 * 3. call GCM_add_header one last time with any length of header
 * 4. call GCM_add_cipher any number of times, as long as length of cipher/plaintext is multiple of 16 bytes
 * 5. call GCM_add_cipher one last time with any length of cipher/plaintext
 * 6. call GCM_finish to extract the tag.
 *
 * See http://www.mindspring.com/~dmcgrew/gcm-nist-6.pdf
 */

package org.miracl.core;

public class GCM {
    public static final int NB = 4;
    public static final int GCM_ACCEPTING_HEADER = 0;
    public static final int GCM_ACCEPTING_CIPHER = 1;
    public static final int GCM_NOT_ACCEPTING_MORE = 2;
    public static final int GCM_FINISHED = 3;
    public static final int GCM_ENCRYPTING = 0;
    public static final int GCM_DECRYPTING = 1;

    private int[][] table = new int[128][4]; /* 2k bytes */
    private byte[] stateX = new byte[16];
    private byte[]Y_0 = new byte[16];
    private int counter;
    private int[] lenA = new int[2];
    private int[] lenC = new int[2];
    private int status;
    private AES a = new AES();

    private static int pack(byte[] b) {
        /* pack bytes into a 32-bit Word */
        return ((((int)b[0]) & 0xff) << 24) | (((int)b[1] & 0xff) << 16) | (((int)b[2] & 0xff) << 8) | ((int)b[3] & 0xff);
    }

    private static byte[] unpack(int a) {
        /* unpack bytes from a word */
        byte [] b = new byte[4];
        b[3] = (byte)(a);
        b[2] = (byte)(a >>> 8);
        b[1] = (byte)(a >>> 16);
        b[0] = (byte)(a >>> 24);
        return b;
    }

    private void precompute(byte[] H) {
        int i, j, c;
        byte[] b = new byte[4];

        for (i = j = 0; i < NB; i++, j += 4) {
            b[0] = H[j]; b[1] = H[j + 1]; b[2] = H[j + 2]; b[3] = H[j + 3];
            table[0][i] = pack(b);
        }
        for (i = 1; i < 128; i++) {
            c = 0;
            for (j = 0; j < NB; j++) {table[i][j] = c | (table[i - 1][j]) >>> 1; c = table[i - 1][j] << 31;}
            if (c != 0) table[i][0] ^= 0xE1000000; /* irreducible polynomial */
        }
    }

    private void gf2mul() {
        /* gf2m mul - Z=H*X mod 2^128 */
        int i, j, m, k;
        int[] P = new int[4];
        int c;
        byte[] b;//=new byte[4];

        P[0] = P[1] = P[2] = P[3] = 0;
        j = 8; m = 0;
        for (i = 0; i < 128; i++) {
            c = (stateX[m] >>> (--j)) & 1; c = ~c + 1;
            for (k = 0; k < NB; k++) P[k] ^= (table[i][k] & c);
            if (j == 0) {
                j = 8; m++;
                if (m == 16) break;
            }
        }
        for (i = j = 0; i < NB; i++, j += 4) {
            b = unpack(P[i]);
            stateX[j] = b[0]; stateX[j + 1] = b[1]; stateX[j + 2] = b[2]; stateX[j + 3] = b[3];
        }
    }

    private void wrap() {
        /* Finish off GHASH */
        int i, j;
        int[] F = new int[4];
        byte[] L = new byte[16];
        byte[] b;//=new byte[4];

        /* convert lengths from bytes to bits */
        F[0] = (lenA[0] << 3) | (lenA[1] & 0xE0000000) >>> 29;
        F[1] = lenA[1] << 3;
        F[2] = (lenC[0] << 3) | (lenC[1] & 0xE0000000) >>> 29;
        F[3] = lenC[1] << 3;
        for (i = j = 0; i < NB; i++, j += 4) {
            b = unpack(F[i]);
            L[j] = b[0]; L[j + 1] = b[1]; L[j + 2] = b[2]; L[j + 3] = b[3];
        }
        for (i = 0; i < 16; i++) stateX[i] ^= L[i];
        gf2mul();
    }

    /* Initialize GCM mode */
    public void init(int nk, byte[] key, int niv, byte[] iv) {
        /* iv size niv is usually 12 bytes (96 bits). AES key size nk can be 16,24 or 32 bytes */
        int i;
        byte[] H = new byte[16];
        byte[] b;//=new byte[4];

        for (i = 0; i < 16; i++) {H[i] = 0; stateX[i] = 0;}

        a.init(AES.ECB, nk, key, iv);
        a.ecb_encrypt(H);     /* E(K,0) */
        precompute(H);

        lenA[0] = lenC[0] = lenA[1] = lenC[1] = 0;
        if (niv == 12) {
            for (i = 0; i < 12; i++) a.f[i] = iv[i];
            b = unpack((int)1);
            a.f[12] = b[0]; a.f[13] = b[1]; a.f[14] = b[2]; a.f[15] = b[3]; /* initialise IV */
            for (i = 0; i < 16; i++) Y_0[i] = a.f[i];
        } else {
            status = GCM_ACCEPTING_CIPHER;
            ghash(iv, niv); /* GHASH(H,0,IV) */
            wrap();
            for (i = 0; i < 16; i++) {a.f[i] = stateX[i]; Y_0[i] = a.f[i]; stateX[i] = 0;}
            lenA[0] = lenC[0] = lenA[1] = lenC[1] = 0;
        }
        status = GCM_ACCEPTING_HEADER;
    }

    /* Add Header data - included but not encrypted */
    public boolean add_header(byte[] header, int len) {
        /* Add some header. Won't be encrypted, but will be authenticated. len is length of header */
        int i, j = 0;
        if (status != GCM_ACCEPTING_HEADER) return false;

        while (j < len) {
            for (i = 0; i < 16 && j < len; i++) {
                stateX[i] ^= header[j++];
                lenA[1]++; if (lenA[1] == 0) lenA[0]++;
            }
            gf2mul();
        }
        if (len % 16 != 0) status = GCM_ACCEPTING_CIPHER;
        return true;
    }

    private boolean ghash(byte[] plain, int len) {
        int i, j = 0;
        int counter;
        //	byte[] B=new byte[16];
        //	byte[] b=new byte[4];

        if (status == GCM_ACCEPTING_HEADER) status = GCM_ACCEPTING_CIPHER;
        if (status != GCM_ACCEPTING_CIPHER) return false;

        while (j < len) {
            for (i = 0; i < 16 && j < len; i++) {
                stateX[i] ^= plain[j++];
                lenC[1]++; if (lenC[1] == 0) lenC[0]++;
            }
            gf2mul();
        }
        if (len % 16 != 0) status = GCM_NOT_ACCEPTING_MORE;
        return true;
    }

    /* Add Plaintext - included and encrypted */
    public byte[] add_plain(byte[] plain, int len) {
        int i, j = 0;
        int counter;
        byte[] B = new byte[16];
        byte[] b = new byte[4];
        byte[] cipher = new byte[len];

        if (status == GCM_ACCEPTING_HEADER) status = GCM_ACCEPTING_CIPHER;
        if (status != GCM_ACCEPTING_CIPHER) return new byte[0];

        while (j < len) {

            b[0] = a.f[12]; b[1] = a.f[13]; b[2] = a.f[14]; b[3] = a.f[15];
            counter = pack(b);
            counter++;
            b = unpack(counter);
            a.f[12] = b[0]; a.f[13] = b[1]; a.f[14] = b[2]; a.f[15] = b[3]; /* increment counter */
            for (i = 0; i < 16; i++) B[i] = a.f[i];
            a.ecb_encrypt(B);        /* encrypt it  */

            for (i = 0; i < 16 && j < len; i++) {
                cipher[j] = (byte)(plain[j] ^ B[i]);
                stateX[i] ^= cipher[j++];
                lenC[1]++; if (lenC[1] == 0) lenC[0]++;
            }
            gf2mul();
        }
        if (len % 16 != 0) status = GCM_NOT_ACCEPTING_MORE;
        return cipher;
    }

    /* Add Ciphertext - decrypts to plaintext */
    public byte[] add_cipher(byte[] cipher, int len) {
        int i, j = 0;
        int counter;
        byte[] B = new byte[16];
        byte[] b = new byte[4];
        byte[] plain = new byte[len];

        if (status == GCM_ACCEPTING_HEADER) status = GCM_ACCEPTING_CIPHER;
        if (status != GCM_ACCEPTING_CIPHER) return new byte[0];

        while (j < len) {

            b[0] = a.f[12]; b[1] = a.f[13]; b[2] = a.f[14]; b[3] = a.f[15];
            counter = pack(b);
            counter++;
            b = unpack(counter);
            a.f[12] = b[0]; a.f[13] = b[1]; a.f[14] = b[2]; a.f[15] = b[3]; /* increment counter */
            for (i = 0; i < 16; i++) B[i] = a.f[i];
            a.ecb_encrypt(B);        /* encrypt it  */
            for (i = 0; i < 16 && j < len; i++) {
                byte oc = cipher[j];
                plain[j] = (byte)(cipher[j] ^ B[i]);
                stateX[i] ^= oc; j++;
                lenC[1]++; if (lenC[1] == 0) lenC[0]++;
            }
            gf2mul();
        }
        if (len % 16 != 0) status = GCM_NOT_ACCEPTING_MORE;
        return plain;
    }

    /* Finish and extract Tag */
    public byte[] finish(boolean extract) {
        /* Finish off GHASH and extract tag (MAC) */
        int i;
        byte[] tag = new byte[16];

        wrap();
        /* extract tag */
        if (extract) {
            a.ecb_encrypt(Y_0);        /* E(K,Y0) */
            for (i = 0; i < 16; i++) Y_0[i] ^= stateX[i];
            for (i = 0; i < 16; i++) {tag[i] = Y_0[i]; Y_0[i] = stateX[i] = 0;}
        }
        status = GCM_FINISHED;
        a.end();
        return tag;
    }

    public static byte[] hex2bytes(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                                  + Character.digit(s.charAt(i + 1), 16));
        }
        return data;
    }

    public static void encrypt(byte[] C,byte[] T,byte[] K,byte[] IV,byte[] H,byte[] P) {
	    GCM g=new GCM();
	    g.init(K.length,K,IV.length,IV);
	    g.add_header(H,H.length);
	    byte[] b=g.add_plain(P,P.length);
        for (int i=0;i<b.length;i++) C[i]=b[i];
	    b=g.finish(true);
        for (int i=0;i<b.length;i++) T[i]=b[i];
    }

    public static void decrypt(byte[] P,byte[] T,byte[] K,byte[] IV,byte[] H,byte[] C) {
	    GCM g=new GCM();
	    g.init(K.length,K,IV.length,IV);
	    g.add_header(H,H.length);
	    byte[] b=g.add_cipher(C,C.length);
        for (int i=0;i<b.length;i++) P[i]=b[i];
	    b=g.finish(true);
	    for (int i=0;i<b.length;i++) T[i]=b[i];
    }


    /*
    	public static void main(String[] args) {
    		int i;

    		String KT="feffe9928665731c6d6a8f9467308308";
    		String MT="d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39";
    		String HT="feedfacedeadbeeffeedfacedeadbeefabaddad2";
    //	char* NT="cafebabefacedbaddecaf888";
    // Tag should be 5bc94fbc3221a5db94fae95ae7121a47
    		String NT="9313225df88406e555909c5aff5269aa6a7a9538534f7da1e4c303d2a318a728c3c0c95156809539fcf0e2429a6b525416aedbf5a0de6a57a637b39b";
    // Tag should be 619cc5aefffe0bfa462af43c1699d050


    		byte[] T=new byte[16];   // Tag
    		byte[] K=new byte[16];   // AES Key
    		byte[] H=new byte[64];   // Header - to be included in Authentication, but not encrypted
    		byte[] N=new byte[100];   // IV - Initialisation vector
    		byte[] M=new byte[100];  // Plaintext to be encrypted/authenticated
    		byte[] C=new byte[100];  // Ciphertext
    		byte[] P=new byte[100];  // Recovered Plaintext

    		GCM g=new GCM();

    		M=hex2bytes(MT);
    		H=hex2bytes(HT);
    		N=hex2bytes(NT);
    		K=hex2bytes(KT);

    		int len=M.length;
    		int lenH=H.length;
    		int lenK=K.length;
    		int lenIV=N.length;

     		System.out.format("Plaintext=\n");
    		for (i=0;i<len;i++) System.out.format("%02x",M[i]);
    		System.out.format("\n");

    		g.init(16,K,lenIV,N);
    		g.add_header(H,lenH);
    		C=g.add_plain(M,len);
    		T=g.finish(true);

    		System.out.format("Ciphertext=\n");
    		for (i=0;i<len;i++) System.out.format("%02x",C[i]);
    		System.out.format("\n");

    		System.out.format("Tag=\n");
    		for (i=0;i<16;i++) System.out.format("%02x",T[i]);
    		System.out.format("\n");

    		g.init(16,K,lenIV,N);
    		g.add_header(H,lenH);
    		P=g.add_cipher(C,len);
    		T=g.finish(true);

     		System.out.format("Plaintext=\n");
    		for (i=0;i<len;i++) System.out.format("%02x",P[i]);
    		System.out.format("\n");

    		System.out.format("Tag=\n");
    		for (i=0;i<16;i++) System.out.format("%02x",T[i]);
    		System.out.format("\n");
    	}
    */
}
