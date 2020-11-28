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

//  TestNHS.rs - Tests NewHope Simple API

// See https://eprint.iacr.org/2016/1157 (Alkim, Ducas, Popplemann and Schwabe)

// rustc TestNHS.rs --extern core=libcore.rlib

extern crate core;

//use std::str;
//use std::io;

use core::rand::{RAND, RAND_impl};
use core::nhs;

fn main() {
    let mut raw: [u8; 100] = [0; 100];

    println!("\nTesting New Hope Key Exchange");

    let mut srng = RAND_impl::new();
    srng.clean();
    for i in 0..100 {
        raw[i] = (i + 1) as u8
    }

    srng.seed(100, &raw);

    let mut crng = RAND_impl::new();
    crng.clean();
    for i in 0..100 {
        raw[i] = (i + 2) as u8
    }

    crng.seed(100, &raw);

    let mut ss: [u8; 1792] = [0; 1792];
    let mut sb: [u8; 1824] = [0; 1824];
    let mut uc: [u8; 2176] = [0; 2176];

    let mut keya: [u8; 32] = [0; 32];
    let mut keyb: [u8; 32] = [0; 32];

    nhs::server_1(&mut srng, &mut sb, &mut ss);

    nhs::client(&mut crng, &sb, &mut uc, &mut keyb);

    nhs::server_2(&ss, &uc, &mut keya);

    print!("Alice key= ");
    for i in 0..keya.len() {
        print!("{:02X}", keya[i]);
    }
    println!("");

    print!("Bob's key= ");
    for i in 0..keyb.len() {
        print!("{:02X}", keyb[i]);
    }
    println!("");
}
