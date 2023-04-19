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

/* Run through some test vectors for hash-to-curve draft standard */

package org.miracl.core.XXX;  //

import junit.framework.TestCase;       //
import org.miracl.core.HMAC;

public class TestHTP extends TestCase { 

    private static int ceil(int a,int b) {
        return (((a)-1)/(b)+1);
    }

    private static FP[] hash_to_field(int hash,int hlen,byte[] DST,byte[] M,int ctr) {
		FP[] u=new FP[2];
        BIG q=new BIG(ROM.Modulus);
        int k=q.nbits();
        BIG r=new BIG(ROM.CURVE_Order);
        int m=r.nbits();
        int L=ceil(k+ceil(m,2),8);
        byte[] OKM = HMAC.XMD_Expand(hash,hlen,L*ctr,DST,M);
        byte[] fd = new byte[L]; 
        for (int i=0;i<ctr;i++)
        {
            for (int j=0;j<L;j++)
                fd[j]=OKM[i*L+j];
            DBIG dx=DBIG.fromBytes(fd);
            FP w=new FP(dx.mod(q));
            u[i]=new FP(w);
        }
        return u;
    }

    private static void htp(String mess,String ro,String nu,int hlen) {
        byte[] DSTRO = ro.getBytes();
        byte[] DSTNU = nu.getBytes();
        byte[] M = mess.getBytes();
     
        BIG r = new BIG(ROM.CURVE_Order);
        if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY)
        {
            System.out.println("\nRandom oracle - message= "+mess);
            FP[] u=hash_to_field(HMAC.MC_SHA2,hlen,DSTRO,M,2);
            System.out.println("u[0]= "+u[0].toString());
            System.out.println("u[1]= "+u[1].toString());

            ECP P=ECP.map2point(u[0]);
            System.out.println("Q[0]= "+P.toString());
            if (P.is_infinity())
                fail("Hashing to curve failed\n");
            ECP P1=ECP.map2point(u[1]);
            System.out.println("Q[1]= "+P1.toString());
            if (P1.is_infinity())
                fail("Hashing to curve failed\n");
            P.add(P1);
            P.cfp();
            P.affine();
            System.out.println("P= "+P.toString());
            P=P.mul(r);
            if (!P.is_infinity())
                fail("Hashing to curve failed\n");
        }
        System.out.println("\nNon-Uniform");
        FP[] un=hash_to_field(HMAC.MC_SHA2,hlen,DSTNU,M,1);
        System.out.println("u[0]= "+un[0].toString());
        ECP Q=ECP.map2point(un[0]);
        System.out.println("Q= "+Q.toString());
        Q.cfp();
        Q.affine();
        System.out.println("P= "+Q.toString());
        Q=Q.mul(r);
        if (!Q.is_infinity())
            fail("Hashing to curve failed\n");
    }

    public void testHTP() {
        String packageName = this.getClass().getPackage().getName();
        String ro="";
        String nu="";

        boolean oneofmine=false;
        if (packageName == "org.miracl.core.ED25519") {
            ro="QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_";
            nu="QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_";
            System.out.println("\nTesting HTP for curve ED25519");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.C25519") {
            ro="QUUX-V01-CS02-with-curve25519_XMD:SHA-512_ELL2_RO_";
            nu="QUUX-V01-CS02-with-curve25519_XMD:SHA-512_ELL2_NU_";
            System.out.println("\nTesting HTP for curve C25519");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.NIST256") {
            ro="QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_RO_";
            nu="QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_NU_";
            System.out.println("\nTesting HTP for curve NIST256");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.ED448") {
            ro="QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_RO_";
            nu="QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_NU_";
            System.out.println("\nTesting HTP for curve ED448");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.SECP256K1") {
            ro="QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_RO_";
            nu="QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_NU_";
            System.out.println("\nTesting HTP for curve SECP256K1");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.BLS12381") {
            ro="QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_RO_";
            nu="QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_NU_";
            System.out.println("\nTesting HTP for curve BLS12381_G1");
            oneofmine=true;
        }

        if (!oneofmine) 
        {
            System.out.println("HTP not specified for this curve");
            return;
        }

        int hlen=CONFIG_CURVE.HASH_TYPE;  // default
        if (packageName == "org.miracl.core.ED25519" || packageName == "org.miracl.core.C25519") { // exception
            hlen=CONFIG_CURVE.SHA512;
        }
        htp("",ro,nu,hlen);
        htp("abc",ro,nu,hlen);
        htp("abcdef0123456789",ro,nu,hlen);
        htp("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
        htp("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
    }
}
