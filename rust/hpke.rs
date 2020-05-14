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

const GROUP: usize = ecdh::EGS;
const FIELD: usize = ecdh::EFS;  
const POINT: usize = 2*ecdh::EFS+1;

const MAX_LABEL: usize = 20;                // may need adjustment

#[allow(non_snake_case)]
fn reverse (x: &mut [u8]) {
	let lx=x.len();
	for i in 0..lx/2 {
		let ch=x[i];
		x[i]=x[lx-i-1];
		x[lx-i-1]=ch;
	}
}
/*
fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}
*/

#[allow(non_snake_case)]
fn labeledExtract(prk: &mut [u8],salt: &[u8],label: &str,ikm: Option<&[u8]>) {
    let rfc=concat!("RFCXX","XX ");
    let prefix1=rfc.as_bytes();
    let prefix2=label.as_bytes();
    let mut likm: [u8; 8+MAX_LABEL+POINT] = [0; 8+MAX_LABEL+POINT];
    let mut k=0;
    for i in 0..prefix1.len() {
        likm[k]=prefix1[i];
        k+=1;
    }
    for i in 0..prefix2.len() {
        likm[k]=prefix2[i];
        k+=1;
    }
    if let Some(sikm) = ikm {
        for i in 0..sikm.len() {
            likm[k]=sikm[i];
            k+=1;
        }
    }
    hmac::hkdf_extract(hmac::MC_SHA2,ecp::HASH_TYPE,prk,Some(&salt),&likm[0..k]);
}

#[allow(non_snake_case)]
fn labeledExpand(okm: &mut [u8],prk: &[u8],label: &str,info: &[u8],el: usize) {
    let mut ar: [u8; 2] = [0; 2];
    let rfc=concat!("RFCXX","XX ");
    let prefix1=rfc.as_bytes();
    let prefix2=label.as_bytes();
    hmac::inttobytes(el,&mut ar);
    let mut linfo: [u8; 10+MAX_LABEL+3*POINT] = [0; 10+MAX_LABEL+3*POINT];
    linfo[0]=ar[0];
    linfo[1]=ar[1];
    let mut k=2;
    for i in 0..prefix1.len() {
        linfo[k]=prefix1[i];
        k+=1;
    }
    for i in 0..prefix2.len() {
        linfo[k]=prefix2[i];
        k+=1;
    }
    for i in 0..info.len() {
        linfo[k]=info[i];
        k+=1;
    }
    hmac:: hkdf_expand(hmac::MC_SHA2,ecp::HASH_TYPE,okm,el,prk,&linfo[0..k]);
}

#[allow(non_snake_case)]
fn extractAndExpand(okm: &mut [u8],dh: &[u8],context: &[u8]) {
    let zeros: [u8;ecp::HASH_TYPE]=[0;ecp::HASH_TYPE];
    let mut prk: [u8;ecp::HASH_TYPE]=[0;ecp::HASH_TYPE];
    labeledExtract(&mut prk,&zeros,"dh",Some(dh));
    labeledExpand(okm,&prk,"prk",context,ecp::HASH_TYPE);
}

#[allow(non_snake_case)]
pub fn encap(config_id: usize,ske: &mut [u8],z: &mut [u8],mut pkE: &mut [u8],mut pkR: &mut [u8]) {
    let mut skE: [u8; GROUP] = [0; GROUP];
    let mut dh: [u8; FIELD] = [0; FIELD];
    let mut kemcontext: [u8; 2*POINT] = [0;2*POINT];
    let kem = config_id&255;

    for i in 0..GROUP {
		skE[i]=ske[i];
	}
	if kem==32 || kem==33 {
		reverse(&mut skE);
		if kem==32 {
			skE[GROUP-1]&=248;
			skE[0]&=127;
			skE[0]|=64;
		} else {
			skE[GROUP-1]&=252;
			skE[0]|=128;
		}
	}
	ecdh::key_pair_generate(None, &mut skE, &mut pkE);
    
	if kem==32 || kem==33 {
		reverse(&mut pkR);
	}
	ecdh::ecpsvdp_dh(&skE, &pkR, &mut dh);
	if kem==32 || kem==33 {
		reverse(&mut pkR);
		reverse(&mut pkE);
		reverse(&mut dh);
	}
    let mut k=0;
    for i in 0..pkE.len() {
        kemcontext[k]=pkE[i];
        k+=1;
    }
    for i in 0..pkR.len() {
        kemcontext[k]=pkR[i];
        k+=1;
    }

    extractAndExpand(z,&dh,&kemcontext[0..k]);
}

#[allow(non_snake_case)]
pub fn decap(config_id: usize,z: &mut [u8],mut pkE: &mut [u8],skr: &mut [u8]) {
	let mut skR: [u8; ecdh::EGS] = [0; ecdh::EGS];
    let mut dh: [u8; FIELD] = [0; FIELD];
    let mut kemcontext: [u8; 2*POINT] = [0;2*POINT];
    let mut pkR: [u8;POINT]=[0;POINT];

	let kem = config_id&255;
	for i in 0..GROUP {
		skR[i]=skr[i];
	}
	if kem==32 || kem==33 {
		reverse(&mut skR);
		reverse(&mut pkE);
		if kem==32 {
			skR[GROUP-1]&=248;
			skR[0]&=127;
			skR[0]|=64;
		} else {
			skR[GROUP-1]&=252;
			skR[0]|=128;
		}
	}
	ecdh::ecpsvdp_dh(&skR, &pkE, &mut dh);
	if kem==32 || kem==33 {
		reverse(&mut pkE);
		reverse(&mut dh);
	}
	ecdh::key_pair_generate(None, &mut skR, &mut pkR);

	if kem==32 || kem==33 {
		reverse(&mut pkR[0..pkE.len()]);
	}
    let mut k=0;
    for i in 0..pkE.len() {   
        kemcontext[k]=pkE[i];
        k+=1;
    }
    for i in 0..pkE.len() {  // not a mistake
        kemcontext[k]=pkR[i];
        k+=1;
    }

    extractAndExpand(z,&dh,&kemcontext[0..k]);
}

#[allow(non_snake_case)]
pub fn authencap(config_id: usize,ske: &mut [u8],z: &mut [u8],mut pkE: &mut [u8],mut pkR: &mut [u8],ski: &mut [u8]) {
	let mut skE: [u8; ecdh::EGS] = [0; ecdh::EGS];
	let mut skS: [u8; ecdh::EGS] = [0; ecdh::EGS];
    let mut dh: [u8; FIELD] = [0; FIELD];
    let mut dh1: [u8; FIELD] = [0; FIELD];
    let mut zz: [u8; 2*FIELD] = [0; 2*FIELD];
    let mut pkS: [u8;POINT]=[0;POINT];
    let mut kemcontext: [u8; 3*POINT] = [0;3*POINT];
	let kem = config_id&255;
    let pklen=pkE.len();

	for i in 0..GROUP {
		skE[i]=ske[i];
		skS[i]=ski[i];
	}	
	if kem==32 || kem==33 {
		reverse(&mut skE);
		reverse(&mut skS);
		if kem==32 {
			skE[GROUP-1]&=248;
			skE[0]&=127;
			skE[0]|=64;
			skS[GROUP-1]&=248;
			skS[0]&=127;
			skS[0]|=64;	
		} else {
			skE[GROUP-1]&=252;
			skE[0]|=128;
			skS[GROUP-1]&=252;
			skS[0]|=128;
		}
	}
    ecdh::key_pair_generate(None, &mut skE, &mut pkE);
	
	if kem==32 || kem==33 {
		reverse(&mut pkR);
	}
    ecdh::ecpsvdp_dh(&skE, &pkR, &mut dh);
    ecdh::ecpsvdp_dh(&skS, &pkR, &mut dh1);

	if kem==32 || kem==33 {
		reverse(&mut dh);
		reverse(&mut dh1);
	}

	for i in 0..FIELD {
        zz[i]=dh[i];
		zz[i+FIELD] = dh1[i];
	}

	ecdh::key_pair_generate(None, &mut skS, &mut pkS);

	if kem==32 || kem==33 {
		reverse(&mut pkR);
		reverse(&mut pkE);
		reverse(&mut pkS[0..pklen]);
    }
    for i in 0..pklen {
        kemcontext[i]=pkE[i];
        kemcontext[pklen+i]= pkR[i];
        kemcontext[2*pklen+i]= pkS[i];
    }
    extractAndExpand(z,&zz,&kemcontext);
}

#[allow(non_snake_case)]
pub fn authdecap(config_id: usize,z: &mut [u8],mut pkE: &mut [u8],skr: &mut [u8],mut pkS: &mut [u8]) {
	let mut skR: [u8; ecdh::EGS] = [0; ecdh::EGS];
    let mut dh: [u8; FIELD] = [0; FIELD];
    let mut dh1: [u8; FIELD] = [0; FIELD];
    let mut zz: [u8; 2*FIELD] = [0; 2*FIELD];
    let mut pkR: [u8;POINT]=[0;POINT];
    let mut kemcontext: [u8; 3*POINT] = [0;3*POINT];
	let kem = config_id&255;
    let pklen=pkE.len();

	for i in 0..GROUP {
		skR[i]=skr[i];
	}
	if kem==32 || kem==33 {
		reverse(&mut skR);
		reverse(&mut pkE);
		reverse(&mut pkS);
		if kem==32 {
			skR[GROUP-1]&=248;
			skR[0]&=127;
			skR[0]|=64;
		} else {
			skR[GROUP-1]&=252;
			skR[0]|=128;
		}
	}
    ecdh::ecpsvdp_dh(&skR, &pkE, &mut dh);
    ecdh::ecpsvdp_dh(&skR, &pkS, &mut dh1);

	if kem==32 || kem==33 {
		reverse(&mut dh);
		reverse(&mut dh1);
	}
	for i in 0..ecdh::EFS {
        zz[i]=dh[i];
		zz[i+FIELD] = dh1[i];
	}
	ecdh::key_pair_generate(None, &mut skR, &mut pkR);

	if kem==32 || kem==33 {
		reverse(&mut pkS);
		reverse(&mut pkE);
		reverse(&mut pkR[0..pklen]);
    }
    for i in 0..pklen {
        kemcontext[i]=pkE[i];
        kemcontext[pklen+i]= pkR[i];
        kemcontext[2*pklen+i]= pkS[i];
    }
    extractAndExpand(z,&zz,&kemcontext);
}

#[allow(non_snake_case)]
pub fn keyschedule(config_id: usize,key: &mut [u8],nonce: &mut [u8],exp_secret: &mut [u8],mode: usize,z: &mut [u8],info: &[u8],psk: Option<&[u8]>,pskID: Option<&[u8]>) {

    let mut context: [u8; 7+2*ecp::HASH_TYPE] = [0; 7+2*ecp::HASH_TYPE];
    let zeros: [u8;ecp::HASH_TYPE]=[0;ecp::HASH_TYPE];
	let kem_id=config_id&255;
	let kdf_id=(config_id>>8)&3;
	let aead_id=(config_id>>10)&3;

    let mut k=0;
    let mut ar: [u8; 2] = [0; 2];
    let mut h: [u8; 64] = [0; 64];
    let mut secret: [u8; 64] = [0; 64];

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
    context[k]=mode as u8; k+=1;

    labeledExtract(&mut h,&zeros,"pskID_hash",pskID);
    for i in 0..ecp::HASH_TYPE {
		context[k] = h[i]; k+=1;
    }  
    labeledExtract(&mut h,&zeros,"info",Some(&info));
    for i in 0..ecp::HASH_TYPE {
		context[k] = h[i]; k+=1;
    }
    if let Some(spsk) = psk {
        labeledExtract(&mut h,&zeros,"psk_hash",Some(&spsk));
    } else {
        labeledExtract(&mut h,&zeros,"psk_hash",Some(&zeros));
    }
    labeledExtract(&mut secret,&h,"zz",Some(z));
    labeledExpand(key,&secret,"key",&context[0..k],ecp::AESKEY);
    labeledExpand(nonce,&secret,"nonce",&context[0..k],12);
    labeledExpand(exp_secret,&secret,"exp",&context[0..k],ecp::HASH_TYPE);
}
