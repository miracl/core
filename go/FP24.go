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

/* MiotCL Fp^12 functions */
/* FP12 elements are of the form a+i.b+i^2.c */

package XXX

//import "fmt"

type FP24 struct {
	a     *FP8
	b     *FP8
	c     *FP8
	stype int
}

/* Constructors */
func NewFP24fp8(d *FP8) *FP24 {
	F := new(FP24)
	F.a = NewFP8copy(d)
	F.b = NewFP8()
	F.c = NewFP8()
	F.stype = FP_SPARSEST
	return F
}

func NewFP24() *FP24 {
	F := new(FP24)
	F.a = NewFP8()
	F.b = NewFP8()
	F.c = NewFP8()
	F.stype = FP_ZERO
	return F
}

func NewFP24int(d int) *FP24 {
	F := new(FP24)
	F.a = NewFP8int(d)
	F.b = NewFP8()
	F.c = NewFP8()
	if d == 1 {
		F.stype = FP_ONE
	} else {
		F.stype = FP_SPARSEST
	}
	return F
}

func NewFP24fp8s(d *FP8, e *FP8, f *FP8) *FP24 {
	F := new(FP24)
	F.a = NewFP8copy(d)
	F.b = NewFP8copy(e)
	F.c = NewFP8copy(f)
	F.stype = FP_DENSE
	return F
}

func NewFP24copy(x *FP24) *FP24 {
	F := new(FP24)
	F.a = NewFP8copy(x.a)
	F.b = NewFP8copy(x.b)
	F.c = NewFP8copy(x.c)
	F.stype = x.stype
	return F
}

/* reduce all components of this mod Modulus */
func (F *FP24) reduce() {
	F.a.reduce()
	F.b.reduce()
	F.c.reduce()
}

/* normalise all components of this */
func (F *FP24) norm() {
	F.a.norm()
	F.b.norm()
	F.c.norm()
}

/* test x==0 ? */
func (F *FP24) iszilch() bool {
	return (F.a.iszilch() && F.b.iszilch() && F.c.iszilch())
}

/* Conditional move */
func (F *FP24) cmove(g *FP24, d int) {
	F.a.cmove(g.a, d)
	F.b.cmove(g.b, d)
	F.c.cmove(g.c, d)
	d = ^(d - 1)
	F.stype ^= (F.stype ^ g.stype) & d
}

/* Constant time select from pre-computed table */
func (F *FP24) selector(g []*FP24, b int32) {

	m := b >> 31
	babs := (b ^ m) - m

	babs = (babs - 1) / 2

	F.cmove(g[0], teq(babs, 0)) // conditional move
	F.cmove(g[1], teq(babs, 1))
	F.cmove(g[2], teq(babs, 2))
	F.cmove(g[3], teq(babs, 3))
	F.cmove(g[4], teq(babs, 4))
	F.cmove(g[5], teq(babs, 5))
	F.cmove(g[6], teq(babs, 6))
	F.cmove(g[7], teq(babs, 7))

	invF := NewFP24copy(F)
	invF.conj()
	F.cmove(invF, int(m&1))
}

/* test x==1 ? */
func (F *FP24) Isunity() bool {
	one := NewFP8int(1)
	return (F.a.Equals(one) && F.b.iszilch() && F.c.iszilch())
}

/* return 1 if x==y, else 0 */
func (F *FP24) Equals(x *FP24) bool {
	return (F.a.Equals(x.a) && F.b.Equals(x.b) && F.c.Equals(x.c))
}

/* extract a from this */
func (F *FP24) geta() *FP8 {
	return F.a
}

/* extract b */
func (F *FP24) getb() *FP8 {
	return F.b
}

/* extract c */
func (F *FP24) getc() *FP8 {
	return F.c
}

/* copy this=x */
func (F *FP24) Copy(x *FP24) {
	F.a.copy(x.a)
	F.b.copy(x.b)
	F.c.copy(x.c)
	F.stype = x.stype
}

/* set this=1 */
func (F *FP24) one() {
	F.a.one()
	F.b.zero()
	F.c.zero()
	F.stype = FP_ONE
}

/* set this=0 */
func (F *FP24) zero() {
	F.a.zero()
	F.b.zero()
	F.c.zero()
	F.stype = FP_ZERO
}

/* this=conj(this) */
func (F *FP24) conj() {
	F.a.conj()
	F.b.nconj()
	F.c.conj()
}

/* Granger-Scott Unitary Squaring */
func (F *FP24) usqr() {
	A := NewFP8copy(F.a)
	B := NewFP8copy(F.c)
	C := NewFP8copy(F.b)
	D := NewFP8()

	F.a.sqr()
	D.copy(F.a)
	D.add(F.a)
	F.a.add(D)

	F.a.norm()
	A.nconj()

	A.add(A)
	F.a.add(A)
	B.sqr()
	B.times_i()

	D.copy(B)
	D.add(B)
	B.add(D)
	B.norm()

	C.sqr()
	D.copy(C)
	D.add(C)
	C.add(D)
	C.norm()

	F.b.conj()
	F.b.add(F.b)
	F.c.nconj()

	F.c.add(F.c)
	F.b.add(B)
	F.c.add(C)
	F.reduce()
	F.stype = FP_DENSE
}

/* Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */
func (F *FP24) sqr() {
	if F.stype == FP_ONE {
		return
	}
	A := NewFP8copy(F.a)
	B := NewFP8copy(F.b)
	C := NewFP8copy(F.c)
	D := NewFP8copy(F.a)

	A.sqr()
	B.mul(F.c)
	B.add(B)
	B.norm()
	C.sqr()
	D.mul(F.b)
	D.add(D)

	F.c.add(F.a)
	F.c.add(F.b)
	F.c.norm()
	F.c.sqr()

	F.a.copy(A)

	A.add(B)
	A.norm()
	A.add(C)
	A.add(D)
	A.norm()

	A.neg()
	B.times_i()
	C.times_i()

	F.a.add(B)

	F.b.copy(C)
	F.b.add(D)
	F.c.add(A)
	if F.stype == FP_SPARSER || F.stype == FP_SPARSEST {
		F.stype = FP_SPARSE
	} else {
		F.stype = FP_DENSE
	}
	F.norm()
}

/* FP24 full multiplication this=this*y */
func (F *FP24) Mul(y *FP24) {
	z0 := NewFP8copy(F.a)
	z1 := NewFP8()
	z2 := NewFP8copy(F.b)
	z3 := NewFP8()
	t0 := NewFP8copy(F.a)
	t1 := NewFP8copy(y.a)

	z0.mul(y.a)
	z2.mul(y.b)

	t0.add(F.b)
	t0.norm()
	t1.add(y.b)
	t1.norm()

	z1.copy(t0)
	z1.mul(t1)
	t0.copy(F.b)
	t0.add(F.c)
	t0.norm()

	t1.copy(y.b)
	t1.add(y.c)
	t1.norm()
	z3.copy(t0)
	z3.mul(t1)

	t0.copy(z0)
	t0.neg()
	t1.copy(z2)
	t1.neg()

	z1.add(t0)
	//z1.norm();
	F.b.copy(z1)
	F.b.add(t1)

	z3.add(t1)
	z2.add(t0)

	t0.copy(F.a)
	t0.add(F.c)
	t0.norm()
	t1.copy(y.a)
	t1.add(y.c)
	t1.norm()
	t0.mul(t1)
	z2.add(t0)

	t0.copy(F.c)
	t0.mul(y.c)
	t1.copy(t0)
	t1.neg()

	F.c.copy(z2)
	F.c.add(t1)
	z3.add(t1)
	t0.times_i()
	F.b.add(t0)
	z3.norm()
	z3.times_i()
	F.a.copy(z0)
	F.a.add(z3)
	F.stype = FP_DENSE
	F.norm()
}

/* FP24 full multiplication w=w*y */
/* Supports sparse multiplicands */
/* Usually w is denser than y */
func (F *FP24) ssmul(y *FP24) {
	if F.stype == FP_ONE {
		F.Copy(y)
		return
	}
	if y.stype == FP_ONE {
		return
	}
	if y.stype >= FP_SPARSE {
		z0 := NewFP8copy(F.a)
		z1 := NewFP8()
		z2 := NewFP8()
		z3 := NewFP8()
		z0.mul(y.a)

		if SEXTIC_TWIST == M_TYPE {
			if y.stype == FP_SPARSE || F.stype == FP_SPARSE {
				z2.getb().copy(F.b.getb())
				z2.getb().mul(y.b.getb())
				z2.geta().zero()
				if y.stype != FP_SPARSE {
					z2.geta().copy(F.b.getb())
					z2.geta().mul(y.b.geta())
				}
				if F.stype != FP_SPARSE {
					z2.geta().copy(F.b.geta())
					z2.geta().mul(y.b.getb())
				}
				z2.times_i()
			} else {
				z2.copy(F.b)
				z2.mul(y.b)
			}
		} else {
			z2.copy(F.b)
			z2.mul(y.b)
		}
		t0 := NewFP8copy(F.a)
		t1 := NewFP8copy(y.a)
		t0.add(F.b)
		t0.norm()
		t1.add(y.b)
		t1.norm()

		z1.copy(t0)
		z1.mul(t1)
		t0.copy(F.b)
		t0.add(F.c)
		t0.norm()
		t1.copy(y.b)
		t1.add(y.c)
		t1.norm()

		z3.copy(t0)
		z3.mul(t1)

		t0.copy(z0)
		t0.neg()
		t1.copy(z2)
		t1.neg()

		z1.add(t0)
		F.b.copy(z1)
		F.b.add(t1)

		z3.add(t1)
		z2.add(t0)

		t0.copy(F.a)
		t0.add(F.c)
		t0.norm()
		t1.copy(y.a)
		t1.add(y.c)
		t1.norm()

		t0.mul(t1)
		z2.add(t0)

		if SEXTIC_TWIST == D_TYPE {
			if y.stype == FP_SPARSE || F.stype == FP_SPARSE {
				t0.geta().copy(F.c.geta())
				t0.geta().mul(y.c.geta())
				t0.getb().zero()
				if y.stype != FP_SPARSE {
					t0.getb().copy(F.c.geta())
					t0.getb().mul(y.c.getb())
				}
				if F.stype != FP_SPARSE {
					t0.getb().copy(F.c.getb())
					t0.getb().mul(y.c.geta())
				}
			} else {
				t0.copy(F.c)
				t0.mul(y.c)
			}
		} else {
			t0.copy(F.c)
			t0.mul(y.c)
		}
		t1.copy(t0)
		t1.neg()

		F.c.copy(z2)
		F.c.add(t1)
		z3.add(t1)
		t0.times_i()
		F.b.add(t0)
		z3.norm()
		z3.times_i()
		F.a.copy(z0)
		F.a.add(z3)
	} else {
		if F.stype == FP_SPARSER || F.stype == FP_SPARSEST {
			F.smul(y)
			return
		}
		if SEXTIC_TWIST == D_TYPE { // dense by sparser - 13m
			z0 := NewFP8copy(F.a)
			z2 := NewFP8copy(F.b)
			z3 := NewFP8copy(F.b)
			t0 := NewFP8()
			t1 := NewFP8copy(y.a)
			z0.mul(y.a)

			if y.stype == FP_SPARSEST {
				z2.tmul(y.b.a.a.a)
			} else {
				z2.pmul(y.b.geta())
			}
			F.b.add(F.a)
			t1.geta().add(y.b.geta())

			t1.norm()
			F.b.norm()
			F.b.mul(t1)
			z3.add(F.c)
			z3.norm()

			if y.stype == FP_SPARSEST {
				z3.tmul(y.b.a.a.a)
			} else {
				z3.pmul(y.b.geta())
			}
			t0.copy(z0)
			t0.neg()
			t1.copy(z2)
			t1.neg()

			F.b.add(t0)

			F.b.add(t1)
			z3.add(t1)
			z2.add(t0)

			t0.copy(F.a)
			t0.add(F.c)
			t0.norm()
			z3.norm()
			t0.mul(y.a)
			F.c.copy(z2)
			F.c.add(t0)

			z3.times_i()
			F.a.copy(z0)
			F.a.add(z3)
		}
		if SEXTIC_TWIST == M_TYPE {
			z0 := NewFP8copy(F.a)
			z1 := NewFP8()
			z2 := NewFP8()
			z3 := NewFP8()
			t0 := NewFP8copy(F.a)
			t1 := NewFP8()

			z0.mul(y.a)
			t0.add(F.b)
			t0.norm()

			z1.copy(t0)
			z1.mul(y.a)
			t0.copy(F.b)
			t0.add(F.c)
			t0.norm()

			z3.copy(t0)

			if y.stype == FP_SPARSEST {
				z3.tmul(y.c.b.a.a)
			} else {
				z3.pmul(y.c.getb())
			}
			z3.times_i()

			t0.copy(z0)
			t0.neg()
			z1.add(t0)
			F.b.copy(z1)
			z2.copy(t0)

			t0.copy(F.a)
			t0.add(F.c)
			t0.norm()
			t1.copy(y.a)
			t1.add(y.c)
			t1.norm()

			t0.mul(t1)
			z2.add(t0)
			t0.copy(F.c)
			if y.stype == FP_SPARSEST {
				t0.tmul(y.c.b.a.a)
			} else {
				t0.pmul(y.c.getb())
			}
			t0.times_i()
			t1.copy(t0)
			t1.neg()

			F.c.copy(z2)
			F.c.add(t1)
			z3.add(t1)
			t0.times_i()
			F.b.add(t0)
			z3.norm()
			z3.times_i()
			F.a.copy(z0)
			F.a.add(z3)
		}
	}
	F.stype = FP_DENSE
	F.norm()
}

/* Special case of multiplication arises from special form of ATE pairing line function */
func (F *FP24) smul(y *FP24) {
	if SEXTIC_TWIST == D_TYPE {
		w1 := NewFP4copy(F.a.geta())
		w2 := NewFP4copy(F.a.getb())
		var w3 *FP4

		w1.mul(y.a.geta())
		w2.mul(y.a.getb())

		if y.stype == FP_SPARSEST || F.stype == FP_SPARSEST {
			if y.stype == FP_SPARSEST && F.stype == FP_SPARSEST {
				t := NewFPcopy(F.b.a.a.a)
				t.mul(y.b.a.a.a)
				w3 = NewFP4fp(t)
			} else {
				if y.stype != FP_SPARSEST {
					w3 = NewFP4copy(y.b.geta())
					w3.qmul(F.b.a.a.a)
				} else {
					w3 = NewFP4copy(F.b.geta())
					w3.qmul(y.b.a.a.a)
				}
			}
		} else {
			w3 = NewFP4copy(F.b.geta())
			w3.mul(y.b.geta())
		}
		ta := NewFP4copy(F.a.geta())
		tb := NewFP4copy(y.a.geta())
		ta.add(F.a.getb())
		ta.norm()
		tb.add(y.a.getb())
		tb.norm()
		tc := NewFP4copy(ta)
		tc.mul(tb)
		t := NewFP4copy(w1)
		t.add(w2)
		t.neg()
		tc.add(t)

		ta.copy(F.a.geta())
		ta.add(F.b.geta())
		ta.norm()
		tb.copy(y.a.geta())
		tb.add(y.b.geta())
		tb.norm()
		td := NewFP4copy(ta)
		td.mul(tb)
		t.copy(w1)
		t.add(w3)
		t.neg()
		td.add(t)

		ta.copy(F.a.getb())
		ta.add(F.b.geta())
		ta.norm()
		tb.copy(y.a.getb())
		tb.add(y.b.geta())
		tb.norm()
		te := NewFP4copy(ta)
		te.mul(tb)
		t.copy(w2)
		t.add(w3)
		t.neg()
		te.add(t)

		w2.times_i()
		w1.add(w2)

		F.a.geta().copy(w1)
		F.a.getb().copy(tc)
		F.b.geta().copy(td)
		F.b.getb().copy(te)
		F.c.geta().copy(w3)
		F.c.getb().zero()

		F.a.norm()
		F.b.norm()
	} else {
		w1 := NewFP4copy(F.a.geta())
		w2 := NewFP4copy(F.a.getb())
		var w3 *FP4

		w1.mul(y.a.geta())
		w2.mul(y.a.getb())
		if y.stype == FP_SPARSEST || F.stype == FP_SPARSEST {
			if y.stype == FP_SPARSEST && F.stype == FP_SPARSEST {
				t := NewFPcopy(F.c.b.a.a)
				t.mul(y.c.b.a.a)
				w3 = NewFP4fp(t)
			} else {
				if y.stype != FP_SPARSEST {
					w3 = NewFP4copy(y.c.getb())
					w3.qmul(F.c.b.a.a)
				} else {
					w3 = NewFP4copy(F.c.getb())
					w3.qmul(y.c.b.a.a)
				}
			}
		} else {
			w3 = NewFP4copy(F.c.getb())
			w3.mul(y.c.getb())
		}
		ta := NewFP4copy(F.a.geta())
		tb := NewFP4copy(y.a.geta())
		ta.add(F.a.getb())
		ta.norm()
		tb.add(y.a.getb())
		tb.norm()
		tc := NewFP4copy(ta)
		tc.mul(tb)
		t := NewFP4copy(w1)
		t.add(w2)
		t.neg()
		tc.add(t)

		ta.copy(F.a.geta())
		ta.add(F.c.getb())
		ta.norm()
		tb.copy(y.a.geta())
		tb.add(y.c.getb())
		tb.norm()
		td := NewFP4copy(ta)
		td.mul(tb)
		t.copy(w1)
		t.add(w3)
		t.neg()
		td.add(t)

		ta.copy(F.a.getb())
		ta.add(F.c.getb())
		ta.norm()
		tb.copy(y.a.getb())
		tb.add(y.c.getb())
		tb.norm()
		te := NewFP4copy(ta)
		te.mul(tb)
		t.copy(w2)
		t.add(w3)
		t.neg()
		te.add(t)

		w2.times_i()
		w1.add(w2)
		F.a.geta().copy(w1)
		F.a.getb().copy(tc)

		w3.times_i()
		w3.norm()
		F.b.geta().zero()
		F.b.getb().copy(w3)

		te.norm()
		te.times_i()
		F.c.geta().copy(te)
		F.c.getb().copy(td)

		F.a.norm()
		F.c.norm()

	}
	F.stype = FP_SPARSE
}

/* this=1/this */
func (F *FP24) Inverse() {
	f0 := NewFP8copy(F.a)
	f1 := NewFP8copy(F.b)
	f2 := NewFP8copy(F.a)
	f3 := NewFP8()

	//F.norm()
	f0.sqr()
	f1.mul(F.c)
	f1.times_i()
	f0.sub(f1)
	f0.norm()

	f1.copy(F.c)
	f1.sqr()
	f1.times_i()
	f2.mul(F.b)
	f1.sub(f2)
	f1.norm()

	f2.copy(F.b)
	f2.sqr()
	f3.copy(F.a)
	f3.mul(F.c)
	f2.sub(f3)
	f2.norm()

	f3.copy(F.b)
	f3.mul(f2)
	f3.times_i()
	F.a.mul(f0)
	f3.add(F.a)
	F.c.mul(f1)
	F.c.times_i()

	f3.add(F.c)
	f3.norm()
	f3.inverse(nil)
	F.a.copy(f0)
	F.a.mul(f3)
	F.b.copy(f1)
	F.b.mul(f3)
	F.c.copy(f2)
	F.c.mul(f3)
	F.stype = FP_DENSE
}

/* this=this^p using Frobenius */
func (F *FP24) frob(f *FP2, n int) {
	f2 := NewFP2copy(f)
	f3 := NewFP2copy(f)

	f2.sqr()
	f3.mul(f2)

	f3.mul_ip()
	f3.norm()

	for i := 0; i < n; i++ {
		F.a.frob(f3)
		F.b.frob(f3)
		F.c.frob(f3)

		F.b.qmul(f)
		F.b.times_i2()
		F.c.qmul(f2)
		F.c.times_i2()
		F.c.times_i2()
	}
	F.stype = FP_DENSE
}

/* trace function */
func (F *FP24) trace() *FP8 {
	t := NewFP8()
	t.copy(F.a)
	t.imul(3)
	t.reduce()
	return t
}

/* convert from byte array to FP24 */
func FP24_fromBytes(w []byte) *FP24 {
	var t [8*int(MODBYTES)]byte
	MB := 8*int(MODBYTES)

	for i:=0;i<MB;i++ {
		t[i]=w[i]
	}
    c:=FP8_fromBytes(t[:])
	for i:=0;i<MB;i++ {
		t[i]=w[i+MB]
	}
    b:=FP8_fromBytes(t[:])
	for i:=0;i<MB;i++ {
		t[i]=w[i+2*MB]
	}
    a:=FP8_fromBytes(t[:])
	return NewFP24fp8s(a,b,c)
}

/* convert this to byte array */
func (F *FP24) ToBytes(w []byte) {
	var t [8*int(MODBYTES)]byte
	MB := 8*int(MODBYTES)
    F.c.ToBytes(t[:])
	for i:=0;i<MB;i++ { 
		w[i]=t[i]
	}
    F.b.ToBytes(t[:])
	for i:=0;i<MB;i++ {
		w[i+MB]=t[i]
	}
    F.a.ToBytes(t[:])
	for i:=0;i<MB;i++ {
		w[i+2*MB]=t[i]
	}
}

/* convert to hex string */
func (F *FP24) ToString() string {
	return ("[" + F.a.toString() + "," + F.b.toString() + "," + F.c.toString() + "]")
}

/* this=this^e */
func (F *FP24) Pow(e *BIG) *FP24 {
	sf := NewFP24copy(F)
	sf.norm()

	e1 := NewBIGcopy(e)
	e1.norm()
	e3 := NewBIGcopy(e1)
	e3.pmul(3)
	e3.norm()
	w := NewFP24copy(sf)
    if e3.iszilch() {
        w.one()
        return w
    }
	nb := e3.nbits()
	for i := nb - 2; i >= 1; i-- {
		w.usqr()
		bt := e3.bit(i) - e1.bit(i)
		if bt == 1 {
			w.Mul(sf)
		}
		if bt == -1 {
			sf.conj()
			w.Mul(sf)
			sf.conj()
		}
	}
	w.reduce()
	return w
}

/* constant time powering by small integer of max length bts */
func (F *FP24) pinpow(e int, bts int) {
	var R []*FP24
	R = append(R, NewFP24int(1))
	R = append(R, NewFP24copy(F))

	for i := bts - 1; i >= 0; i-- {
		b := (e >> uint(i)) & 1
		R[1-b].Mul(R[b])
		R[b].usqr()
	}
	F.Copy(R[0])
}

/* Fast compressed FP8 power of unitary FP24 */
/*
func (F *FP24) Compow(e *BIG, r *BIG) *FP8 {
	q := NewBIGints(Modulus)
	f := NewFP2bigs(NewBIGints(Fra), NewBIGints(Frb))

	m := NewBIGcopy(q)
	m.Mod(r)

	a := NewBIGcopy(e)
	a.Mod(m)

	b := NewBIGcopy(e)
	b.div(m)

	g1 := NewFP24copy(F)
	c := g1.trace()

	if b.iszilch() {
		c = c.xtr_pow(e)
		return c
	}

	g2 := NewFP24copy(F)
	g2.frob(f, 1)
	cp := g2.trace()

	g1.conj()
	g2.Mul(g1)
	cpm1 := g2.trace()
	g2.Mul(g1)
	cpm2 := g2.trace()

	c = c.xtr_pow2(cp, cpm1, cpm2, a, b)
	return c
}
*/
/* p=q0^u0.q1^u1.q2^u2.q3^u3.. */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
// Side channel attack secure

func pow8(q []*FP24, u []*BIG) *FP24 {
	var g1 []*FP24
	var g2 []*FP24
	var w1 [NLEN*int(BASEBITS) + 1]int8
	var s1 [NLEN*int(BASEBITS) + 1]int8
	var w2 [NLEN*int(BASEBITS) + 1]int8
	var s2 [NLEN*int(BASEBITS) + 1]int8
	var t []*BIG
	r := NewFP24()
	p := NewFP24()
	mt := NewBIGint(0)
	var bt int8
	var k int

	for i := 0; i < 8; i++ {
		t = append(t, NewBIGcopy(u[i]))
	}

	g1 = append(g1, NewFP24copy(q[0])) // q[0]
	g1 = append(g1, NewFP24copy(g1[0]))
	g1[1].Mul(q[1]) // q[0].q[1]
	g1 = append(g1, NewFP24copy(g1[0]))
	g1[2].Mul(q[2]) // q[0].q[2]
	g1 = append(g1, NewFP24copy(g1[1]))
	g1[3].Mul(q[2]) // q[0].q[1].q[2]
	g1 = append(g1, NewFP24copy(g1[0]))
	g1[4].Mul(q[3]) // q[0].q[3]
	g1 = append(g1, NewFP24copy(g1[1]))
	g1[5].Mul(q[3]) // q[0].q[1].q[3]
	g1 = append(g1, NewFP24copy(g1[2]))
	g1[6].Mul(q[3]) // q[0].q[2].q[3]
	g1 = append(g1, NewFP24copy(g1[3]))
	g1[7].Mul(q[3]) // q[0].q[1].q[2].q[3]

	g2 = append(g2, NewFP24copy(q[4])) // q[0]
	g2 = append(g2, NewFP24copy(g2[0]))
	g2[1].Mul(q[5]) // q[0].q[1]
	g2 = append(g2, NewFP24copy(g2[0]))
	g2[2].Mul(q[6]) // q[0].q[2]
	g2 = append(g2, NewFP24copy(g2[1]))
	g2[3].Mul(q[6]) // q[0].q[1].q[2]
	g2 = append(g2, NewFP24copy(g2[0]))
	g2[4].Mul(q[7]) // q[0].q[3]
	g2 = append(g2, NewFP24copy(g2[1]))
	g2[5].Mul(q[7]) // q[0].q[1].q[3]
	g2 = append(g2, NewFP24copy(g2[2]))
	g2[6].Mul(q[7]) // q[0].q[2].q[3]
	g2 = append(g2, NewFP24copy(g2[3]))
	g2[7].Mul(q[7]) // q[0].q[1].q[2].q[3]

	// Make them odd
	pb1 := 1 - t[0].parity()
	t[0].inc(pb1)
	//	t[0].norm();

	pb2 := 1 - t[4].parity()
	t[4].inc(pb2)
	//	t[4].norm();

	// Number of bits
	mt.zero()
	for i := 0; i < 8; i++ {
		t[i].norm()
		mt.or(t[i])
	}

	nb := 1 + mt.nbits()

	// Sign pivot
	s1[nb-1] = 1
	s2[nb-1] = 1
	for i := 0; i < nb-1; i++ {
		t[0].fshr(1)
		s1[i] = 2*int8(t[0].parity()) - 1
		t[4].fshr(1)
		s2[i] = 2*int8(t[4].parity()) - 1

	}

	// Recoded exponents
	for i := 0; i < nb; i++ {
		w1[i] = 0
		k = 1
		for j := 1; j < 4; j++ {
			bt = s1[i] * int8(t[j].parity())
			t[j].fshr(1)
			t[j].dec(int(bt) >> 1)
			t[j].norm()
			w1[i] += bt * int8(k)
			k *= 2
		}
		w2[i] = 0
		k = 1
		for j := 5; j < 8; j++ {
			bt = s2[i] * int8(t[j].parity())
			t[j].fshr(1)
			t[j].dec(int(bt) >> 1)
			t[j].norm()
			w2[i] += bt * int8(k)
			k *= 2
		}
	}
	// Main loop
	p.selector(g1, int32(2*w1[nb-1]+1))
	r.selector(g2, int32(2*w2[nb-1]+1))
	p.Mul(r)
	for i := nb - 2; i >= 0; i-- {
		p.usqr()
		r.selector(g1, int32(2*w1[i]+s1[i]))
		p.Mul(r)
		r.selector(g2, int32(2*w2[i]+s2[i]))
		p.Mul(r)
	}

	// apply correction
	r.Copy(q[0])
	r.conj()
	r.Mul(p)
	p.cmove(r, pb1)
	r.Copy(q[4])
	r.conj()
	r.Mul(p)
	p.cmove(r, pb2)

	p.reduce()
	return p
}
