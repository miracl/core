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

/* ECDH/ECIES/ECDSA API Functions */

use crate::xxx::big;
use crate::xxx::big::BIG;
use crate::xxx::ecp;
use crate::xxx::ecp::ECP;
use crate::xxx::rom;

use crate::aes;
use crate::hmac;
use crate::rand::RAND;

pub const INVALID_PUBLIC_KEY: isize = -2;
pub const ERROR: isize = -3;
//pub const INVALID: isize = -4;
pub const EFS: usize = big::MODBYTES as usize;
pub const EGS: usize = big::MODBYTES as usize;

#[allow(non_snake_case)]
pub fn in_range(s: &[u8]) -> bool {
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let sc = BIG::frombytes(&s);
    if sc.iszilch() {
        return false;
    }
    if BIG::comp(&sc, &r) >= 0 {
        return false;
    }
    return true;
}

/* Calculate a public/private EC GF(p) key pair w,s where W=s.G mod EC(p),
 * where s is the secret key and W is the public key
 * and G is fixed generator.
 * If RNG is NULL then the private key is provided externally in s
 * otherwise it is generated randomly internally */
#[allow(non_snake_case)]
pub fn key_pair_generate(rng: Option<&mut RAND>, s: &mut [u8], w: &mut [u8]) -> isize {
    let res = 0;
    let mut sc: BIG;
    let G = ECP::generator();

    let r = BIG::new_ints(&rom::CURVE_ORDER);

    if let Some(mut x) = rng {
        sc = BIG::randtrunc(&r, 16 * ecp::AESKEY, &mut x);
    } else {
        sc = BIG::frombytes(&s);
        sc.rmod(&r);
    }

    sc.tobytes(s);

    let WP = G.mul(&mut sc);

    WP.tobytes(w, false); // To use point compression on public keys, change to true

    return res;
}

/* validate public key */
#[allow(non_snake_case)]
pub fn public_key_validate(w: &[u8]) -> isize {
    let mut WP = ECP::frombytes(w);
    let mut res = 0;

    let r = BIG::new_ints(&rom::CURVE_ORDER);

    if WP.is_infinity() {
        res = INVALID_PUBLIC_KEY
    }
    if res == 0 {
        let q = BIG::new_ints(&rom::MODULUS);
        let nb = q.nbits();
        let mut k = BIG::new();
        k.one();
        k.shl((nb + 4) / 2);
        k.add(&q);
        k.div(&r);

        while k.parity() == 0 {
            k.shr(1);
            WP.dbl();
        }

        if !k.isunity() {
            WP = WP.mul(&mut k)
        }
        if WP.is_infinity() {
            res = INVALID_PUBLIC_KEY
        }
    }
    return res;
}

/* IEEE-1363 Diffie-Hellman online calculation Z=S.WD */
#[allow(non_snake_case)]
pub fn ecpsvdp_dh(s: &[u8], wd: &[u8], z: &mut [u8], typ: isize) -> isize {
    let mut res = 0;

    let mut sc = BIG::frombytes(&s);

    let mut W = ECP::frombytes(&wd);
    if W.is_infinity() {
        res = ERROR
    }

    if res == 0 {
        let r = BIG::new_ints(&rom::CURVE_ORDER);
        sc.rmod(&r);
        W = W.mul(&mut sc);
        if W.is_infinity() {
            res = ERROR;
        } else {
            if ecp::CURVETYPE != ecp::MONTGOMERY {
                if typ>0 {
                    if typ==1 {
                        W.tobytes(z,true);
                    } else {
                        W.tobytes(z,false);
                    }
                } else {
                    W.getx().tobytes(z);
                }
                return res;
            } else {
                W.getx().tobytes(z);
            }
        }
    }
    return res;
}

/* IEEE ECDSA Signature, C and D are signature on F using private key S */
#[allow(non_snake_case)]
pub fn ecpsp_dsa(
    sha: usize,
    rng: &mut RAND,
    s: &[u8],
    f: &[u8],
    c: &mut [u8],
    d: &mut [u8],
) -> isize {
    let mut t: [u8; EFS] = [0; EFS];
    let mut b: [u8; big::MODBYTES as usize] = [0; big::MODBYTES as usize];

    hmac::GPhashit(hmac::MC_SHA2, sha, &mut b, big::MODBYTES as usize,0,Some(f), -1, None);

    let G = ECP::generator();

    let r = BIG::new_ints(&rom::CURVE_ORDER);

    let mut sc = BIG::frombytes(s); /* s or &s? */
    let fb = BIG::frombytes(&b);

    let mut cb = BIG::new();
    let mut db = BIG::new();
    let mut tb = BIG::new();
    let mut V = ECP::new();

    while db.iszilch() {
        let mut u = BIG::randomnum(&r, rng);
        let mut w = BIG::randomnum(&r, rng); /* side channel masking */

        V.copy(&G);
        V = V.mul(&mut u);
        let vx = V.getx();
        cb.copy(&vx);
        cb.rmod(&r);
        if cb.iszilch() {
            continue;
        }

        tb.copy(&BIG::modmul(&mut u, &mut w, &r));
        u.copy(&tb);

        u.invmodp(&r);
        db.copy(&BIG::modmul(&mut sc, &mut cb, &r));
        db.add(&fb);

        tb.copy(&BIG::modmul(&mut db, &mut w, &r));
        db.copy(&tb);

        tb.copy(&BIG::modmul(&mut u, &mut db, &r));
        db.copy(&tb);
    }

    cb.tobytes(&mut t);
    for i in 0..EFS {
        c[i] = t[i]
    }
    db.tobytes(&mut t);
    for i in 0..EFS {
        d[i] = t[i]
    }
    return 0;
}

/* IEEE1363 ECDSA Signature Verification. Signature C and D on F is verified using public key W */
#[allow(non_snake_case)]
pub fn ecpvp_dsa(sha: usize, w: &[u8], f: &[u8], c: &[u8], d: &[u8]) -> isize {
    let mut res = 0;

    let mut b: [u8; big::MODBYTES as usize] = [0; big::MODBYTES as usize];

    hmac::GPhashit(hmac::MC_SHA2, sha, &mut b, big::MODBYTES as usize, 0,Some(f), -1, None);

    let mut G = ECP::generator();

    let r = BIG::new_ints(&rom::CURVE_ORDER);

    let mut cb = BIG::frombytes(c); /* c or &c ? */
    let mut db = BIG::frombytes(d); /* d or &d ? */
    let mut fb = BIG::frombytes(&b);
    let mut tb = BIG::new();

    if cb.iszilch() || BIG::comp(&cb, &r) >= 0 || db.iszilch() || BIG::comp(&db, &r) >= 0 {
        res = ERROR;
    }

    if res == 0 {
        db.invmodp(&r);
        tb.copy(&BIG::modmul(&mut fb, &mut db, &r));
        fb.copy(&tb);
        let h2 = BIG::modmul(&mut cb, &mut db, &r);

        let WP = ECP::frombytes(&w);
        if WP.is_infinity() {
            res = ERROR;
        } else {
            let mut P = ECP::new();
            P.copy(&WP);

            P = P.mul2(&h2, &mut G, &fb);

            if P.is_infinity() {
                res = ERROR;
            } else {
                db = P.getx();
                db.rmod(&r);

                if BIG::comp(&db, &cb) != 0 {
                    res = ERROR
                }
            }
        }
    }

    return res;
}

/* IEEE1363 ECIES encryption. Encryption of plaintext M uses public key W and produces ciphertext V,C,T */
#[allow(non_snake_case)]
pub fn ecies_encrypt(
    sha: usize,
    p1: &[u8],
    p2: &[u8],
    rng: &mut RAND,
    w: &[u8],
    m: &[u8],
    v: &mut [u8],
    t: &mut [u8],
) -> Option<Vec<u8>> {
    let mut z: [u8; EFS] = [0; EFS];
    let mut k1: [u8; ecp::AESKEY] = [0; ecp::AESKEY];
    let mut k2: [u8; ecp::AESKEY] = [0; ecp::AESKEY];
    let mut u: [u8; EGS] = [0; EGS];
    let mut vz: [u8; 3 * EFS + 1] = [0; 3 * EFS + 1];
    let mut k: [u8; 2 * ecp::AESKEY] = [0; 2 * ecp::AESKEY];

    if key_pair_generate(Some(rng), &mut u, v) != 0 {
        return None;
    }
    if ecpsvdp_dh(&u, &w, &mut z, 0) != 0 {
        return None;
    }

    for i in 0..2 * EFS + 1 {
        vz[i] = v[i]
    }
    for i in 0..EFS {
        vz[2 * EFS + 1 + i] = z[i]
    }

    hmac::kdf2(hmac::MC_SHA2, sha, &vz, Some(p1), 2 * ecp::AESKEY, &mut k);

    for i in 0..ecp::AESKEY {
        k1[i] = k[i];
        k2[i] = k[ecp::AESKEY + i]
    }

    let mut c = aes::cbc_iv0_encrypt(&k1, m);

    let mut l2: [u8; 8] = [0; 8];
    let p2l = p2.len();

    hmac::inttobytes(p2l, &mut l2);

    for i in 0..p2l {
        c.push(p2[i]);
    }
    for i in 0..8 {
        c.push(l2[i]);
    }

    hmac::hmac1(hmac::MC_SHA2, sha, t, t.len(), &k2, &c);

    for _ in 0..p2l + 8 {
        c.pop();
    }

    return Some(c);
}

/* constant time n-byte compare */
fn ncomp(t1: &[u8], t2: &[u8], n: usize) -> bool {
    let mut res = 0;
    for i in 0..n {
        res |= (t1[i] ^ t2[i]) as isize;
    }
    if res == 0 {
        return true;
    }
    return false;
}

/* IEEE1363 ECIES decryption. Decryption of ciphertext V,C,T using private key U outputs plaintext M */
#[allow(non_snake_case)]
pub fn ecies_decrypt(
    sha: usize,
    p1: &[u8],
    p2: &[u8],
    v: &[u8],
    c: &mut Vec<u8>,
    t: &[u8],
    u: &[u8],
) -> Option<Vec<u8>> {
    let mut z: [u8; EFS] = [0; EFS];
    let mut k1: [u8; ecp::AESKEY] = [0; ecp::AESKEY];
    let mut k2: [u8; ecp::AESKEY] = [0; ecp::AESKEY];
    let mut vz: [u8; 3 * EFS + 1] = [0; 3 * EFS + 1];
    let mut k: [u8; 2 * ecp::AESKEY] = [0; 2 * ecp::AESKEY];

    let mut tag: [u8; 32] = [0; 32]; /* 32 is max length of tag */

    for i in 0..t.len() {
        tag[i] = t[i]
    }

    if ecpsvdp_dh(&u, &v, &mut z, 0) != 0 {
        return None;
    }

    for i in 0..2 * EFS + 1 {
        vz[i] = v[i]
    }
    for i in 0..EFS {
        vz[2 * EFS + 1 + i] = z[i]
    }

    hmac::kdf2(hmac::MC_SHA2, sha, &vz, Some(p1), 2 * ecp::AESKEY, &mut k);

    for i in 0..ecp::AESKEY {
        k1[i] = k[i];
        k2[i] = k[ecp::AESKEY + i]
    }

    let m = aes::cbc_iv0_decrypt(&k1, &c);

    if m == None {
        return None;
    }

    let mut l2: [u8; 8] = [0; 8];
    let p2l = p2.len();

    hmac::inttobytes(p2l, &mut l2);

    for i in 0..p2l {
        c.push(p2[i]);
    }
    for i in 0..8 {
        c.push(l2[i]);
    }

    let tl=tag.len();
    hmac::hmac1(hmac::MC_SHA2, sha, &mut tag, tl, &k2, &c);

    for _ in 0..p2l + 8 {
        c.pop();
    }

    if !ncomp(&t, &tag, t.len()) {
        return None;
    }

    return m;
}
