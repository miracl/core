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

//  TestKYBER.rs - Tests Kyber API

// rustc TestKYBER.rs --extern mcore=libmcore.rlib

extern crate mcore;

//use std::str;
//use std::io;

use mcore::rand::RAND;
use mcore::kyber;

const LOOPS: usize = 100;

fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

fn main() {
    let mut raw: [u8; 100] = [0; 100];

    println!("Testing Kyber KEM");

    let mut rng = RAND::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = (i + 1) as u8
    }
    rng.seed(100, &raw);

    println!("Testing Kyber Encryption");
    for j in 0..LOOPS {
        let mut sk: [u8; kyber::SECRET_CCA_SIZE_768] = [0; kyber::SECRET_CCA_SIZE_768];
        let mut pk: [u8; kyber::PUBLIC_SIZE_768] = [0; kyber::PUBLIC_SIZE_768];
        let mut ct: [u8; kyber::CIPHERTEXT_SIZE_768] = [0; kyber::CIPHERTEXT_SIZE_768];
        let mut ss: [u8; kyber::SHARED_SECRET_768]=[0;kyber::SHARED_SECRET_768];
        let mut r64: [u8;64]=[0;64];
        let mut r32: [u8;32]=[0;32];
 
        for i in 0..64 {
            r64[i]=rng.getbyte();
        }
        kyber::keypair_768(&r64,&mut sk,&mut pk);
        for i in 0..32 {
            r32[i]=rng.getbyte();
        }
        kyber::encrypt_768(&r32,&pk,&mut ss,&mut ct);
        println!("j= {}",j);
        print!("ss= "); printbinary(&ss);
        kyber::decrypt_768(&sk,&ct,&mut ss);
        print!("ss= "); printbinary(&ss);
    }
}
