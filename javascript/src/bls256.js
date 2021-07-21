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

/* Boneh-Lynn-Shacham signature 256-bit API Functions */

var BLS256 = function(ctx) {
    "use strict";

    var BLS256 = {
        BLS_OK: 0,
        BLS_FAIL: -1,

        BFS: ctx.BIG.MODBYTES,
        BGS: ctx.BIG.MODBYTES,
        G2_TAB: [],

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

        init: function() {
            var G = ctx.ECP8.generator();
			if (G.is_infinity()) return this.BLS_FAIL;
            this.G2_TAB = ctx.PAIR8.precomp(G);
			return this.BLS_OK;
        },

        ceil: function(a,b) {
            return Math.floor(((a)-1)/(b)+1);
        },

/* output u \in F_p */
        hash_to_field: function(hash,hlen,DST,M,ctr) {
            var q = new ctx.BIG(0);
            q.rcopy(ctx.ROM_FIELD.Modulus);
            var nbq=q.nbits();
            var L = this.ceil(nbq+ctx.ECP.AESKEY*8,8);
            var u=[];
            var fd=[];
            var OKM=ctx.HMAC.XMD_Expand(hash,hlen,L*ctr,DST,M);

            for (var i=0;i<ctr;i++)
            {
                for (var j=0;j<L;j++)
                    fd[j]=OKM[i*L+j];
                u[i]=new ctx.FP(ctx.DBIG.fromBytes(fd).ctmod(q,8*L-nbq));
            }
            return u;
        },

        /* hash a message to an ECP point, using SHA2 */
        bls_hash_to_point: function(M) {
            var dst= "BLS_SIG_XXXG1_XMD:SHA-512_SVDW_RO_NUL_";
            var u=this.hash_to_field(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,this.asciitobytes(dst),M,2);

            var P=ctx.ECP.map2point(u[0]);
            var P1=ctx.ECP.map2point(u[1]);
            P.add(P1);
            P.cfp();
            P.affine();
            return P;
        },

        /* generate key pair, private key S, public key W */
        KeyPairGenerate: function(IKM, S, W) {
            var r = new ctx.BIG(0);
            r.rcopy(ctx.ROM_CURVE.CURVE_Order); 
            var nbr = r.nbits();
            var L = this.ceil(3*this.ceil(nbr,8),2);
            var G = ctx.ECP8.generator();
	        var LEN=ctx.HMAC.inttobytes(L, 2);
            var AIKM = [];
            for (var i=0;i<IKM.length;i++)
                AIKM[i]=IKM[i];
            AIKM[IKM.length]=0;

            var salt="BLS-SIG-KEYGEN-SALT-";
            var PRK=ctx.HMAC.HKDF_Extract(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,this.asciitobytes(salt),AIKM);
            var OKM=ctx.HMAC.HKDF_Expand(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,L,PRK,LEN);

            var dx=ctx.DBIG.fromBytes(OKM);
            var s=dx.ctmod(r,8*L-nbr);
            s.toBytes(S);
// SkToPk
            G = ctx.PAIR8.G2mul(G, s);
            G.toBytes(W,true);
            return this.BLS_OK;
        },

        /* Sign message m using private key S to produce signature SIG */

        core_sign: function(SIG, M, S) {
            var D = this.bls_hash_to_point(M);
            var s = ctx.BIG.fromBytes(S);
            D = ctx.PAIR8.G1mul(D, s);
            D.toBytes(SIG, true);
            return this.BLS_OK;
        },

        /* Verify signature given message m, the signature SIG, and the public key W */

        core_verify: function(SIG, M, W) {
            var HM = this.bls_hash_to_point(M);

            var D = ctx.ECP.fromBytes(SIG);
            if (!ctx.PAIR8.G1member(D)) return this.BLS_FAIL;
            D.neg();

            var PK = ctx.ECP8.fromBytes(W);
            if (!ctx.PAIR8.G2member(PK)) return this.BLS_FAIL;
           
            //if (!ctx.PAIR8.G2member(PK)) alert("Not a member");
    //alert("PK= "+PK.toString());

            // Use new multi-pairing mechanism 
            var r = ctx.PAIR8.initmp();
            //			ctx.PAIR8.another(r,G,D);
            ctx.PAIR8.another_pc(r, this.G2_TAB, D);
            ctx.PAIR8.another(r, PK, HM);
            var v = ctx.PAIR8.miller(r);

            //.. or alternatively
            //var G = ctx.ECP8.generator();
            //var v=ctx.PAIR8.ate2(G,D,PK,HM);

            v = ctx.PAIR8.fexp(v);
            if (v.isunity())
                return this.BLS_OK;
            return this.BLS_FAIL;
        }
    };

    return BLS256;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports = {
        BLS256: BLS256
    };
}

