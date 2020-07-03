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
package org.miracl.core.XXX;  //

import java.util.Scanner;
import junit.framework.TestCase;      //
import org.miracl.core.RAND;
import org.miracl.core.SHARE;

public class TestMPIN extends TestCase { //
    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }

    public static void testMPIN() {
        RAND rng = new RAND();
        int EGS = MPIN.EGS;
        int EFS = MPIN.EFS;
        int G1S = 2 * EFS + 1; /* Group 1 Size */
        int G2S = 4 * EFS + 1; /* Group 2 Size */

        byte[] S = new byte[EGS];
        byte[] SST = new byte[G2S];
        byte[] TOKEN = new byte[G1S];
        byte[] SEC = new byte[G1S];
        byte[] U = new byte[G1S];
        byte[] X = new byte[EGS];
        byte[] Y = new byte[EGS];
        byte[] HCID = new byte[G1S];
        byte[] HSID = new byte[G1S];
        byte[] RAW = new byte[100];

        String dst = "XXXG1_XMD:SHA-256_SVDW_NU_MPIN";
        byte[] DST=dst.getBytes();

        rng.clean();
        for (int i = 0; i < 100; i++) RAW[i] = (byte)(i);
        rng.seed(100, RAW);

        System.out.println("\nTesting MPIN 2-factor authentication protocol on curve XXX");

// Trusted Authority (TA) set-up 
        MPIN.RANDOM_GENERATE(rng, S);
        System.out.print("Master Secret s: 0x");  printBinary(S);

// Create Client Identity 
            String IDstr = "testUser@miracl.com";
            byte[] CLIENT_ID = IDstr.getBytes();
            MPIN.ENCODE_TO_CURVE(DST,CLIENT_ID,HCID);
            System.out.print("Client ID Hashed to Curve= "); printBinary(HCID);

// Client approaches Trusted Authority and is issued secret

        MPIN.GET_CLIENT_SECRET(S, HCID, TOKEN);
        System.out.print("Client Secret CS: 0x");
        printBinary(TOKEN);
// TA sends Client secret to Client

// Server is issued secret by TA
        MPIN.GET_SERVER_SECRET(S, SST);
//       System.out.print("Server Secret SS: 0x");  printBinary(SST);


// Client extracts PIN from secret to create Token 
            int pin = 1234;
            System.out.println("Client extracts PIN= " + pin);
            int rtn = MPIN.EXTRACT_PIN(HCID, pin, TOKEN);
            if (rtn != 0)
                fail("FAILURE: EXTRACT_PIN rtn: " + rtn);
            System.out.print("Client Token TK: 0x"); printBinary(TOKEN);

// Exercise Secret Sharing
            byte[] R=new byte[128];
            for (int i=0;i<128;i++)
                R[i]=(byte)rng.getByte();
       // create 4 unique shares of TOKEN
            SHARE Sh1=new SHARE(1,3,TOKEN,R);  // indicate 3 shares required for recovery
            SHARE Sh2=new SHARE(2,3,TOKEN,R);
            SHARE Sh3=new SHARE(3,3,TOKEN,R);
            SHARE Sh4=new SHARE(4,3,TOKEN,R);

            SHARE[] Shares=new SHARE[3];

            Shares[0]=Sh1;  // any 3 shares to recover TOKEN
            Shares[1]=Sh2;
            Shares[2]=Sh4;
       
            TOKEN=SHARE.recover(Shares);  // recover token

// MPin Protocol

// Client enters ID and PIN
//		System.out.print("\nPIN= ");
//		Scanner scan=new Scanner(System.in);
//		pin=scan.nextInt();

            pin = 1234;
         
// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
            rtn = MPIN.CLIENT_1(HCID, rng, X, pin, TOKEN, SEC, U);
            if (rtn != 0)
                fail("FAILURE: CLIENT_1 rtn: " + rtn);

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                    MPIN.ENCODE_TO_CURVE(DST,CLIENT_ID,HSID);

// Server generates Random number Y and sends it to Client
                    MPIN.RANDOM_GENERATE(rng, Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC 
            rtn = MPIN.CLIENT_2(X, Y, SEC);
            if (rtn != 0)
                fail("FAILURE: CLIENT_2 rtn: " + rtn);

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 
         
                    rtn = MPIN.SERVER(HSID, Y, SST, U, SEC);
                    if (rtn != 0)
                    {
                        if (rtn == MPIN.BAD_PIN) 
                            fail("Server says - Bad Pin. I don't know you. Feck off\n");
                        else
                            fail("FAILURE: SERVER_2 rtn: " + rtn);
                    } else { 
                        System.out.println("Server says - PIN is good! You really are " + IDstr + "\n");
                    }
    }
}
