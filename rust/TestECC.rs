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

/* test driver and function exerciser for ECDH/ECIES/ECDSA API Functions */

extern crate core;

use std::str;

use core::rand::RAND;
use core::hmac;

pub fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

fn ecdh_ed25519(mut rng: &mut RAND) {
    //use core::ed25519;
    use core::ed25519::ecdh;
    use core::ed25519::ecp;

    let pw = "M0ng00se";
    let pp: &[u8] = b"M0ng00se";
    const EFS: usize = ecdh::EFS;
    const EGS: usize = ecdh::EGS;
    const EAS: usize = ecp::AESKEY;

    let sha = ecp::HASH_TYPE;
    let mut salt: [u8; 8] = [0; 8];
    let mut s1: [u8; EGS] = [0; EGS];
    let mut w0: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut w1: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut z0: [u8; EFS] = [0; EFS];
    let mut z1: [u8; EFS] = [0; EFS];
    let mut key: [u8; EAS] = [0; EAS];
    let mut cs: [u8; EGS] = [0; EGS];
    let mut ds: [u8; EGS] = [0; EGS];
    let mut m: Vec<u8> = vec![0; 32]; // array that could be of any length. So use heap.
    let mut p1: [u8; 3] = [0; 3];
    let mut p2: [u8; 4] = [0; 4];
    let mut v: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut t: [u8; 12] = [0; 12];

    for i in 0..8 {
        salt[i] = (i + 1) as u8
    } // set Salt

    println!("\nTesting ECDH/ECDSA/ECIES");
    println!("Curve ed25519");
    println!("Alice's Passphrase= {}", pw);

    let mut s0: [u8; EFS] = [0; EGS];
    hmac::pbkdf2(hmac::MC_SHA2, sha, pp, &salt, 1000, EGS, &mut s0);

    print!("Alice's private key= 0x");
    printbinary(&s0);

    /* Generate Key pair S/W */
    ecdh::key_pair_generate(None, &mut s0, &mut w0);

    print!("Alice's public key= 0x");
    printbinary(&w0);

    let mut res = ecdh::public_key_validate(&w0);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    ecdh::key_pair_generate(Some(&mut rng), &mut s1, &mut w1);

    print!("Servers private key= 0x");
    printbinary(&s1);

    print!("Servers public key= 0x");
    printbinary(&w1);

    res = ecdh::public_key_validate(&w1);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }
    /* Calculate common key using DH - IEEE 1363 method */

    ecdh::ecpsvdp_dh(&s0, &w1, &mut z0, 0);
    ecdh::ecpsvdp_dh(&s1, &w0, &mut z1, 0);

    let mut same = true;
    for i in 0..EFS {
        if z0[i] != z1[i] {
            same = false
        }
    }

    if !same {
        println!("*** ECPSVDP-DH Failed");
        return;
    }

    hmac::kdf2(hmac::MC_SHA2, sha, &z0, None, EAS, &mut key);

    print!("Alice's DH Key=  0x");
    printbinary(&key);
    print!("Servers DH Key=  0x");
    printbinary(&key);

    if ecp::CURVETYPE != ecp::MONTGOMERY {
        for i in 0..17 {
            m[i] = i as u8
        }

        println!("Testing ECIES");

        p1[0] = 0x0;
        p1[1] = 0x1;
        p1[2] = 0x2;
        p2[0] = 0x0;
        p2[1] = 0x1;
        p2[2] = 0x2;
        p2[3] = 0x3;

        let cc = ecdh::ecies_encrypt(sha, &p1, &p2, &mut rng, &w1, &m[0..17], &mut v, &mut t);

        if let Some(mut c) = cc {
            println!("Ciphertext= ");
            print!("V= 0x");
            printbinary(&v);
            print!("C= 0x");
            printbinary(&c);
            print!("T= 0x");
            printbinary(&t);

            let mm = ecdh::ecies_decrypt(sha, &p1, &p2, &v, &mut c, &t, &s1);
            if let Some(rm) = mm {
                println!("Decryption succeeded");
                print!("Message is 0x");
                printbinary(&rm);
            } else {
                println!("*** ECIES Decryption Failed");
                return;
            }
        } else {
            println!("*** ECIES Encryption Failed");
            return;
        }

        println!("Testing ECDSA");

        if ecdh::ecpsp_dsa(sha, &mut rng, &s0, &m[0..17], &mut cs, &mut ds) != 0 {
            println!("***ECDSA Signature Failed");
            return;
        }
        println!("Signature= ");
        print!("C= 0x");
        printbinary(&cs);
        print!("D= 0x");
        printbinary(&ds);

        if ecdh::ecpvp_dsa(sha, &w0, &m[0..17], &cs, &ds) != 0 {
            println!("***ECDSA Verification Failed");
            return;
        } else {
            println!("ECDSA Signature/Verification succeeded ")
        }
    }
}

fn ecdh_nist256(mut rng: &mut RAND) {
    //use core::nist256;
    use core::nist256::ecdh;
    use core::nist256::ecp;

    let pw = "M0ng00se";
    let pp: &[u8] = b"M0ng00se";
    const EFS: usize = ecdh::EFS;
    const EGS: usize = ecdh::EGS;
    const EAS: usize = ecp::AESKEY;

    let sha = ecp::HASH_TYPE;
    let mut salt: [u8; 8] = [0; 8];
    let mut s1: [u8; EGS] = [0; EGS];
    let mut w0: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut w1: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut z0: [u8; EFS] = [0; EFS];
    let mut z1: [u8; EFS] = [0; EFS];
    let mut key: [u8; EAS] = [0; EAS];
    let mut cs: [u8; EGS] = [0; EGS];
    let mut ds: [u8; EGS] = [0; EGS];
    let mut m: Vec<u8> = vec![0; 32]; // array that could be of any length. So use heap.
    let mut p1: [u8; 3] = [0; 3];
    let mut p2: [u8; 4] = [0; 4];
    let mut v: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut t: [u8; 12] = [0; 12];

    for i in 0..8 {
        salt[i] = (i + 1) as u8
    } // set Salt

    println!("\nTesting ECDH/ECDSA/ECIES");
    println!("Curve nist256");
    println!("Alice's Passphrase= {}", pw);

    let mut s0: [u8; EFS] = [0; EGS];
    hmac::pbkdf2(hmac::MC_SHA2, sha, pp, &salt, 1000, EGS, &mut s0);

    print!("Alice's private key= 0x");
    printbinary(&s0);

    /* Generate Key pair S/W */
    ecdh::key_pair_generate(None, &mut s0, &mut w0);

    print!("Alice's public key= 0x");
    printbinary(&w0);

    let mut res = ecdh::public_key_validate(&w0);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    ecdh::key_pair_generate(Some(&mut rng), &mut s1, &mut w1);

    print!("Servers private key= 0x");
    printbinary(&s1);

    print!("Servers public key= 0x");
    printbinary(&w1);

    res = ecdh::public_key_validate(&w1);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }
    /* Calculate common key using DH - IEEE 1363 method */

    ecdh::ecpsvdp_dh(&s0, &w1, &mut z0, 0);
    ecdh::ecpsvdp_dh(&s1, &w0, &mut z1, 0);

    let mut same = true;
    for i in 0..EFS {
        if z0[i] != z1[i] {
            same = false
        }
    }

    if !same {
        println!("*** ECPSVDP-DH Failed");
        return;
    }

    hmac::kdf2(hmac::MC_SHA2, sha, &z0, None, EAS, &mut key);

    print!("Alice's DH Key=  0x");
    printbinary(&key);
    print!("Servers DH Key=  0x");
    printbinary(&key);

    if ecp::CURVETYPE != ecp::MONTGOMERY {
        for i in 0..17 {
            m[i] = i as u8
        }

        println!("Testing ECIES");

        p1[0] = 0x0;
        p1[1] = 0x1;
        p1[2] = 0x2;
        p2[0] = 0x0;
        p2[1] = 0x1;
        p2[2] = 0x2;
        p2[3] = 0x3;

        let cc = ecdh::ecies_encrypt(sha, &p1, &p2, &mut rng, &w1, &m[0..17], &mut v, &mut t);

        if let Some(mut c) = cc {
            println!("Ciphertext= ");
            print!("V= 0x");
            printbinary(&v);
            print!("C= 0x");
            printbinary(&c);
            print!("T= 0x");
            printbinary(&t);

            let mm = ecdh::ecies_decrypt(sha, &p1, &p2, &v, &mut c, &t, &s1);
            if let Some(rm) = mm {
                println!("Decryption succeeded");
                print!("Message is 0x");
                printbinary(&rm);
            } else {
                println!("*** ECIES Decryption Failed");
                return;
            }
        } else {
            println!("*** ECIES Encryption Failed");
            return;
        }

        println!("Testing ECDSA");

        if ecdh::ecpsp_dsa(sha, &mut rng, &s0, &m[0..17], &mut cs, &mut ds) != 0 {
            println!("***ECDSA Signature Failed");
            return;
        }
        println!("Signature= ");
        print!("C= 0x");
        printbinary(&cs);
        print!("D= 0x");
        printbinary(&ds);

        if ecdh::ecpvp_dsa(sha, &w0, &m[0..17], &cs, &ds) != 0 {
            println!("***ECDSA Verification Failed");
            return;
        } else {
            println!("ECDSA Signature/Verification succeeded ")
        }
    }
}

fn ecdh_goldilocks(mut rng: &mut RAND) {
    //use core::goldilocks;
    use core::goldilocks::ecdh;
    use core::goldilocks::ecp;

    
    let pw = "M0ng00se";
    let pp: &[u8] = b"M0ng00se";
    const EFS: usize = ecdh::EFS;
    const EGS: usize = ecdh::EGS;
    const EAS: usize = ecp::AESKEY;

    let sha = ecp::HASH_TYPE;
    let mut salt: [u8; 8] = [0; 8];
    let mut s1: [u8; EGS] = [0; EGS];
    let mut w0: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut w1: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut z0: [u8; EFS] = [0; EFS];
    let mut z1: [u8; EFS] = [0; EFS];
    let mut key: [u8; EAS] = [0; EAS];
    let mut cs: [u8; EGS] = [0; EGS];
    let mut ds: [u8; EGS] = [0; EGS];
    let mut m: Vec<u8> = vec![0; 32]; // array that could be of any length. So use heap.
    let mut p1: [u8; 3] = [0; 3];
    let mut p2: [u8; 4] = [0; 4];
    let mut v: [u8; 2 * EFS + 1] = [0; 2 * EFS + 1];
    let mut t: [u8; 12] = [0; 12];

    for i in 0..8 {
        salt[i] = (i + 1) as u8
    } // set Salt

    println!("\nTesting ECDH/ECDSA/ECIES");
    println!("Curve goldilocks");
    println!("Alice's Passphrase= {}", pw);

    let mut s0: [u8; EFS] = [0; EGS];
    hmac::pbkdf2(hmac::MC_SHA2, sha, pp, &salt, 1000, EGS, &mut s0);

    print!("Alice's private key= 0x");
    printbinary(&s0);

    /* Generate Key pair S/W */
    ecdh::key_pair_generate(None, &mut s0, &mut w0);

    print!("Alice's public key= 0x");
    printbinary(&w0);

    let mut res = ecdh::public_key_validate(&w0);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    ecdh::key_pair_generate(Some(&mut rng), &mut s1, &mut w1);

    print!("Servers private key= 0x");
    printbinary(&s1);

    print!("Servers public key= 0x");
    printbinary(&w1);

    res = ecdh::public_key_validate(&w1);
    if res != 0 {
        println!("ECP Public Key is invalid!");
        return;
    }
    /* Calculate common key using DH - IEEE 1363 method */

    ecdh::ecpsvdp_dh(&s0, &w1, &mut z0, 0);
    ecdh::ecpsvdp_dh(&s1, &w0, &mut z1, 0);

    let mut same = true;
    for i in 0..EFS {
        if z0[i] != z1[i] {
            same = false
        }
    }

    if !same {
        println!("*** ECPSVDP-DH Failed");
        return;
    }

    hmac::kdf2(hmac::MC_SHA2, sha, &z0, None, EAS, &mut key);

    print!("Alice's DH Key=  0x");
    printbinary(&key);
    print!("Servers DH Key=  0x");
    printbinary(&key);

    if ecp::CURVETYPE != ecp::MONTGOMERY {
        for i in 0..17 {
            m[i] = i as u8
        }

        println!("Testing ECIES");

        p1[0] = 0x0;
        p1[1] = 0x1;
        p1[2] = 0x2;
        p2[0] = 0x0;
        p2[1] = 0x1;
        p2[2] = 0x2;
        p2[3] = 0x3;

        let cc = ecdh::ecies_encrypt(sha, &p1, &p2, &mut rng, &w1, &m[0..17], &mut v, &mut t);

        if let Some(mut c) = cc {
            println!("Ciphertext= ");
            print!("V= 0x");
            printbinary(&v);
            print!("C= 0x");
            printbinary(&c);
            print!("T= 0x");
            printbinary(&t);

            let mm = ecdh::ecies_decrypt(sha, &p1, &p2, &v, &mut c, &t, &s1);
            if let Some(rm) = mm {
                println!("Decryption succeeded");
                print!("Message is 0x");
                printbinary(&rm);
            } else {
                println!("*** ECIES Decryption Failed");
                return;
            }
        } else {
            println!("*** ECIES Encryption Failed");
            return;
        }

        println!("Testing ECDSA");

        if ecdh::ecpsp_dsa(sha, &mut rng, &s0, &m[0..17], &mut cs, &mut ds) != 0 {
            println!("***ECDSA Signature Failed");
            return;
        }
        println!("Signature= ");
        print!("C= 0x");
        printbinary(&cs);
        print!("D= 0x");
        printbinary(&ds);

        if ecdh::ecpvp_dsa(sha, &w0, &m[0..17], &cs, &ds) != 0 {
            println!("***ECDSA Verification Failed");
            return;
        } else {
            println!("ECDSA Signature/Verification succeeded ")
        }
    }
}

fn rsa_2048(mut rng: &mut RAND) {
    //use core::rsa2048;
    use core::rsa2048::ff;
    use core::rsa2048::rsa;

    let sha = rsa::HASH_TYPE;
    let message: &[u8] = b"Hello World\n";
    const RFS: usize = rsa::RFS;

    let mut pbc = rsa::new_public_key(ff::FFLEN);
    let mut prv = rsa::new_private_key(ff::HFLEN);

    let mut ml: [u8; RFS] = [0; RFS];
    let mut ms: [u8; RFS] = [0; RFS];
    let mut c: [u8; RFS] = [0; RFS];
    let mut s: [u8; RFS] = [0; RFS];
    let mut e: [u8; RFS] = [0; RFS];

    println!("\nTesting RSA");
    println!("Generating public/private key pair");
    rsa::key_pair(&mut rng, 65537, &mut prv, &mut pbc);

    println!("Encrypting test string\n");
    hmac::oaep_encode(sha, &message, &mut rng, None, &mut e, RFS); /* OAEP encode message M to E  */

    rsa::encrypt(&pbc, &e, &mut c); /* encrypt encoded message */
    print!("Ciphertext= 0x");
    printbinary(&c);

    println!("Decrypting test string");
    rsa::decrypt(&prv, &c, &mut ml);
    let mlen = hmac::oaep_decode(sha, None, &mut ml, RFS); /* OAEP decode message  */

    let mess = str::from_utf8(&ml[0..mlen]).unwrap();
    print!("{}", &mess);

//    print!("M= 0x"); printbinary(&message);
    hmac::pss_encode(sha,&message,&mut rng,&mut c,RFS);
//    print!("T= 0x"); printbinary(&c);
    if hmac::pss_verify(sha,&message,&c) {
        println!("PSS Encoding OK");
    } else {
        println!("PSS Encoding FAILED");
    }

    println!("Signing message");
    hmac::pkcs15(sha, message, &mut c, RFS);

    rsa::decrypt(&prv, &c, &mut s); /* create signature in S */

    print!("Signature= 0x");
    printbinary(&s);

    rsa::encrypt(&pbc, &s, &mut ms);

    let mut cmp = true;
    if c.len() != ms.len() {
        cmp = false;
    } else {
        for j in 0..c.len() {
            if c[j] != ms[j] {
                cmp = false
            }
        }
    }
    if cmp {
        println!("Signature is valid");
    } else {
        println!("Signature is INVALID");
    }

    rsa::private_key_kill(&mut prv);
}

fn main() {
    let mut raw: [u8; 100] = [0; 100];

    let mut rng = RAND::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = i as u8
    }

    rng.seed(100, &raw);

    ecdh_ed25519(&mut rng);
    ecdh_nist256(&mut rng);
    ecdh_goldilocks(&mut rng);
    rsa_2048(&mut rng);
}
