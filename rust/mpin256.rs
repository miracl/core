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

use std::time::SystemTime;
use std::time::UNIX_EPOCH;

use crate::xxx::big;
use crate::xxx::big::BIG;
use crate::xxx::ecp;
use crate::xxx::ecp::ECP;
use crate::xxx::ecp8::ECP8;
use crate::xxx::fp16::FP16;
use crate::xxx::fp48::FP48;
use crate::xxx::pair256;
use crate::xxx::rom;

use crate::hmac;
use crate::rand::RAND;

/* MPIN API Functions */

/* Configure mode of operation */

pub const EFS: usize = big::MODBYTES as usize;
pub const EGS: usize = big::MODBYTES as usize;
pub const BAD_PARAMS: isize = -11;
pub const INVALID_POINT: isize = -14;
pub const WRONG_ORDER: isize = -18;
pub const BAD_PIN: isize = -19;
pub const SHA256: usize = 32;
pub const SHA384: usize = 48;
pub const SHA512: usize = 64;

/* Configure your PIN here */

pub const MAXPIN: i32 = 10000; /* PIN less than this */
pub const PBLEN: i32 = 14; /* Number of bits in PIN */
pub const TS: usize = 10; /* 10 for 4 digit PIN, 14 for 6-digit PIN - 2^TS/TS approx = sqrt(MAXPIN) */
pub const TRAP: usize = 200; /* 200 for 4 digit PIN, 2000 for 6-digit PIN  - approx 2*sqrt(MAXPIN) */

#[allow(non_snake_case)]
fn hash(sha: usize, c: &mut FP16, U: &mut ECP, r: &mut [u8]) -> bool {
    let mut w: [u8; EFS] = [0; EFS];
    let mut t: [u8; 18 * EFS] = [0; 18 * EFS];

    c.geta().geta().geta().geta().tobytes(&mut w);
    for i in 0..EFS {
        t[i] = w[i]
    }
    c.geta().geta().geta().getb().tobytes(&mut w);
    for i in EFS..2 * EFS {
        t[i] = w[i - EFS]
    }
    c.geta().geta().getb().geta().tobytes(&mut w);
    for i in 2 * EFS..3 * EFS {
        t[i] = w[i - 2 * EFS]
    }
    c.geta().geta().getb().getb().tobytes(&mut w);
    for i in 3 * EFS..4 * EFS {
        t[i] = w[i - 3 * EFS]
    }
    c.geta().getb().geta().geta().tobytes(&mut w);
    for i in 4 * EFS..5 * EFS {
        t[i] = w[i - 4 * EFS]
    }
    c.geta().getb().geta().getb().tobytes(&mut w);
    for i in 5 * EFS..6 * EFS {
        t[i] = w[i - 5 * EFS]
    }
    c.geta().getb().getb().geta().tobytes(&mut w);
    for i in 6 * EFS..7 * EFS {
        t[i] = w[i - 6 * EFS]
    }
    c.geta().getb().getb().getb().tobytes(&mut w);
    for i in 7 * EFS..8 * EFS {
        t[i] = w[i - 7 * EFS]
    }

    c.getb().geta().geta().geta().tobytes(&mut w);
    for i in 8 * EFS..9 * EFS {
        t[i] = w[i - 8 * EFS]
    }
    c.getb().geta().geta().getb().tobytes(&mut w);
    for i in 9 * EFS..10 * EFS {
        t[i] = w[i - 9 * EFS]
    }
    c.getb().geta().getb().geta().tobytes(&mut w);
    for i in 10 * EFS..11 * EFS {
        t[i] = w[i - 10 * EFS]
    }
    c.getb().geta().getb().getb().tobytes(&mut w);
    for i in 11 * EFS..12 * EFS {
        t[i] = w[i - 11 * EFS]
    }
    c.getb().getb().geta().geta().tobytes(&mut w);
    for i in 12 * EFS..13 * EFS {
        t[i] = w[i - 12 * EFS]
    }
    c.getb().getb().geta().getb().tobytes(&mut w);
    for i in 13 * EFS..14 * EFS {
        t[i] = w[i - 13 * EFS]
    }
    c.getb().getb().getb().geta().tobytes(&mut w);
    for i in 14 * EFS..15 * EFS {
        t[i] = w[i - 14 * EFS]
    }
    c.getb().getb().getb().getb().tobytes(&mut w);
    for i in 15 * EFS..16 * EFS {
        t[i] = w[i - 15 * EFS]
    }

    U.getx().tobytes(&mut w);
    for i in 16 * EFS..17 * EFS {
        t[i] = w[i - 16 * EFS]
    }
    U.gety().tobytes(&mut w);
    for i in 17 * EFS..18 * EFS {
        t[i] = w[i - 17 * EFS]
    }

    hmac::GPhashit(hmac::MC_SHA2,sha,r,ecp::AESKEY,None,-1,Some(&t));

    return true;
}

/* return time in slots since epoch */
pub fn today() -> usize {
    return (SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_secs()
        / (60 * 1440)) as usize;
}

/* these next two functions help to implement elligator squared - http://eprint.iacr.org/2014/043 */
/* maps a random u to a point on the curve */
#[allow(non_snake_case)]
fn emap(u: &BIG, cb: isize) -> ECP {
    let mut P: ECP;
    let mut x = BIG::new_copy(u);
    let mut p = BIG::new_ints(&rom::MODULUS);
    x.rmod(&mut p);
    loop {
        P = ECP::new_bigint(&x, cb);
        if !P.is_infinity() {
            break;
        }
        x.inc(1);
        x.norm();
    }
    return P;
}

/* returns u derived from P. Random value in range 1 to return value should then be added to u */
#[allow(non_snake_case)]
fn unmap(u: &mut BIG, P: &mut ECP) -> isize {
    let s = P.gets();
    let mut R: ECP;
    let mut r = 0;
    let x = P.getx();
    u.copy(&x);
    loop {
        u.dec(1);
        u.norm();
        r += 1;
        R = ECP::new_bigint(u, s);
        if !R.is_infinity() {
            break;
        }
    }
    return r as isize;
}

pub fn hash_id(sha: usize, id: &[u8], w: &mut [u8]) -> bool {
    hmac::GPhashit(hmac::MC_SHA2,sha,w,EFS,None,-1,Some(id));
    return true;
}

/* these next two functions implement elligator squared - http://eprint.iacr.org/2014/043 */
/* Elliptic curve point E in format (0x04,x,y} is converted to form {0x0-,u,v} */
/* Note that u and v are indistinguisible from random strings */
#[allow(non_snake_case)]
pub fn encoding(rng: &mut RAND, e: &mut [u8]) -> isize {
    let mut t: [u8; EFS] = [0; EFS];

    for i in 0..EFS {
        t[i] = e[i + 1]
    }
    let mut u = BIG::frombytes(&t);
    for i in 0..EFS {
        t[i] = e[i + EFS + 1]
    }
    let mut v = BIG::frombytes(&t);

    let mut P = ECP::new_bigs(&u, &v);
    if P.is_infinity() {
        return INVALID_POINT;
    }

    let p = BIG::new_ints(&rom::MODULUS);
    u = BIG::randomnum(&p, rng);

    let mut su = rng.getbyte() as isize;
    su %= 2;

    let mut W = emap(&mut u, su);
    P.sub(&mut W);
    let sv = P.gets();
    let rn = unmap(&mut v, &mut P);
    let mut m = rng.getbyte() as isize;
    m %= rn;
    v.inc(m + 1);
    e[0] = (su + 2 * sv) as u8;
    u.tobytes(&mut t);
    for i in 0..EFS {
        e[i + 1] = t[i]
    }
    v.tobytes(&mut t);
    for i in 0..EFS {
        e[i + EFS + 1] = t[i]
    }

    return 0;
}

#[allow(non_snake_case)]
pub fn decoding(d: &mut [u8]) -> isize {
    let mut t: [u8; EFS] = [0; EFS];

    if (d[0] & 0x04) != 0 {
        return INVALID_POINT;
    }

    for i in 0..EFS {
        t[i] = d[i + 1]
    }
    let mut u = BIG::frombytes(&t);
    for i in 0..EFS {
        t[i] = d[i + EFS + 1]
    }
    let mut v = BIG::frombytes(&t);

    let su = (d[0] & 1) as isize;
    let sv = ((d[0] >> 1) & 1) as isize;
    let mut W = emap(&mut u, su);
    let mut P = emap(&mut v, sv);
    P.add(&mut W);
    u = P.getx();
    v = P.gety();
    d[0] = 0x04;
    u.tobytes(&mut t);
    for i in 0..EFS {
        d[i + 1] = t[i]
    }
    v.tobytes(&mut t);
    for i in 0..EFS {
        d[i + EFS + 1] = t[i]
    }

    return 0;
}

/* R=R1+R2 in group G1 */
#[allow(non_snake_case)]
pub fn recombine_g1(r1: &[u8], r2: &[u8], r: &mut [u8]) -> isize {
    let mut P = ECP::frombytes(&r1);
    let mut Q = ECP::frombytes(&r2);

    if P.is_infinity() || Q.is_infinity() {
        return INVALID_POINT;
    }

    P.add(&mut Q);

    P.tobytes(r, false);
    return 0;
}

/* W=W1+W2 in group G2 */
#[allow(non_snake_case)]
pub fn recombine_g2(w1: &[u8], w2: &[u8], w: &mut [u8]) -> isize {
    let mut P = ECP8::frombytes(&w1);
    let mut Q = ECP8::frombytes(&w2);

    if P.is_infinity() || Q.is_infinity() {
        return INVALID_POINT;
    }

    P.add(&mut Q);

    P.tobytes(w, false);
    return 0;
}

/* create random secret S */
pub fn random_generate(mut rng: &mut RAND, s: &mut [u8]) -> isize {
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let mut sc = BIG::randtrunc(&r, 16 * ecp::AESKEY, &mut rng);
    sc.tobytes(s);
    return 0;
}

/* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
#[allow(non_snake_case)]
pub fn get_server_secret(s: &[u8], sst: &mut [u8]) -> isize {
    let mut Q = ECP8::generator();

    let mut sc = BIG::frombytes(s);
    Q = pair256::g2mul(&mut Q, &mut sc);
    Q.tobytes(sst, false);
    return 0;
}

/*
 W=x*H(G);
 if RNG == NULL then X is passed in
 if RNG != NULL the X is passed out
 if type=0 W=x*G where G is point on the curve, else W=x*M(G), where M(G) is mapping of octet G to point on the curve
*/
#[allow(non_snake_case)]
pub fn get_g1_multiple(
    rng: Option<&mut RAND>,
    typ: usize,
    x: &mut [u8],
    g: &[u8],
    w: &mut [u8],
) -> isize {
    let mut sx: BIG;
    let r = BIG::new_ints(&rom::CURVE_ORDER);

    if let Some(rd) = rng {
        sx = BIG::randtrunc(&r, 16 * ecp::AESKEY, rd);
        sx.tobytes(x);
    } else {
        sx = BIG::frombytes(x);
    }
    let mut P: ECP;

    if typ == 0 {
        P = ECP::frombytes(g);
        if P.is_infinity() {
            return INVALID_POINT;
        }
    } else {
        P = ECP::mapit(g)
    }

    pair256::g1mul(&mut P, &mut sx).tobytes(w, false);
    return 0;
}

/* Client secret CST=S*H(CID) where CID is client ID and S is master secret */
/* CID is hashed externally */
pub fn get_client_secret(s: &mut [u8], cid: &[u8], cst: &mut [u8]) -> isize {
    return get_g1_multiple(None, 1, s, cid, cst);
}

/* Extract PIN from TOKEN for identity CID */
#[allow(non_snake_case)]
pub fn extract_pin(sha: usize, cid: &[u8], pin: i32, token: &mut [u8]) -> isize {
    return extract_factor(sha, cid, pin % MAXPIN, PBLEN, token);
}

/* Extract factor from TOKEN for identity CID */
#[allow(non_snake_case)]
pub fn extract_factor(
    sha: usize,
    cid: &[u8],
    factor: i32,
    facbits: i32,
    token: &mut [u8],
) -> isize {
    let mut P = ECP::frombytes(&token);
    const RM: usize = big::MODBYTES as usize;
    let mut h: [u8; RM] = [0; RM];
    if P.is_infinity() {
        return INVALID_POINT;
    }
    hmac::GPhashit(hmac::MC_SHA2,sha,&mut h,EFS,None,-1,Some(cid));
    let mut R = ECP::mapit(&h);

    R = R.pinmul(factor, facbits);
    P.sub(&mut R);

    P.tobytes(token, false);

    return 0;
}

/* Restore factor to TOKEN for identity CID */
#[allow(non_snake_case)]
pub fn restore_factor(
    sha: usize,
    cid: &[u8],
    factor: i32,
    facbits: i32,
    token: &mut [u8],
) -> isize {
    let mut P = ECP::frombytes(&token);
    const RM: usize = big::MODBYTES as usize;
    let mut h: [u8; RM] = [0; RM];
    if P.is_infinity() {
        return INVALID_POINT;
    }
    hmac::GPhashit(hmac::MC_SHA2,sha,&mut h,EFS,None,-1,Some(cid));
    let mut R = ECP::mapit(&h);

    R = R.pinmul(factor, facbits);
    P.add(&mut R);

    P.tobytes(token, false);

    return 0;
}

/* Functions to support M-Pin Full */
#[allow(non_snake_case)]
pub fn precompute(token: &[u8], cid: &[u8], g1: &mut [u8], g2: &mut [u8]) -> isize {
    let T = ECP::frombytes(&token);
    if T.is_infinity() {
        return INVALID_POINT;
    }

    let P = ECP::mapit(&cid);

    let Q = ECP8::generator();

    let mut g = pair256::ate(&Q, &T);
    g = pair256::fexp(&g);
    g.tobytes(g1);

    g = pair256::ate(&Q, &P);
    g = pair256::fexp(&g);
    g.tobytes(g2);

    return 0;
}

/* Time Permit CTT=S*(date|H(CID)) where S is master secret */
#[allow(non_snake_case)]
pub fn get_client_permit(sha: usize, date: usize, s: &[u8], cid: &[u8], ctt: &mut [u8]) -> isize {
    const RM: usize = big::MODBYTES as usize;
    let mut h: [u8; RM] = [0; RM];
    hmac::GPhashit(hmac::MC_SHA2,sha,&mut h,EFS,None,date as isize,Some(cid));
    let mut P = ECP::mapit(&h);

    let mut sc = BIG::frombytes(s);
    pair256::g1mul(&mut P, &mut sc).tobytes(ctt, false);
    return 0;
}

/* Implement step 1 on client side of MPin protocol */
#[allow(non_snake_case)]
pub fn client_1(
    sha: usize,
    date: usize,
    client_id: &[u8],
    rng: Option<&mut RAND>,
    x: &mut [u8],
    pin: usize,
    token: &[u8],
    sec: &mut [u8],
    xid: Option<&mut [u8]>,
    xcid: Option<&mut [u8]>,
    permit: Option<&[u8]>,
) -> isize {
    let r = BIG::new_ints(&rom::CURVE_ORDER);

    let mut sx: BIG;

    if let Some(rd) = rng {
        sx = BIG::randtrunc(&r, 16 * ecp::AESKEY, rd);
        sx.tobytes(x);
    } else {
        sx = BIG::frombytes(x);
    }

    const RM: usize = big::MODBYTES as usize;
    let mut h: [u8; RM] = [0; RM];

    hmac::GPhashit(hmac::MC_SHA2,sha, &mut h,EFS,None,-1,Some(client_id));
    let mut P = ECP::mapit(&h);

    let mut T = ECP::frombytes(&token);
    if T.is_infinity() {
        return INVALID_POINT;
    }

    let mut W = P.pinmul((pin as i32) % MAXPIN, PBLEN);
    T.add(&mut W);
    if date != 0 {
        if let Some(rpermit) = permit {
            W = ECP::frombytes(&rpermit);
        }
        if W.is_infinity() {
            return INVALID_POINT;
        }
        T.add(&mut W);
        let mut h2: [u8; RM] = [0; RM];
        hmac::GPhashit(hmac::MC_SHA2,sha,&mut h2,EFS,None,date as isize,Some(&h));
        W = ECP::mapit(&h2);
        if let Some(mut rxid) = xid {
            P = pair256::g1mul(&mut P, &mut sx);
            P.tobytes(&mut rxid, false);
            W = pair256::g1mul(&mut W, &mut sx);
            P.add(&mut W);
        } else {
            P.add(&mut W);
            P = pair256::g1mul(&mut P, &mut sx);
        }
        if let Some(mut rxcid) = xcid {
            P.tobytes(&mut rxcid, false)
        }
    } else {
        if let Some(mut rxid) = xid {
            P = pair256::g1mul(&mut P, &mut sx);
            P.tobytes(&mut rxid, false);
        }
    }

    T.tobytes(sec, false);
    return 0;
}

/* Outputs H(CID) and H(T|H(CID)) for time permits. If no time permits set HID=HTID */
#[allow(non_snake_case)]
pub fn server_1(sha: usize, date: usize, cid: &[u8], hid: &mut [u8], htid: Option<&mut [u8]>) {
    const RM: usize = big::MODBYTES as usize;
    let mut h: [u8; RM] = [0; RM];

    hmac::GPhashit(hmac::MC_SHA2,sha,&mut h,EFS,None,-1,Some(cid));

    let mut P = ECP::mapit(&h);

    P.tobytes(hid, false);
    if date != 0 {
        let mut h2: [u8; RM] = [0; RM];
        hmac::GPhashit(hmac::MC_SHA2,sha,&mut h2,EFS,None,date as isize,Some(&h));
        let mut R = ECP::mapit(&h2);
        P.add(&mut R);
        if let Some(rhtid) = htid {
            P.tobytes(rhtid, false);
        }
    }
}

/* Implement step 2 on client side of MPin protocol */
#[allow(non_snake_case)]
pub fn client_2(x: &[u8], y: &[u8], sec: &mut [u8]) -> isize {
    let mut r = BIG::new_ints(&rom::CURVE_ORDER);
    let mut P = ECP::frombytes(sec);
    if P.is_infinity() {
        return INVALID_POINT;
    }

    let mut px = BIG::frombytes(x);
    let py = BIG::frombytes(y);
    px.add(&py);
    px.rmod(&mut r);

    P = pair256::g1mul(&mut P, &mut px);
    P.neg();
    P.tobytes(sec, false);

    return 0;
}

/* return time since epoch */
pub fn get_time() -> usize {
    return (SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_secs()) as usize;
}

/* Generate Y = H(epoch, xCID/xID) */
pub fn get_y(sha: usize, timevalue: usize, xcid: &[u8], y: &mut [u8]) {
    const RM: usize = big::MODBYTES as usize;
    let mut h: [u8; RM] = [0; RM];

    hmac::GPhashit(hmac::MC_SHA2,sha,&mut h,EFS,None,timevalue as isize,Some(xcid));

    let mut sy = BIG::frombytes(&h);
    let mut q = BIG::new_ints(&rom::CURVE_ORDER);
    sy.rmod(&mut q);
    sy.tobytes(y);
}

/* Implement step 2 of MPin protocol on server side */
#[allow(non_snake_case)]
pub fn server_2(
    date: usize,
    hid: &[u8],
    htid: Option<&[u8]>,
    y: &[u8],
    sst: &[u8],
    xid: Option<&[u8]>,
    xcid: Option<&[u8]>,
    msec: &[u8],
    e: Option<&mut [u8]>,
    f: Option<&mut [u8]>,
) -> isize {
    let Q = ECP8::generator();

    let sQ = ECP8::frombytes(&sst);
    if sQ.is_infinity() {
        return INVALID_POINT;
    }

    let mut R: ECP;
    if date != 0 {
        if let Some(rxcid) = xcid {
            R = ECP::frombytes(&rxcid);
        } else {
            return BAD_PARAMS;
        }
    } else {
        if let Some(rxid) = xid {
            R = ECP::frombytes(&rxid)
        } else {
            return BAD_PARAMS;
        }
    }
    if R.is_infinity() {
        return INVALID_POINT;
    }

    let mut sy = BIG::frombytes(&y);
    let mut P: ECP;
    if date != 0 {
        if let Some(rhtid) = htid {
            P = ECP::frombytes(&rhtid)
        } else {
            return BAD_PARAMS;
        }
    } else {
        P = ECP::frombytes(&hid);
    }

    if P.is_infinity() {
        return INVALID_POINT;
    }

    P = pair256::g1mul(&mut P, &mut sy);
    P.add(&mut R);
    R = ECP::frombytes(&msec);
    if R.is_infinity() {
        return INVALID_POINT;
    }

    let mut g: FP48;

    g = pair256::ate2(&Q, &R, &sQ, &P);
    g = pair256::fexp(&g);

    if !g.isunity() {
        if let Some(rxid) = xid {
            if let Some(re) = e {
                if let Some(rf) = f {
                    g.tobytes(re);
                    if date != 0 {
                        P = ECP::frombytes(&hid);
                        if P.is_infinity() {
                            return INVALID_POINT;
                        }
                        R = ECP::frombytes(&rxid);
                        if R.is_infinity() {
                            return INVALID_POINT;
                        }
                        P = pair256::g1mul(&mut P, &mut sy);
                        P.add(&mut R); //P.affine();
                    }
                    g = pair256::ate(&Q, &P);
                    g = pair256::fexp(&g);
                    g.tobytes(rf);
                }
            }
        }

        return BAD_PIN;
    }

    return 0;
}

/* Pollards kangaroos used to return PIN error */
pub fn kangaroo(e: &[u8], f: &[u8]) -> isize {
    let mut ge = FP48::frombytes(e);
    let mut gf = FP48::frombytes(f);
    let mut distance: [isize; TS] = [0; TS];
    let mut t = FP48::new_copy(&gf);

    let mut table: [FP48; TS] = [FP48::new(); TS];
    let mut s: isize = 1;
    for m in 0..TS {
        distance[m] = s;
        table[m] = FP48::new_copy(&t);
        s *= 2;
        t.usqr();
    }
    t.one();
    let mut dn: isize = 0;
    let mut i: usize;
    for _ in 0..TRAP {
        i = (t.geta().geta().geta().geta().geta().lastbits(20) % (TS as isize)) as usize;
        t.mul(&mut table[i]);
        dn += distance[i];
    }
    gf.copy(&t);
    gf.conj();
    let mut steps: usize = 0;
    let mut dm: isize = 0;
    let mut res: isize = 0;
    while dm - dn < MAXPIN as isize {
        steps += 1;
        if steps > 4 * TRAP {
            break;
        }
        i = (ge.geta().geta().geta().geta().geta().lastbits(20) % (TS as isize)) as usize;
        ge.mul(&mut table[i]);
        dm += distance[i];
        if ge.equals(&mut t) {
            res = dm - dn;
            break;
        }
        if ge.equals(&mut gf) {
            res = dn - dm;
            break;
        }
    }
    if steps > 4 * TRAP || dm - dn >= MAXPIN as isize {
        res = 0
    } // Trap Failed  - probable invalid token
    return res;
}

/* Hash the M-Pin transcript - new */

pub fn hash_all(
    sha: usize,
    hid: &[u8],
    xid: &[u8],
    xcid: Option<&[u8]>,
    sec: &[u8],
    y: &[u8],
    r: &[u8],
    w: &[u8],
    h: &mut [u8],
) -> bool {
    let mut tlen: usize = 0;
    const RM: usize = big::MODBYTES as usize;
    let mut t: [u8; 10 * RM + 4] = [0; 10 * RM + 4];

    for i in 0..hid.len() {
        t[i] = hid[i]
    }
    tlen += hid.len();

    if let Some(rxcid) = xcid {
        for i in 0..rxcid.len() {
            t[i + tlen] = rxcid[i]
        }
        tlen += rxcid.len();
    } else {
        for i in 0..xid.len() {
            t[i + tlen] = xid[i]
        }
        tlen += xid.len();
    }

    for i in 0..sec.len() {
        t[i + tlen] = sec[i]
    }
    tlen += sec.len();
    for i in 0..y.len() {
        t[i + tlen] = y[i]
    }
    tlen += y.len();
    for i in 0..r.len() {
        t[i + tlen] = r[i]
    }
    tlen += r.len();
    for i in 0..w.len() {
        t[i + tlen] = w[i]
    }
    tlen += w.len();
    if tlen != 10 * RM + 4 {
        return false;
    }

    hmac::GPhashit(hmac::MC_SHA2,sha,h,EFS,None,-1,Some(&t));
    return true;
}

/* calculate common key on client side */
/* wCID = w.(A+AT) */
#[allow(non_snake_case)]
pub fn client_key(
    sha: usize,
    g1: &[u8],
    g2: &[u8],
    pin: usize,
    r: &[u8],
    x: &[u8],
    h: &[u8],
    wcid: &[u8],
    ck: &mut [u8],
) -> isize {
    let mut g1 = FP48::frombytes(&g1);
    let mut g2 = FP48::frombytes(&g2);
    let mut z = BIG::frombytes(&r);
    let mut x = BIG::frombytes(&x);
    let h = BIG::frombytes(&h);

    let mut W = ECP::frombytes(&wcid);
    if W.is_infinity() {
        return INVALID_POINT;
    }

    W = pair256::g1mul(&mut W, &mut x);

    let mut r = BIG::new_ints(&rom::CURVE_ORDER);

    z.add(&h); //new
    z.rmod(&mut r);

    g2.pinpow(pin as i32, PBLEN);
    g1.mul(&mut g2);

    g1 = pair256::gtpow(&g1,&z);
    let mut c = g1.trace();
    //let mut c = g1.compow(&z, &mut r);

    hash(sha, &mut c, &mut W, ck);

    return 0;
}

/* calculate common key on server side */
/* Z=r.A - no time permits involved */
#[allow(non_snake_case)]
pub fn server_key(
    sha: usize,
    z: &[u8],
    sst: &[u8],
    w: &[u8],
    h: &[u8],
    hid: &[u8],
    xid: &[u8],
    xcid: Option<&[u8]>,
    sk: &mut [u8],
) -> isize {
    let sQ = ECP8::frombytes(&sst);
    if sQ.is_infinity() {
        return INVALID_POINT;
    }
    let mut R = ECP::frombytes(&z);
    if R.is_infinity() {
        return INVALID_POINT;
    }
    let mut A = ECP::frombytes(&hid);
    if A.is_infinity() {
        return INVALID_POINT;
    }

    let mut U = ECP::new();
    if let Some(rxcid) = xcid {
        U.copy(&ECP::frombytes(&rxcid));
    } else {
        U.copy(&ECP::frombytes(&xid));
    }

    if U.is_infinity() {
        return INVALID_POINT;
    }

    let mut w = BIG::frombytes(&w);
    let mut h = BIG::frombytes(&h);
    A = pair256::g1mul(&mut A, &mut h); // new
    R.add(&mut A);

    U = pair256::g1mul(&mut U, &mut w);
    let mut g = pair256::ate(&sQ, &R);
    g = pair256::fexp(&g);

    let mut c = g.trace();

    hash(sha, &mut c, &mut U, sk);

    return 0;
}
