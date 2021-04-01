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
import org.miracl.core.HMAC;

public class TestRSA extends TestCase {
    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }

    public static void testRSA() {
        byte[] RAW = new byte[100];
        RAND rng = new RAND();

        rng.clean();
        for (int i = 0; i < 100; i++) RAW[i] = (byte)(i);

        rng.seed(100, RAW);

        int i;
        int RFS = RSA.RFS;
        int sha = RSA.HASH_TYPE;

        String message = "Hello World\n";

        public_key pub = new public_key(CONFIG_FF.FFLEN);
        private_key priv = new private_key(FF.HFLEN);

        byte[] ML = new byte[RFS];
        byte[] C = new byte[RFS];
        byte[] S = new byte[RFS];


        System.out.println("Testing RSA code");

        System.out.println("Generating public/private key pair");
        RSA.KEY_PAIR(rng, 65537, priv, pub);

        byte[] M = message.getBytes();
        System.out.print("Encrypting test string\n");


        byte[] E = HMAC.OAEP_ENCODE(sha, M, rng, null, RFS); /* OAEP encode message M to E  */

        if (E.length == 0) {
            fail("Encoding failed");
        }

        RSA.ENCRYPT(pub, E, C);   /* encrypt encoded message */
        System.out.print("Ciphertext= 0x"); printBinary(C);

        System.out.print("Decrypting test string\n");
        RSA.DECRYPT(priv, C, ML);
        byte[] MS = HMAC.OAEP_DECODE(sha, null, ML, RFS); /* OAEP decode message  */

        message = new String(MS);
        System.out.print(message);

        //System.out.print("M= 0x"); printBinary(M);
        byte [] T=HMAC.PSS_ENCODE(sha,M,rng,RFS);
        //System.out.print("T= 0x"); printBinary(T);
        if (HMAC.PSS_VERIFY(sha,M,T))
            System.out.println("PSS Encoding OK");
        else
            fail("PSS Encoding FAILED");


// Signature
        System.out.println("Signing message");
        HMAC.PKCS15(sha, M, C, RFS);

        RSA.DECRYPT(priv, C, S); /* create signature in S */

        System.out.print("Signature= 0x"); printBinary(S);

// Verificaction
        boolean valid=false;
        RSA.ENCRYPT(pub, S, ML);
        HMAC.PKCS15(sha, M, C, RFS);
    
        boolean cmp = true;
        for (int j = 0; j < RFS; j++)
            if (C[j] != ML[j]) cmp = false;
        if (cmp)  {
            valid=true;
        } else {
            HMAC.PKCS15b(sha, M, C, RFS);  
            cmp=true;
            for (int j = 0; j < RFS; j++)
                if (C[j] != ML[j]) cmp = false;
            if (cmp) valid=true;
        }

        if (valid) System.out.println("Signature is valid");
        else fail("Signature is INVALID");

        RSA.PRIVATE_KEY_KILL(priv);
        System.out.println("");

    }
}
