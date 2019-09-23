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
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::hashit(&s);
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
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
    let s = big::BIG::randomnum(&r, &mut rng);
    
    let mut P = ecp::ECP::hashit(&s);
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
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
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::hashit(&s);
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
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
    use core::bn254::pair;
    use core::bn254::rom;
    let mut fail = false;
    println!("\nTesting/Timing bn254 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE == ecp::BLS {
        println!("BLS Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::hashit(&s);
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

    let mut W = ecp2::ECP2::hashit(&s);
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
    use core::bls12383::pair;
    use core::bls12383::rom;
    let mut fail = false;
    println!("\nTesting/Timing bls12383 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE == ecp::BLS {
        println!("BLS Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::hashit(&s);
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

    let mut W = ecp2::ECP2::hashit(&s);
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
    use core::bls24479::pair192;
    use core::bls24479::rom;
    let mut fail = false;
    println!("\nTesting/Timing bls24479 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE == ecp::BLS {
        println!("BLS24 Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::hashit(&s);
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
    }
    
    P = pair192::g1mul(&G, &r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair192::g1mul(&G, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp4::ECP4::generator();

    let mut W = ecp4::ECP4::hashit(&s);
    W.cfp();
    if W.is_infinity() {
        println!("HASHING FAILURE - P=O");
		fail=true;
    }
    W = pair192::g2mul(&W, &r);
    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
		fail=true;
    }

    W = pair192::g2mul(&Q, &r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair192::g2mul(&Q, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair192::ate(&Q, &P);
    w = pair192::fexp(&w);

    let mut g = pair192::gtpow(&w, &r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair192::gtpow(&w, &s);
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
        w = pair192::ate(&Q, &P);
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

    P = pair192::g1mul(&P, &s);
    g = pair192::ate(&Q, &P);
    g = pair192::fexp(&g);

    P.copy(&G);
    Q = pair192::g2mul(&Q, &s);
    w = pair192::ate(&Q, &P);
    w = pair192::fexp(&w);

    if !pair192::g1member(&P) {
        println!("FAILURE - P is not in G1 ");
        fail=true;
    }

    if !pair192::g2member(&Q) {
        println!("FAILURE - Q is not in G2 ");
        fail=true;
    }

    if !pair192::gtmember(&w) {
        println!("FAILURE - e(Q,P) is not in GT ");
        fail=true;
    }

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair192::ate(&Q, &P);
    g = pair192::fexp(&g);
    g = pair192::gtpow(&g, &s);

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
    use core::bls48556::pair256;
    use core::bls48556::rom;
    let mut fail = false;
    println!("\nTesting/Timing bls48556 Pairings");

    if ecp::CURVE_PAIRING_TYPE == ecp::BN {
        println!("BN Pairing-Friendly Curve");
    }
    if ecp::CURVE_PAIRING_TYPE == ecp::BLS {
        println!("bls48 Pairing-Friendly Curve");
    }

    println!("Modulus size {:} bits", fp::MODBITS);
    println!("{:} bit build", arch::CHUNK);

    let G = ecp::ECP::generator();

    let r = big::BIG::new_ints(&rom::CURVE_ORDER);
    let s = big::BIG::randomnum(&r, &mut rng);

    let mut P = ecp::ECP::hashit(&s);
    if P.is_infinity() {
        println!("HASH FAILURE - P=O");
        fail = true;
    }
    
    P = pair256::g1mul(&G, &r);

    if !P.is_infinity() {
        println!("FAILURE - rP!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        P = pair256::g1mul(&G, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G1  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut Q = ecp8::ECP8::generator();

    let mut W = ecp8::ECP8::hashit(&s);
    W.cfp();
    if W.is_infinity() {
        println!("HASHING FAILURE - P=O");
		fail=true;
    }
    W = pair256::g2mul(&W, &r);
    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
		fail=true;
    }

    W = pair256::g2mul(&Q, &r);

    if !W.is_infinity() {
        println!("FAILURE - rQ!=O");
        fail = true;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        W = pair256::g2mul(&Q, &s);
        iterations += 1;
        let elapsed = start.elapsed();
        dur = (elapsed.as_secs() * 1_000) + (elapsed.subsec_nanos() / 1_000_000) as u64;
    }
    let duration = (dur as f64) / (iterations as f64);
    print!("G2  mul              - {:} iterations  ", iterations);
    println!(" {:0.2} ms per iteration", duration);

    let mut w = pair256::ate(&Q, &P);
    w = pair256::fexp(&w);

    let mut g = pair256::gtpow(&w, &r);

    if !g.isunity() {
        println!("FAILURE - g^r!=1");
        return;
    }

    let start = Instant::now();
    let mut iterations = 0;
    let mut dur = 0 as u64;
    while dur < (MIN_TIME as u64) * 1000 || iterations < MIN_ITERS {
        let _ = pair256::gtpow(&w, &s);
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
        w = pair256::ate(&Q, &P);
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

    P = pair256::g1mul(&P, &s);
    g = pair256::ate(&Q, &P);
    g = pair256::fexp(&g);

    P.copy(&G);
    Q = pair256::g2mul(&Q, &s);
    w = pair256::ate(&Q, &P);
    w = pair256::fexp(&w);

    if !pair256::g1member(&P) {
        println!("FAILURE - P is not in G1 ");
        fail=true;
    }

    if !pair256::g2member(&Q) {
        println!("FAILURE - Q is not in G2 ");
        fail=true;
    }

    if !pair256::gtmember(&w) {
        println!("FAILURE - e(Q,P) is not in GT ");
        fail=true;
    }

    if !g.equals(&w) {
        println!("FAILURE - e(sQ,p)!=e(Q,sP) ");
        fail = true;
    }

    Q.copy(&W);
    g = pair256::ate(&Q, &P);
    g = pair256::fexp(&g);
    g = pair256::gtpow(&g, &s);

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
