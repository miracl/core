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
 * Implementation of the Secure Hashing Algorithm (SHA-512)
 *
 * Generates a 512 bit message digest. It should be impossible to come
 * come up with two messages that hash to the same value ("collision free").
 *
 * For use with byte-oriented messages only.
 */


package org.miracl.core;

public class HASH512 {

    private long[] length = new long[2];
    private long[] h = new long[8];
    private long[] w = new long[80];

    public static final long H0 = 0x6a09e667f3bcc908L;
    public static final long H1 = 0xbb67ae8584caa73bL;
    public static final long H2 = 0x3c6ef372fe94f82bL;
    public static final long H3 = 0xa54ff53a5f1d36f1L;
    public static final long H4 = 0x510e527fade682d1L;
    public static final long H5 = 0x9b05688c2b3e6c1fL;
    public static final long H6 = 0x1f83d9abfb41bd6bL;
    public static final long H7 = 0x5be0cd19137e2179L;

    public static final int len = 64;

    public static final long[] K = {
        0x428a2f98d728ae22L, 0x7137449123ef65cdL, 0xb5c0fbcfec4d3b2fL, 0xe9b5dba58189dbbcL,
        0x3956c25bf348b538L, 0x59f111f1b605d019L, 0x923f82a4af194f9bL, 0xab1c5ed5da6d8118L,
        0xd807aa98a3030242L, 0x12835b0145706fbeL, 0x243185be4ee4b28cL, 0x550c7dc3d5ffb4e2L,
        0x72be5d74f27b896fL, 0x80deb1fe3b1696b1L, 0x9bdc06a725c71235L, 0xc19bf174cf692694L,
        0xe49b69c19ef14ad2L, 0xefbe4786384f25e3L, 0x0fc19dc68b8cd5b5L, 0x240ca1cc77ac9c65L,
        0x2de92c6f592b0275L, 0x4a7484aa6ea6e483L, 0x5cb0a9dcbd41fbd4L, 0x76f988da831153b5L,
        0x983e5152ee66dfabL, 0xa831c66d2db43210L, 0xb00327c898fb213fL, 0xbf597fc7beef0ee4L,
        0xc6e00bf33da88fc2L, 0xd5a79147930aa725L, 0x06ca6351e003826fL, 0x142929670a0e6e70L,
        0x27b70a8546d22ffcL, 0x2e1b21385c26c926L, 0x4d2c6dfc5ac42aedL, 0x53380d139d95b3dfL,
        0x650a73548baf63deL, 0x766a0abb3c77b2a8L, 0x81c2c92e47edaee6L, 0x92722c851482353bL,
        0xa2bfe8a14cf10364L, 0xa81a664bbc423001L, 0xc24b8b70d0f89791L, 0xc76c51a30654be30L,
        0xd192e819d6ef5218L, 0xd69906245565a910L, 0xf40e35855771202aL, 0x106aa07032bbd1b8L,
        0x19a4c116b8d2d0c8L, 0x1e376c085141ab53L, 0x2748774cdf8eeb99L, 0x34b0bcb5e19b48a8L,
        0x391c0cb3c5c95a63L, 0x4ed8aa4ae3418acbL, 0x5b9cca4f7763e373L, 0x682e6ff3d6b2b8a3L,
        0x748f82ee5defb2fcL, 0x78a5636f43172f60L, 0x84c87814a1f0ab72L, 0x8cc702081a6439ecL,
        0x90befffa23631e28L, 0xa4506cebde82bde9L, 0xbef9a3f7b2c67915L, 0xc67178f2e372532bL,
        0xca273eceea26619cL, 0xd186b8c721c0c207L, 0xeada7dd6cde0eb1eL, 0xf57d4f7fee6ed178L,
        0x06f067aa72176fbaL, 0x0a637dc5a2c898a6L, 0x113f9804bef90daeL, 0x1b710b35131c471bL,
        0x28db77f523047d84L, 0x32caab7b40c72493L, 0x3c9ebe0a15c9bebcL, 0x431d67c49c100d4cL,
        0x4cc5d4becb3e42b6L, 0x597f299cfc657e2aL, 0x5fcb6fab3ad6faecL, 0x6c44198c4a475817L
    };

    /* functions */
    private static long S(int n, long x) {
        return (((x) >>> n) | ((x) << (64 - n)));
    }

    private static long R(int n, long x) {
        return ((x) >>> n);
    }

    private static long Ch(long x, long y, long z) {
        return ((x & y) ^ (~(x) & z));
    }

    private static long Maj(long x, long y, long z) {
        return ((x & y) ^ (x & z) ^ (y & z));
    }

    private static long Sig0(long x) {
        return (S(28, x) ^ S(34, x) ^ S(39, x));
    }

    private static long Sig1(long x) {
        return (S(14, x) ^ S(18, x) ^ S(41, x));
    }

    private static long theta0(long x) {
        return (S(1, x) ^ S(8, x) ^ R(7, x));
    }

    private static long theta1(long x) {
        return (S(19, x) ^ S(61, x) ^ R(6, x));
    }


    private void transform() {
        /* basic transformation step */
        long a, b, c, d, e, f, g, hh, t1, t2;
        int j;
        for (j = 16; j < 80; j++)
            w[j] = theta1(w[j - 2]) + w[j - 7] + theta0(w[j - 15]) + w[j - 16];
        a = h[0]; b = h[1]; c = h[2]; d = h[3];
        e = h[4]; f = h[5]; g = h[6]; hh = h[7];

        for (j = 0; j < 80; j++) {
            /* 80 times - mush it up */
            t1 = hh + Sig1(e) + Ch(e, f, g) + K[j] + w[j];
            t2 = Sig0(a) + Maj(a, b, c);
            hh = g; g = f; f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;

        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }

    /* Initialise Hash function */
    public void init() {
        /* initialise */
        int i;
        for (i = 0; i < 80; i++) w[i] = 0L;
        length[0] = length[1] = 0L;
        h[0] = H0;
        h[1] = H1;
        h[2] = H2;
        h[3] = H3;
        h[4] = H4;
        h[5] = H5;
        h[6] = H6;
        h[7] = H7;
    }

    /* Constructors */
    public HASH512() {
        init();
    }

    public HASH512(HASH512 hh) {
        int i;
        for (i = 0; i < 80; i++) w[i] = hh.w[i];
        length[0] = hh.length[0]; 
        length[1] = hh.length[1];
        for (i=0;i<8;i++) h[i]=hh.h[i];
    }

    /* process a single byte */
    public void process(int byt) {
        /* process the next message byte */
        int cnt;
        cnt = (int)(length[0] / 64) % 16;

        w[cnt] <<= 8;
        w[cnt] |= (byt & 0xFF);
        length[0] += 8;
        if (length[0] == 0L) { length[1]++; length[0] = 0L; }
        if ((length[0] % 1024) == 0) transform();
    }

    /* process an array of bytes */
    public void process_array(byte[] b) {
        for (int i = 0; i < b.length; i++) process((int)b[i]);
    }

    /* process a 32-bit integer */
    public void process_num(int n) {
        process((n >> 24) & 0xff);
        process((n >> 16) & 0xff);
        process((n >> 8) & 0xff);
        process(n & 0xff);
    }

    /* Generate 64-byte Hash */
    public byte[] hash() {
        /* pad message and finish - supply digest */
        int i;
        byte[] digest = new byte[64];
        long len0, len1;
        len0 = length[0];
        len1 = length[1];
        process(0x80);
        while ((length[0] % 1024) != 896) process(0);
        w[14] = len1;
        w[15] = len0;
        transform();
        for (i = 0; i < len; i++) {
            /* convert to bytes */
            digest[i] = (byte)((h[i / 8] >> (8 * (7 - i % 8))) & 0xffL);
        }
        init();
        return digest;
    }

    public byte[] continuing_hash() {
        HASH512 sh=new HASH512(this);
        return sh.hash();
    }

    /* test program: should produce digest */

//8e959b75dae313da 8cf4f72814fc143f 8f7779c6eb9f7fa1 7299aeadb6889018 501d289e4900f7e4 331b99dec4b5433a c7d329eeb6dd2654 5e96e55b874be909

   /* public static void main(String[] args) {

        byte[] test = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu".getBytes();
        byte[] digest;
        int i;
        HASH512 sh = new HASH512();

        for (i = 0; i < test.length; i++)
            sh.process(test[i]);

        digest = sh.hash();
        for (i = 0; i < 64; i++) System.out.format("%02x", digest[i]);

        //	for (i=0;i<32;i++) System.out.format("%d ",digest[i]);

        System.out.println("");

    } */
}

