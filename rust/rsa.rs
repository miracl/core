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
use crate::xxx::ff::SF;
use crate::xxx::ff::DF;

use crate::rand::RAND;

pub const RFS: usize = (big::MODBYTES as usize) * ff::FFLEN;
pub const SHA256: usize = 32;
pub const SHA384: usize = 48;
pub const SHA512: usize = 64;

pub const SL: usize = ff::SL;
pub const DL: usize = ff::DL;

pub const HASH_TYPE: usize = SHA256;

pub struct RsaPrivateKey {
    p: SF,
    q: SF,
    dp: SF,
    dq: SF,
    c: SF,
}

pub struct RsaPublicKey {
    e: isize,
    n: DF,
}

pub fn new_private_key() -> RsaPrivateKey {
    RsaPrivateKey {
        p: SF::new(),
        q: SF::new(),
        dp: SF::new(),
        dq: SF::new(),
        c: SF::new(),
    }
}

pub fn new_public_key() -> RsaPublicKey {
    RsaPublicKey {
        e: 0,
        n: DF::new(),
    }
}

pub fn set_public_key(pk: &mut RsaPublicKey,e: isize, f: &[u8]) {
    pk.e=e;
    let mut r = DF::new();
    r.frombytes(f);
    pk.n.copy(&r);
}

// Input private key from OpenSSL format
// e.g as in openssl rsa -in privkey.pem -noout -text
// Note order swap - For MIRACL c=1/p mod q, for OpenSSL c=1/q mod p
pub fn rsa_private_key_from_openssl(p: &[u8],q: &[u8],dp: &[u8],dq: &[u8],c: &[u8], prv: &mut RsaPrivateKey) {
    prv.p.frombytes(q);
    prv.q.frombytes(p);
    prv.dp.frombytes(dq);
    prv.dq.frombytes(dp);
    prv.c.frombytes(c);    
}

pub fn rsa_private_key_to_openssl(prv: &RsaPrivateKey,p: &mut [u8],q: &mut [u8],dp: &mut [u8],dq: &mut [u8],c: &mut [u8]) {
    let mut t = SF::new();
    t.copy(&prv.p); t.tobytes(q);
    t.copy(&prv.q); t.tobytes(p);
    t.copy(&prv.dp); t.tobytes(dq);
    t.copy(&prv.dq); t.tobytes(dp);
    t.copy(&prv.c); t.tobytes(c);
}

pub fn key_pair_from_openssl(e: isize, p: &[u8],q: &[u8],dp: &[u8],dq: &[u8],c: &[u8], prv: &mut RsaPrivateKey, pbc: &mut RsaPublicKey) {
    rsa_private_key_from_openssl(p,q,dp,dq,c,prv);
    pbc.n = prv.p.mul(&prv.q);
    pbc.e = e;
}

pub fn key_pair(rng: &mut RAND, e: isize, prv: &mut RsaPrivateKey, pbc: &mut RsaPublicKey) {
    /* IEEE1363 A16.11/A16.12 more or less */
    let mut t = SF::new();
    let mut p1 = SF::new();
    let mut q1 = SF::new();

    loop {
        prv.p.random(rng);
        while prv.p.lastbits(2) != 3 {
            prv.p.inc(1)
        }
        while !prv.p.isprime(rng) {
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
        while !prv.q.isprime(rng) {
            prv.q.inc(4);
        }

        q1.copy(&prv.q);
        q1.dec(1);

        if q1.cfactor(e) {
            continue;
        }

        break;
    }

    pbc.n = prv.p.mul(&prv.q);
    pbc.e = e;

// Note this only works for the 3 mod 4 primes generated as above.

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
    let mut r = DF::new();
    r.frombytes(f);
    r.power(pbc.e, &pbc.n);
    r.tobytes(g);
}

/* RSA decryption with the private key */
pub fn decrypt(prv: &RsaPrivateKey, g: &[u8], f: &mut [u8]) {
    let mut r = DF::new();

    r.frombytes(g);
    let mut jp = r.dmod(&prv.p);
    let mut jq = r.dmod(&prv.q);

    jp.skpow(&prv.dp, &prv.p);
    jq.skpow(&prv.dq, &prv.q);

    r.zero();
    r.dscopy(&jp);
    jp.rmod(&prv.q);
    if jp.comp(&jq) > 0 {
        jq.add(&prv.q)
    }
    jq.sub(&jp);
    jq.norm();

    let mut t = prv.c.mul(&jq);
    jq = t.dmod(&prv.q);

    t = jq.mul(&prv.p);
    r.add(&t);
    r.norm();

    r.tobytes(f);
}
