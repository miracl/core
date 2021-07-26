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

/* ECDH/ECIES/ECDSA API Functions */

package org.miracl.core.XXX;

import org.miracl.core.RAND;
import org.miracl.core.HASH256;
import org.miracl.core.HASH384;
import org.miracl.core.HASH512;
import org.miracl.core.HMAC;
import org.miracl.core.AES;

public final class ECDH {
    public static final int INVALID_PUBLIC_KEY = -2;
    public static final int ERROR = -3;
    //public static final int INVALID = -4;
    public static final int EFS = CONFIG_BIG.MODBYTES;
    public static final int EGS = CONFIG_BIG.MODBYTES;

// Transform a point multiplier to RFC7748 form
    public static void RFC7748(BIG r)
    {
        int c,lg=0;
        BIG t=new BIG(1);
        c=ROM.CURVE_Cof_I;
        while (c!=1)
        {
            lg++;
            c/=2;
        }
        int n=8*EGS-lg+1;
        r.mod2m(n);
        t.shl(n);
        r.add(t);
        c=r.lastbits(lg);
        r.dec(c);
    }

    /* return true if S is in ranger 0 < S < order , else return false */
    public static boolean IN_RANGE(byte[] S) {
        BIG r, s;
        r = new BIG(ROM.CURVE_Order);
        s = BIG.fromBytes(S);
        if (s.iszilch()) return false;
        if (BIG.comp(s,r)>=0) return false;
        return true;
    }

    /* Calculate a public/private EC GF(p) key pair W,S where W=S.G mod EC(p),
     * where S is the secret key and W is the public key
     * and G is fixed generator.
     * If RNG is NULL then the private key is provided externally in S
     * otherwise it is generated randomly internally */
    public static int KEY_PAIR_GENERATE(RAND RNG, byte[] S, byte[] W) {
        BIG r, s;
        ECP G, WP;
        int res = 0;

        G = ECP.generator();
        r = new BIG(ROM.CURVE_Order);

        if (RNG == null) {
            s = BIG.fromBytes(S);
        } else {
            if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.WEIERSTRASS)
                s = BIG.random(RNG);            // from random bytes
            else
                s = BIG.randomnum(r, RNG);      // Removes biases
        }

        if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.WEIERSTRASS)
            RFC7748(s);             // For Montgomery or Edwards, apply RFC7748 transformation

        s.toBytes(S);
        WP = G.clmul(s,r);
        WP.toBytes(W, false);       // To use point compression on public keys, change to true

        return res;
    }

    /* validate public key. */
    public static int PUBLIC_KEY_VALIDATE(byte[] W) {
        BIG r, q, k;
        ECP WP = ECP.fromBytes(W);
        int nb, res = 0;

        r = new BIG(ROM.CURVE_Order);

        if (WP.is_infinity()) res = INVALID_PUBLIC_KEY;

        if (res == 0) {

            q = new BIG(ROM.Modulus);
            nb = q.nbits();
            k = new BIG(1); k.shl((nb + 4) / 2);
            k.add(q);
            k.div(r);

            while (k.parity() == 0) {
                k.shr(1);
                WP.dbl();
            }

            if (!k.isunity()) WP = WP.mul(k);
            if (WP.is_infinity()) res = INVALID_PUBLIC_KEY;
        }
        return res;
    }

    /* IEEE-1363 Diffie-Hellman online calculation Z=S.WD */
    // type = 0 is just x coordinate output
    // type = 1 for standard compressed output
    // type = 2 for standard uncompress output 04|x|y
    public static int SVDP_DH(byte[] S, byte[] WD, byte[] Z, int type) {
        BIG r, s;
        int res = 0;

        s = BIG.fromBytes(S);
        ECP W = ECP.fromBytes(WD);
        if (W.is_infinity()) res = ERROR;

        if (res == 0) {
            r = new BIG(ROM.CURVE_Order);
            W = W.clmul(s,r);
            if (W.is_infinity()) res = ERROR;
            else {
		        if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY)
		        {
                    if (type>0) {
                        if (type==1)
                            W.toBytes(Z,true);
                        else 
                            W.toBytes(Z,false);
                    } else {
                        W.getX().toBytes(Z);
                    }
                    return res;
                } else {
                    W.getX().toBytes(Z);
                }
            }
        }
        return res;
    }

    /* IEEE ECDSA Signature, C and D are signature on F using private key S */
    public static int SP_DSA(int sha, RAND RNG, byte[] S, byte[] F, byte[] C, byte[] D) {
        byte[] T = new byte[EGS];
        BIG r, s, f, c, d, u, vx, w;
        ECP G, V;

        byte[] B = HMAC.GPhashit(HMAC.MC_SHA2, sha, EGS, 0,F, -1, null );

        G = ECP.generator();
        r = new BIG(ROM.CURVE_Order);

        s = BIG.fromBytes(S);
        f = BIG.fromBytes(B);

        c = new BIG(0);
        d = new BIG(0);
        V = new ECP();

        do {
            u = BIG.randomnum(r, RNG);
            w = BIG.randomnum(r, RNG); /* IMPORTANT - side channel masking to protect invmodp() */

            V.copy(G);
            V = V.clmul(u,r);
            vx = V.getX();
            c.copy(vx);
            c.mod(r);
            if (c.iszilch()) continue;

            u.copy(BIG.modmul(u, w, r));

            u.invmodp(r);
            d.copy(BIG.modmul(s, c, r));
            d.copy(BIG.modadd(d, f, r));

            d.copy(BIG.modmul(d, w, r));
            d.copy(BIG.modmul(u, d, r));
        } while (d.iszilch());

        c.toBytes(T);
        for (int i = 0; i < EGS; i++) C[i] = T[i];
        d.toBytes(T);
        for (int i = 0; i < EGS; i++) D[i] = T[i];
        return 0;
    }

    /* IEEE1363 ECDSA Signature Verification. Signature C and D on F is verified using public key W */
    public static int VP_DSA(int sha, byte[] W, byte[] F, byte[] C, byte[] D) {
        BIG r, f, c, d, h2;
        int res = 0;
        ECP G, WP, P;
        int valid;

        byte[] B = HMAC.GPhashit(HMAC.MC_SHA2, sha, EGS, 0, F, -1, null);

        G = ECP.generator();
        r = new BIG(ROM.CURVE_Order);

        c = BIG.fromBytes(C);
        d = BIG.fromBytes(D);
        f = BIG.fromBytes(B);

        if (c.iszilch() || BIG.comp(c, r) >= 0 || d.iszilch() || BIG.comp(d, r) >= 0)
            res = ERROR;

        if (res == 0) {
            d.invmodp(r);
            f.copy(BIG.modmul(f, d, r));
            h2 = BIG.modmul(c, d, r);

            WP = ECP.fromBytes(W);
            if (WP.is_infinity()) res = ERROR;
            else {
                P = new ECP();
                P.copy(WP);
                P = P.mul2(h2, G, f);
                if (P.is_infinity()) res = ERROR;
                else {
                    d = P.getX();
                    d.mod(r);
                    if (BIG.comp(d, c) != 0) res = ERROR;
                }
            }
        }
        return res;
    }

    /* IEEE1363 ECIES encryption. Encryption of plaintext M uses public key W and produces ciphertext V,C,T */
    public static byte[] ECIES_ENCRYPT(int sha, byte[] P1, byte[] P2, RAND RNG, byte[] W, byte[] M, byte[] V, byte[] T) {
        int i, len;

        byte[] Z = new byte[EFS];
        byte[] VZ = new byte[3 * EFS + 1];
        byte[] K1 = new byte[CONFIG_CURVE.AESKEY];
        byte[] K2 = new byte[CONFIG_CURVE.AESKEY];
        byte[] U = new byte[EGS];

        if (KEY_PAIR_GENERATE(RNG, U, V) != 0) return new byte[0];
        if (SVDP_DH(U, W, Z, 0) != 0) return new byte[0];

        for (i = 0; i < 2 * EFS + 1; i++) VZ[i] = V[i];
        for (i = 0; i < EFS; i++) VZ[2 * EFS + 1 + i] = Z[i];


        byte[] K = HMAC.KDF2(HMAC.MC_SHA2,sha, VZ, P1, 2 * CONFIG_CURVE.AESKEY);

        for (i = 0; i < CONFIG_CURVE.AESKEY; i++) {K1[i] = K[i]; K2[i] = K[CONFIG_CURVE.AESKEY + i];}

        byte[] C = AES.CBC_IV0_ENCRYPT(K1, M);
        byte[] L2 = HMAC.inttoBytes(P2.length, 8);

        byte[] AC = new byte[C.length + P2.length + 8];
        for (i = 0; i < C.length; i++) AC[i] = C[i];
        for (i = 0; i < P2.length; i++) AC[C.length + i] = P2[i];
        for (i = 0; i < 8; i++) AC[C.length + P2.length + i] = L2[i];

        HMAC.HMAC1(HMAC.MC_SHA2, sha, T, T.length, K2, AC);

        return C;
    }

    /* constant time n-byte compare */
    static boolean ncomp(byte[] T1, byte[] T2, int n) {
        int res = 0;
        for (int i = 0; i < n; i++) {
            res |= (int)(T1[i] ^ T2[i]);
        }
        if (res == 0) return true;
        return false;
    }

    /* IEEE1363 ECIES decryption. Decryption of ciphertext V,C,T using private key U outputs plaintext M */
    public static byte[] ECIES_DECRYPT(int sha, byte[] P1, byte[] P2, byte[] V, byte[] C, byte[] T, byte[] U) {

        int i, len;

        byte[] Z = new byte[EFS];
        byte[] VZ = new byte[3 * EFS + 1];
        byte[] K1 = new byte[CONFIG_CURVE.AESKEY];
        byte[] K2 = new byte[CONFIG_CURVE.AESKEY];
        byte[] TAG = new byte[T.length];

        if (SVDP_DH(U, V, Z, 0) != 0) return new byte[0];

        for (i = 0; i < 2 * EFS + 1; i++) VZ[i] = V[i];
        for (i = 0; i < EFS; i++) VZ[2 * EFS + 1 + i] = Z[i];

        byte[] K = HMAC.KDF2(HMAC.MC_SHA2,sha, VZ, P1, 2 * CONFIG_CURVE.AESKEY);

        for (i = 0; i < CONFIG_CURVE.AESKEY; i++) {K1[i] = K[i]; K2[i] = K[CONFIG_CURVE.AESKEY + i];}

        byte[] M = AES.CBC_IV0_DECRYPT(K1, C);

        if (M.length == 0) return M;

        byte[] L2 = HMAC.inttoBytes(P2.length, 8);

        byte[] AC = new byte[C.length + P2.length + 8];

        for (i = 0; i < C.length; i++) AC[i] = C[i];
        for (i = 0; i < P2.length; i++) AC[C.length + i] = P2[i];
        for (i = 0; i < 8; i++) AC[C.length + P2.length + i] = L2[i];

        HMAC.HMAC1(HMAC.MC_SHA2, sha, TAG, TAG.length, K2, AC);
        if (!ncomp(T, TAG, T.length)) return new byte[0];
        return M;
    }
}
