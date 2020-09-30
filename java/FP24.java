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

/* CORE Fp^24 functions */
/* FP24 elements are of the form a+i.b+i^2.c */

package org.miracl.core.XXX;

public final class FP24 {
    public static final int ZERO = 0;
    public static final int ONE = 1;
    public static final int SPARSEST = 2;
    public static final int SPARSER = 3;
    public static final int SPARSE = 4;
    public static final int DENSE = 5;

    private final FP8 a;
    private final FP8 b;
    private final FP8 c;
    private int type;

    public void settype(int a) {
        type = a;
    }

    public int gettype() {
        return type;
    }
    /* reduce all components of this mod Modulus */
    public void reduce() {
        a.reduce();
        b.reduce();
        c.reduce();
    }

    /* normalise all components of this */
    public void norm() {
        a.norm();
        b.norm();
        c.norm();
    }
    /* test x==0 ? */
    public boolean iszilch() {
        return (a.iszilch() && b.iszilch() && c.iszilch());
    }

    public void cmove(FP24 g, int d) {
        a.cmove(g.a, d);
        b.cmove(g.b, d);
        c.cmove(g.c, d);
        d = ~(d - 1);
        type ^= (type ^ g.type) & d;
    }


    /* return 1 if b==c, no branching */
    public static int teq(int b, int c) {
        int x = b ^ c;
        x -= 1; // if x=0, x now -1
        return ((x >> 31) & 1);
    }

    /* Constant time select from pre-computed table */
    public void select(FP24 g[], int b) {
        int m = b >> 31;
        int babs = (b ^ m) - m;

        babs = (babs - 1) / 2;

        cmove(g[0], teq(babs, 0)); // conditional move
        cmove(g[1], teq(babs, 1));
        cmove(g[2], teq(babs, 2));
        cmove(g[3], teq(babs, 3));
        cmove(g[4], teq(babs, 4));
        cmove(g[5], teq(babs, 5));
        cmove(g[6], teq(babs, 6));
        cmove(g[7], teq(babs, 7));

        FP24 invf = new FP24(this);
        invf.conj();
        cmove(invf, (int)(m & 1));
    }

    /* test x==1 ? */
    public boolean isunity() {
        FP8 one = new FP8(1);
        return (a.equals(one) && b.iszilch() && c.iszilch());
    }
    /* return 1 if x==y, else 0 */
    public boolean equals(FP24 x) {
        return (a.equals(x.a) && b.equals(x.b) && c.equals(x.c));
    }
    /* extract a from this */
    public FP8 geta() {
        return a;
    }
    /* extract b */
    public FP8 getb() {
        return b;
    }
    /* extract c */
    public FP8 getc() {
        return c;
    }
    /* copy this=x */
    public void copy(FP24 x) {
        a.copy(x.a);
        b.copy(x.b);
        c.copy(x.c);
        type = x.type;
    }
    /* set this=1 */
    public void one() {
        a.one();
        b.zero();
        c.zero();
        type = ONE;
    }
    public void zero() {
        a.zero();
        b.zero();
        c.zero();
        type = ZERO;
    }
    /* this=conj(this) */
    public void conj() {
        a.conj();
        b.nconj();
        c.conj();
    }
    /* Constructors */
    public FP24(FP8 d) {
        a = new FP8(d);
        b = new FP8();
        c = new FP8();
        type = SPARSEST;
    }

    public FP24() {
        a = new FP8();
        b = new FP8();
        c = new FP8();
        type = ZERO;
    }

    public FP24(int d) {
        a = new FP8(d);
        b = new FP8();
        c = new FP8();
        if (d == 1)
            type = ONE;
        else
            type = SPARSEST;
    }

    public FP24(FP8 d, FP8 e, FP8 f) {
        a = new FP8(d);
        b = new FP8(e);
        c = new FP8(f);
        type = DENSE;
    }

    public FP24(FP24 x) {
        a = new FP8(x.a);
        b = new FP8(x.b);
        c = new FP8(x.c);
        type = x.type;
    }

    /* Granger-Scott Unitary Squaring */
    public void usqr() {
        FP8 A = new FP8(a);
        FP8 B = new FP8(c);
        FP8 C = new FP8(b);
        FP8 D = new FP8();

        a.sqr();
        D.copy(a); D.add(a);
        a.add(D);

        a.norm();
        A.nconj();

        A.add(A);
        a.add(A);
        B.sqr();
        B.times_i();

        D.copy(B); D.add(B);
        B.add(D);
        B.norm();

        C.sqr();
        D.copy(C); D.add(C);
        C.add(D);
        C.norm();

        b.conj();
        b.add(b);
        c.nconj();

        c.add(c);
        b.add(B);
        c.add(C);
        type = DENSE;
        reduce();
    }

    /* Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */
    public void sqr() {
        if (type == ONE)
            return;

        FP8 A = new FP8(a);
        FP8 B = new FP8(b);
        FP8 C = new FP8(c);
        FP8 D = new FP8(a);

        A.sqr();
        B.mul(c);
        B.add(B);
        B.norm();
        C.sqr();
        D.mul(b);
        D.add(D);

        c.add(a);
        c.add(b);
        c.norm();
        c.sqr();

        a.copy(A);

        A.add(B);
        A.norm();
        A.add(C);
        A.add(D);
        A.norm();

        A.neg();
        B.times_i();
        C.times_i();

        a.add(B);

        b.copy(C); b.add(D);
        c.add(A);
        if (type == SPARSER || type == SPARSEST)
            type = SPARSE;
        else
            type = DENSE;
        norm();
    }

    /* FP12 full multiplication this=this*y */
    public void mul(FP24 y) {
        FP8 z0 = new FP8(a);
        FP8 z1 = new FP8();
        FP8 z2 = new FP8(b);
        FP8 z3 = new FP8();
        FP8 t0 = new FP8(a);
        FP8 t1 = new FP8(y.a);

        z0.mul(y.a);
        z2.mul(y.b);

        t0.add(b);
        t1.add(y.b);

        t0.norm();
        t1.norm();

        z1.copy(t0); z1.mul(t1);
        t0.copy(b); t0.add(c);

        t1.copy(y.b); t1.add(y.c);

        t0.norm();
        t1.norm();

        z3.copy(t0); z3.mul(t1);

        t0.copy(z0); t0.neg();
        t1.copy(z2); t1.neg();

        z1.add(t0);
        b.copy(z1); b.add(t1);

        z3.add(t1);
        z2.add(t0);

        t0.copy(a); t0.add(c);
        t1.copy(y.a); t1.add(y.c);

        t0.norm();
        t1.norm();

        t0.mul(t1);
        z2.add(t0);

        t0.copy(c); t0.mul(y.c);
        t1.copy(t0); t1.neg();

        c.copy(z2); c.add(t1);
        z3.add(t1);
        t0.times_i();
        b.add(t0);
        z3.norm();
        z3.times_i();
        a.copy(z0); a.add(z3);
        norm();
        type = DENSE;
    }


    /* FP24 multiplication w=w*y */
    /* catering for special case that arises from special form of ATE pairing line function */
    /* w and y are both sparser line functions - cost = 6m */
    public void smul(FP24 y) {
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
            FP4 w1 = new FP4(a.geta());
            FP4 w2 = new FP4(a.getb());
            FP4 w3;

            w1.mul(y.a.geta());
            w2.mul(y.a.getb());

            if (y.type == SPARSEST || type == SPARSEST) {
                if (y.type == SPARSEST && type == SPARSEST) {
                    FP t = new FP(b.geta().geta().geta());
                    t.mul(y.b.geta().geta().geta());
                    w3 = new FP4(t);
                } else {
                    if (y.type != SPARSEST) {
                        w3 = new FP4(y.b.geta());
                        w3.qmul(b.geta().geta().geta());
                    } else {
                        w3 = new FP4(b.geta());
                        w3.qmul(y.b.geta().geta().geta());
                    }
                }
            } else {
                w3 = new FP4(b.geta());
                w3.mul(y.b.geta());
            }
            FP4 ta = new FP4(a.geta());
            FP4 tb = new FP4(y.a.geta());
            ta.add(a.getb()); ta.norm();
            tb.add(y.a.getb()); tb.norm();
            FP4 tc = new FP4(ta);
            tc.mul(tb);
            FP4 t = new FP4(w1);
            t.add(w2);
            t.neg();
            tc.add(t);

            ta.copy(a.geta()); ta.add(b.geta()); ta.norm();
            tb.copy(y.a.geta()); tb.add(y.b.geta()); tb.norm();
            FP4 td = new FP4(ta);
            td.mul(tb);
            t.copy(w1);
            t.add(w3);
            t.neg();
            td.add(t);

            ta.copy(a.getb()); ta.add(b.geta()); ta.norm();
            tb.copy(y.a.getb()); tb.add(y.b.geta()); tb.norm();
            FP4 te = new FP4(ta);
            te.mul(tb);
            t.copy(w2);
            t.add(w3);
            t.neg();
            te.add(t);

            w2.times_i();
            w1.add(w2);

            a.geta().copy(w1); a.getb().copy(tc);
            b.geta().copy(td); b.getb().copy(te);
            c.geta().copy(w3); c.getb().zero();

            a.norm();
            b.norm();

        } else {
            FP4 w1 = new FP4(a.geta());
            FP4 w2 = new FP4(a.getb());
            FP4 w3;

            w1.mul(y.a.geta());
            w2.mul(y.a.getb());

            if (y.type == SPARSEST || type == SPARSEST) {
                if (y.type == SPARSEST && type == SPARSEST) {
                    FP t = new FP(c.getb().geta().geta());
                    t.mul(y.c.getb().geta().geta());
                    w3 = new FP4(t);
                } else {
                    if (y.type != SPARSEST) {
                        w3 = new FP4(y.c.getb());
                        w3.qmul(c.getb().geta().geta());
                    } else {
                        w3 = new FP4(c.getb());
                        w3.qmul(y.c.getb().geta().geta());
                    }
                }
            } else {
                w3 = new FP4(c.getb());
                w3.mul(y.c.getb());
            }

            FP4 ta = new FP4(a.geta());
            FP4 tb = new FP4(y.a.geta());
            ta.add(a.getb()); ta.norm();
            tb.add(y.a.getb()); tb.norm();
            FP4 tc = new FP4(ta);
            tc.mul(tb);
            FP4 t = new FP4(w1);
            t.add(w2);
            t.neg();
            tc.add(t);

            ta.copy(a.geta()); ta.add(c.getb()); ta.norm();
            tb.copy(y.a.geta()); tb.add(y.c.getb()); tb.norm();
            FP4 td = new FP4(ta);
            td.mul(tb);
            t.copy(w1);
            t.add(w3);
            t.neg();
            td.add(t);

            ta.copy(a.getb()); ta.add(c.getb()); ta.norm();
            tb.copy(y.a.getb()); tb.add(y.c.getb()); tb.norm();
            FP4 te = new FP4(ta);
            te.mul(tb);
            t.copy(w2);
            t.add(w3);
            t.neg();
            te.add(t);

            w2.times_i();
            w1.add(w2);
            a.geta().copy(w1); a.getb().copy(tc);

            w3.times_i();
            w3.norm();
            b.geta().zero(); b.getb().copy(w3);

            te.norm();
            te.times_i();
            c.geta().copy(te);
            c.getb().copy(td);

            a.norm();
            c.norm();

        }
        type = SPARSE;
    }

    /* FP24 full multiplication w=w*y */
    /* Supports sparse multiplicands */
    /* Usually w is denser than y */
    public void ssmul(FP24 y) {
        if (type == ONE) {
            copy(y);
            return;
        }
        if (y.type == ONE)
            return;

        if (y.type >= SPARSE) {
            FP8 z0 = new FP8(a);
            FP8 z1 = new FP8();
            FP8 z2 = new FP8();
            FP8 z3 = new FP8();
            z0.mul(y.a);

            if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
                if (y.type == SPARSE || type == SPARSE) {
                    z2.getb().copy(b.getb());
                    z2.getb().mul(y.b.getb());
                    z2.geta().zero();
                    if (y.type != SPARSE) {
                        z2.geta().copy(b.getb());
                        z2.geta().mul(y.b.geta());
                    }
                    if (type != SPARSE) {
                        z2.geta().copy(b.geta());
                        z2.geta().mul(y.b.getb());
                    }
                    z2.times_i();
                } else {
                    z2.copy(b);
                    z2.mul(y.b);
                }
            } else {
                z2.copy(b);
                z2.mul(y.b);
            }
            FP8 t0 = new FP8(a);
            FP8 t1 = new FP8(y.a);
            t0.add(b); t0.norm();
            t1.add(y.b); t1.norm();

            z1.copy(t0); z1.mul(t1);
            t0.copy(b); t0.add(c); t0.norm();
            t1.copy(y.b); t1.add(y.c); t1.norm();

            z3.copy(t0); z3.mul(t1);

            t0.copy(z0); t0.neg();
            t1.copy(z2); t1.neg();

            z1.add(t0);
            b.copy(z1); b.add(t1);

            z3.add(t1);
            z2.add(t0);

            t0.copy(a); t0.add(c); t0.norm();
            t1.copy(y.a); t1.add(y.c); t1.norm();

            t0.mul(t1);
            z2.add(t0);

            if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
                if (y.type == SPARSE || type == SPARSE) {
                    t0.geta().copy(c.geta());
                    t0.geta().mul(y.c.geta());
                    t0.getb().zero();
                    if (y.type != SPARSE) {
                        t0.getb().copy(c.geta());
                        t0.getb().mul(y.c.getb());
                    }
                    if (type != SPARSE) {
                        t0.getb().copy(c.getb());
                        t0.getb().mul(y.c.geta());
                    }
                } else {
                    t0.copy(c);
                    t0.mul(y.c);
                }
            } else {
                t0.copy(c);
                t0.mul(y.c);
            }
            t1.copy(t0); t1.neg();

            c.copy(z2); c.add(t1);
            z3.add(t1);
            t0.times_i();
            b.add(t0);
            z3.norm();
            z3.times_i();
            a.copy(z0); a.add(z3);
        } else {
            if (type == SPARSER || type == SPARSEST) {
                smul(y);
                return;
            }
            if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
                // dense by sparser - 13m
                FP8 z0 = new FP8(a);
                FP8 z2 = new FP8(b);
                FP8 z3 = new FP8(b);
                FP8 t0 = new FP8();
                FP8 t1 = new FP8(y.a);
                z0.mul(y.a);

                if (y.type == SPARSEST)
                    z2.tmul(y.b.geta().geta().geta());
                else
                    z2.pmul(y.b.geta());

                b.add(a);
                t1.geta().add(y.b.geta());

                t1.norm();
                b.norm();
                b.mul(t1);
                z3.add(c);
                z3.norm();

                if (y.type == SPARSEST)
                    z3.tmul(y.b.geta().geta().geta());
                else
                    z3.pmul(y.b.geta());

                t0.copy(z0); t0.neg();
                t1.copy(z2); t1.neg();

                b.add(t0);

                b.add(t1);
                z3.add(t1);
                z2.add(t0);

                t0.copy(a); t0.add(c); t0.norm();
                z3.norm();
                t0.mul(y.a);
                c.copy(z2); c.add(t0);

                z3.times_i();
                a.copy(z0); a.add(z3);
            }
            if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
                FP8 z0 = new FP8(a);
                FP8 z1 = new FP8();
                FP8 z2 = new FP8();
                FP8 z3 = new FP8();
                FP8 t0 = new FP8(a);
                FP8 t1 = new FP8();

                z0.mul(y.a);
                t0.add(b); t0.norm();

                z1.copy(t0); z1.mul(y.a);
                t0.copy(b); t0.add(c);
                t0.norm();

                z3.copy(t0);

                if (y.type == SPARSEST)
                    z3.tmul(y.c.getb().geta().geta());
                else
                    z3.pmul(y.c.getb());

                z3.times_i();

                t0.copy(z0); t0.neg();
                z1.add(t0);
                b.copy(z1);
                z2.copy(t0);

                t0.copy(a); t0.add(c); t0.norm();
                t1.copy(y.a); t1.add(y.c); t1.norm();

                t0.mul(t1);
                z2.add(t0);
                t0.copy(c);

                if (y.type == SPARSEST)
                    t0.tmul(y.c.getb().geta().geta());
                else
                    t0.pmul(y.c.getb());

                t0.times_i();
                t1.copy(t0); t1.neg();

                c.copy(z2); c.add(t1);
                z3.add(t1);
                t0.times_i();
                b.add(t0);
                z3.norm();
                z3.times_i();
                a.copy(z0); a.add(z3);
            }
        }
        type = DENSE;
        norm();
    }

    /* this=1/this */
    public void inverse() {
        FP8 f0 = new FP8(a);
        FP8 f1 = new FP8(b);
        FP8 f2 = new FP8(a);
        FP8 f3 = new FP8();

        //norm();
        f0.sqr();
        f1.mul(c);
        f1.times_i();
        f0.sub(f1);
        f0.norm();

        f1.copy(c); f1.sqr();
        f1.times_i();
        f2.mul(b);
        f1.sub(f2);
        f1.norm();

        f2.copy(b); f2.sqr();
        f3.copy(a); f3.mul(c);
        f2.sub(f3);
        f2.norm();

        f3.copy(b); f3.mul(f2);
        f3.times_i();
        a.mul(f0);
        f3.add(a);
        c.mul(f1);
        c.times_i();

        f3.add(c);
        f3.norm();
        f3.inverse(null);
        a.copy(f0); a.mul(f3);
        b.copy(f1); b.mul(f3);
        c.copy(f2); c.mul(f3);
        type = DENSE;
    }

    /* this=this^p using Frobenius */
    public void frob(FP2 f, int n) {
        FP2 f2 = new FP2(f);
        FP2 f3 = new FP2(f);

        f2.sqr();
        f3.mul(f2);

        f3.mul_ip(); f3.norm();

        for (int i = 0; i < n; i++) {
            a.frob(f3);
            b.frob(f3);
            c.frob(f3);

            b.qmul(f); b.times_i2();
            c.qmul(f2); c.times_i2(); c.times_i2();
        }
        type = DENSE;
    }

    /* trace function */
    public FP8 trace() {
        FP8 t = new FP8();
        t.copy(a);
        t.imul(3);
        t.reduce();
        return t;
    }

    /* convert from byte array to FP24 */
    public static FP24 fromBytes(byte[] w) {
		FP8 a,b,c;
		byte[] t=new byte[8*CONFIG_BIG.MODBYTES];
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++) t[i]=w[i];
        c=FP8.fromBytes(t);
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++) t[i]=w[i+8*CONFIG_BIG.MODBYTES];
        b=FP8.fromBytes(t);
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++) t[i]=w[i+16*CONFIG_BIG.MODBYTES];
        a=FP8.fromBytes(t);

		return new FP24(a,b,c);
    }

    /* convert this to byte array */
    public void toBytes(byte[] w) {
        byte[] t=new byte[8*CONFIG_BIG.MODBYTES];
        c.toBytes(t);
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++) w[i]=t[i];
        b.toBytes(t);
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++) w[i+8*CONFIG_BIG.MODBYTES]=t[i];
        a.toBytes(t);
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++) w[i+16*CONFIG_BIG.MODBYTES]=t[i];

    }

    /* convert to hex string */
    public String toString() {
        return ("[" + a.toString() + "," + b.toString() + "," + c.toString() + "]");
    }

    /* this=this^e */
    /* Note this is simple square and multiply, so not side-channel safe */
    public FP24 pow(BIG e) {
        FP24 sf = new FP24(this);
        sf.norm();

        BIG e1 = new BIG(e);
        e1.norm();
        BIG e3 = new BIG(e1);
        e3.pmul(3);
        e3.norm();

        FP24 w = new FP24(sf);
        if (e3.iszilch()) {
            w.one();
            return w;
        }
        int nb = e3.nbits();
        for (int i = nb - 2; i >= 1; i--) {
            w.usqr();
            int bt = e3.bit(i) - e1.bit(i);
            if (bt == 1)
                w.mul(sf);
            if (bt == -1) {
                sf.conj(); w.mul(sf); sf.conj();
            }
        }
        w.reduce();
        return w;

    }

    /* constant time powering by small integer of max length bts */
    public void pinpow(int e, int bts) {
        int i, b;
        FP24 [] R = new FP24[2];
        R[0] = new FP24(1);
        R[1] = new FP24(this);
        for (i = bts - 1; i >= 0; i--) {
            b = (e >> i) & 1;
            R[1 - b].mul(R[b]);
            R[b].usqr();
        }
        this.copy(R[0]);
    }
/*
    public FP8 compow(BIG e, BIG r) {
        FP24 g1 = new FP24();
        FP24 g2 = new FP24();
        FP2 f = new FP2(new BIG(ROM.Fra), new BIG(ROM.Frb));
        BIG q = new BIG(ROM.Modulus);

        BIG m = new BIG(q);
        m.mod(r);

        BIG a = new BIG(e);
        a.mod(m);

        BIG b = new BIG(e);
        b.div(m);

        g1.copy(this);
        g2.copy(this);

        FP8 c = g1.trace();

        if (b.iszilch()) {
            c = c.xtr_pow(e);
            return c;
        }

        g2.frob(f, 1);
        FP8 cp = g2.trace();
        g1.conj();
        g2.mul(g1);
        FP8 cpm1 = g2.trace();
        g2.mul(g1);
        FP8 cpm2 = g2.trace();

        c = c.xtr_pow2(cp, cpm1, cpm2, a, b);

        return c;
    }
*/
    /* p=q0^u0.q1^u1.q2^u2.q3^u3.... */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
// Side channel attack secure

    public static FP24 pow8(FP24[] q, BIG[] u) {
        int i, j, k, nb, pb1, pb2;
        FP24 [] g1 = new FP24[8];
        FP24 [] g2 = new FP24[8];
        FP24 r = new FP24(1);
        FP24 p = new FP24();
        BIG [] t = new BIG[8];
        BIG mt = new BIG(0);
        byte[] w1 = new byte[BIG.NLEN * CONFIG_BIG.BASEBITS + 1];
        byte[] s1 = new byte[BIG.NLEN * CONFIG_BIG.BASEBITS + 1];
        byte[] w2 = new byte[BIG.NLEN * CONFIG_BIG.BASEBITS + 1];
        byte[] s2 = new byte[BIG.NLEN * CONFIG_BIG.BASEBITS + 1];

        for (i = 0; i < 8; i++) {
            t[i] = new BIG(u[i]);
            t[i].norm();
        }

        g1[0] = new FP24(q[0]); // q[0]
        g1[1] = new FP24(g1[0]); g1[1].mul(q[1]); // q[0].q[1]
        g1[2] = new FP24(g1[0]); g1[2].mul(q[2]); // q[0].q[2]
        g1[3] = new FP24(g1[1]); g1[3].mul(q[2]); // q[0].q[1].q[2]
        g1[4] = new FP24(q[0]);  g1[4].mul(q[3]); // q[0].q[3]
        g1[5] = new FP24(g1[1]); g1[5].mul(q[3]); // q[0].q[1].q[3]
        g1[6] = new FP24(g1[2]); g1[6].mul(q[3]); // q[0].q[2].q[3]
        g1[7] = new FP24(g1[3]); g1[7].mul(q[3]); // q[0].q[1].q[2].q[3]

        g2[0] = new FP24(q[4]); // q[0]
        g2[1] = new FP24(g2[0]); g2[1].mul(q[5]); // q[0].q[1]
        g2[2] = new FP24(g2[0]); g2[2].mul(q[6]); // q[0].q[2]
        g2[3] = new FP24(g2[1]); g2[3].mul(q[6]); // q[0].q[1].q[2]
        g2[4] = new FP24(q[4]);  g2[4].mul(q[7]); // q[0].q[3]
        g2[5] = new FP24(g2[1]); g2[5].mul(q[7]); // q[0].q[1].q[3]
        g2[6] = new FP24(g2[2]); g2[6].mul(q[7]); // q[0].q[2].q[3]
        g2[7] = new FP24(g2[3]); g2[7].mul(q[7]); // q[0].q[1].q[2].q[3]

        // Make it odd
        pb1 = 1 - t[0].parity();
        t[0].inc(pb1);
        t[0].norm();

        pb2 = 1 - t[4].parity();
        t[4].inc(pb2);
        t[4].norm();


        // Number of bits
        mt.zero();
        for (i = 0; i < 8; i++) {
            mt.or(t[i]);
        }
        nb = 1 + mt.nbits();

        // Sign pivot
        s1[nb - 1] = 1;
        s2[nb - 1] = 1;
        for (i = 0; i < nb - 1; i++) {
            t[0].fshr(1);
            s1[i] = (byte)(2 * t[0].parity() - 1);
            t[4].fshr(1);
            s2[i] = (byte)(2 * t[4].parity() - 1);
        }

        // Recoded exponent
        for (i = 0; i < nb; i++) {
            w1[i] = 0;
            k = 1;
            for (j = 1; j < 4; j++) {
                byte bt = (byte)(s1[i] * t[j].parity());
                t[j].fshr(1);
                t[j].dec((int)(bt) >> 1);
                t[j].norm();
                w1[i] += bt * (byte)k;
                k *= 2;
            }

            w2[i] = 0;
            k = 1;
            for (j = 5; j < 8; j++) {
                byte bt = (byte)(s2[i] * t[j].parity());
                t[j].fshr(1);
                t[j].dec((int)(bt) >> 1);
                t[j].norm();
                w2[i] += bt * (byte)k;
                k *= 2;
            }
        }

        // Main loop
        p.select(g1, (int)(2 * w1[nb - 1] + 1));
        r.select(g2, (int)(2 * w2[nb - 1] + 1));
        p.mul(r);
        for (i = nb - 2; i >= 0; i--) {
            p.usqr();
            r.select(g1, (int)(2 * w1[i] + s1[i]));
            p.mul(r);
            r.select(g2, (int)(2 * w2[i] + s2[i]));
            p.mul(r);

        }

        // apply correction
        r.copy(q[0]); r.conj();
        r.mul(p);
        p.cmove(r, pb1);

        r.copy(q[4]); r.conj();
        r.mul(p);
        p.cmove(r, pb2);

        p.reduce();
        return p;
    }

}
