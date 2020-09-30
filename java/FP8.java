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

/* Finite Field arithmetic  Fp^8 functions */

/* FP8 elements are of the form a+ib, where i is sqrt(sqrt(-1+sqrt(-1)))  */

package org.miracl.core.XXX;
import org.miracl.core.RAND;

public final class FP8 {
	private final FP4 a;
	private final FP4 b;
/* reduce all components of this mod Modulus */
	public void reduce()
	{
		a.reduce();
		b.reduce();
	}
/* normalise all components of this mod Modulus */
	public void norm()
	{
		a.norm();
		b.norm();
	}
/* test this==0 ? */
	public boolean iszilch() {
		return (a.iszilch() && b.iszilch());
	}

    public int islarger() {
        if (iszilch()) return 0;
        int cmp=b.islarger();
        if (cmp!=0) return cmp;
        return a.islarger();
    }

    public void toBytes(byte[] bf) {
		byte[] t=new byte[4*CONFIG_BIG.MODBYTES];
        b.toBytes(t);
		for (int i=0;i<4*CONFIG_BIG.MODBYTES;i++)
            bf[i]=t[i];
        a.toBytes(t);
		for (int i=0;i<4*CONFIG_BIG.MODBYTES;i++)
            bf[i+4*CONFIG_BIG.MODBYTES]=t[i];
    }

    public static FP8 fromBytes(byte[] bf) {
		byte[] t=new byte[4*CONFIG_BIG.MODBYTES];
		for (int i=0;i<4*CONFIG_BIG.MODBYTES;i++)
            t[i]=bf[i];
        FP4 tb=FP4.fromBytes(t);
		for (int i=0;i<4*CONFIG_BIG.MODBYTES;i++)
            t[i]=bf[i+4*CONFIG_BIG.MODBYTES];
        FP4 ta=FP4.fromBytes(t);
        return new FP8(ta,tb);
    }


	public void cmove(FP8 g,int d)
	{
		a.cmove(g.a,d);
		b.cmove(g.b,d);
	}

/* test this==1 ? */
	public boolean isunity() {
		FP4 one=new FP4(1);
		return (a.equals(one) && b.iszilch());
	}

/* test is w real? That is in a+ib test b is zero */
	public boolean isreal()
	{
		return b.iszilch();
	}
/* extract real part a */
	public FP4 real()
	{
		return a;
	}

	public FP4 geta()
	{
		return a;
	}
/* extract imaginary part b */
	public FP4 getb()
	{
		return b;
	}
/* test this=x? */
	public boolean equals(FP8 x)
	{
		return (a.equals(x.a) && b.equals(x.b));
	}
/* constructors */
	public FP8()
	{
		a=new FP4();
		b=new FP4();
	}

	public FP8(int c)
	{
		a=new FP4(c);
		b=new FP4();
	}

	public FP8(int c,int d)
	{
		a=new FP4(c);
		b=new FP4(d);
	}

	public FP8(FP8 x)
	{
		a=new FP4(x.a);
		b=new FP4(x.b);
	}

	public FP8(FP4 c,FP4 d)
	{
		a=new FP4(c);
		b=new FP4(d);
	}

	public FP8(FP4 c)
	{
		a=new FP4(c);
		b=new FP4();
	}

	public FP8(FP c)
	{
		a=new FP4(c);
		b=new FP4();
	}

    public FP8(RAND rng)
    {
        a=new FP4(rng);
        b=new FP4(rng);
    }
/* copy this=x */
	public void copy(FP8 x)
	{
		a.copy(x.a);
		b.copy(x.b);
	}
/* set this=0 */
	public void zero()
	{
		a.zero();
		b.zero();
	}
/* set this=1 */
	public void one()
	{
		a.one();
		b.zero();
	}
/* get sign */ 
    public int sign()
    {
        int p1=a.sign();
        int p2=b.sign();
        if (CONFIG_FIELD.BIG_ENDIAN_SIGN)
        {
            int u=b.iszilch()? 1:0;
            p2^=(p1^p2)&u;
            return p2;
        } else {
            int u=a.iszilch()? 1:0;
            p1^=(p1^p2)&u;
            return p1;
        }
    }

/* set this=-this */
	public void neg()
	{
		norm();
		FP4 m=new FP4(a);
		FP4 t=new FP4();
		m.add(b);

		m.neg();
		t.copy(m); t.add(b);
		b.copy(m);
		b.add(a);
		a.copy(t);
		norm();
	}

/* this=conjugate(this) */
	public void conj()
	{
		b.neg(); norm();
	}
/* this=-conjugate(this) */
	public void nconj()
	{
		a.neg(); norm();
	}
/* this+=x */
	public void add(FP8 x)
	{
		a.add(x.a);
		b.add(x.b);
	}
/* this-=x */
	public void sub(FP8 x)
	{
		FP8 m=new FP8(x);
		m.neg();
		add(m);
	}

/* this=x-this */
	public void rsub(FP8 x)
	{
		neg();
		add(x);
	}


/* this*=s where s is FP4 */
	public void pmul(FP4 s)
	{
		a.mul(s);
		b.mul(s);
	}
/* this*=s where s is FP2 */
	public void qmul(FP2 s)
	{
		a.pmul(s);
		b.pmul(s);
	}
/* this*=s where s is FP */
	public void tmul(FP s)
	{
		a.qmul(s);
		b.qmul(s);
	}
/* this*=c where c is int */
	public void imul(int c)
	{
		a.imul(c);
		b.imul(c);
	}

/* this*=this */
	public void sqr()
	{
		FP4 t1=new FP4(a);
		FP4 t2=new FP4(b);
		FP4 t3=new FP4(a);

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
	public void mul(FP8 y)
	{
		FP4 t1=new FP4(a);
		FP4 t2=new FP4(b);
		FP4 t3=new FP4();
		FP4 t4=new FP4(b);

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
	public String toString()
	{
		return ("["+a.toString()+","+b.toString()+"]");
	}

/* this=1/this */
	public void inverse(FP h)
	{
		FP4 t1=new FP4(a);
		FP4 t2=new FP4(b);

		t1.sqr();
		t2.sqr();
		t2.times_i();
		t2.norm();
		t1.sub(t2); t1.norm();

		t1.inverse(h);

		a.mul(t1);
		t1.neg();
		t1.norm();
		b.mul(t1);
	}

/* this*=i where i = sqrt(-1+sqrt(-1)) */
	public void times_i()
	{
		FP4 s=new FP4(b);
		FP4 t=new FP4(a);
		s.times_i();

		b.copy(t);
		a.copy(s);
		norm();
		if (CONFIG_FIELD.TOWER==CONFIG_FIELD.POSITOWER) {
			neg();
			norm();
		}
	}

	public void times_i2()
	{
		a.times_i();
		b.times_i();
	}

/* this=this^p using Frobenius */
	public void frob(FP2 f)
	{
		FP2 ff=new FP2(f); ff.sqr(); ff.mul_ip(); ff.norm();

		a.frob(ff);
		b.frob(ff);
		b.pmul(f);
		b.times_i();

	}

/* this=this^e */
/*
	public FP8 pow(BIG e)
	{
		FP8 w=new FP8(this);
		w.norm();
		BIG z=new BIG(e);
		FP8 r=new FP8(1);
		z.norm();
		while (true)
		{
			int bt=z.parity();
			z.fshr(1);
			if (bt==1) r.mul(w);
			if (z.iszilch()) break;
			w.sqr();
		}
		r.reduce();
		return r;
	}
*/
/* XTR xtr_a function */
/*
	public void xtr_A(FP8 w,FP8 y,FP8 z)
	{
		FP8 r=new FP8(w);
		FP8 t=new FP8(w);

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
		FP8 w=new FP8(this);
		sqr(); w.conj();
		w.add(w);
		w.norm();
		sub(w);
		reduce();
	}
*/
/* r=x^n using XTR method on traces of FP24s */
/*
	public FP8 xtr_pow(BIG n) {
		FP8 sf=new FP8(this);
		sf.norm();
		FP8 a=new FP8(3);
		FP8 b=new FP8(sf);
		FP8 c=new FP8(b);
		c.xtr_D();
		FP8 t=new FP8();
		FP8 r=new FP8();

		int par=n.parity();
		BIG v=new BIG(n); v.norm(); v.fshr(1);
		if (par==0) {v.dec(1); v.norm();}

		int nb=v.nbits();
		for (int i=nb-1;i>=0;i--)
		{
			if (v.bit(i)!=1)
			{
				t.copy(b);
				sf.conj();
				c.conj();
				b.xtr_A(a,sf,c);
				sf.conj();
				c.copy(t);
				c.xtr_D();
				a.xtr_D();
			}
			else
			{
				t.copy(a); t.conj();
				a.copy(b);
				a.xtr_D();
				b.xtr_A(c,sf,t);
				c.xtr_D();
			}
		}
		if (par==0) r.copy(c);
		else r.copy(b);
		r.reduce();
		return r;
	}
*/
/* r=ck^a.cl^n using XTR double exponentiation method on traces of FP24s. See Stam thesis. */
/*
	public FP8 xtr_pow2(FP8 ck,FP8 ckml,FP8 ckm2l,BIG a,BIG b)
	{

		BIG e=new BIG(a);
		BIG d=new BIG(b);
		BIG w=new BIG(0);
		e.norm(); d.norm();

		FP8 cu=new FP8(ck);  // can probably be passed in w/o copying
		FP8 cv=new FP8(this);
		FP8 cumv=new FP8(ckml);
		FP8 cum2v=new FP8(ckm2l);
		FP8 r=new FP8();
		FP8 t=new FP8();

		int f2=0;
		while (d.parity()==0 && e.parity()==0)
		{
			d.fshr(1);
			e.fshr(1);
			f2++;
		}

		while (BIG.comp(d,e)!=0)
		{
			if (BIG.comp(d,e)>0)
			{
				w.copy(e); w.imul(4); w.norm();
				if (BIG.comp(d,w)<=0)
				{
					w.copy(d); d.copy(e);
					e.rsub(w); e.norm();

					t.copy(cv);
					t.xtr_A(cu,cumv,cum2v);
					cum2v.copy(cumv);
					cum2v.conj();
					cumv.copy(cv);
					cv.copy(cu);
					cu.copy(t);

				}
				else if (d.parity()==0)
				{
					d.fshr(1);
					r.copy(cum2v); r.conj();
					t.copy(cumv);
					t.xtr_A(cu,cv,r);
					cum2v.copy(cumv);
					cum2v.xtr_D();
					cumv.copy(t);
					cu.xtr_D();
				}
				else if (e.parity()==1)
				{
					d.sub(e); d.norm();
					d.fshr(1);
					t.copy(cv);
					t.xtr_A(cu,cumv,cum2v);
					cu.xtr_D();
					cum2v.copy(cv);
					cum2v.xtr_D();
					cum2v.conj();
					cv.copy(t);
				}
				else
				{
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
			if (BIG.comp(d,e)<0)
			{
				w.copy(d); w.imul(4); w.norm();
				if (BIG.comp(e,w)<=0)
				{
					e.sub(d); e.norm();
					t.copy(cv);
					t.xtr_A(cu,cumv,cum2v);
					cum2v.copy(cumv);
					cumv.copy(cu);
					cu.copy(t);
				}
				else if (e.parity()==0)
				{
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
				else if (d.parity()==1)
				{
					w.copy(e);
					e.copy(d);
					w.sub(d); w.norm();
					d.copy(w); d.fshr(1);
					t.copy(cv);
					t.xtr_A(cu,cumv,cum2v);
					cumv.conj();
					cum2v.copy(cu);
					cum2v.xtr_D();
					cum2v.conj();
					cu.copy(cv);
					cu.xtr_D();
					cv.copy(t);
				}
				else
				{
					d.fshr(1);
					r.copy(cum2v); r.conj();
					t.copy(cumv);
					t.xtr_A(cu,cv,r);
					cum2v.copy(cumv);
					cum2v.xtr_D();
					cumv.copy(t);
					cu.xtr_D();
				}
			}
		}
		r.copy(cv);
		r.xtr_A(cu,cumv,cum2v);
		for (int i=0;i<f2;i++)
			r.xtr_D();
		r=r.xtr_pow(d);
		return r;
	}
*/
/* this/=2 */
	public void div2()
	{
		a.div2();
		b.div2();
	}

	public void div_i()
	{
		FP4 u=new FP4(a);
		FP4 v=new FP4(b);
		u.div_i();
		a.copy(v);
		b.copy(u);
		if (CONFIG_FIELD.TOWER==CONFIG_FIELD.POSITOWER) {
			neg();
			norm();
		}
	}
/*
    public void pow(BIG b)
    {
        FP8 w = new FP8(this);
        FP8 r = new FP8(1);
        BIG z = new BIG(b);
        while (true)
        {
            int bt = z.parity();
            z.shr(1);
            if (bt == 1) r.mul(w);
            if (z.iszilch()) break;
            w.sqr();
        }
        r.reduce();
        copy(r);
    }
*/

/* PFGE48S
    public int qr(FP h)
    {
        FP8 c = new FP8(this);
        c.conj();
        c.mul(this);
        return c.geta().qr(h);
    }


// sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b)/2)+ib/(2*sqrt(a+sqrt(a*a-n*b*b)/2)) 
// returns true if this is QR 
	public void sqrt(FP h)
	{
		if (iszilch()) return;
		FP4 wa=new FP4(a);
        FP4 wb=new FP4(a);
		FP4 ws=new FP4(b);
		FP4 wt=new FP4(a);
        FP hint=new FP();

		ws.sqr();
		wa.sqr();
		ws.times_i();
		ws.norm();
		wa.sub(ws);

		ws.copy(wa); ws.norm();

        ws.sqrt(h);

		wa.copy(wt); wa.add(ws); 
        wa.norm(); wa.div2();

        wb.copy(b); wb.div2();
        int qr=wa.qr(hint);

// tweak hint - multiply old hint by Norm(1/Beta)^e where Beta is irreducible polynomial
        ws.copy(wa);
        FP twk=new FP(new BIG(ROM.TWK));
        twk.mul(hint);
        ws.div_i(); ws.norm();

        wa.cmove(ws,1-qr);
        hint.cmove(twk,1-qr);

        a.copy(wa); a.sqrt(hint);
        ws.copy(wa); ws.inverse(hint);
        ws.mul(a);
        b.copy(ws); b.mul(wb);
        wt.copy(a);

        a.cmove(b,1-qr);
        b.cmove(wt,1-qr);

        int sgn=this.sign();
        FP8 nr=new FP8(this);
        nr.neg(); nr.norm();
        this.cmove(nr,sgn);
	}
PFGE48F */
}
