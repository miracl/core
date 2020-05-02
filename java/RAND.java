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
 *   Cryptographic strong random number generator
 *
 *   Unguessable seed -> SHA -> PRNG internal state -> SHA -> random numbers
 *   Slow - but secure
 *
 *   See ftp://ftp.rsasecurity.com/pub/pdfs/bull-1.pdf for a justification
 */

/* Marsaglia & Zaman Random number generator constants */


package org.miracl.core;

public class RAND {
    /* Cryptographically strong pseudo-random number generator */

    private static final int NK = 21;
    private static final int NJ = 6;
    private static final int NV = 8;
    private int[] ira = new int[NK]; /* random number...   */
    private int rndptr;   /* ...array & pointer */
    private int borrow;
    private int pool_ptr;
    private byte[] pool = new byte[32];  /* random pool */

    public RAND() {
        clean();
    }

    private int sbrand() {
        /* Marsaglia & Zaman random number generator */
        int i, k;
        long pdiff, t;

        rndptr++;
        if (rndptr < NK) return ira[rndptr];
        rndptr = 0;
        for (i = 0, k = NK - NJ; i < NK; i++, k++) {
            /* calculate next NK values */
            if (k == NK) k = 0;
            t = ((long)ira[k]) & 0xffffffffL;
            pdiff = (t - (((long)ira[i]) & 0xffffffffL) - (long)borrow) & 0xffffffffL;
            if (pdiff < t) borrow = 0;
            if (pdiff > t) borrow = 1;
            ira[i] = (int)(pdiff & 0xffffffffL);
        }

        return ira[0];
    }

    public void sirand(int seed) {
        int i, in;
        int t, m = 1;
        borrow = 0;
        rndptr = 0;
        ira[0] ^= seed;
        for (i = 1; i < NK; i++) {
            /* fill initialisation vector */
            in = (NV * i) % NK;
            ira[in] ^= m;    /* note XOR */
            t = m;
            m = seed - m;
            seed = t;
        }
        for (i = 0; i < 10000; i++) sbrand(); /* "warm-up" & stir the generator */
    }

    private void fill_pool() {
        HASH256 sh = new HASH256();
        for (int i = 0; i < 128; i++) sh.process(sbrand());
        pool = sh.hash();
        pool_ptr = 0;
    }

    private static int pack(byte[] b) {
        /* pack 4 bytes into a 32-bit Word */
        return ((((int)b[3]) & 0xff) << 24) | (((int)b[2] & 0xff) << 16) | (((int)b[1] & 0xff) << 8) | ((int)b[0] & 0xff);
    }

    /* Initialize RNG with some real entropy from some external source */
    public void seed(int rawlen, byte[] raw) {
        /* initialise from at least 128 byte string of raw random entropy */
        int i;
        byte [] digest;
        byte [] b = new byte[4];
        HASH256 sh = new HASH256();
        pool_ptr = 0;
        for (i = 0; i < NK; i++) ira[i] = 0;
        if (rawlen > 0) {
            for (i = 0; i < rawlen; i++)
                sh.process(raw[i]);
            digest = sh.hash();

            /* initialise PRNG from distilled randomness */

            for (i = 0; i < 8; i++) {
                b[0] = digest[4 * i]; b[1] = digest[4 * i + 1]; b[2] = digest[4 * i + 2]; b[3] = digest[4 * i + 3];
                sirand(pack(b));
            }
        }
        fill_pool();
    }

    /* Terminate and clean up */
    public void clean() {
        /* kill internal state */
        int i;
        pool_ptr = rndptr = 0;
        for (i = 0; i < 32; i++) pool[i] = 0;
        for (i = 0; i < NK; i++) ira[i] = 0;
        borrow = 0;
    }

    /* get random byte */
    public int getByte() {
        int r;
        r = pool[pool_ptr++];
        if (pool_ptr >= 32) fill_pool();
        return (r & 0xff);
    }

    /* test main program */
    /*
    	public static void main(String[] args) {
    		int i;
    		byte[] raw=new byte[100];
    		RAND rng=new RAND();

    		rng.clean();
    		for (i=0;i<100;i++) raw[i]=(byte)i;

    		rng.seed(100,raw);

    		for (i=0;i<1000;i++)
    			System.out.format("%03d ",rng.getByte());
    	} */
}
