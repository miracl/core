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

/* MiotCL Weierstrass elliptic curve functions over FP2 */

package XXX

//import "fmt"

type ECP8 struct {
	x *FP8
	y *FP8
	z *FP8
}

func NewECP8() *ECP8 {
	E := new(ECP8)
	E.x = NewFP8()
	E.y = NewFP8int(1)
	E.z = NewFP8()
	return E
}

/* Test this=O? */
func (E *ECP8) Is_infinity() bool {
	return E.x.iszilch() && E.z.iszilch()
}

/* copy this=P */
func (E *ECP8) Copy(P *ECP8) {
	E.x.copy(P.x)
	E.y.copy(P.y)
	E.z.copy(P.z)
}

/* set this=O */
func (E *ECP8) inf() {
	E.x.zero()
	E.y.one()
	E.z.zero()
}

/* set this=-this */
func (E *ECP8) neg() {
	E.y.norm()
	E.y.neg()
	E.y.norm()
}

/* Conditional move of Q to P dependant on d */
func (E *ECP8) cmove(Q *ECP8, d int) {
	E.x.cmove(Q.x, d)
	E.y.cmove(Q.y, d)
	E.z.cmove(Q.z, d)
}

/* Constant time select from pre-computed table */
func (E *ECP8) selector(W []*ECP8, b int32) {
	MP := NewECP8()
	m := b >> 31
	babs := (b ^ m) - m

	babs = (babs - 1) / 2

	E.cmove(W[0], teq(babs, 0)) // conditional move
	E.cmove(W[1], teq(babs, 1))
	E.cmove(W[2], teq(babs, 2))
	E.cmove(W[3], teq(babs, 3))
	E.cmove(W[4], teq(babs, 4))
	E.cmove(W[5], teq(babs, 5))
	E.cmove(W[6], teq(babs, 6))
	E.cmove(W[7], teq(babs, 7))

	MP.Copy(E)
	MP.neg()
	E.cmove(MP, int(m&1))
}

/* Test if P == Q */
func (E *ECP8) Equals(Q *ECP8) bool {

	a := NewFP8copy(E.x)
	b := NewFP8copy(Q.x)
	a.mul(Q.z)
	b.mul(E.z)

	if !a.Equals(b) {
		return false
	}
	a.copy(E.y)
	b.copy(Q.y)
	a.mul(Q.z)
	b.mul(E.z)
	if !a.Equals(b) {
		return false
	}

	return true
}

/* set to Affine - (x,y,z) to (x,y) */
func (E *ECP8) Affine() {
	if E.Is_infinity() {
		return
	}
	one := NewFP8int(1)
	if E.z.Equals(one) {
		E.x.reduce()
		E.y.reduce()
		return
	}
	E.z.inverse()

	E.x.mul(E.z)
	E.x.reduce()
	E.y.mul(E.z)
	E.y.reduce()
	E.z.copy(one)
}

/* extract affine x as FP2 */
func (E *ECP8) GetX() *FP8 {
	W := NewECP8()
	W.Copy(E)
	W.Affine()
	return W.x
}

/* extract affine y as FP2 */
func (E *ECP8) GetY() *FP8 {
	W := NewECP8()
	W.Copy(E)
	W.Affine()
	return W.y
}

/* extract projective x */
func (E *ECP8) getx() *FP8 {
	return E.x
}

/* extract projective y */
func (E *ECP8) gety() *FP8 {
	return E.y
}

/* extract projective z */
func (E *ECP8) getz() *FP8 {
	return E.z
}

/* convert to byte array */
func (E *ECP8) ToBytes(b []byte) {
	var t [int(MODBYTES)]byte
	MB := int(MODBYTES)

	W := NewECP8()
	W.Copy(E)
	W.Affine()

	W.x.geta().geta().GetA().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i] = t[i]
	}
	W.x.geta().geta().GetB().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+MB] = t[i]
	}
	W.x.geta().getb().GetA().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+2*MB] = t[i]
	}
	W.x.geta().getb().GetB().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+3*MB] = t[i]
	}

	W.x.getb().geta().GetA().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+4*MB] = t[i]
	}
	W.x.getb().geta().GetB().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+5*MB] = t[i]
	}
	W.x.getb().getb().GetA().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+6*MB] = t[i]
	}
	W.x.getb().getb().GetB().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+7*MB] = t[i]
	}

	W.y.geta().geta().GetA().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+8*MB] = t[i]
	}
	W.y.geta().geta().GetB().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+9*MB] = t[i]
	}
	W.y.geta().getb().GetA().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+10*MB] = t[i]
	}
	W.y.geta().getb().GetB().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+11*MB] = t[i]
	}

	W.y.getb().geta().GetA().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+12*MB] = t[i]
	}
	W.y.getb().geta().GetB().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+13*MB] = t[i]
	}
	W.y.getb().getb().GetA().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+14*MB] = t[i]
	}
	W.y.getb().getb().GetB().ToBytes(t[:])
	for i := 0; i < MB; i++ {
		b[i+15*MB] = t[i]
	}
}

/* convert from byte array to point */
func ECP8_fromBytes(b []byte) *ECP8 {
	var t [int(MODBYTES)]byte
	MB := int(MODBYTES)

	for i := 0; i < MB; i++ {
		t[i] = b[i]
	}
	ra := FromBytes(t[:])
	for i := 0; i < MB; i++ {
		t[i] = b[i+MB]
	}
	rb := FromBytes(t[:])

	ra4 := NewFP2bigs(ra, rb)

	for i := 0; i < MB; i++ {
		t[i] = b[i+2*MB]
	}
	ra = FromBytes(t[:])
	for i := 0; i < MB; i++ {
		t[i] = b[i+3*MB]
	}
	rb = FromBytes(t[:])

	rb4 := NewFP2bigs(ra, rb)

	ra8 := NewFP4fp2s(ra4, rb4)

	for i := 0; i < MB; i++ {
		t[i] = b[i+4*MB]
	}
	ra = FromBytes(t[:])
	for i := 0; i < MB; i++ {
		t[i] = b[i+5*MB]
	}
	rb = FromBytes(t[:])

	ra4 = NewFP2bigs(ra, rb)

	for i := 0; i < MB; i++ {
		t[i] = b[i+6*MB]
	}
	ra = FromBytes(t[:])
	for i := 0; i < MB; i++ {
		t[i] = b[i+7*MB]
	}
	rb = FromBytes(t[:])

	rb4 = NewFP2bigs(ra, rb)

	rb8 := NewFP4fp2s(ra4, rb4)

	rx := NewFP8fp4s(ra8, rb8)

	for i := 0; i < MB; i++ {
		t[i] = b[i+8*MB]
	}
	ra = FromBytes(t[:])
	for i := 0; i < MB; i++ {
		t[i] = b[i+9*MB]
	}
	rb = FromBytes(t[:])

	ra4 = NewFP2bigs(ra, rb)

	for i := 0; i < MB; i++ {
		t[i] = b[i+10*MB]
	}
	ra = FromBytes(t[:])
	for i := 0; i < MB; i++ {
		t[i] = b[i+11*MB]
	}
	rb = FromBytes(t[:])

	rb4 = NewFP2bigs(ra, rb)

	ra8 = NewFP4fp2s(ra4, rb4)

	for i := 0; i < MB; i++ {
		t[i] = b[i+12*MB]
	}
	ra = FromBytes(t[:])
	for i := 0; i < MB; i++ {
		t[i] = b[i+13*MB]
	}
	rb = FromBytes(t[:])

	ra4 = NewFP2bigs(ra, rb)

	for i := 0; i < MB; i++ {
		t[i] = b[i+14*MB]
	}
	ra = FromBytes(t[:])
	for i := 0; i < MB; i++ {
		t[i] = b[i+15*MB]
	}
	rb = FromBytes(t[:])

	rb4 = NewFP2bigs(ra, rb)

	rb8 = NewFP4fp2s(ra4, rb4)

	ry := NewFP8fp4s(ra8, rb8)

	return NewECP8fp8s(rx, ry)
}

/* convert this to hex string */
func (E *ECP8) ToString() string {
	W := NewECP8()
	W.Copy(E)
	W.Affine()
	if W.Is_infinity() {
		return "infinity"
	}
	return "(" + W.x.toString() + "," + W.y.toString() + ")"
}

/* Calculate RHS of twisted curve equation x^3+B/i */
func RHS8(x *FP8) *FP8 {
	r := NewFP8copy(x)
	r.sqr()
	b2 := NewFP2big(NewBIGints(CURVE_B))
	b4 := NewFP4fp2(b2)
	b := NewFP8fp4(b4)

	if SEXTIC_TWIST == D_TYPE {
		b.div_i()
	}
	if SEXTIC_TWIST == M_TYPE {
		b.times_i()
	}
	r.mul(x)
	r.add(b)

	r.reduce()
	return r
}

/* construct this from (x,y) - but set to O if not on curve */
func NewECP8fp8s(ix *FP8, iy *FP8) *ECP8 {
	E := new(ECP8)
	E.x = NewFP8copy(ix)
	E.y = NewFP8copy(iy)
	E.z = NewFP8int(1)
	E.x.norm()
	rhs := RHS8(E.x)
	y2 := NewFP8copy(E.y)
	y2.sqr()
	if !y2.Equals(rhs) {
		E.inf()
	}
	return E
}

/* construct this from x - but set to O if not on curve */
func NewECP8fp8(ix *FP8) *ECP8 {
	E := new(ECP8)
	E.x = NewFP8copy(ix)
	E.y = NewFP8int(1)
	E.z = NewFP8int(1)
	E.x.norm()
	rhs := RHS8(E.x)
	if rhs.sqrt() {
		E.y.copy(rhs)
	} else {
		E.inf()
	}
	return E
}

/* this+=this */
func (E *ECP8) dbl() int {
	iy := NewFP8copy(E.y)
	if SEXTIC_TWIST == D_TYPE {
		iy.times_i()
	}

	t0 := NewFP8copy(E.y)
	t0.sqr()
	if SEXTIC_TWIST == D_TYPE {
		t0.times_i()
	}
	t1 := NewFP8copy(iy)
	t1.mul(E.z)
	t2 := NewFP8copy(E.z)
	t2.sqr()

	E.z.copy(t0)
	E.z.add(t0)
	E.z.norm()
	E.z.add(E.z)
	E.z.add(E.z)
	E.z.norm()

	t2.imul(3 * CURVE_B_I)
	if SEXTIC_TWIST == M_TYPE {
		t2.times_i()
	}
	x3 := NewFP8copy(t2)
	x3.mul(E.z)

	y3 := NewFP8copy(t0)

	y3.add(t2)
	y3.norm()
	E.z.mul(t1)
	t1.copy(t2)
	t1.add(t2)
	t2.add(t1)
	t2.norm()
	t0.sub(t2)
	t0.norm() //y^2-9bz^2
	y3.mul(t0)
	y3.add(x3) //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2
	t1.copy(E.x)
	t1.mul(iy) //
	E.x.copy(t0)
	E.x.norm()
	E.x.mul(t1)
	E.x.add(E.x) //(y^2-9bz^2)xy2

	E.x.norm()
	E.y.copy(y3)
	E.y.norm()

	return 1
}

/* this+=Q - return 0 for add, 1 for double, -1 for O */
func (E *ECP8) Add(Q *ECP8) int {
	b := 3 * CURVE_B_I
	t0 := NewFP8copy(E.x)
	t0.mul(Q.x) // x.Q.x
	t1 := NewFP8copy(E.y)
	t1.mul(Q.y) // y.Q.y

	t2 := NewFP8copy(E.z)
	t2.mul(Q.z)
	t3 := NewFP8copy(E.x)
	t3.add(E.y)
	t3.norm() //t3=X1+Y1
	t4 := NewFP8copy(Q.x)
	t4.add(Q.y)
	t4.norm()  //t4=X2+Y2
	t3.mul(t4) //t3=(X1+Y1)(X2+Y2)
	t4.copy(t0)
	t4.add(t1) //t4=X1.X2+Y1.Y2

	t3.sub(t4)
	t3.norm()
	if SEXTIC_TWIST == D_TYPE {
		t3.times_i() //t3=(X1+Y1)(X2+Y2)-(X1.X2+Y1.Y2) = X1.Y2+X2.Y1
	}
	t4.copy(E.y)
	t4.add(E.z)
	t4.norm() //t4=Y1+Z1
	x3 := NewFP8copy(Q.y)
	x3.add(Q.z)
	x3.norm() //x3=Y2+Z2

	t4.mul(x3)  //t4=(Y1+Z1)(Y2+Z2)
	x3.copy(t1) //
	x3.add(t2)  //X3=Y1.Y2+Z1.Z2

	t4.sub(x3)
	t4.norm()
	if SEXTIC_TWIST == D_TYPE {
		t4.times_i() //t4=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1
	}
	x3.copy(E.x)
	x3.add(E.z)
	x3.norm() // x3=X1+Z1
	y3 := NewFP8copy(Q.x)
	y3.add(Q.z)
	y3.norm()  // y3=X2+Z2
	x3.mul(y3) // x3=(X1+Z1)(X2+Z2)
	y3.copy(t0)
	y3.add(t2) // y3=X1.X2+Z1+Z2
	y3.rsub(x3)
	y3.norm() // y3=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1

	if SEXTIC_TWIST == D_TYPE {
		t0.times_i() // x.Q.x
		t1.times_i() // y.Q.y
	}
	x3.copy(t0)
	x3.add(t0)
	t0.add(x3)
	t0.norm()
	t2.imul(b)
	if SEXTIC_TWIST == M_TYPE {
		t2.times_i()
	}
	z3 := NewFP8copy(t1)
	z3.add(t2)
	z3.norm()
	t1.sub(t2)
	t1.norm()
	y3.imul(b)
	if SEXTIC_TWIST == M_TYPE {
		y3.times_i()
	}
	x3.copy(y3)
	x3.mul(t4)
	t2.copy(t3)
	t2.mul(t1)
	x3.rsub(t2)
	y3.mul(t0)
	t1.mul(z3)
	y3.add(t1)
	t0.mul(t3)
	z3.mul(t4)
	z3.add(t0)

	E.x.copy(x3)
	E.x.norm()
	E.y.copy(y3)
	E.y.norm()
	E.z.copy(z3)
	E.z.norm()

	return 0
}

/* set this-=Q */
func (E *ECP8) Sub(Q *ECP8) int {
	NQ := NewECP8()
	NQ.Copy(Q)
	NQ.neg()
	D := E.Add(NQ)
	return D
}

func ECP8_frob_constants() [3]*FP2 {
	Fra := NewBIGints(Fra)
	Frb := NewBIGints(Frb)
	X := NewFP2bigs(Fra, Frb)

	F0 := NewFP2copy(X)
	F0.sqr()
	F2 := NewFP2copy(F0)
	F2.mul_ip()
	F2.norm()
	F1 := NewFP2copy(F2)
	F1.sqr()
	F2.mul(F1)

	F2.mul_ip()
	F2.norm()

	F1.copy(X)
	if SEXTIC_TWIST == M_TYPE {
		F1.mul_ip();  F1.norm()
		F1.inverse()
		F0.copy(F1)
		F0.sqr()
		F1.mul(F0)
	}
	if SEXTIC_TWIST == D_TYPE {
		F0.copy(F1); F0.sqr()
		F1.mul(F0)
		F0.mul_ip();  F0.norm()
		F1.mul_ip();  F1.norm()
		F1.mul_ip();  F1.norm()
	}

	F := [3]*FP2{F0, F1, F2}
	return F
}

/* set this*=q, where q is Modulus, using Frobenius */
func (E *ECP8) frob(F [3]*FP2, n int) {
	for i := 0; i < n; i++ {
		E.x.frob(F[2])
		if SEXTIC_TWIST == M_TYPE {
			E.x.qmul(F[0])
			E.x.times_i2()
		}
		if SEXTIC_TWIST == D_TYPE {
			E.x.qmul(F[0])
			E.x.times_i2()
		}	
		E.y.frob(F[2])
		if SEXTIC_TWIST == M_TYPE {
			E.y.qmul(F[1])
			E.y.times_i2()
			E.y.times_i()
		}
		if SEXTIC_TWIST == D_TYPE {
			E.y.qmul(F[1])
			E.y.times_i()
		}

		E.z.frob(F[2])
	}
}

/* P*=e */
func (E *ECP8) mul(e *BIG) *ECP8 {
	/* fixed size windows */
	mt := NewBIG()
	t := NewBIG()
	P := NewECP8()
	Q := NewECP8()
	C := NewECP8()

	if E.Is_infinity() {
		return NewECP8()
	}

	var W []*ECP8
	var w [1 + (NLEN*int(BASEBITS)+3)/4]int8

	/* precompute table */
	Q.Copy(E)
	Q.dbl()

	W = append(W, NewECP8())
	W[0].Copy(E)

	for i := 1; i < 8; i++ {
		W = append(W, NewECP8())
		W[i].Copy(W[i-1])
		W[i].Add(Q)
	}

	/* make exponent odd - add 2P if even, P if odd */
	t.copy(e)
	s := int(t.parity())
	t.inc(1)
	t.norm()
	ns := int(t.parity())
	mt.copy(t)
	mt.inc(1)
	mt.norm()
	t.cmove(mt, s)
	Q.cmove(E, ns)
	C.Copy(Q)

	nb := 1 + (t.nbits()+3)/4
	/* convert exponent to signed 4-bit window */
	for i := 0; i < nb; i++ {
		w[i] = int8(t.lastbits(5) - 16)
		t.dec(int(w[i]))
		t.norm()
		t.fshr(4)
	}
	w[nb] = int8(t.lastbits(5))

	P.Copy(W[(w[nb]-1)/2])
	for i := nb - 1; i >= 0; i-- {
		Q.selector(W, int32(w[i]))
		P.dbl()
		P.dbl()
		P.dbl()
		P.dbl()
		P.Add(Q)
	}
	P.Sub(C)
	P.Affine()
	return P
}

/* Public version */
func (E *ECP8) Mul(e *BIG) *ECP8 {
	return E.mul(e)
}

func ECP8_generator() *ECP8 {
	var G *ECP8
	G = NewECP8fp8s(
		NewFP8fp4s(
			NewFP4fp2s(
				NewFP2bigs(NewBIGints(CURVE_Pxaaa), NewBIGints(CURVE_Pxaab)),
				NewFP2bigs(NewBIGints(CURVE_Pxaba), NewBIGints(CURVE_Pxabb))),
			NewFP4fp2s(
				NewFP2bigs(NewBIGints(CURVE_Pxbaa), NewBIGints(CURVE_Pxbab)),
				NewFP2bigs(NewBIGints(CURVE_Pxbba), NewBIGints(CURVE_Pxbbb)))),
		NewFP8fp4s(
			NewFP4fp2s(
				NewFP2bigs(NewBIGints(CURVE_Pyaaa), NewBIGints(CURVE_Pyaab)),
				NewFP2bigs(NewBIGints(CURVE_Pyaba), NewBIGints(CURVE_Pyabb))),
			NewFP4fp2s(
				NewFP2bigs(NewBIGints(CURVE_Pybaa), NewBIGints(CURVE_Pybab)),
				NewFP2bigs(NewBIGints(CURVE_Pybba), NewBIGints(CURVE_Pybbb)))))
	return G
}

/* needed for SOK */
func ECP8_mapit(h []byte) *ECP8 {
	q := NewBIGints(Modulus)
	hv := FromBytes(h[:])
	one := NewBIGint(1)
	var X2 *FP2
	var X4 *FP4
	var X *FP8
	var Q *ECP8
	hv.Mod(q)
	for true {
		X2 = NewFP2bigs(one, hv)
		X4 = NewFP4fp2(X2)
		X = NewFP8fp4(X4)
		Q = NewECP8fp8(X)
		if !Q.Is_infinity() {
			break
		}
		hv.inc(1)
		hv.norm()
	}
	F := ECP8_frob_constants()
	x := NewBIGints(CURVE_Bnx)

	xQ := Q.mul(x)
	x2Q := xQ.mul(x)
	x3Q := x2Q.mul(x)
	x4Q := x3Q.mul(x)
	x5Q := x4Q.mul(x)
	x6Q := x5Q.mul(x)
	x7Q := x6Q.mul(x)
	x8Q := x7Q.mul(x)

	if SIGN_OF_X == NEGATIVEX {
		xQ.neg()
		x3Q.neg()
		x5Q.neg()
		x7Q.neg()
	}
	x8Q.Sub(x7Q)
	x8Q.Sub(Q)

	x7Q.Sub(x6Q)
	x7Q.frob(F, 1)

	x6Q.Sub(x5Q)
	x6Q.frob(F, 2)

	x5Q.Sub(x4Q)
	x5Q.frob(F, 3)

	x4Q.Sub(x3Q)
	x4Q.frob(F, 4)

	x3Q.Sub(x2Q)
	x3Q.frob(F, 5)

	x2Q.Sub(xQ)
	x2Q.frob(F, 6)

	xQ.Sub(Q)
	xQ.frob(F, 7)

	Q.dbl()
	Q.frob(F, 8)

	Q.Add(x8Q)
	Q.Add(x7Q)
	Q.Add(x6Q)
	Q.Add(x5Q)

	Q.Add(x4Q)
	Q.Add(x3Q)
	Q.Add(x2Q)
	Q.Add(xQ)

	Q.Affine()
	return Q
}

/* P=u0.Q0+u1*Q1+u2*Q2+u3*Q3.. */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
// Side channel attack secure
func mul16(Q []*ECP8, u []*BIG) *ECP8 {
	W := NewECP8()
	P := NewECP8()
	var T1 []*ECP8
	var T2 []*ECP8
	var T3 []*ECP8
	var T4 []*ECP8
	mt := NewBIG()
	var t []*BIG
	var bt int8
	var k int

	var w1 [NLEN*int(BASEBITS) + 1]int8
	var s1 [NLEN*int(BASEBITS) + 1]int8
	var w2 [NLEN*int(BASEBITS) + 1]int8
	var s2 [NLEN*int(BASEBITS) + 1]int8
	var w3 [NLEN*int(BASEBITS) + 1]int8
	var s3 [NLEN*int(BASEBITS) + 1]int8
	var w4 [NLEN*int(BASEBITS) + 1]int8
	var s4 [NLEN*int(BASEBITS) + 1]int8

	for i := 0; i < 16; i++ {
		t = append(t, NewBIGcopy(u[i]))
	}

	T1 = append(T1, NewECP8())
	T1[0].Copy(Q[0]) // Q[0]
	T1 = append(T1, NewECP8())
	T1[1].Copy(T1[0])
	T1[1].Add(Q[1]) // Q[0]+Q[1]
	T1 = append(T1, NewECP8())
	T1[2].Copy(T1[0])
	T1[2].Add(Q[2]) // Q[0]+Q[2]
	T1 = append(T1, NewECP8())
	T1[3].Copy(T1[1])
	T1[3].Add(Q[2]) // Q[0]+Q[1]+Q[2]
	T1 = append(T1, NewECP8())
	T1[4].Copy(T1[0])
	T1[4].Add(Q[3]) // Q[0]+Q[3]
	T1 = append(T1, NewECP8())
	T1[5].Copy(T1[1])
	T1[5].Add(Q[3]) // Q[0]+Q[1]+Q[3]
	T1 = append(T1, NewECP8())
	T1[6].Copy(T1[2])
	T1[6].Add(Q[3]) // Q[0]+Q[2]+Q[3]
	T1 = append(T1, NewECP8())
	T1[7].Copy(T1[3])
	T1[7].Add(Q[3]) // Q[0]+Q[1]+Q[2]+Q[3]


	T2 = append(T2, NewECP8())
	T2[0].Copy(Q[4]) // Q[0]
	T2 = append(T2, NewECP8())
	T2[1].Copy(T2[0])
	T2[1].Add(Q[5]) // Q[0]+Q[1]
	T2 = append(T2, NewECP8())
	T2[2].Copy(T2[0])
	T2[2].Add(Q[6]) // Q[0]+Q[2]
	T2 = append(T2, NewECP8())
	T2[3].Copy(T2[1])
	T2[3].Add(Q[6]) // Q[0]+Q[1]+Q[2]
	T2 = append(T2, NewECP8())
	T2[4].Copy(T2[0])
	T2[4].Add(Q[7]) // Q[0]+Q[3]
	T2 = append(T2, NewECP8())
	T2[5].Copy(T2[1])
	T2[5].Add(Q[7]) // Q[0]+Q[1]+Q[3]
	T2 = append(T2, NewECP8())
	T2[6].Copy(T2[2])
	T2[6].Add(Q[7]) // Q[0]+Q[2]+Q[3]
	T2 = append(T2, NewECP8())
	T2[7].Copy(T2[3])
	T2[7].Add(Q[7]) // Q[0]+Q[1]+Q[2]+Q[3]

	T3 = append(T3, NewECP8())
	T3[0].Copy(Q[8]) // Q[0]
	T3 = append(T3, NewECP8())
	T3[1].Copy(T3[0])
	T3[1].Add(Q[9]) // Q[0]+Q[1]
	T3 = append(T3, NewECP8())
	T3[2].Copy(T3[0])
	T3[2].Add(Q[10]) // Q[0]+Q[2]
	T3 = append(T3, NewECP8())
	T3[3].Copy(T3[1])
	T3[3].Add(Q[10]) // Q[0]+Q[1]+Q[2]
	T3 = append(T3, NewECP8())
	T3[4].Copy(T3[0])
	T3[4].Add(Q[11]) // Q[0]+Q[3]
	T3 = append(T3, NewECP8())
	T3[5].Copy(T3[1])
	T3[5].Add(Q[11]) // Q[0]+Q[1]+Q[3]
	T3 = append(T3, NewECP8())
	T3[6].Copy(T3[2])
	T3[6].Add(Q[11]) // Q[0]+Q[2]+Q[3]
	T3 = append(T3, NewECP8())
	T3[7].Copy(T3[3])
	T3[7].Add(Q[11]) // Q[0]+Q[1]+Q[2]+Q[3]

	T4 = append(T4, NewECP8())
	T4[0].Copy(Q[12]) // Q[0]
	T4 = append(T4, NewECP8())
	T4[1].Copy(T4[0])
	T4[1].Add(Q[13]) // Q[0]+Q[1]
	T4 = append(T4, NewECP8())
	T4[2].Copy(T4[0])
	T4[2].Add(Q[14]) // Q[0]+Q[2]
	T4 = append(T4, NewECP8())
	T4[3].Copy(T4[1])
	T4[3].Add(Q[14]) // Q[0]+Q[1]+Q[2]
	T4 = append(T4, NewECP8())
	T4[4].Copy(T4[0])
	T4[4].Add(Q[15]) // Q[0]+Q[3]
	T4 = append(T4, NewECP8())
	T4[5].Copy(T4[1])
	T4[5].Add(Q[15]) // Q[0]+Q[1]+Q[3]
	T4 = append(T4, NewECP8())
	T4[6].Copy(T4[2])
	T4[6].Add(Q[15]) // Q[0]+Q[2]+Q[3]
	T4 = append(T4, NewECP8())
	T4[7].Copy(T4[3])
	T4[7].Add(Q[15]) // Q[0]+Q[1]+Q[2]+Q[3]

	// Make them odd
	pb1 := 1 - t[0].parity()
	t[0].inc(pb1)

	pb2 := 1 - t[4].parity()
	t[4].inc(pb2)

	pb3 := 1 - t[8].parity()
	t[8].inc(pb3)

	pb4 := 1 - t[12].parity()
	t[12].inc(pb4)

	// Number of bits
	mt.zero()
	for i := 0; i < 16; i++ {
		t[i].norm()
		mt.or(t[i])
	}

	nb := 1 + mt.nbits()

	// Sign pivot
	s1[nb-1] = 1
	s2[nb-1] = 1
	s3[nb-1] = 1
	s4[nb-1] = 1
	for i := 0; i < nb-1; i++ {
		t[0].fshr(1)
		s1[i] = 2*int8(t[0].parity()) - 1
		t[4].fshr(1)
		s2[i] = 2*int8(t[4].parity()) - 1
		t[8].fshr(1)
		s3[i] = 2*int8(t[8].parity()) - 1
		t[12].fshr(1)
		s4[i] = 2*int8(t[12].parity()) - 1

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
		w3[i] = 0
		k = 1
		for j := 9; j < 12; j++ {
			bt = s3[i] * int8(t[j].parity())
			t[j].fshr(1)
			t[j].dec(int(bt) >> 1)
			t[j].norm()
			w3[i] += bt * int8(k)
			k *= 2
		}
		w4[i] = 0
		k = 1
		for j := 13; j < 16; j++ {
			bt = s4[i] * int8(t[j].parity())
			t[j].fshr(1)
			t[j].dec(int(bt) >> 1)
			t[j].norm()
			w4[i] += bt * int8(k)
			k *= 2
		}
	}

	// Main loop
	P.selector(T1, int32(2*w1[nb-1]+1))
	W.selector(T2, int32(2*w2[nb-1]+1))
	P.Add(W)
	W.selector(T3, int32(2*w3[nb-1]+1))
	P.Add(W)
	W.selector(T4, int32(2*w4[nb-1]+1))
	P.Add(W)
	for i := nb - 2; i >= 0; i-- {
		P.dbl()
		W.selector(T1, int32(2*w1[i]+s1[i]))
		P.Add(W)
		W.selector(T2, int32(2*w2[i]+s2[i]))
		P.Add(W)
		W.selector(T3, int32(2*w3[i]+s3[i]))
		P.Add(W)
		W.selector(T4, int32(2*w4[i]+s4[i]))
		P.Add(W)

	}

	// apply correction
	W.Copy(P)
	W.Sub(Q[0])
	P.cmove(W, pb1)
	W.Copy(P)
	W.Sub(Q[4])
	P.cmove(W, pb2)
	W.Copy(P)
	W.Sub(Q[8])
	P.cmove(W, pb3)
	W.Copy(P)
	W.Sub(Q[12])
	P.cmove(W, pb4)

	P.Affine()
	return P
}
