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

public class HASH256 {
    private int[] length = new int[2];
    private int[] h = new int[8];
    private int[] w = new int[80];

    public static final int H0 = 0x6A09E667;
    public static final int H1 = 0xBB67AE85;
    public static final int H2 = 0x3C6EF372;
    public static final int H3 = 0xA54FF53A;
    public static final int H4 = 0x510E527F;
    public static final int H5 = 0x9B05688C;
    public static final int H6 = 0x1F83D9AB;
    public static final int H7 = 0x5BE0CD19;

    public static final int len = 32;

    public static final int[] K = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };


    /* functions */
    private static int S(int n, int x) {
        return (((x) >>> n) | ((x) << (32 - n)));
    }

    private static int R(int n, int x) {
        return ((x) >>> n);
    }

    private static int Ch(int x, int y, int z) {
        return ((x & y) ^ (~(x) & z));
    }

    private static int Maj(int x, int y, int z) {
        return ((x & y) ^ (x & z) ^ (y & z));
    }

    private static int Sig0(int x) {
        return (S(2, x) ^ S(13, x) ^ S(22, x));
    }

    private static int Sig1(int x) {
        return (S(6, x) ^ S(11, x) ^ S(25, x));
    }

    private static int theta0(int x) {
        return (S(7, x) ^ S(18, x) ^ R(3, x));
    }

    private static int theta1(int x) {
        return (S(17, x) ^ S(19, x) ^ R(10, x));
    }


    private void transform() {
        /* basic transformation step */
        int a, b, c, d, e, f, g, hh, t1, t2;
        int j;
        for (j = 16; j < 64; j++)
            w[j] = theta1(w[j - 2]) + w[j - 7] + theta0(w[j - 15]) + w[j - 16];
        a = h[0]; b = h[1]; c = h[2]; d = h[3];
        e = h[4]; f = h[5]; g = h[6]; hh = h[7];

        for (j = 0; j < 64; j++) {
            /* 64 times - mush it up */
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
        for (i = 0; i < 64; i++) w[i] = 0;
        length[0] = length[1] = 0;
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
    public HASH256() {
        init();
    }

    public HASH256(HASH256 hh) {
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
        cnt = (length[0] / 32) % 16;

        w[cnt] <<= 8;
        w[cnt] |= (byt & 0xFF);
        length[0] += 8;
        if (length[0] == 0) { length[1]++; length[0] = 0; }
        if ((length[0] % 512) == 0) transform();
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

    /* Generate 32-byte Hash */
    public byte[] hash() {
        /* pad message and finish - supply digest */
        int i;
        byte[] digest = new byte[32];
        int len0, len1;
        len0 = length[0];
        len1 = length[1];
        process(0x80);
        while ((length[0] % 512) != 448) process(0);
        w[14] = len1;
        w[15] = len0;
        transform();
        for (i = 0; i < len; i++) {
            /* convert to bytes */
            digest[i] = (byte)((h[i / 4] >> (8 * (3 - i % 4))) & 0xff);
        }
        init();
        return digest;
    }

    public byte[] continuing_hash() {
        HASH256 sh=new HASH256(this);
        return sh.hash();
    }

    /* test program: should produce digest */

//248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1

/*    public static void main(String[] args) {
        byte[] test = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq".getBytes();
        byte[] digest;
        int i;
        HASH256 sh = new HASH256();

        for (i = 0; i < test.length; i++)
            sh.process(test[i]);

        digest = sh.hash();
        for (i = 0; i < 32; i++) System.out.format("%02x", digest[i]);

        //	for (i=0;i<32;i++) System.out.format("%d ",digest[i]);

        System.out.println("");
    }*/
}


