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

package org.miracl.core.XXX;  //

import java.util.Scanner;
import junit.framework.TestCase;       //
import org.miracl.core.RAND;
import org.miracl.core.AES;
import org.miracl.core.HMAC;

public class TestECDH extends TestCase { //
    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }

    public static void testECDH() {
        byte[] RAW = new byte[100];
        RAND rng = new RAND();
        int i, j = 0, res;
        int result;
        String pp = new String("M0ng00se");

        rng.clean();
        for (i = 0; i < 100; i++) RAW[i] = (byte)(i);
        rng.seed(100, RAW);

        int EGS = ECDH.EGS;
        int EFS = ECDH.EFS;
        int EAS = CONFIG_CURVE.AESKEY;
        int sha = CONFIG_CURVE.HASH_TYPE;

        byte[] S1 = new byte[EGS];
        byte[] W0;
        byte[] W1;
        if (CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY) {
            W0 = new byte[2 * EFS + 1];
            W1 = new byte[2 * EFS + 1];
        } else {
            W0 = new byte[EFS];
            W1 = new byte[EFS];
        }

        byte[] Z0 = new byte[EFS];
        byte[] Z1 = new byte[EFS];

        byte[] SALT = new byte[8];
        byte[] P1 = new byte[3];
        byte[] P2 = new byte[4];
        byte[] V = new byte[2 * EFS + 1];
        byte[] M = new byte[17];
        byte[] T = new byte[12];
        byte[] CS = new byte[EGS];
        byte[] DS = new byte[EGS];

        for (i = 0; i < 8; i++) SALT[i] = (byte)(i + 1); // set Salt

        System.out.println("\nTesting ECDH code");
        System.out.println("Alice's Passphrase= " + pp);
        byte[] PW = pp.getBytes();

        /* private key S0 of size EGS bytes derived from Password and Salt */

        byte[] S0 = HMAC.PBKDF2(HMAC.MC_SHA2, sha, PW, SALT, 1000, EGS);

        System.out.print("Alice's private key= 0x");
        printBinary(S0);

        /* Generate Key pair S/W */
        ECDH.KEY_PAIR_GENERATE(null, S0, W0);

        System.out.print("Alice's public key= 0x");
        printBinary(W0);

        res = ECDH.PUBLIC_KEY_VALIDATE(W0);
        if (res != 0) {
            fail("ECP Public Key is invalid!");
        }
        /* Random private key for other party */
        ECDH.KEY_PAIR_GENERATE(rng, S1, W1);

        System.out.print("Servers private key= 0x");
        printBinary(S1);

        System.out.print("Servers public key= 0x");
        printBinary(W1);


        res = ECDH.PUBLIC_KEY_VALIDATE(W1);
        if (res != 0) {
            fail("ECP Public Key is invalid!");
        }

        /* Calculate common key using DH - IEEE 1363 method */

        ECDH.SVDP_DH(S0, W1, Z0, 0);
        ECDH.SVDP_DH(S1, W0, Z1, 0);

        boolean same = true;
        for (i = 0; i < EFS; i++)
            if (Z0[i] != Z1[i]) same = false;

        if (!same) {
            fail("*** ECPSVDP-DH Failed");
        }

        byte[] KEY = HMAC.KDF2(HMAC.MC_SHA2, sha, Z0, null, EAS);

        System.out.print("Alice's DH Key=  0x"); printBinary(KEY);
        System.out.print("Servers DH Key=  0x"); printBinary(KEY);

        if (CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY) {
            System.out.println("Testing ECIES");

            P1[0] = 0x0; P1[1] = 0x1; P1[2] = 0x2;
            P2[0] = 0x0; P2[1] = 0x1; P2[2] = 0x2; P2[3] = 0x3;

            for (i = 0; i <= 16; i++) M[i] = (byte)i;

            byte[] C = ECDH.ECIES_ENCRYPT(sha, P1, P2, rng, W1, M, V, T);

            System.out.println("Ciphertext= ");
            System.out.print("V= 0x"); printBinary(V);
            System.out.print("C= 0x"); printBinary(C);
            System.out.print("T= 0x"); printBinary(T);


            M = ECDH.ECIES_DECRYPT(sha, P1, P2, V, C, T, S1);
            if (M.length == 0) {
                fail("*** ECIES Decryption Failed");
            } else System.out.println("Decryption succeeded");

            System.out.print("Message is 0x"); printBinary(M);

            System.out.println("Testing ECDSA");

            if (ECDH.SP_DSA(sha, rng, S0, M, CS, DS) != 0) {
                fail("***ECDSA Signature Failed");
            }
            System.out.println("Signature= ");
            System.out.print("C= 0x"); printBinary(CS);
            System.out.print("D= 0x"); printBinary(DS);

            if (ECDH.VP_DSA(sha, W0, M, CS, DS) != 0) {
                fail("***ECDSA Verification Failed");
            } else System.out.println("ECDSA Signature/Verification succeeded " + j);
            System.out.println("");

        }
    }

}
