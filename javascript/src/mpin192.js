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

/* MPIN 192-bit API Functions */

var MPIN192 = function(ctx) {
    "use strict";

    var MPIN192 = {
        BAD_PARAMS: -11,
        INVALID_POINT: -14,
        WRONG_ORDER: -18,
        BAD_PIN: -19,
        /* configure PIN here */
        MAXPIN: 10000,
        /* max PIN */
        PBLEN: 14,
        /* MAXPIN length in bits */
        TS: 12,
        /* 10 for 4 digit PIN, 14 for 6-digit PIN - 2^TS/TS approx = sqrt(MAXPIN) */
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

        comparebytes: function(a, b) {
            if (a.length != b.length) {
                return false;
            }

            for (var i = 0; i < a.length; i++) {
                if (a[i] != b[i]) {
                    return false;
                }
            }

            return true;
        },

        ceil: function(a,b) {
            return Math.floor(((a)-1)/(b)+1);
        },

        ENCODE_TO_CURVE: function(DST,ID,HCID) {
            var q=new ctx.BIG(0);
	        q.rcopy(ctx.ROM_FIELD.Modulus);
            var k=q.nbits();
            var r=new ctx.BIG(0);
	        r.rcopy(ctx.ROM_CURVE.CURVE_Order);
            var m=r.nbits();
            var L=this.ceil(k+this.ceil(m,2),8);
            var OKM = ctx.HMAC.XMD_Expand(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,L,DST,ID);
            var fd = []; 

            for (var j=0;j<L;j++)
                fd[j]=OKM[j];
            var dx=ctx.DBIG.fromBytes(fd);
            var u=new ctx.FP(dx.mod(q));
            var P=ctx.ECP.map2point(u);
            P.cfp();
            P.affine();
            P.toBytes(HCID,false);
        },

        /* create random secret S */
        RANDOM_GENERATE: function(rng, S) {
            var r = new ctx.BIG(0),
                s;
            r.rcopy(ctx.ROM_CURVE.CURVE_Order);
            s = ctx.BIG.randtrunc(r, 16 * ctx.ECP.AESKEY, rng);
            s.toBytes(S);
            return 0;
        },

        /* Extract PIN from TOKEN for identity CID */
        EXTRACT_PIN: function(CID, pin, TOKEN) {
            var P, R;
            P = ctx.ECP.fromBytes(TOKEN);
            if (P.is_infinity()) {
                return this.INVALID_POINT;
            }
            R=ctx.ECP.fromBytes(CID);
            if (R.is_infinity()) {
                return this.INVALID_POINT;
            }
            pin %= this.MAXPIN;
            R = R.pinmul(pin, this.PBLEN);
            P.sub(R);
            P.toBytes(TOKEN, false);
            return 0;
        },

        /* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
        GET_SERVER_SECRET: function(S, SST) {
            var s, Q;
            Q = ctx.ECP4.generator();
            s = ctx.BIG.fromBytes(S);
            Q = ctx.PAIR4.G2mul(Q, s);
            Q.toBytes(SST, false);
            return 0;
        },

        /* Client secret CST=S*H(CID) where CID is client ID and S is master secret */
        GET_CLIENT_SECRET: function(S, IDHTC, CST) {
            var s = ctx.BIG.fromBytes(S);
            var P = ctx.ECP.fromBytes(IDHTC);
            if (P.is_infinity()) {
                return this.INVALID_POINT;
            }
            ctx.PAIR4.G1mul(P, s).toBytes(CST, false);
            return 0;
        },

        /* Implement step 1 on client side of MPin protocol */
        CLIENT_1: function(CID, rng, X, pin, TOKEN, SEC, xID) {
            var r = new ctx.BIG(0),
                x, P, T, W;

            r.rcopy(ctx.ROM_CURVE.CURVE_Order);
            if (rng !== null) {
                x = ctx.BIG.randtrunc(r, 16 * ctx.ECP.AESKEY, rng);
                x.toBytes(X);
            } else {
                x = ctx.BIG.fromBytes(X);
            }
            P=ctx.ECP.fromBytes(CID);
            if (P.is_infinity()) {
                return this.INVALID_POINT;
            }
            T = ctx.ECP.fromBytes(TOKEN);
            if (T.is_infinity()) {
                return this.INVALID_POINT;
            }

            pin %= this.MAXPIN;
            W = P.pinmul(pin, this.PBLEN);
            T.add(W);

            P = ctx.PAIR4.G1mul(P, x);
            P.toBytes(xID, false);
 
            T.toBytes(SEC, false);
            return 0;
        },

        /* Implement step 2 on client side of MPin protocol */
        CLIENT_2: function(X, Y, SEC) {
            var r = new ctx.BIG(0),
                P, px, py;

            r.rcopy(ctx.ROM_CURVE.CURVE_Order);

            P = ctx.ECP.fromBytes(SEC);
            if (P.is_infinity()) {
                return this.INVALID_POINT;
            }
            px = ctx.BIG.fromBytes(X);
            py = ctx.BIG.fromBytes(Y);
            px.add(py);
            px.mod(r);

            P = ctx.PAIR4.G1mul(P, px);
            P.neg();
            P.toBytes(SEC, false);
            return 0;
        },

        /* Implement step 2 of MPin protocol on server side.  */
        SERVER: function(HID, Y, SST, xID, mSEC) {
            var Q, sQ, R, y, P, g;
            Q = ctx.ECP4.generator();
            sQ = ctx.ECP4.fromBytes(SST);
            if (sQ.is_infinity()) {
                return this.INVALID_POINT;
            }
            if (xID == null) {
                return this.BAD_PARAMS;
            }
            R = ctx.ECP.fromBytes(xID);
            if (R.is_infinity()) {
                return this.INVALID_POINT;
            }
            y = ctx.BIG.fromBytes(Y);
            if (HID == null) {
                return this.BAD_PARAMS;
            }
            P = ctx.ECP.fromBytes(HID);
            
            if (P.is_infinity()) {
                return this.INVALID_POINT;
            }

            P = ctx.PAIR4.G1mul(P, y);
            P.add(R);
            R = ctx.ECP.fromBytes(mSEC);
            if (R.is_infinity()) {
                return this.INVALID_POINT;
            }

            g = ctx.PAIR4.ate2(Q, R, sQ, P);
            g = ctx.PAIR4.fexp(g);

            if (!g.isunity()) {
                return this.BAD_PIN;
            }
            return 0;
        }
    };

    return MPIN192;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports = {
        MPIN192: MPIN192
    };
}

