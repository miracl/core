/*
 * Copyright (c) 2012-2021 MIRACL UK Ltd.
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

/* test driver and function exerciser for Dilithium functions */

// See https://eprint.iacr.org/2017/633 (Ducas, Lepoint, Lyubashevsky, Schwabe, Seiler, Stehle)

package org.miracl.core;

import java.util.Scanner;
import junit.framework.TestCase;       //
import org.miracl.core.RAND;
import org.miracl.core.DILITHIUM;

public class TestDLTHM extends TestCase {
    public static void testDLTHM() {
        int i,attempts,LOOPS=100;
        boolean result;
        byte[] RAW = new byte[100];
        byte[] TAU = new byte[32];
		byte[] SK= new byte[DILITHIUM.SK_SIZE_3];
		byte[] PK= new byte[DILITHIUM.PK_SIZE_3];
		byte[] SIG=new byte[DILITHIUM.SIG_SIZE_3];
		//byte[] CT=new byte[KYBER.CIPHERTEXT_SIZE_768];
        //byte[] M = new byte[128];
    
        RAND RNG = new RAND();
      
        for (i = 0; i < 100; i++) RAW[i] = (byte)(i + 1);
        RNG.seed(100, RAW);

        System.out.println("\nTesting Dilithium signature");

		int tats=0;
        for (int j=0;j<LOOPS;j++) {
			String mess=new String("Hello World");
			for (i = 0; i < 32; i++) TAU[i] = (byte)RNG.getByte(); //(byte)(i + 1);
			
            DILITHIUM.keypair_3(TAU,SK,PK);
/*
            System.out.printf("SK= 0x");
            for (i = 0; i < SK.length; i++)
                System.out.printf("%02x", SK[i]);
            System.out.println();

            System.out.printf("PK= 0x");
            for (i = 0; i < PK.length; i++)
                System.out.printf("%02x", PK[i]);
            System.out.println();
*/
            attempts=DILITHIUM.signature_3(false,null,SK,null,mess.getBytes(),SIG);
            tats+=attempts;
/*
            System.out.printf("Signature = 0x");
            for (i = 0; i < SIG.length; i++)
                System.out.printf("%02x", SIG[i]);
            System.out.println();
*/
            System.out.printf("Signature %d bits created on attempt %d\n",8*SIG.length,attempts);

            result=DILITHIUM.verify_3(false,PK,null,mess.getBytes(),SIG);
            if (result) {
                System.out.printf("Signature is verified\n");
            } else {
                System.out.printf("Signature is NOT verified\n"); 
                break;
            }
		}
        if (LOOPS>1)
            System.out.printf("Average= %d\n",tats/LOOPS);
	}
}
