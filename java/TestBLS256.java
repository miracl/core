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

/* Test driver and function exerciser for Boneh-Lynn-Shacham BLS Signature API Functions */

/* To reverse the groups G1 and G2, edit BLS*.go

Swap G1 <-> G2
Swap ECP <-> ECPn
Disable G2 precomputation
Switch G1/G2 parameter order in pairing function calls

Swap G1S and G2S in this program

See CPP library version for example

*/

package org.miracl.core.XXX;  //

import junit.framework.TestCase;      //
import org.miracl.core.RAND;

public class TestBLS256 extends TestCase { //
    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }

    public static void testBLS256() {
        RAND rng = new RAND();
        int BGS = BLS256.BGS;
        int BFS = BLS256.BFS;
        int G1S = BFS + 1; /* Group 1 Size compressed */
        int G2S = 8 * BFS + 1; /* Group 2 Size compressed */

        byte[] S = new byte[BGS];
        byte[] W = new byte[G2S];
        byte[] SIG = new byte[G1S];
        byte[] RAW = new byte[100];
        byte[] IKM = new byte[64];

        rng.clean();
        for (int i = 0; i < 100; i++) RAW[i] = (byte)(i);
        rng.seed(100, RAW);

        for (int i=0;i<IKM.length;i++)
            //IKM[i]=(byte)(i+1);
            IKM[i]=(byte)rng.getByte();

        System.out.println("\nTesting BLS code");

        int res = BLS256.init();
        if (res != 0)
            fail("Failed to initialize");

        String mess = new String("This is a test message");

        res = BLS256.KeyPairGenerate(IKM, S, W);
        if (res != 0)
            fail("Failed to Generate Keys");
        System.out.print("Private key : 0x");  printBinary(S);
        System.out.print("Public  key : 0x");  printBinary(W);

        BLS256.core_sign(SIG, mess.getBytes(), S);
        System.out.print("Signature : 0x");  printBinary(SIG);

        res = BLS256.core_verify(SIG, mess.getBytes(), W);

        if (res == 0)
            System.out.println("Signature is OK");
        else
            fail("Signature is *NOT* OK");

    }
}
