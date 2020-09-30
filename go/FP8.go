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

/* FP4 elements are of the form a+ib, where i is sqrt(-1+sqrt(-1)) */

package XXX
import "github.com/miracl/core/go/core"
//import "fmt"

type FP8 struct {
	a *FP4
	b *FP4
}

func NewFP8() *FP8 {
	F := new(FP8)
	F.a = NewFP4()
	F.b = NewFP4()
	return F
}

/* Constructors */
func NewFP8int(a int) *FP8 {
	F := new(FP8)
	F.a = NewFP4int(a)
	F.b = NewFP4()
	return F
}

/* Constructors */
func NewFP8ints(a int,b int) *FP8 {
	F := new(FP8)
	F.a = NewFP4int(a)
	F.b = NewFP4int(b)
	return F
}

func NewFP8copy(x *FP8) *FP8 {
	F := new(FP8)
	F.a = NewFP4copy(x.a)
	F.b = NewFP4copy(x.b)
	return F
}

func NewFP8fp4s(c *FP4, d *FP4) *FP8 {
	F := new(FP8)
	F.a = NewFP4copy(c)
	F.b = NewFP4copy(d)
	return F
}

func NewFP8fp4(c *FP4) *FP8 {
	F := new(FP8)
	F.a = NewFP4copy(c)
	F.b = NewFP4()
	return F
}

func NewFP8fp(c *FP) *FP8 {
	F := new(FP8)
	F.a = NewFP4fp(c)
	F.b = NewFP4()
	return F
}

func NewFP8rand(rng *core.RAND) *FP8 {
	F := NewFP8fp4s(NewFP4rand(rng),NewFP4rand(rng))
	return F
}

/* reduce all components of this mod Modulus */
func (F *FP8) reduce() {
	F.a.reduce()
	F.b.reduce()
}

/* normalise all components of this mod Modulus */
func (F *FP8) norm() {
	F.a.norm()
	F.b.norm()
}

/* test this==0 ? */
func (F *FP8) iszilch() bool {
	return F.a.iszilch() && F.b.iszilch()
}


func (F *FP8) islarger() int {
    if F.iszilch() {
		return 0;
	}
	cmp:=F.b.islarger()
	if cmp!=0 {
		return cmp
	}
	return F.a.islarger()
}

func (F *FP8) ToBytes(bf []byte) {
	var t [4*int(MODBYTES)]byte
	MB := 4*int(MODBYTES)
	F.b.ToBytes(t[:]);
	for i:=0;i<MB;i++ {
		bf[i]=t[i];
	}
	F.a.ToBytes(t[:]);
	for i:=0;i<MB;i++ {
		bf[i+MB]=t[i];
	}
}

func FP8_fromBytes(bf []byte) *FP8 {
	var t [4*int(MODBYTES)]byte
	MB := 4*int(MODBYTES)
	for i:=0;i<MB;i++ {
        t[i]=bf[i];
	}
    tb:=FP4_fromBytes(t[:])
	for i:=0;i<MB;i++ {
        t[i]=bf[i+MB]
	}
    ta:=FP4_fromBytes(t[:])
	return NewFP8fp4s(ta,tb)
}


/* Conditional move */
func (F *FP8) cmove(g *FP8, d int) {
	F.a.cmove(g.a, d)
	F.b.cmove(g.b, d)
}

/* test this==1 ? */
func (F *FP8) isunity() bool {
	one := NewFP4int(1)
	return F.a.Equals(one) && F.b.iszilch()
}

/* test is w real? That is in a+ib test b is zero */
func (F *FP8) isreal() bool {
	return F.b.iszilch()
}

/* extract real part a */
func (F *FP8) real() *FP4 {
	return F.a
}

func (F *FP8) geta() *FP4 {
	return F.a
}

/* extract imaginary part b */
func (F *FP8) getb() *FP4 {
	return F.b
}

/* test this=x? */
func (F *FP8) Equals(x *FP8) bool {
	return (F.a.Equals(x.a) && F.b.Equals(x.b))
}

/* copy this=x */
func (F *FP8) copy(x *FP8) {
	F.a.copy(x.a)
	F.b.copy(x.b)
}

/* set this=0 */
func (F *FP8) zero() {
	F.a.zero()
	F.b.zero()
}

/* set this=1 */
func (F *FP8) one() {
	F.a.one()
	F.b.zero()
}

/* Return sign */
func (F *FP8) sign() int {
	p1 := F.a.sign();
	p2 := F.b.sign();
	var u int
	if BIG_ENDIAN_SIGN {
		if F.b.iszilch() {
			u=1;
		} else {
			u=0;
		}
		p2^=(p1^p2)&u;
		return p2;
	} else {
		if F.a.iszilch() {
			u=1;
		} else {
			u=0;
		}
		p1^=(p1^p2)&u;
		return p1;
	}
}

/* set this=-this */
func (F *FP8) neg() {
	F.norm()
	m := NewFP4copy(F.a)
	t := NewFP4()
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
func (F *FP8) conj() {
	F.b.neg()
	F.norm()
}

/* this=-conjugate(this) */
func (F *FP8) nconj() {
	F.a.neg()
	F.norm()
}

/* this+=x */
func (F *FP8) add(x *FP8) {
	F.a.add(x.a)
	F.b.add(x.b)
}

/* this-=x */
func (F *FP8) sub(x *FP8) {
	m := NewFP8copy(x)
	m.neg()
	F.add(m)
}

/* this-=x */
func (F *FP8) rsub(x *FP8) {
	F.neg()
	F.add(x)
}

/* this*=s where s is FP4 */
func (F *FP8) pmul(s *FP4) {
	F.a.mul(s)
	F.b.mul(s)
}

/* this*=s where s is FP2 */
func (F *FP8) qmul(s *FP2) {
	F.a.pmul(s)
	F.b.pmul(s)
}

/* this*=s where s is FP */
func (F *FP8) tmul(s *FP) {
	F.a.qmul(s)
	F.b.qmul(s)
}

/* this*=c where c is int */
func (F *FP8) imul(c int) {
	F.a.imul(c)
	F.b.imul(c)
}

/* this*=this */
func (F *FP8) sqr() {
	t1 := NewFP4copy(F.a)
	t2 := NewFP4copy(F.b)
	t3 := NewFP4copy(F.a)

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
func (F *FP8) mul(y *FP8) {
	t1 := NewFP4copy(F.a)
	t2 := NewFP4copy(F.b)
	t3 := NewFP4()
	t4 := NewFP4copy(F.b)

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
func (F *FP8) toString() string {
	return ("[" + F.a.toString() + "," + F.b.toString() + "]")
}

/* this=1/this */
func (F *FP8) inverse(h *FP) {
	t1 := NewFP4copy(F.a)
	t2 := NewFP4copy(F.b)

	t1.sqr()
	t2.sqr()
	t2.times_i()
	t2.norm()
	t1.sub(t2)
	t1.norm()

	t1.inverse(h)

	F.a.mul(t1)
	t1.neg()
	t1.norm()
	F.b.mul(t1)
}

/* this*=i where i = sqrt(sqrt(-1+sqrt(-1))) */
func (F *FP8) times_i() {
	s := NewFP4copy(F.b)
	t := NewFP4copy(F.a)
	s.times_i()
	F.a.copy(s)
	F.b.copy(t)
	F.norm()
	if TOWER == POSITOWER {
		F.neg()
		F.norm()
	}
}

func (F *FP8) times_i2() {
	F.a.times_i()
	F.b.times_i()
}

/* this=this^p using Frobenius */
func (F *FP8) frob(f *FP2) {
	ff := NewFP2copy(f)
	ff.sqr()
	ff.mul_ip()
	ff.norm()

	F.a.frob(ff)
	F.b.frob(ff)
	F.b.pmul(f)
	F.b.times_i()
}

/* this=this^e 
func (F *FP8) pow(e *BIG) *FP8 {
	w := NewFP8copy(F)
	w.norm()
	z := NewBIGcopy(e)
	r := NewFP8int(1)
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
} */

/* XTR xtr_a function */
/*
func (F *FP8) xtr_A(w *FP8, y *FP8, z *FP8) {
	r := NewFP8copy(w)
	t := NewFP8copy(w)
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
func (F *FP8) xtr_D() {
	w := NewFP8copy(F)
	F.sqr()
	w.conj()
	w.add(w)
	w.norm()
	F.sub(w)
	F.reduce()
}
*/
/* r=x^n using XTR method on traces of FP24s */
/*
func (F *FP8) xtr_pow(n *BIG) *FP8 {
	a := NewFP8int(3)
	b := NewFP8copy(F)
	c := NewFP8copy(b)
	c.xtr_D()
	t := NewFP8()
	r := NewFP8()
	sf := NewFP8copy(F)
	sf.norm()

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
/* r=ck^a.cl^n using XTR double exponentiation method on traces of FP24s. See Stam thesis. */
/*
func (F *FP8) xtr_pow2(ck *FP8, ckml *FP8, ckm2l *FP8, a *BIG, b *BIG) *FP8 {

	e := NewBIGcopy(a)
	d := NewBIGcopy(b)
	w := NewBIGint(0)
	e.norm()
	d.norm()
	cu := NewFP8copy(ck) // can probably be passed in w/o copying
	cv := NewFP8copy(F)
	cumv := NewFP8copy(ckml)
	cum2v := NewFP8copy(ckm2l)
	r := NewFP8()
	t := NewFP8()

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
/* this/=2 */
func (F *FP8) div2() {
	F.a.div2()
	F.b.div2()
}

func (F *FP8) div_i() {
	u := NewFP4copy(F.a)
	v := NewFP4copy(F.b)
	u.div_i()
	F.a.copy(v)
	F.b.copy(u)
	if TOWER == POSITOWER {
		F.neg()
		F.norm()
	}
}
/*
func (F *FP8) pow(b *BIG) {
	w := NewFP8copy(F);
	r := NewFP8int(1)
	z := NewBIGcopy(b)
	for true {
		bt := z.parity()
		z.shr(1)
		if bt==1 {
			r.mul(w)
		}
		if z.iszilch() {break}
		w.sqr()
	}
	r.reduce();
	F.copy(r);
}
*/

/* PFGE48S
// Test for Quadratic Residue 
func (F *FP8) qr(h *FP) int {
	c := NewFP8copy(F)
	c.conj()
	c.mul(F)
	return c.a.qr(h)
}


// sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b)/2)+ib/(2*sqrt(a+sqrt(a*a-n*b*b)/2)) 
func (F *FP8) sqrt(h *FP)  {
	if F.iszilch() {
		return 
	}

	a := NewFP4copy(F.a)
	b := NewFP4()
	s := NewFP4copy(F.b)
	t := NewFP4copy(F.a)
	hint := NewFP()

	s.sqr()
	a.sqr()
	s.times_i()
	s.norm()
	a.sub(s)

	s.copy(a); s.norm()

	s.sqrt(h)
	a.copy(t)
	b.copy(t)

	a.add(s)
	a.norm()
	a.div2()

    b.copy(F.b); b.div2()
    qr:=a.qr(hint)

// tweak hint - multiply old hint by Norm(1/Beta)^e where Beta is irreducible polynomial
    s.copy(a)
	twk:=NewFPbig(NewBIGints(TWK))
    twk.mul(hint)
    s.div_i(); s.norm()

    a.cmove(s,1-qr)
    hint.cmove(twk,1-qr)

    F.a.copy(a); F.a.sqrt(hint)
    s.copy(a); s.inverse(hint)
    s.mul(F.a)
    F.b.copy(s); F.b.mul(b)
    t.copy(F.a);

    F.a.cmove(F.b,1-qr);
    F.b.cmove(t,1-qr);

	sgn:=F.sign()
	nr:=NewFP8copy(F)
	nr.neg(); nr.norm()
	F.cmove(nr,sgn)
}
PFGE48F */