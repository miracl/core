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
use crate::xxx::ecp4::ECP4;
use crate::xxx::fp::FP;
use crate::xxx::fp2::FP2;
use crate::xxx::fp24::FP24;
use crate::xxx::fp24;
use crate::xxx::fp4::FP4;
use crate::xxx::fp8::FP8;
use crate::xxx::rom;

#[allow(non_snake_case)]
fn dbl(A: &mut ECP4, aa: &mut FP4, bb: &mut FP4, cc: &mut FP4) {
    cc.copy(&A.getpx()); //X
    let mut yy = FP4::new_copy(&A.getpy()); //Y
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
fn add(A: &mut ECP4, B: &ECP4, aa: &mut FP4, bb: &mut FP4, cc: &mut FP4) {

    aa.copy(&A.getpx()); // X1
    cc.copy(&A.getpy()); // Y1
    let mut t1 = FP4::new_copy(&A.getpz()); // Z1
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
fn linedbl(A: &mut ECP4, qx: &FP, qy: &FP) -> FP24 {
    let mut a = FP8::new();
    let mut b = FP8::new();
    let mut c = FP8::new();
    let mut aa = FP4::new(); 
    let mut bb = FP4::new(); 
    let mut cc = FP4::new(); 

    dbl(A,&mut aa,&mut bb,&mut cc);

    cc.qmul(qx);
    aa.qmul(qy);

    a.copy(&FP8::new_fp4s(&aa, &bb)); // -2YZ.Ys | 3b.Z^2-Y^2 | 3X^2.Xs
    if ecp::SEXTIC_TWIST == ecp::D_TYPE {
        b.copy(&FP8::new_fp4(&cc)); // L(0,1) | L(0,0) | L(1,0)
    }
    if ecp::SEXTIC_TWIST == ecp::M_TYPE {
        c.copy(&FP8::new_fp4(&cc));
        c.times_i();
    }
    let mut res= FP24::new_fp8s(&a, &b, &c);
    res.settype(fp24::SPARSER);
    return res;
}

#[allow(non_snake_case)]
fn lineadd(A: &mut ECP4, B: &ECP4, qx: &FP, qy: &FP) -> FP24 {
    let mut a = FP8::new();
    let mut b = FP8::new();
    let mut c = FP8::new();
    let mut aa = FP4::new(); 
    let mut bb = FP4::new(); 
    let mut cc = FP4::new();
    
    add(A,B,&mut aa,&mut bb,&mut cc);

    cc.qmul(qx);
    aa.qmul(qy);

    a.copy(&FP8::new_fp4s(&aa, &bb)); // -2YZ.Ys | 3b.Z^2-Y^2 | 3X^2.Xs
    if ecp::SEXTIC_TWIST == ecp::D_TYPE {
        b.copy(&FP8::new_fp4(&cc)); // L(0,1) | L(0,0) | L(1,0)
    }
    if ecp::SEXTIC_TWIST == ecp::M_TYPE {
        c.copy(&FP8::new_fp4(&cc));
        c.times_i();
    }

    let mut res= FP24::new_fp8s(&a, &b, &c);
    res.settype(fp24::SPARSER);
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
pub fn initmp() -> [FP24; ecp::ATE_BITS] {
    let r: [FP24; ecp::ATE_BITS] = [FP24::new_int(1); ecp::ATE_BITS];
    return r
}

/* basic Miller loop */
pub fn miller(r:&mut [FP24]) -> FP24 {
    let mut res=FP24::new_int(1);
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

fn pack(aa: &FP4,bb: &FP4,cc: &FP4) -> FP8 {
    let mut i=FP4::new_copy(cc);
    i.inverse(None);
    let mut a=FP4::new_copy(aa);
    let mut b=FP4::new_copy(bb);
    a.mul(&i);
    b.mul(&i);
    return FP8::new_fp4s(&a,&b);
}

fn unpack(t: &FP8, qx: &FP, qy: &FP) -> FP24 {
    let b:FP8;
    let mut c:FP8;
    let w=FP4::new_fp(qx);
 
    let mut aa=t.geta();
    let bb=t.getb();
    aa.qmul(qy);
    let a=FP8::new_fp4s(&aa,&bb);

    if ecp::SEXTIC_TWIST==ecp::D_TYPE {
        b=FP8::new_fp4(&w);
	c=FP8::new();
    } else  {
        b=FP8::new();
	c=FP8::new_fp4(&w); c.times_i();
    }
    let mut v=FP24::new_fp8s(&a,&b,&c);
    v.settype(fp24::SPARSEST);
    return v;
}

#[allow(non_snake_case)]
pub fn precomp(T: &mut [FP8],GV: &ECP4) {
    let mut n = BIG::new();
    let mut n3 = BIG::new();
    let nb=lbits(&mut n3,&mut n);
    let mut aa = FP4::new(); 
    let mut bb = FP4::new(); 
    let mut cc = FP4::new(); 

    let mut P=ECP4::new(); P.copy(GV);

    let mut A = ECP4::new();

    A.copy(&P);
    let mut NP = ECP4::new();
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
pub fn another_pc(r:&mut [FP24],T: &[FP8],QV: &ECP) {
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
pub fn another(r:&mut [FP24],P1: &ECP4,Q1: &ECP) {
    let mut n = BIG::new();
    let mut n3 = BIG::new();
   
    if Q1.is_infinity() {
        return;
    }    
// P is needed in affine form for line function, Q for (Qx,Qy) extraction
    let mut P = ECP4::new();
    P.copy(P1);
    P.affine();
    let mut Q = ECP::new();
    Q.copy(Q1);
    Q.affine();

    let qx = FP::new_copy(&Q.getpx());
    let qy = FP::new_copy(&Q.getpy());
    let mut A = ECP4::new();

    A.copy(&P);
    let mut NP = ECP4::new();
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
pub fn ate(P1: &ECP4, Q1: &ECP) -> FP24 {
    let mut n = BIG::new();
    let mut n3 = BIG::new();

    if Q1.is_infinity() {
        return FP24::new_int(1);
    }

    let mut P = ECP4::new();
    P.copy(P1);
    P.affine();
    let mut Q = ECP::new();
    Q.copy(Q1);
    Q.affine();

    let qx = FP::new_copy(&Q.getpx());
    let qy = FP::new_copy(&Q.getpy());

    let mut A = ECP4::new();
    let mut r = FP24::new_int(1);

    A.copy(&P);
    let mut NP = ECP4::new();
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
pub fn ate2(P1: &ECP4, Q1: &ECP, R1: &ECP4, S1: &ECP) -> FP24 {
    let mut n = BIG::new();
    let mut n3 = BIG::new();

    if Q1.is_infinity() {
        return ate(R1,S1);
    }
    if S1.is_infinity() {
        return ate(P1,Q1);
    }

    let mut P = ECP4::new();
    P.copy(P1);
    P.affine();
    let mut Q = ECP::new();
    Q.copy(Q1);
    Q.affine();
    let mut R = ECP4::new();
    R.copy(R1);
    R.affine();
    let mut S = ECP::new();
    S.copy(S1);
    S.affine();

    let qx = FP::new_copy(&Q.getpx());
    let qy = FP::new_copy(&Q.getpy());

    let sx = FP::new_copy(&S.getpx());
    let sy = FP::new_copy(&S.getpy());

    let mut A = ECP4::new();
    let mut B = ECP4::new();
    let mut r = FP24::new_int(1);

    A.copy(&P);
    B.copy(&R);

    let mut NP = ECP4::new();
    NP.copy(&P);
    NP.neg();
    let mut NR = ECP4::new();
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
pub fn fexp(m: &FP24) -> FP24 {
    let f = FP2::new_bigs(&BIG::new_ints(&rom::FRA), &BIG::new_ints(&rom::FRB));
    let x = BIG::new_ints(&rom::CURVE_BNX);
    let mut r = FP24::new_copy(m);

    /* Easy part of final exp */
    let mut lv = FP24::new_copy(&r);
    lv.inverse();
    r.conj();

    r.mul(&lv);
    lv.copy(&r);
    r.frob(&f, 4);
    r.mul(&lv);

    /* Hard part of final exp */

// See https://eprint.iacr.org/2020/875.pdf
        let mut y1 = FP24::new_copy(&r);
        y1.usqr();
        y1.mul(&r); // y1=r^3

        let mut y0 = FP24::new_copy(&r.pow(&x));
        if ecp::SIGN_OF_X == ecp::NEGATIVEX {
            y0.conj();
        }
        let mut t0 = FP24::new_copy(&r); t0.conj();
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

// ^(x^4+p^4-1)
        y0.copy(&r.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        y0.copy(&y0.pow(&x));
        t0.copy(&r);
        t0.frob(&f,4);
        y0.mul(&t0);
        t0.copy(&r); t0.conj();
        r.copy(&y0);
        r.mul(&t0);

        r.mul(&y1);
        r.reduce();

/*    
    // Ghamman & Fouotsa Method

    let mut t7 = FP24::new_copy(&r);
    t7.usqr();
    let mut t1 = t7.pow(&mut x);

    x.fshr(1);
    let mut t2 = t1.pow(&mut x);
    x.fshl(1);

    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t1.conj();
    }
    let mut t3 = FP24::new_copy(&t1);
    t3.conj();
    t2.mul(&t3);
    t2.mul(&r);

    t3.copy(&t2.pow(&mut x));
    let mut t4 = t3.pow(&mut x);
    let mut t5 = t4.pow(&mut x);

    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t3.conj();
        t5.conj();
    }

    t3.frob(&f, 6);
    t4.frob(&f, 5);
    t3.mul(&t4);

    let mut t6 = t5.pow(&mut x);
    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t6.conj();
    }

    t5.frob(&f, 4);
    t3.mul(&t5);

    let mut t0 = FP24::new_copy(&t2);
    t0.conj();
    t6.mul(&t0);

    t5.copy(&t6);
    t5.frob(&f, 3);

    t3.mul(&t5);
    t5.copy(&t6.pow(&mut x));
    t6.copy(&t5.pow(&mut x));

    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t5.conj();
    }

    t0.copy(&t5);
    t0.frob(&f, 2);
    t3.mul(&t0);
    t0.copy(&t6);
    t0.frob(&f, 1);

    t3.mul(&t0);
    t5.copy(&t6.pow(&mut x));

    if ecp::SIGN_OF_X == ecp::NEGATIVEX {
        t5.conj();
    }
    t2.frob(&f, 7);

    t5.mul(&t7);
    t3.mul(&t2);
    t3.mul(&t5);

    r.mul(&t3);

    r.reduce(); */
    return r;
}

#[allow(non_snake_case)]
/* GLV method */
fn glv(e: &BIG) -> [BIG; 2] {
    let mut u: [BIG; 2] = [BIG::new(), BIG::new()];
    let q = BIG::new_ints(&rom::CURVE_ORDER);
    let mut x = BIG::new_ints(&rom::CURVE_BNX);
    let x2 = BIG::smul(&x, &x);
    x.copy(&BIG::smul(&x2, &x2));
    u[0].copy(&e);
    u[0].rmod(&x);
    u[1].copy(&e);
    u[1].div(&x);
    u[1].rsub(&q);

    return u;
}

#[allow(non_snake_case)]
/* Galbraith & Scott Method */
pub fn gs(e: &BIG) -> [BIG; 8] {
    let mut u: [BIG; 8] = [
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
    for i in 0..7 {
        u[i].copy(&w);
        u[i].rmod(&x);
        w.div(&x);
    }
    u[7].copy(&w);
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
pub fn g2mul(P: &ECP4, e: &BIG) -> ECP4 {
    let mut R = ECP4::new();
    if rom::USE_GS_G2 {
        let mut Q: [ECP4; 8] = [
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
        ];
        let q = BIG::new_ints(&rom::CURVE_ORDER);
        let mut u = gs(e);
        let mut T = ECP4::new();

        let f = ECP4::frob_constants();

        let mut t = BIG::new();
        Q[0].copy(&P);
        for i in 1..8 {
            T.copy(&Q[i - 1]);
            Q[i].copy(&T);
            Q[i].frob(&f, 1);
        }
        for i in 0..8 {
            let np = u[i].nbits();
            t.copy(&BIG::modneg(&mut u[i], &q));
            let nn = t.nbits();
            if nn < np {
                u[i].copy(&t);
                Q[i].neg();
            }
            u[i].norm();
        }

        R.copy(&ECP4::mul8(&mut Q, &u));
    } else {
        R.copy(&P.mul(e));
    }
    return R;
}

/* f=f^e */
/* Note that this method requires a lot of RAM! Better to use compressed XTR method, see FP4.java */
pub fn gtpow(d: &FP24, e: &BIG) -> FP24 {
    let mut r = FP24::new();
    if rom::USE_GS_GT {
        let mut g: [FP24; 8] = [
            FP24::new(),
            FP24::new(),
            FP24::new(),
            FP24::new(),
            FP24::new(),
            FP24::new(),
            FP24::new(),
            FP24::new(),
        ];
        let f = FP2::new_bigs(&BIG::new_ints(&rom::FRA), &BIG::new_ints(&rom::FRB));
        let q = BIG::new_ints(&rom::CURVE_ORDER);
        let mut t = BIG::new();
        let mut u = gs(e);
        let mut w = FP24::new();

        g[0].copy(&d);
        for i in 1..8 {
            w.copy(&g[i - 1]);
            g[i].copy(&w);
            g[i].frob(&f, 1);
        }
        for i in 0..8 {
            let np = u[i].nbits();
            t.copy(&BIG::modneg(&mut u[i], &q));
            let nn = t.nbits();
            if nn < np {
                u[i].copy(&t);
                g[i].conj();
            }
            u[i].norm();
        }
        r.copy(&FP24::pow8(&mut g, &u));
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
pub fn g2member(P: &ECP4) -> bool {
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
/* First check that m!=1, conj(m)*m==1, and m.m^{p^8}=m^{p^4} */
pub fn gtmember(m: &FP24) -> bool {
    if m.isunity() {
        return false;
    }
    let mut r = FP24::new_copy(&m);
    r.conj();
    r.mul(&m);
    if !r.isunity() {
        return false;
    }
    let f = FP2::new_bigs(&BIG::new_ints(&rom::FRA), &BIG::new_ints(&rom::FRB));
    r.copy(&m); r.frob(&f,4);
    let mut w = FP24::new_copy(&r); w.frob(&f,4); 
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

