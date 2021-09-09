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

extern crate mcore;

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

fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        print!("{:02X}", array[i])
    }
    println!("")
}

fn test(SK: &str,PK: &str) {
    use mcore::nist256::ecdh;

    const EFS: usize = ecdh::EFS;
    let mut ss: [u8; EFS] = [0; EFS];

    let mut sk: [u8; EFS]=[0;EFS];  
    let mut pk: [u8; 2*EFS+1]=[0;2*EFS+1];
    decode_hex(&mut sk,&SK);
    decode_hex(&mut pk,&PK);
    let res=ecdh::ecpsvdp_dh(&sk, &pk, &mut ss, 0);

    print!("Public  Key=   "); printbinary(&pk[0..PK.len()/2]);
    print!("Private Key=   "); printbinary(&sk);
	print!("Shared secret= "); printbinary(&ss);
    if res==0 {
        println!("valid");
    } else {
        println!("invalid");
    }
    println!("");
}

fn main() {

//tcId1
    test("0612465c89a023ab17855b0a6bcebfd3febb53aef84138647b5352e02c10c346",
    "0462d5bd3372af75fe85a040715d0f502428e07046868b0bfdfa61d731afe44f26ac333a93a9e70a81cd5a95b5bf8d13990eb741c8c38872b4a07d275a014e30cf");
//tcId2
    test("0612465c89a023ab17855b0a6bcebfd3febb53aef84138647b5352e02c10c346",
    "0362d5bd3372af75fe85a040715d0f502428e07046868b0bfdfa61d731afe44f26");
//tcId3
    test("0a0d622a47e48f6bc1038ace438c6f528aa00ad2bd1da5f13ee46bf5f633d71a",
    "0458fd4168a87795603e2b04390285bdca6e57de6027fe211dd9d25e2212d29e62080d36bd224d7405509295eed02a17150e03b314f96da37445b0d1d29377d12c");
//tcId8
    test("0a0d622a47e48f6bc1038ace438c6f528aa00ad2bd1da5f13ee46bf5f633d71a",
    "04767d7fbb84aa6a4db1079372644e42ecb2fec200c178822392cb8b950ffdd0c91c86853cafd09b52ba2f287f0ebaa26415a3cfabaf92c6a617a19988563d9dea");
//tcId26
    test("0a0d622a47e48f6bc1038ace438c6f528aa00ad2bd1da5f13ee46bf5f633d71a",
    "048e5d22d5e53ec797c55ecd68a08a7c3361cd99ca7fad1a68ea802a6a4cb58a918ea7a07023ef67677024bd3841e187c64b30a30a3750eb2ee873fbe58fa1357b"); 
//tcId19
    test("0a0d622a47e48f6bc1038ace438c6f528aa00ad2bd1da5f13ee46bf5f633d71a",
    "043cbc1b31b43f17dc200dd70c2944c04c6cb1b082820c234a300b05b7763844c74fde0a4ef93887469793270eb2ff148287da9265b0334f9e2609aac16e8ad503");
//tcId193
    test("7e4aa54f714bf01df85c50269bea3a86721f84afe74f7b41ea58abcf3474e88d",
    "0400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
//tcId210
    test("6f953faff3599e6c762d7f4cabfeed092de2add1df1bc5748c6cbb725cf35458",
    "02fd4bf61763b46581fd9174d623516cf3c81edd40e29ffa2777fb6cb0ae3ce535");
//tcId211
    test("d27edf0ff5b6b6b465753e7158370332c153b468a1be087ad0f490bdb99e5f02",
    "03efdde3b32872a9effcf3b94cbf73aa7b39f9683ece9121b9852167f4e3da609b");
}
