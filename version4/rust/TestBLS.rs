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

extern crate amcl;

use amcl::rand::RAND;

pub fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

fn bls_bn254(mut rng: &mut RAND) {
    use amcl::bn254::bls;

    const BFS: usize = bls::BFS;
    const BGS: usize = bls::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize = BFS + 1; /* Group 1 Size */
    const G2S: usize = 4 * BFS; /* Group 2 Size */

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];

    let m = String::from("This is a test message");

    bls::init();
    bls::key_pair_generate(&mut rng, &mut s, &mut w);
    print!("Private key : 0x");
    printbinary(&s);
    print!("Public  key : 0x");
    printbinary(&w);

    bls::sign(&mut sig, &m, &s);
    print!("Signature : 0x");
    printbinary(&sig);

    let res = bls::verify(&sig, &m, &w);
    if res == 0 {
        println!("Signature is OK");
    } else {
        println!("Signature is *NOT* OK");
    }
}

fn bls_bls383(mut rng: &mut RAND) {
    use amcl::bls383::bls;

    const BFS: usize = bls::BFS;
    const BGS: usize = bls::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize = BFS + 1; /* Group 1 Size */
    const G2S: usize = 4 * BFS; /* Group 2 Size */

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];

    let m = String::from("This is a test message");

    bls::init();
    bls::key_pair_generate(&mut rng, &mut s, &mut w);
    print!("Private key : 0x");
    printbinary(&s);
    print!("Public  key : 0x");
    printbinary(&w);

    bls::sign(&mut sig, &m, &s);
    print!("Signature : 0x");
    printbinary(&sig);

    let res = bls::verify(&sig, &m, &w);
    if res == 0 {
        println!("Signature is OK");
    } else {
        println!("Signature is *NOT* OK");
    }
}

fn bls_bls24(mut rng: &mut RAND) {
    use amcl::bls24::bls192;

    const BFS: usize = bls192::BFS;
    const BGS: usize = bls192::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize = BFS + 1; /* Group 1 Size */
    const G2S: usize = 8 * BFS; /* Group 2 Size */

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];

    let m = String::from("This is a test message");

    bls192::init();
    bls192::key_pair_generate(&mut rng, &mut s, &mut w);
    print!("Private key : 0x");
    printbinary(&s);
    print!("Public  key : 0x");
    printbinary(&w);

    bls192::sign(&mut sig, &m, &s);
    print!("Signature : 0x");
    printbinary(&sig);

    let res = bls192::verify(&sig, &m, &w);
    if res == 0 {
        println!("Signature is OK");
    } else {
        println!("Signature is *NOT* OK");
    }
}

fn bls_bls48(mut rng: &mut RAND) {
    use amcl::bls48::bls256;

    const BFS: usize = bls256::BFS;
    const BGS: usize = bls256::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize = BFS + 1; /* Group 1 Size */
    const G2S: usize = 16 * BFS; /* Group 2 Size */

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];

    let m = String::from("This is a test message");
    bls256::init();
    bls256::key_pair_generate(&mut rng, &mut s, &mut w);
    print!("Private key : 0x");
    printbinary(&s);
    print!("Public  key : 0x");
    printbinary(&w);

    bls256::sign(&mut sig, &m, &s);
    print!("Signature : 0x");
    printbinary(&sig);

    let res = bls256::verify(&sig, &m, &w);
    if res == 0 {
        println!("Signature is OK");
    } else {
        println!("Signature is *NOT* OK");
    }
}

fn main() {
    use amcl::arch;

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
    bls_bls383(&mut rng);
    println!("\nTesting BLS signature for curve bls24");
    bls_bls24(&mut rng);
    println!("\nTesting BLS signature for curve bls48");
    bls_bls48(&mut rng);
}
