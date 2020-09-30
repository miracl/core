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

/* FP4 elements are of the form a+ib, where i is sqrt(-1+sqrt(-1)) */

package XXX

//import "fmt"

type FP16 struct {
	a *FP8
	b *FP8
}

func NewFP16() *FP16 {
	F := new(FP16)
	F.a = NewFP8()
	F.b = NewFP8()
	return F
}

/* Constructors */
func NewFP16int(a int) *FP16 {
	F := new(FP16)
	F.a = NewFP8int(a)
	F.b = NewFP8()
	return F
}

func NewFP16copy(x *FP16) *FP16 {
	F := new(FP16)
	F.a = NewFP8copy(x.a)
	F.b = NewFP8copy(x.b)
	return F
}

func NewFP16fp8s(c *FP8, d *FP8) *FP16 {
	F := new(FP16)
	F.a = NewFP8copy(c)
	F.b = NewFP8copy(d)
	return F
}

func NewFP16fp8(c *FP8) *FP16 {
	F := new(FP16)
	F.a = NewFP8copy(c)
	F.b = NewFP8()
	return F
}

/* reduce all components of this mod Modulus */
func (F *FP16) reduce() {
	F.a.reduce()
	F.b.reduce()
}

/* normalise all components of this mod Modulus */
func (F *FP16) norm() {
	F.a.norm()
	F.b.norm()
}

/* test this==0 ? */
func (F *FP16) iszilch() bool {
	return F.a.iszilch() && F.b.iszilch()
}

func (F *FP16) ToBytes(bf []byte) {
	var t [8*int(MODBYTES)]byte
	MB := 8*int(MODBYTES)
	F.b.ToBytes(t[:]);
	for i:=0;i<MB;i++ {
		bf[i]=t[i];
	}
	F.a.ToBytes(t[:]);
	for i:=0;i<MB;i++ {
		bf[i+MB]=t[i];
	}
}

func FP16_fromBytes(bf []byte) *FP16 {
	var t [8*int(MODBYTES)]byte
	MB := 8*int(MODBYTES)
	for i:=0;i<MB;i++ {
        t[i]=bf[i];
	}
    tb:=FP8_fromBytes(t[:])
	for i:=0;i<MB;i++ {
        t[i]=bf[i+MB]
	}
    ta:=FP8_fromBytes(t[:])
	return NewFP16fp8s(ta,tb)
}

/* Conditional move */
func (F *FP16) cmove(g *FP16, d int) {
	F.a.cmove(g.a, d)
	F.b.cmove(g.b, d)
}

/* test this==1 ? */
func (F *FP16) isunity() bool {
	one := NewFP8int(1)
	return F.a.Equals(one) && F.b.iszilch()
}

/* test is w real? That is in a+ib test b is zero */
func (F *FP16) isreal() bool {
	return F.b.iszilch()
}

/* extract real part a */
func (F *FP16) real() *FP8 {
	return F.a
}

func (F *FP16) geta() *FP8 {
	return F.a
}

/* extract imaginary part b */
func (F *FP16) getb() *FP8 {
	return F.b
}

/* test this=x? */
func (F *FP16) Equals(x *FP16) bool {
	return (F.a.Equals(x.a) && F.b.Equals(x.b))
}

/* copy this=x */
func (F *FP16) copy(x *FP16) {
	F.a.copy(x.a)
	F.b.copy(x.b)
}

/* set this=0 */
func (F *FP16) zero() {
	F.a.zero()
	F.b.zero()
}

/* set this=1 */
func (F *FP16) one() {
	F.a.one()
	F.b.zero()
}

/* set this=-this */
func (F *FP16) neg() {
	F.norm()
	m := NewFP8copy(F.a)
	t := NewFP8()
	m.add(F.b)
	m.neg()
	t.copy(m)
	t.add(F.b)
	F.b.copy(m)
	F.b.add(F.a)
	F.a.copy(t)
	F.norm()
}

/* this=conjugate(this) */
func (F *FP16) conj() {
	F.b.neg()
	F.norm()
}

/* this=-conjugate(this) */
func (F *FP16) nconj() {
	F.a.neg()
	F.norm()
}

/* this+=x */
func (F *FP16) add(x *FP16) {
	F.a.add(x.a)
	F.b.add(x.b)
}

/* this-=x */
func (F *FP16) sub(x *FP16) {
	m := NewFP16copy(x)
	m.neg()
	F.add(m)
}

/* this-=x */
func (F *FP16) rsub(x *FP16) {
	F.neg()
	F.add(x)
}

/* this*=s where s is FP8 */
func (F *FP16) pmul(s *FP8) {
	F.a.mul(s)
	F.b.mul(s)
}

/* this*=s where s is FP2 */
func (F *FP16) qmul(s *FP2) {
	F.a.qmul(s)
	F.b.qmul(s)
}

/* this*=s where s is FP */
func (F *FP16) tmul(s *FP) {
	F.a.tmul(s)
	F.b.tmul(s)
}

/* this*=c where c is int */
func (F *FP16) imul(c int) {
	F.a.imul(c)
	F.b.imul(c)
}

/* this*=this */
func (F *FP16) sqr() {
	t1 := NewFP8copy(F.a)
	t2 := NewFP8copy(F.b)
	t3 := NewFP8copy(F.a)

	t3.mul(F.b)
	t1.add(F.b)
	t2.times_i()

	t2.add(F.a)

	t1.norm()
	t2.norm()

	F.a.copy(t1)
	F.a.mul(t2)

	t2.copy(t3)
	t2.times_i()
	t2.add(t3)
	t2.norm()
	t2.neg()
	F.a.add(t2)

	F.b.copy(t3)
	F.b.add(t3)

	F.norm()
}

/* this*=y */
func (F *FP16) mul(y *FP16) {
	t1 := NewFP8copy(F.a)
	t2 := NewFP8copy(F.b)
	t3 := NewFP8()
	t4 := NewFP8copy(F.b)

	t1.mul(y.a)
	t2.mul(y.b)
	t3.copy(y.b)
	t3.add(y.a)
	t4.add(F.a)

	t3.norm()
	t4.norm()

	t4.mul(t3)

	t3.copy(t1)
	t3.neg()
	t4.add(t3)
	t4.norm()

	t3.copy(t2)
	t3.neg()
	F.b.copy(t4)
	F.b.add(t3)

	t2.times_i()
	F.a.copy(t2)
	F.a.add(t1)

	F.norm()
}

/* convert this to hex string */
func (F *FP16) toString() string {
	return ("[" + F.a.toString() + "," + F.b.toString() + "]")
}

/* this=1/this */
func (F *FP16) inverse() {
	t1 := NewFP8copy(F.a)
	t2 := NewFP8copy(F.b)

	t1.sqr()
	t2.sqr()
	t2.times_i()
	t2.norm()
	t1.sub(t2)
	t1.norm()

	t1.inverse(nil)

	F.a.mul(t1)
	t1.neg()
	t1.norm()
	F.b.mul(t1)
}

/* this*=i where i = sqrt(sqrt(-1+sqrt(-1))) */
func (F *FP16) times_i() {
	s := NewFP8copy(F.b)
	t := NewFP8copy(F.a)
	s.times_i()
	F.a.copy(s)
	F.b.copy(t)
	F.norm()
}

func (F *FP16) times_i2() {
	F.a.times_i()
	F.b.times_i()
}

func (F *FP16) times_i4() {
	F.a.times_i2()
	F.b.times_i2()
}

/* this=this^p using Frobenius */
func (F *FP16) frob(f *FP2) {
	ff := NewFP2copy(f)
	ff.sqr()
	ff.norm()

	F.a.frob(ff)
	F.b.frob(ff)
	F.b.qmul(f)
	F.b.times_i()

}

/* this=this^e */
func (F *FP16) pow(e *BIG) *FP16 {
	w := NewFP16copy(F)
	w.norm()
	z := NewBIGcopy(e)
	r := NewFP16int(1)
	z.norm()
	for true {
		bt := z.parity()
		z.fshr(1)
		if bt == 1 {
			r.mul(w)
		}
		if z.iszilch() {
			break
		}
		w.sqr()
	}
	r.reduce()
	return r
}

/* XTR xtr_a function */
/*
func (F *FP16) xtr_A(w *FP16, y *FP16, z *FP16) {
	r := NewFP16copy(w)
	t := NewFP16copy(w)
	r.sub(y)
	r.norm()
	r.pmul(F.a)
	t.add(y)
	t.norm()
	t.pmul(F.b)
	t.times_i()

	F.copy(r)
	F.add(t)
	F.add(z)

	F.norm()
}
*/
/* XTR xtr_d function */
/*
func (F *FP16) xtr_D() {
	w := NewFP16copy(F)
	F.sqr()
	w.conj()
	w.add(w)
	w.norm()
	F.sub(w)
	F.reduce()
}
*/
/* r=x^n using XTR method on traces of FP48s */
/*
func (F *FP16) xtr_pow(n *BIG) *FP16 {
	sf := NewFP16copy(F)
	sf.norm()
	a := NewFP16int(3)
	b := NewFP16copy(sf)
	c := NewFP16copy(b)
	c.xtr_D()
	t := NewFP16()
	r := NewFP16()

	par := n.parity()
	v := NewBIGcopy(n)
	v.norm()
	v.fshr(1)
	if par == 0 {
		v.dec(1)
		v.norm()
	}

	nb := v.nbits()
	for i := nb - 1; i >= 0; i-- {
		if v.bit(i) != 1 {
			t.copy(b)
			sf.conj()
			c.conj()
			b.xtr_A(a, sf, c)
			sf.conj()
			c.copy(t)
			c.xtr_D()
			a.xtr_D()
		} else {
			t.copy(a)
			t.conj()
			a.copy(b)
			a.xtr_D()
			b.xtr_A(c, sf, t)
			c.xtr_D()
		}
	}
	if par == 0 {
		r.copy(c)
	} else {
		r.copy(b)
	}
	r.reduce()
	return r
}
*/
/* r=ck^a.cl^n using XTR double exponentiation method on traces of FP48s. See Stam thesis. */
/*
func (F *FP16) xtr_pow2(ck *FP16, ckml *FP16, ckm2l *FP16, a *BIG, b *BIG) *FP16 {

	e := NewBIGcopy(a)
	d := NewBIGcopy(b)
	w := NewBIGint(0)
	e.norm()
	d.norm()
	cu := NewFP16copy(ck) // can probably be passed in w/o copying
	cv := NewFP16copy(F)
	cumv := NewFP16copy(ckml)
	cum2v := NewFP16copy(ckm2l)
	r := NewFP16()
	t := NewFP16()

	f2 := 0
	for d.parity() == 0 && e.parity() == 0 {
		d.fshr(1)
		e.fshr(1)
		f2++
	}

	for Comp(d, e) != 0 {
		if Comp(d, e) > 0 {
			w.copy(e)
			w.imul(4)
			w.norm()
			if Comp(d, w) <= 0 {
				w.copy(d)
				d.copy(e)
				e.rsub(w)
				e.norm()

				t.copy(cv)
				t.xtr_A(cu, cumv, cum2v)
				cum2v.copy(cumv)
				cum2v.conj()
				cumv.copy(cv)
				cv.copy(cu)
				cu.copy(t)
			} else {
				if d.parity() == 0 {
					d.fshr(1)
					r.copy(cum2v)
					r.conj()
					t.copy(cumv)
					t.xtr_A(cu, cv, r)
					cum2v.copy(cumv)
					cum2v.xtr_D()
					cumv.copy(t)
					cu.xtr_D()
				} else {
					if e.parity() == 1 {
						d.sub(e)
						d.norm()
						d.fshr(1)
						t.copy(cv)
						t.xtr_A(cu, cumv, cum2v)
						cu.xtr_D()
						cum2v.copy(cv)
						cum2v.xtr_D()
						cum2v.conj()
						cv.copy(t)
					} else {
						w.copy(d)
						d.copy(e)
						d.fshr(1)
						e.copy(w)
						t.copy(cumv)
						t.xtr_D()
						cumv.copy(cum2v)
						cumv.conj()
						cum2v.copy(t)
						cum2v.conj()
						t.copy(cv)
						t.xtr_D()
						cv.copy(cu)
						cu.copy(t)
					}
				}
			}
		}
		if Comp(d, e) < 0 {
			w.copy(d)
			w.imul(4)
			w.norm()
			if Comp(e, w) <= 0 {
				e.sub(d)
				e.norm()
				t.copy(cv)
				t.xtr_A(cu, cumv, cum2v)
				cum2v.copy(cumv)
				cumv.copy(cu)
				cu.copy(t)
			} else {
				if e.parity() == 0 {
					w.copy(d)
					d.copy(e)
					d.fshr(1)
					e.copy(w)
					t.copy(cumv)
					t.xtr_D()
					cumv.copy(cum2v)
					cumv.conj()
					cum2v.copy(t)
					cum2v.conj()
					t.copy(cv)
					t.xtr_D()
					cv.copy(cu)
					cu.copy(t)
				} else {
					if d.parity() == 1 {
						w.copy(e)
						e.copy(d)
						w.sub(d)
						w.norm()
						d.copy(w)
						d.fshr(1)
						t.copy(cv)
						t.xtr_A(cu, cumv, cum2v)
						cumv.conj()
						cum2v.copy(cu)
						cum2v.xtr_D()
						cum2v.conj()
						cu.copy(cv)
						cu.xtr_D()
						cv.copy(t)
					} else {
						d.fshr(1)
						r.copy(cum2v)
						r.conj()
						t.copy(cumv)
						t.xtr_A(cu, cv, r)
						cum2v.copy(cumv)
						cum2v.xtr_D()
						cumv.copy(t)
						cu.xtr_D()
					}
				}
			}
		}
	}
	r.copy(cv)
	r.xtr_A(cu, cumv, cum2v)
	for i := 0; i < f2; i++ {
		r.xtr_D()
	}
	r = r.xtr_pow(d)
	return r
}
*/
