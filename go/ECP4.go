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

/* MiotCL Weierstrass elliptic curve functions over FP2 */

package XXX

//import "fmt"

type ECP4 struct {
	x *FP4
	y *FP4
	z *FP4
}

func NewECP4() *ECP4 {
	E := new(ECP4)
	E.x = NewFP4()
	E.y = NewFP4int(1)
	E.z = NewFP4()
	return E
}

/* Test this=O? */
func (E *ECP4) Is_infinity() bool {
	return E.x.iszilch() && E.z.iszilch()
}

/* copy this=P */
func (E *ECP4) Copy(P *ECP4) {
	E.x.copy(P.x)
	E.y.copy(P.y)
	E.z.copy(P.z)
}

/* set this=O */
func (E *ECP4) inf() {
	E.x.zero()
	E.y.one()
	E.z.zero()
}

/* set this=-this */
func (E *ECP4) neg() {
	E.y.norm()
	E.y.neg()
	E.y.norm()
}

/* Conditional move of Q to P dependant on d */
func (E *ECP4) cmove(Q *ECP4, d int) {
	E.x.cmove(Q.x, d)
	E.y.cmove(Q.y, d)
	E.z.cmove(Q.z, d)
}

/* Constant time select from pre-computed table */
func (E *ECP4) selector(W []*ECP4, b int32) {
	MP := NewECP4()
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
func (E *ECP4) Equals(Q *ECP4) bool {
	if E.Is_infinity() && Q.Is_infinity() {
		return true
	}
	if E.Is_infinity() || Q.Is_infinity() {
		return false
	}

	a := NewFP4copy(E.x)
	b := NewFP4copy(Q.x)
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
func (E *ECP4) Affine() {
	if E.Is_infinity() {
		return
	}
	one := NewFP4int(1)
	if E.z.Equals(one) {
		E.x.reduce()
		E.y.reduce()
		return
	}
	E.z.inverse(nil)

	E.x.mul(E.z)
	E.x.reduce()
	E.y.mul(E.z)
	E.y.reduce()
	E.z.copy(one)
}

/* extract affine x as FP2 */
func (E *ECP4) GetX() *FP4 {
	W := NewECP4()
	W.Copy(E)
	W.Affine()
	return W.x
}

/* extract affine y as FP2 */
func (E *ECP4) GetY() *FP4 {
	W := NewECP4()
	W.Copy(E)
	W.Affine()
	return W.y
}

/* extract projective x */
func (E *ECP4) getx() *FP4 {
	return E.x
}

/* extract projective y */
func (E *ECP4) gety() *FP4 {
	return E.y
}

/* extract projective z */
func (E *ECP4) getz() *FP4 {
	return E.z
}

/* convert to byte array */
func (E *ECP4) ToBytes(b []byte, compress bool) {
	var t [4*int(MODBYTES)]byte
	MB := 4*int(MODBYTES)
	alt:=false
	W := NewECP4()
	W.Copy(E)
	W.Affine()
	W.x.ToBytes(t[:])

	if (MODBITS-1)%8 <= 4 && ALLOW_ALT_COMPRESS {
		alt=true
	}

    if alt {
		for i:=0;i<MB;i++ {
			b[i]=t[i]
		}
        if !compress {
            W.y.ToBytes(t[:]);
            for i:=0;i<MB;i++ {
				b[i+MB]=t[i]
			}
        } else {
            b[0]|=0x80
            if W.y.islarger()==1 {
				b[0]|=0x20
			}
        }
		
	} else {
		for i:=0;i<MB;i++ {
			b[i+1]=t[i]
		}
        if !compress {
            b[0]=0x04
            W.y.ToBytes(t[:])
	        for i:=0;i<MB;i++ {
			    b[i+MB+1]=t[i]
			}
        } else {
            b[0]=0x02
            if W.y.sign() == 1 {
                b[0]=0x03
			}
        }
	}
}

/* convert from byte array to point */
func ECP4_fromBytes(b []byte) *ECP4 {
	var t [4*int(MODBYTES)]byte
	MB := 4*int(MODBYTES)
	typ := int(b[0])
	alt:=false

	if (MODBITS-1)%8 <= 4 && ALLOW_ALT_COMPRESS {
		alt=true
	}

	if alt {
        for i:=0;i<MB;i++  {
			t[i]=b[i]
		}
        t[0]&=0x1f
        rx:=FP4_fromBytes(t[:]);
        if (b[0]&0x80)==0 {
            for i:=0;i<MB;i++ {
				t[i]=b[i+MB]
			}
            ry:=FP4_fromBytes(t[:])
            return NewECP4fp4s(rx,ry)
        } else {
            sgn:=(b[0]&0x20)>>5
            P:=NewECP4fp4(rx,0)
            cmp:=P.y.islarger()
            if (sgn==1 && cmp!=1) || (sgn==0 && cmp==1) {
				P.neg()
			}
            return P;
        }
    } else {
		for i:=0;i<MB;i++ {
			t[i]=b[i+1]
		}
        rx:=FP4_fromBytes(t[:])
        if typ == 0x04 {
		    for i:=0;i<MB;i++ {
				t[i]=b[i+MB+1]
			}
		    ry:=FP4_fromBytes(t[:])
		    return NewECP4fp4s(rx,ry)
        } else {
            return NewECP4fp4(rx,typ&1)
        }
    }
}

/* convert this to hex string */
func (E *ECP4) ToString() string {
	W := NewECP4()
	W.Copy(E)
	W.Affine()
	if W.Is_infinity() {
		return "infinity"
	}
	return "(" + W.x.toString() + "," + W.y.toString() + ")"
}

/* Calculate RHS of twisted curve equation x^3+B/i */
func RHS4(x *FP4) *FP4 {
	r := NewFP4copy(x)
	r.sqr()
	b2 := NewFP2big(NewBIGints(CURVE_B))
	b := NewFP4fp2(b2)

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
func NewECP4fp4s(ix *FP4, iy *FP4) *ECP4 {
	E := new(ECP4)
	E.x = NewFP4copy(ix)
	E.y = NewFP4copy(iy)
	E.z = NewFP4int(1)
	E.x.norm()
	rhs := RHS4(E.x)
	y2 := NewFP4copy(E.y)
	y2.sqr()
	if !y2.Equals(rhs) {
		E.inf()
	}
	return E
}

/* construct this from x - but set to O if not on curve */
func NewECP4fp4(ix *FP4, s int) *ECP4 {
	E := new(ECP4)
	h:=NewFP()
	E.x = NewFP4copy(ix)
	E.y = NewFP4int(1)
	E.z = NewFP4int(1)
	E.x.norm()
	rhs := RHS4(E.x)
	if rhs.qr(h) == 1 {
		rhs.sqrt(h)
		if rhs.sign() != s {
			rhs.neg()
		}
		rhs.reduce()
		E.y.copy(rhs)

	} else {
		E.inf()
	}
	return E
}

/* this+=this */
func (E *ECP4) dbl() int {
	iy := NewFP4copy(E.y)
	if SEXTIC_TWIST == D_TYPE {
		iy.times_i()
	}

	t0 := NewFP4copy(E.y) //***** Change
	t0.sqr()
	if SEXTIC_TWIST == D_TYPE {
		t0.times_i()
	}
	t1 := NewFP4copy(iy)
	t1.mul(E.z)
	t2 := NewFP4copy(E.z)
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
	x3 := NewFP4copy(t2)
	x3.mul(E.z)

	y3 := NewFP4copy(t0)

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
func (E *ECP4) Add(Q *ECP4) int {

	b := 3 * CURVE_B_I
	t0 := NewFP4copy(E.x)
	t0.mul(Q.x) // x.Q.x
	t1 := NewFP4copy(E.y)
	t1.mul(Q.y) // y.Q.y

	t2 := NewFP4copy(E.z)
	t2.mul(Q.z)
	t3 := NewFP4copy(E.x)
	t3.add(E.y)
	t3.norm() //t3=X1+Y1
	t4 := NewFP4copy(Q.x)
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
	x3 := NewFP4copy(Q.y)
	x3.add(Q.z)
	x3.norm() //x3=Y2+Z2

	t4.mul(x3) //t4=(Y1+Z1)(Y2+Z2)
	x3.copy(t1)
	x3.add(t2) //X3=Y1.Y2+Z1.Z2

	t4.sub(x3)
	t4.norm()
	if SEXTIC_TWIST == D_TYPE {
		t4.times_i() //t4=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1
	}
	x3.copy(E.x)
	x3.add(E.z)
	x3.norm() // x3=X1+Z1
	y3 := NewFP4copy(Q.x)
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
	z3 := NewFP4copy(t1)
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
func (E *ECP4) Sub(Q *ECP4) int {
	NQ := NewECP4()
	NQ.Copy(Q)
	NQ.neg()
	D := E.Add(NQ)
	return D
}

func ECP4_frob_constants() [3]*FP2 {

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
	F1.copy(X)
	if SEXTIC_TWIST == M_TYPE {
		F1.mul_ip()
		F1.inverse(nil)
		F0.copy(F1)
		F0.sqr()
	}
	F0.mul_ip()
	F0.norm()
	F1.mul(F0)
	F := [3]*FP2{F0, F1, F2}
	return F
}

/* set this*=q, where q is Modulus, using Frobenius */
func (E *ECP4) frob(F [3]*FP2, n int) {
	for i := 0; i < n; i++ {
		E.x.frob(F[2])
		E.x.pmul(F[0])

		E.y.frob(F[2])
		E.y.pmul(F[1])
		E.y.times_i()

		E.z.frob(F[2])
	}
}

func (E *ECP4) reduce() {
	E.x.reduce()
	E.y.reduce()
	E.z.reduce()
}

/* P*=e */
func (E *ECP4) mul(e *BIG) *ECP4 {
	/* fixed size windows */
	mt := NewBIG()
	t := NewBIG()
	P := NewECP4()
	Q := NewECP4()
	C := NewECP4()

	if E.Is_infinity() {
		return NewECP4()
	}

	var W []*ECP4
	var w [1 + (NLEN*int(BASEBITS)+3)/4]int8

	/* precompute table */
	Q.Copy(E)
	Q.dbl()

	W = append(W, NewECP4())
	W[0].Copy(E)

	for i := 1; i < 8; i++ {
		W = append(W, NewECP4())
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
func (E *ECP4) Mul(e *BIG) *ECP4 {
	return E.mul(e)
}


/* clear cofactor */
func (E *ECP4) Cfp()  {

	F := ECP4_frob_constants()
	x := NewBIGints(CURVE_Bnx)
	xQ := E.mul(x)
	x2Q := xQ.mul(x)
	x3Q := x2Q.mul(x)
	x4Q := x3Q.mul(x)

	if SIGN_OF_X == NEGATIVEX {
		xQ.neg()
		x3Q.neg()
	}

	x4Q.Sub(x3Q)
	x4Q.Sub(E)

	x3Q.Sub(x2Q)
	x3Q.frob(F, 1)

	x2Q.Sub(xQ)
	x2Q.frob(F, 2)

	xQ.Sub(E)
	xQ.frob(F, 3)

	E.dbl()
	E.frob(F, 4)

	E.Add(x4Q)
	E.Add(x3Q)
	E.Add(x2Q)
	E.Add(xQ)

	E.Affine()
}


func ECP4_generator() *ECP4 {
	var G *ECP4
	G = NewECP4fp4s(
		NewFP4fp2s(
			NewFP2bigs(NewBIGints(CURVE_Pxaa), NewBIGints(CURVE_Pxab)),
			NewFP2bigs(NewBIGints(CURVE_Pxba), NewBIGints(CURVE_Pxbb))),
		NewFP4fp2s(
			NewFP2bigs(NewBIGints(CURVE_Pyaa), NewBIGints(CURVE_Pyab)),

			NewFP2bigs(NewBIGints(CURVE_Pyba), NewBIGints(CURVE_Pybb))))
	return G
}

/* Hunt and Peck a BIG to a curve point */
func ECP4_hap2point(h *BIG) *ECP4 {
	one := NewBIGint(1)
	x := NewBIGcopy(h)
	var X2 *FP2
	var X4 *FP4
	var Q *ECP4
	for true {
		X2 = NewFP2bigs(one, x)
		X4 = NewFP4fp2(X2)
		Q = NewECP4fp4(X4,0)
		if !Q.Is_infinity() {
			break
		}
		x.inc(1)
		x.norm()
	}
	return Q
}

/* Constant time Map to Point */
func ECP4_map2point(H *FP4) *ECP4 {
    // Shallue and van de Woestijne
    NY:=NewFP4int(1)
    T:=NewFP4copy(H)
    sgn:=T.sign()

	Z:=NewFPint(RIADZG2A);
	X1:=NewFP4fp(Z)
	X3:=NewFP4copy(X1)
	A:=RHS4(X1)
	W:=NewFP4copy(A)
	W.sqrt(nil)

	s:=NewFPbig(NewBIGints(SQRTm3))
	Z.mul(s)

	T.sqr()
	Y:=NewFP4copy(A); Y.mul(T)
	T.copy(NY); T.add(Y); T.norm()
	Y.rsub(NY); Y.norm()
	NY.copy(T); NY.mul(Y)
	
	NY.qmul(Z)
	NY.inverse(nil)

	W.qmul(Z)
    if (W.sign()==1) {
        W.neg()
        W.norm()
    }
    W.qmul(Z)
	W.mul(H); W.mul(Y); W.mul(NY)

	X1.neg(); X1.norm(); X1.div2()
	X2:=NewFP4copy(X1)
	X1.sub(W); X1.norm()
	X2.add(W); X2.norm()
	A.add(A); A.add(A); A.norm()
	T.sqr(); T.mul(NY); T.sqr()
	A.mul(T)
	X3.add(A); X3.norm()	

    Y.copy(RHS4(X2))
    X3.cmove(X2,Y.qr(nil))
    Y.copy(RHS4(X1))
    X3.cmove(X1,Y.qr(nil))
    Y.copy(RHS4(X3))
    Y.sqrt(nil)

    ne:=Y.sign()^sgn
    W.copy(Y); W.neg(); W.norm()
    Y.cmove(W,ne)

    return NewECP4fp4s(X3,Y);
}

/* Map octet string to curve point */
func ECP4_mapit(h []byte) *ECP4 {
	q := NewBIGints(Modulus)
	dx:=DBIG_fromBytes(h);
    x:=dx.Mod(q);
		
	Q:=ECP4_hap2point(x)
	Q.Cfp()
    return Q
}

/* P=u0.Q0+u1*Q1+u2*Q2+u3*Q3.. */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
// Side channel attack secure
func mul8(Q []*ECP4, u []*BIG) *ECP4 {
	W := NewECP4()
	P := NewECP4()
	var T1 []*ECP4
	var T2 []*ECP4
	mt := NewBIG()
	var t []*BIG
	var bt int8
	var k int

	var w1 [NLEN*int(BASEBITS) + 1]int8
	var s1 [NLEN*int(BASEBITS) + 1]int8
	var w2 [NLEN*int(BASEBITS) + 1]int8
	var s2 [NLEN*int(BASEBITS) + 1]int8

	for i := 0; i < 8; i++ {
		t = append(t, NewBIGcopy(u[i]))
		//Q[i].Affine();
	}

	T1 = append(T1, NewECP4())
	T1[0].Copy(Q[0]) // Q[0]
	T1 = append(T1, NewECP4())
	T1[1].Copy(T1[0])
	T1[1].Add(Q[1]) // Q[0]+Q[1]
	T1 = append(T1, NewECP4())
	T1[2].Copy(T1[0])
	T1[2].Add(Q[2]) // Q[0]+Q[2]
	T1 = append(T1, NewECP4())
	T1[3].Copy(T1[1])
	T1[3].Add(Q[2]) // Q[0]+Q[1]+Q[2]
	T1 = append(T1, NewECP4())
	T1[4].Copy(T1[0])
	T1[4].Add(Q[3]) // Q[0]+Q[3]
	T1 = append(T1, NewECP4())
	T1[5].Copy(T1[1])
	T1[5].Add(Q[3]) // Q[0]+Q[1]+Q[3]
	T1 = append(T1, NewECP4())
	T1[6].Copy(T1[2])
	T1[6].Add(Q[3]) // Q[0]+Q[2]+Q[3]
	T1 = append(T1, NewECP4())
	T1[7].Copy(T1[3])
	T1[7].Add(Q[3]) // Q[0]+Q[1]+Q[2]+Q[3]

	T2 = append(T2, NewECP4())
	T2[0].Copy(Q[4]) // Q[0]
	T2 = append(T2, NewECP4())
	T2[1].Copy(T2[0])
	T2[1].Add(Q[5]) // Q[0]+Q[1]
	T2 = append(T2, NewECP4())
	T2[2].Copy(T2[0])
	T2[2].Add(Q[6]) // Q[0]+Q[2]
	T2 = append(T2, NewECP4())
	T2[3].Copy(T2[1])
	T2[3].Add(Q[6]) // Q[0]+Q[1]+Q[2]
	T2 = append(T2, NewECP4())
	T2[4].Copy(T2[0])
	T2[4].Add(Q[7]) // Q[0]+Q[3]
	T2 = append(T2, NewECP4())
	T2[5].Copy(T2[1])
	T2[5].Add(Q[7]) // Q[0]+Q[1]+Q[3]
	T2 = append(T2, NewECP4())
	T2[6].Copy(T2[2])
	T2[6].Add(Q[7]) // Q[0]+Q[2]+Q[3]
	T2 = append(T2, NewECP4())
	T2[7].Copy(T2[3])
	T2[7].Add(Q[7]) // Q[0]+Q[1]+Q[2]+Q[3]

	// Make them odd
	pb1 := 1 - t[0].parity()
	t[0].inc(pb1)

	pb2 := 1 - t[4].parity()
	t[4].inc(pb2)

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
	P.selector(T1, int32(2*w1[nb-1]+1))
	W.selector(T2, int32(2*w2[nb-1]+1))
	P.Add(W)
	for i := nb - 2; i >= 0; i-- {
		P.dbl()
		W.selector(T1, int32(2*w1[i]+s1[i]))
		P.Add(W)
		W.selector(T2, int32(2*w2[i]+s2[i]))
		P.Add(W)

	}

	// apply correction
	W.Copy(P)
	W.Sub(Q[0])
	P.cmove(W, pb1)
	W.Copy(P)
	W.Sub(Q[4])
	P.cmove(W, pb2)

	P.Affine()
	return P
}
