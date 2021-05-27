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

//
//  TestHPKE.swift
//
//  Created by Michael Scott on 20/12/2019.
//  Copyright (c) 2019 Michael Scott. All rights reserved.
//

/* Test HPKE API */

import Foundation
import core // comment out for Xcode
import c25519
import nist521

public func printBinary(_ array: [UInt8])
{
    for i in 0 ..< array.count
    {
        let h=String(format:"%02x",array[i])
        print("\(h)", terminator: "")
    }
    print(" ")
}

public func decode_hex(_ s: String) -> [UInt8] {
    var x=[UInt8]()
    var len=s.count
    if (len%2) == 1 {
        return x
    }
    len/=2
    var j=0

    let arr: [UInt8] = Array(s.utf8)
    for _ in 0..<len {
        let n1=arr[j]
        let n2=arr[j+1]
        x.append(UInt8((n1 % 32 + 9) % 25 * 16 + (n2 % 32 + 9) % 25)); j+=2
    }

    return x
}

public func TestHPKE_c25519()
{
    let EFS=c25519.ECDH.EFS
    let EGS=c25519.ECDH.EGS

    let config_id=0x520
    var skE=[UInt8](repeating: 0,count: EGS)
    var skR=[UInt8](repeating: 0,count: EGS)
    var skS=[UInt8](repeating: 0,count: EGS)
    var pkE=[UInt8](repeating: 0,count: EFS)
    var pkR=[UInt8](repeating: 0,count: EFS)
    var pkS=[UInt8](repeating: 0,count: EFS)

    let info=decode_hex("4f6465206f6e2061204772656369616e2055726e")
    let psk=decode_hex("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82")
    let pskID=decode_hex("456e6e796e20447572696e206172616e204d6f726961")
    let plain=decode_hex("4265617574792069732074727574682c20747275746820626561757479")
    let aad=decode_hex("436f756e742d30")

    print("\nTesting HPKE for curve c25519")

// Mode 0
    var mode=0;
    print("\nMode 0");
    var seedE=decode_hex("7268600d403fce431561aef583ee1613527cff655c1343f29812e66706df3234")
    var seedR=decode_hex("6db9df30aa07dd42ee5e8181afdb977e538f5e1fec8a06223f33f7013e525037")
    c25519.HPKE.DeriveKeyPair(config_id,&skE,&pkE,seedE)
    c25519.HPKE.DeriveKeyPair(config_id,&skR,&pkR,seedR)

    var Z=c25519.HPKE.ENCAP(config_id,skE,pkE,pkR)
    print("pkE= ",terminator:""); printBinary(pkE)
    print("Encapsulated Z= ",terminator:""); printBinary(Z)

    Z=c25519.HPKE.DECAP(config_id,skR,pkE,pkR)
    print("Decapsulated Z= ",terminator:""); printBinary(Z)

    var (key,nonce,exp_secret)=c25519.HPKE.KEYSCHEDULE(config_id,mode,Z,info,nil,nil);
    print("key= ",terminator:""); printBinary(key)
    print("nonce= ",terminator:""); printBinary(nonce)
    print("exporter secret= ",terminator:""); printBinary(exp_secret)

    var (cipher,tag)=GCM.ENCRYPT(key,nonce,aad,plain)
    print("cipher= ",terminator:""); printBinary(cipher)
    print("tag= ",terminator:""); printBinary(tag)

// Mode 1
    mode=1;
    print("\nMode 1");
    seedE=decode_hex("78628c354e46f3e169bd231be7b2ff1c77aa302460a26dbfa15515684c00130b")
    seedR=decode_hex("d4a09d09f575fef425905d2ab396c1449141463f698f8efdb7accfaff8995098")
    c25519.HPKE.DeriveKeyPair(config_id,&skE,&pkE,seedE)
    c25519.HPKE.DeriveKeyPair(config_id,&skR,&pkR,seedR)

    Z=c25519.HPKE.ENCAP(config_id,skE,pkE,pkR)
    print("pkE= ",terminator:""); printBinary(pkE)
    print("Encapsulated Z= ",terminator:""); printBinary(Z)

    Z=c25519.HPKE.DECAP(config_id,skR,pkE,pkR)
    print("Decapsulated Z= ",terminator:""); printBinary(Z)

    (key,nonce,exp_secret)=c25519.HPKE.KEYSCHEDULE(config_id,mode,Z,info,psk,pskID);
    print("key= ",terminator:""); printBinary(key)
    print("nonce= ",terminator:""); printBinary(nonce)
    print("exporter secret= ",terminator:""); printBinary(exp_secret)

    (cipher,tag)=GCM.ENCRYPT(key,nonce,aad,plain)
    print("cipher= ",terminator:""); printBinary(cipher)
    print("tag= ",terminator:""); printBinary(tag)

// Mode 2
    mode=2;
    print("\nMode 2");
    seedE=decode_hex("6e6d8f200ea2fb20c30b003a8b4f433d2f4ed4c2658d5bc8ce2fef718059c9f7")
    seedR=decode_hex("f1d4a30a4cef8d6d4e3b016e6fd3799ea057db4f345472ed302a67ce1c20cdec")
    var seedS=decode_hex("94b020ce91d73fca4649006c7e7329a67b40c55e9e93cc907d282bbbff386f58")
    c25519.HPKE.DeriveKeyPair(config_id,&skE,&pkE,seedE)
    c25519.HPKE.DeriveKeyPair(config_id,&skR,&pkR,seedR)
    c25519.HPKE.DeriveKeyPair(config_id,&skS,&pkS,seedS)

    Z=c25519.HPKE.AUTHENCAP(config_id,skE,skS,pkE,pkR,pkS)
    print("pkE= ",terminator:""); printBinary(pkE)
    print("Encapsulated Z= ",terminator:""); printBinary(Z)

    Z=c25519.HPKE.AUTHDECAP(config_id,skR,pkE,pkR,pkS)
    print("Decapsulated Z= ",terminator:""); printBinary(Z)

    (key,nonce,exp_secret)=c25519.HPKE.KEYSCHEDULE(config_id,mode,Z,info,nil,nil);
    print("key= ",terminator:""); printBinary(key)
    print("nonce= ",terminator:""); printBinary(nonce)
    print("exporter secret= ",terminator:""); printBinary(exp_secret)

    (cipher,tag)=GCM.ENCRYPT(key,nonce,aad,plain)
    print("cipher= ",terminator:""); printBinary(cipher)
    print("tag= ",terminator:""); printBinary(tag)

// Mode 3
    mode=3;
    print("\nMode 3");
    seedE=decode_hex("4303619085a20ebcf18edd22782952b8a7161e1dbae6e46e143a52a96127cf84")
    seedR=decode_hex("4b16221f3b269a88e207270b5e1de28cb01f847841b344b8314d6a622fe5ee90")
    seedS=decode_hex("62f77dcf5df0dd7eac54eac9f654f426d4161ec850cc65c54f8b65d2e0b4e345")
    c25519.HPKE.DeriveKeyPair(config_id,&skE,&pkE,seedE)
    c25519.HPKE.DeriveKeyPair(config_id,&skR,&pkR,seedR)
    c25519.HPKE.DeriveKeyPair(config_id,&skS,&pkS,seedS)

    Z=c25519.HPKE.AUTHENCAP(config_id,skE,skS,pkE,pkR,pkS)
    print("pkE= ",terminator:""); printBinary(pkE)
    print("Encapsulated Z= ",terminator:""); printBinary(Z)

    Z=c25519.HPKE.AUTHDECAP(config_id,skR,pkE,pkR,pkS)
    print("Decapsulated Z= ",terminator:""); printBinary(Z)

    (key,nonce,exp_secret)=c25519.HPKE.KEYSCHEDULE(config_id,mode,Z,info,psk,pskID);
    print("key= ",terminator:""); printBinary(key)
    print("nonce= ",terminator:""); printBinary(nonce)
    print("exporter secret= ",terminator:""); printBinary(exp_secret)

    (cipher,tag)=GCM.ENCRYPT(key,nonce,aad,plain)
    print("cipher= ",terminator:""); printBinary(cipher)
    print("tag= ",terminator:""); printBinary(tag)
}

public func TestHPKE_nist521()
{
    let EGS=nist521.ECDH.EGS
    let EFS=nist521.ECDH.EFS

    var skE=[UInt8](repeating: 0,count: EGS)
    var skR=[UInt8](repeating: 0,count: EGS)
    var skS=[UInt8](repeating: 0,count: EGS)
    var pkE=[UInt8](repeating: 0,count: 2*EFS+1)
    var pkR=[UInt8](repeating: 0,count: 2*EFS+1)
    var pkS=[UInt8](repeating: 0,count: 2*EFS+1)

    let config_id=0xB12
 
    let info=decode_hex("4f6465206f6e2061204772656369616e2055726e")
    let psk=decode_hex("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82")
    let pskID=decode_hex("456e6e796e20447572696e206172616e204d6f726961")
    let plain=decode_hex("4265617574792069732074727574682c20747275746820626561757479")
    let aad=decode_hex("436f756e742d30")

    print("\nTesting HPKE for curve nist521")

// Mode 0
    var mode=0
    print("\nMode 0")
    var seedE=decode_hex("7f06ab8215105fc46aceeb2e3dc5028b44364f960426eb0d8e4026c2f8b5d7e7a986688f1591abf5ab753c357a5d6f0440414b4ed4ede71317772ac98d9239f70904")
    var seedR=decode_hex("2ad954bbe39b7122529f7dde780bff626cd97f850d0784a432784e69d86eccaade43b6c10a8ffdb94bf943c6da479db137914ec835a7e715e36e45e29b587bab3bf1")
    nist521.HPKE.DeriveKeyPair(config_id,&skE,&pkE,seedE)
    nist521.HPKE.DeriveKeyPair(config_id,&skR,&pkR,seedR)

    var Z=nist521.HPKE.ENCAP(config_id,skE,pkE,pkR)
    print("pkE= ",terminator:""); printBinary(pkE)
    print("Encapsulated Z= ",terminator:""); printBinary(Z)

    Z=nist521.HPKE.DECAP(config_id,skR,pkE,pkR)
    print("Decapsulated Z= ",terminator:""); printBinary(Z)

    var (key,nonce,exp_secret)=nist521.HPKE.KEYSCHEDULE(config_id,mode,Z,info,nil,nil);
    print("key= ",terminator:""); printBinary(key)
    print("nonce= ",terminator:""); printBinary(nonce)
    print("exporter secret= ",terminator:""); printBinary(exp_secret)

    var (cipher,tag)=GCM.ENCRYPT(key,nonce,aad,plain)
    print("cipher= ",terminator:""); printBinary(cipher)
    print("tag= ",terminator:""); printBinary(tag)

// Mode 1
    mode=1
    print("\nMode 1")
    seedE=decode_hex("f3ebfa9a69a924e672114fcd9e06fa9559e937f7eccce4181a2b506df53dbe514be12f094bb28e01de19dd345b4f7ede5ad7eaa6b9c3019592ec68eaae9a14732ce0")
    seedR=decode_hex("a2a2458705e278e574f835effecd18232f8a4c459e7550a09d44348ae5d3b1ea9d95c51995e657ad6f7cae659f5e186126a471c017f8f5e41da9eba74d4e0473e179")
    nist521.HPKE.DeriveKeyPair(config_id,&skE,&pkE,seedE)
    nist521.HPKE.DeriveKeyPair(config_id,&skR,&pkR,seedR)

    Z=nist521.HPKE.ENCAP(config_id,skE,pkE,pkR)
    print("pkE= ",terminator:""); printBinary(pkE)
    print("Encapsulated Z= ",terminator:""); printBinary(Z)

    Z=nist521.HPKE.DECAP(config_id,skR,pkE,pkR)
    print("Decapsulated Z= ",terminator:""); printBinary(Z)

    (key,nonce,exp_secret)=nist521.HPKE.KEYSCHEDULE(config_id,mode,Z,info,psk,pskID);
    print("key= ",terminator:""); printBinary(key)
    print("nonce= ",terminator:""); printBinary(nonce)
    print("exporter secret= ",terminator:""); printBinary(exp_secret)

    (cipher,tag)=GCM.ENCRYPT(key,nonce,aad,plain)
    print("cipher= ",terminator:""); printBinary(cipher)
    print("tag= ",terminator:""); printBinary(tag)

// Mode 2
    mode=2
    print("\nMode 2")
    seedE=decode_hex("fe1c589c2a05893895a537f38c7cb4300b5a7e8fef3d6ccb8f07a498029c61e90262e009dc254c7f6235f9c6b2fd6aeff0a714db131b09258c16e217b7bd2aa619b0")
    seedR=decode_hex("8feea0438481fc0ecd470d6adfcda334a759c6b8650452c5a5dd9b2dd2cc9be33d2bb7ee64605fc07ab4664a58bb9a8de80defe510b6c97d2daf85b92cd4bb0a66bf")
    var seedS=decode_hex("2f66a68b85ef04822b054ef521838c00c64f8b6226935593b69e13a1a2461a4f1a74c10c836e87eed150c0db85d4e4f506cbb746149befac6f5c07dc48a615ef92db")
    nist521.HPKE.DeriveKeyPair(config_id,&skE,&pkE,seedE)
    nist521.HPKE.DeriveKeyPair(config_id,&skR,&pkR,seedR)
    nist521.HPKE.DeriveKeyPair(config_id,&skS,&pkS,seedS)

    Z=nist521.HPKE.AUTHENCAP(config_id,skE,skS,pkE,pkR,pkS)
    print("pkE= ",terminator:""); printBinary(pkE)
    print("Encapsulated Z= ",terminator:""); printBinary(Z)

    Z=nist521.HPKE.AUTHDECAP(config_id,skR,pkE,pkR,pkS)
    print("Decapsulated Z= ",terminator:""); printBinary(Z)

    (key,nonce,exp_secret)=nist521.HPKE.KEYSCHEDULE(config_id,mode,Z,info,nil,nil);
    print("key= ",terminator:""); printBinary(key)
    print("nonce= ",terminator:""); printBinary(nonce)
    print("exporter secret= ",terminator:""); printBinary(exp_secret)

    (cipher,tag)=GCM.ENCRYPT(key,nonce,aad,plain)
    print("cipher= ",terminator:""); printBinary(cipher)
    print("tag= ",terminator:""); printBinary(tag)

// Mode 3
    mode=3
    print("\nMode 3")
    seedE=decode_hex("54272797b1fbc128a6967ff1fd606e0c67868f7762ce1421439cbc9e90ce1b28d566e6c2acbce712e48eebf236696eb680849d6873e9959395b2931975d61d38bd6c")
    seedR=decode_hex("3db434a8bc25b27eb0c590dc64997ab1378a99f52b2cb5a5a5b2fa540888f6c0f09794c654f4468524e040e6b4eca2c9dcf229f908b9d318f960cc9e9baa92c5eee6")
    seedS=decode_hex("65d523d9b37e1273eb25ad0527d3a7bd33f67208dd1666d9904c6bc04969ae5831a8b849e7ff642581f2c3e56be84609600d3c6bbdaded3f6989c37d2892b1e978d5")
    nist521.HPKE.DeriveKeyPair(config_id,&skE,&pkE,seedE)
    nist521.HPKE.DeriveKeyPair(config_id,&skR,&pkR,seedR)
    nist521.HPKE.DeriveKeyPair(config_id,&skS,&pkS,seedS)

    Z=nist521.HPKE.AUTHENCAP(config_id,skE,skS,pkE,pkR,pkS)
    print("pkE= ",terminator:""); printBinary(pkE)
    print("Encapsulated Z= ",terminator:""); printBinary(Z)

    Z=nist521.HPKE.AUTHDECAP(config_id,skR,pkE,pkR,pkS)
    print("Decapsulated Z= ",terminator:""); printBinary(Z)

    (key,nonce,exp_secret)=nist521.HPKE.KEYSCHEDULE(config_id,mode,Z,info,psk,pskID);
    print("key= ",terminator:""); printBinary(key)
    print("nonce= ",terminator:""); printBinary(nonce)
    print("exporter secret= ",terminator:""); printBinary(exp_secret)

    (cipher,tag)=GCM.ENCRYPT(key,nonce,aad,plain)
    print("cipher= ",terminator:""); printBinary(cipher)
    print("tag= ",terminator:""); printBinary(tag)

}

TestHPKE_c25519()
TestHPKE_nist521()

