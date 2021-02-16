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
    var seedE=decode_hex("1dacee520c81ade608f4fa3e5ccae0ecedcc7880e3fc6f3e5afd2e4af8396571")
    var seedR=decode_hex("0a3367dadc97e200074936b5adedcd5680f30672d1ec7158fdfcb795040ec909")
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
    seedE=decode_hex("7f5b697a87fb11d733e31265410c80b72525637e1b872f9ede00d507c0c534d7")
    seedR=decode_hex("89a6ce4b7b8eb12eebff3864615a2539e3e64fa2d4ab525c18c7bc451a2b5e40")
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
    seedE=decode_hex("f7faa81395a22af1071b5273f9f94db0609ab9a7f4eca54834ea408d97294cc2")
    seedR=decode_hex("a10d107753f85bf7128768693e53129fb44a281ea4849a86231632db48dbb28c")
    var seedS=decode_hex("e1257ce84dd98bca7847d75661b658cd2cc6c4efbf9e989a40f390720b57d409")
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
    seedE=decode_hex("e98713f7e24e7025fb7abddfff317d865ad5f4c3b6be22cda48dbcdde6151a68")
    seedR=decode_hex("b281654cdb2cd6a9670c3035eaa702736a951585a938381198f7d29085ca7f3a")
    seedS=decode_hex("ea95c4d3c5e7868af29fc0cb0841485dead8d54b57ee1b8ed31a153295454835")
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
    var seedE=decode_hex("bb8f0c6c0d81a0a54d6084e3d7d8001875ce2b4831400075246ddeabc3f62d065d91fa8f2ca0cfee3a94f1fa6f43f53a2e39241ccd5ad9f0d889aba103e78286ca17")
    var seedR=decode_hex("28b1403adcf3d2bba7d7c733df4ef8ab9f4c2cf92c840beb7de3fd139611a95be850589a49825450c033b2e1f6cb66127c83827d5464fca1e427c2bb31bf18e857e2")
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
    seedE=decode_hex("bd12d21f5a36063ec0242aa26f22144aa7d24c3cc2e6a1805868250d8e9ce6fb2a04f8852cd6ec3241249fcf262b8f245d179636fcfbf275a75539cfc756a4dd3f53")
    seedR=decode_hex("e9a016952866c33da399e64ecd629cf32826285698726f2c22debd4799b8fac112ac1a546ce39452406396a389fb10f51036c7022dc1cdde177b54d005731fb79ce4")
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
    seedE=decode_hex("0e5ec115c172744e793ef333abfd5d9838fd31eef595f0b77a2f5623404aa42f0e77198c2ac78defeae75cc79cf5dc5c7587b6eda733e48f3b17d5e3496482f19cdb")
    seedR=decode_hex("3f3c12492eee6f1f8029a89002b6539ea9754b48412c14d2fe94368cf1fa473f0171cdb1117aaa447ad6914bfb794ead14a9cd22cd83caa22bd905bd7d614d401ddc")
    var seedS=decode_hex("02d1581b120d0415bda1fb2cbe6a3c81edf28209f0fc1460745ad9018747c31e6d42a36ed1734214312311db80bb3f88c2da6ea7624cf2915d32f2993ebeb92c2934")
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
    seedE=decode_hex("c393a0a0cab3313f0526d72b8bef55b26a156493263fa1a653069ed5f2f3637f371b15704778c89a6e01c3fd958dfee5f85613e2d4b2eb17a152810690d8252df44b")
    seedR=decode_hex("e7fa07c9938a3d9d883d256abd7fb5777ed7c4195a8d244c0e682fd1a08fdcce58ab1aa7ba31b1fa120f03d9df438509a1e31992506dbc196ac4489079b3b64bea9b")
    seedS=decode_hex("403e8d5ff0155c71232a887f542df221c385956eaca3acceffdc4681b99c7e9613f90ba03b0c03c0c57a90b2dd31effc6278ab6dc059f3e5389dff6495bda12b4da4")
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

