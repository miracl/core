/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
*/

use crate::xxx::big;
use crate::xxx::big::BIG;
use crate::xxx::ecp;
use crate::xxx::fp2::FP2;
use crate::xxx::fp4::FP4;
use crate::xxx::rom;
use crate::xxx::fp;
//use crate::xxx::fp::FP;
use crate::xxx::dbig::DBIG;

pub struct ECP4 {
    x: FP4,
    y: FP4,
    z: FP4,
}

#[allow(non_snake_case)]
impl ECP4 {
    pub fn new() -> ECP4 {
        ECP4 {
            x: FP4::new(),
            y: FP4::new_int(1),
            z: FP4::new(),
        }
    }
    #[allow(non_snake_case)]
    /* construct this from (x,y) - but set to O if not on curve */
    pub fn new_fp4s(ix: &FP4, iy: &FP4) -> ECP4 {
        let mut E = ECP4::new();
        E.x.copy(&ix);
        E.y.copy(&iy);
        E.z.one();
        E.x.norm();

        let mut rhs = ECP4::rhs(&E.x);
        let mut y2 = FP4::new_copy(&E.y);
        y2.sqr();
        if !y2.equals(&mut rhs) {
            E.inf();
        }
        return E;
    }

    /* construct this from x - but set to O if not on curve */
    pub fn new_fp4(ix: &FP4, s:isize) -> ECP4 {
        let mut E = ECP4::new();
        E.x.copy(&ix);
        E.y.one();
        E.z.one();
        E.x.norm();

        let mut rhs = ECP4::rhs(&E.x);
 	    if rhs.qr() == 1 {
		    rhs.sqrt();
		    if rhs.sign() != s {
			    rhs.neg();
		    }
		    rhs.reduce();
		    E.y.copy(&rhs);
        } else {
            E.inf();
        }
        return E;
    }

    /* Test this=O? */
    pub fn is_infinity(&self) -> bool {
        return self.x.iszilch() && self.z.iszilch();
    }

    /* copy self=P */
    pub fn copy(&mut self, P: &ECP4) {
        self.x.copy(&P.x);
        self.y.copy(&P.y);
        self.z.copy(&P.z);
    }

    /* set self=O */
    pub fn inf(&mut self) {
        self.x.zero();
        self.y.one();
        self.z.zero();
    }

    /* set self=-self */
    pub fn neg(&mut self) {
        self.y.norm();
        self.y.neg();
        self.y.norm();
    }

    /* Conditional move of Q to self dependant on d */
    pub fn cmove(&mut self, Q: &ECP4, d: isize) {
        self.x.cmove(&Q.x, d);
        self.y.cmove(&Q.y, d);
        self.z.cmove(&Q.z, d);
    }

    /* return 1 if b==c, no branching */
    fn teq(b: i32, c: i32) -> isize {
        let mut x = b ^ c;
        x -= 1; // if x=0, x now -1
        return ((x >> 31) & 1) as isize;
    }

    /* Constant time select from pre-computed table */
    pub fn selector(&mut self, W: &[ECP4], b: i32) {
        let mut MP = ECP4::new();
        let m = b >> 31;
        let mut babs = (b ^ m) - m;

        babs = (babs - 1) / 2;

        self.cmove(&W[0], ECP4::teq(babs, 0)); // conditional move
        self.cmove(&W[1], ECP4::teq(babs, 1));
        self.cmove(&W[2], ECP4::teq(babs, 2));
        self.cmove(&W[3], ECP4::teq(babs, 3));
        self.cmove(&W[4], ECP4::teq(babs, 4));
        self.cmove(&W[5], ECP4::teq(babs, 5));
        self.cmove(&W[6], ECP4::teq(babs, 6));
        self.cmove(&W[7], ECP4::teq(babs, 7));

        MP.copy(self);
        MP.neg();
        self.cmove(&MP, (m & 1) as isize);
    }

    /* Test if P == Q */
    pub fn equals(&mut self, Q: &mut ECP4) -> bool {
        let mut a = FP4::new_copy(&self.x);
        let mut b = FP4::new_copy(&Q.x);

        a.mul(&Q.z);
        b.mul(&self.z);
        if !a.equals(&mut b) {
            return false;
        }
        a.copy(&self.y);
        a.mul(&Q.z);
        b.copy(&Q.y);
        b.mul(&self.z);
        if !a.equals(&mut b) {
            return false;
        }

        return true;
    }

    /* set to Affine - (x,y,z) to (x,y) */
    pub fn affine(&mut self) {
        if self.is_infinity() {
            return;
        }
        let mut one = FP4::new_int(1);
        if self.z.equals(&mut one) {
            return;
        }
        self.z.inverse();

        self.x.mul(&self.z);
        self.x.reduce();
        self.y.mul(&self.z);
        self.y.reduce();
        self.z.copy(&one);
    }

    /* extract affine x as FP4 */
    pub fn getx(&self) -> FP4 {
        let mut W = ECP4::new();
        W.copy(self);
        W.affine();
        return FP4::new_copy(&self.x);
    }

    /* extract affine y as FP4 */
    pub fn gety(&self) -> FP4 {
        let mut W = ECP4::new();
        W.copy(self);
        W.affine();
        return FP4::new_copy(&W.y);
    }

    /* extract projective x */
    pub fn getpx(&self) -> FP4 {
        return FP4::new_copy(&self.x);
    }
    /* extract projective y */
    pub fn getpy(&self) -> FP4 {
        return FP4::new_copy(&self.y);
    }
    /* extract projective z */
    pub fn getpz(&self) -> FP4 {
        return FP4::new_copy(&self.z);
    }

    /* convert to byte array */
    pub fn tobytes(&self, b: &mut [u8], compress: bool) {
        let mut t: [u8; big::MODBYTES as usize] = [0; big::MODBYTES as usize];
        let mb = big::MODBYTES as usize;

        let mut W = ECP4::new();
        W.copy(self);
        W.affine();
        b[0]=0x06;

        W.x.geta().geta().tobytes(&mut t);
        for i in 0..mb {
            b[i+1] = t[i]
        }
        W.x.geta().getb().tobytes(&mut t);
        for i in 0..mb {
            b[i + mb +1] = t[i]
        }

        W.x.getb().geta().tobytes(&mut t);
        for i in 0..mb {
            b[i + 2 * mb +1] = t[i]
        }
        W.x.getb().getb().tobytes(&mut t);
        for i in 0..mb {
            b[i + 3 * mb +1] = t[i]
        }
        if !compress {
            b[0]=0x04;
            W.y.geta().geta().tobytes(&mut t);
            for i in 0..mb {
                b[i + 4 * mb +1] = t[i]
            }
            W.y.geta().getb().tobytes(&mut t);
            for i in 0..mb {
                b[i + 5 * mb +1] = t[i]
            }

            W.y.getb().geta().tobytes(&mut t);
            for i in 0..mb {
                b[i + 6 * mb +1] = t[i]
            }
            W.y.getb().getb().tobytes(&mut t);
            for i in 0..mb {
                b[i + 7 * mb +1] = t[i]
            }
        } else {
            b[0]=0x02;
            if W.y.sign() == 1 {
                b[0]=0x03;
            }
        }
    }

    /* convert from byte array to point */
    pub fn frombytes(b: &[u8]) -> ECP4 {
        let mut t: [u8; big::MODBYTES as usize] = [0; big::MODBYTES as usize];
        let mb = big::MODBYTES as usize;
        let typ=b[0] as isize;

        for i in 0..mb {
            t[i] = b[i + 1]
        }
        let mut ra = BIG::frombytes(&t);
        for i in 0..mb {
            t[i] = b[i + mb + 1]
        }
        let mut rb = BIG::frombytes(&t);

        let mut ra4 = FP2::new_bigs(&ra, &rb);

        for i in 0..mb {
            t[i] = b[i + 2 * mb + 1]
        }
        ra.copy(&BIG::frombytes(&t));
        for i in 0..mb {
            t[i] = b[i + 3 * mb + 1]
        }
        rb.copy(&BIG::frombytes(&t));

        let mut rb4 = FP2::new_bigs(&ra, &rb);

        let rx = FP4::new_fp2s(&ra4, &rb4);
        if typ==0x04 {
            for i in 0..mb {
                t[i] = b[i + 4 * mb + 1]
            }
            ra.copy(&BIG::frombytes(&t));
            for i in 0..mb {
                t[i] = b[i + 5 * mb + 1]
            }
            rb.copy(&BIG::frombytes(&t));

            ra4.copy(&FP2::new_bigs(&ra, &rb));

            for i in 0..mb {
                t[i] = b[i + 6 * mb + 1]
            }
            ra.copy(&BIG::frombytes(&t));
            for i in 0..mb {
                t[i] = b[i + 7 * mb + 1]
            }
            rb.copy(&BIG::frombytes(&t));

            rb4.copy(&FP2::new_bigs(&ra, &rb));

            let ry = FP4::new_fp2s(&ra4, &rb4);

            return ECP4::new_fp4s(&rx, &ry);
        } else {
            return ECP4::new_fp4(&rx,typ&1);
        }
    }

    /* convert this to hex string */
    pub fn tostring(&self) -> String {
        let mut W = ECP4::new();
        W.copy(self);
        W.affine();
        if W.is_infinity() {
            return String::from("infinity");
        }
        return format!("({},{})", W.x.tostring(), W.y.tostring());
    }

    /* Calculate RHS of twisted curve equation x^3+B/i */
    pub fn rhs(x: &FP4) -> FP4 {
        //x.norm();
        let mut r = FP4::new_copy(x);
        r.sqr();
        let mut b = FP4::new_fp2(&FP2::new_big(&BIG::new_ints(&rom::CURVE_B)));
        if ecp::SEXTIC_TWIST == ecp::D_TYPE {
            b.div_i();
        }
        if ecp::SEXTIC_TWIST == ecp::M_TYPE {
            b.times_i();
        }

        r.mul(x);
        r.add(&b);

        r.reduce();
        return r;
    }

    /* self+=self */
    pub fn dbl(&mut self) -> isize {
        let mut iy = FP4::new_copy(&self.y);
        if ecp::SEXTIC_TWIST == ecp::D_TYPE {
            iy.times_i(); //iy.norm();
        }

        let mut t0 = FP4::new_copy(&self.y);
        t0.sqr();
        if ecp::SEXTIC_TWIST == ecp::D_TYPE {
            t0.times_i();
        }
        let mut t1 = FP4::new_copy(&iy);
        t1.mul(&self.z);
        let mut t2 = FP4::new_copy(&self.z);
        t2.sqr();

        self.z.copy(&t0);
        self.z.add(&t0);
        self.z.norm();
        self.z.dbl();
        self.z.dbl();
        self.z.norm();

        t2.imul(3 * rom::CURVE_B_I);
        if ecp::SEXTIC_TWIST == ecp::M_TYPE {
            t2.times_i();
        }
        let mut x3 = FP4::new_copy(&t2);
        x3.mul(&self.z);

        let mut y3 = FP4::new_copy(&t0);

        y3.add(&t2);
        y3.norm();
        self.z.mul(&t1);
        t1.copy(&t2);
        t1.add(&t2);
        t2.add(&t1);
        t2.norm();
        t0.sub(&t2);
        t0.norm(); //y^2-9bz^2
        y3.mul(&t0);
        y3.add(&x3); //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2
        t1.copy(&self.x);
        t1.mul(&iy); //
        self.x.copy(&t0);
        self.x.norm();
        self.x.mul(&t1);
        self.x.dbl(); //(y^2-9bz^2)xy2

        self.x.norm();
        self.y.copy(&y3);
        self.y.norm();

        return 1;
    }

    /* self+=Q - return 0 for add, 1 for double, -1 for O */
    pub fn add(&mut self, Q: &ECP4) -> isize {
        let b = 3 * rom::CURVE_B_I;
        let mut t0 = FP4::new_copy(&self.x);
        t0.mul(&Q.x); // x.Q.x
        let mut t1 = FP4::new_copy(&self.y);
        t1.mul(&Q.y); // y.Q.y

        let mut t2 = FP4::new_copy(&self.z);
        t2.mul(&Q.z);
        let mut t3 = FP4::new_copy(&self.x);
        t3.add(&self.y);
        t3.norm(); //t3=X1+Y1
        let mut t4 = FP4::new_copy(&Q.x);
        t4.add(&Q.y);
        t4.norm(); //t4=X2+Y2
        t3.mul(&t4); //t3=(X1+Y1)(X2+Y2)
        t4.copy(&t0);
        t4.add(&t1); //t4=X1.X2+Y1.Y2

        t3.sub(&t4);
        t3.norm();
        if ecp::SEXTIC_TWIST == ecp::D_TYPE {
            t3.times_i(); //t3=(X1+Y1)(X2+Y2)-(X1.X2+Y1.Y2) = X1.Y2+X2.Y1
        }
        t4.copy(&self.y);
        t4.add(&self.z);
        t4.norm(); //t4=Y1+Z1
        let mut x3 = FP4::new_copy(&Q.y);
        x3.add(&Q.z);
        x3.norm(); //x3=Y2+Z2

        t4.mul(&x3); //t4=(Y1+Z1)(Y2+Z2)
        x3.copy(&t1); //
        x3.add(&t2); //X3=Y1.Y2+Z1.Z2

        t4.sub(&x3);
        t4.norm();
        if ecp::SEXTIC_TWIST == ecp::D_TYPE {
            t4.times_i(); //t4=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1
        }
        x3.copy(&self.x);
        x3.add(&self.z);
        x3.norm(); // x3=X1+Z1
        let mut y3 = FP4::new_copy(&Q.x);
        y3.add(&Q.z);
        y3.norm(); // y3=X2+Z2
        x3.mul(&y3); // x3=(X1+Z1)(X2+Z2)
        y3.copy(&t0);
        y3.add(&t2); // y3=X1.X2+Z1+Z2
        y3.rsub(&x3);
        y3.norm(); // y3=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1

        if ecp::SEXTIC_TWIST == ecp::D_TYPE {
            t0.times_i(); // x.Q.x
            t1.times_i(); // y.Q.y
        }
        x3.copy(&t0);
        x3.add(&t0);
        t0.add(&x3);
        t0.norm();
        t2.imul(b);
        if ecp::SEXTIC_TWIST == ecp::M_TYPE {
            t2.times_i();
        }
        let mut z3 = FP4::new_copy(&t1);
        z3.add(&t2);
        z3.norm();
        t1.sub(&t2);
        t1.norm();
        y3.imul(b);
        if ecp::SEXTIC_TWIST == ecp::M_TYPE {
            y3.times_i();
        }
        x3.copy(&y3);
        x3.mul(&t4);
        t2.copy(&t3);
        t2.mul(&t1);
        x3.rsub(&t2);
        y3.mul(&t0);
        t1.mul(&z3);
        y3.add(&t1);
        t0.mul(&t3);
        z3.mul(&t4);
        z3.add(&t0);

        self.x.copy(&x3);
        self.x.norm();
        self.y.copy(&y3);
        self.y.norm();
        self.z.copy(&z3);
        self.z.norm();

        return 0;
    }

    /* set this-=Q */
    pub fn sub(&mut self, Q: &ECP4) -> isize {
        let mut NQ = ECP4::new();
        NQ.copy(Q);
        NQ.neg();
        let d = self.add(&NQ);
        return d;
    }

    pub fn frob_constants() -> [FP2; 3] {
        let f = FP2::new_bigs(&BIG::new_ints(&rom::FRA), &BIG::new_ints(&rom::FRB));

        let mut f0 = FP2::new_copy(&f);
        f0.sqr();
        let mut f2 = FP2::new_copy(&f0);
        f2.mul_ip();
        f2.norm();
        let mut f1 = FP2::new_copy(&f2);
        f1.sqr();
        f2.mul(&f1);
        f1.copy(&f);
        if ecp::SEXTIC_TWIST == ecp::M_TYPE {
            f1.mul_ip();
            f1.inverse();
            f0.copy(&f1);
            f0.sqr();
        }
        f0.mul_ip();
        f0.norm();
        f1.mul(&f0);

        let F: [FP2; 3] = [f0, f1, f2];
        return F;
    }

    /* set this*=q, where q is Modulus, using Frobenius */
    pub fn frob(&mut self, f: &[FP2; 3], n: isize) {
        for _i in 0..n {
            self.x.frob(&f[2]);
            self.x.pmul(&f[0]);

            self.y.frob(&f[2]);
            self.y.pmul(&f[1]);
            self.y.times_i();

            self.z.frob(&f[2]);
        }
    }

    /* self*=e */
    pub fn mul(&self, e: &BIG) -> ECP4 {
        /* fixed size windows */
        let mut mt = BIG::new();
        let mut t = BIG::new();
        let mut P = ECP4::new();
        let mut Q = ECP4::new();
        let mut C = ECP4::new();

        if self.is_infinity() {
            return P;
        }

        let mut W: [ECP4; 8] = [
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
        ];

        const CT: usize = 1 + (big::NLEN * (big::BASEBITS as usize) + 3) / 4;
        let mut w: [i8; CT] = [0; CT];

        /* precompute table */
        Q.copy(&self);
        Q.dbl();

        W[0].copy(&self);

        for i in 1..8 {
            C.copy(&W[i - 1]);
            W[i].copy(&C);
            W[i].add(&mut Q);
        }

        /* make exponent odd - add 2P if even, P if odd */
        t.copy(&e);
        let s = t.parity();
        t.inc(1);
        t.norm();
        let ns = t.parity();
        mt.copy(&t);
        mt.inc(1);
        mt.norm();
        t.cmove(&mt, s);
        Q.cmove(&self, ns);
        C.copy(&Q);

        let nb = 1 + (t.nbits() + 3) / 4;

        /* convert exponent to signed 4-bit window */
        for i in 0..nb {
            w[i] = (t.lastbits(5) - 16) as i8;
            t.dec(w[i] as isize);
            t.norm();
            t.fshr(4);
        }
        w[nb] = (t.lastbits(5)) as i8;

        P.copy(&W[((w[nb] as usize) - 1) / 2]);
        for i in (0..nb).rev() {
            Q.selector(&W, w[i] as i32);
            P.dbl();
            P.dbl();
            P.dbl();
            P.dbl();
            P.add(&mut Q);
        }
        P.sub(&mut C);
        P.affine();
        return P;
    }

    #[allow(non_snake_case)]
    pub fn cfp(&mut self) {
        let f = ECP4::frob_constants();
        let mut x = BIG::new_ints(&rom::CURVE_BNX);

        let mut xQ = self.mul(&mut x);
        let mut x2Q = xQ.mul(&mut x);
        let mut x3Q = x2Q.mul(&mut x);
        let mut x4Q = x3Q.mul(&mut x);

        if ecp::SIGN_OF_X == ecp::NEGATIVEX {
            xQ.neg();
            x3Q.neg();
        }

        x4Q.sub(&x3Q);
        x4Q.sub(&self);

        x3Q.sub(&x2Q);
        x3Q.frob(&f, 1);

        x2Q.sub(&xQ);
        x2Q.frob(&f, 2);

        xQ.sub(&self);
        xQ.frob(&f, 3);

        self.dbl();
        self.frob(&f, 4);

        self.add(&x4Q);
        self.add(&x3Q);
        self.add(&x2Q);
        self.add(&xQ);

        self.affine();
    }

    /* P=u0.Q0+u1*Q1+u2*Q2+u3*Q3.. */
    // Bos & Costello https://eprint.iacr.org/2013/458.pdf
    // Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
    // Side channel attack secure

    pub fn mul8(Q: &mut [ECP4], u: &[BIG]) -> ECP4 {
        let mut W = ECP4::new();
        let mut P = ECP4::new();

        let mut T1: [ECP4; 8] = [
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
        ];
        let mut T2: [ECP4; 8] = [
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
            ECP4::new(),
        ];

        let mut mt = BIG::new();

        let mut t: [BIG; 8] = [
            BIG::new_copy(&u[0]),
            BIG::new_copy(&u[1]),
            BIG::new_copy(&u[2]),
            BIG::new_copy(&u[3]),
            BIG::new_copy(&u[4]),
            BIG::new_copy(&u[5]),
            BIG::new_copy(&u[6]),
            BIG::new_copy(&u[7]),
        ];

        const CT: usize = 1 + big::NLEN * (big::BASEBITS as usize);
        let mut w1: [i8; CT] = [0; CT];
        let mut s1: [i8; CT] = [0; CT];
        let mut w2: [i8; CT] = [0; CT];
        let mut s2: [i8; CT] = [0; CT];

        for i in 0..8 {
            //Q[i].affine();
            t[i].norm();
        }

        T1[0].copy(&Q[0]);
        W.copy(&T1[0]);
        T1[1].copy(&W);
        T1[1].add(&mut Q[1]); // Q[0]+Q[1]
        T1[2].copy(&W);
        T1[2].add(&mut Q[2]);
        W.copy(&T1[1]); // Q[0]+Q[2]
        T1[3].copy(&W);
        T1[3].add(&mut Q[2]);
        W.copy(&T1[0]); // Q[0]+Q[1]+Q[2]
        T1[4].copy(&W);
        T1[4].add(&mut Q[3]);
        W.copy(&T1[1]); // Q[0]+Q[3]
        T1[5].copy(&W);
        T1[5].add(&mut Q[3]);
        W.copy(&T1[2]); // Q[0]+Q[1]+Q[3]
        T1[6].copy(&W);
        T1[6].add(&mut Q[3]);
        W.copy(&T1[3]); // Q[0]+Q[2]+Q[3]
        T1[7].copy(&W);
        T1[7].add(&mut Q[3]); // Q[0]+Q[1]+Q[2]+Q[3]

        // Use frobenius
        let f = ECP4::frob_constants();
        for i in 0..8 {
            T2[i].copy(&T1[i]);
            T2[i].frob(&f, 4);
        }

        // Make it odd
        let pb1 = 1 - t[0].parity();
        t[0].inc(pb1);
        t[0].norm();

        let pb2 = 1 - t[4].parity();
        t[4].inc(pb2);
        t[4].norm();

        // Number of bits
        mt.zero();
        for i in 0..8 {
            mt.or(&t[i]);
        }

        let nb = 1 + mt.nbits();

        // Sign pivot

        s1[nb - 1] = 1;
        s2[nb - 1] = 1;
        for i in 0..nb - 1 {
            t[0].fshr(1);
            s1[i] = (2 * t[0].parity() - 1) as i8;
            t[4].fshr(1);
            s2[i] = (2 * t[4].parity() - 1) as i8;
        }

        // Recoded exponent
        for i in 0..nb {
            w1[i] = 0;
            let mut k = 1;
            for j in 1..4 {
                let bt = s1[i] * (t[j].parity() as i8);
                t[j].fshr(1);
                t[j].dec((bt >> 1) as isize);
                t[j].norm();
                w1[i] += bt * (k as i8);
                k = 2 * k;
            }

            w2[i] = 0;
            k = 1;
            for j in 5..8 {
                let bt = s2[i] * (t[j].parity() as i8);
                t[j].fshr(1);
                t[j].dec((bt >> 1) as isize);
                t[j].norm();
                w2[i] += bt * (k as i8);
                k = 2 * k;
            }
        }

        // Main loop
        P.selector(&T1, (2 * w1[nb - 1] + 1) as i32);
        W.selector(&T2, (2 * w2[nb - 1] + 1) as i32);
        P.add(&mut W);
        for i in (0..nb - 1).rev() {
            P.dbl();
            W.selector(&T1, (2 * w1[i] + s1[i]) as i32);
            P.add(&mut W);
            W.selector(&T2, (2 * w2[i] + s2[i]) as i32);
            P.add(&mut W);
        }

        // apply correction
        W.copy(&P);
        W.sub(&mut Q[0]);
        P.cmove(&W, pb1);

        W.copy(&P);
        W.sub(&mut Q[4]);
        P.cmove(&W, pb2);

        P.affine();

        return P;
    }

    pub fn generator() -> ECP4 {
        return ECP4::new_fp4s(
            &FP4::new_fp2s(
                &FP2::new_bigs(
                    &BIG::new_ints(&rom::CURVE_PXAA),
                    &BIG::new_ints(&rom::CURVE_PXAB),
                ),
                &FP2::new_bigs(
                    &BIG::new_ints(&rom::CURVE_PXBA),
                    &BIG::new_ints(&rom::CURVE_PXBB),
                ),
            ),
            &FP4::new_fp2s(
                &FP2::new_bigs(
                    &BIG::new_ints(&rom::CURVE_PYAA),
                    &BIG::new_ints(&rom::CURVE_PYAB),
                ),
                &FP2::new_bigs(
                    &BIG::new_ints(&rom::CURVE_PYBA),
                    &BIG::new_ints(&rom::CURVE_PYBB),
                ),
            ),
        );
    }

/* Hunt and Peck a BIG to a curve point */
    #[allow(non_snake_case)]
    pub fn hap2point(h: &BIG) -> ECP4 {
        let mut Q: ECP4;
        let one = BIG::new_int(1);
        let mut x =BIG::new_copy(&h);
        loop {
            let X = FP4::new_fp2(&FP2::new_bigs(&one, &x));
            Q = ECP4::new_fp4(&X,0);
            if !Q.is_infinity() {
                break;
            }
            x.inc(1);
            x.norm();
        }
        return Q;
    }

/* Constant time Map to Point */
    #[allow(non_snake_case)]
    pub fn map2point(H: &FP4) -> ECP4 {
    // Shallue and van de Woestijne
        let mut W=FP4::new_int(1);
        let mut B = FP4::new_fp2(&FP2::new_big(&BIG::new_ints(&rom::CURVE_B)));
        let mut T=FP4::new_copy(H);
        //let mut s=FP::new_int(-3);
        //let one=FP::new_int(1);
		if ecp::SEXTIC_TWIST == ecp::D_TYPE {
            B.div_i();
        }
		if ecp::SEXTIC_TWIST == ecp::M_TYPE {
            B.times_i();
        }
        B.norm();
        let sgn=T.sign();

        let mut Z=FP4::new_ints(fp::RIADZG2A,fp::RIADZG2B);
        let mut X1=FP4::new_copy(&Z);
        let mut X3=FP4::new_copy(&X1);
        let mut A=ECP4::rhs(&X1);
        T.sqr();
        let mut Y=FP4::new_copy(&A); Y.mul(&T);
        T.copy(&W); T.add(&Y); T.norm();
        Y.rsub(&W); Y.norm();
        let mut NY=FP4::new_copy(&T); NY.mul(&Y); NY.inverse();
        A.neg(); A.norm();
        W.copy(&A); W.imul(3); W.sqrt();
        W.mul(&Z);
        W.mul(&H); W.mul(&Y); W.mul(&NY);

        X1.neg(); X1.norm(); X1.div2();
        let mut X2=FP4::new_copy(&X1);
        X1.sub(&W); X1.norm();
        X2.add(&W); X2.norm();
        A.dbl(); A.dbl(); A.norm();
        T.sqr(); T.mul(&NY); T.sqr();
        A.mul(&T);
        Z.sqr(); Z.imul(3); T.copy(&Z);
        T.inverse();
        A.mul(&T);
        X3.add(&A); X3.norm();
/*
        let w=s.sqrt(None);
        let mut j=FP::new_copy(&w); j.sub(&one); j.norm(); j.div2();

        let mut S=FP4::new_copy(&T);
        S.qmul(&w);
        let mut Y=FP4::new_copy(&T);
        Y.sqr();
        Y.add(&W);
        B.add(&Y); B.norm(); B.inverse();
        B.mul(&S);

        let mut X1=FP4::new_copy(&B); X1.mul(&T);
        Y.copy(&FP4::new_fp(&j));
        let mut X2=FP4::new_copy(&X1); X2.sub(&Y); X2.norm();
        X1.copy(&X2); X1.neg(); X1.norm();
        X2.sub(&W); X2.norm();

        B.sqr(); B.inverse();
        let mut X3=FP4::new_copy(&B); X3.add(&W); X3.norm();
*/
        Y.copy(&ECP4::rhs(&X2));
        X3.cmove(&X2,Y.qr());
        Y.copy(&ECP4::rhs(&X1));
        X3.cmove(&X1,Y.qr());
        Y.copy(&ECP4::rhs(&X3));
        Y.sqrt();

        let ne=Y.sign()^sgn;
        W.copy(&Y); W.neg(); W.norm();
        Y.cmove(&W,ne);

        return ECP4::new_fp4s(&X3,&Y);
    }

/* Map byte string to curve point */
    #[allow(non_snake_case)]
    pub fn mapit(h: &[u8]) -> ECP4 {
        let q = BIG::new_ints(&rom::MODULUS);
        let mut dx = DBIG::frombytes(h);
        let mut x=dx.dmod(&q);
        let mut P=ECP4::hap2point(&mut x);
        P.cfp();
        return P;
    }


}
