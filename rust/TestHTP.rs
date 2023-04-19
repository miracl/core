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

/* Run through some test vectors for hash-to-curve draft standard */

#![allow(non_snake_case)]
extern crate mcore;
use mcore::hmac;

fn ceil(a: usize,b: usize) -> usize {
    (a-1)/b+1
}

fn hash_to_field_ed25519(hash: usize,hlen: usize,dst: &[u8],msg: &[u8],ctr: usize) -> [mcore::ed25519::fp::FP;2] {

    use mcore::ed25519::big::BIG;
    use mcore::ed25519::dbig::DBIG;
    use mcore::ed25519::fp::FP;
    use mcore::ed25519::rom;

    let mut u: [FP; 2] = [FP::new(),FP::new()];

    let q = BIG::new_ints(&rom::MODULUS);
    let k=q.nbits();
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let m=r.nbits();
    let L=ceil(k+ceil(m,2),8);
    let mut okm: [u8;512]=[0;512];
    hmac::xmd_expand(hash,hlen,&mut okm,L*ctr,&dst,&msg);
    let mut fd: [u8;256]=[0;256];
    for i in 0..ctr {
        for j in 0..L {
            fd[j]=okm[i*L+j];
        }
		let mut dx=DBIG::frombytes(&fd[0..L]);
		let w=FP::new_big(&dx.dmod(&q));
		u[i].copy(&w);
    }

    u
}

fn htp_ed25519(mess: &str) {
//    use mcore::ed25519::ecp;
    use mcore::ed25519::ecp::ECP;
    println!("\nRandom oracle - message= {}",mess);
    let m = mess.as_bytes();
    let mut dst = "QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_".as_bytes();
    let mut u=hash_to_field_ed25519(hmac::MC_SHA2,64,dst,m,2);
    println!("u[0]= {}",u[0].tostring());
    println!("u[1]= {}",u[1].tostring());
    let mut P=ECP::map2point(&u[0]);
    println!("Q[0]= {}",P.tostring());
    let P1=ECP::map2point(&u[1]);
    println!("Q[1]= {}",P1.tostring());
    P.add(&P1);
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());

    println!("\nNon-Uniform");
    dst = "QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_".as_bytes();
    u=hash_to_field_ed25519(hmac::MC_SHA2,64,dst,m,1);
    println!("u[0]= {}",u[0].tostring());
    P=ECP::map2point(&u[0]);
    println!("Q= {}",P.tostring());
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());
}

fn hash_to_field_c25519(hash: usize,hlen: usize,dst: &[u8],msg: &[u8],ctr: usize) -> [mcore::c25519::fp::FP;2] {
    use mcore::c25519::big::BIG;
    use mcore::c25519::dbig::DBIG;
    use mcore::c25519::fp::FP;
    use mcore::c25519::rom;

    let mut u: [FP; 2] = [FP::new(),FP::new()];

    let q = BIG::new_ints(&rom::MODULUS);
    let k=q.nbits();
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let m=r.nbits();
    let L=ceil(k+ceil(m,2),8);
    let mut okm: [u8;512]=[0;512];
    hmac::xmd_expand(hash,hlen,&mut okm,L*ctr,&dst,&msg);
    let mut fd: [u8;256]=[0;256];
    for i in 0..ctr {
        for j in 0..L {
            fd[j]=okm[i*L+j];
        }
		let mut dx=DBIG::frombytes(&fd[0..L]);
		let w=FP::new_big(&dx.dmod(&q));
		u[i].copy(&w);
    }

    u
}

fn htp_c25519(mess: &str) {
//    use mcore::c25519::ecp;
    use mcore::c25519::ecp::ECP;
    let m = mess.as_bytes();

    println!("\nNon-Uniform");
    let dst = "QUUX-V01-CS02-with-curve25519_XMD:SHA-512_ELL2_NU_".as_bytes();
    let u=hash_to_field_c25519(hmac::MC_SHA2,64,dst,m,1);
    println!("u[0]= {}",u[0].tostring());
    let mut P=ECP::map2point(&u[0]);
    println!("Q= {}",P.tostring());
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());
}

fn hash_to_field_nist256(hash: usize,hlen: usize,dst: &[u8],msg: &[u8],ctr: usize) -> [mcore::nist256::fp::FP;2] {

    use mcore::nist256::big::BIG;
    use mcore::nist256::dbig::DBIG;
    use mcore::nist256::fp::FP;
    use mcore::nist256::rom;

    let mut u: [FP; 2] = [FP::new(),FP::new()];

    let q = BIG::new_ints(&rom::MODULUS);
    let k=q.nbits();
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let m=r.nbits();
    let L=ceil(k+ceil(m,2),8);
    let mut okm: [u8;512]=[0;512];
    hmac::xmd_expand(hash,hlen,&mut okm,L*ctr,&dst,&msg);
    let mut fd: [u8;256]=[0;256];
    for i in 0..ctr {
        for j in 0..L {
            fd[j]=okm[i*L+j];
        }
		let mut dx=DBIG::frombytes(&fd[0..L]);
		let w=FP::new_big(&dx.dmod(&q));
		u[i].copy(&w);
    }

    u
}

fn htp_nist256(mess: &str) {
    use mcore::nist256::ecp;
    use mcore::nist256::ecp::ECP;
    println!("\nRandom oracle - message= {}",mess);
    let m = mess.as_bytes();
    let mut dst = "QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_RO_".as_bytes();
    let mut u=hash_to_field_nist256(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,2);
    println!("u[0]= {}",u[0].tostring());
    println!("u[1]= {}",u[1].tostring());
    let mut P=ECP::map2point(&u[0]);
    println!("Q[0]= {}",P.tostring());
    let P1=ECP::map2point(&u[1]);
    println!("Q[1]= {}",P1.tostring());
    P.add(&P1);
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());

    println!("\nNon-Uniform");
    dst = "QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_NU_".as_bytes();
    u=hash_to_field_nist256(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,1);
    println!("u[0]= {}",u[0].tostring());
    P=ECP::map2point(&u[0]);
    println!("Q= {}",P.tostring());
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());
}

fn hash_to_field_ed448(hash: usize,hlen: usize,dst: &[u8],msg: &[u8],ctr: usize) -> [mcore::ed448::fp::FP;2] {

    use mcore::ed448::big::BIG;
    use mcore::ed448::dbig::DBIG;
    use mcore::ed448::fp::FP;
    use mcore::ed448::rom;

    let mut u: [FP; 2] = [FP::new(),FP::new()];

    let q = BIG::new_ints(&rom::MODULUS);
    let k=q.nbits();
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let m=r.nbits();
    let L=ceil(k+ceil(m,2),8);
    let mut okm: [u8;512]=[0;512];
    hmac::xmd_expand(hash,hlen,&mut okm,L*ctr,&dst,&msg);
    let mut fd: [u8;256]=[0;256];
    for i in 0..ctr {
        for j in 0..L {
            fd[j]=okm[i*L+j];
        }
		let mut dx=DBIG::frombytes(&fd[0..L]);
		let w=FP::new_big(&dx.dmod(&q));
		u[i].copy(&w);
    }

    u
}

fn htp_ed448(mess: &str) {
    use mcore::ed448::ecp;
    use mcore::ed448::ecp::ECP;
    println!("\nRandom oracle - message= {}",mess);
    let m = mess.as_bytes();
    let mut dst = "QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_RO_".as_bytes();
    let mut u=hash_to_field_ed448(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,2);
    println!("u[0]= {}",u[0].tostring());
    println!("u[1]= {}",u[1].tostring());
    let mut P=ECP::map2point(&u[0]);
    println!("Q[0]= {}",P.tostring());
    let P1=ECP::map2point(&u[1]);
    println!("Q[1]= {}",P1.tostring());
    P.add(&P1);
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());

    println!("\nNon-Uniform");
    dst = "QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_NU_".as_bytes();
    u=hash_to_field_ed448(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,1);
    println!("u[0]= {}",u[0].tostring());
    P=ECP::map2point(&u[0]);
    println!("Q= {}",P.tostring());
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());
}

fn hash_to_field_secp256k1(hash: usize,hlen: usize,dst: &[u8],msg: &[u8],ctr: usize) -> [mcore::secp256k1::fp::FP;2] {

    use mcore::secp256k1::big::BIG;
    use mcore::secp256k1::dbig::DBIG;
    use mcore::secp256k1::fp::FP;
    use mcore::secp256k1::rom;

    let mut u: [FP; 2] = [FP::new(),FP::new()];

    let q = BIG::new_ints(&rom::MODULUS);
    let k=q.nbits();
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let m=r.nbits();
    let L=ceil(k+ceil(m,2),8);
    let mut okm: [u8;512]=[0;512];
    hmac::xmd_expand(hash,hlen,&mut okm,L*ctr,&dst,&msg);
    let mut fd: [u8;256]=[0;256];
    for i in 0..ctr {
        for j in 0..L {
            fd[j]=okm[i*L+j];
        }
		let mut dx=DBIG::frombytes(&fd[0..L]);
		let w=FP::new_big(&dx.dmod(&q));
		u[i].copy(&w);
    }

    u
}

fn htp_secp256k1(mess: &str) {
    use mcore::secp256k1::ecp;
    use mcore::secp256k1::ecp::ECP;
    println!("\nRandom oracle - message= {}",mess);
    let m = mess.as_bytes();
    let mut dst = "QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_RO_".as_bytes();
    let mut u=hash_to_field_secp256k1(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,2);
    println!("u[0]= {}",u[0].tostring());
    println!("u[1]= {}",u[1].tostring());
    let mut P=ECP::map2point(&u[0]);
    println!("Q[0]= {}",P.tostring());
    let P1=ECP::map2point(&u[1]);
    println!("Q[1]= {}",P1.tostring());
    P.add(&P1);
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());

    println!("\nNon-Uniform");
    dst = "QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_NU_".as_bytes();
    u=hash_to_field_secp256k1(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,1);
    println!("u[0]= {}",u[0].tostring());
    P=ECP::map2point(&u[0]);
    println!("Q= {}",P.tostring());
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());
}

fn hash_to_field_bls12381(hash: usize,hlen: usize,dst: &[u8],msg: &[u8],ctr: usize) -> [mcore::bls12381::fp::FP;2] {

    use mcore::bls12381::big::BIG;
    use mcore::bls12381::dbig::DBIG;
    use mcore::bls12381::fp::FP;
    use mcore::bls12381::rom;

    let mut u: [FP; 2] = [FP::new(),FP::new()];

    let q = BIG::new_ints(&rom::MODULUS);
    let k=q.nbits();
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let m=r.nbits();
    let L=ceil(k+ceil(m,2),8);
    let mut okm: [u8;512]=[0;512];
    hmac::xmd_expand(hash,hlen,&mut okm,L*ctr,&dst,&msg);
    let mut fd: [u8;256]=[0;256];
    for i in 0..ctr {
        for j in 0..L {
            fd[j]=okm[i*L+j];
        }
		let mut dx=DBIG::frombytes(&fd[0..L]);
		let w=FP::new_big(&dx.dmod(&q));
		u[i].copy(&w);
    }

    u
}

fn htp_bls12381(mess: &str) {
    use mcore::bls12381::ecp;
    use mcore::bls12381::ecp::ECP;
    println!("\nRandom oracle - message= {}",mess);
    let m = mess.as_bytes();
    let mut dst = "QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_RO_".as_bytes();
    let mut u=hash_to_field_bls12381(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,2);
    println!("u[0]= {}",u[0].tostring());
    println!("u[1]= {}",u[1].tostring());
    let mut P=ECP::map2point(&u[0]);
    println!("Q[0]= {}",P.tostring());
    let P1=ECP::map2point(&u[1]);
    println!("Q[1]= {}",P1.tostring());
    P.add(&P1);
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());

    println!("\nNon-Uniform");
    dst = "QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_NU_".as_bytes();
    u=hash_to_field_bls12381(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,1);
    println!("u[0]= {}",u[0].tostring());
    P=ECP::map2point(&u[0]);
    println!("Q= {}",P.tostring());
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());
}

fn hash_to_field2_bls12381(hash: usize,hlen: usize,dst: &[u8],msg: &[u8],ctr: usize) -> [mcore::bls12381::fp2::FP2;2] {

    use mcore::bls12381::big::BIG;
    use mcore::bls12381::dbig::DBIG;
    use mcore::bls12381::fp::FP;
    use mcore::bls12381::fp2::FP2;
    use mcore::bls12381::rom;

    let mut u: [FP2; 2] = [FP2::new(),FP2::new()];

    let q = BIG::new_ints(&rom::MODULUS);
    let k=q.nbits();
    let r = BIG::new_ints(&rom::CURVE_ORDER);
    let m=r.nbits();
    let L=ceil(k+ceil(m,2),8);
    let mut okm: [u8;512]=[0;512];
    hmac::xmd_expand(hash,hlen,&mut okm,2*L*ctr,&dst,&msg);
    let mut fd: [u8;256]=[0;256];
    for i in 0..ctr {
        for j in 0..L {
            fd[j]=okm[2*i*L+j];
        }
		let mut dx=DBIG::frombytes(&fd[0..L]);
		let w1=FP::new_big(&dx.dmod(&q));

        for j in 0..L {
            fd[j]=okm[(2*i+1)*L+j];
        }
		dx=DBIG::frombytes(&fd[0..L]);
		let w2=FP::new_big(&dx.dmod(&q));
		u[i].copy(&FP2::new_fps(&w1,&w2));
    }

    u
}

fn htp2_bls12381(mess: &str) {
    use mcore::bls12381::ecp;
    use mcore::bls12381::ecp2::ECP2;
    println!("\nRandom oracle - message= {}",mess);
    let m = mess.as_bytes();
    let mut dst = "QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_RO_".as_bytes();
    let mut u=hash_to_field2_bls12381(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,2);
    println!("u[0]= {}",u[0].tostring());
    println!("u[1]= {}",u[1].tostring());
    let mut P=ECP2::map2point(&u[0]);
    println!("Q[0]= {}",P.tostring());
    let P1=ECP2::map2point(&u[1]);
    println!("Q[1]= {}",P1.tostring());
    P.add(&P1);
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());

    println!("\nNon-Uniform");
    dst = "QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_NU_".as_bytes();
    u=hash_to_field2_bls12381(hmac::MC_SHA2,ecp::HASH_TYPE,dst,m,1);
    println!("u[0]= {}",u[0].tostring());
    P=ECP2::map2point(&u[0]);
    println!("Q= {}",P.tostring());
    P.cfp();
    P.affine();
    println!("P= {}",P.tostring());
}

fn main() {
    println!("\nTesting HTP for curve ed25519");
    htp_ed25519("");
    htp_ed25519("abc");
    htp_ed25519("abcdef0123456789");
    htp_ed25519("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_ed25519("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    println!("\nTesting HTP for curve c25519");
    htp_c25519("");
    htp_c25519("abc");
    htp_c25519("abcdef0123456789");
    htp_c25519("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_c25519("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    println!("\nTesting HTP for curve nist256\n");
    htp_nist256("");
    htp_nist256("abc");
    htp_nist256("abcdef0123456789");
    htp_nist256("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_nist256("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    println!("\nTesting HTP for curve ed448");
    htp_ed448("");
    htp_ed448("abc");
    htp_ed448("abcdef0123456789");
    htp_ed448("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_ed448("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    println!("\nTesting HTP for curve secp256k1");
    htp_secp256k1("");
    htp_secp256k1("abc");
    htp_secp256k1("abcdef0123456789");
    htp_secp256k1("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_secp256k1("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    println!("\nTesting HTP for curve bls12381_G1");
    htp_bls12381("");
    htp_bls12381("abc");
    htp_bls12381("abcdef0123456789");
    htp_bls12381("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp_bls12381("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    println!("\nTesting HTP for curve bls12381_G2");
    htp2_bls12381("");
    htp2_bls12381("abc");
    htp2_bls12381("abcdef0123456789");
    htp2_bls12381("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
    htp2_bls12381("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

}