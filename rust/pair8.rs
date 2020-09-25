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

use crate::xxx::big::BIG;
use crate::xxx::ecp;
use crate::xxx::ecp::ECP;
use crate::xxx::ecp8::ECP8;
use crate::xxx::fp::FP;
use crate::xxx::fp16::FP16;
use crate::xxx::fp2::FP2;
use crate::xxx::fp48::FP48;
use crate::xxx::fp48;
use crate::xxx::fp8::FP8;
use crate::xxx::rom;

#[allow(non_snake_case)]
fn dbl(A: &mut ECP8,aa: &mut FP8, bb: &mut FP8, cc: &mut FP8) {
    cc.copy(&A.getpx()); //X
    let mut yy = FP8::new_copy(&A.getpy()); //Y
    bb.copy(&A.getpz()); //Z

    aa.copy(&yy); //Y
    aa.mul(&bb); //YZ
    cc.sqr(); //X^2
    yy.sqr(); //Y^2
    bb.sqr(); //Z^2

    aa.dbl();
    aa.neg(); aa.norm();
    aa.times_i();

    let sb = 3 * rom::CURVE_B_I;
    bb.imul(sb);
    cc.imul(3);
    if ecp::SEXTIC_TWIST == ecp::D_TYPE {
	yy.times_i();
	cc.times_i();
    }
    if ecp::SEXTIC_TWIST == ecp::M_TYPE {
	bb.times_i();
    }
    bb.sub(&yy);
    bb.norm();
    A.dbl();

}

#[allow(non_snake_case)]
fn add(A: &mut ECP8, B: &ECP8,  aa: &mut FP8, bb: &mut FP8, cc: &mut FP8) {
    aa.copy(&A.getpx()); // X1
    cc.copy(&A.getpy()); // Y1
    let mut t1 = FP8::new_copy(&A.getpz()); // Z1
    bb.copy(&A.getpz()); // Z1

    t1.mul(&B.getpy()); // T1=Z1.Y2
    bb.mul(&B.getpx()); // T2=Z1.X2

    aa.sub(&bb);
    aa.norm(); // X1=X1-Z1.X2
    cc.sub(&t1);
    cc.norm(); // Y1=Y1-Z1.Y2

    t1.copy(&aa); // T1=X1-Z1.X2

    if ecp::SEXTIC_TWIST == ecp::M_TYPE {
        aa.times_i();
    }

    t1.mul(&B.getpy()); // T1=(X1-Z1.X2).Y2

    bb.copy(&cc); // T2=Y1-Z1.Y2
    bb.mul(&B.getpx()); // T2=(Y1-Z1.Y2).X2
    bb.sub(&t1);
    bb.norm(); // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    cc.neg();
    cc.norm(); // Y1=-(Y1-Z1.Y2).Xs

    A.add(B);
}

#[allow(non_snake_case)]
fn linedbl(A: &mut ECP8, qx: &FP, qy: &FP) -> FP48 {
    let mut a = FP16::new();
    let mut b = FP16::new();
    let mut c = FP16::new();
    let mut aa = FP8::new(); 
    let mut bb = FP8::new(); 
    let mut cc = FP8::new(); 

    dbl(A,&mut aa,&mut bb,&mut cc);

    cc.tmul(qx);
    aa.tmul(qy);

    a.copy(&FP16::new_fp8s(&aa, &bb)); // -2YZ.Ys | 3b.Z^2-Y^2 | 3X^2.Xs
    if ecp::SEXTIC_TWIST == ecp::D_TYPE {
        b.copy(&FP16::new_fp8(&cc)); // L(0,1) | L(0,0) | L(1,0)
    }
    if ecp::SEXTIC_TWIST == ecp::M_TYPE {
        c.copy(&FP16::new_fp8(&cc));
        c.times_i();
    }
  
    let mut res= FP48::new_fp16s(&a, &b, &c);
    res.settype(fp48::SPARSER);
    return res;
}

#[allow(non_snake_case)]
fn lineadd(A: &mut ECP8, B: &ECP8, qx: &FP, qy: &FP) -> FP48 {
    let mut a = FP16::new();
    let mut b = FP16::new();
    let mut c = FP16::new();
    let mut aa = FP8::new(); 
    let mut bb = FP8::new(); 
    let mut cc = FP8::new(); 

    add(A,B,&mut aa,&mut bb,&mut cc);

    cc.tmul(qx);
    aa.tmul(qy);

    a.copy(&FP16::new_fp8s(&aa, &bb)); // -2YZ.Ys | 3b.Z^2-Y^2 | 3X^2.Xs
    if ecp::SEXTIC_TWIST == ecp::D_TYPE {
        b.copy(&FP16::new_fp8(&cc)); // L(0,1) | L(0,0) | L(1,0)
    }
    if ecp::SEXTIC_TWIST == ecp::M_TYPE {
        c.copy(&FP16::new_fp8(&cc));
        c.times_i();
    }
  
    let mut res= FP48::new_fp16s(&a, &b, &c);
    res.settype(fp48::SPARSER);
    return res;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
#[allow(non_snake_case)]
fn lbits(n3: &mut BIG,n: &mut BIG) -> usize {
    n.copy(&BIG::new_ints(&rom::CURVE_BNX));
    n3.copy(&n);
    n3.pmul(3);
    n3.norm();
    return n3.nbits();
}

/* prepare for multi-pairing */
pub fn initmp() -> [FP48; ecp::ATE_BITS] {
    let r: [FP48; ecp::ATE_BITS] = [FP48::new_int(1); ecp::ATE_BITS];
    return r
}

/* basic Miller loop */
pub fn miller(r:&mut [FP48]) -> FP48 {
    let mut res=FP48::new_int(1);
    for i in (1..ecp::ATE_BITS).rev() {
        res.sqr();
        res.ssmul(&r[i]);
	r[i].zero();
    }

    if ecp::SIGN_OF_X==ecp::NEGATIVEX {
        res.conj();
    }
    res.ssmul(&r[0]);
    r[0].zero();
    return res;
}

fn pack(aa: &FP8,bb: &FP8,cc: &FP8) -> FP16 {
    let mut i=FP8::new_copy(cc);
    i.inverse(None);
    let mut a=FP8::new_copy(aa);
    let mut b=FP8::new_copy(bb);
    a.mul(&i);
    b.mul(&i);
    return FP16::new_fp8s(&a,&b);
}

fn unpack(t: &FP16, qx: &FP, qy: &FP) -> FP48 {
    let b:FP16;
    let mut c:FP16;
    let w=FP8::new_fp(qx);

    let mut aa=t.geta();
    let bb=t.getb();
    aa.tmul(qy);
    let a=FP16::new_fp8s(&aa,&bb);
   
    if ecp::SEXTIC_TWIST==ecp::D_TYPE {
        b=FP16::new_fp8(&w);
	c=FP16::new();
    } else  {
        b=FP16::new();
	c=FP16::new_fp8(&w); c.times_i();
    }
    let mut v=FP48::new_fp16s(&a,&b,&c);
    v.settype(fp48::SPARSEST);
    return v;
}

#[allow(non_snake_case)]
pub fn precomp(T: &mut [FP16],GV: &ECP8) {
    let mut n = BIG::new();
    let mut n3 = BIG::new();
    let nb=lbits(&mut n3,&mut n);
    let mut aa = FP8::new(); 
    let mut bb = FP8::new(); 
    let mut cc = FP8::new(); 

    let mut P=ECP8::new(); P.copy(GV);

    let mut A = ECP8::new();

    A.copy(&P);
    let mut NP = ECP8::new();
    NP.copy(&P);
    NP.neg();
    let mut j=0;

    for i in (1..nb-1).rev() {
        dbl(&mut A,&mut aa,&mut bb,&mut cc);
	T[j].copy(&pack(&aa,&bb,&cc)); j+=1;
	let bt=n3.bit(i)-n.bit(i);
	if bt==1 {
	    add(&mut A,&P,&mut aa,&mut bb,&mut cc);
	    T[j].copy(&pack(&aa,&bb,&cc)); j+=1;
	}
	if bt==-1 {
	    add(&mut A,&NP,&mut aa,&mut bb,&mut cc);
	    T[j].copy(&pack(&aa,&bb,&cc)); j+=1;
	}
    }

}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
#[allow(non_snake_case)]
pub fn another_pc(r:&mut [FP48],T: &[FP16],QV: &ECP) {
    let mut n = BIG::new();
    let mut n3 = BIG::new();

    if QV.is_infinity() {
        return;
    }

    let nb=lbits(&mut n3,&mut n);

    let mut Q = ECP::new();
    Q.copy(QV);
    Q.affine();
    let qx = FP::new_copy(&Q.getpx());
    let qy = FP::new_copy(&Q.getpy());

    let mut j=0;
    for i in (1..nb-1).rev() {
        let mut lv=unpack(&T[j],&qx,&qy); j+=1;
	let bt=n3.bit(i)-n.bit(i);
	if bt==1 {
	    let lv2=unpack(&T[j],&qx,&qy); j+=1;
	    lv.smul(&lv2);
	}
	if bt==-1 {
	    let lv2=unpack(&T[j],&qx,&qy); j+=1;
	    lv.smul(&lv2);
	}
        r[i].ssmul(&lv);
    }
}

/* Accumulate another set of line functions for n-pairing */
#[allow(non_snake_case)]
pub fn another(r:&mut [FP48],P1: &ECP8,Q1: &ECP) {
    let mut n = BIG::new();
    let mut n3 = BIG::new();

    if Q1.is_infinity() {
        return;
    }

// P is needed in affine form for line function, Q for (Qx,Qy) extraction
    let mut P = ECP8::new();
    P.copy(P1);
    P.affine();
    let mut Q = ECP::new();
    Q.copy(Q1);
    Q.affine();

    let qx = FP::new_copy(&Q.getpx());
    let qy = FP::new_copy(&Q.getpy());
    let mut A = ECP8::new();

    A.copy(&P);
    let mut NP = ECP8::new();
    NP.copy(&P);
    NP.neg();

    let nb=lbits(&mut n3,&mut n);

    for i in (1..nb-1).rev() {
        let mut lv=linedbl(&mut A,&qx,&qy);

	let bt=n3.bit(i)-n.bit(i);
        if bt==1 {
            let lv2=lineadd(&mut A,&P,&qx,&qy);
            lv.smul(&lv2);
        }
        if bt==-1 {
            let lv2=lineadd(&mut A,&NP,&qx,&qy);
            lv.smul(&lv2);
        }
        r[i].ssmul(&lv);
    }
}

#[allow(non_snake_case)]
/* Optimal R-ate pairing */
pub fn ate(P1: &ECP8, Q1: &ECP) -> FP48 {
    let mut n = BIG::new();
    let mut n3 = BIG::new();

    if Q1.is_infinity() {
        return FP48::new_int(1);
    }

    let mut P = ECP8::new();
    P.copy(P1);
    P.affine();
    let mut Q = ECP::new();
    Q.copy(Q1);
    Q.affine();

    let qx = FP::new_copy(&Q.getpx());
    let qy = FP::new_copy(&Q.getpy());

    let mut A = ECP8::new();
    let mut r = FP48::new_int(1);

    A.copy(&P);
    let mut NP = ECP8::new();
    NP.copy(&P);
    NP.neg();

    let nb=lbits(&mut n3,&mut n);

    for i in (1..nb - 1).rev() {
        r.sqr();
        let mut lv = linedbl(&mut A, &qx, &qy);
        let bt = n3.bit(i) - n.bit(i);
        if bt == 1 {
            let lv2 = lineadd(&mut A, &P, &qx, &qy);
            lv.smul(&lv2);
        }
        if bt == -1 {
            let lv2 = lineadd(&mut A, &NP, &qx, &qy);
            lv.smul(&lv2);
        }
        r.ssmul(&lv);
    }

    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        r.conj();
    }

    return r;
}

#[allow(non_snake_case)]
/* Optimal R-ate double pairing e(P,Q).e(R,S) */
pub fn ate2(P1: &ECP8, Q1: &ECP, R1: &ECP8, S1: &ECP) -> FP48 {
    let mut n = BIG::new();
    let mut n3 = BIG::new();

    if Q1.is_infinity() {
        return ate(R1,S1);
    }
    if S1.is_infinity() {
        return ate(P1,Q1);
    }

    let mut P = ECP8::new();
    P.copy(P1);
    P.affine();
    let mut Q = ECP::new();
    Q.copy(Q1);
    Q.affine();
    let mut R = ECP8::new();
    R.copy(R1);
    R.affine();
    let mut S = ECP::new();
    S.copy(S1);
    S.affine();

    let qx = FP::new_copy(&Q.getpx());
    let qy = FP::new_copy(&Q.getpy());

    let sx = FP::new_copy(&S.getpx());
    let sy = FP::new_copy(&S.getpy());

    let mut A = ECP8::new();
    let mut B = ECP8::new();
    let mut r = FP48::new_int(1);

    A.copy(&P);
    B.copy(&R);

    let mut NP = ECP8::new();
    NP.copy(&P);
    NP.neg();
    let mut NR = ECP8::new();
    NR.copy(&R);
    NR.neg();

     let nb=lbits(&mut n3,&mut n);

    for i in (1..nb - 1).rev() {
        r.sqr();
        let mut lv = linedbl(&mut A, &qx, &qy);
        let lv2 = linedbl(&mut B, &sx, &sy);
	lv.smul(&lv2);
        r.ssmul(&lv);
        let bt = n3.bit(i) - n.bit(i);
        if bt == 1 {
            lv = lineadd(&mut A, &P, &qx, &qy);
            let lv2 = lineadd(&mut B, &R, &sx, &sy);
	    lv.smul(&lv2);
            r.ssmul(&lv);
        }
        if bt == -1 {
            lv = lineadd(&mut A, &NP, &qx, &qy);
            let lv2 = lineadd(&mut B, &NR, &sx, &sy);
	    lv.smul(&lv2);
            r.ssmul(&lv);
        }
    }

    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        r.conj();
    }

    return r;
}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
pub fn fexp(m: &FP48) -> FP48 {
    let f = FP2::new_bigs(&BIG::new_ints(&rom::FRA), &BIG::new_ints(&rom::FRB));
    let x = BIG::new_ints(&rom::CURVE_BNX);
    let mut r = FP48::new_copy(m);
    //let mut t1:FP48;
    //let mut t2:FP48;

    /* Easy part of final exp */
    let mut lv = FP48::new_copy(&r);
    lv.inverse();
    r.conj();

    r.mul(&lv);
    lv.copy(&r);
    r.frob(&f, 8);
    r.mul(&lv);

    /* Hard part of final exp */

// See https://eprint.iacr.org/2020/875.pdf
        let mut y1 = FP48::new_copy(&r);
        y1.usqr();
        y1.mul(&r); // y1=r^3

        let mut y0 = FP48::new_copy(&r.pow(&x));
        if ecp::SIGN_OF_X == ecp::NEGATIVEX {
            y0.conj();
        }
        let mut t0 = FP48::new_copy(&r); t0.conj();
        r.copy(&y0);
        r.mul(&t0);

        y0.copy(&r.pow(&x));
        if ecp::SIGN_OF_X == ecp::NEGATIVEX {
            y0.conj();
        }
        t0.copy(&r); t0.conj();
        r.copy(&y0);
        r.mul(&t0);

// ^(x+p)
        y0.copy(&r.pow(&x));
        if ecp::SIGN_OF_X == ecp::NEGATIVEX {
            y0.conj();
        }
        t0.copy(&r);
        t0.frob(&f,1);
        r.copy(&y0);
        r.mul(&t0);

// ^(x^2+p^2)
        y0.copy(&r.pow(&x));
        y0.copy(&y0.pow(&x));
        t0.copy(&r);
        t0.frob(&f,2); 
        r.copy(&y0);
        r.mul(&t0);

// ^(x^4+p^4)
        y0.copy(&r.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        t0.copy(&r);
        t0.frob(&f,4);
        r.copy(&y0);
        r.mul(&t0);

// ^(x^+p^8-1)
        y0.copy(&r.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        t0.copy(&r);
        t0.frob(&f,8);
        y0.mul(&t0);
        t0.copy(&r); t0.conj();
        r.copy(&y0);
        r.mul(&t0);

        r.mul(&y1);
        r.reduce();

/*
    // Ghamman & Fouotsa Method

    let mut t7 = FP48::new_copy(&r);
    t7.usqr();

    if x.parity()==1 {
        t2 = r.pow(&mut x);
        t1 = FP48::new_copy(&t2); t1.usqr();
        t2 = t2.pow(&mut x);
    } else {
        t1 = t7.pow(&mut x);
        x.fshr(1);
        t2 = t1.pow(&mut x);
        x.fshl(1);
    }
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    let mut t3 = FP48::new_copy(&t1);
    t3.conj();
    t2.mul(&t3);
    t2.mul(&r);

    r.mul(&t7);

    t1.copy(&t2.pow(&mut x));

    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }
    t3.copy(&t1);
    t3.frob(&f, 14);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 13);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 12);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 11);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 10);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 9);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 8);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t2);
    t3.conj();
    t1.mul(&t3);
    t3.copy(&t1);
    t3.frob(&f, 7);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 6);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 5);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 4);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 3);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 2);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    t3.copy(&t1);
    t3.frob(&f, 1);
    r.mul(&t3);
    lv.copy(&t1.pow(&mut x));
    t1.copy(&lv);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }

    r.mul(&t1);
    t2.frob(&f, 15);
    r.mul(&t2);

    r.reduce(); */
    return r;
}

#[allow(non_snake_case)]
/* GLV method */
fn glv(e: &BIG) -> [BIG; 2] {
    let mut u: [BIG; 2] = [BIG::new(), BIG::new()];
    let q = BIG::new_ints(&rom::CURVE_ORDER);
    let mut x = BIG::new_ints(&rom::CURVE_BNX);
    let mut x2 = BIG::smul(&x, &x);
    x.copy(&BIG::smul(&x2, &x2));
    x2.copy(&BIG::smul(&x, &x));
    u[0].copy(&e);
    u[0].rmod(&x2);
    u[1].copy(&e);
    u[1].div(&x2);
    u[1].rsub(&q);

    return u;
}

#[allow(non_snake_case)]
/* Galbraith & Scott Method */
pub fn gs(e: &BIG) -> [BIG; 16] {
    let mut u: [BIG; 16] = [
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
        BIG::new(),
    ];
    let q = BIG::new_ints(&rom::CURVE_ORDER);
    let x = BIG::new_ints(&rom::CURVE_BNX);
    let mut w = BIG::new_copy(&e);
    for i in 0..15 {
        u[i].copy(&w);
        u[i].rmod(&x);
        w.div(&x);
    }
    u[15].copy(&w);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        let mut t = BIG::new();
        t.copy(&BIG::modneg(&mut u[1], &q));
        u[1].copy(&t);
        t.copy(&BIG::modneg(&mut u[3], &q));
        u[3].copy(&t);
        t.copy(&BIG::modneg(&mut u[5], &q));
        u[5].copy(&t);
        t.copy(&BIG::modneg(&mut u[7], &q));
        u[7].copy(&t);
        t.copy(&BIG::modneg(&mut u[9], &q));
        u[9].copy(&t);
        t.copy(&BIG::modneg(&mut u[11], &q));
        u[11].copy(&t);
        t.copy(&BIG::modneg(&mut u[13], &q));
        u[13].copy(&t);
        t.copy(&BIG::modneg(&mut u[15], &q));
        u[15].copy(&t);
    }
    return u;
}

#[allow(non_snake_case)]
/* Multiply P by e in group G1 */
pub fn g1mul(P: &ECP, e: &BIG) -> ECP {
    let mut R = ECP::new();
    if rom::USE_GLV {
        R.copy(P);
        let mut Q = ECP::new();
        Q.copy(P);
        Q.affine();
        let q = BIG::new_ints(&rom::CURVE_ORDER);
        let mut cru = FP::new_big(&BIG::new_ints(&rom::CRU));
        let mut u = glv(e);
        Q.mulx(&mut cru);

        let mut np = u[0].nbits();
        let mut t: BIG = BIG::modneg(&mut u[0], &q);
        let mut nn = t.nbits();
        if nn < np {
            u[0].copy(&t);
            R.neg();
        }

        np = u[1].nbits();
        t = BIG::modneg(&mut u[1], &q);
        nn = t.nbits();
        if nn < np {
            u[1].copy(&t);
            Q.neg();
        }
        u[0].norm();
        u[1].norm();
        R = R.mul2(&u[0], &mut Q, &u[1]);
    } else {
        R = P.mul(e);
    }
    return R;
}

#[allow(non_snake_case)]
/* Multiply P by e in group G2 */
pub fn g2mul(P: &ECP8, e: &BIG) -> ECP8 {
    let mut R = ECP8::new();
    if rom::USE_GS_G2 {
        let mut Q: [ECP8; 16] = [
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
            ECP8::new(),
        ];
        let q = BIG::new_ints(&rom::CURVE_ORDER);
        let mut u = gs(e);
        let mut T = ECP8::new();

        let f = ECP8::frob_constants();

        let mut t = BIG::new();
        Q[0].copy(&P);
        for i in 1..16 {
            T.copy(&Q[i - 1]);
            Q[i].copy(&T);
            Q[i].frob(&f, 1);
        }
	//println!("");
        for i in 0..16 {
            let np = u[i].nbits();
            t.copy(&BIG::modneg(&mut u[i], &q));
            let nn = t.nbits();
            if nn < np {
                u[i].copy(&t);
                Q[i].neg();
            }
            u[i].norm();
	    //println!("Q= {}",Q[i].tostring());
	    //println!("u= {}",u[i].tostring());
        }

        R.copy(&ECP8::mul16(&mut Q, &u));
    } else {
        R.copy(&P.mul(e));
    }
    return R;
}

/* f=f^e */
/* Note that this method requires a lot of RAM! Better to use compressed XTR method, see FP4.java */
pub fn gtpow(d: &FP48, e: &BIG) -> FP48 {
    let mut r = FP48::new();
    if rom::USE_GS_GT {
        let mut g: [FP48; 16] = [
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
            FP48::new(),
        ];
        let f = FP2::new_bigs(&BIG::new_ints(&rom::FRA), &BIG::new_ints(&rom::FRB));
        let q = BIG::new_ints(&rom::CURVE_ORDER);
        let mut t = BIG::new();
        let mut u = gs(e);
        let mut w = FP48::new();

        g[0].copy(&d);
        for i in 1..16 {
            w.copy(&g[i - 1]);
            g[i].copy(&w);
            g[i].frob(&f, 1);
        }
        for i in 0..16 {
            let np = u[i].nbits();
            t.copy(&BIG::modneg(&mut u[i], &q));
            let nn = t.nbits();
            if nn < np {
                u[i].copy(&t);
                g[i].conj();
            }
            u[i].norm();
        }
        r.copy(&FP48::pow16(&mut g, &u));
    } else {
        r.copy(&d.pow(e));
    }
    return r;
}

/* test G1 group membership */
#[allow(non_snake_case)]
pub fn g1member(P: &ECP) -> bool {
    let q = BIG::new_ints(&rom::CURVE_ORDER);
    if P.is_infinity() {
        return false;
    }
    let W=g1mul(&P,&q);
    if !W.is_infinity() {
        return false;
    }
    return true;
}

/* test G2 group membership */
#[allow(non_snake_case)]
pub fn g2member(P: &ECP8) -> bool {
    let q = BIG::new_ints(&rom::CURVE_ORDER);
    if P.is_infinity() {
        return false;
    }
    let W=g2mul(&P,&q);
    if !W.is_infinity() {
        return false;
    }
    return true;
}

/* test GT group membership */
/* First check that m!=1, conj(m)*m==1, and m.m^{p^16}=m^{p^8} */
pub fn gtmember(m: &FP48) -> bool {
    if m.isunity() {
        return false;
    }
    let mut r = FP48::new_copy(&m);
    r.conj();
    r.mul(&m);
    if !r.isunity() {
        return false;
    }
    let f = FP2::new_bigs(&BIG::new_ints(&rom::FRA), &BIG::new_ints(&rom::FRB));
    r.copy(&m); r.frob(&f,8); 
    let mut w = FP48::new_copy(&r); w.frob(&f,8); 
    w.mul(&m);
    if !w.equals(&r) {
        return false;
    }
    let q = BIG::new_ints(&rom::CURVE_ORDER);
    w.copy(&m);
    r.copy(&gtpow(&w,&q));
    if !r.isunity() {
        return false;
    }
    return true;
}

