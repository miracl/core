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

/* Finite Field arithmetic */
/* CORE mod p functions */

var FP = function(ctx) {
    "use strict";

    /* General purpose Constructor */
    var FP = function(x) {
        if (x instanceof FP) {
            this.f = new ctx.BIG(x.f);
            this.XES = x.XES;
        } else {
            if (x instanceof ctx.BIG) {
                this.f = new ctx.BIG(x);
            } else {
                if (x<0)
                {
                    var m = new ctx.BIG(0);
                    m.rcopy(ctx.ROM_FIELD.Modulus);
                    m.inc(x); m.norm();
                    this.f = new ctx.BIG(m);
                } else {
                    this.f = new ctx.BIG(x);
                }
            }
			this.XES = 1;
			//if (x!=0) 
			if (!this.f.iszilch())
				this.nres();
        }
    };

    FP.NOT_SPECIAL = 0;
    FP.PSEUDO_MERSENNE = 1;
    FP.GENERALISED_MERSENNE = 2;
    FP.MONTGOMERY_FRIENDLY = 3;

	FP.ZERO = 0;
	FP.ONE = 1;
	FP.SPARSEST = 2;
	FP.SPARSER = 3;
	FP.SPARSE = 4;
	FP.DENSE= 5;

	FP.NEGATOWER = 0;
	FP.POSITOWER = 1;

    FP.MODBITS = ctx.config["@NBT"];
    FP.PM1D2 = ctx.config["@M8"];
    FP.RIADZ = ctx.config["@RZ"];
    FP.RIADZG2A = ctx.config["@RZ2A"];
    FP.RIADZG2B = ctx.config["@RZ2B"];
    FP.MODTYPE = ctx.config["@MT"];
	FP.QNRI = ctx.config["@QI"];
	FP.TOWER = ctx.config["@TW"];

    FP.FEXCESS = ((1 << ctx.config["@SH"])-1); // 2^(BASEBITS*NLEN-MODBITS)-1
    FP.OMASK = (-1) << FP.TBITS;
    FP.TBITS = FP.MODBITS % ctx.BIG.BASEBITS;
    FP.TMASK = (1 << FP.TBITS) - 1;

    FP.BIG_ENDIAN_SIGN = false;

    FP.prototype = {
        /* set this=0 */
        zero: function() {
            this.XES = 1;
            this.f.zero();
        },

        /* copy from a ctx.BIG in ROM */
        rcopy: function(y) {
            this.f.rcopy(y);
            this.nres();
        },

        /* copy from another ctx.BIG */
        bcopy: function(y) {
            this.f.copy(y);
            this.nres();
        },

        /* copy from another FP */
        copy: function(y) {
            this.XES = y.XES;
            this.f.copy(y.f);
        },

        /* conditional swap of a and b depending on d */
        cswap: function(b, d) {
            this.f.cswap(b.f, d);
            var t, c = d;
            c = ~(c - 1);
            t = c & (this.XES ^ b.XES);
            this.XES ^= t;
            b.XES ^= t;
        },

        /* conditional copy of b to a depending on d */
        cmove: function(b, d) {
            var c = d;

            c = ~(c - 1);

            this.f.cmove(b.f, d);
            this.XES ^= (this.XES ^ b.XES) & c;
        },

        /* convert to Montgomery n-residue form */
        nres: function() {
            var r, d;

            if (FP.MODTYPE != FP.PSEUDO_MERSENNE && FP.MODTYPE != FP.GENERALISED_MERSENNE) {
                r = new ctx.BIG();
                r.rcopy(ctx.ROM_FIELD.R2modp);

                d = ctx.BIG.mul(this.f, r);
                this.f.copy(FP.mod(d));
                this.XES = 2;
            } else {
                var m = new ctx.BIG(0);
                m.rcopy(ctx.ROM_FIELD.Modulus);
                this.f.mod(m);
                this.XES = 1;
            }

            return this;
        },

        /* convert back to regular form */
        redc: function() {
            var r = new ctx.BIG(0),
                d, w;

            r.copy(this.f);

            if (FP.MODTYPE != FP.PSEUDO_MERSENNE && FP.MODTYPE != FP.GENERALISED_MERSENNE) {
                d = new ctx.DBIG(0);
                d.hcopy(this.f);
                w = FP.mod(d);
                r.copy(w);
            }

            return r;
        },

        /* convert this to string */
        toString: function() {
            var s = this.redc().toString();
            return s;
        },

        /* test this=0 */
        iszilch: function() {
			var c=new FP(0); c.copy(this);
            c.reduce();
            return c.f.iszilch();
        },

        islarger: function() {
            if (this.iszilch()) return;
            var sx = new ctx.BIG(0);
            sx.rcopy(ctx.ROM_FIELD.Modulus);
            var fx = this.redc();
            sx.sub(fx); sx.norm();
            return ctx.BIG.comp(fx,sx);
        },

        toBytes: function(b) {
            this.redc().toBytes(b)
        },

        /* test this=1 */
        isunity: function() {
			var c=new FP(0); c.copy(this);
            c.reduce();
            return c.redc().isunity();
        },

        sign: function () {
            if (FP.BIG_ENDIAN_SIGN)
            {
                var m = new ctx.BIG(0);
                m.rcopy(ctx.ROM_FIELD.Modulus);
                m.dec(1);
                m.fshr(1);
                var n = new FP(0); n.copy(this);
                n.reduce();
                var w=n.redc();
                var cp=ctx.BIG.comp(w,m);
                return ((cp+1)&2)>>1;
            } else {
			    var c=new FP(0); c.copy(this);
                c.reduce();
                return c.redc().parity();
            }
        },

        /* reduce this mod Modulus */
        reduce: function() {
            var q,carry,sr,sb,m = new ctx.BIG(0);
            m.rcopy(ctx.ROM_FIELD.Modulus);
			var r = new ctx.BIG(0);
            r.rcopy(ctx.ROM_FIELD.Modulus);
			this.f.norm();

			if (this.XES>16)
			{
				q=FP.quo(this.f,m);
				carry=r.pmul(q);
				r.w[ctx.BIG.NLEN-1]+=(carry<<ctx.BIG.BASEBITS); // correction - put any carry out back in again
				this.f.sub(r);
				this.f.norm();
				sb=2;
			}
			else {
					sb=FP.logb2(this.XES-1);
			}
			m.fshl(sb);

			while (sb>0)
			{
// constant time...
				sr=ctx.BIG.ssn(r,this.f,m);  // optimized combined shift, subtract and norm
				this.f.cmove(r,1-sr);
				sb--;
			}			

            this.XES = 1;
        },

        /* set this=1 */
        one: function() {
            this.f.one();
            this.nres();
        },

        /* normalise this */
        norm: function() {
            return this.f.norm();
        },

        /* this*=b mod Modulus */
        mul: function(b) {
            var d;

            if (this.XES * b.XES > FP.FEXCESS) {
                this.reduce();
            }

            d = ctx.BIG.mul(this.f, b.f);
            this.f.copy(FP.mod(d));
            this.XES = 2;

            return this;
        },

        /* this*=c mod Modulus where c is an int */
        imul: function(c) {
            var s = false,
                d, n;

            if (c < 0) {
                c = -c;
                s = true;
            }

            if (FP.MODTYPE == FP.PSEUDO_MERSENNE || FP.MODTYPE == FP.GENERALISED_MERSENNE) {
                d = this.f.pxmul(c);
                this.f.copy(FP.mod(d));
                this.XES = 2;
            } else {
                if (this.XES * c <= FP.FEXCESS) {
                    this.f.pmul(c);
                    this.XES *= c;
                } else {
                    n = new FP(c);
                    this.mul(n);
                }
            }

            if (s) {
                this.neg();
                this.norm();
            }
            return this;
        },

        /* this*=this mod Modulus */
        sqr: function() {
            var d, t;

            if (this.XES * this.XES > FP.FEXCESS) {
                this.reduce();
            }

            d = ctx.BIG.sqr(this.f);
            t = FP.mod(d);
            this.f.copy(t);
            this.XES = 2;

            return this;
        },

        /* this+=b */
        add: function(b) {
            this.f.add(b.f);
            this.XES += b.XES;

            if (this.XES > FP.FEXCESS) {
                this.reduce();
            }

            return this;
        },
        /* this=-this mod Modulus */
        neg: function() {
            var m = new ctx.BIG(0),
                sb;

            m.rcopy(ctx.ROM_FIELD.Modulus);

            sb = FP.logb2(this.XES - 1);

            m.fshl(sb);
            this.XES = (1 << sb)+1;
            this.f.rsub(m);

            if (this.XES > FP.FEXCESS) {
                this.reduce();
            }

            return this;
        },

        /* this-=b */
        sub: function(b) {
            var n = new FP(0);

            n.copy(b);
            n.neg();
            this.add(n);

            return this;
        },

        rsub: function(b) {
            var n = new FP(0);

            n.copy(this);
            n.neg();
            this.copy(b);
            this.add(n);
        },

        /* this/=2 mod Modulus */
        div2: function() {
            var p;
            p = new ctx.BIG(0);
            p.rcopy(ctx.ROM_FIELD.Modulus);
            var pr=this.f.parity();
            var w = new ctx.BIG(0); w.copy(this.f);
            this.f.fshr(1);
            w.add(p); w.norm();
            w.fshr(1);
            this.f.cmove(w,pr);
            return this;
        },

        /* return jacobi symbol (this/Modulus) */
        jacobi: function() {
            var p = new ctx.BIG(0),
                w = this.redc();

            p.rcopy(ctx.ROM_FIELD.Modulus);

            return w.jacobi(p);
        },

        /* return TRUE if this==a */
        equals: function(a) {
			var ft=new FP(0); ft.copy(this);
			var sd=new FP(0); sd.copy(a);
            ft.reduce();
            sd.reduce();

            if (ctx.BIG.comp(ft.f, sd.f) === 0) {
                return true;
            }

            return false;
        },

        /* return this^e mod Modulus */
        pow: function(e) {
            var i,w=[],
                tb=[],
                t=new ctx.BIG(e),
                nb, lsbs, r;
			this.norm();
            t.norm();
            nb= 1 + Math.floor((t.nbits() + 3) / 4);

            for (i=0;i<nb;i++) {
                lsbs=t.lastbits(4);
                t.dec(lsbs);
                t.norm();
                w[i]=lsbs;
                t.fshr(4);
            }
            tb[0]=new FP(1);
            tb[1]=new FP(this);
            for (i=2;i<16;i++) {
                tb[i]=new FP(tb[i-1]);
                tb[i].mul(this);
            }
            r=new FP(tb[w[nb-1]]);
            for (i=nb-2;i>=0;i--) {
                r.sqr();
                r.sqr();
                r.sqr();
                r.sqr();
                r.mul(tb[w[i]]);
            }
            r.reduce();
            return r;
        },

// return this^(p-3)/4 or this^(p-5)/8
// See https://eprint.iacr.org/2018/1038
		fpow: function() {
			var i,j,k,bw,w,c,nw,lo,m,n,nd,e;
			var xp=[];
			var ac=[1,2,3,6,12,15,30,60,120,240,255];
// phase 1
			
			xp[0]=new FP(this);	// 1 
			xp[1]=new FP(this); xp[1].sqr(); // 2
			xp[2]=new FP(xp[1]); xp[2].mul(this);  //3
			xp[3]=new FP(xp[2]); xp[3].sqr();  // 6 
			xp[4]=new FP(xp[3]); xp[4].sqr();  // 12
			xp[5]=new FP(xp[4]); xp[5].mul(xp[2]);  // 15
			xp[6]=new FP(xp[5]); xp[6].sqr();  // 30
			xp[7]=new FP(xp[6]); xp[7].sqr();  // 60
			xp[8]=new FP(xp[7]); xp[8].sqr();  // 120
			xp[9]=new FP(xp[8]); xp[9].sqr();  // 240
			xp[10]=new FP(xp[9]); xp[10].mul(xp[5]);  // 255		
			

			n=FP.MODBITS;
			if (FP.MODTYPE == FP.GENERALISED_MERSENNE)   // Goldilocks ONLY
				n/=2;

            e=FP.PM1D2;

            n-=(e+1);
            c=(ctx.ROM_FIELD.MConst+(1<<e)+1)/(1<<(e+1));

            nd=0;
            while (c%2==0)
            {
                c/=2;
                n-=1;
                nd++;
            }

			bw=0; w=1; while (w<c) {w*=2; bw+=1;}
			k=w-c;

			i=10; var key=new FP(0);
			if (k!=0)
			{
				while (ac[i]>k) i--;
				key.copy(xp[i]); 
				k-=ac[i];
			}
			while (k!=0)
			{
				i--;
				if (ac[i]>k) continue;
				key.mul(xp[i]);
				k-=ac[i]; 
			}

// phase 2 
			xp[1].copy(xp[2]);
			xp[2].copy(xp[5]);
			xp[3].copy(xp[10]);
	
			j=3; m=8;
			nw=n-bw;
			var t=new FP(0);
			while (2*m<nw)
			{
				t.copy(xp[j++]);
				for (i=0;i<m;i++)
					t.sqr(); 
				xp[j].copy(xp[j-1]);
				xp[j].mul(t);
				m*=2;
			}
			lo=nw-m;
			var r=new FP(xp[j]);

			while (lo!=0)
			{
				m/=2; j--;
				if (lo<m) continue;
				lo-=m;
				t.copy(r);
				for (i=0;i<m;i++)
					t.sqr();
				r.copy(t);
				r.mul(xp[j]);
			}

// phase 3
			if (bw!=0)
			{
				for (i=0;i<bw;i++ )
					r.sqr();
				r.mul(key); 
			}

			if (FP.MODTYPE == FP.GENERALISED_MERSENNE)   // Goldilocks ONLY
			{
				key.copy(r);
				r.sqr();
				r.mul(this);
				for (i=0;i<n+1;i++)
					r.sqr();
				r.mul(key);
			}

            for (i=0;i<nd;i++)
                r.sqr();

			return r;
		},

// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
        progen: function() {
            if (FP.MODTYPE == FP.PSEUDO_MERSENNE || FP.MODTYPE == FP.GENERALISED_MERSENNE) 
            {
                this.copy(this.fpow());
                return;
            }
            var e=FP.PM1D2;
            var m=new ctx.BIG(0);
			m.rcopy(ctx.ROM_FIELD.Modulus);
            m.dec(1);
            m.shr(e);
            m.dec(1);
            m.fshr(1);
        
            this.copy(this.pow(m));
        },


    /* this=1/this mod Modulus */
        inverse: function(h) {
            var e=FP.PM1D2;
            this.norm();
            var s=new FP(this);
            for (var i=0;i<e-1;i++)
            {
                s.sqr();
                s.mul(this);
            }
            if (h==null)
                this.progen();
            else
                this.copy(h);
           
            for (var i=0;i<=e;i++)
                this.sqr();
            this.mul(s);
            this.reduce();
            return this;
        }, 

// Calculate both inverse and square root of x, return QR
        invsqrt: function(i,s) {
            var h=new FP(0);
            var qr=this.qr(h);
            s.copy(this.sqrt(h));
            i.copy(this);
            i.inverse(h);
            return qr;
        },

    /* test for Quadratic residue */
        qr: function (h)  {
            var r=new FP(this);
            var e=FP.PM1D2;
            r.progen();
            if (h!=null)
                h.copy(r);

            r.sqr();
            r.mul(this);
            for (var i=0;i<e-1;i++)
                r.sqr();

            return r.isunity()?1:0;
        },

    /* return sqrt(this) mod Modulus */
        sqrt: function(h) {
            var e=FP.PM1D2;
            var g=new FP(this);
            if (h==null)
                g.progen();
            else
                g.copy(h);

            var m=new ctx.BIG(0);
			m.rcopy(ctx.ROM_FIELD.ROI);
            
            var v=new FP(m);

            var t=new FP(g);
            t.sqr();
            t.mul(this);

            var r=new FP(this);
            r.mul(g);
            var b=new FP(t);
       
            for (var k=e;k>1;k--)
            {
                for (var j=1;j<k-1;j++)
                    b.sqr();
                var u=b.isunity()? 0:1;
                g.copy(r); g.mul(v);
                r.cmove(g,u);
                v.sqr();
                g.copy(t); g.mul(v);
                t.cmove(g,u);
                b.copy(t);
            }
            var sgn=r.sign();
            var nr=new FP(r);
            nr.neg(); nr.norm();
            r.cmove(nr,sgn);
            return r;
        }


    };

// Two for the price of One  - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
    FP.tpo = function(i,s) {
        var w = new FP(s);
        var t = new FP(i);
        w.mul(i);
        t.mul(w);
        var qr=t.invsqrt(i,s);
        i.mul(w);
        s.mul(i);
        return qr;
    };

    FP.fromBytes = function(b) {
        var t=ctx.BIG.fromBytes(b);
        return new FP(t);
    };

    FP.rand = function(rng) {
        var m = new ctx.BIG(0);
        m.rcopy(ctx.ROM_FIELD.Modulus);    
        var w=ctx.BIG.randomnum(m,rng);
        return new FP(w);
    };

    FP.logb2 = function(v) {
        var r;

        v |= v >>> 1;
        v |= v >>> 2;
        v |= v >>> 4;
        v |= v >>> 8;
        v |= v >>> 16;

        v = v - ((v >>> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >>> 2) & 0x33333333);
        r = ((v + (v >>> 4) & 0xF0F0F0F) * 0x1010101) >>> 24;

        return r;
    };

	FP.quo = function(n,m) {
		var num,den,hb=ctx.BIG.CHUNK>>1;
		if (FP.TBITS<hb)
		{
			var sh=hb-FP.TBITS;
			num=(n.w[ctx.BIG.NLEN-1]<<sh)|(n.w[ctx.BIG.NLEN-2]>>(ctx.BIG.BASEBITS-sh));
			den=(m.w[ctx.BIG.NLEN-1]<<sh)|(m.w[ctx.BIG.NLEN-2]>>(ctx.BIG.BASEBITS-sh));
		} else {
			num=n.w[ctx.BIG.NLEN-1];
			den=m.w[ctx.BIG.NLEN-1];			
		}
		return Math.floor(num/(den+1))
	};

    /* reduce a ctx.DBIG to a ctx.BIG using a "special" modulus */
    FP.mod = function(d) {
        var b = new ctx.BIG(0),
            i, t, v, tw, tt, lo, carry, m, dd;

        if (FP.MODTYPE == FP.PSEUDO_MERSENNE) {
            t = d.split(FP.MODBITS);
            b.hcopy(d);

            if (ctx.ROM_FIELD.MConst != 1) {
                v = t.pmul(ctx.ROM_FIELD.MConst);
            } else {
                v = 0;
            }

            t.add(b);
            t.norm();

            tw = t.w[ctx.BIG.NLEN - 1];
            t.w[ctx.BIG.NLEN - 1] &= FP.TMASK;
            t.inc(ctx.ROM_FIELD.MConst * ((tw >> FP.TBITS) + (v << (ctx.BIG.BASEBITS - FP.TBITS))));
            //      b.add(t);
            t.norm();

            return t;
        }

        if (FP.MODTYPE == FP.MONTGOMERY_FRIENDLY) {
            for (i = 0; i < ctx.BIG.NLEN; i++) {
                d.w[ctx.BIG.NLEN + i] += d.muladd(d.w[i], ctx.ROM_FIELD.MConst - 1, d.w[i], ctx.BIG.NLEN + i - 1);
            }

            for (i = 0; i < ctx.BIG.NLEN; i++) {
                b.w[i] = d.w[ctx.BIG.NLEN + i];
            }

            b.norm();
        }

        if (FP.MODTYPE == FP.GENERALISED_MERSENNE) { // GoldiLocks Only
            t = d.split(FP.MODBITS);
            b.hcopy(d);
            b.add(t);
            dd = new ctx.DBIG(0);
            dd.hcopy(t);
            dd.shl(FP.MODBITS / 2);

            tt = dd.split(FP.MODBITS);
            lo = new ctx.BIG();
            lo.hcopy(dd);

            b.add(tt);
            b.add(lo);
            //b.norm();
            tt.shl(FP.MODBITS / 2);
            b.add(tt);

            carry = b.w[ctx.BIG.NLEN - 1] >> FP.TBITS;
            b.w[ctx.BIG.NLEN - 1] &= FP.TMASK;
            b.w[0] += carry;

            b.w[Math.floor(224 / ctx.BIG.BASEBITS)] += carry << (224 % ctx.BIG.BASEBITS);
            b.norm();
        }

        if (FP.MODTYPE == FP.NOT_SPECIAL) {
            m = new ctx.BIG(0);
            m.rcopy(ctx.ROM_FIELD.Modulus);

            b.copy(ctx.BIG.monty(m, ctx.ROM_FIELD.MConst, d));
        }

        return b;
    };

    return FP;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports = {
        FP: FP
    };
}

