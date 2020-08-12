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

/* MPIN 128-bit API Functions */

package org.miracl.core.XXX;

import org.miracl.core.RAND;
import org.miracl.core.HMAC;

public class MPIN {
    public static final int EFS = CONFIG_BIG.MODBYTES;
    public static final int EGS = CONFIG_BIG.MODBYTES;
    public static final int INVALID_POINT = -14;
    public static final int BAD_PARAMS = -11;
    public static final int WRONG_ORDER = -18;
    public static final int BAD_PIN = -19;

    /* Configure your PIN here */

    public static final int MAXPIN = 10000; /* PIN less than this */
    public static final int PBLEN = 14;    /* Number of bits in PIN */

    private static int ceil(int a,int b) {
        return (((a)-1)/(b)+1);
    }

    public static void ENCODE_TO_CURVE(byte[] DST,byte[] ID,byte[] HCID) {
        BIG q=new BIG(ROM.Modulus);
        int k=q.nbits();
        BIG r=new BIG(ROM.CURVE_Order);
        int m=r.nbits();
        int L=ceil(k+ceil(m,2),8);
        byte[] OKM = HMAC.XMD_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,DST,ID);
        byte[] fd = new byte[L]; 

        for (int j=0;j<L;j++)
                fd[j]=OKM[j];
        DBIG dx=DBIG.fromBytes(fd);
        FP u=new FP(dx.mod(q));
        ECP P=ECP.map2point(u);
        P.cfp();
        P.affine();
        P.toBytes(HCID,false);
    }

    /* create random secret S */
    public static int RANDOM_GENERATE(RAND rng, byte[] S) {
        BIG r = new BIG(ROM.CURVE_Order);
        BIG s = BIG.randtrunc(r, 16 * CONFIG_CURVE.AESKEY, rng);
        s.toBytes(S);
        return 0;
    }

    /* Extract PIN from TOKEN for identity CID */
    public static int EXTRACT_PIN(byte[] CID, int pin, byte[] TOKEN) {
        ECP P = ECP.fromBytes(TOKEN);
        if (P.is_infinity()) return INVALID_POINT;
        ECP R = ECP.fromBytes(CID);
        if (R.is_infinity()) return INVALID_POINT;

        pin %= MAXPIN;
        R = R.pinmul(pin, PBLEN);
        P.sub(R);
        P.toBytes(TOKEN, false);
        return 0;
    }

    /* Implement step 2 on client side of MPin protocol */
    public static int CLIENT_2(byte[] X, byte[] Y, byte[] SEC) {
        BIG r = new BIG(ROM.CURVE_Order);
        ECP P = ECP.fromBytes(SEC);
        if (P.is_infinity()) return INVALID_POINT;

        BIG px = BIG.fromBytes(X);
        BIG py = BIG.fromBytes(Y);
        px.add(py);
        px.mod(r);

        P = PAIR.G1mul(P, px);
        P.neg();
        P.toBytes(SEC, false);
        return 0;
    }

/* Client secret CST=s*IDHTC where IDHTC is client ID hashed to a curve point, and s is the master secret */
    public static int GET_CLIENT_SECRET(byte[] S, byte[] IDHTC, byte[] CST) {
        BIG s = BIG.fromBytes(S);
        ECP P = ECP.fromBytes(IDHTC);
        if (P.is_infinity()) return INVALID_POINT;
        PAIR.G1mul(P, s).toBytes(CST, false);
        return 0;
    }

    /* Implement step 1 on client side of MPin protocol */
    public static int CLIENT_1(byte[] CID, RAND rng, byte[] X, int pin, byte[] TOKEN, byte[] SEC, byte[] xID) {
        BIG r = new BIG(ROM.CURVE_Order);
        BIG x;
        if (rng != null) {
            x = BIG.randtrunc(r, 16 * CONFIG_CURVE.AESKEY, rng);
            x.toBytes(X);
        } else {
            x = BIG.fromBytes(X);
        }
        ECP P, T, W;
        BIG px;

        P=ECP.fromBytes(CID);
        if (P.is_infinity()) return INVALID_POINT;

        T = ECP.fromBytes(TOKEN);
        if (T.is_infinity()) return INVALID_POINT;

        pin %= MAXPIN;
        W = P.pinmul(pin, PBLEN);
        T.add(W);

        P = PAIR.G1mul(P, x);
        P.toBytes(xID, false);
        
        T.toBytes(SEC, false);
        return 0;
    }

    /* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
    public static int GET_SERVER_SECRET(byte[] S, byte[] SST) {
        ECP2 Q = ECP2.generator();
        BIG s = BIG.fromBytes(S);
        Q = PAIR.G2mul(Q, s);
        Q.toBytes(SST, false);
        return 0;
    }

    /* Implement step 2 of MPin protocol on server side */
    public static int SERVER(byte[] HID, byte[] Y, byte[] SST, byte[] xID, byte[] mSEC) {
        BIG q = new BIG(ROM.Modulus);
        ECP2 Q = ECP2.generator();
        ECP2 sQ = ECP2.fromBytes(SST);
        if (sQ.is_infinity()) return INVALID_POINT;
        ECP R;
        if (xID == null) return BAD_PARAMS;
        R = ECP.fromBytes(xID);
        if (R.is_infinity()) return INVALID_POINT;
        BIG y = BIG.fromBytes(Y);
        ECP P;

        if (HID == null) return BAD_PARAMS;
        P = ECP.fromBytes(HID);

        if (P.is_infinity()) return INVALID_POINT;

        P = PAIR.G1mul(P, y);
        P.add(R); //P.affine();
        R = ECP.fromBytes(mSEC);
        if (R.is_infinity()) return INVALID_POINT;

        FP12 g;
        g = PAIR.ate2(Q, R, sQ, P);
        g = PAIR.fexp(g);

        if (!g.isunity()) {
            return BAD_PIN;
        }
        return 0;
    }
}
