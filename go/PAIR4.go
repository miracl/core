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

/* BLS Curve Pairing functions */

package XXX

//import "fmt"

// Point doubling for pairings
func dbl(A *ECP4, AA *FP4, BB *FP4, CC *FP4) {
	CC.copy(A.getx())          //X
	YY := NewFP4copy(A.gety()) //Y
	BB.copy(A.getz())          //Z
	AA.copy(YY)                //Y
	AA.mul(BB)                 //YZ
	CC.sqr()                   //X^2
	YY.sqr()                   //Y^2
	BB.sqr()                   //Z^2

	AA.add(AA)
	AA.neg()
	AA.norm() //-2AA
	AA.times_i()

	sb := 3 * CURVE_B_I
	BB.imul(sb)
	CC.imul(3)
	if SEXTIC_TWIST == D_TYPE {
		YY.times_i()
		CC.times_i()
	}
	if SEXTIC_TWIST == M_TYPE {
		BB.times_i()
	}
	BB.sub(YY)
	BB.norm()

	A.dbl()

	/*
		AA.imul(4)
		AA.neg()
		AA.norm()   //-4YZ

		CC.imul(6)  //6X^2

		sb := 3 * CURVE_B_I
		BB.imul(sb) // 3bZ^2
		if SEXTIC_TWIST == D_TYPE {
			BB.div_2i()
		}
		if SEXTIC_TWIST == M_TYPE {
			BB.times_i()
			BB.add(BB)
			AA.times_i()
			AA.norm()
		}
		BB.norm() // 3b.Z^2

		YY.add(YY)
		BB.sub(YY)
		BB.norm() // 3b.Z^2-2Y^2

		A.dbl() */
}

// Point addition for pairings
func add(A *ECP4, B *ECP4, AA *FP4, BB *FP4, CC *FP4) {
	AA.copy(A.getx())          // X1
	CC.copy(A.gety())          // Y1
	T1 := NewFP4copy(A.getz()) // Z1
	BB.copy(A.getz())          // Z1

	T1.mul(B.gety()) // T1=Z1.Y2
	BB.mul(B.getx()) // T2=Z1.X2

	AA.sub(BB)
	AA.norm() // X1=X1-Z1.X2
	CC.sub(T1)
	CC.norm() // Y1=Y1-Z1.Y2

	T1.copy(AA) // T1=X1-Z1.X2

	if SEXTIC_TWIST == M_TYPE {
		AA.times_i()
		AA.norm()
	}

	T1.mul(B.gety()) // T1=(X1-Z1.X2).Y2

	BB.copy(CC)      // T2=Y1-Z1.Y2
	BB.mul(B.getx()) // T2=(Y1-Z1.Y2).X2
	BB.sub(T1)
	BB.norm() // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2
	CC.neg()
	CC.norm() // Y1=-(Y1-Z1.Y2).Xs

	A.Add(B)
}

func line(A *ECP4, B *ECP4, Qx *FP, Qy *FP) *FP24 {
	AA := NewFP4()
	BB := NewFP4()
	CC := NewFP4()

	var a *FP8
	var b *FP8
	var c *FP8

	if A == B {
		dbl(A, AA, BB, CC)
	} else {
		add(A, B, AA, BB, CC)
	}
	CC.qmul(Qx)
	AA.qmul(Qy)

	a = NewFP8fp4s(AA, BB)

	if SEXTIC_TWIST == D_TYPE {
		b = NewFP8fp4(CC) // L(0,1) | L(0,0) | L(1,0)
		c = NewFP8()
	}
	if SEXTIC_TWIST == M_TYPE {
		b = NewFP8()
		c = NewFP8fp4(CC)
		c.times_i()
	}

	r := NewFP24fp8s(a, b, c)
	r.stype = FP_SPARSER
	return r
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
func lbits(n3 *BIG, n *BIG) int {
	n.copy(NewBIGints(CURVE_Bnx))
	n3.copy(n)
	n3.pmul(3)
	n3.norm()
	return n3.nbits()
}

/* prepare for multi-pairing */
func Initmp() []*FP24 {
	var r []*FP24
	for i := ATE_BITS - 1; i >= 0; i-- {
		r = append(r, NewFP24int(1))
	}
	return r
}

/* basic Miller loop */
func Miller(r []*FP24) *FP24 {
	res := NewFP24int(1)
	for i := ATE_BITS - 1; i >= 1; i-- {
		res.sqr()
		res.ssmul(r[i])
		r[i].zero()
	}

	if SIGN_OF_X == NEGATIVEX {
		res.conj()
	}
	res.ssmul(r[0])
	r[0].zero()
	return res
}

// Store precomputed line details in an FP8
func pack(AA *FP4, BB *FP4, CC *FP4) *FP8 {
	i := NewFP4copy(CC)
	i.inverse(nil)
	a := NewFP4copy(AA)
	a.mul(i)
	b := NewFP4copy(BB)
	b.mul(i)
	return NewFP8fp4s(a, b)
}

// Unpack G2 line function details and include G1
func unpack(T *FP8, Qx *FP, Qy *FP) *FP24 {
	var a *FP8
	var b *FP8
	var c *FP8

	a = NewFP8copy(T)
	a.geta().qmul(Qy)
	t := NewFP4fp(Qx)
	if SEXTIC_TWIST == D_TYPE {
		b = NewFP8fp4(t)
		c = NewFP8()
	}
	if SEXTIC_TWIST == M_TYPE {
		b = NewFP8()
		c = NewFP8fp4(t)
		c.times_i()
	}
	v := NewFP24fp8s(a, b, c)
	v.stype = FP_SPARSEST
	return v
}

func precomp(GV *ECP4) []*FP8 {
	n := NewBIG()
	n3 := NewBIG()
	AA := NewFP4()
	BB := NewFP4()
	CC := NewFP4()
	var bt int
	P := NewECP4()
	P.Copy(GV)

	A := NewECP4()
	A.Copy(P)
	MP := NewECP4()
	MP.Copy(P)
	MP.neg()

	nb := lbits(n3, n)
	var T []*FP8

	for i := nb - 2; i >= 1; i-- {
		dbl(A, AA, BB, CC)
		T = append(T, pack(AA, BB, CC))
		bt = n3.bit(i) - n.bit(i)
		if bt == 1 {
			add(A, P, AA, BB, CC)
			T = append(T, pack(AA, BB, CC))
		}
		if bt == -1 {
			add(A, MP, AA, BB, CC)
			T = append(T, pack(AA, BB, CC))
		}
	}
	return T
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
func Another_pc(r []*FP24, T []*FP8, QV *ECP) {
	n := NewBIG()
	n3 := NewBIG()
	var lv, lv2 *FP24
	var bt, j int

	if QV.Is_infinity() {
		return
	}

	Q := NewECP()
	Q.Copy(QV)
	Q.Affine()
	Qx := NewFPcopy(Q.getx())
	Qy := NewFPcopy(Q.gety())

	nb := lbits(n3, n)
	j = 0
	for i := nb - 2; i >= 1; i-- {
		lv = unpack(T[j], Qx, Qy)
		j += 1
		bt = n3.bit(i) - n.bit(i)
		if bt == 1 {
			lv2 = unpack(T[j], Qx, Qy)
			j += 1
			lv.smul(lv2)
		}
		if bt == -1 {
			lv2 = unpack(T[j], Qx, Qy)
			j += 1
			lv.smul(lv2)
		}
		r[i].ssmul(lv)
	}
}

/* Accumulate another set of line functions for n-pairing */
func Another(r []*FP24, P1 *ECP4, Q1 *ECP) {
	n := NewBIG()
	n3 := NewBIG()
	var lv, lv2 *FP24

	if Q1.Is_infinity() {
		return
	}

	// P is needed in affine form for line function, Q for (Qx,Qy) extraction
	P := NewECP4()
	P.Copy(P1)
	Q := NewECP()
	Q.Copy(Q1)

	P.Affine()
	Q.Affine()

	Qx := NewFPcopy(Q.getx())
	Qy := NewFPcopy(Q.gety())

	A := NewECP4()
	A.Copy(P)

	MP := NewECP4()
	MP.Copy(P)
	MP.neg()

	nb := lbits(n3, n)

	for i := nb - 2; i >= 1; i-- {
		lv = line(A, A, Qx, Qy)

		bt := n3.bit(i) - n.bit(i)
		if bt == 1 {
			lv2 = line(A, P, Qx, Qy)
			lv.smul(lv2)
		}
		if bt == -1 {
			lv2 = line(A, MP, Qx, Qy)
			lv.smul(lv2)
		}
		r[i].ssmul(lv)
	}
}

/* Optimal R-ate pairing */
func Ate(P1 *ECP4, Q1 *ECP) *FP24 {
	n := NewBIG()
	n3 := NewBIG()
	var lv, lv2 *FP24

	if Q1.Is_infinity() {
		return NewFP24int(1)
	}

	P := NewECP4()
	P.Copy(P1)
	P.Affine()
	Q := NewECP()
	Q.Copy(Q1)
	Q.Affine()

	Qx := NewFPcopy(Q.getx())
	Qy := NewFPcopy(Q.gety())

	A := NewECP4()
	r := NewFP24int(1)

	A.Copy(P)
	NP := NewECP4()
	NP.Copy(P)
	NP.neg()

	nb := lbits(n3, n)

	for i := nb - 2; i >= 1; i-- {
		r.sqr()
		lv = line(A, A, Qx, Qy)

		bt := n3.bit(i) - n.bit(i)
		if bt == 1 {
			lv2 = line(A, P, Qx, Qy)
			lv.smul(lv2)
		}
		if bt == -1 {
			lv2 = line(A, NP, Qx, Qy)
			lv.smul(lv2)
		}
		r.ssmul(lv)
	}

	if SIGN_OF_X == NEGATIVEX {
		r.conj()
	}

	return r
}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
func Ate2(P1 *ECP4, Q1 *ECP, R1 *ECP4, S1 *ECP) *FP24 {
	n := NewBIG()
	n3 := NewBIG()
	var lv, lv2 *FP24

	if Q1.Is_infinity() {
		return Ate(R1, S1)
	}
	if S1.Is_infinity() {
		return Ate(P1, Q1)
	}

	P := NewECP4()
	P.Copy(P1)
	P.Affine()
	Q := NewECP()
	Q.Copy(Q1)
	Q.Affine()
	R := NewECP4()
	R.Copy(R1)
	R.Affine()
	S := NewECP()
	S.Copy(S1)
	S.Affine()

	Qx := NewFPcopy(Q.getx())
	Qy := NewFPcopy(Q.gety())
	Sx := NewFPcopy(S.getx())
	Sy := NewFPcopy(S.gety())

	A := NewECP4()
	B := NewECP4()
	r := NewFP24int(1)

	A.Copy(P)
	B.Copy(R)
	NP := NewECP4()
	NP.Copy(P)
	NP.neg()
	NR := NewECP4()
	NR.Copy(R)
	NR.neg()

	nb := lbits(n3, n)

	for i := nb - 2; i >= 1; i-- {
		r.sqr()
		lv = line(A, A, Qx, Qy)
		lv2 = line(B, B, Sx, Sy)
		lv.smul(lv2)
		r.ssmul(lv)
		bt := n3.bit(i) - n.bit(i)
		if bt == 1 {
			lv = line(A, P, Qx, Qy)
			lv2 = line(B, R, Sx, Sy)
			lv.smul(lv2)
			r.ssmul(lv)
		}
		if bt == -1 {
			lv = line(A, NP, Qx, Qy)
			lv2 = line(B, NR, Sx, Sy)
			lv.smul(lv2)
			r.ssmul(lv)
		}
	}

	if SIGN_OF_X == NEGATIVEX {
		r.conj()
	}

	return r
}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
func Fexp(m *FP24) *FP24 {
	f := NewFP2bigs(NewBIGints(Fra), NewBIGints(Frb))
	x := NewBIGints(CURVE_Bnx)
	r := NewFP24copy(m)

	/* Easy part of final exp */
	lv := NewFP24copy(r)

	lv.Inverse()
	r.conj()

	r.Mul(lv)
	lv.Copy(r)
	r.frob(f, 4)
	r.Mul(lv)

	/* Hard part of final exp */
// See https://eprint.iacr.org/2020/875.pdf
		y1:=NewFP24copy(r)
		y1.usqr()
		y1.Mul(r) // y1=r^3

		y0:=NewFP24copy(r.Pow(x))
		if SIGN_OF_X == NEGATIVEX {
			y0.conj()
		}
		t0:=NewFP24copy(r); t0.conj()
		r.Copy(y0)
		r.Mul(t0)

		y0.Copy(r.Pow(x))
		if SIGN_OF_X == NEGATIVEX {
			y0.conj()
		}
		t0.Copy(r); t0.conj()
		r.Copy(y0)
		r.Mul(t0)

// ^(x+p)
		y0.Copy(r.Pow(x));
		if SIGN_OF_X == NEGATIVEX {
			y0.conj()
		}
		t0.Copy(r)
		t0.frob(f,1)
		r.Copy(y0)
		r.Mul(t0);

// ^(x^2+p^2)
		y0.Copy(r.Pow(x))
		y0.Copy(y0.Pow(x))
		t0.Copy(r)
		t0.frob(f,2)
		r.Copy(y0)
		r.Mul(t0)

// ^(x^4+p^4-1)
		y0.Copy(r.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		t0.Copy(r)
		t0.frob(f,4); 
		y0.Mul(t0)
		t0.Copy(r); t0.conj()
		r.Copy(y0)
		r.Mul(t0)

		r.Mul(y1)
		r.reduce();

/*
	// Ghamman & Fouotsa Method

	t7 := NewFP24copy(r)
	t7.usqr()
	t1 := t7.Pow(x)

	x.fshr(1)
	t2 := t1.Pow(x)
	x.fshl(1)

	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}
	t3 := NewFP24copy(t1)
	t3.conj()
	t2.Mul(t3)
	t2.Mul(r)

	t3 = t2.Pow(x)
	t4 := t3.Pow(x)
	t5 := t4.Pow(x)

	if SIGN_OF_X == NEGATIVEX {
		t3.conj()
		t5.conj()
	}

	t3.frob(f, 6)
	t4.frob(f, 5)
	t3.Mul(t4)

	t6 := t5.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t6.conj()
	}

	t5.frob(f, 4)
	t3.Mul(t5)

	t0 := NewFP24copy(t2)
	t0.conj()
	t6.Mul(t0)

	t5.Copy(t6)
	t5.frob(f, 3)

	t3.Mul(t5)
	t5 = t6.Pow(x)
	t6 = t5.Pow(x)

	if SIGN_OF_X == NEGATIVEX {
		t5.conj()
	}

	t0.Copy(t5)
	t0.frob(f, 2)
	t3.Mul(t0)
	t0.Copy(t6)
	t0.frob(f, 1)

	t3.Mul(t0)
	t5 = t6.Pow(x)

	if SIGN_OF_X == NEGATIVEX {
		t5.conj()
	}
	t2.frob(f, 7)

	t5.Mul(t7)
	t3.Mul(t2)
	t3.Mul(t5)

	r.Mul(t3)
	r.reduce()
*/
	return r
}

/* GLV method */
func glv(e *BIG) []*BIG {
	var u []*BIG

	q := NewBIGints(CURVE_Order)
	x := NewBIGints(CURVE_Bnx)
	x2 := smul(x, x)
	x = smul(x2, x2)
	u = append(u, NewBIGcopy(e))
	u[0].Mod(x)
	u = append(u, NewBIGcopy(e))
	u[1].div(x)
	u[1].rsub(q)
	return u
}

/* Galbraith & Scott Method */
func gs(e *BIG) []*BIG {
	var u []*BIG

	q := NewBIGints(CURVE_Order)
	x := NewBIGints(CURVE_Bnx)
	w := NewBIGcopy(e)
	for i := 0; i < 7; i++ {
		u = append(u, NewBIGcopy(w))
		u[i].Mod(x)
		w.div(x)
	}
	u = append(u, NewBIGcopy(w))
	if SIGN_OF_X == NEGATIVEX {
		u[1].copy(Modneg(u[1], q))
		u[3].copy(Modneg(u[3], q))
		u[5].copy(Modneg(u[5], q))
		u[7].copy(Modneg(u[7], q))

	}

	return u
}

/* Multiply P by e in group G1 */
func G1mul(P *ECP, e *BIG) *ECP {
	var R *ECP
	if USE_GLV {
		R = NewECP()
		R.Copy(P)
		Q := NewECP()
		Q.Copy(P)
		Q.Affine()
		q := NewBIGints(CURVE_Order)
		cru := NewFPbig(NewBIGints(CRu))
		t := NewBIGint(0)
		u := glv(e)
		Q.getx().mul(cru)

		np := u[0].nbits()
		t.copy(Modneg(u[0], q))
		nn := t.nbits()
		if nn < np {
			u[0].copy(t)
			R.Neg()
		}

		np = u[1].nbits()
		t.copy(Modneg(u[1], q))
		nn = t.nbits()
		if nn < np {
			u[1].copy(t)
			Q.Neg()
		}
		u[0].norm()
		u[1].norm()
		R = R.Mul2(u[0], Q, u[1])

	} else {
		R = P.mul(e)
	}
	return R
}

/* Multiply P by e in group G2 */
func G2mul(P *ECP4, e *BIG) *ECP4 {
	var R *ECP4
	if USE_GS_G2 {
		var Q []*ECP4

		F := ECP4_frob_constants()

		q := NewBIGints(CURVE_Order)
		u := gs(e)

		t := NewBIGint(0)

		Q = append(Q, NewECP4())
		Q[0].Copy(P)
		for i := 1; i < 8; i++ {
			Q = append(Q, NewECP4())
			Q[i].Copy(Q[i-1])
			Q[i].frob(F, 1)
		}
		for i := 0; i < 8; i++ {
			np := u[i].nbits()
			t.copy(Modneg(u[i], q))
			nn := t.nbits()
			if nn < np {
				u[i].copy(t)
				Q[i].neg()
			}
			u[i].norm()
		}

		R = mul8(Q, u)

	} else {
		R = P.mul(e)
	}
	return R
}

/* f=f^e */
/* Note that this method requires a lot of RAM!  */
func GTpow(d *FP24, e *BIG) *FP24 {
	var r *FP24
	if USE_GS_GT {
		var g []*FP24
		f := NewFP2bigs(NewBIGints(Fra), NewBIGints(Frb))
		q := NewBIGints(CURVE_Order)
		t := NewBIGint(0)

		u := gs(e)

		g = append(g, NewFP24copy(d))
		for i := 1; i < 8; i++ {
			g = append(g, NewFP24())
			g[i].Copy(g[i-1])
			g[i].frob(f, 1)
		}
		for i := 0; i < 8; i++ {
			np := u[i].nbits()
			t.copy(Modneg(u[i], q))
			nn := t.nbits()
			if nn < np {
				u[i].copy(t)
				g[i].conj()
			}
			u[i].norm()
		}
		r = pow8(g, u)
	} else {
		r = d.Pow(e)
	}
	return r
}

/* test G1 group membership */
	func G1member(P *ECP) bool {
		q := NewBIGints(CURVE_Order)
		if P.Is_infinity() {return false}
		W:=G1mul(P,q)
		if !W.Is_infinity() {return false}
		return true
	}

/* test G2 group membership */
	func G2member(P *ECP4) bool {
		q := NewBIGints(CURVE_Order)
		if P.Is_infinity() {return false}
		W:=G2mul(P,q)
		if !W.Is_infinity() {return false}
		return true
	}

/* test group membership - no longer needed*/
/* Check that m!=1, conj(m)*m==1, and m.m^{p^8}=m^{p^4} */

func GTmember(m *FP24) bool {
	if m.Isunity() {return false}
	r:=NewFP24copy(m)
	r.conj()
	r.Mul(m)
	if !r.Isunity() {return false}

	f:=NewFP2bigs(NewBIGints(Fra),NewBIGints(Frb))

	r.Copy(m); r.frob(f,4)
	w:=NewFP24copy(r); w.frob(f,4)
	w.Mul(m)
	if !w.Equals(r) {return false}

	q := NewBIGints(CURVE_Order)
	w.Copy(m)
	r.Copy(GTpow(w,q))
	if !r.Isunity() {return false}
	return true
}
