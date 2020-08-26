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

public class TestHTP2 extends TestCase { 

    private static int ceil(int a,int b) {
        return (((a)-1)/(b)+1);
    }

    private static FP2[] hash_to_field2(int hash,int hlen,byte[] DST,byte[] M,int ctr) {
		FP2[] u=new FP2[2];
        BIG q=new BIG(ROM.Modulus);
        int k=q.nbits();
        BIG r=new BIG(ROM.CURVE_Order);
        int m=r.nbits();
        int L=ceil(k+ceil(m,2),8);
        byte[] OKM = HMAC.XMD_Expand(hash,hlen,2*L*ctr,DST,M);
        byte[] fd = new byte[L]; 
        for (int i=0;i<ctr;i++)
        {
            for (int j=0;j<L;j++)
                fd[j]=OKM[2*i*L+j];
            DBIG dx=DBIG.fromBytes(fd);
            FP w1=new FP(dx.mod(q));

            for (int j=0;j<L;j++)
                fd[j]=OKM[(2*i+1)*L+j];
            dx=DBIG.fromBytes(fd);
            FP w2=new FP(dx.mod(q));
            u[i]=new FP2(w1,w2);
        }
        return u;
    }

    private static void htp2(String mess,String ro,String nu,int hlen) {
        System.out.println("\nRandom oracle - message= "+mess);
        byte[] DSTRO = ro.getBytes();
        byte[] DSTNU = nu.getBytes();
        byte[] M = mess.getBytes();
        BIG r = new BIG(ROM.CURVE_Order);

        FP2[] u=hash_to_field2(HMAC.MC_SHA2,hlen,DSTRO,M,2);
        System.out.println("u[0]= "+u[0].toString());
        System.out.println("u[1]= "+u[1].toString());

        ECP2 P=ECP2.map2point(u[0]);
        System.out.println("Q[0]= "+P.toString());
        if (P.is_infinity())
            fail("Hashing to curve failed\n");
        ECP2 P1=ECP2.map2point(u[1]);
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

        System.out.println("\nNon-Uniform");
        u=hash_to_field2(HMAC.MC_SHA2,hlen,DSTNU,M,1);
        System.out.println("u[0]= "+u[0].toString());
        P=ECP2.map2point(u[0]);
        System.out.println("Q= "+P.toString());
        P.cfp();
        P.affine();
        System.out.println("P= "+P.toString());
        P=P.mul(r);
        if (!P.is_infinity())
            fail("Hashing to curve failed\n");
    }

    public void testHTP2() {
        String packageName = this.getClass().getPackage().getName();
        String ro="";
        String nu="";

        boolean oneofmine=false;

        if (packageName == "org.miracl.core.BLS12381") {
            ro="QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_RO_";
            nu="QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_NU_";
            System.out.println("\nTesting HTP for curve BLS12381_G2");
            oneofmine=true;
        }

        if (!oneofmine) 
        {
            System.out.println("HTP2 not specified for this curve");
            return;
        }

        int hlen=CONFIG_CURVE.HASH_TYPE;  // default

        htp2("",ro,nu,hlen);
        htp2("abc",ro,nu,hlen);
        htp2("abcdef0123456789",ro,nu,hlen);
        htp2("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
        htp2("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
    
    }
}
