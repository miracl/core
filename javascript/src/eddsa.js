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

/* EDDSA API Functions */

var EDDSA = function(ctx) {
    "use strict";

    var EDDSA = {

        INVALID_PUBLIC_KEY: -2,
        ERROR: -3,
        //INVALID: -4,
        EFS: ctx.BIG.MODBYTES,
        EGS: ctx.BIG.MODBYTES,
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

    // reverse first n bytes of buff - for little endian
        reverse: function(n,buff) {
            for (var i=0;i<n/2;i++) { 
                var ch = buff[i]; 
                buff[i] = buff[n - i - 1]; 
                buff[n - i - 1] = ch; 
            }
        },
            
    // dom - domain function
        dom: function(pp,ph,cl) {
            var PP=this.asciitobytes(pp);
            var len=PP.length;
            var dom = [];
            for (var i=0;i<len;i++ )
                dom[i]=PP[i];
            if (ph) dom[len]=1;
            else dom[len]=0;
            dom[len+1]=cl;
            return dom;
        },

        H: function(S) {
            if (ctx.ECP.AESKEY<=16) { // for ed25519?
                var sh = new ctx.HASH512();
                for (var i=0;i<S.length;i++)
                    sh.process(S[i]);
                return sh.hash();
            } else {                       // for ed448?
                var digest = [];
                var sh=new ctx.SHA3(ctx.SHA3.SHAKE256);
                for (var i=0;i<S.length;i++)
                    sh.process(S[i]);
                sh.shake(digest,2*S.length);
                return digest;
            }
        },

        H2: function(ph,context,R,Q,M)  {
            var b=Q.length;
            var cl;
            if (context==null) cl=0;
            else cl=context.length; 
            if (ctx.ECP.AESKEY<=16) { // Ed25519??
                var sh = new ctx.HASH512();
                if (ph || cl>0)
                {                   // if not prehash and no context, omit dom2()
                    domain=this.dom("Sig"+ctx.config["CURVE"]+" no "+ctx.config["CURVE"]+" collisions",ph,cl);
                    for (var i=0;i<domain.length;i++)
                        sh.process(domain[i]);
                    for (var i=0;i<cl;i++)
                        sh.process(context[i]);
                }
                for (var i=0;i<b;i++ )
                    sh.process(R[i]);
                for (var i=0;i<b;i++ )
                    sh.process(Q[i]);
                for (var i=0;i<M.length;i++ )
                    sh.process(M[i]);
                var h=sh.hash();
                this.reverse(64,h);
                return ctx.DBIG.fromBytes(h);
            } else {                       // for ed448?
                var domain=this.dom("Sig"+ctx.config["CURVE"],ph,cl);
                var h=[];
                var sh=new ctx.SHA3(ctx.SHA3.SHAKE256);
                for (var i=0;i<domain.length;i++)
                    sh.process(domain[i]);
                for (var i=0;i<cl;i++)
                    sh.process(context[i]);
                for (var i=0;i<b;i++) 
                    sh.process(R[i]);
                for (var i=0;i<b;i++) 
                    sh.process(Q[i]);
                for (var i=0;i<M.length;i++) 
                    sh.process(M[i]);
                sh.shake(h,2*b);
                this.reverse(2*b,h);
                return ctx.DBIG.fromBytes(h);
            }
        },

        getR: function(ph,b,digest,context,M) {
            var domain;
            var cl;
            if (context==null) cl=0;
            else cl=context.length; 

            if (ctx.ECP.AESKEY<=16) { // Ed25519??
                var sh = new ctx.HASH512();
                if (ph || cl>0)
                {                   // if not prehash and no context, omit dom2()
                    domain=this.dom("Sig"+ctx.config["CURVE"]+" no "+ctx.config["CURVE"]+" collisions",ph,cl);
                    for (var i=0;i<domain.length;i++)
                        sh.process(domain[i]);
                    for (var i=0;i<cl;i++)
                        sh.process(context[i]);
                }
                for (var i=b;i<2*b;i++ )
                    sh.process(digest[i]);
                for (var i=0;i<M.length;i++)
                    sh.process(M[i]);
                var h=sh.hash();
                this.reverse(64,h);
                return ctx.DBIG.fromBytes(h);
            } else {                       // for ed448?
                domain=this.dom("Sig"+ctx.config["CURVE"],ph,cl);
                var sh=new ctx.SHA3(ctx.SHA3.SHAKE256);
                var h=[];
                for (var i=0;i<domain.length;i++)
                    sh.process(domain[i]);
                for (var i=0;i<cl;i++)
                    sh.process(context[i]);
                for (var i=b;i<2*b;i++ )
                    sh.process(digest[i]);
                for (var i=0;i<M.length;i++) 
                    sh.process(M[i]);
                sh.shake(h,2*b); 
                this.reverse(2*b,h);
                return ctx.DBIG.fromBytes(h);
            }
        },

// encode integer (little endian)
        encode_int: function(x,w) {
            var b,index=0;
            if (8*ctx.BIG.MODBYTES==ctx.FP.MODBITS) index=1; // extra byte needed for compression        
            b=ctx.BIG.MODBYTES+index;

            w[0]=0;
            x.tobytearray(w,index);
            this.reverse(b,w);
            return b;
        },

// encode point
        encode: function(P,w) {
            var b,index=0;
            if (8*ctx.BIG.MODBYTES==ctx.FP.MODBITS) index=1; // extra byte needed for compression        
            b=ctx.BIG.MODBYTES+index;

            var x=P.getX();
            var y=P.getY();
            this.encode_int(y,w);
            w[b-1]|=x.parity()<<7;
        },

// get sign
        getsign: function(x) {
            var b,index=0;
            if (8*ctx.BIG.MODBYTES==ctx.FP.MODBITS) index=1; // extra byte needed for compression        
            b=ctx.BIG.MODBYTES+index;
            if ((x[b-1]&0x80)!=0)
                return 1;
            else 
                return 0;
        },

// decode integer (little endian)
        decode_int: function(strip_sign,ei) {
            var sign=0;
            var b,index=0;
            if (8*ctx.BIG.MODBYTES==ctx.FP.MODBITS) index=1; // extra byte needed for compression        
            b=ctx.BIG.MODBYTES+index;

            var r=[];

            for (var i=0;i<b;i++)
                r[i]=ei[i];
            this.reverse(b,r);

            if (strip_sign)
                r[0]&=0x7f;

            return ctx.BIG.frombytearray(r,index);
        },

// decode compressed point
        decode: function(W) {
            var P=new ctx.ECP();
            var sign=this.getsign(W); // lsb of x
            var y=this.decode_int(true,W);
            var one=new ctx.FP(1);
            var hint=new ctx.FP();
            var x=new ctx.FP(y); 
            x.sqr(); 
            var d=new ctx.FP(x); 
            x.sub(one);
            x.norm();
            var t=new ctx.FP(0);
            t.rcopy(ctx.ROM_CURVE.CURVE_B);
            d.mul(t);
            if (ctx.ECP.CURVE_A==1) d.sub(one);
            if (ctx.ECP.CURVE_A==-1) d.add(one);
            d.norm();
// inverse square root trick for sqrt(x/d)
            t.copy(x);
            t.sqr();
            x.mul(t);
            x.mul(d);
            if (x.qr(hint)!=1)
            {
                P.inf();
                return P;
            }
            d.copy(x.sqrt(hint));
            x.inverse(hint);
            x.mul(d);
            x.mul(t);
            x.reduce();
            if (x.redc().parity()!=sign)
                x.neg();
            x.norm();
            P.setxy(x.redc(),y);
            return P;
        },

/* Calculate a public/private EC GF(p) key pair. Q=D.G mod EC(p),
 * where D is the secret key and Q is the public key
 * and G is fixed generator.
 * RNG is a cryptographically strong RNG 
 * If RNG==NULL, D is provided externally 
 */
        KEY_PAIR_GENERATE: function(RNG, D, Q) {
            var res = 0;
            var b,index=0;
            if (8*ctx.BIG.MODBYTES==ctx.FP.MODBITS) index=1; // extra byte needed for compression        
            b=ctx.BIG.MODBYTES+index;

            var G=ctx.ECP.generator();

            if (RNG != null) {
                for (var i=0;i<b;i++)
                    D[i]=RNG.getByte();
            }

            var digest=this.H(D);

// reverse bytes for little endian
            this.reverse(b,digest);
            var s=ctx.BIG.frombytearray(digest,index);
            this.RFC7748(s);
            G.copy(G.mul(s));
            this.encode(G,Q);
            return res;
        },

// Generate a signature using key pair (D,Q) on message M
// Set ph=true if message has already been pre-hashed
// if ph=false, then context should be NULL for ed25519. However RFC8032 mode ed25519ctx is supported by supplying a non-NULL or non-empty context
        SIGNATURE: function(ph,D,context,M,SIG) {
            var digest=this.H(D);   // hash of private key
            var res = 0;
            var b,index=0;
            if (8*ctx.BIG.MODBYTES==ctx.FP.MODBITS) index=1; // extra byte needed for compression        
            b=ctx.BIG.MODBYTES+index;

            var S=[];
            var Q=[];

            this.KEY_PAIR_GENERATE(null,D,Q);

            var q = new ctx.BIG();
            q.rcopy(ctx.ROM_CURVE.CURVE_Order);
       
            if (D.length!=Q.length || D.length!=b)
                res=INVALID_PUBLIC_KEY;
            if (res==0)
            {
                var dr=this.getR(ph,b,digest,context,M);
                var sr=dr.mod(q);
                var R=ctx.ECP.generator().mul(sr);
                this.encode(R,S);
                for (var i=0;i<b;i++)
                    SIG[i]=S[i];
// reverse bytes for little endian        
                this.reverse(b,digest);
                var s=ctx.BIG.frombytearray(digest,index);
                this.RFC7748(s);
                dr=this.H2(ph,context,SIG,Q,M);
                var sd=dr.mod(q);
                this.encode_int(ctx.BIG.modadd(sr,ctx.BIG.modmul(s,sd,q),q),S);
                for (var i=0;i<b;i++)
                    SIG[b+i]=S[i];
            }
            return res;
        },

// verify a signature using public key Q
// same context (if any) as used for signature
        VERIFY: function(ph,Q,context,M,SIG) {
            var lg=0;
            var b,index=0;
            if (8*ctx.BIG.MODBYTES==ctx.FP.MODBITS) index=1; // extra byte needed for compression        
            b=ctx.BIG.MODBYTES+index;
            var S=[];

            var c=ctx.ROM_CURVE.CURVE_Cof_I;
            while (c!=1) {
                lg++;
                c/=2;
            }
            var q = new ctx.BIG();
            q.rcopy(ctx.ROM_CURVE.CURVE_Order);
            var R=this.decode(SIG);
            if (R.is_infinity()) return false;
            for (var i=0;i<b;i++)
                S[i]=SIG[b+i];
            var t=this.decode_int(false,S);
            if (ctx.BIG.comp(t,q)>=0) return false;
            var du=this.H2(ph,context,SIG,Q,M);
            var su=du.mod(q);

            var G=ctx.ECP.generator();
            var QD=this.decode(Q); 
            if (QD.is_infinity()) return false;
            QD.neg();
            for (var i=0;i<lg;i++)
            { // use cofactor 2^c
                G.dbl(); QD.dbl(); R.dbl();
            }
        
            if (!G.mul2(t,QD,su).equals(R)) return false;
            return true;
        }

    };

    return EDDSA;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports = {
        EDDSA: EDDSA
    };
}

