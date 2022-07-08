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

//  TestDLTHM.rs - Tests Dilithium API

// See https://eprint.iacr.org/2017/633 (Ducas, Lepoint, Lyubashevsky, Schwabe, Seiler, Stehle)

// rustc TestDLTHM.rs --extern mcore=libmcore.rlib

extern crate mcore;

//use std::str;
//use std::io;

use mcore::rand::RAND;
use mcore::dilithium;

const LOOPS: usize = 100;

fn main() {
    let mut raw: [u8; 100] = [0; 100];

    println!("Testing Dilithium signature");

    let mut rng = RAND::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = (i + 1) as u8
    }
    rng.seed(100, &raw);

    let mut tats=0;
    for _ in 0..LOOPS {
        let mut tau:[u8;32]=[0;32];
        let mut sk: [u8; dilithium::SK_SIZE] = [0; dilithium::SK_SIZE];
        let mut pk: [u8; dilithium::PK_SIZE] = [0; dilithium::PK_SIZE];
        let mut sig: [u8; dilithium::SIG_SIZE] = [0; dilithium::SIG_SIZE];

        let message = "Hello World";
        let m = message.as_bytes();

        for i in 0..32 {
            tau[i] = rng.getbyte();
        }

        dilithium::keypair(&tau, &mut sk, &mut pk);

        println!("private key {} bits",8*dilithium::SK_SIZE);
        println!("public key {} bits",8*dilithium::PK_SIZE);

        let attempts=dilithium::signature(&sk,&m,&mut sig);
        tats+=attempts;
        println!("signature {} bits created on attempt {}",8*dilithium::SIG_SIZE,attempts);

        let result=dilithium::verify(&pk,&m,&sig);
        if result {
            println!("Signature is verified");
        } else {
            println!("Signature is NOT verified");
            break;
        }
    }
    if LOOPS>1 {
        println!("Average= {}",tats/LOOPS);
    }
}
