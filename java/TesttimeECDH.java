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

/* test driver and function exerciser for ECDH/ECIES/ECDSA API Functions */
package org.miracl.core.XXX;

import java.util.Scanner;
import junit.framework.TestCase;
import org.miracl.core.RAND;

public class TesttimeECDH extends TestCase {
    public static final int MIN_TIME = 10; /* seconds */
    public static final int MIN_ITERS = 10;

    public static void testtimeECDH() {
        byte[] RAW = new byte[100];
        RAND rng = new RAND();
        int i, iterations;
        long start, elapsed;
        double dur;

        rng.clean();
        for (i = 0; i < 100; i++) RAW[i] = (byte)(i);
        rng.seed(100, RAW);

        System.out.println("\nTesting/Timing ECC");
        if (CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.WEIERSTRASS) {
            System.out.print("Weierstrass parameterization\n");
        }
        if (CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.EDWARDS) {
            System.out.print("Edwards parameterization\n");
        }
        if (CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY) {
            System.out.print("Montgomery parameterization\n");
        }

        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.PSEUDO_MERSENNE) {
            System.out.print("Pseudo-Mersenne Modulus\n");
        }
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.MONTGOMERY_FRIENDLY) {
            System.out.print("Montgomery friendly Modulus\n");
        }
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.GENERALISED_MERSENNE) {
            System.out.print("Generalised-Mersenne Modulus\n");
        }
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.NOT_SPECIAL) {
            System.out.print("Not special Modulus\n");
        }

        System.out.format("Modulus size %d bits\n", CONFIG_FIELD.MODBITS);
        System.out.format("%d bit build\n", BIG.CHUNK);
        BIG r, s, wx, wy;
        ECP G, WP;

        G = ECP.generator();

        r = new BIG(ROM.CURVE_Order);
        s = BIG.randtrunc(r, 16 * CONFIG_CURVE.AESKEY, rng);

        WP = ECP.map2point(new FP(rng));
//        System.out.print("WP= "+WP.toString()+"\n");
        WP.cfp();
        if (WP.is_infinity()) {
            fail("HASHING FAILURE - P=O");
        }
        WP=WP.mul(r);
        if (!WP.is_infinity()) {
//        System.out.print("WP= "+WP.toString()+"\n");
            fail("HASHING FAILURE - P=O");

        }

        WP = G.mul(r);
        if (!WP.is_infinity()) {
            fail("FAILURE - rG!=O");
        }

        start = System.currentTimeMillis();
        iterations = 0;
        do {
            WP = G.mul(s);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("EC  mul - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

    }
}
