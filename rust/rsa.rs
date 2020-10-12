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

use crate::xxx::big;
use crate::xxx::ff;
use crate::xxx::ff::FF;

use crate::rand::RAND;

pub const RFS: usize = (big::MODBYTES as usize) * ff::FFLEN;
pub const SHA256: usize = 32;
pub const SHA384: usize = 48;
pub const SHA512: usize = 64;

pub const HASH_TYPE: usize = SHA256;

pub struct RsaPrivateKey {
    p: FF,
    q: FF,
    dp: FF,
    dq: FF,
    c: FF,
}

pub struct RsaPublicKey {
    e: isize,
    n: FF,
}

pub fn new_private_key(n: usize) -> RsaPrivateKey {
    RsaPrivateKey {
        p: FF::new_int(n),
        q: FF::new_int(n),
        dp: FF::new_int(n),
        dq: FF::new_int(n),
        c: FF::new_int(n),
    }
}

pub fn new_public_key(m: usize) -> RsaPublicKey {
    RsaPublicKey {
        e: 0,
        n: FF::new_int(m),
    }
}

pub fn key_pair(rng: &mut RAND, e: isize, prv: &mut RsaPrivateKey, pbc: &mut RsaPublicKey) {
    /* IEEE1363 A16.11/A16.12 more or less */
    let n = pbc.n.getlen() / 2;
    let mut t = FF::new_int(n);
    let mut p1 = FF::new_int(n);
    let mut q1 = FF::new_int(n);

    loop {
        prv.p.random(rng);
        while prv.p.lastbits(2) != 3 {
            prv.p.inc(1)
        }
        while !FF::prime(&prv.p, rng) {
            prv.p.inc(4);
        }

        p1.copy(&prv.p);
        p1.dec(1);

        if p1.cfactor(e) {
            continue;
        }
        break;
    }

    loop {
        prv.q.random(rng);
        while prv.q.lastbits(2) != 3 {
            prv.q.inc(1)
        }
        while !FF::prime(&prv.q, rng) {
            prv.q.inc(4);
        }

        q1.copy(&prv.q);
        q1.dec(1);

        if q1.cfactor(e) {
            continue;
        }

        break;
    }

    pbc.n = FF::mul(&prv.p, &prv.q);
    pbc.e = e;

    t.copy(&p1);
    t.shr();
    prv.dp.set(e);
    prv.dp.invmodp(&t);
    if prv.dp.parity() == 0 {
        prv.dp.add(&t)
    }
    prv.dp.norm();

    t.copy(&q1);
    t.shr();
    prv.dq.set(e);
    prv.dq.invmodp(&t);
    if prv.dq.parity() == 0 {
        prv.dq.add(&t)
    }
    prv.dq.norm();

    prv.c.copy(&prv.p);
    prv.c.invmodp(&prv.q);
}


/* destroy the Private Key structure */
pub fn private_key_kill(prv: &mut RsaPrivateKey) {
    prv.p.zero();
    prv.q.zero();
    prv.dp.zero();
    prv.dq.zero();
    prv.c.zero();
}

/* RSA encryption with the public key */
pub fn encrypt(pbc: &RsaPublicKey, f: &[u8], g: &mut [u8]) {
    let m = pbc.n.getlen();
    let mut r = FF::new_int(m);

    FF::frombytes(&mut r, f);
    r.power(pbc.e, &pbc.n);
    r.tobytes(g);
}

/* RSA decryption with the private key */
pub fn decrypt(prv: &RsaPrivateKey, g: &[u8], f: &mut [u8]) {
    let n = prv.p.getlen();
    let mut r = FF::new_int(2 * n);

    FF::frombytes(&mut r, g);
    let mut jp = r.dmod(&prv.p);
    let mut jq = r.dmod(&prv.q);

    jp.skpow(&prv.dp, &prv.p);
    jq.skpow(&prv.dq, &prv.q);

    r.zero();
    r.dscopy(&jp);
    jp.rmod(&prv.q);
    if FF::comp(&jp, &jq) > 0 {
        jq.add(&prv.q)
    }
    jq.sub(&jp);
    jq.norm();

    let mut t = FF::mul(&prv.c, &jq);
    jq = t.dmod(&prv.q);

    t = FF::mul(&jq, &prv.p);
    r.add(&t);
    r.norm();

    r.tobytes(f);
}
