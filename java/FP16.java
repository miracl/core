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

/* Finite Field arithmetic  Fp^16 functions */

/* FP16 elements are of the form a+ib, where i is sqrt(sqrt(sqrt(-1+sqrt(-1))))  */

package org.miracl.core.XXX;

public final class FP16 {
    private final FP8 a;
    private final FP8 b;
    /* reduce all components of this mod Modulus */
    public void reduce() {
        a.reduce();
        b.reduce();
    }
    /* normalise all components of this mod Modulus */
    public void norm() {
        a.norm();
        b.norm();
    }
    /* test this==0 ? */
    public boolean iszilch() {
        return (a.iszilch() && b.iszilch());
    }

    public void toBytes(byte[] bf) {
		byte[] t=new byte[8*CONFIG_BIG.MODBYTES];
        b.toBytes(t);
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++)
            bf[i]=t[i];
        a.toBytes(t);
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++)
            bf[i+8*CONFIG_BIG.MODBYTES]=t[i];
    }

    public static FP16 fromBytes(byte[] bf) {
		byte[] t=new byte[8*CONFIG_BIG.MODBYTES];
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++)
            t[i]=bf[i];
        FP8 tb=FP8.fromBytes(t);
		for (int i=0;i<8*CONFIG_BIG.MODBYTES;i++)
            t[i]=bf[i+8*CONFIG_BIG.MODBYTES];
        FP8 ta=FP8.fromBytes(t);
        return new FP16(ta,tb);
    }



    public void cmove(FP16 g, int d) {
        a.cmove(g.a, d);
        b.cmove(g.b, d);
    }

    /* test this==1 ? */
    public boolean isunity() {
        FP8 one = new FP8(1);
        return (a.equals(one) && b.iszilch());
    }

    /* test is w real? That is in a+ib test b is zero */
    public boolean isreal() {
        return b.iszilch();
    }
    /* extract real part a */
    public FP8 real() {
        return a;
    }

    public FP8 geta() {
        return a;
    }
    /* extract imaginary part b */
    public FP8 getb() {
        return b;
    }
    /* test this=x? */
    public boolean equals(FP16 x) {
        return (a.equals(x.a) && b.equals(x.b));
    }
    /* constructors */

    public FP16() {
        a = new FP8();
        b = new FP8();
    }

    public FP16(int c) {
        a = new FP8(c);
        b = new FP8();
    }

    public FP16(FP16 x) {
        a = new FP8(x.a);
        b = new FP8(x.b);
    }

    public FP16(FP8 c, FP8 d) {
        a = new FP8(c);
        b = new FP8(d);
    }

    public FP16(FP8 c) {
        a = new FP8(c);
        b = new FP8();
    }
    /* copy this=x */
    public void copy(FP16 x) {
        a.copy(x.a);
        b.copy(x.b);
    }
    /* set this=0 */
    public void zero() {
        a.zero();
        b.zero();
    }

    /* set this=1 */
    public void one() {
        a.one();
        b.zero();
    }
    /* set this=-this */
    public void neg() {
        norm();
        FP8 m = new FP8(a);
        FP8 t = new FP8();
        m.add(b);
        m.neg();
        t.copy(m); t.add(b);
        b.copy(m);
        b.add(a);
        a.copy(t);
        norm();
    }

    /* this=conjugate(this) */
    public void conj() {
        b.neg(); norm();
    }
    /* this=-conjugate(this) */
    public void nconj() {
        a.neg(); norm();
    }
    /* this+=x */
    public void add(FP16 x) {
        a.add(x.a);
        b.add(x.b);
    }
    /* this-=x */
    public void sub(FP16 x) {
        FP16 m = new FP16(x);
        m.neg();
        add(m);
    }

    /* this=x-this */
    public void rsub(FP16 x) {
        neg();
        add(x);
    }

    /* this*=s where s is FP8 */
    public void pmul(FP8 s) {
        a.mul(s);
        b.mul(s);
    }
    /* this*=s where s is FP2 */
    public void qmul(FP2 s) {
        a.qmul(s);
        b.qmul(s);
    }

    /* this*=s where s is FP */
    public void tmul(FP s) {
        a.tmul(s);
        b.tmul(s);
    }

    /* this*=c where c is int */
    public void imul(int c) {
        a.imul(c);
        b.imul(c);
    }

    /* this*=this */
    public void sqr() {
        FP8 t1 = new FP8(a);
        FP8 t2 = new FP8(b);
        FP8 t3 = new FP8(a);

        t3.mul(b);
        t1.add(b);
        t2.times_i();

        t2.add(a);

        t1.norm();
        t2.norm();

        a.copy(t1);

        a.mul(t2);

        t2.copy(t3);
        t2.times_i();
        t2.add(t3);
        t2.norm();
        t2.neg();
        a.add(t2);

        b.copy(t3);
        b.add(t3);

        norm();
    }

    /* this*=y */
    public void mul(FP16 y) {
        FP8 t1 = new FP8(a);
        FP8 t2 = new FP8(b);
        FP8 t3 = new FP8();
        FP8 t4 = new FP8(b);

        t1.mul(y.a);
        t2.mul(y.b);
        t3.copy(y.b);
        t3.add(y.a);
        t4.add(a);

        t3.norm();
        t4.norm();

        t4.mul(t3);

        t3.copy(t1);
        t3.neg();
        t4.add(t3);
        t4.norm();

        t3.copy(t2);
        t3.neg();
        b.copy(t4);
        b.add(t3);

        t2.times_i();
        a.copy(t2);
        a.add(t1);

        norm();
    }

    /* convert this to hex string */
    public String toString() {
        return ("[" + a.toString() + "," + b.toString() + "]");
    }

    /* this=1/this */
    public void inverse() {
        FP8 t1 = new FP8(a);
        FP8 t2 = new FP8(b);

        t1.sqr();
        t2.sqr();
        t2.times_i();
        t2.norm();
        t1.sub(t2); t1.norm();

        t1.inverse(null);
        a.mul(t1);
        t1.neg();
        t1.norm();
        b.mul(t1);
    }

    /* this*=i where i = sqrt(-1+sqrt(-1)) */
    public void times_i() {
        FP8 s = new FP8(b);
        FP8 t = new FP8(a);
        s.times_i();
        b.copy(t);
        a.copy(s);
        norm();
    }

    public void times_i2() {
        a.times_i();
        b.times_i();
    }

    public void times_i4() {
        a.times_i2();
        b.times_i2();
    }

    /* this=this^p using Frobenius */
    public void frob(FP2 f) {
        FP2 ff = new FP2(f); ff.sqr(); ff.norm();

        a.frob(ff);
        b.frob(ff);
        b.qmul(f);
        b.times_i();
    }

    /* this=this^e */
    public FP16 pow(BIG e) {
        FP16 w = new FP16(this);
        w.norm();
        BIG z = new BIG(e);
        FP16 r = new FP16(1);
        z.norm();
        while (true) {
            int bt = z.parity();
            z.fshr(1);
            if (bt == 1) r.mul(w);
            if (z.iszilch()) break;
            w.sqr();
        }
        r.reduce();
        return r;
    }

    /* XTR xtr_a function */
/*
    public void xtr_A(FP16 w, FP16 y, FP16 z) {
        FP16 r = new FP16(w);
        FP16 t = new FP16(w);

        r.sub(y);
        r.norm();
        r.pmul(a);
        t.add(y);
        t.norm();
        t.pmul(b);
        t.times_i();

        copy(r);
        add(t);
        add(z);

        norm();
    }
*/
    /* XTR xtr_d function */
/*
    public void xtr_D() {
        FP16 w = new FP16(this);
        sqr(); w.conj();
        w.add(w);
        w.norm();
        sub(w);
        reduce();
    }
*/
    /* r=x^n using XTR method on traces of FP48s */
/*
    public FP16 xtr_pow(BIG n) {
        FP16 sf = new FP16(this);
        sf.norm();
        FP16 a = new FP16(3);
        FP16 b = new FP16(sf);
        FP16 c = new FP16(b);
        c.xtr_D();
        FP16 t = new FP16();
        FP16 r = new FP16();


        int par = n.parity();
        BIG v = new BIG(n); v.norm(); v.fshr(1);
        if (par == 0) {v.dec(1); v.norm();}

        int nb = v.nbits();
        for (int i = nb - 1; i >= 0; i--) {
            if (v.bit(i) != 1) {
                t.copy(b);
                sf.conj();
                c.conj();
                b.xtr_A(a, sf, c);
                sf.conj();
                c.copy(t);
                c.xtr_D();
                a.xtr_D();
            } else {
                t.copy(a); t.conj();
                a.copy(b);
                a.xtr_D();
                b.xtr_A(c, sf, t);
                c.xtr_D();
            }
        }
        if (par == 0) r.copy(c);
        else r.copy(b);
        r.reduce();
        return r;
    }
*/
    /* r=ck^a.cl^n using XTR double exponentiation method on traces of FP48s. See Stam thesis. */
/*
    public FP16 xtr_pow2(FP16 ck, FP16 ckml, FP16 ckm2l, BIG a, BIG b) {
        BIG e = new BIG(a);
        BIG d = new BIG(b);
        BIG w = new BIG(0);
        d.norm(); d.norm();
        FP16 cu = new FP16(ck); // can probably be passed in w/o copying
        FP16 cv = new FP16(this);
        FP16 cumv = new FP16(ckml);
        FP16 cum2v = new FP16(ckm2l);
        FP16 r = new FP16();
        FP16 t = new FP16();

        int f2 = 0;
        while (d.parity() == 0 && e.parity() == 0) {
            d.fshr(1);
            e.fshr(1);
            f2++;
        }

        while (BIG.comp(d, e) != 0) {
            if (BIG.comp(d, e) > 0) {
                w.copy(e); w.imul(4); w.norm();
                if (BIG.comp(d, w) <= 0) {
                    w.copy(d); d.copy(e);
                    e.rsub(w); e.norm();

                    t.copy(cv);
                    t.xtr_A(cu, cumv, cum2v);
                    cum2v.copy(cumv);
                    cum2v.conj();
                    cumv.copy(cv);
                    cv.copy(cu);
                    cu.copy(t);

                } else if (d.parity() == 0) {
                    d.fshr(1);
                    r.copy(cum2v); r.conj();
                    t.copy(cumv);
                    t.xtr_A(cu, cv, r);
                    cum2v.copy(cumv);
                    cum2v.xtr_D();
                    cumv.copy(t);
                    cu.xtr_D();
                } else if (e.parity() == 1) {
                    d.sub(e); d.norm();
                    d.fshr(1);
                    t.copy(cv);
                    t.xtr_A(cu, cumv, cum2v);
                    cu.xtr_D();
                    cum2v.copy(cv);
                    cum2v.xtr_D();
                    cum2v.conj();
                    cv.copy(t);
                } else {
                    w.copy(d);
                    d.copy(e); d.fshr(1);
                    e.copy(w);
                    t.copy(cumv);
                    t.xtr_D();
                    cumv.copy(cum2v); cumv.conj();
                    cum2v.copy(t); cum2v.conj();
                    t.copy(cv);
                    t.xtr_D();
                    cv.copy(cu);
                    cu.copy(t);
                }
            }
            if (BIG.comp(d, e) < 0) {
                w.copy(d); w.imul(4); w.norm();
                if (BIG.comp(e, w) <= 0) {
                    e.sub(d); e.norm();
                    t.copy(cv);
                    t.xtr_A(cu, cumv, cum2v);
                    cum2v.copy(cumv);
                    cumv.copy(cu);
                    cu.copy(t);
                } else if (e.parity() == 0) {
                    w.copy(d);
                    d.copy(e); d.fshr(1);
                    e.copy(w);
                    t.copy(cumv);
                    t.xtr_D();
                    cumv.copy(cum2v); cumv.conj();
                    cum2v.copy(t); cum2v.conj();
                    t.copy(cv);
                    t.xtr_D();
                    cv.copy(cu);
                    cu.copy(t);
                } else if (d.parity() == 1) {
                    w.copy(e);
                    e.copy(d);
                    w.sub(d); w.norm();
                    d.copy(w); d.fshr(1);
                    t.copy(cv);
                    t.xtr_A(cu, cumv, cum2v);
                    cumv.conj();
                    cum2v.copy(cu);
                    cum2v.xtr_D();
                    cum2v.conj();
                    cu.copy(cv);
                    cu.xtr_D();
                    cv.copy(t);
                } else {
                    d.fshr(1);
                    r.copy(cum2v); r.conj();
                    t.copy(cumv);
                    t.xtr_A(cu, cv, r);
                    cum2v.copy(cumv);
                    cum2v.xtr_D();
                    cumv.copy(t);
                    cu.xtr_D();
                }
            }
        }
        r.copy(cv);
        r.xtr_A(cu, cumv, cum2v);
        for (int i = 0; i < f2; i++)
            r.xtr_D();
        r = r.xtr_pow(d);
        return r;
    }
*/
}
