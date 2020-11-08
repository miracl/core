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
	let mut seedE=decode_hex("591c66abd531b9c8287cf76ac053efba38d61e994e7f656c30dab6723a8af9ce");
	let mut seedR=decode_hex("8a219e9a42233826f165d2c1036399fa84cfb3bcb93872bc49287dfbe6f1fec9");
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
	seedE=decode_hex("f39542a6e45dc0e6225de671455fea9d918d4fa241c7fb301895a138ce7c2f52");
	seedR=decode_hex("5aa0335739cb3dbafce48d5d9cccd7d499b0727b445522e1408aba8171aabfae");
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
	seedE=decode_hex("0b119a7a132355f3c2a0ad03e10e252d7292fcde480e726d03a81f127d02c050");
	seedR=decode_hex("874ba77ded181cd1cb4f2b4f37386a5b2b86c84c57d2a6d1b93a05fb5edd69f0");
	let mut seedS=decode_hex("2e369ba803dcee17dba926e4252b8dd9a47417a35bcf90f0e55e4af85c83153b");
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
	seedE=decode_hex("96360b18f7822f85f4a0a1dd200d00ab82ad0032b05a1f34cb120a0cbda4865a");
	seedR=decode_hex("48710faa5c8b8b2a4447c9ccf1cc232fb49f6c0fd04b151949757e15b414eea8");
	seedS=decode_hex("dd413f8d2d4b195a7cd0793453d04660203a51db65a57d16c2a720e6d44e526c");
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
	let mut seedE=decode_hex("3fdb7eab3a6a99de8abbcb507be5704ae6a8994008b8a8e6b63fbd97fa8619c66bd8665c22079939f3f63f978c5806802b22ba5bb396da9cf252ee67068bf57461bf");
	let mut seedR=decode_hex("2e99ac709379c7eb15ca068253bbae4dd6297c2397f47a89b8cb3ef4e83f235f83cb1ce3d2f754c47431ff0d0d8d2c429a7b6768d9524c3be60b9fb7749c49cb816b");
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
	seedE=decode_hex("ae300665d34d5ab7c0508a94a741ba2cb285966106ba9cefbe1f9c24c3eb626108d0c9ccc8291d90c50c6d04ac181ccd8efc2cc52383eb205637a84d2be5362bf247");
	seedR=decode_hex("dbbda5e5a54ee85cfe076e4746ceb971bed338a7fe0b625d6c7c28b3c82c8128258906a52543655ecb62889e9dbe8feec747d06e4216f88ca4adba200179e0e75276");
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
	seedE=decode_hex("11c0c7337b294452826e14a7f6c9e7981a03c467a08f47a8b478b37f3e9c90266898e3c3f8e84235a6a2837269c84b355d7f5ca133085172a08f00c3857da8a1410b");
	seedR=decode_hex("bb56b3452a80ad82b2d48c19ce76194a198eefdb67040959bc9e479db0682a4b5b46d7d020df66864d374b25deb5927144e3f08f2f9eacdd5f54b8b5c65d91ee211f");
	let mut seedS=decode_hex("bab663b9c05f680f401a494ae8c8714fd95cbcd56a01e9e8194b4b3da863a5e8313d4916dc58f6d3aaa2dafe420ae81b2a6c0075223afc6b13f3734a26ca30da5e38");
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
	seedE=decode_hex("4ebb461a5c46330d6de3a40d19ac499cc206733cf1a4fb3ba922d976aa1c45848668f04a3b5a4845a0d1c83755967d8914a9824fbb8823d161c16c93c51636e1ad89");
	seedR=decode_hex("1ae2f1008c46c7a6e9275b1e29c906475c6bc019b1dfc38cbce68c5233de9d33ba93fe9d7b9ea5beb04f4adc5a3b72238f6e3d904d29eb0680ea240103d3335a3c47");
	seedS=decode_hex("e0f2ada4f2a900fded767dc9868119ee3e4767afac667a780b68e5e2b4d7d363dbf02717ab314369c45f34dcec3de384a65e8453a971ad0353a507f34dc1d5d9b8f5");
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