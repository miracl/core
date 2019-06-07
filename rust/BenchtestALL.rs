/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
#![allow(non_snake_case)]
extern crate amcl;

//use std::str;
//use std::io;

use amcl::arch;
use amcl::rand::RAND;

use std::time::Instant;

const MIN_ITERS: isize = 10;
const MIN_TIME: isize = 10;

fn ed25519(mut rng: &mut RAND) {
    //use amcl::ed25519;
    use amcl::ed25519::big;
    use amcl::ed25519::ecp;
    use amcl::ed25519::fp;
    use amcl::ed25519::rom;
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

    let mut r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let mut s = big::BIG::randomnum(&r, &mut rng);

    let P = G.mul(&mut r);
    if !P.is_infinity() {
        println!("FAILURE - rG!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = G.mul(&mut s);
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
    //use amcl::nist256;
    use amcl::nist256::big;
    use amcl::nist256::ecp;
    use amcl::nist256::fp;
    use amcl::nist256::rom;
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

    let mut r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let mut s = big::BIG::randomnum(&r, &mut rng);

    let P = G.mul(&mut r);
    if !P.is_infinity() {
        println!("FAILURE - rG!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = G.mul(&mut s);
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
    //use amcl::goldilocks;
    use amcl::goldilocks::big;
    use amcl::goldilocks::ecp;
    use amcl::goldilocks::fp;
    use amcl::goldilocks::rom;
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

    let mut r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let mut s = big::BIG::randomnum(&r, &mut rng);

    let P = G.mul(&mut r);
    if !P.is_infinity() {
        println!("FAILURE - rG!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = G.mul(&mut s);
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
    //use amcl::bn254;
    use amcl::bn254::big;
    use amcl::bn254::ecp;
    use amcl::bn254::ecp2;
    use amcl::bn254::fp;
    use amcl::bn254::pair;
    use amcl::bn254::rom;
    let mut fail = false;
    println!("\nTesting/Timing BN254 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE == ecp::BLS {
        println!("BLS Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let mut G = ecp::ECP::generator();

    let mut r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let mut s = big::BIG::randomnum(&r, &mut rng);

    let mut P = pair::g1mul(&mut G, &mut r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair::g1mul(&mut G, &mut s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp2::ECP2::generator();
    let mut W = pair::g2mul(&mut Q, &mut r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair::g2mul(&mut Q, &mut s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair::ate(&mut Q, &mut P);
    w = pair::fexp(&w);

    let mut g = pair::gtpow(&mut w, &mut r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair::gtpow(&mut w, &mut s);
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
        let _ = w.compow(&s, &mut r);
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
        w = pair::ate(&mut Q, &mut P);
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

    P = pair::g1mul(&mut P, &mut s);
    g = pair::ate(&mut Q, &mut P);
    g = pair::fexp(&g);

    P.copy(&G);
    Q = pair::g2mul(&mut Q, &mut s);
    w = pair::ate(&mut Q, &mut P);
    w = pair::fexp(&w);

    if !g.equals(&mut w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair::ate(&mut Q, &mut P);
    g = pair::fexp(&g);
    g = pair::gtpow(&mut g, &mut s);

    if !g.equals(&mut w) {
        println!("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        fail = true;
    }
    if !fail {
        println!("All tests pass");
    }
}

fn bls383(mut rng: &mut RAND) {
    //use amcl::bls383;
    use amcl::bls383::big;
    use amcl::bls383::ecp;
    use amcl::bls383::ecp2;
    use amcl::bls383::fp;
    use amcl::bls383::pair;
    use amcl::bls383::rom;
    let mut fail = false;
    println!("\nTesting/Timing BLS383 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE == ecp::BLS {
        println!("BLS Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let mut G = ecp::ECP::generator();

    let mut r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let mut s = big::BIG::randomnum(&r, &mut rng);

    let mut P = pair::g1mul(&mut G, &mut r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair::g1mul(&mut G, &mut s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp2::ECP2::generator();
    let mut W = pair::g2mul(&mut Q, &mut r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair::g2mul(&mut Q, &mut s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair::ate(&mut Q, &mut P);
    w = pair::fexp(&w);

    let mut g = pair::gtpow(&mut w, &mut r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair::gtpow(&mut w, &mut s);
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
        let _ = w.compow(&s, &mut r);
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
        w = pair::ate(&mut Q, &mut P);
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

    P = pair::g1mul(&mut P, &mut s);
    g = pair::ate(&mut Q, &mut P);
    g = pair::fexp(&g);

    P.copy(&G);
    Q = pair::g2mul(&mut Q, &mut s);
    w = pair::ate(&mut Q, &mut P);
    w = pair::fexp(&w);

    if !g.equals(&mut w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair::ate(&mut Q, &mut P);
    g = pair::fexp(&g);
    g = pair::gtpow(&mut g, &mut s);

    if !g.equals(&mut w) {
        println!("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        fail = true;
    }
    if !fail {
        println!("All tests pass");
    }
}

fn bls24(mut rng: &mut RAND) {
    //use amcl::bls24;
    use amcl::bls24::big;
    use amcl::bls24::ecp;
    use amcl::bls24::ecp4;
    use amcl::bls24::fp;
    use amcl::bls24::pair192;
    use amcl::bls24::rom;
    let mut fail = false;
    println!("\nTesting/Timing BLS24 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE == ecp::BLS {
        println!("BLS24 Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let mut G = ecp::ECP::generator();

    let mut r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let mut s = big::BIG::randomnum(&r, &mut rng);

    let mut P = pair192::g1mul(&mut G, &mut r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair192::g1mul(&mut G, &mut s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp4::ECP4::generator();
    let mut W = pair192::g2mul(&mut Q, &mut r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair192::g2mul(&mut Q, &mut s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair192::ate(&mut Q, &mut P);
    w = pair192::fexp(&w);

    let mut g = pair192::gtpow(&mut w, &mut r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair192::gtpow(&mut w, &mut s);
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
        let _ = w.compow(&s, &mut r);
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
        w = pair192::ate(&mut Q, &mut P);
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
        let _ = pair192::fexp(&w);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing FEXP         - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    P.copy(&G);
    Q.copy(&W);

    P = pair192::g1mul(&mut P, &mut s);
    g = pair192::ate(&mut Q, &mut P);
    g = pair192::fexp(&g);

    P.copy(&G);
    Q = pair192::g2mul(&mut Q, &mut s);
    w = pair192::ate(&mut Q, &mut P);
    w = pair192::fexp(&w);

    if !g.equals(&mut w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair192::ate(&mut Q, &mut P);
    g = pair192::fexp(&g);
    g = pair192::gtpow(&mut g, &mut s);

    if !g.equals(&mut w) {
        println!("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        fail = true;
    }
    if !fail {
        println!("All tests pass");
    }
}

fn bls48(mut rng: &mut RAND) {
    //use amcl::bls48;
    use amcl::bls48::big;
    use amcl::bls48::ecp;
    use amcl::bls48::ecp8;
    use amcl::bls48::fp;
    use amcl::bls48::pair256;
    use amcl::bls48::rom;
    let mut fail = false;
    println!("\nTesting/Timing bls48 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE == ecp::BLS {
        println!("bls48 Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let mut G = ecp::ECP::generator();

    let mut r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let mut s = big::BIG::randomnum(&r, &mut rng);

    let mut P = pair256::g1mul(&mut G, &mut r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair256::g1mul(&mut G, &mut s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp8::ECP8::generator();
    //println!("Q= {}",Q.tostring());
    //println!("r= {}",r.tostring());
    let mut W = pair256::g2mul(&mut Q, &mut r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair256::g2mul(&mut Q, &mut s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair256::ate(&mut Q, &mut P);
    w = pair256::fexp(&w);

    let mut g = pair256::gtpow(&mut w, &mut r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair256::gtpow(&mut w, &mut s);
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
        let _ = w.compow(&s, &mut r);
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
        w = pair256::ate(&mut Q, &mut P);
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
        let _ = pair256::fexp(&w);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("PAIRing FEXP         - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    P.copy(&G);
    Q.copy(&W);

    P = pair256::g1mul(&mut P, &mut s);
    g = pair256::ate(&mut Q, &mut P);
    g = pair256::fexp(&g);

    P.copy(&G);
    Q = pair256::g2mul(&mut Q, &mut s);
    w = pair256::ate(&mut Q, &mut P);
    w = pair256::fexp(&w);

    if !g.equals(&mut w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair256::ate(&mut Q, &mut P);
    g = pair256::fexp(&g);
    g = pair256::gtpow(&mut g, &mut s);

    if !g.equals(&mut w) {
        println!("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        fail = true;
    }
    if !fail {
        println!("All tests pass");
    }
}

fn rsa2048(mut rng: &mut RAND) {
    use amcl::rsa2048::ff;
    use amcl::rsa2048::rsa;
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
    bls383(&mut rng);
    bls24(&mut rng);
    bls48(&mut rng);
    rsa2048(&mut rng);
}
