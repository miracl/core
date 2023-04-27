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

/* test driver and function exerciser for EDDSA API Functions */

package org.miracl.core.XXX;  //
import org.miracl.core.SHA3;
import org.miracl.core.HASH512;

import java.util.Scanner;
import junit.framework.TestCase;       //
import org.miracl.core.RAND;

public class TestEDDSA extends TestCase { //
    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }

    private static byte[] hexDecodeString(String s) {
        byte[] b = new byte[s.length() / 2];
        for (int i = 0; i < b.length; i++) {
            int j = i * 2;
            int v = Integer.parseInt(s.substring(j, j + 2), 16);
            b[i] = (byte) v;
        }
        return b;
    }

    private static byte[] prehashSHA2(byte[] M)  {
        HASH512 sh = new HASH512();
        for (int i=0;i<M.length;i++ )
            sh.process(M[i]);
        return sh.hash();
    }

    private  static byte[] prehashSHA3(byte[] M)  {
        byte[] digest = new byte[64];
        SHA3 sh=new SHA3(SHA3.SHAKE256);
        for (int i=0;i<M.length;i++ )
            sh.process(M[i]);
        sh.shake(digest,64);
        return digest;
    }

    public static byte[] prehashEDDSA(byte[] M)  {
        if (CONFIG_CURVE.AESKEY<=128) { // Ed25519??
            return prehashSHA2(M);
        } else {  // Ed448?
            return prehashSHA3(M);
        }
    }

    public static void testEDDSA() {
        if ("ED25519".equals("XXX") || "ED448".equals("XXX")) { // only for these Edwards curves
        //if (CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.EDWARDS) { // only for Edwards curves
            byte[] RAW = new byte[100];
            RAND rng = new RAND();
            boolean prehash=false;
            for (int i = 0; i < 100; i++) RAW[i] = (byte)(i);

            rng.clean();
            rng.seed(100, RAW);

            int bytes=CONFIG_BIG.MODBYTES;
            if (CONFIG_FIELD.MODBITS%8==0) bytes++; // add extra byte

// if it could be Ed25519 or Ed448, then use test vectors from RFC8032
            byte[] M = new byte[] {0x61,0x62,0x63};
            byte[] D = new byte[bytes];

            if (bytes==32) { // Ed25519?
                D=hexDecodeString("c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7");
                M=new byte[] {(byte)0xAF,(byte)0x82};
                rng=null;
            }
            if (bytes==57) { // Ed448?
                D=hexDecodeString("c4eab05d357007c632f3dbb48489924d552b08fe0c353a0d4a1f00acda2c463afbea67c5e8d2877c5e3bc397a659949ef8021e954e0a12274e");
                M=new byte[] {0x03};
                rng=null;
            }

            byte[] Q = new byte[bytes];
            byte[] SIG = new byte[2*bytes];

            System.out.println("\nTesting EDDSA code");

        /* Generate Key pair D/Q */
            EDDSA.KEY_PAIR_GENERATE(rng, D, Q);

            System.out.print("Secret key= 0x");
            printBinary(D);
            System.out.print("Public key= 0x");
            printBinary(Q);

            if (prehash) {
                byte[] HM = prehashEDDSA(M);
                EDDSA.SIGNATURE(true,D, null,HM, SIG);
            } else {
                EDDSA.SIGNATURE(false,D, null,M, SIG);
            }
            System.out.print("Signature= 0x"); printBinary(SIG);
 
            boolean verify;
            if (prehash) {
                byte[] HM = prehashEDDSA(M);
                verify=EDDSA.VERIFY(true,Q, null ,HM, SIG);
            } else {
                verify=EDDSA.VERIFY(false,Q, null ,M, SIG);
            }
            if (verify)
                System.out.print("Signature is verified\n");
            else
                fail("Signature is NOT verified\n");
        }
    }
}
