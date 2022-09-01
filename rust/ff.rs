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

use crate::arch::Chunk;
use crate::rand::RAND;
use crate::xxx::big;
use crate::xxx::big::BIG;
use crate::xxx::dbig::DBIG;

use crate::arch::DChunk;

/* Finite field support - for RSA, DH etc. */
/* RSA/DH modulus length as multiple of BIGBITS */

pub const FFLEN:usize=@ML@;

pub const P_MBITS: usize = (big::MODBYTES as usize) * 8;
pub const P_OMASK: Chunk = (-1) << (P_MBITS % big::BASEBITS);
pub const P_FEXCESS: Chunk = 1 << (big::BASEBITS * big::NLEN - P_MBITS - 1);
pub const P_TBITS: usize = P_MBITS % big::BASEBITS;

pub const SL:usize=FFLEN/2;       // single length
pub const DL:usize=FFLEN;         // double length
pub const QL:usize=2*FFLEN;       // quadruple length

pub struct SF {
    v: [BIG; SL],
}
pub struct DF {
    v: [BIG; DL],  
}
pub struct QF {
    v: [BIG; QL],  
}

#[cfg(feature = "std")]
impl std::fmt::Debug for SF {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(formatter, "{}", self.tostring())
    }
}

#[cfg(feature = "std")]
impl std::fmt::Display for SF {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(formatter, "{}", self.tostring())
    }
}

// wrt RSA primes p and q
impl SF {
// Constructor 
    pub fn new() -> SF {
        SF {
            v: [BIG::new();SL],
        }
    }

    pub fn zero(&mut self) {                // SL and DL
        for i in 0..SL {
            self.v[i].zero();
        }
    }

    /* set to integer */
    pub fn set(&mut self, m: isize) {      // SL
        self.zero();
        self.v[0].set(0, m as Chunk);
    }

    // simple copy
    pub fn copy(&mut self, b: &SF) {
        generic_copy(&mut self.v,&b.v);
    }

    pub fn one(&mut self) {
        self.v[0].one();
        for i in 1..SL {
            self.v[i].zero();
        }
    }

    /* test equals 0 */
    pub fn iszilch(&self) -> bool {
        return generic_iszilch(&self.v); 
    }

    /* shift right by BIGBITS-bit words */
    pub fn shrw(&mut self, n: usize) {
        generic_shrw(&mut self.v,n);
    }

    /* shift left by BIGBITS-bit words */
    pub fn shlw(&mut self, n: usize) {
        generic_shlw(&mut self.v,n);
    }

    /* extract last bit */
    pub fn parity(&self) -> isize {
        self.v[0].parity()
    }

    pub fn lastbits(&mut self, m: usize) -> isize {
        self.v[0].lastbits(m)
    }

    /* simple add */
    pub fn add(&mut self, b: &SF) {
        generic_add(&mut self.v,&b.v);
    }

    /* simple sub */
    pub fn sub(&mut self, b: &SF) {
        generic_sub(&mut self.v,&b.v);
    }

    /* reverse sub */
    pub fn revsub(&mut self, b: &SF) {
        for i in 0..SL {
            self.v[i].rsub(&b.v[i]);
        }
    }

    pub fn norm(&mut self) {
        generic_norm(&mut self.v);
//        rnorm(&mut self.v,false);
    }

    /* increment/decrement by a small integer */
    pub fn inc(&mut self, m: isize) {
        self.v[0].inc(m);
        self.norm();
    }

    pub fn dec(&mut self, m: isize) {
        self.v[0].dec(m);
        self.norm();
    }

    /* shift left by one bit */
    pub fn shl(&mut self) {
        generic_shl(&mut self.v);
    }

    /* shift right by one bit */

    pub fn shr(&mut self) {
        generic_shr(&mut self.v);
    }

    /* Convert to Hex String */
    #[cfg(feature = "std")]
    pub fn tostring(&self) -> String {
        let mut cs:[BIG;SL]=[BIG::new();SL];
        return generic_tostring(&self.v,&mut cs,SL);
    }
     
    /* Convert SFs to/from byte arrays */
    pub fn tobytes(&mut self, b: &mut [u8]) {
        for i in 0..SL {
            self.v[i].tobytearray(b, (SL - i - 1) * (big::MODBYTES as usize))
        }
    }

    pub fn mod2m(&mut self, m: usize) {
        for i in m..SL {
            self.v[i].zero()
        }
    }

    pub fn random(&mut self, rng: &mut RAND) {              // SL only
        for i in 0..SL {
            self.v[i].copy(&BIG::random(rng))
        }
        /* make sure top bit is 1 */
        while self.v[SL - 1].nbits() < (big::MODBYTES as usize) * 8 {
            self.v[SL - 1].copy(&BIG::random(rng));
        }
    }

    /* compare x and y - must be normalised, and of same length */
    pub fn comp(&self, b: &SF) -> isize {  
        return generic_comp(&self.v,&b.v);
    }

    pub fn frombytes(&mut self, b: &[u8]) {                       // SL and DL only
        for i in 0..SL {
            self.v[i] = BIG::frombytearray(b, (SL - i - 1) * (big::MODBYTES as usize))
        }
    }

    /* in-place swapping using xor - side channel resistant - lengths must be the same */
    pub fn cswap(&mut self, b: &mut SF, d: isize) {              // SL only
        for i in 0..SL {
            self.v[i].cswap(&mut b.v[i], d);
        }
    }


    /* Set self=self mod m */
    pub fn rmod(&mut self, m: &SF) {
        let mut c:[BIG;SL]=[BIG::new();SL];       
        generic_rmod(&mut self.v,&m.v,&mut c);
    }

    /* Set return=1/this mod p. Binary method - a<p on entry */
    pub fn invmodp(&mut self, p: &SF) {                      // SL only
        let mut u = SF::new();
        let mut v = SF::new();
        let mut x1 = SF::new();
        let mut x2 = SF::new();
        let mut t = SF::new();
        let mut one = SF::new();

        one.one();
        u.copy(&self);
        v.copy(&p);
        x1.copy(&one);
        x2.zero();

        // reduce n in here as well!
        while u.comp(&one) != 0 && v.comp(&one) != 0 {
            while u.parity() == 0 {
                u.shr();
                if x1.parity() != 0 {
                    x1.add(&p);
                    x1.norm();
                }
                x1.shr();
            }
            while v.parity() == 0 {
                v.shr();
                if x2.parity() != 0 {
                    x2.add(&p);
                    x2.norm();
                }
                x2.shr();
            }
            if u.comp(&v) >= 0 {
                u.sub(&v);
                u.norm();
                if x1.comp(&x2) >= 0 {
                    x1.sub(&x2);
                } else {
                    t.copy(&p);
                    t.sub(&x2);
                    x1.add(&t);
                }
                x1.norm();
            } else {
                v.sub(&u);
                v.norm();
                if x2.comp(&x1) >= 0 {
                    x2.sub(&x1);
                } else {
                    t.copy(&p);
                    t.sub(&x1);
                    x2.add(&t);
                }
                x2.norm();
            }
        }
        if u.comp(&one) == 0 {
            self.copy(&x1);
        } else {
            self.copy(&x2);
        }
    }

    /* return low part of product this*y */
    pub fn lmul(&mut self, y: &SF) {
        let mut t:[BIG;2*SL]=[BIG::new();2*SL];
        let mut x = SF::new();
        x.copy(&self);
        karmul_lower(&mut self.v,&x.v,&y.v,&mut t[0..2*SL]);
    }

    /* U=1/a mod 2^m - Arazi & Qi */
    pub fn invmod2m(&self) -> SF {          // SL and DL       (?)            
        let mut b = SF::new();
        let mut c = SF::new();
        let mut u = SF::new();
        let mut t = SF::new();

        u.zero();
        u.v[0].copy(&self.v[0]);
        u.v[0].invmod2m();

        let mut i = 1;
        while i < SL {
            b.copy(&self);
            b.mod2m(i);
            t.copy(&u); t.lmul(&b);
            t.shrw(i);
            b.copy(&t);
            c.copy(&self);
            c.shrw(i);
            c.mod2m(i);
            c.lmul(&u);
            c.mod2m(i);

            b.add(&c);
            b.norm();
            b.lmul(&u);
            b.mod2m(i);

            c.one();
            c.shlw(i);
            b.revsub(&c);
            b.norm();
            b.shlw(i);
            u.add(&b);
            i <<= 1;
        }
        u.norm();
        u
    }


/*****************************************/

    /* x=y */
    pub fn sdcopy(&mut self, b: &DF) {
        generic_sdcopy(&mut self.v,&b.v);
    }

    /* x=y>>n */
    pub fn sducopy(&mut self, b: &DF) {
        generic_sducopy(&mut self.v,&b.v);
    }

    /* nresidue mod m */
    pub fn nres(&mut self, m: &SF) {
        if SL == 1 {
            let mut d = DBIG::new_scopy(&(self.v[0]));
            d.shl(big::NLEN * (big::BASEBITS as usize));
            self.v[0].copy(&d.dmod(&(m.v[0])));
        } else {
            let mut d = DF::new();
            d.dsucopy(&self);
            self.copy(&d.dmod(m));
        }
    }

    pub fn redc(&mut self, m: &SF, md: &SF) {
        if SL == 1 {
            let mut d = DBIG::new_scopy(&(self.v[0]));
            self.v[0].copy(&BIG::monty(&(m.v[0]),((1 as Chunk) << big::BASEBITS) - md.v[0].w[0],&mut d));
        } else {
            let mut d = DF::new();
            self.rmod(m);
            d.dscopy(&self);
            self.copy(&d.reduce(&m, &md));
            self.rmod(m);
        }
    }

    /* generate random x less than p */
    pub fn randomnum(&mut self, p: &SF, rng: &mut RAND) {    // SL only
        let mut d = DF::new();
        for i in 0..DL {
            d.v[i].copy(&BIG::random(rng));
        }
        self.copy(&d.dmod(p));
    }

    /* this*=y mod p */
    pub fn modmul(&mut self, y: &SF, p: &SF, nd: &SF) {      // SL and DL
        if pexceed(&self.v[SL - 1], &y.v[SL - 1]) {
            self.rmod(p)
        }
        if SL == 1 {
            let mut d = BIG::mul(&self.v[0], &y.v[0]);
            self.v[0].copy(&BIG::monty(&(p.v[0]),((1 as Chunk) << big::BASEBITS) - nd.v[0].w[0],&mut d));
        } else {
            let mut d = self.mul(y);
            self.copy(&d.reduce(p, nd));
        }
    }

    /* this*=y mod p */
    pub fn modsqr(&mut self, p: &SF, nd: &SF) {                 // SL and DL
        if sexceed(&self.v[SL - 1]) {
            self.rmod(p);
        }
        if SL == 1 {
            let mut d = BIG::sqr(&self.v[0]);
            self.v[0].copy(&BIG::monty(&(p.v[0]),((1 as Chunk) << big::BASEBITS) - nd.v[0].w[0],&mut d));
        } else {
            let mut d = self.sqr();
            d.norm();
            self.copy(&d.reduce(p, nd));
        }
    }

    /* this=this^e mod p using side-channel resistant Montgomery Ladder, for large e */
    pub fn skpow(&mut self, e: &SF, p: &SF) {                  // SL only
        let mut r0 = SF::new();
        let mut r1 = SF::new();
        let nd = p.invmod2m();

        self.rmod(p);
        r0.one();
        r1.copy(&self);
        r0.nres(p);
        r1.nres(p);

        let mut i = 8 * (big::MODBYTES as usize) * SL - 1;
        loop {
            let b = (e.v[i / (big::BIGBITS as usize)]).bit(i % (big::BIGBITS as usize)) as isize;
            self.copy(&r0);
            self.modmul(&r1, p, &nd);

            r0.cswap(&mut r1, b);
            r0.modsqr(p, &nd);

            r1.copy(&self);
            r0.cswap(&mut r1, b);
            if i == 0 {
                break;
            }
            i -= 1;
        }
        self.copy(&r0);
        self.redc(p, &nd);
    }

    /* this=this^e mod p, faster but not side channel resistant */
    pub fn pow(&mut self, e: &SF, p: &SF) {                      // SL only
        let mut w = SF::new();
        let nd = p.invmod2m();

        w.copy(&self);
        self.one();
        self.nres(p);
        w.nres(p);
        let mut i = 8 * (big::MODBYTES as usize) * SL - 1;
        loop {
            self.modsqr(p, &nd);
            let b = (e.v[i / (big::BIGBITS as usize)]).bit(i % (big::BIGBITS as usize)) as isize;
            if b == 1 {
                self.modmul(&w, p, &nd)
            }
            if i == 0 {
                break;
            }
            i -= 1;
        }
        self.redc(p, &nd);
    }

    /* quick and dirty check for common factor with n */
    pub fn cfactor(&self, s: isize) -> bool {              // SL only
        let mut x = SF::new();
        let mut y = SF::new();

        y.set(s);
        x.copy(&self);
        x.norm();

        x.sub(&y);
        x.norm();

        while !x.iszilch() && x.parity() == 0 {
            x.shr()
        }

        while x.comp(&y) > 0 {
            x.sub(&y);
            x.norm();
            while !x.iszilch() && x.parity() == 0 {
                x.shr()
            }
        }

        let g = x.v[0].get(0) as isize;
        let r = igcd(s, g);
        r > 1
    }


    /* Miller-Rabin test for primality. Slow. */
    pub fn isprime(&self, rng: &mut RAND) -> bool {            // SL only
        let mut s = 0;
        let mut d = SF::new();
        let mut x = SF::new();
        let mut unity = SF::new();
        let mut nm1 = SF::new();
        let mut p = SF::new();
        p.copy(self);
        let nd = p.invmod2m();

        let sf = 4849845; /* 3*5*.. *19 */
        p.norm();

        if p.cfactor(sf) {
            return false;
        }
        unity.one();
        nm1.copy(&p);
        nm1.sub(&unity);
        nm1.norm();
        d.copy(&nm1);

        while d.parity() == 0 {
            d.shr();
            s += 1;
        }
        if s == 0 {
            return false;
        }
        for _ in 0..10 {
            x.randomnum(&p, rng);
            x.pow(&d, &p);
            if x.comp(&unity) == 0 || x.comp(&nm1) == 0 {
                continue;
            }
            let mut looper = false;
            for _ in 1..s {
                x.modsqr(&p,&nd);
                if x.comp(&unity) == 0 {
                    return false;
                }
                if x.comp(&nm1) == 0 {
                    looper = true;
                    break;
                }
            }
            if looper {
                continue;
            }
            return false;
        }
        true
    }

    /* z=x*y. Assumes x and y are of same length. */
    pub fn mul(&self, y: &SF) -> DF {                    // SL and DL
        let mut z = DF::new();
        let mut t = DF::new();
        karmul(&mut z.v,&self.v,&y.v,&mut t.v[0..DL]);
        z
    }

    /* z=x^2 */
    pub fn sqr(&self) -> DF {                           // SL and DL
        let mut z = DF::new();
        let mut t = DF::new();
        karsqr(&mut z.v,&self.v,&mut t.v[0..DL]);
        z
    }
}

#[cfg(feature = "std")]
impl std::fmt::Debug for DF {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(formatter, "{}", self.tostring())
    }
}

#[cfg(feature = "std")]
impl std::fmt::Display for DF {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(formatter, "{}", self.tostring())
    }
}

// wrt RSA modulus
impl DF {
// Constructor 
    pub fn new() -> DF {
        DF {
            v: [BIG::new();DL],
        }
    }

    pub fn zero(&mut self) {                // SL and DL
        for i in 0..DL {
            self.v[i].zero();
        }
    }

    // simple copy
    pub fn copy(&mut self, b: &DF) {
        generic_copy(&mut self.v,&b.v);
    }

    pub fn one(&mut self) {
        self.v[0].one();
        for i in 1..DL {
            self.v[i].zero();
        }
    }

    /* test equals 0 */
    pub fn iszilch(&self) -> bool {
        return generic_iszilch(&self.v); 
    }

    /* shift right by BIGBITS-bit words */
    pub fn shrw(&mut self, n: usize) {
        generic_shrw(&mut self.v,n);
    }

    /* shift left by BIGBITS-bit words */
    pub fn shlw(&mut self, n: usize) {
        generic_shlw(&mut self.v,n);
    }

    /* simple add */
    pub fn add(&mut self, b: &DF) {
        generic_add(&mut self.v,&b.v);
    }

    /* simple sub */
    pub fn sub(&mut self, b: &DF) {
        generic_sub(&mut self.v,&b.v);
    }

    /* reverse sub */
    pub fn revsub(&mut self, b: &DF) {
        for i in 0..DL {
            self.v[i].rsub(&b.v[i]);
        }
    }

    pub fn norm(&mut self) {
        rnorm(&mut self.v,false);
    }

    /* increment/decrement by a small integer */
    pub fn inc(&mut self, m: isize) {
        self.v[0].inc(m);
        self.norm();
    }

    pub fn dec(&mut self, m: isize) {
        self.v[0].dec(m);
        self.norm();
    }

    /* shift left by one bit */
    pub fn shl(&mut self) {
        generic_shl(&mut self.v);
    }

    /* shift right by one bit */
    pub fn shr(&mut self) {
        generic_shr(&mut self.v);
    }

    /* Convert to Hex String */
    #[cfg(feature = "std")]
    pub fn tostring(&self) -> String {
        let mut cs:[BIG;DL]=[BIG::new();DL];
        return generic_tostring(&self.v,&mut cs,DL);
    }

    /* Convert DFs to/from byte arrays */
    pub fn tobytes(&mut self, b: &mut [u8]) {
        for i in 0..DL {
            self.v[i].tobytearray(b, (DL - i - 1) * (big::MODBYTES as usize))
        }
    }

    /* compare x and y - must be normalised, and of same length */
    pub fn comp(&self, b: &DF) -> isize {                          // SL & DL only
        return generic_comp(&self.v,&b.v);
    }

    pub fn frombytes(&mut self, b: &[u8]) {                       // DL only
        for i in 0..DL {
            self.v[i] = BIG::frombytearray(b, (DL - i - 1) * (big::MODBYTES as usize))
        }
    }

    /* Set self=self mod m */
    pub fn rmod(&mut self, m: &DF) {
        let mut c:[BIG;DL]=[BIG::new();DL];       
        generic_rmod(&mut self.v,&m.v,&mut c);
    }

    /* return low part of product this*y */
    pub fn lmul(&mut self, y: &DF) {
        let mut t:[BIG;2*DL]=[BIG::new();2*DL];
        let mut x = DF::new();
        x.copy(&self);
        karmul_lower(&mut self.v,&x.v,&y.v,&mut t[0..2*DL]);
    }

    pub fn mod2m(&mut self, m: usize) {
        for i in m..DL {
            self.v[i].zero()
        }
    }

    /* U=1/a mod 2^m - Arazi & Qi */
    pub fn invmod2m(&self) -> DF {          // SL and DL       (?)            
        let mut b = DF::new();
        let mut c = DF::new();
        let mut u = DF::new();
        let mut t = DF::new();

        u.zero();
        u.v[0].copy(&self.v[0]);
        u.v[0].invmod2m();

        let mut i = 1;
        while i < DL {
            b.copy(&self);
            b.mod2m(i);
            t.copy(&u); t.lmul(&b);
            t.shrw(i);
            b.copy(&t);
            c.copy(&self);
            c.shrw(i);
            c.mod2m(i);
            c.lmul(&u);
            c.mod2m(i);

            b.add(&c);
            b.norm();
            b.lmul(&u);
            b.mod2m(i);

            c.one();
            c.shlw(i);
            b.revsub(&c);
            b.norm();
            b.shlw(i);
            u.add(&b);
            i <<= 1;
        }
        u.norm();
        u
    }

/*************************************************/


    /* nresidue mod m */
    pub fn nres(&mut self, m: &DF) {
        let mut d = QF::new();
        d.dsucopy(&self);
        self.copy(&d.dmod(m));
    }

    pub fn redc(&mut self, m: &DF, md: &DF) {
        let mut d = QF::new();
        self.rmod(m);
        d.dscopy(&self);
        self.copy(&d.reduce(&m, &md));
        self.rmod(m);
    }


    /* x=y<<n */
    pub fn dsucopy(&mut self, b: &SF) {
        generic_dsucopy(&mut self.v,&b.v);
    }

    /* x=y */
    pub fn dscopy(&mut self, b: &SF) {
        for i in 0..SL {
            self.v[i].copy(&b.v[i]);
            self.v[SL + i].zero();
        }
    }

    /* x=y */
    pub fn sdcopy(&mut self, b: &QF) {
        generic_sdcopy(&mut self.v,&b.v);
    }

    /* x=y>>n */
    pub fn sducopy(&mut self, b: &QF) {
        generic_sducopy(&mut self.v,&b.v);
    }

    /* return This mod modulus, ms is modulus, md is Montgomery Constant */
    pub fn reduce(&mut self, ms: &SF, md: &SF) -> SF {
        let mut r = SF::new();
        let mut m:[BIG;SL]=[BIG::new();SL];
        let mut t:[BIG;DL]=[BIG::new();DL];
        generic_reduce(&mut r.v,&mut self.v,&ms.v,&md.v,&mut m,&mut t);
        r
    }

    /* Set r=this mod b */
    /* this is of length - 2*n */
    /* r,b is of length - n */
    pub fn dmod(&mut self, b: &SF) -> SF {
        let mut r = SF::new();
        let mut m:[BIG;DL]=[BIG::new();DL];
        let mut x:[BIG;DL]=[BIG::new();DL];
        let k = big::BIGBITS * SL;
        generic_dmod(&mut r.v,&mut self.v,&b.v,&mut m,&mut x,k);
        return r;
    }

    /* this*=y mod p */
    pub fn modmul(&mut self, y: &DF, p: &DF, nd: &DF) {      // SL and DL
        if pexceed(&self.v[DL - 1], &y.v[DL - 1]) {
            self.rmod(p);
        }
        let mut d = self.mul(y);
        self.copy(&d.reduce(p, nd));
    }

    /* this*=y mod p */
    pub fn modsqr(&mut self, p: &DF, nd: &DF) {                 // SL and DL
        if sexceed(&self.v[DL - 1]) {
            self.rmod(p);
        }
        let mut d = self.sqr();
        d.norm();
        self.copy(&d.reduce(p, nd));
    }

    /* raise to an integer power - right-to-left method */
    pub fn power(&mut self, e: isize, p: &DF) {                   // DL only
        let mut w = DF::new();
        let nd = p.invmod2m();
        let mut f = true;
        let mut ee = e;

        w.copy(&self);
        w.nres(p);

        if ee == 2 {
            self.copy(&w);
            self.modsqr(p, &nd);
        } else {
            loop {
                if ee % 2 == 1 {
                    if f {
                        self.copy(&w);
                    } else {
                        self.modmul(&w, p, &nd)
                    }
                    f = false;
                }
                ee >>= 1;
                if ee == 0 {
                    break;
                }
                w.modsqr(p, &nd);
            }
        }
        self.redc(p, &nd);
    }

    /* z=x*y. Assumes x and y are of same length. */
    pub fn mul(&self, y: &DF) -> QF {                    // SL and DL
        let mut z = QF::new();
        let mut t = QF::new();
        karmul(&mut z.v,&self.v,&y.v,&mut t.v[0..QL]);
        z
    }

    /* z=x^2 */
    pub fn sqr(&self) -> QF {                           // SL and DL
        let mut z = QF::new();
        let mut t = QF::new();
        karsqr(&mut z.v,&self.v,&mut t.v[0..QL]);
        z
    }
}


impl QF {
// Constructor 
    pub fn new() -> QF {
        QF {
            v: [BIG::new();QL],
        }
    }

    /* x=y<<n */
    pub fn dsucopy(&mut self, b: &DF) {
        generic_dsucopy(&mut self.v,&b.v);
    }

    /* x=y */
    pub fn dscopy(&mut self, b: &DF) {
        for i in 0..DL {
            self.v[i].copy(&b.v[i]);
            self.v[DL + i].zero();
        }
    }

    pub fn norm(&mut self) {
        rnorm(&mut self.v,false);
    }

    // shift right by one bit 
    pub fn shr(&mut self) {
        generic_shr(&mut self.v);
    }

    /* return This mod modulus, ms is modulus, md is Montgomery Constant */
    pub fn reduce(&mut self, ms: &DF, md: &DF) -> DF {
        /* fast karatsuba Montgomery reduction */
        let mut r = DF::new();
        let mut m:[BIG;DL]=[BIG::new();DL];
        let mut t:[BIG;QL]=[BIG::new();QL];
        generic_reduce(&mut r.v,&mut self.v,&ms.v,&md.v,&mut m,&mut t);
        r
    }

    /* compare x and y - must be normalised, and of same length */
    pub fn comp(&self, b: &QF) -> isize {   
        return generic_comp(&self.v,&b.v);
    }

    /* Set r=this mod b */
    /* this is of length - 2*n */
    /* r,b is of length - n */
    pub fn dmod(&mut self, b: &DF) -> DF {
        let mut r = DF::new();
        let mut m:[BIG;QL]=[BIG::new();QL];
        let mut x:[BIG;QL]=[BIG::new();QL];
        let k = big::BIGBITS * DL;
        generic_dmod(&mut r.v,&mut self.v,&b.v,&mut m,&mut x,k);
        return r;
    }
}

#[cfg(feature = "std")]
fn generic_tostring(v: &[BIG],cs: &mut [BIG],len: usize) -> String {
    generic_copy(cs,v);
    generic_norm(cs);
    let mut s = String::new();
    let mut i=len-1;
    loop {
        s = s + cs[i].tostring().as_ref();
        if i == 0 {
            break;
        }
        i -= 1;
    }
    s
}       

// Set b=b mod m 
fn generic_rmod(v: &mut [BIG], m: &[BIG], c: &mut [BIG]) {
    let mut k = 1;
    generic_copy(c,m);
    generic_norm(v);
    if generic_comp(v,c) < 0 {
        return;
    }
    generic_shl(c);
    while generic_comp(v,c) >=0 {
        generic_shl(c);
        k+=1;
    }
    while k>0 {
        generic_shr(c);
        if generic_comp(v,c) >=0 {
            generic_sub(v,c);
            generic_norm(v);
        }
        k-=1;
    } 
}

fn generic_dmod(r: &mut[BIG],v: &mut [BIG],b: &[BIG],m: &mut [BIG],x: &mut [BIG],len: usize) {
    generic_copy(x,v);
    generic_norm(x);
    generic_dsucopy(m,b);
    let mut k=len;

    while generic_comp(x,m) >=0 {
        generic_sub(x,m);
        generic_norm(x);
    }
    while k>0 {
        generic_shr(m);
        if generic_comp(x,m) >=0 {
            generic_sub(x,m);
            generic_norm(x);
        }
        k-=1;
    }
    generic_sdcopy(r,x);
}

fn generic_copy(a: &mut [BIG],b: &[BIG]) {
    for i in 0..a.len() {
       a[i].copy(&b[i]);
    }
}

fn generic_iszilch(v: &[BIG]) -> bool {
   for i in 0..v.len() {
        if !v[i].iszilch() {
            return false;
        }
    }
    true 
}

fn generic_shl(v: &mut [BIG]) {
    let mut delay_carry: isize = 0;
    let len=v.len();
    for i in 0..len - 1 {
        let carry = v[i].fshl(1);
        v[i].inc(delay_carry);
        v[i].xortop((carry as Chunk) << P_TBITS);
        delay_carry = carry;
    }
    v[len - 1].fshl(1);
    v[len - 1].inc(delay_carry);        
}

fn generic_shr(v: &mut [BIG]) {
    let len=v.len();
    let mut i = len - 1;
    while i > 0 {
        let carry = v[i].fshr(1);
        v[i - 1].xortop((carry as Chunk) << P_TBITS);
        i -= 1;
    }
    v[0].fshr(1); 
}

fn generic_shrw(v: &mut [BIG],n: usize) {
    let mut t = BIG::new();
    for i in 0..n {
        t.copy(&v[i + n]);
            v[i].copy(&t);
            v[i + n].zero();
    }
}

fn generic_shlw(v: &mut [BIG],n: usize) {
    let mut t = BIG::new();
    for i in 0..n {
        t.copy(&v[i]);
        v[n + i].copy(&t);
        v[i].zero();
    }
}

/* compare x and y - must be normalised, and of same length */
fn generic_comp(a: &[BIG], b: &[BIG]) -> isize { 
    let mut i = a.len() - 1;
    loop {
        let j = BIG::comp(&a[i], &b[i]);
        if j != 0 {
            return j;
        }
        if i == 0 {
            return 0;
        }
        i -= 1;
    }
}

fn generic_reduce(r: &mut [BIG],v: &mut[BIG], ms: &[BIG], md:&[BIG], m:&mut [BIG],t:&mut [BIG]) {
    let len=ms.len();
    generic_sducopy(r,v);
    karmul_lower(m,&v[0..len],&md,&mut t[0..2*len]);
    karmul_upper(v,ms,m,&mut t[0..2*len]);
    generic_sducopy(m,v);
    generic_add(r,ms);
    generic_sub(r,m);
    generic_norm(r);
}

fn generic_add(a: &mut [BIG],b: &[BIG]) {
    for i in 0..a.len() {
        a[i].add(&b[i]);
    } 
}

fn generic_sub(a: &mut [BIG],b: &[BIG]) {
    for i in 0..a.len() {
        a[i].sub(&b[i]);
    } 
}

fn generic_sducopy(a: &mut [BIG],b: &[BIG]) {
    let len=a.len();
    for i in 0..len {
        a[i].copy(&b[len + i]);
    } 
}

fn generic_sdcopy(a: &mut [BIG],b: &[BIG]) {
    let len=a.len();
    for i in 0..len {
        a[i].copy(&b[i]);
    } 
}

fn generic_dsucopy(a: &mut [BIG],b: &[BIG]) {
    let len=b.len();
    for i in 0..len {
        a[i+len].copy(&b[i]);
        a[i].zero();
    }
}

fn generic_norm(a: &mut [BIG]) {
        rnorm(a,false);
}

pub fn excess(a: &BIG) -> Chunk {
    ((a.w[big::NLEN - 1] & P_OMASK) >> (P_TBITS)) + 1
}
pub fn pexceed(a: &BIG, b: &BIG) -> bool {
    let ea = excess(a);
    let eb = excess(b);
    ((ea + 1) as DChunk) * ((eb + 1) as DChunk) > P_FEXCESS as DChunk
}

pub fn sexceed(a: &BIG) -> bool {
    let ea = excess(a);
    ((ea + 1) as DChunk) * ((ea + 1) as DChunk) > P_FEXCESS as DChunk
}

// Karatsuba support
// recursive add 
pub fn radd(z: &mut [BIG], x: &[BIG], y: &[BIG]) {
    let n=y.len();
    for i in 0..n {
        z[i].copy(&x[i]);
        z[i].add(&y[i]);
    }
}

// recursive inc 
pub fn rinc(z: &mut [BIG], y: &[BIG]) {
    let n=y.len();
    for i in 0..n {
        z[i].add(&y[i]);
    }
}

// add bottom half to top half
pub fn rsinc(z: &mut [BIG]) {
    let n=z.len()/2;
    let mut t = BIG::new();
    for i in 0..n {
        t.copy(&z[i]);
        z[n + i].add(&t);
    }
}

// recursive sub 
pub fn rsub(z: &mut [BIG], x: &[BIG], y: &[BIG]) {
    let n=y.len();
    for i in 0..n {
        z[i].copy(&x[i]);
        z[i].sub(&y[i]);
    }
}

// recursive dec 
pub fn rdec(z: &mut [BIG],y: &[BIG]) {
    let n=y.len();
    for i in 0..n {
        z[i].sub(&y[i]);
    }
}

// normalise - but hold any overflow in top part unless n<0 
pub fn rnorm(z: &mut [BIG],trunc: bool) {
    let mut carry: Chunk;
    let n=z.len();
    let nn: usize = n as usize;

    for i in 0..nn - 1 {
        carry = z[i].norm();
        z[i].xortop(carry << P_TBITS);
        z[i + 1].w[0] += carry;
    }
    carry = z[nn - 1].norm();
    if trunc {
        z[nn - 1].xortop(carry << P_TBITS);
    }
}

// z=x*y, t is workspace 
fn karmul(z: &mut [BIG],x: &[BIG],y: &[BIG],t: *mut [BIG]) {
    let n=x.len();
    if n == 1 {
        let xx = BIG::new_copy(&x[0]);
        let yy = BIG::new_copy(&y[0]);
        let mut d = BIG::mul(&xx, &yy);
        z[1] = d.split(8 * big::MODBYTES);
        z[0].dcopy(&d);
        return;
    }
    let nd2 = n / 2;
    radd(&mut z[0..nd2],&x[0..nd2],&x[nd2..n]);
    rnorm(&mut z[0..nd2],false);
    radd(&mut z[nd2..n],&y[0..nd2],&y[nd2..n]);
    rnorm(&mut z[nd2..n],false);

// note the problem here: The Rust borrow checker doesn't see that 0..n and n..2n are disjoint!
// This is the ONLY reason for using *t and requiring an unsafe block
    unsafe {
        karmul(&mut (*t)[0..n],&z[0..nd2],&z[nd2..n],&mut (*t)[n..2*n]);  // 2 borrows here (!?)
        karmul(&mut z[0..n],&x[0..nd2],&y[0..nd2],&mut (*t)[n..2*n]);
        karmul(&mut z[n..2*n],&x[nd2..n],&y[nd2..n],&mut (*t)[n..2*n]);
        rdec(&mut (*t)[0..n],&z[0..n]);
        rdec(&mut (*t)[0..n],&z[n..2*n]);
        rinc(&mut z[nd2..n+nd2],&(*t)[0..n]);
    }
    rnorm(&mut z[0..2*n],false);
}

fn karsqr(z: &mut [BIG], x: &[BIG], t: *mut [BIG]) {
    let n=x.len();
    if n == 1 {
        let xx = BIG::new_copy(&x[0]);
        let mut d = BIG::sqr(&xx);
        z[1].copy(&d.split(8 * big::MODBYTES));
        z[0].dcopy(&d);
        return;
    }

    let nd2 = n / 2;
    unsafe {
        karsqr(&mut z[0..n], &x[0..nd2], &mut (*t)[n..2*n]);
        karsqr(&mut z[n..2*n], &x[nd2..n], &mut (*t)[n..2*n]);
        karmul(&mut (*t)[0..n],&x[0..nd2],&x[nd2..n],&mut (*t)[n..2*n]);
        rinc(&mut z[nd2..n+nd2],&(*t)[0..n]);
        rinc(&mut z[nd2..n+nd2],&(*t)[0..n]);
    }
    rnorm(&mut z[nd2..n+nd2],false);
}

// Calculates Least Significant bottom half of x*y 
fn karmul_lower(z: &mut [BIG],x: &[BIG],y: &[BIG],t: *mut [BIG],) {
    let n=x.len();
    if n == 1 {
        // only calculate bottom half of product 
        z[0].copy(&BIG::smul(&x[0], &y[0]));
        return;
    }
    let nd2 = n / 2;
    unsafe {
        karmul(&mut z[0..n], &x[0..nd2], &y[0..nd2], &mut (*t)[n..2*n]);
        karmul_lower(&mut (*t)[0..n],&x[nd2..n],&y[0..nd2],&mut (*t)[n..2*n]);
        rinc(&mut z[nd2..n],&(*t)[0..nd2]);
        karmul_lower(&mut (*t)[0..n],&x[0..nd2],&y[nd2..n],&mut (*t)[n..2*n]);
        rinc(&mut z[nd2..n],&(*t)[0..nd2]);
    }
    rnorm(&mut z[nd2..n],true);
}

// Calculates Most Significant upper half of x*y, given lower part 
fn karmul_upper(z: &mut [BIG], x: &[BIG], y: &[BIG], t: *mut [BIG]) {
    let n=x.len();
    let nd2 = n / 2;

    radd(&mut z[n..n+nd2],&x[0..nd2],&x[nd2..n]);
    radd(&mut z[n+nd2..2*n],&y[0..nd2],&y[nd2..n]);
    rnorm(&mut z[n..n+nd2], false);
    rnorm(&mut z[n+nd2..2*n],false);

    unsafe {
        karmul(&mut (*t)[0..n],&z[n+nd2..2*n],&z[n..n+nd2],&mut (*t)[n..2*n]);
        karmul(&mut z[n..2*n],&x[nd2..n],&y[nd2..n],&mut (*t)[n..2*n]);
        rdec(&mut (*t)[0..n],&z[n..2*n]);
        rsinc(&mut z[0..n]);
        rdec(&mut z[nd2..n],&(*t)[0..nd2]);
        rnorm(&mut z[0..n],true);
        rdec(&mut (*t)[0..n],&z[0..n]);
        rinc(&mut z[nd2..n+nd2],&(*t)[0..n]);
    }
    rnorm(&mut z[nd2..n+nd2],false);
}

pub fn igcd(x: isize, y: isize) -> isize {
/* integer GCD, returns GCD of x and y */

    if y == 0 {
        return x;
    }
    let mut xx = x;
    let mut yy = y;
    loop {
        let r = xx % yy;
        if r == 0 {
            break;
        }
        xx = yy;
        yy = r;
    }
    yy
}

    /* this =this^e mod p using side-channel resistant Montgomery Ladder, for short e 
    pub fn skpows(&mut self, e: &BIG, p: &SF) {
        let n = p.length;
        let mut r0 = SF::new_int(n);
        let mut r1 = SF::new_int(n);
        let nd = p.invmod2m();

        self.rmod(p);
        r0.one();
        r1.copy(&self);
        r0.nres(p);
        r1.nres(p);

        let mut i = 8 * (big::MODBYTES as usize) - 1;
        loop {
            let b = e.bit(i);
            self.copy(&r0);
            self.modmul(&r1, p, &nd);

            r0.cswap(&mut r1, b);
            r0.modsqr(p, &nd);

            r1.copy(&self);
            r0.cswap(&mut r1, b);
            if i == 0 {
                break;
            }
            i -= 1;
        }
        self.copy(&r0);
        self.redc(p, &nd);
    } */

    /* double exponentiation r=x^e.y^f mod p 
    pub fn pow2(&mut self, e: &BIG, y: &SF, f: &BIG, p: &SF) {
        let n = p.length;
        let mut xn = SF::new_int(n);
        let mut yn = SF::new_int(n);
        let mut xy = SF::new_int(n);
        let nd = p.invmod2m();

        xn.copy(&self);
        yn.copy(y);
        xn.nres(p);
        yn.nres(p);
        xy.copy(&xn);
        xy.modmul(&yn, p, &nd);
        self.one();
        self.nres(p);

        let mut i = 8 * (big::MODBYTES as usize) - 1;
        loop {
            let eb = e.bit(i);
            let fb = f.bit(i);
            self.modsqr(p, &nd);
            if eb == 1 {
                if fb == 1 {
                    self.modmul(&xy, p, &nd);
                } else {
                    self.modmul(&xn, p, &nd)
                }
            } else {
                if fb == 1 {
                    self.modmul(&yn, p, &nd)
                }
            }
            if i == 0 {
                break;
            }
            i -= 1;
        }
        self.redc(p, &nd);
    } */
