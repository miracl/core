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

use crate::xxx::ecp;
use crate::xxx::ecdh;

use crate::hmac;
use crate::rand::RAND;

#[allow(non_snake_case)]
fn reverse (x: &mut [u8]) {
	let lx=x.len();
	for i in 0..lx/2 {
		let ch=x[i];
		x[i]=x[lx-i-1];
		x[lx-i-1]=ch;
	}
}

fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

#[allow(non_snake_case)]
pub fn encap(config_id: usize,rng: Option<&mut RAND>,ske: &mut [u8],mut z: &mut [u8],mut pkE: &mut [u8],mut pkR: &mut [u8]) {
    let mut skE: [u8; ecdh::EGS] = [0; ecdh::EGS];
    let kem = config_id&7;
    if let Some(mut x) = rng {
        ecdh::key_pair_generate(Some(&mut x), &mut skE, &mut pkE);
    } else {
		for i in 0..ecdh::EGS {
			skE[i]=ske[i];
		}
		if kem==2 || kem==4 {
			reverse(&mut skE);
			if kem==2 {
				skE[ecdh::EGS-1]&=248;
				skE[0]&=127;
				skE[0]|=64;
			} else {
				skE[ecdh::EGS-1]&=252;
				skE[0]|=128;
			}
		}
	    ecdh::key_pair_generate(None, &mut skE, &mut pkE);
    }
	if kem==2 || kem==4 {
		reverse(&mut pkR);
	}
	ecdh::ecpsvdp_dh(&skE, &pkR, &mut z);
	if kem==2 || kem==4 {
		reverse(&mut pkR);
		reverse(&mut pkE);
		reverse(&mut z);
	}
}

#[allow(non_snake_case)]
pub fn decap(config_id: usize,mut z: &mut [u8],mut pkE: &mut [u8],skr: &mut [u8]) {
	let mut skR: [u8; ecdh::EGS] = [0; ecdh::EGS];
	let kem = config_id&7;
	for i in 0..ecdh::EGS {
		skR[i]=skr[i];
	}
	if kem==2 || kem==4 {
		reverse(&mut skR);
		reverse(&mut pkE);
		if kem==2 {
			skR[ecdh::EGS-1]&=248;
			skR[0]&=127;
			skR[0]|=64;
		} else {
			skR[ecdh::EGS-1]&=252;
			skR[0]|=128;
		}
	}
	ecdh::ecpsvdp_dh(&skR, &pkE, &mut z);
	if kem==2 || kem==4 {
		reverse(&mut pkE);
		reverse(&mut z);
	}
}

#[allow(non_snake_case)]
pub fn authencap(config_id: usize,rng: Option<&mut RAND>,ske: &mut [u8],z: &mut [u8],mut pkE: &mut [u8],mut pkR: &mut [u8],ski: &mut [u8]) {
	let mut skE: [u8; ecdh::EGS] = [0; ecdh::EGS];
	let mut skI: [u8; ecdh::EGS] = [0; ecdh::EGS];
    let mut iz: [u8; ecdh::EFS] = [0; ecdh::EFS];
    let mut nz: [u8; ecdh::EFS] = [0; ecdh::EFS];
	let kem = config_id&7;

    if let Some(mut x) = rng {
        ecdh::key_pair_generate(Some(&mut x), &mut skE, &mut pkE);
    } else {
		for i in 0..ecdh::EGS {
			skE[i]=ske[i];
			skI[i]=ski[i];
		}	
		if kem==2 || kem==4 {
			reverse(&mut skE);
			reverse(&mut skI);
			if kem==2 {
				skE[ecdh::EGS-1]&=248;
				skE[0]&=127;
				skE[0]|=64;
				skI[ecdh::EGS-1]&=248;
				skI[0]&=127;
				skI[0]|=64;	
			} else {
				skE[ecdh::EGS-1]&=252;
				skE[0]|=128;
				skI[ecdh::EGS-1]&=252;
				skI[0]|=128;
			}
		}
        ecdh::key_pair_generate(None, &mut skE, &mut pkE);
	}
	if kem==2 || kem==4 {
		reverse(&mut pkR);
	}
    ecdh::ecpsvdp_dh(&skE, &pkR, &mut iz);
    ecdh::ecpsvdp_dh(&skI, &pkR, &mut nz);

	if kem==2 || kem==4 {
		reverse(&mut pkR);
		reverse(&mut pkE);
		reverse(&mut iz);
		reverse(&mut nz);
	}

	for i in 0..ecdh::EFS {
        z[i]=iz[i];
		z[i+ecdh::EFS] = nz[i];
	}
}

#[allow(non_snake_case)]
pub fn authdecap(config_id: usize,z: &mut [u8],mut pkE: &mut [u8],skr: &mut [u8],mut pkI: &mut [u8]) {
	let mut skR: [u8; ecdh::EGS] = [0; ecdh::EGS];
    let mut iz: [u8; ecdh::EFS] = [0; ecdh::EFS];
    let mut nz: [u8; ecdh::EFS] = [0; ecdh::EFS];
	let kem = config_id&7;
	for i in 0..ecdh::EGS {
		skR[i]=skr[i];
	}
	if kem==2 || kem==4 {
		reverse(&mut skR);
		reverse(&mut pkE);
		reverse(&mut pkI);
		if kem==2 {
			skR[ecdh::EGS-1]&=248;
			skR[0]&=127;
			skR[0]|=64;
		} else {
			skR[ecdh::EGS-1]&=252;
			skR[0]|=128;
		}
	}
    ecdh::ecpsvdp_dh(&skR, &pkE, &mut iz);
    ecdh::ecpsvdp_dh(&skR, &pkI, &mut nz);

	if kem==2 || kem==4 {
		reverse(&mut pkI);
		reverse(&mut pkE);
		reverse(&mut iz);
		reverse(&mut nz);
	}
	for i in 0..ecdh::EFS {
        z[i]=iz[i];
		z[i+ecdh::EFS] = nz[i];
	}
}

#[allow(non_snake_case)]
pub fn keyschedule(config_id: usize,mut key: &mut [u8],mut nonce: &mut [u8],mode: usize,pkR: &mut [u8],z: &mut [u8],pkE: &mut [u8],info: &[u8],psk: Option<&[u8]>,pskID: Option<&[u8]>,pkI: Option<&mut [u8]>) {
    let mut context: [u8; 550] = [0; 550];
	let mut full_context: [u8; 550] = [0; 550];
	let kem_id=config_id&7;
	let kdf_id=(config_id>>3)&3;
	let aead_id=(config_id>>5)&3;
    let mut k=0;
    let mut ar: [u8; 2] = [0; 2];
    let mut h: [u8; 64] = [0; 64];
    let mut secret: [u8; 64] = [0; 64];

    let ctxlen=7+3*pkR.len()+2*ecp::HASH_TYPE;

    context[k]=mode as u8; k+=1;
    hmac::inttobytes(kem_id,&mut ar);
    for i in 0..ar.len() {
		context[k] = ar[i]; k+=1;
    }
    hmac::inttobytes(kdf_id,&mut ar);
    for i in 0..ar.len() {
		context[k] = ar[i]; k+=1;
    }
   
    hmac::inttobytes(aead_id,&mut ar);
    for i in 0..ar.len() {
		context[k] = ar[i]; k+=1;
    }    

	for i in 0..pkE.len() { 
		context[k] = pkE[i]; k+=1;
    }
	for i in 0..pkR.len() { 
		context[k] = pkR[i]; k+=1;
  	}
    if let Some(rpkI) = pkI {
	    for i in 0..rpkI.len() { 
		    context[k] = rpkI[i]; k+=1;
        }
    } else {
	    for _ in 0..pkR.len() { 
		    context[k] = 0; k+=1;
        }
    }
              
	hmac::SPhashit(hmac::MC_SHA2,ecp::HASH_TYPE,&mut h,pskID);
	for i in 0..ecp::HASH_TYPE { 
		context[k] = h[i]; k+=1;
    }
	
	hmac::SPhashit(hmac::MC_SHA2,ecp::HASH_TYPE,&mut h,Some(&info));
	for i in 0..ecp::HASH_TYPE { 
		context[k] = h[i]; k+=1;
    }

    print!("Context= "); printbinary(&context[0..ctxlen]);
    hmac::hkdf_extract(hmac::MC_SHA2,ecp::HASH_TYPE,&mut secret,psk,&z);
    print!("Secret= "); printbinary(&secret[0..ecp::HASH_TYPE]);
	
    let pk: &[u8] = b"hpke key";
    let mut j=0;

	for i in 0..pk.len() { 
		full_context[j] = pk[i]; j+=1;
    }
 	for i in 0..ctxlen { 
		full_context[j] = context[i]; j+=1;
    }
    hmac:: hkdf_expand(hmac::MC_SHA2,ecp::HASH_TYPE,&mut key,ecp::AESKEY,&secret[0..ecp::HASH_TYPE],&full_context[0..ctxlen+pk.len()]);
 
    let pn: &[u8] = b"hpke nonce";
    j=0;

	for i in 0..pn.len() { 
		full_context[j] = pn[i]; j+=1;
    }
 	for i in 0..ctxlen { 
		full_context[j] = context[i]; j+=1;
    }
   
    hmac:: hkdf_expand(hmac::MC_SHA2,ecp::HASH_TYPE,&mut nonce,12,&secret[0..ecp::HASH_TYPE],&full_context[0..ctxlen+pn.len()]);
}
