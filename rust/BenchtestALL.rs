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
#![allow(non_snake_case)]
extern crate core;

//use std::str;
//use std::io;

use core::arch;
use core::rand::RAND;

use std::time::Instant;

const MIN_ITERS: isize = 10;
const MIN_TIME: isize = 10;

fn ed25519(mut rng: &mut RAND) {
    //use core::ed25519;
    use core::ed25519::big;
    use core::ed25519::ecp;
    use core::ed25519::fp;
    use core::ed25519::rom;
    let mut fail = false;

    println!("\nTesting/Timing ed25519 ECC");

    if ecp::CURVETYPE == ecp::WEIERSTRASS {
        println!("Weierstrass parameterization");
    }
    if ecp::CURVETYPE == ecp::EDWARDS {
        println!("Edwards parameterization");
    }
    if ecp::CURVETYPE == ecp::MONTGOMERY {
        println!("Montgomery parameterization");
    }

    if fp::MODTYPE == fp::PSEUDO_MERSENNE {
        println!("Pseudo-Mersenne Modulus");
    }
    if fp::MODTYPE == fp::MONTGOMERY_FRIENDLY {
        println!("Montgomery friendly Modulus");
    }
    if fp::MODTYPE == fp::GENERALISED_MERSENNE {
        println!("Generalised-Mersenne Modulus");
    }
    if fp::MODTYPE == fp::NOT_SPECIAL {
        println!("Not special Modulus");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    
    let mut P = ecp::ECP::new();
    let s = big::BIG::randomnum(&r, &mut rng);
    for _ in 0..10 {
        P.copy(&ecp::ECP::map2point(&fp::FP::new_rand(&mut rng)));
        if P.is_infinity() {
            println!("HASH FAILURE - P=O");
            fail = true;
        }
    }

    P = G.mul(&r);
    if !P.is_infinity() {
        println!("FAILURE - rG!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = G.mul(&s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("EC  mul - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);
    if !fail {
        println!("All tests pass");
    }
}

fn nist256(mut rng: &mut RAND) {
    //use core::nist256;
    use core::nist256::big;
    use core::nist256::ecp;
    use core::nist256::fp;
    use core::nist256::rom;
    let mut fail = false;
    println!("\nTesting/Timing nist256 ECC");

    if ecp::CURVETYPE == ecp::WEIERSTRASS {
        println!("Weierstrass parameterization");
    }
    if ecp::CURVETYPE == ecp::EDWARDS {
        println!("Edwards parameterization");
    }
    if ecp::CURVETYPE == ecp::MONTGOMERY {
        println!("Montgomery parameterization");
    }

    if fp::MODTYPE == fp::PSEUDO_MERSENNE {
        println!("Pseudo-Mersenne Modulus");
    }
    if fp::MODTYPE == fp::MONTGOMERY_FRIENDLY {
        println!("Montgomery friendly Modulus");
    }
    if fp::MODTYPE == fp::GENERALISED_MERSENNE {
        println!("Generalised-Mersenne Modulus");
    }
    if fp::MODTYPE == fp::NOT_SPECIAL {
        println!("Not special Modulus");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);

    let mut P = ecp::ECP::new();
    let s = big::BIG::randomnum(&r, &mut rng);
    for _ in 0..10 {
        P.copy(&ecp::ECP::map2point(&fp::FP::new_rand(&mut rng)));
        if P.is_infinity() {
            println!("HASH FAILURE - P=O");
            fail = true;
        }
    }
    
    P = G.mul(&r);
    if !P.is_infinity() {
        println!("FAILURE - rG!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = G.mul(&s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("EC  mul - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);
    if !fail {
        println!("All tests pass");
    }
}

fn goldilocks(mut rng: &mut RAND) {
    //use core::goldilocks;
    use core::goldilocks::big;
    use core::goldilocks::ecp;
    use core::goldilocks::fp;
    use core::goldilocks::rom;
    let mut fail = false;
    println!("\nTesting/Timing goldilocks ECC");

    if ecp::CURVETYPE == ecp::WEIERSTRASS {
        println!("Weierstrass parameterization");
    }
    if ecp::CURVETYPE == ecp::EDWARDS {
        println!("Edwards parameterization");
    }
    if ecp::CURVETYPE == ecp::MONTGOMERY {
        println!("Montgomery parameterization");
    }

    if fp::MODTYPE == fp::PSEUDO_MERSENNE {
        println!("Pseudo-Mersenne Modulus");
    }
    if fp::MODTYPE == fp::MONTGOMERY_FRIENDLY {
        println!("Montgomery friendly Modulus");
    }
    if fp::MODTYPE == fp::GENERALISED_MERSENNE {
        println!("Generalised-Mersenne Modulus");
    }
    if fp::MODTYPE == fp::NOT_SPECIAL {
        println!("Not special Modulus");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);

    let mut P = ecp::ECP::new();
    let s = big::BIG::randomnum(&r, &mut rng);
    for _ in 0..10 {
        P.copy(&ecp::ECP::map2point(&fp::FP::new_rand(&mut rng)));
        if P.is_infinity() {
            println!("HASH FAILURE - P=O");
            fail = true;
        }
    }

    P = G.mul(&r);
    if !P.is_infinity() {
        println!("FAILURE - rG!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = G.mul(&s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("EC  mul - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);
    if !fail {
        println!("All tests pass");
    }
}

fn bn254(mut rng: &mut RAND) {
    //use core::bn254;
    use core::bn254::big;
    use core::bn254::ecp;
    use core::bn254::ecp2;
    use core::bn254::fp;
    use core::bn254::fp2;
    use core::bn254::pair;
    use core::bn254::rom;
    let mut fail = false;
    println!("\nTesting/Timing bn254 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE > ecp::BN {
        println!("BLS Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::map2point(&fp::FP::new_rand(&mut rng));
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
    } 
    
    P = pair::g1mul(&G, &r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair::g1mul(&G, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp2::ECP2::generator();

    let mut W = ecp2::ECP2::map2point(&fp2::FP2::new_rand(&mut rng));
    W.cfp();
    if W.is_infinity() {
        println!("HASHING FAILURE - P=O");
		fail=true;
    }
    W = pair::g2mul(&W, &r);
    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
		fail=true;
    }


    W = pair::g2mul(&Q, &r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair::g2mul(&Q, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair::ate(&Q, &P);
    w = pair::fexp(&w);

    let mut g = pair::gtpow(&w, &r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair::gtpow(&w, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("GT  pow              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = w.compow(&s, &r);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("GT  pow (compressed) - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        w = pair::ate(&Q, &P);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing ATE          - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair::fexp(&w);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing FEXP         - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    P.copy(&G);
    Q.copy(&W);

    P = pair::g1mul(&P, &s);
    g = pair::ate(&Q, &P);
    g = pair::fexp(&g);

    P.copy(&G);
    Q = pair::g2mul(&Q, &s);
    w = pair::ate(&Q, &P);
    w = pair::fexp(&w);

    if !pair::g1member(&P) {
        println!("FAILURE - P is not in G1 ");
        fail=true;
    }

    if !pair::g2member(&Q) {
        println!("FAILURE - Q is not in G2 ");
        fail=true;
    }

    if !pair::gtmember(&w) {
        println!("FAILURE - e(Q,P) is not in GT ");
        fail=true;
    }

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair::ate(&Q, &P);
    g = pair::fexp(&g);
    g = pair::gtpow(&g, &s);

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        fail = true;
    }
    if !fail {
        println!("All tests pass");
    }
}

fn bls12383(mut rng: &mut RAND) {
    //use core::bls12383;
    use core::bls12383::big;
    use core::bls12383::ecp;
    use core::bls12383::ecp2;
    use core::bls12383::fp;
    use core::bls12383::fp2;
    use core::bls12383::pair;
    use core::bls12383::rom;

    let mut fail = false;
    println!("\nTesting/Timing bls12383 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE > ecp::BN {
        println!("BLS Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::map2point(&fp::FP::new_rand(&mut rng));
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
    }
    
    P = pair::g1mul(&G, &r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair::g1mul(&G, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp2::ECP2::generator();

    let mut W = ecp2::ECP2::map2point(&fp2::FP2::new_rand(&mut rng));
    W.cfp();
    if W.is_infinity() {
        println!("HASHING FAILURE - P=O");
		fail=true;
    }
    W = pair::g2mul(&W, &r);
    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
		fail=true;
    }

    W = pair::g2mul(&Q, &r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair::g2mul(&Q, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair::ate(&Q, &P);
    w = pair::fexp(&w);

    let mut g = pair::gtpow(&w, &r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair::gtpow(&w, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("GT  pow              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = w.compow(&s, &r);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("GT  pow (compressed) - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        w = pair::ate(&Q, &P);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing ATE          - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair::fexp(&w);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing FEXP         - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    P.copy(&G);
    Q.copy(&W);

    P = pair::g1mul(&P, &s);
    g = pair::ate(&Q, &P);
    g = pair::fexp(&g);

    P.copy(&G);
    Q = pair::g2mul(&Q, &s);
    w = pair::ate(&Q, &P);
    w = pair::fexp(&w);

    if !pair::g1member(&P) {
        println!("FAILURE - P is not in G1 ");
        fail=true;
    }

    if !pair::g2member(&Q) {
        println!("FAILURE - Q is not in G2 ");
        fail=true;
    }

    if !pair::gtmember(&w) {
        println!("FAILURE - e(Q,P) is not in GT ");
        fail=true;
    }

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair::ate(&Q, &P);
    g = pair::fexp(&g);
    g = pair::gtpow(&g, &s);

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        fail = true;
    }
    if !fail {
        println!("All tests pass");
    }
}

fn bls24479(mut rng: &mut RAND) {
    //use core::bls24479;
    use core::bls24479::big;
    use core::bls24479::ecp;
    use core::bls24479::ecp4;
    use core::bls24479::fp;
    use core::bls24479::fp4;
    use core::bls24479::pair4;
    use core::bls24479::rom;
    let mut fail = false;
    println!("\nTesting/Timing bls24479 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE > ecp::BN {
        println!("BLS24 Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::map2point(&fp::FP::new_rand(&mut rng));
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
    }
    
    P = pair4::g1mul(&G, &r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair4::g1mul(&G, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp4::ECP4::generator();

    let mut W = ecp4::ECP4::map2point(&fp4::FP4::new_rand(&mut rng));
    W.cfp();
    if W.is_infinity() {
        println!("HASHING FAILURE - P=O");
		fail=true;
    }
    W = pair4::g2mul(&W, &r);
    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
		fail=true;
    }

    W = pair4::g2mul(&Q, &r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair4::g2mul(&Q, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair4::ate(&Q, &P);
    w = pair4::fexp(&w);

    let mut g = pair4::gtpow(&w, &r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair4::gtpow(&w, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("GT  pow              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        w = pair4::ate(&Q, &P);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing ATE          - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair4::fexp(&w);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing FEXP         - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    P.copy(&G);
    Q.copy(&W);

    P = pair4::g1mul(&P, &s);
    g = pair4::ate(&Q, &P);
    g = pair4::fexp(&g);

    P.copy(&G);
    Q = pair4::g2mul(&Q, &s);
    w = pair4::ate(&Q, &P);
    w = pair4::fexp(&w);

    if !pair4::g1member(&P) {
        println!("FAILURE - P is not in G1 ");
        fail=true;
    }

    if !pair4::g2member(&Q) {
        println!("FAILURE - Q is not in G2 ");
        fail=true;
    }

    if !pair4::gtmember(&w) {
        println!("FAILURE - e(Q,P) is not in GT ");
        fail=true;
    }

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair4::ate(&Q, &P);
    g = pair4::fexp(&g);
    g = pair4::gtpow(&g, &s);

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        fail = true;
    }
    if !fail {
        println!("All tests pass");
    }
}

fn bls48556(mut rng: &mut RAND) {
    //use core::bls48556;
    use core::bls48556::big;
    use core::bls48556::ecp;
    use core::bls48556::ecp8;
    use core::bls48556::fp;
    use core::bls48556::fp8;
    use core::bls48556::pair8;
    use core::bls48556::rom;
    let mut fail = false;
    println!("\nTesting/Timing bls48556 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE > ecp::BN {
        println!("bls48 Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::map2point(&fp::FP::new_rand(&mut rng));
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
    }
    
    P = pair8::g1mul(&G, &r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair8::g1mul(&G, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp8::ECP8::generator();

    let mut W = ecp8::ECP8::map2point(&fp8::FP8::new_rand(&mut rng));
    W.cfp();
    if W.is_infinity() {
        println!("HASHING FAILURE - P=O");
		fail=true;
    }
    W = pair8::g2mul(&W, &r);
    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
		fail=true;
    }

    W = pair8::g2mul(&Q, &r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair8::g2mul(&Q, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair8::ate(&Q, &P);
    w = pair8::fexp(&w);

    let mut g = pair8::gtpow(&w, &r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair8::gtpow(&w, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("GT  pow              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        w = pair8::ate(&Q, &P);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing ATE          - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair8::fexp(&w);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing FEXP         - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    P.copy(&G);
    Q.copy(&W);

    P = pair8::g1mul(&P, &s);
    g = pair8::ate(&Q, &P);
    g = pair8::fexp(&g);

    P.copy(&G);
    Q = pair8::g2mul(&Q, &s);
    w = pair8::ate(&Q, &P);
    w = pair8::fexp(&w);

    if !pair8::g1member(&P) {
        println!("FAILURE - P is not in G1 ");
        fail=true;
    }

    if !pair8::g2member(&Q) {
        println!("FAILURE - Q is not in G2 ");
        fail=true;
    }

    if !pair8::gtmember(&w) {
        println!("FAILURE - e(Q,P) is not in GT ");
        fail=true;
    }

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair8::ate(&Q, &P);
    g = pair8::fexp(&g);
    g = pair8::gtpow(&g, &s);

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        fail = true;
    }
    if !fail {
        println!("All tests pass");
    }
}

fn rsa2048(mut rng: &mut RAND) {
    use core::rsa2048::ff;
    use core::rsa2048::rsa;
    let mut pbc = rsa::new_public_key(ff::FFLEN);
    let mut prv = rsa::new_private_key(ff::HFLEN);
    let mut c: [u8; rsa::RFS] = [0; rsa::RFS];
    let mut m: [u8; rsa::RFS] = [0; rsa::RFS];
    let mut p: [u8; rsa::RFS] = [0; rsa::RFS];

    let mut fail = false;
    println!("\nTesting/Timing 2048-bit RSA");
    println!("Generating 2048 -bit RSA public/private key pair");

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        rsa::key_pair(&mut rng, 65537, &mut prv, &mut pbc);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("RSA gen - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    for i in 0..rsa::RFS {
        m[i] = (i % 128) as u8;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        rsa::encrypt(&pbc, &m, &mut c);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("RSA enc - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        rsa::decrypt(&prv, &c, &mut p);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("RSA dec - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut cmp = true;
    for i in 0..rsa::RFS {
        if p[i] != m[i] {
            cmp = false;
        }
    }

    if !cmp {
        println!("FAILURE - RSA decryption");
        fail = true;
    }

    if !fail {
        println!("All tests pass");
    }
}

#[allow(non_snake_case)]
fn main() {
    let mut raw: [u8; 100] = [0; 100];

    let mut rng = RAND::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = i as u8
    }

    rng.seed(100, &raw);

    ed25519(&mut rng);
    nist256(&mut rng);
    goldilocks(&mut rng);
    bn254(&mut rng);
    bls12383(&mut rng);
    bls24479(&mut rng);
    bls48556(&mut rng);
    rsa2048(&mut rng);
}
