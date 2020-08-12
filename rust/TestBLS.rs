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

/* Test driver and function exerciser for Boneh-Lynn-Shacham BLS Signature API Functions */

/* To reverse the groups G1 and G2, edit BLS*.go

Swap G1 <-> G2
Swap ECP <-> ECPn
Disable G2 precomputation
Switch G1/G2 parameter order in pairing function calls

Swap G1S and G2S in this program

*/

extern crate core;

use core::rand::RAND;

pub fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

fn bls_bn254(rng: &mut RAND) {
    use core::bn254::bls;

    const BFS: usize = bls::BFS;
    const BGS: usize = bls::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize = BFS + 1; /* Group 1 Size - compressed */
    const G2S: usize = 2 * BFS + 1; /* Group 2 Size - compressed*/

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];
    let mut ikm: [u8; 32] = [0; 32];

    for i in 0..32 {
        //ikm[i] = (i+1) as u8;
        ikm[i]=rng.getbyte();
    }

    let m = String::from("This is a test message");

    let mut res = bls::init();
    if res != 0 {
        println!("Failed to Initialize");
        return;
    }

    res = bls::key_pair_generate(&ikm, &mut s, &mut w);
    if res != 0 {
        println!("Failed to Generate Keys");
        return;
    }

    print!("Private key : 0x");
    printbinary(&s);
    print!("Public  key : 0x");
    printbinary(&w);

    bls::core_sign(&mut sig, &m.as_bytes(), &s);
    print!("Signature : 0x");
    printbinary(&sig);

    res = bls::core_verify(&sig, &m.as_bytes(), &w);
    if res == 0 {
        println!("Signature is OK");
    } else {
        println!("Signature is *NOT* OK");
    }
}

fn bls_bls12383(rng: &mut RAND) {
    use core::bls12383::bls;

    const BFS: usize = bls::BFS;
    const BGS: usize = bls::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize = BFS + 1; /* Group 1 Size  - compressed */
    const G2S: usize = 2 * BFS + 1; /* Group 2 Size  - compressed */

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];
    let mut ikm: [u8; 32] = [0; 32];

    for i in 0..32 {
        //ikm[i] = (i+1) as u8;
        ikm[i]=rng.getbyte();
    }

    let m = String::from("This is a test message");

    let mut res = bls::init();
    if res != 0 {
        println!("Failed to Initialize");
        return;
    }
    res = bls::key_pair_generate(&ikm, &mut s, &mut w);
    if res != 0 {
        println!("Failed to Generate Keys");
        return;
    }
    print!("Private key : 0x");
    printbinary(&s);
    print!("Public  key : 0x");
    printbinary(&w);

    bls::core_sign(&mut sig, &m.as_bytes(), &s);
    print!("Signature : 0x");
    printbinary(&sig);

    res = bls::core_verify(&sig, &m.as_bytes(), &w);
    if res == 0 {
        println!("Signature is OK");
    } else {
        println!("Signature is *NOT* OK");
    }
}

fn bls_bls24479(rng: &mut RAND) {
    use core::bls24479::bls192;

    const BFS: usize = bls192::BFS;
    const BGS: usize = bls192::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize = BFS + 1; /* Group 1 Size  - compressed */
    const G2S: usize = 4 * BFS + 1; /* Group 2 Size  - compressed */

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];
    let mut ikm: [u8; 48] = [0; 48];

    for i in 0..48 {
        //ikm[i] = (i+1) as u8;
        ikm[i]=rng.getbyte();
    }

    let m = String::from("This is a test message");
    let mut res = bls192::init();
    if res != 0 {
        println!("Failed to Initialize");
        return;
    }
    res = bls192::key_pair_generate(&ikm, &mut s, &mut w);
    if res != 0 {
        println!("Failed to Generate Keys");
        return;
    }
    print!("Private key : 0x");
    printbinary(&s);
    print!("Public  key : 0x");
    printbinary(&w);

    bls192::core_sign(&mut sig, &m.as_bytes(), &s);
    print!("Signature : 0x");
    printbinary(&sig);

    res = bls192::core_verify(&sig, &m.as_bytes(), &w);
    if res == 0 {
        println!("Signature is OK");
    } else {
        println!("Signature is *NOT* OK");
    }
}

fn bls_bls48556(rng: &mut RAND) {
    use core::bls48556::bls256;

    const BFS: usize = bls256::BFS;
    const BGS: usize = bls256::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize = BFS + 1; /* Group 1 Size  - compressed */
    const G2S: usize = 8 * BFS + 1; /* Group 2 Size  - compressed */

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];
    let mut ikm: [u8; 64] = [0; 64];

    for i in 0..64 {
        //ikm[i] = (i+1) as u8;
        ikm[i]=rng.getbyte();
    }

    let m = String::from("This is a test message");
    let mut res = bls256::init();
    if res != 0 {
        println!("Failed to Initialize");
        return;
    }
    res = bls256::key_pair_generate(&ikm, &mut s, &mut w);
    if res != 0 {
        println!("Failed to Generate Keys");
        return;
    }
    print!("Private key : 0x");
    printbinary(&s);
    print!("Public  key : 0x");
    printbinary(&w);

    bls256::core_sign(&mut sig, &m.as_bytes(), &s);
    print!("Signature : 0x");
    printbinary(&sig);

    res = bls256::core_verify(&sig, &m.as_bytes(), &w);
    if res == 0 {
        println!("Signature is OK");
    } else {
        println!("Signature is *NOT* OK");
    }
}

fn main() {
    use core::arch;

    let mut raw: [u8; 100] = [0; 100];

    let mut rng = RAND::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = i as u8
    }

    rng.seed(100, &raw);

    println!("{} bit build", arch::CHUNK);

    println!("Testing BLS signature for curve bn254");
    bls_bn254(&mut rng);
    println!("\nTesting BLS signature for curve bls383");
    bls_bls12383(&mut rng);
    println!("\nTesting BLS signature for curve bls24");
    bls_bls24479(&mut rng);
    println!("\nTesting BLS signature for curve bls48");
    bls_bls48556(&mut rng);
}
