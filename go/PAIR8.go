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
func dbl(A *ECP8, AA *FP8, BB *FP8, CC *FP8) {
	CC.copy(A.getx())          //X
	YY := NewFP8copy(A.gety()) //Y
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
}

// Point addition for pairings
func add(A *ECP8, B *ECP8, AA *FP8, BB *FP8, CC *FP8) {
	AA.copy(A.getx())          // X1
	CC.copy(A.gety())          // Y1
	T1 := NewFP8copy(A.getz()) // Z1
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

func line(A *ECP8, B *ECP8, Qx *FP, Qy *FP) *FP48 {
	AA := NewFP8()
	BB := NewFP8()
	CC := NewFP8()

	var a *FP16
	var b *FP16
	var c *FP16

	if A == B {
		dbl(A, AA, BB, CC)
	} else {
		add(A, B, AA, BB, CC)
	}
	CC.tmul(Qx)
	AA.tmul(Qy)

	a = NewFP16fp8s(AA, BB)

	if SEXTIC_TWIST == D_TYPE {
		b = NewFP16fp8(CC) // L(0,1) | L(0,0) | L(1,0)
		c = NewFP16()
	}
	if SEXTIC_TWIST == M_TYPE {
		b = NewFP16()
		c = NewFP16fp8(CC)
		c.times_i()
	}

	r := NewFP48fp16s(a, b, c)
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
func Initmp() []*FP48 {
	var r []*FP48
	for i := ATE_BITS - 1; i >= 0; i-- {
		r = append(r, NewFP48int(1))
	}
	return r
}

/* basic Miller loop */
func Miller(r []*FP48) *FP48 {
	res := NewFP48int(1)
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
func pack(AA *FP8, BB *FP8, CC *FP8) *FP16 {
	i := NewFP8copy(CC)
	i.inverse(nil)
	a := NewFP8copy(AA)
	a.mul(i)
	b := NewFP8copy(BB)
	b.mul(i)
	return NewFP16fp8s(a, b)
}

// Unpack G2 line function details and include G1
func unpack(T *FP16, Qx *FP, Qy *FP) *FP48 {
	var a *FP16
	var b *FP16
	var c *FP16

	a = NewFP16copy(T)
	a.geta().tmul(Qy)
	t := NewFP8fp(Qx)
	if SEXTIC_TWIST == D_TYPE {
		b = NewFP16fp8(t)
		c = NewFP16()
	}
	if SEXTIC_TWIST == M_TYPE {
		b = NewFP16()
		c = NewFP16fp8(t)
		c.times_i()
	}
	v := NewFP48fp16s(a, b, c)
	v.stype = FP_SPARSEST
	return v
}

func precomp(GV *ECP8) []*FP16 {
	n := NewBIG()
	n3 := NewBIG()
	AA := NewFP8()
	BB := NewFP8()
	CC := NewFP8()
	var bt int
	P := NewECP8()
	P.Copy(GV)

	A := NewECP8()
	A.Copy(P)
	MP := NewECP8()
	MP.Copy(P)
	MP.neg()

	nb := lbits(n3, n)
	var T []*FP16

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

func Another_pc(r []*FP48, T []*FP16, QV *ECP) {
	n := NewBIG()
	n3 := NewBIG()
	var lv, lv2 *FP48
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
func Another(r []*FP48, P1 *ECP8, Q1 *ECP) {
	n := NewBIG()
	n3 := NewBIG()
	var lv, lv2 *FP48

	if Q1.Is_infinity() {
		return
	}
	// P is needed in affine form for line function, Q for (Qx,Qy) extraction
	P := NewECP8()
	P.Copy(P1)
	Q := NewECP()
	Q.Copy(Q1)

	P.Affine()
	Q.Affine()

	Qx := NewFPcopy(Q.getx())
	Qy := NewFPcopy(Q.gety())

	A := NewECP8()
	A.Copy(P)

	MP := NewECP8()
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
func Ate(P1 *ECP8, Q1 *ECP) *FP48 {
	n := NewBIG()
	n3 := NewBIG()
	var lv, lv2 *FP48

	if Q1.Is_infinity() {
		return NewFP48int(1)
	}

	P := NewECP8()
	P.Copy(P1)
	P.Affine()
	Q := NewECP()
	Q.Copy(Q1)
	Q.Affine()

	Qx := NewFPcopy(Q.getx())
	Qy := NewFPcopy(Q.gety())

	A := NewECP8()
	r := NewFP48int(1)

	A.Copy(P)
	NP := NewECP8()
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
func Ate2(P1 *ECP8, Q1 *ECP, R1 *ECP8, S1 *ECP) *FP48 {
	n := NewBIG()
	n3 := NewBIG()
	var lv, lv2 *FP48

	if Q1.Is_infinity() {
		return Ate(R1, S1)
	}
	if S1.Is_infinity() {
		return Ate(P1, Q1)
	}

	P := NewECP8()
	P.Copy(P1)
	P.Affine()
	Q := NewECP()
	Q.Copy(Q1)
	Q.Affine()
	R := NewECP8()
	R.Copy(R1)
	R.Affine()
	S := NewECP()
	S.Copy(S1)
	S.Affine()

	Qx := NewFPcopy(Q.getx())
	Qy := NewFPcopy(Q.gety())
	Sx := NewFPcopy(S.getx())
	Sy := NewFPcopy(S.gety())

	A := NewECP8()
	B := NewECP8()
	r := NewFP48int(1)

	A.Copy(P)
	B.Copy(R)
	NP := NewECP8()
	NP.Copy(P)
	NP.neg()
	NR := NewECP8()
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
func Fexp(m *FP48) *FP48 {
	f := NewFP2bigs(NewBIGints(Fra), NewBIGints(Frb))
	x := NewBIGints(CURVE_Bnx)
	r := NewFP48copy(m)
//	var t1, t2 *FP48

	/* Easy part of final exp */
	lv := NewFP48copy(r)

	lv.Inverse()
	r.conj()

	r.Mul(lv)
	lv.Copy(r)
	r.frob(f, 8)
	r.Mul(lv)

	/* Hard part of final exp */
// See https://eprint.iacr.org/2020/875.pdf
		y1:=NewFP48copy(r)
		y1.usqr()
		y1.Mul(r) // y1=r^3

		y0:=NewFP48copy(r.Pow(x))
		if SIGN_OF_X == NEGATIVEX {
			y0.conj()
		}
		t0:=NewFP48copy(r); t0.conj()
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

// ^(x^4+p^4)
		y0.Copy(r.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		t0.Copy(r)
		t0.frob(f,4); 
		r.Copy(y0)
		r.Mul(t0)

// ^(x^8+p^8-1)
		y0.Copy(r.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		y0.Copy(y0.Pow(x))
		t0.Copy(r)
		t0.frob(f,8); 
		y0.Mul(t0)
		t0.Copy(r); t0.conj()
		r.Copy(y0)
		r.Mul(t0)


		r.Mul(y1)
		r.reduce();

/*
	// Ghamman & Fouotsa Method

	t7 := NewFP48copy(r)
	t7.usqr()

	if x.parity() == 1 {
		t2 = r.Pow(x)
		t1 = NewFP48copy(t2)
		t1.usqr()
		t2 = t2.Pow(x)
	} else {
		t1 = t7.Pow(x)
		x.fshr(1)
		t2 = t1.Pow(x)
		x.fshl(1)
	}

	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3 := NewFP48copy(t1)
	t3.conj()
	t2.Mul(t3)
	t2.Mul(r)

	r.Mul(t7)

	t1 = t2.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}
	t3.Copy(t1)
	t3.frob(f, 14)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 13)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 12)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 11)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 10)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 9)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 8)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t2)
	t3.conj()
	t1.Mul(t3)
	t3.Copy(t1)
	t3.frob(f, 7)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 6)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 5)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 4)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 3)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 2)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	t3.Copy(t1)
	t3.frob(f, 1)
	r.Mul(t3)
	t1 = t1.Pow(x)
	if SIGN_OF_X == NEGATIVEX {
		t1.conj()
	}

	r.Mul(t1)
	t2.frob(f, 15)
	r.Mul(t2)

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
	x2 = smul(x, x)
	u = append(u, NewBIGcopy(e))
	u[0].Mod(x2)
	u = append(u, NewBIGcopy(e))
	u[1].div(x2)
	u[1].rsub(q)
	return u
}

/* Galbraith & Scott Method */
func gs(e *BIG) []*BIG {
	var u []*BIG

	q := NewBIGints(CURVE_Order)
	x := NewBIGints(CURVE_Bnx)
	w := NewBIGcopy(e)
	for i := 0; i < 15; i++ {
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
		u[9].copy(Modneg(u[9], q))
		u[11].copy(Modneg(u[11], q))
		u[13].copy(Modneg(u[13], q))
		u[15].copy(Modneg(u[15], q))
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
func G2mul(P *ECP8, e *BIG) *ECP8 {
	var R *ECP8
	if USE_GS_G2 {
		var Q []*ECP8

		F := ECP8_frob_constants()

		q := NewBIGints(CURVE_Order)
		u := gs(e)

		t := NewBIGint(0)

		Q = append(Q, NewECP8())
		Q[0].Copy(P)
		for i := 1; i < 16; i++ {
			Q = append(Q, NewECP8())
			Q[i].Copy(Q[i-1])
			Q[i].frob(F, 1)
		}
		for i := 0; i < 16; i++ {
			np := u[i].nbits()
			t.copy(Modneg(u[i], q))
			nn := t.nbits()
			if nn < np {
				u[i].copy(t)
				Q[i].neg()
			}
			u[i].norm()
		}

		R = mul16(Q, u)

	} else {
		R = P.mul(e)
	}
	return R
}

/* f=f^e */
/* Note that this method requires a lot of RAM!  */
func GTpow(d *FP48, e *BIG) *FP48 {
	var r *FP48
	if USE_GS_GT {
		var g []*FP48
		f := NewFP2bigs(NewBIGints(Fra), NewBIGints(Frb))
		q := NewBIGints(CURVE_Order)
		t := NewBIGint(0)

		u := gs(e)

		g = append(g, NewFP48copy(d))
		for i := 1; i < 16; i++ {
			g = append(g, NewFP48())
			g[i].Copy(g[i-1])
			g[i].frob(f, 1)
		}
		for i := 0; i < 16; i++ {
			np := u[i].nbits()
			t.copy(Modneg(u[i], q))
			nn := t.nbits()
			if nn < np {
				u[i].copy(t)
				g[i].conj()
			}
			u[i].norm()
		}
		r = pow16(g, u)
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
	func G2member(P *ECP8) bool {
		q := NewBIGints(CURVE_Order)
		if P.Is_infinity() {return false}
		W:=G2mul(P,q)
		if !W.Is_infinity() {return false}
		return true
	}

/* test group membership - no longer needed*/
/* Check that m!=1, conj(m)*m==1, and m.m^{p^16}=m^{p^8} */

func GTmember(m *FP48) bool {
	if m.Isunity() {return false}
	r:=NewFP48copy(m)
	r.conj()
	r.Mul(m)
	if !r.Isunity() {return false}

	f:=NewFP2bigs(NewBIGints(Fra),NewBIGints(Frb))

	r.Copy(m); r.frob(f,8)
	w:=NewFP48copy(r); w.frob(f,8)
	w.Mul(m)
	if !w.Equals(r) {return false}

	q := NewBIGints(CURVE_Order)
	w.Copy(m)
	r.Copy(GTpow(w,q))
	if !r.Isunity() {return false}
	return true
}

