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

extern crate core;
use core::gcm;

fn decode_hex(s: &str) -> Vec<u8> {
    let mut x: Vec<u8> = Vec::new();
    let mut len=s.chars().count();
    if (len%2) == 1 {
        return x;
    }
    len/=2;
    let mut j=0;
    let cv: Vec<char> = s.chars().collect();
    for _ in 0..len {
        let n1=cv[j] as usize;
        let n2=cv[j+1] as usize;
	    x.push(((n1 % 32 + 9) % 25 * 16 + (n2 % 32 + 9) % 25) as u8); j+=2;
    }
    x
}

fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

#[allow(non_snake_case)]
fn hpke_c25519() {
    //use core::ed25519;
    use core::c25519::hpke;
    use core::c25519::ecdh;
    use core::c25519::ecp;

    const EGS: usize = ecdh::EGS;
    const EFS: usize = ecdh::EFS;
    const EAS: usize = ecp::AESKEY;
    const EHS: usize = ecp::HASH_TYPE;

	let config_id=0x520;
    let mut skE: [u8; EGS] = [0; EGS];
    let mut skR: [u8; EGS] = [0; EGS];
    let mut skS: [u8; EGS] = [0; EGS];
    let mut pkE: [u8; EFS] = [0; EFS];
    let mut pkR: [u8; EFS] = [0; EFS];
    let mut pkS: [u8; EFS] = [0; EFS];

    let mut z: [u8;EHS] = [0;EHS];

    let mut key:[u8;EAS]=[0;EAS];
    let mut nonce:[u8;12]=[0;12];
    let mut exp_secret:[u8;EHS]=[0;EHS];

	let info=decode_hex("4f6465206f6e2061204772656369616e2055726e");
	let psk=decode_hex("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82");
	let pskID=decode_hex("456e6e796e20447572696e206172616e204d6f726961");
	let plain=decode_hex("4265617574792069732074727574682c20747275746820626561757479");
	let aad=decode_hex("436f756e742d30");

    let mut cipher:[u8;64]=[0;64];
    let mut tag:[u8;16]=[0;16];
    println!("\nTesting HPKE code for curve c25519");
// Mode 0
    let mut mode=0;
    println!("\nMode 0");
	let mut seedE=decode_hex("6305de86b3cec022fae6f2f2d2951f0f90c8662112124fd62f17e0a99bdbd08e");
	let mut seedR=decode_hex("6d9014e4609687b0a3670a22f2a14eac5ae6ad8c0beb62fb3ecb13dc8ebf5e06");
    hpke::deriveKeyPair(config_id,&mut skE,&mut pkE,&seedE);
    hpke::deriveKeyPair(config_id,&mut skR,&mut pkR,&seedR);

	hpke::encap(config_id,&skE,&mut z,&pkE,&pkR);
	print!("pkE= "); printbinary(&pkE);
	print!("Encapsulated z= "); printbinary(&z);

	hpke::decap(config_id,&skR,&mut z,&pkE,&pkR);
	print!("Decapsulated z= "); printbinary(&z);

	hpke::keyschedule(config_id,&mut key,&mut nonce,&mut exp_secret,mode,&mut z,&info,None,None);
	print!("key= "); printbinary(&key);
	print!("nonce= "); printbinary(&nonce);
	print!("exporter secret= "); printbinary(&exp_secret);

	gcm::encrypt(&mut cipher[0..plain.len()],&mut tag,&key,&nonce,&aad,&plain);
	print!("Cipher= ");  printbinary(&cipher[0..plain.len()]);
	print!("Tag= ");  printbinary(&tag);

// Mode 1
    mode=1;
    println!("\nMode 1");
	seedE=decode_hex("abd63dfd2fb9ccf8789cae5a6aff91e07f9f5925c27f005e702bf956b0000a85");
	seedR=decode_hex("654e8b44e8e29fc75f3beadf7f28dc065e38a53c1a731e15f2d46fd6130574da");
    hpke::deriveKeyPair(config_id,&mut skE,&mut pkE,&seedE);
    hpke::deriveKeyPair(config_id,&mut skR,&mut pkR,&seedR);
   
	hpke::encap(config_id,&skE,&mut z,&pkE,&pkR);
	print!("pkE= "); printbinary(&pkE);
	print!("Encapsulated z= "); printbinary(&z);

	hpke::decap(config_id,&skR,&mut z,&pkE,&pkR);
	print!("Decapsulated z= "); printbinary(&z);

	hpke::keyschedule(config_id,&mut key,&mut nonce,&mut exp_secret,mode,&mut z,&info,Some(&psk),Some(&pskID));
	print!("key= "); printbinary(&key);
	print!("nonce= "); printbinary(&nonce);
	print!("exporter secret= "); printbinary(&exp_secret);

	gcm::encrypt(&mut cipher[0..plain.len()],&mut tag,&key,&nonce,&aad,&plain);
	print!("Cipher= ");  printbinary(&cipher[0..plain.len()]);
	print!("Tag= ");  printbinary(&tag);

// Mode 2
    mode=2;
    println!("\nMode 2");
	seedE=decode_hex("42d691088397246b00e9d9ce8f5406a317433558dc28132e02618970005d02fd");
	seedR=decode_hex("fb953f486ef5f7a1ceddff40bffe02b857c8af9d611966e417a24d6efa7c9d1c");
	let mut seedS=decode_hex("131aa907c85b05726e7a058b064bf29cb2cb72a2afbffbd8076a884291f3143e");
    hpke::deriveKeyPair(config_id,&mut skE,&mut pkE,&seedE);
    hpke::deriveKeyPair(config_id,&mut skR,&mut pkR,&seedR);
    hpke::deriveKeyPair(config_id,&mut skS,&mut pkS,&seedS);
   
	hpke::authencap(config_id,&skE,&skS,&mut z,&pkE,&pkR,&pkS);
	print!("pkE= "); printbinary(&pkE);
	print!("Encapsulated z= "); printbinary(&z);

	hpke::authdecap(config_id,&skR,&mut z,&pkE,&pkR,&pkS);
	print!("Decapsulated z= "); printbinary(&z);

	hpke::keyschedule(config_id,&mut key,&mut nonce,&mut exp_secret,mode,&mut z,&info,None,None);
	print!("key= "); printbinary(&key);
	print!("nonce= "); printbinary(&nonce);
	print!("exporter secret= "); printbinary(&exp_secret);

	gcm::encrypt(&mut cipher[0..plain.len()],&mut tag,&key,&nonce,&aad,&plain);
	print!("Cipher= ");  printbinary(&cipher[0..plain.len()]);
	print!("Tag= ");  printbinary(&tag);

// Mode 3
    mode=3;
    println!("\nMode 3");
	seedE=decode_hex("0c3a14fe896a7166f4d5e2a21c903b69f9ca71783290ca10f1b8c5eb258149be");
	seedR=decode_hex("b759021868fba28c1ddd509eaac450a896ab0f5edffead0a019fecb574950d64");
	seedS=decode_hex("2e7219703b6659698e4c2d141d13e0092df7039212db9c97e347c7d2c0aee239");
    hpke::deriveKeyPair(config_id,&mut skE,&mut pkE,&seedE);
    hpke::deriveKeyPair(config_id,&mut skR,&mut pkR,&seedR);
    hpke::deriveKeyPair(config_id,&mut skS,&mut pkS,&seedS);

	hpke::authencap(config_id,&skE,&skS,&mut z,&pkE,&pkR,&pkS);
	print!("pkE= "); printbinary(&pkE);
	print!("Encapsulated z= "); printbinary(&z);

	hpke::authdecap(config_id,&skR,&mut z,&pkE,&pkR,&pkS);
	print!("Decapsulated z= "); printbinary(&z);

	hpke::keyschedule(config_id,&mut key,&mut nonce,&mut exp_secret,mode,&mut z,&info,Some(&psk),Some(&pskID));
	print!("key= "); printbinary(&key);
	print!("nonce= "); printbinary(&nonce);
	print!("exporter secret= "); printbinary(&exp_secret);

	gcm::encrypt(&mut cipher[0..plain.len()],&mut tag,&key,&nonce,&aad,&plain);
	print!("Cipher= ");  printbinary(&cipher[0..plain.len()]);
	print!("Tag= ");  printbinary(&tag);

}

#[allow(non_snake_case)]
fn hpke_nist521() {
    //use core::ed25519;
    use core::nist521::hpke;
    use core::nist521::ecdh;
    use core::nist521::ecp;

    const EGS: usize = ecdh::EGS;
    const EFS: usize = ecdh::EFS;
    const EAS: usize = ecp::AESKEY;
    const EHS: usize = ecp::HASH_TYPE;

	let config_id=0xB12;
    let mut skE: [u8; EGS] = [0; EGS];
    let mut skR: [u8; EGS] = [0; EGS];
    let mut skS: [u8; EGS] = [0; EGS];
    let mut pkE: [u8; 2*EFS+1] = [0; 2*EFS+1];
    let mut pkR: [u8; 2*EFS+1] = [0; 2*EFS+1];
    let mut pkS: [u8; 2*EFS+1] = [0; 2*EFS+1];

    let mut z: [u8;EHS] = [0; EHS];

    let mut key:[u8;EAS]=[0;EAS];
    let mut nonce:[u8;12]=[0;12];
    let mut exp_secret:[u8;EHS]=[0;EHS];

	let info=decode_hex("4f6465206f6e2061204772656369616e2055726e");
	let psk=decode_hex("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82");
	let pskID=decode_hex("456e6e796e20447572696e206172616e204d6f726961");
	let plain=decode_hex("4265617574792069732074727574682c20747275746820626561757479");
	let aad=decode_hex("436f756e742d30");

    let mut cipher:[u8;EAS]=[0;EAS];
    let mut tag:[u8;16]=[0;16];
    println!("\nTesting HPKE code for curve nist521");
// Mode 0
    let mut mode=0;
    println!("\nMode 0");
	let mut seedE=decode_hex("ea39fec1941c5f516e19533f40d415c65fde023c10c559f3845e71ffccea478101573d069cc67874d5b2aba6a22eb51cdd689836b7e9cabbb4469c57947db7316fa7");
	let mut seedR=decode_hex("8249fd42416aba5b0d51dcd3548d774ae172148cbba1519107c5d84a160225441a9c018fdf3b9ffc2c41c1c62e29208d5165a59e7f14fe93b4f911cbbebda1904391");
    hpke::deriveKeyPair(config_id,&mut skE,&mut pkE,&seedE);
    hpke::deriveKeyPair(config_id,&mut skR,&mut pkR,&seedR);   

	hpke::encap(config_id,&skE,&mut z,&pkE,&pkR);
	print!("pkE= "); printbinary(&pkE);
	print!("Encapsulated z= "); printbinary(&z);

	hpke::decap(config_id,&skR,&mut z,&pkE,&pkR);
	print!("Decapsulated z= "); printbinary(&z);

	hpke::keyschedule(config_id,&mut key,&mut nonce,&mut exp_secret,mode,&mut z,&info,None,None);
	print!("key= "); printbinary(&key);
	print!("nonce= "); printbinary(&nonce);
	print!("exporter secret= "); printbinary(&exp_secret);

	gcm::encrypt(&mut cipher[0..plain.len()],&mut tag,&key,&nonce,&aad,&plain);
	print!("Cipher= ");  printbinary(&cipher[0..plain.len()]);
	print!("Tag= ");  printbinary(&tag);

// Mode 1
    mode=1;
    println!("\nMode 1");
	seedE=decode_hex("b563555965facaa37a5e754cf2e50193953e1d527e61637e521df1868354799258f0d15d8807750cea08e9b6a358d2440ab06725861b237fb69973cf41e802434433");
	seedR=decode_hex("2be954cd856c0c548ec6de490c821be20c4fc9610b5dba1831c698045fa01dbceb49f8324bae26e5d1ee62db3246d65492812e2c539b96bc580d46c247304adfc55e");
    hpke::deriveKeyPair(config_id,&mut skE,&mut pkE,&seedE);
    hpke::deriveKeyPair(config_id,&mut skR,&mut pkR,&seedR);   
   
	hpke::encap(config_id,&skE,&mut z,&pkE,&pkR);
	print!("pkE= "); printbinary(&pkE);
	print!("Encapsulated z= "); printbinary(&z);

	hpke::decap(config_id,&skR,&mut z,&pkE,&pkR);
	print!("Decapsulated z= "); printbinary(&z);

	hpke::keyschedule(config_id,&mut key,&mut nonce,&mut exp_secret,mode,&mut z,&info,Some(&psk),Some(&pskID));
	print!("key= "); printbinary(&key);
	print!("nonce= "); printbinary(&nonce);
	print!("exporter secret= "); printbinary(&exp_secret);

	gcm::encrypt(&mut cipher[0..plain.len()],&mut tag,&key,&nonce,&aad,&plain);
	print!("Cipher= ");  printbinary(&cipher[0..plain.len()]);
	print!("Tag= ");  printbinary(&tag);

// Mode 2
    mode=2;
    println!("\nMode 2");
	seedE=decode_hex("c9621b9ef899275dc970606a2b0806fe860f62d539f3ee618a9409009b8ae154bc4acf495dd9fa8f850c4dca82b923b42270d7b16ed343c7e86e3036c88d0d7d77ee");
	seedR=decode_hex("ef805c20cda1fa06e06cfb968ad68c748a3dd94337f7357ca0060a382a84fa5de6df3e3216f886957694547264d5bc63450cfdcc4d2b33fc8ebf8d7c708f8b5e4bb5");
	let mut seedS=decode_hex("d8779e14425887ebb21b1952b1a0b77842830aef910724b082807dfebc8ec309b4969da762369e77834593970215b85510c9a0347ff14c8583aae7c9c2208275b740");
    hpke::deriveKeyPair(config_id,&mut skE,&mut pkE,&seedE);
    hpke::deriveKeyPair(config_id,&mut skR,&mut pkR,&seedR);   
    hpke::deriveKeyPair(config_id,&mut skS,&mut pkS,&seedS);   
    
	hpke::authencap(config_id,&skE,&skS,&mut z,&pkE,&pkR,&pkS);
	print!("pkE= "); printbinary(&pkE);
	print!("Encapsulated z= "); printbinary(&z);

	hpke::authdecap(config_id,&skR,&mut z,&pkE,&pkR,&pkS);
	print!("Decapsulated z= "); printbinary(&z);

	hpke::keyschedule(config_id,&mut key,&mut nonce,&mut exp_secret,mode,&mut z,&info,None,None);
	print!("key= "); printbinary(&key);
	print!("nonce= "); printbinary(&nonce);
	print!("exporter secret= "); printbinary(&exp_secret);

	gcm::encrypt(&mut cipher[0..plain.len()],&mut tag,&key,&nonce,&aad,&plain);
	print!("Cipher= ");  printbinary(&cipher[0..plain.len()]);
	print!("Tag= ");  printbinary(&tag);

// Mode 3
    mode=3;
    println!("\nMode 3");
	seedE=decode_hex("d7537fd470c0beece615e26dd109922460292e973127cb4e22da16c0756fc336224e07dbecdf36edd144ebcd82aece3db52f814a33a639b5e7c964b27f6e3195cd73");
	seedR=decode_hex("f0858f5e1865db4fe45dc3274bcd273a29088d80f9203a16ec1210e3d81dd50a99f15c427d547fea55593e2ef834beb5f80c536fdd2881a8943c05488a371a3c988d");
	seedS=decode_hex("1e8d0026273feb61537181872e03ed2c7756f0ed1c4bb9ecd159614c2afdcaacc5fcf70f6d30d7ea6760c98a1ce1138a82497eb72461ca5da50c8729d431de53857f");
    hpke::deriveKeyPair(config_id,&mut skE,&mut pkE,&seedE);
    hpke::deriveKeyPair(config_id,&mut skR,&mut pkR,&seedR);   
    hpke::deriveKeyPair(config_id,&mut skS,&mut pkS,&seedS);   

	hpke::authencap(config_id,&skE,&skS,&mut z,&pkE,&pkR,&pkS);
	print!("pkE= "); printbinary(&pkE);
	print!("Encapsulated z= "); printbinary(&z);

	hpke::authdecap(config_id,&skR,&mut z,&pkE,&pkR,&pkS);
	print!("Decapsulated z= "); printbinary(&z);

	hpke::keyschedule(config_id,&mut key,&mut nonce,&mut exp_secret,mode,&mut z,&info,Some(&psk),Some(&pskID));
	print!("key= "); printbinary(&key);
	print!("nonce= "); printbinary(&nonce);
	print!("exporter secret= "); printbinary(&exp_secret);

	gcm::encrypt(&mut cipher[0..plain.len()],&mut tag,&key,&nonce,&aad,&plain);
	print!("Cipher= ");  printbinary(&cipher[0..plain.len()]);
	print!("Tag= ");  printbinary(&tag);

}

fn main() {

    hpke_c25519();
    hpke_nist521();
}