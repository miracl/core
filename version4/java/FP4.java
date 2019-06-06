/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/* Finite Field arithmetic  Fp^4 functions */

/* FP4 elements are of the form a+ib, where i is sqrt(-1+sqrt(-1))  */

package org.miracl.core.XXX;

public final class FP4 {
	private final FP2 a;
	private final FP2 b;
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

	public void cmove(FP4 g,int d)
	{
		a.cmove(g.a,d);
		b.cmove(g.b,d);
	}

/* test this==1 ? */
	public boolean isunity() {
		FP2 one=new FP2(1);
		return (a.equals(one) && b.iszilch());
	}

/* test is w real? That is in a+ib test b is zero */
	public boolean isreal()
	{
		return b.iszilch();
	}
/* extract real part a */
	public FP2 real()
	{
		return a;
	}

	public FP2 geta()
	{
		return a;
	}
/* extract imaginary part b */
	public FP2 getb()
	{
		return b;
	}
/* test this=x? */
	public boolean equals(FP4 x)
	{
		return (a.equals(x.a) && b.equals(x.b));
	}
/* constructors */

	public FP4()
	{
		a=new FP2();
		b=new FP2();
	}

	public FP4(int c)
	{
		a=new FP2(c);
		b=new FP2();
	}

	public FP4(FP4 x)
	{
		a=new FP2(x.a);
		b=new FP2(x.b);
	}

	public FP4(FP2 c,FP2 d)
	{
		a=new FP2(c);
		b=new FP2(d);
	}

	public FP4(FP2 c)
	{
		a=new FP2(c);
		b=new FP2();
	}

	public FP4(FP c)
	{
		a=new FP2(c);
		b=new FP2();
	}

/* copy this=x */
	public void copy(FP4 x)
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
/* set this=-this */
	public void neg()
	{
		norm();
		FP2 m=new FP2(a);
		FP2 t=new FP2();
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
	public void add(FP4 x)
	{
		a.add(x.a);
		b.add(x.b);
	}
/* this-=x */
	public void sub(FP4 x)
	{
		FP4 m=new FP4(x);
		m.neg();
		add(m);
	}

/* this*=s where s is FP2 */
	public void pmul(FP2 s)
	{
		a.mul(s);
		b.mul(s);
	}

/* this=x-this */
	public void rsub(FP4 x)
	{
		neg();
		add(x);
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
		FP2 t1=new FP2(a);
		FP2 t2=new FP2(b);
		FP2 t3=new FP2(a);

		t3.mul(b);
		t1.add(b);
		t2.mul_ip();

		t2.add(a);

		t1.norm();
		t2.norm();

		a.copy(t1);

		a.mul(t2);

		t2.copy(t3);
		t2.mul_ip();
		t2.add(t3);
		t2.norm();
		t2.neg();
		a.add(t2);

		b.copy(t3);
		b.add(t3);

		norm();
	}
/* this*=y */
	public void mul(FP4 y)
	{
		FP2 t1=new FP2(a);
		FP2 t2=new FP2(b);
		FP2 t3=new FP2();
		FP2 t4=new FP2(b);

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

		t2.mul_ip();
		a.copy(t2);
		a.add(t1);

		norm();
	}
/* convert this to hex string */
	public String toString() 
	{
		return ("["+a.toString()+","+b.toString()+"]");
	}

	public String toRawString() 
	{
		return ("["+a.toRawString()+","+b.toRawString()+"]");
	}

/* this=1/this */
	public void inverse()
	{
		FP2 t1=new FP2(a);
		FP2 t2=new FP2(b);

		t1.sqr();
		t2.sqr();
		t2.mul_ip();
		t2.norm();
		t1.sub(t2);

		t1.inverse();
		a.mul(t1);
		t1.neg();
		t1.norm();
		b.mul(t1);
	}


/* this*=i where i = sqrt(-1+sqrt(-1)) */
	public void times_i()
	{
		FP2 t=new FP2(b);
		b.copy(a);
		t.mul_ip();
		a.copy(t);
		norm();
		if (CONFIG_FIELD.TOWER==CONFIG_FIELD.POSITOWER) {
			neg();
			norm();
		}
	}

/* this=this^p using Frobenius */
	public void frob(FP2 f)
	{
		a.conj();
		b.conj();
		b.mul(f);
	}

/* this=this^e */
	public FP4 pow(BIG e)
	{
		FP4 w=new FP4(this);
		w.norm();
		BIG z=new BIG(e);
		FP4 r=new FP4(1);
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
/* XTR xtr_a function */
	public void xtr_A(FP4 w,FP4 y,FP4 z) 
	{
		FP4 r=new FP4(w);
		FP4 t=new FP4(w);
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

/* XTR xtr_d function */
	public void xtr_D() {
		FP4 w=new FP4(this);
		sqr(); w.conj();
		w.add(w);
		w.norm();
		sub(w);
		reduce();
	}

/* r=x^n using XTR method on traces of FP12s */
	public FP4 xtr_pow(BIG n) {
		FP4 sf=new FP4(this);
		sf.norm();
		FP4 a=new FP4(3);
		FP4 b=new FP4(sf);
		FP4 c=new FP4(b);
		c.xtr_D();
		FP4 t=new FP4();
		FP4 r=new FP4();

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

/* r=ck^a.cl^n using XTR double exponentiation method on traces of FP12s. See Stam thesis. */
	public FP4 xtr_pow2(FP4 ck,FP4 ckml,FP4 ckm2l,BIG a,BIG b)
	{

		BIG e=new BIG(a);
		BIG d=new BIG(b);
		BIG w=new BIG(0);
		e.norm(); d.norm();

		FP4 cu=new FP4(ck);  // can probably be passed in w/o copying
		FP4 cv=new FP4(this);
		FP4 cumv=new FP4(ckml);
		FP4 cum2v=new FP4(ckm2l);
		FP4 r=new FP4();
		FP4 t=new FP4();

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

/* this/=2 */
	public void div2()
	{
		a.div2();
		b.div2();
	}

	public void div_i()
	{
		FP2 u=new FP2(a);
		FP2 v=new FP2(b);
		u.div_ip();
		a.copy(v);
		b.copy(u);
		if (CONFIG_FIELD.TOWER==CONFIG_FIELD.POSITOWER) {
			neg();
			norm();
		}
	}

/* sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b)/2)+ib/(2*sqrt(a+sqrt(a*a-n*b*b)/2)) */
/* returns true if this is QR */
	public boolean sqrt()
	{
		if (iszilch()) return true;
		FP2 wa=new FP2(a);
		FP2 ws=new FP2(b);
		FP2 wt=new FP2(a);
		
		if (ws.iszilch())
		{
			if (wt.sqrt())
			{
				a.copy(wt);
				b.zero();
			} else {
				wt.div_ip();
				wt.sqrt();
				b.copy(wt);
				a.zero();
			}
			return true;
		}

		ws.sqr();
		wa.sqr();
		ws.mul_ip();
		ws.norm();
		wa.sub(ws);

		ws.copy(wa);
		if (!ws.sqrt()) {
			return false;
		}

		wa.copy(wt); wa.add(ws); wa.norm(); wa.div2();

		if (!wa.sqrt()) {
			wa.copy(wt); wa.sub(ws); wa.norm(); wa.div2();
			if (!wa.sqrt()) {
				return false;
			}
		}
		wt.copy(b);
		ws.copy(wa); ws.add(wa);
		ws.inverse();

		wt.mul(ws);
		a.copy(wa);
		b.copy(wt);

		return true;
	}

/* this*=s where s is FP */
	public void qmul(FP s)
	{
		a.pmul(s);
		b.pmul(s);
	}

}
