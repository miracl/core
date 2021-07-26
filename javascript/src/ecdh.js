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

var ECDH = function(ctx) {
    "use strict";

    var ECDH = {

        INVALID_PUBLIC_KEY: -2,
        ERROR: -3,
        //INVALID: -4,
        EFS: ctx.BIG.MODBYTES,
        EGS: ctx.BIG.MODBYTES,
        SHA256: 32,
        SHA384: 48,
        SHA512: 64,

        bytestostring: function(b) {
            var s = "",
                len = b.length,
                ch, i;

            for (i = 0; i < len; i++) {
                ch = b[i];
                s += ((ch >>> 4) & 15).toString(16);
                s += (ch & 15).toString(16);
            }

            return s;
        },

        asciitobytes: function(s) {
            var b = [],
                i;

            for (i = 0; i < s.length; i++) {
                b.push(s.charCodeAt(i));
            }

            return b;
        },

        stringtobytes: function(s) {
            var b = [],
                i;

            for (i = 0; i < s.length; i+=2) {
                b.push(parseInt(s.substr(i,2),16));
            }

            return b;
        },

        RFC7748: function(r) {
            var c,lg=0;
            var t = new ctx.BIG(1);
            c=ctx.ROM_CURVE.CURVE_Cof_I;
            while (c!=1)
            {
                lg++;
                c/=2;
            }
            var n=8*this.EGS-lg+1;
            r.mod2m(n);
            t.shl(n);
            r.add(t);
            c=r.lastbits(lg);
            r.dec(c);
        },

        IN_RANGE: function(S) {
            var r,s;
            r = new ctx.BIG(0);
            r.rcopy(ctx.ROM_CURVE.CURVE_Order);
            s = ctx.BIG.fromBytes(S);
            if (s.iszilch()) return false;
            if (ctx.BIG.comp(s,r)>=0) return false;
            return true;
        },

        KEY_PAIR_GENERATE: function(RNG, S, W) {
            var res = 0,
                r, s, G, WP;
            // var T=[];

            G = ctx.ECP.generator();
            r = new ctx.BIG(0);
            r.rcopy(ctx.ROM_CURVE.CURVE_Order);

            if (RNG === null) {
                s = ctx.BIG.fromBytes(S);
            } else {
                if (ctx.ECP.CURVETYPE != ctx.ECP.WEIERSTRASS)
                    s = ctx.BIG.random(RNG);            // from random bytes
                else
                    s = ctx.BIG.randomnum(r, RNG);      // Removes biases

            }

            if (ctx.ECP.CURVETYPE != ctx.ECP.WEIERSTRASS)
                this.RFC7748(s);            // For Montgomery or Edwards, apply RFC7748 transformation

            s.toBytes(S);
            WP = G.clmul(s,r);
            WP.toBytes(W, false);           // To use point compression on public keys, change to true 

            return res;
        },

        PUBLIC_KEY_VALIDATE: function(W) {
            var WP = ctx.ECP.fromBytes(W),
                res = 0,
                r, q, nb, k;

            r = new ctx.BIG(0);
            r.rcopy(ctx.ROM_CURVE.CURVE_Order);

            if (WP.is_infinity()) {
                res = this.INVALID_PUBLIC_KEY;
            }

            if (res === 0) {
                q = new ctx.BIG(0);
                q.rcopy(ctx.ROM_FIELD.Modulus);
                nb = q.nbits();
                k = new ctx.BIG(1);
                k.shl(Math.floor((nb + 4) / 2));
                k.add(q);
                k.div(r);

                while (k.parity() == 0) {
                    k.shr(1);
                    WP.dbl();
                }

                if (!k.isunity()) {
                    WP = WP.mul(k);
                }

                if (WP.is_infinity()) {
                    res = this.INVALID_PUBLIC_KEY;
                }
            }

            return res;
        },
    /* IEEE-1363 Diffie-Hellman online calculation Z=S.WD */
    // type = 0 is just x coordinate output
    // type = 1 for standard compressed output
    // type = 2 for standard uncompress output 04|x|y
        ECPSVDP_DH: function(S, WD, Z, type) {
            var res = 0,
                r, s, i,
                W;

            s = ctx.BIG.fromBytes(S);

            W = ctx.ECP.fromBytes(WD);
            if (W.is_infinity()) {
                res = this.ERROR;
            }

            if (res === 0) {
                r = new ctx.BIG(0);
                r.rcopy(ctx.ROM_CURVE.CURVE_Order);
                W = W.clmul(s,r);

                if (W.is_infinity()) {
                    res = this.ERROR;
                } else {
                    if (ctx.ECP.CURVETYPE != ctx.ECP.MONTGOMERY)
                    {
                        if (type>0)
                        {
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
        },
    /* IEEE ECDSA Signature, C and D are signature on F using private key S */
        ECPSP_DSA: function(sha, RNG, S, F, C, D) {
            var T = [],
                i, r, s, f, c, d, u, vx, w,
                G, V, B;

            B = ctx.HMAC.GPhashit(ctx.HMAC.MC_SHA2, sha, this.EGS, 0, F, -1, null);

            G = ctx.ECP.generator();

            r = new ctx.BIG(0);
            r.rcopy(ctx.ROM_CURVE.CURVE_Order);

            s = ctx.BIG.fromBytes(S);
            f = ctx.BIG.fromBytes(B);

            c = new ctx.BIG(0);
            d = new ctx.BIG(0);
            V = new ctx.ECP();

            do {
                u = ctx.BIG.randomnum(r, RNG);
                w = ctx.BIG.randomnum(r, RNG); /* IMPORTANT - side channel masking to protect invmodp() */
 
                V.copy(G);
                V = V.clmul(u,r);
                vx = V.getX();
                c.copy(vx);
                c.mod(r);
                if (c.iszilch()) {
                    continue;
                }
                u = ctx.BIG.modmul(u, w, r);
                u.invmodp(r);
                d = ctx.BIG.modmul(s, c, r);
                d = ctx.BIG.modadd(d, f, r);
                d = ctx.BIG.modmul(d, w, r);
                d = ctx.BIG.modmul(u, d, r);
            } while (d.iszilch());

            c.toBytes(T);
            for (i = 0; i < this.EGS; i++) {
                C[i] = T[i];
            }
            d.toBytes(T);
            for (i = 0; i < this.EGS; i++) {
                D[i] = T[i];
            }

            return 0;
        },

    /* IEEE1363 ECDSA Signature Verification. Signature C and D on F is verified using public key W */
        ECPVP_DSA: function(sha, W, F, C, D) {
            var B = [],
                res = 0,
                r, f, c, d, h2,
                G, WP, P;

            B = ctx.HMAC.GPhashit(ctx.HMAC.MC_SHA2, sha, this.EGS, 0, F, -1, null);

            G = ctx.ECP.generator();

            r = new ctx.BIG(0);
            r.rcopy(ctx.ROM_CURVE.CURVE_Order);

            c = ctx.BIG.fromBytes(C);
            d = ctx.BIG.fromBytes(D);
            f = ctx.BIG.fromBytes(B);

            if (c.iszilch() || ctx.BIG.comp(c, r) >= 0 || d.iszilch() || ctx.BIG.comp(d, r) >= 0) {
                res = this.ERROR;
            }

            if (res === 0) {
                d.invmodp(r);
                f = ctx.BIG.modmul(f, d, r);
                h2 = ctx.BIG.modmul(c, d, r);

                WP = ctx.ECP.fromBytes(W);
                if (WP.is_infinity()) {
                    res = this.ERROR;
                } else {
                    P = new ctx.ECP();
/*
    var X=[]; X[0]=new ctx.ECP(); X[1]=new ctx.ECP();
    var e=[]; e[0]=new ctx.BIG(); e[1]=new ctx.BIG();
    X[0].copy(WP); X[1].copy(G);
    e[0].copy(h2); e[1].copy(f);
    P=ctx.ECP.muln(2,X,e);
*/

                    P.copy(WP);
                    P = P.mul2(h2, G, f);

                    if (P.is_infinity()) {
                        res = this.ERROR;
                    } else {
                        d = P.getX();
                        d.mod(r);
                        if (ctx.BIG.comp(d, c) !== 0) {
                            res = this.ERROR;
                        }
                    }
                }
            }

            return res;
        },

        ECIES_ENCRYPT: function(sha, P1, P2, RNG, W, M, V, T) {
            var Z = [],
                VZ = [],
                K1 = [],
                K2 = [],
                U = [],
                C = [],
                K, L2, AC, i;

            if (this.KEY_PAIR_GENERATE(RNG, U, V) !== 0) {
                return C;
            }

            if (this.ECPSVDP_DH(U, W, Z, 0) !== 0) {
                return C;
            }

            for (i = 0; i < 2 * this.EFS + 1; i++) {
                VZ[i] = V[i];
            }

            for (i = 0; i < this.EFS; i++) {
                VZ[2 * this.EFS + 1 + i] = Z[i];
            }

            K = ctx.HMAC.KDF2(ctx.HMAC.MC_SHA2, sha, VZ, P1, 2 * ctx.ECP.AESKEY);

            for (i = 0; i < ctx.ECP.AESKEY; i++) {
                K1[i] = K[i];
                K2[i] = K[ctx.ECP.AESKEY + i];
            }

            C = ctx.AES.CBC_IV0_ENCRYPT(K1, M);

            L2 = ctx.HMAC.inttobytes(P2.length, 8);

            AC = [];
            for (i = 0; i < C.length; i++) {
                AC[i] = C[i];
            }
            for (i = 0; i < P2.length; i++) {
                AC[C.length + i] = P2[i];
            }
            for (i = 0; i < 8; i++) {
                AC[C.length + P2.length + i] = L2[i];
            }

            ctx.HMAC.HMAC1(ctx.HMAC.MC_SHA2, sha, T, T.length, K2, AC);

            return C;
        },

        ncomp: function(T1, T2, n) {
            var res = 0;
            for (var i = 0; i < n; i++) {
                res |= (T1[i] ^ T2[i]);
            }
            if (res == 0) return true;
            return false;
        },

        ECIES_DECRYPT: function(sha, P1, P2, V, C, T, U) {
            var Z = [],
                VZ = [],
                K1 = [],
                K2 = [],
                TAG = new Array(T.length),
                M = [],
                K, L2, AC, i;

            if (this.ECPSVDP_DH(U, V, Z, 0) !== 0) {
                return M;
            }

            for (i = 0; i < 2 * this.EFS + 1; i++) {
                VZ[i] = V[i];
            }

            for (i = 0; i < this.EFS; i++) {
                VZ[2 * this.EFS + 1 + i] = Z[i];
            }

            K = ctx.HMAC.KDF2(ctx.HMAC.MC_SHA2, sha, VZ, P1, 2 * ctx.ECP.AESKEY);

            for (i = 0; i < ctx.ECP.AESKEY; i++) {
                K1[i] = K[i];
                K2[i] = K[ctx.ECP.AESKEY + i];
            }

            M = ctx.AES.CBC_IV0_DECRYPT(K1, C);

            if (M.length === 0) {
                return M;
            }

            L2 = ctx.HMAC.inttobytes(P2.length, 8);

            AC = [];

            for (i = 0; i < C.length; i++) {
                AC[i] = C[i];
            }
            for (i = 0; i < P2.length; i++) {
                AC[C.length + i] = P2[i];
            }
            for (i = 0; i < 8; i++) {
                AC[C.length + P2.length + i] = L2[i];
            }

            ctx.HMAC.HMAC1(ctx.HMAC.MC_SHA2, sha, TAG, TAG.length, K2, AC);

            if (!this.ncomp(T, TAG, T.length)) {
                return [];
            }

            return M;
        }
    };

    return ECDH;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports = {
        ECDH: ECDH
    };
}

