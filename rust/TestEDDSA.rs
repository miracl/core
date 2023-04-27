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

/* test driver and function exerciser for EDDSA API Functions */

extern crate mcore;

use std::str;

use mcore::rand::RAND;
use mcore::hash512::HASH512;
use mcore::sha3;
use mcore::sha3::SHA3;

fn char2int(inp: u8) -> u8 {
    if inp>='0' as u8 && inp <='9' as u8 {
        return inp-'0' as u8;
    }
    if inp>='A' as u8 && inp <='F' as u8 {
        return inp-('A' as u8) +10;
    }
    if inp>='a' as u8 && inp <='f' as u8 {
        return inp-('a' as u8) +10;
    }
    return 0;
}

// s better have even number of characters!
fn decode_hex(x: &mut[u8],s: &str) -> usize {
    let c=s.as_bytes();
    let mut i=0;
    let mut j=0;

    while j<c.len() {
        x[i]=char2int(c[j])*16+char2int(c[j+1]);
        i+=1;
        j+=2;
    }
    return i;
}

pub fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

fn prehash_sha2(m: &[u8],h: &mut [u8]) -> usize  {
    let mut sh=HASH512::new();
    for i in 0..m.len() {
            sh.process(m[i]);
    }
    let hs = sh.hash();
    for i in 0..64 {
        h[i]=hs[i];
    }
    return 64;
}

fn prehash_sha3(m: &[u8],h: &mut [u8]) -> usize  {
    let mut sh=SHA3::new(sha3::SHAKE256);
    for i in 0..m.len() {
        sh.process(m[i]);
    }
    sh.shake(h,64);
    return 64;
}

fn eddsa_ed25519(_rng: &mut RAND) {  // remove _ if using rng
    use mcore::ed25519::eddsa;
    const EFS: usize = eddsa::EFS;
    let prehash=false;

    const D:&str="c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7";  let mut d: [u8; D.len()/2]=[0;D.len()/2];
    decode_hex(&mut d,&D);

    let m:[u8;2]=[0xaf,0x82];

    println!("\nTesting EDDSA");
    println!("Curve Ed25519");

    let mut q: [u8; EFS] = [0; EFS];

    /* Generate Key pair S/W */
    eddsa::key_pair_generate(None::<&mut RAND>, &mut d, &mut q);

    print!("Secret key= 0x");
    printbinary(&d);

    print!("Public key= 0x");
    printbinary(&q);

    let mut sig:[u8;2*EFS]=[0;2*EFS];

    if prehash {
        let mut hm:[u8;64]=[0;64];
        prehash_sha2(&m,&mut hm);
        eddsa::signature(true,&d, None,&hm, &mut sig);
    } else {
        eddsa::signature(false,&d, None,&m, &mut sig);
    }
    print!("Signature= 0x"); 
    printbinary(&sig);

    let verify:bool;
    if prehash {
        let mut hm:[u8;64]=[0;64];
        prehash_sha2(&m,&mut hm);
        verify=eddsa::verify(true,&q, None ,&hm, &sig);
    } else {
        verify=eddsa::verify(false,&q, None ,&m, &sig);
    }
    if verify {
        println!("Signature is verified");
    } else {
        println!("Signature is NOT verified");
    }
}

fn eddsa_ed448(_rng: &mut RAND) { // remove _ if using rng
    use mcore::ed448::eddsa;
    const EFS: usize = eddsa::EFS;
    let prehash=false;

    const D:&str="c4eab05d357007c632f3dbb48489924d552b08fe0c353a0d4a1f00acda2c463afbea67c5e8d2877c5e3bc397a659949ef8021e954e0a12274e";  let mut d: [u8; D.len()/2]=[0;D.len()/2];
    decode_hex(&mut d,&D);
    let m:[u8;1]=[0x03];

    println!("\nTesting EDDSA");
    println!("Curve Ed448");

    let mut q: [u8; EFS+1] = [0; EFS+1];  // extra byte for compression

    /* Generate Key pair S/W */
    eddsa::key_pair_generate(None::<&mut RAND>, &mut d, &mut q);

    print!("Secret key= 0x");
    printbinary(&d);

    print!("Public key= 0x");
    printbinary(&q);

    let mut sig:[u8;2*EFS+2]=[0;2*EFS+2];

    if prehash {
        let mut hm:[u8;64]=[0;64];
         prehash_sha3(&m,&mut hm);
         eddsa::signature(true,&d, None,&hm, &mut sig);
    } else {
        eddsa::signature(false,&d, None,&m, &mut sig);
    }
    print!("Signature= 0x"); 
    printbinary(&sig);

    let verify:bool;
    if prehash {
        let mut hm:[u8;64]=[0;64];
        prehash_sha3(&m,&mut hm);
        verify=eddsa::verify(true,&q, None ,&hm, &sig);
    } else {
        verify=eddsa::verify(false,&q, None ,&m, &sig);
    }
    if verify {
        println!("Signature is verified");
    } else {
        println!("Signature is NOT verified");
    }
}

fn main() {
    let mut raw: [u8; 100] = [0; 100];

    let mut rng = RAND::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = i as u8
    }

    rng.seed(100, &raw);

    eddsa_ed25519(&mut rng);
    eddsa_ed448(&mut rng);
}
