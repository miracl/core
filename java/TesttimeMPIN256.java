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

/* test driver and function exerciser for MPIN API Functions */
package org.miracl.core.XXX;

import java.util.Scanner;
import junit.framework.TestCase;
import org.miracl.core.RAND;

public class TesttimeMPIN256 extends TestCase {
    public static final int MIN_TIME = 10; /* seconds */
    public static final int MIN_ITERS = 10;

    public static void testtimeMPIN() {
        byte[] RAW = new byte[100];
        RAND rng = new RAND();

        int i, iterations;
        long start, elapsed;
        double dur;

        rng.clean();
        for (i = 0; i < 100; i++) RAW[i] = (byte)(i);
        rng.seed(100, RAW);


        System.out.println("\nTesting/Timing Pairings");
        if (CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN) {
            System.out.print("BN Pairing-Friendly Curve\n");
        }
        if (CONFIG_CURVE.CURVE_PAIRING_TYPE > CONFIG_CURVE.BN) {
            System.out.print("BLS48 Pairing-Friendly Curve\n");
        }

        System.out.format("Modulus size %d bits\n", CONFIG_FIELD.MODBITS);
        System.out.format("%d bit build\n", BIG.CHUNK);

        ECP G = ECP.generator();

        BIG r = new BIG(ROM.CURVE_Order);
        BIG s = BIG.randtrunc(r, 16 * CONFIG_CURVE.AESKEY, rng);

        ECP P = PAIR8.G1mul(G, r);

        if (!P.is_infinity()) {
            fail("FAILURE - rP!=O");
        }

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            P = PAIR8.G1mul(G, s);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("G1 mul              - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        ECP8 Q = ECP8.generator();

        ECP8 W = ECP8.map2point(new FP8(rng));
        W.cfp();
        if (W.is_infinity()) {
            fail("HASHING FAILURE - P=O");
        }
        W=W.mul(r);
        if (!W.is_infinity()) {
            fail("HASHING FAILURE - P=O");
        } 

        W = PAIR8.G2mul(W, r);
        if (!W.is_infinity()) {
            fail("FAILURE - rQ!=O");
        }

        W = PAIR8.G2mul(Q, r);

        if (!W.is_infinity()) {
            fail("FAILURE - rQ!=O");
        }

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            W = PAIR8.G2mul(Q, s);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("G2 mul              - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        FP48 w = PAIR8.ate(Q, P);
        w = PAIR8.fexp(w);

        FP48 g = PAIR8.GTpow(w, r);

        if (!g.isunity()) {
            fail("FAILURE - g^r!=1");
        }

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            g = PAIR8.GTpow(w, s);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("GT pow              - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            w = PAIR8.ate(Q, P);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("PAIRing ATE         - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            g = PAIR8.fexp(w);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("PAIRing FEXP        - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        P.copy(G);
        Q.copy(W);

        P = PAIR8.G1mul(P, s);

        g = PAIR8.ate(Q, P);
        g = PAIR8.fexp(g);

        P.copy(G);
        Q = PAIR8.G2mul(Q, s);

        w = PAIR8.ate(Q, P);
        w = PAIR8.fexp(w);

        if (!PAIR8.G1member(P))
        {
            fail("FAILURE - P not in G1 ");
        }

        if (!PAIR8.G2member(Q))
        {
            fail("FAILURE - Q not in G2 ");
        }

        if (!PAIR8.GTmember(w))
        {
            fail("FAILURE - e(Q,P) not in GT ");
        }

        if (!g.equals(w)) {
            fail("FAILURE - e(sQ,p)!=e(Q,sP) ");
        }

        Q.copy(W);
        g = PAIR8.ate(Q, P);
        g = PAIR8.fexp(g);
        g = PAIR8.GTpow(g, s);

        if (!g.equals(w)) {
            fail("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        }

        System.out.print("All tests pass\n");
    }

}
