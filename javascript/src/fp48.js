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

/* CORE Fp^48 functions */

/* FP48 elements are of the form a+i.b+i^2.c */

var FP48 = function(ctx) {
    "use strict";

    /* general purpose constructor */
    var FP48 = function(d, e, f) {
/*
		if (!isNaN(d))
		{
			this.a = new ctx.FP16(d);
			this.b = new ctx.FP16(0);
			this.c = new ctx.FP16(0);
			if (d==1) this.stype=ctx.FP.ONE;
			else this.stype=ctx.FP.SPARSEST;
		}
		else
		{
	        if (d instanceof FP48) {
		        this.a = new ctx.FP16(d.a);
			    this.b = new ctx.FP16(d.b);
				this.c = new ctx.FP16(d.c);
			} else {
				this.a = new ctx.FP16(d);
				this.b = new ctx.FP16(e);
				this.c = new ctx.FP16(f);
			}
			this.stype=ctx.FP.DENSE;
		}
*/

       if (d instanceof FP48) {
            // ignore e, d, which are assumed be undefined in this case
            this.a = new ctx.FP16(d.a);
            this.b = new ctx.FP16(d.b);
            this.c = new ctx.FP16(d.c);
            this.stype=ctx.FP.DENSE;
        } else if (typeof d !== "undefined" && typeof e !== "undefined" && typeof f !== "undefined") {
            // all 3 components set to (can be anything that the FP16 constructor supports)
            this.a = new ctx.FP16(d);
            this.b = new ctx.FP16(e);
            this.c = new ctx.FP16(f);
            this.stype=ctx.FP.DENSE;
        } else if (typeof d === "number") {
            // first component is number
            this.a = new ctx.FP16(d);
            this.b = new ctx.FP16(0);
            this.c = new ctx.FP16(0);
            if (d==1) this.stype=ctx.FP.ONE;
            else this.stype=ctx.FP.SPARSER;
        } else {
            // other cases, including `new ctx.FP48()` fall back to zero
            this.a = new ctx.FP16(0);
            this.b = new ctx.FP16(0);
            this.c = new ctx.FP16(0);
            this.stype=ctx.FP.ZERO;
        }

    };

    FP48.prototype = {
        /* reduce all components of this mod Modulus */
        reduce: function() {
            this.a.reduce();
            this.b.reduce();
            this.c.reduce();
        },

        /* normalize all components of this mod Modulus */
        norm: function() {
            this.a.norm();
            this.b.norm();
            this.c.norm();
        },

        /* test x==0 ? */
        iszilch: function() {
            return (this.a.iszilch() && this.b.iszilch() && this.c.iszilch());
        },

        /* test x==1 ? */
        isunity: function() {
            var one = new ctx.FP16(1);
            return (this.a.equals(one) && this.b.iszilch() && this.c.iszilch());
        },

        /* conditional copy of g to this depending on d */
        cmove: function(g, d) {
            this.a.cmove(g.a, d);
            this.b.cmove(g.b, d);
            this.c.cmove(g.c, d);
			d=~(d-1);
			this.stype^=(this.stype^g.stype)&d;

        },

        /* Constant time select from pre-computed table */
        select: function(g, b) {
            var invf = new FP48(0),
                m, babs;

            m = b >> 31;
            babs = (b ^ m) - m;
            babs = (babs - 1) / 2;

            this.cmove(g[0], FP48.teq(babs, 0)); // conditional move
            this.cmove(g[1], FP48.teq(babs, 1));
            this.cmove(g[2], FP48.teq(babs, 2));
            this.cmove(g[3], FP48.teq(babs, 3));
            this.cmove(g[4], FP48.teq(babs, 4));
            this.cmove(g[5], FP48.teq(babs, 5));
            this.cmove(g[6], FP48.teq(babs, 6));
            this.cmove(g[7], FP48.teq(babs, 7));

            invf.copy(this);
            invf.conj();
            this.cmove(invf, (m & 1));
        },

		settype: function(w) {
			this.stype=w;
		},

		gettype: function() {
			return this.stype;
		},

        /* extract a from this */
        geta: function() {
            return this.a;
        },

        /* extract b */
        getb: function() {
            return this.b;
        },

        /* extract c */
        getc: function() {
            return this.c;
        },

        /* return 1 if x==y, else 0 */
        equals: function(x) {
            return (this.a.equals(x.a) && this.b.equals(x.b) && this.c.equals(x.c));
        },

        /* copy this=x */
        copy: function(x) {
            this.a.copy(x.a);
            this.b.copy(x.b);
            this.c.copy(x.c);
			this.stype=x.stype;
        },

        /* set this=1 */
        one: function() {
            this.a.one();
            this.b.zero();
            this.c.zero();
			this.stype=ctx.FP.ONE;
        },

        /* set this=0 */
        zero: function() {
            this.a.zero();
            this.b.zero();
            this.c.zero();
			this.stype=ctx.FP.ZERO;
        },

        /* this=conj(this) */
        conj: function() {
            this.a.conj();
            this.b.nconj();
            this.c.conj();
        },

        /* set this from 3 FP16s */
        set: function(d, e, f) {
            this.a.copy(d);
            this.b.copy(e);
            this.c.copy(f);
			this.stype=ctx.FP.DENSE;
        },

        /* set this from one ctx.FP16 */
        seta: function(d) {
            this.a.copy(d);
            this.b.zero();
            this.c.zero();
			this.stype=ctx.FP.SPARSEST
        },

        /* Granger-Scott Unitary Squaring */
        usqr: function() {
            var A = new ctx.FP16(this.a),
                B = new ctx.FP16(this.c),
                C = new ctx.FP16(this.b),
                D = new ctx.FP16(0);

            this.a.sqr();
            D.copy(this.a);
            D.add(this.a);
            this.a.add(D);

            A.nconj();

            A.add(A);
            this.a.add(A);
            B.sqr();
            B.times_i();

            D.copy(B);
            D.add(B);
            B.add(D);

            C.sqr();
            D.copy(C);
            D.add(C);
            C.add(D);

            this.b.conj();
            this.b.add(this.b);
            this.c.nconj();

            this.c.add(this.c);
            this.b.add(B);
            this.c.add(C);
			this.stype=ctx.FP.DENSE;
            this.reduce();
        },

        /* Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */
        sqr: function() {
			if (this.stype==ctx.FP.ONE)
				return;

            var A = new ctx.FP16(this.a),
                B = new ctx.FP16(this.b),
                C = new ctx.FP16(this.c),
                D = new ctx.FP16(this.a);

            A.sqr();
            B.mul(this.c);
            B.add(B);
            C.sqr();
            D.mul(this.b);
            D.add(D);

            this.c.add(this.a);
            this.c.add(this.b);
            this.c.norm();
            this.c.sqr();

            this.a.copy(A);

            A.add(B);
            A.add(C);
            A.add(D);
            A.neg();
            B.times_i();
            C.times_i();

            this.a.add(B);
            this.b.copy(C);
            this.b.add(D);
            this.c.add(A);
			if (this.stype==ctx.FP.SPARSER || this.stype==ctx.FP.SPARSEST)
				this.stype=ctx.FP.SPARSE;
			else
				this.stype=ctx.FP.DENSE;
            this.norm();
        },

        /* FP48 full multiplication this=this*y */
        mul: function(y) {
            var z0 = new ctx.FP16(this.a),
                z1 = new ctx.FP16(0),
                z2 = new ctx.FP16(this.b),
                z3 = new ctx.FP16(0),
                t0 = new ctx.FP16(this.a),
                t1 = new ctx.FP16(y.a);

            z0.mul(y.a);
            z2.mul(y.b);

            t0.add(this.b);
            t1.add(y.b);

            t0.norm();
            t1.norm();

            z1.copy(t0);
            z1.mul(t1);
            t0.copy(this.b);
            t0.add(this.c);

            t1.copy(y.b);
            t1.add(y.c);

            t0.norm();
            t1.norm();
            z3.copy(t0);
            z3.mul(t1);

            t0.copy(z0);
            t0.neg();
            t1.copy(z2);
            t1.neg();

            z1.add(t0);
            this.b.copy(z1);
            this.b.add(t1);

            z3.add(t1);
            z2.add(t0);

            t0.copy(this.a);
            t0.add(this.c);
            t1.copy(y.a);
            t1.add(y.c);

            t0.norm();
            t1.norm();

            t0.mul(t1);
            z2.add(t0);

            t0.copy(this.c);
            t0.mul(y.c);
            t1.copy(t0);
            t1.neg();

            this.c.copy(z2);
            this.c.add(t1);
            z3.add(t1);
            t0.times_i();
            this.b.add(t0);

            z3.times_i();
            this.a.copy(z0);
            this.a.add(z3);
			this.stype=ctx.FP.DENSE;
            this.norm();
        },

/* FP48 multiplication w=w*y */
/* catering for special case that arises from special form of ATE pairing line function */
/* w and y are both sparser line functions - cost = 6m */
		smul: function(y) {
			if (ctx.ECP.SEXTIC_TWIST==ctx.ECP.D_TYPE)
			{
				var w1=new ctx.FP8(this.a.geta());
				var w2=new ctx.FP8(this.a.getb());
				var w3;

				w1.mul(y.a.geta());
				w2.mul(y.a.getb());
				if (y.stype==ctx.FP.SPARSEST || this.stype==ctx.FP.SPARSEST)
				{
					if (y.stype==ctx.FP.SPARSEST && this.stype==ctx.FP.SPARSEST)
					{
						var t=new ctx.FP(this.b.geta().geta().geta().geta());
						t.mul(y.b.geta().geta().geta().geta());
						w3=new ctx.FP8(t);
					} else {
						if (y.stype!=ctx.FP.SPARSEST)
						{
							w3=new ctx.FP8(y.b.geta());
							w3.tmul(this.b.geta().geta().geta().geta());
						} else {
							w3=new ctx.FP8(this.b.geta());
							w3.tmul(y.b.geta().geta().geta().geta());
						}
					}
				} else {
					w3=new ctx.FP8(this.b.geta());
					w3.mul(y.b.geta());
				}


				var ta=new ctx.FP8(this.a.geta());
				var tb=new ctx.FP8(y.a.geta());
				ta.add(this.a.getb()); ta.norm();
				tb.add(y.a.getb()); tb.norm();
				var tc=new ctx.FP8(ta);
				tc.mul(tb);
				var t=new ctx.FP8(w1);
				t.add(w2);
				t.neg();
				tc.add(t);

				ta.copy(this.a.geta()); ta.add(this.b.geta()); ta.norm();
				tb.copy(y.a.geta()); tb.add(y.b.geta()); tb.norm();
				var td=new ctx.FP8(ta);
				td.mul(tb);
				t.copy(w1);
				t.add(w3);
				t.neg();
				td.add(t);

				ta.copy(this.a.getb()); ta.add(this.b.geta()); ta.norm();
				tb.copy(y.a.getb()); tb.add(y.b.geta()); tb.norm();
				var te=new ctx.FP8(ta);
				te.mul(tb);
				t.copy(w2);
				t.add(w3);
				t.neg();
				te.add(t);

				w2.times_i();
				w1.add(w2);

				this.a.geta().copy(w1); this.a.getb().copy(tc);
				this.b.geta().copy(td); this.b.getb().copy(te);
				this.c.geta().copy(w3); this.c.getb().zero();

				this.a.norm();
				this.b.norm();

			} else {
				var w1=new ctx.FP8(this.a.geta());
				var w2=new ctx.FP8(this.a.getb());
				var w3;

				w1.mul(y.a.geta());
				w2.mul(y.a.getb());
				if (y.stype==ctx.FP.SPARSEST || this.stype==ctx.FP.SPARSEST)
				{
					if (y.stype==ctx.FP.SPARSEST && this.stype==ctx.FP.SPARSEST)
					{
						var t=new ctx.FP(this.c.getb().geta().geta().geta());
						t.mul(y.c.getb().geta().geta().geta());
						w3=new ctx.FP8(t);
					} else {
						if (y.type!=ctx.FP.SPARSEST)
						{
							w3=new ctx.FP8(y.c.getb());
							w3.tmul(this.c.getb().geta().geta().geta());
						} else {
							w3=new ctx.FP8(this.c.getb());
							w3.tmul(y.c.getb().geta().geta().geta());
						}
					}
				} else {
					w3=new ctx.FP8(this.c.getb());
					w3.mul(y.c.getb());
				}
				var ta=new ctx.FP8(this.a.geta());
				var tb=new ctx.FP8(y.a.geta());
				ta.add(this.a.getb()); ta.norm();
				tb.add(y.a.getb()); tb.norm();
				var tc=new ctx.FP8(ta);
				tc.mul(tb);
				var t=new ctx.FP8(w1);
				t.add(w2);
				t.neg();
				tc.add(t);

				ta.copy(this.a.geta()); ta.add(this.c.getb()); ta.norm();
				tb.copy(y.a.geta()); tb.add(y.c.getb()); tb.norm();
				var td=new ctx.FP8(ta);
				td.mul(tb);
				t.copy(w1);
				t.add(w3);
				t.neg();
				td.add(t);

				ta.copy(this.a.getb()); ta.add(this.c.getb()); ta.norm();
				tb.copy(y.a.getb()); tb.add(y.c.getb()); tb.norm();
				var te=new ctx.FP8(ta);
				te.mul(tb);
				t.copy(w2);
				t.add(w3);
				t.neg();
				te.add(t);

				w2.times_i();
				w1.add(w2);
				this.a.geta().copy(w1); this.a.getb().copy(tc);

				w3.times_i();
				w3.norm();
				this.b.geta().zero(); this.b.getb().copy(w3);

				te.norm();
				te.times_i();
				this.c.geta().copy(te);
				this.c.getb().copy(td);

				this.a.norm();
				this.c.norm();

			}
			this.stype=ctx.FP.SPARSE;
		},

/* FP48 full multiplication w=w*y */
/* Supports sparse multiplicands */
/* Usually w is denser than y */
		ssmul: function(y) {
			if (this.stype==ctx.FP.ONE)
			{
				this.copy(y);
				return;
			}
			if (y.stype==ctx.FP.ONE)
				return;

			if (y.stype>=ctx.FP.SPARSE)
			{
				var z0=new ctx.FP16(this.a);
				var z1=new ctx.FP16(0);
				var z2=new ctx.FP16(0);
				var z3=new ctx.FP16(0);
				z0.mul(y.a);

				if (ctx.ECP.SEXTIC_TWIST==ctx.ECP.M_TYPE)
				{
					if (y.stype==ctx.FP.SPARSE || this.stype==ctx.FP.SPARSE)
					{
						z2.getb().copy(this.b.getb());
						z2.getb().mul(y.b.getb());
						z2.geta().zero();
						if (y.stype!=ctx.FP.SPARSE)
						{
							z2.geta().copy(this.b.getb());
							z2.geta().mul(y.b.geta());
						}
						if (this.stype!=ctx.FP.SPARSE)
						{
							z2.geta().copy(this.b.geta());
							z2.geta().mul(y.b.getb());
						}
						z2.times_i();
					} else {
						z2.copy(this.b);
						z2.mul(y.b);
					}
				} else {
					z2.copy(this.b);
					z2.mul(y.b);
				}
				var t0=new ctx.FP16(this.a);
				var t1=new ctx.FP16(y.a);
				t0.add(this.b); t0.norm();
				t1.add(y.b); t1.norm();

				z1.copy(t0); z1.mul(t1);
				t0.copy(this.b); t0.add(this.c); t0.norm();
				t1.copy(y.b); t1.add(y.c); t1.norm();

				z3.copy(t0); z3.mul(t1);

				t0.copy(z0); t0.neg();
				t1.copy(z2); t1.neg();

				z1.add(t0);
				this.b.copy(z1); this.b.add(t1);

				z3.add(t1);
				z2.add(t0);

				t0.copy(this.a); t0.add(this.c); t0.norm();
				t1.copy(y.a); t1.add(y.c); t1.norm();

				t0.mul(t1);
				z2.add(t0);

				if (ctx.ECP.SEXTIC_TWIST==ctx.ECP.D_TYPE)
				{
					if (y.stype==ctx.FP.SPARSE || this.stype==ctx.FP.SPARSE)
					{
						t0.geta().copy(this.c.geta());
						t0.geta().mul(y.c.geta());
						t0.getb().zero();
						if (y.stype!=ctx.FP.SPARSE)
						{
							t0.getb().copy(this.c.geta());
							t0.getb().mul(y.c.getb());
						}
						if (this.stype!=ctx.FP.SPARSE)
						{
							t0.getb().copy(this.c.getb());
							t0.getb().mul(y.c.geta());
						}
					} else {
						t0.copy(this.c);
						t0.mul(y.c);
					}
				} else {
					t0.copy(this.c);
					t0.mul(y.c);
				}
				t1.copy(t0); t1.neg();

				this.c.copy(z2); this.c.add(t1);
				z3.add(t1);
				t0.times_i();
				this.b.add(t0);
				z3.norm();
				z3.times_i();
				this.a.copy(z0); this.a.add(z3);
			} else {
				if (this.stype==ctx.FP.SPARSER || this.stype==ctx.FP.SPARSEST)
				{
					this.smul(y);
					return;
				}
				if (ctx.ECP.SEXTIC_TWIST==ctx.ECP.D_TYPE)
				{ // dense by sparser - 13m
					var z0=new ctx.FP16(this.a);
					var z2=new ctx.FP16(this.b);
					var z3=new ctx.FP16(this.b);
					var t0=new ctx.FP16(0);
					var t1=new ctx.FP16(y.a);
					z0.mul(y.a);

					if (y.stype==ctx.FP.SPARSEST)
						z2.tmul(y.b.geta().geta().geta().geta());
					else
						z2.pmul(y.b.geta());

					this.b.add(this.a);
					t1.geta().add(y.b.geta());

					t1.norm();
					this.b.norm();
					this.b.mul(t1);
					z3.add(this.c);
					z3.norm();

					if (y.stype==ctx.FP.SPARSEST)
						z3.tmul(y.b.geta().geta().geta().geta());
					else
						z3.pmul(y.b.real());

					t0.copy(z0); t0.neg();
					t1.copy(z2); t1.neg();

					this.b.add(t0);

					this.b.add(t1);
					z3.add(t1);
					z2.add(t0);

					t0.copy(this.a); t0.add(this.c); t0.norm();
					z3.norm();
					t0.mul(y.a);
					this.c.copy(z2); this.c.add(t0);

					z3.times_i();
					this.a.copy(z0); this.a.add(z3);
				}
				if (ctx.ECP.SEXTIC_TWIST==ctx.ECP.M_TYPE)
				{
					var z0=new ctx.FP16(this.a);
					var z1=new ctx.FP16(0);
					var z2=new ctx.FP16(0);
					var z3=new ctx.FP16(0);
					var t0=new ctx.FP16(this.a);
					var t1=new ctx.FP16(0);

					z0.mul(y.a);
					t0.add(this.b); t0.norm();

					z1.copy(t0); z1.mul(y.a);
					t0.copy(this.b); t0.add(this.c);
					t0.norm();

					z3.copy(t0);

					if (y.stype==ctx.FP.SPARSEST)
						z3.tmul(y.c.getb().geta().geta().geta());
					else
						z3.pmul(y.c.getb());
					z3.times_i();

					t0.copy(z0); t0.neg();
					z1.add(t0);
					this.b.copy(z1);
					z2.copy(t0);

					t0.copy(this.a); t0.add(this.c); t0.norm();
					t1.copy(y.a); t1.add(y.c); t1.norm();

					t0.mul(t1);
					z2.add(t0);
					t0.copy(this.c);

					if (y.stype==ctx.FP.SPARSEST)
						t0.tmul(y.c.getb().geta().geta().geta());
					else
						t0.pmul(y.c.getb());
					t0.times_i();
					t1.copy(t0); t1.neg();

					this.c.copy(z2); this.c.add(t1);
					z3.add(t1);
					t0.times_i();
					this.b.add(t0);
					z3.norm();
					z3.times_i();
					this.a.copy(z0); this.a.add(z3);
				}
			}
			this.stype=ctx.FP.DENSE;
			this.norm();
		},


        /* this=1/this */
        inverse: function() {
            var f0 = new ctx.FP16(this.a),
                f1 = new ctx.FP16(this.b),
                f2 = new ctx.FP16(this.a),
                f3 = new ctx.FP16(0);

            f0.sqr();
            f1.mul(this.c);
            f1.times_i();
            f0.sub(f1);
            f0.norm();

            f1.copy(this.c);
            f1.sqr();
            f1.times_i();
            f2.mul(this.b);
            f1.sub(f2);
            f1.norm();

            f2.copy(this.b);
            f2.sqr();
            f3.copy(this.a);
            f3.mul(this.c);
            f2.sub(f3);
            f2.norm();

            f3.copy(this.b);
            f3.mul(f2);
            f3.times_i();
            this.a.mul(f0);
            f3.add(this.a);
            this.c.mul(f1);
            this.c.times_i();

            f3.add(this.c);
            f3.norm();
            f3.inverse();
            this.a.copy(f0);
            this.a.mul(f3);
            this.b.copy(f1);
            this.b.mul(f3);
            this.c.copy(f2);
            this.c.mul(f3);
			this.stype=ctx.FP.DENSE;
        },

        /* this=this^p, where p=Modulus, using Frobenius */
        frob: function(f,n) {
            var f2 = new ctx.FP2(f),
                f3 = new ctx.FP2(f),
                i;

            f2.sqr();
            f3.mul(f2);

            f3.mul_ip(); f3.norm();
            f3.mul_ip(); f3.norm();

            for (i=0;i<n;i++) {
                this.a.frob(f3);
                this.b.frob(f3);
                this.c.frob(f3);

                this.b.qmul(f); this.b.times_i4(); this.b.times_i2();
                this.c.qmul(f2); this.c.times_i4(); this.c.times_i4(); this.c.times_i4();
            }
			this.stype=ctx.FP.DENSE;
        },

        /* trace function */
        trace: function() {
            var t = new ctx.FP16(0);

            t.copy(this.a);
            t.imul(3);
            t.reduce();

            return t;
        },

        /* convert this to hex string */
        toString: function() {
            return ("[" + this.a.toString() + "," + this.b.toString() + "," + this.c.toString() + "]");
        },

        /* convert this to byte array */
        toBytes: function(w) {
            var t = [];
            this.c.toBytes(t);
		    for (var i=0;i<4*ctx.BIG.MODBYTES;i++) w[i]=t[i];
            this.b.toBytes(t);
		    for (var i=0;i<4*ctx.BIG.MODBYTES;i++) w[i+16*ctx.BIG.MODBYTES]=t[i];
            this.a.toBytes(t);
		    for (var i=0;i<4*ctx.BIG.MODBYTES;i++) w[i+32*ctx.BIG.MODBYTES]=t[i];
        },

        /* set this=this^e */
        pow: function(e) {
            var e1, e3, w, nb, i, bt, sf;

			sf = new FP48(this);
            sf.norm();
			e1 = new ctx.BIG(e);
            e1.norm();

            e3 = new ctx.BIG(e1);
            e3.pmul(3);
            e3.norm();

            w = new FP48(sf); //w.copy(this);

            if (e3.iszilch()) {
                w.one();
                return w;
            }
            nb = e3.nbits();

            for (i = nb - 2; i >= 1; i--) {
                w.usqr();
                bt = e3.bit(i) - e1.bit(i);

                if (bt == 1) {
                    w.mul(sf);
                }
                if (bt == -1) {
                    sf.conj();
                    w.mul(sf);
                    sf.conj();
                }
            }
            w.reduce();

            return w;
        },

        /* constant time powering by small integer of max length bts */
        pinpow: function(e, bts) {
            var R = [],
                i, b;

            R[0] = new FP48(1);
            R[1] = new FP48(this);

            for (i = bts - 1; i >= 0; i--) {
                b = (e >> i) & 1;
                R[1 - b].mul(R[b]);
                R[b].usqr();
            }

            this.copy(R[0]);
        },

        /* Faster compressed powering for unitary elements */
/*
        compow: function(e, r) {
            var fa, fb, f, q, m, a, b, g1, g2, c, cp, cpm1, cpm2;

            fa = new ctx.BIG(0);
            fa.rcopy(ctx.ROM_FIELD.Fra);
            fb = new ctx.BIG(0);
            fb.rcopy(ctx.ROM_FIELD.Frb);
            f = new ctx.FP2(fa, fb);

            q = new ctx.BIG(0);
            q.rcopy(ctx.ROM_FIELD.Modulus);

            m = new ctx.BIG(q);
            m.mod(r);

            a = new ctx.BIG(e);
            a.mod(m);

            b = new ctx.BIG(e);
            b.div(m);

            g1 = new FP48(0);
            g2 = new FP48(0);
            g1.copy(this);

            c = g1.trace();

            if (b.iszilch()) {
                c=c.xtr_pow(e);
                return c;
            }

            g2.copy(g1);
            g2.frob(f,1);
            cp = g2.trace();
            g1.conj();
            g2.mul(g1);
            cpm1 = g2.trace();
            g2.mul(g1);
            cpm2 = g2.trace();

            c = c.xtr_pow2(cp, cpm1, cpm2, a, b);
            return c;
        }
*/
    };

    /* convert from byte array to FP12 */
    FP48.fromBytes = function(w) {
		var a,b,c;
        var t=[];
		for (var i=0;i<16*ctx.BIG.MODBYTES;i++) t[i]=w[i];
        c=ctx.FP16.fromBytes(t);
		for (var i=0;i<16*ctx.BIG.MODBYTES;i++) t[i]=w[i+16*ctx.BIG.MODBYTES];
        b=ctx.FP16.fromBytes(t);
		for (var i=0;i<16*ctx.BIG.MODBYTES;i++) t[i]=w[i+32*ctx.BIG.MODBYTES];
        a=ctx.FP16.fromBytes(t);
		return new FP48(a,b,c);
    };

    /* return 1 if b==c, no branching */
    FP48.teq = function(b, c) {
        var x = b ^ c;
        x -= 1; // if x=0, x now -1
        return ((x >> 31) & 1);
    };

    /* p=q0^u0.q1^u1.q2^u2.q3^u3... */
    // Bos & Costello https://eprint.iacr.org/2013/458.pdf
    // Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
    // Side channel attack secure
    FP48.pow16 = function(q, u) {
        var g1 = [],
            g2 = [],
            g3 = [],
            g4 = [],
            r = new FP48(0),
            p = new FP48(0),
            t = [],
            mt = new ctx.BIG(0),
            w1 = [],
            s1 = [],
            w2 = [],
            s2 = [],
            w3 = [],
            s3 = [],
            w4 = [],
            s4 = [],
            i, j, k, nb, bt, pb1, pb2, pb3, pb4;

        for (i = 0; i < 16; i++) {
            t[i] = new ctx.BIG(u[i]); t[i].norm();
        }

        g1[0] = new FP48(q[0]);  // q[0]
        g1[1] = new FP48(g1[0]); g1[1].mul(q[1]);   // q[0].q[1]
        g1[2] = new FP48(g1[0]); g1[2].mul(q[2]);   // q[0].q[2]
        g1[3] = new FP48(g1[1]); g1[3].mul(q[2]);   // q[0].q[1].q[2]
        g1[4] = new FP48(q[0]);  g1[4].mul(q[3]);   // q[0].q[3]
        g1[5] = new FP48(g1[1]); g1[5].mul(q[3]);   // q[0].q[1].q[3]
        g1[6] = new FP48(g1[2]); g1[6].mul(q[3]);   // q[0].q[2].q[3]
        g1[7] = new FP48(g1[3]); g1[7].mul(q[3]);   // q[0].q[1].q[2].q[3]

        g2[0] = new FP48(q[4]);  // q[0]
        g2[1] = new FP48(g2[0]); g2[1].mul(q[5]);   // q[0].q[1]
        g2[2] = new FP48(g2[0]); g2[2].mul(q[6]);   // q[0].q[2]
        g2[3] = new FP48(g2[1]); g2[3].mul(q[6]);   // q[0].q[1].q[2]
        g2[4] = new FP48(q[4]);  g2[4].mul(q[7]);   // q[0].q[3]
        g2[5] = new FP48(g2[1]); g2[5].mul(q[7]);   // q[0].q[1].q[3]
        g2[6] = new FP48(g2[2]); g2[6].mul(q[7]);   // q[0].q[2].q[3]
        g2[7] = new FP48(g2[3]); g2[7].mul(q[7]);   // q[0].q[1].q[2].q[3]

        g3[0] = new FP48(q[8]);  // q[0]
        g3[1] = new FP48(g3[0]); g3[1].mul(q[9]);   // q[0].q[1]
        g3[2] = new FP48(g3[0]); g3[2].mul(q[10]);   // q[0].q[2]
        g3[3] = new FP48(g3[1]); g3[3].mul(q[10]);   // q[0].q[1].q[2]
        g3[4] = new FP48(q[8]);  g3[4].mul(q[11]);   // q[0].q[3]
        g3[5] = new FP48(g3[1]); g3[5].mul(q[11]);   // q[0].q[1].q[3]
        g3[6] = new FP48(g3[2]); g3[6].mul(q[11]);   // q[0].q[2].q[3]
        g3[7] = new FP48(g3[3]); g3[7].mul(q[11]);   // q[0].q[1].q[2].q[3]

        g4[0] = new FP48(q[12]);  // q[0]
        g4[1] = new FP48(g4[0]); g4[1].mul(q[13]);   // q[0].q[1]
        g4[2] = new FP48(g4[0]); g4[2].mul(q[14]);   // q[0].q[2]
        g4[3] = new FP48(g4[1]); g4[3].mul(q[14]);   // q[0].q[1].q[2]
        g4[4] = new FP48(q[12]);  g4[4].mul(q[15]);   // q[0].q[3]
        g4[5] = new FP48(g4[1]); g4[5].mul(q[15]);   // q[0].q[1].q[3]
        g4[6] = new FP48(g4[2]); g4[6].mul(q[15]);   // q[0].q[2].q[3]
        g4[7] = new FP48(g4[3]); g4[7].mul(q[15]);   // q[0].q[1].q[2].q[3]

        // Make it odd
        pb1=1-t[0].parity();
        t[0].inc(pb1);
        t[0].norm();

        pb2=1-t[4].parity();
        t[4].inc(pb2);
        t[4].norm();

        pb3=1-t[8].parity();
        t[8].inc(pb3);
        t[8].norm();

        pb4=1-t[12].parity();
        t[12].inc(pb4);
        t[12].norm();

        // Number of bits
        mt.zero();
        for (i=0;i<16;i++) {
            mt.or(t[i]);
        }

        nb=1+mt.nbits();

        // Sign pivot
        s1[nb-1]=1;
        s2[nb-1]=1;
        s3[nb-1]=1;
        s4[nb-1]=1;
        for (i=0;i<nb-1;i++) {
            t[0].fshr(1);
            s1[i]=2*t[0].parity()-1;
            t[4].fshr(1);
            s2[i]=2*t[4].parity()-1;
            t[8].fshr(1);
            s3[i]=2*t[8].parity()-1;
            t[12].fshr(1);
            s4[i]=2*t[12].parity()-1;

        }

        // Recoded exponent
        for (i=0; i<nb; i++) {
            w1[i]=0;
            k=1;
            for (j=1; j<4; j++) {
                bt=s1[i]*t[j].parity();
                t[j].fshr(1);
                t[j].dec(bt>>1);
                t[j].norm();
                w1[i]+=bt*k;
                k*=2;
            }
            w2[i]=0;
            k=1;
            for (j=5; j<8; j++) {
                bt=s2[i]*t[j].parity();
                t[j].fshr(1);
                t[j].dec(bt>>1);
                t[j].norm();
                w2[i]+=bt*k;
                k*=2;
            }
            w3[i]=0;
            k=1;
            for (j=9; j<12; j++) {
                bt=s3[i]*t[j].parity();
                t[j].fshr(1);
                t[j].dec(bt>>1);
                t[j].norm();
                w3[i]+=bt*k;
                k*=2;
            }
            w4[i]=0;
            k=1;
            for (j=13; j<16; j++) {
                bt=s4[i]*t[j].parity();
                t[j].fshr(1);
                t[j].dec(bt>>1);
                t[j].norm();
                w4[i]+=bt*k;
                k*=2;
            }
        }

        // Main loop
        p.select(g1,2*w1[nb-1]+1);
        r.select(g2,2*w2[nb-1]+1);
        p.mul(r);
        r.select(g3,2*w3[nb-1]+1);
        p.mul(r);
        r.select(g4,2*w4[nb-1]+1);
        p.mul(r);
        for (i=nb-2;i>=0;i--) {
            p.usqr();
            r.select(g1,2*w1[i]+s1[i]);
            p.mul(r);
            r.select(g2,2*w2[i]+s2[i]);
            p.mul(r);
            r.select(g3,2*w3[i]+s3[i]);
            p.mul(r);
            r.select(g4,2*w4[i]+s4[i]);
            p.mul(r);
        }

        // apply correction
        r.copy(q[0]); r.conj();
        r.mul(p);
        p.cmove(r,pb1);

        r.copy(q[4]); r.conj();
        r.mul(p);
        p.cmove(r,pb2);

        r.copy(q[8]); r.conj();
        r.mul(p);
        p.cmove(r,pb3);

        r.copy(q[12]); r.conj();
        r.mul(p);
        p.cmove(r,pb4);

        p.reduce();
        return p;
    };

    return FP48;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
	module.exports = {
		FP48: FP48
	};
}

