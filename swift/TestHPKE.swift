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
    let psk=decode_hex("5db3b80a81cb63ca59470c83414ef70a")
    let pskID=decode_hex("456e6e796e20447572696e206172616e204d6f726961")
    let plain=decode_hex("4265617574792069732074727574682c20747275746820626561757479")
    let aad=decode_hex("436f756e742d30")

    print("\nTesting HPKE for curve c25519")

// Mode 0
    var mode=0;
    print("\nMode 0");
    var seedE=decode_hex("a77ae3e14cc2ec9e603a9049423d48e66a5e3139e896e95cf19919430657adc7")
    var seedR=decode_hex("1289f0db1d8f68d0c531b5e53a40911a2a2347059355d7c267717033fef2b08c")
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
    seedE=decode_hex("0fa1407ccee05de0cceb2f2d2381d2df0602dbd43be90eefd288ce4ad0b3ba32")
    seedR=decode_hex("326ee379f778718e6cb343f55668fbb9d0098ba0503cd4414a8f1ce252605c39")
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
    seedE=decode_hex("02900cb4856b5f222293a9bd7bda2f1f81c562dc3002336ad1c39f6572402b7d")
    seedR=decode_hex("518df90f0f5044ce653180c700e4902d37a7ba1cd23482a76e18b300fecaac4e")
    var seedS=decode_hex("262a05ad0c08030cdbbaafc03d64f33b95bf8089f216c62ac39b72064a4b4dcb")
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
    seedE=decode_hex("c1d1028243a951dbf6469025f3a1304407b08fb932104e61c7aab42ab4f1995c")
    seedR=decode_hex("02a965d8f53bbdcc11cc618d4f31f69277500b75959ca97fd533058315511d1b")
    seedS=decode_hex("e9c09a3e50073935e75d3846007a26088a93ebf58ad0bb30ad6c42a9d4d2419e")
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
    let psk=decode_hex("5db3b80a81cb63ca59470c83414ef70a")
    let pskID=decode_hex("456e6e796e20447572696e206172616e204d6f726961")
    let plain=decode_hex("4265617574792069732074727574682c20747275746820626561757479")
    let aad=decode_hex("436f756e742d30")

    print("\nTesting HPKE for curve nist521")

// Mode 0
    var mode=0
    print("\nMode 0")
    var seedE=decode_hex("1ed3768f499b5b3c2beda3166528b649d4b117a0bd450f0e9e19815c2597d1777ac67ea367415fb28c8819c94b383a0a8a15c9f03b4835330e3e6c8bc8319202e473")
    var seedR=decode_hex("62641514bccd2858f3d6513305288d6ca0e443f00a86eb33ccd519d1803aebc5d07bbad0e1013ce61d9c9d713b3c90c8e79a1af01d6c69750f67cbbd1d9d4afeedfa")
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
    seedE=decode_hex("64463def238f309f1e9d1f28c15dc126cffa4ded911a4c527eeb71ba593847fb405756239d2c694ce4effa3996cafb5cc0b3736dd988deb7289210ec92bf6b339302")
    seedR=decode_hex("41b782c18c14986c9d7a636152f13677aeddf479c1c7791ea46e0ebbe35ca9dd524c23d730eef443741d7d965415833d6c549c8c1b31ad05f2b9a88f916b2930528e")
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
    seedE=decode_hex("81dc51e31ef8e9b33fefcdf00bd3b0ff585b941fe76cf39a86c269e2f53be7edb3db0be1a58b6cb8d8e6020fe8a2018c59d47cacb35b2b8c61bd4155438b5eda5c0d")
    seedR=decode_hex("54af23ea93c8fc34deb6a7cd70e657ea8990fc4e9a18656d5764b62f7a33a9e0212adeae1585ad2ef28688c1b558866c1975973c4dff08955c1f9fd7939b10b5fbfc")
    var seedS=decode_hex("b65599d814192278ab826ef197a61b77db50f40495f77502dfaa03acd1f3565a3cefebd59de2328ece0638c90d8a89f9ca58f2850e39e9a4c9c339290d66da12fdf0")
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
    seedE=decode_hex("dc1fda9b21a1af6925ecf9ad79d2422f698b4168587c7908b36f5f58352181b9506554d8d8c9427e0dd2cfda25f0eabf58e9f5597e1b76ac12c799fe96e3cc03dc59")
    seedR=decode_hex("46592c2b171b8cdcce89601fab103f63ed43badadcf9df62a928ae3b7fa91f269eff3485f6401c374e19a8bb988005626b9c26d39795282b1095bcc4f62a67255e15")
    seedS=decode_hex("d02446c344c10cd162486caa69aa1156ac3066e0fd668fa7faaf13bdbc944edbc0cd68ee36e4c30ecc36c2c5ab0978473eb1b5dcfff27985c9328877e85fd48b657d")
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

