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
//  TestEDDSA.swift
//
//  Created by Michael Scott on 25/04/2023.
//

// test driver and function exerciser for EDDSA API Functions 

import Foundation
import core // comment out for Xcode
import ed25519
import ed448

func printBinary(_ array: [UInt8])
{
    for i in 0 ..< array.count
    {
        let h=String(format:"%02x",array[i])
        print("\(h)", terminator: "")
    }
    print(" ");
}

func decode_hex(_ s: String) -> [UInt8] {
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

@discardableResult func prehash_sha2(_ m: [UInt8],_ h:inout [UInt8]) -> Int  {
    var sh=HASH512()
    for i in 0..<m.count {
        sh.process(m[i])
    }
    let hs = sh.hash();
    for i in 0..<64 {
        h[i]=hs[i];
    }
    return 64;
}

@discardableResult func prehash_sha3(_ m: [UInt8],_ h:inout [UInt8]) -> Int  {
    var sh=SHA3(SHA3.SHAKE256)
    for i in 0..<m.count {
        sh.process(m[i])
    }
    sh.shake(&h,64);
    return 64;
}

public func TestEDDSA_ed25519(_ rng: inout RAND,_ prehash:Bool)
{
    print("Testing EDDSA - Curve Ed25519\n");
    var d=decode_hex("c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7")
    let EFS=ed25519.EDDSA.EFS

    var q=[UInt8](repeating: 0,count: EFS)
    let m:[UInt8] = [0xAF, 0x82]
    var sig=[UInt8](repeating: 0,count: 2*EFS)

    var NULLRNG : RAND? = nil

    // Generate Key pair D/Q 
    ed25519.EDDSA.KEY_PAIR_GENERATE(&NULLRNG,&d,&q);

    print("Secret key= 0x",terminator: ""); printBinary(d)
    print("Public key= 0x",terminator: ""); printBinary(q)

    if prehash {
        var hm=[UInt8](repeating: 0,count: 64)
        prehash_sha2(m,&hm)
        ed25519.EDDSA.SIGNATURE(true,d, nil,hm, &sig)
    } else {
        ed25519.EDDSA.SIGNATURE(false,d, nil,m, &sig)
    }
    print("Signature= 0x",terminator: ""); printBinary(sig)

    var verify:Bool
    if prehash {
        var hm=[UInt8](repeating: 0,count: 64)
        prehash_sha2(m,&hm)
        verify=ed25519.EDDSA.VERIFY(true,q, nil ,hm, sig);
    } else {
        verify=ed25519.EDDSA.VERIFY(false,q, nil ,m, sig);
    }
    if verify {
        print("Signature is verified\n");
    } else {
        print("Signature is NOT verified\n");
    }
}


public func TestEDDSA_ed448(_ rng: inout RAND,_ prehash:Bool)
{
    print("Testing EDDSA - Curve Ed448\n");
    var d=decode_hex("c4eab05d357007c632f3dbb48489924d552b08fe0c353a0d4a1f00acda2c463afbea67c5e8d2877c5e3bc397a659949ef8021e954e0a12274e")
    let EFS=ed448.EDDSA.EFS

    var q=[UInt8](repeating: 0,count: EFS+1)
    let m:[UInt8] = [0x03]
    var sig=[UInt8](repeating: 0,count: 2*EFS+2)

    var NULLRNG : RAND? = nil

    // Generate Key pair D/Q 
    ed448.EDDSA.KEY_PAIR_GENERATE(&NULLRNG,&d,&q);

    print("Secret key= 0x",terminator: ""); printBinary(d)
    print("Public key= 0x",terminator: ""); printBinary(q)

    if prehash {
        var hm=[UInt8](repeating: 0,count: 64)
        prehash_sha3(m,&hm)
        ed448.EDDSA.SIGNATURE(true,d, nil,hm, &sig)
    } else {
        ed448.EDDSA.SIGNATURE(false,d, nil,m, &sig)
    }
    print("Signature= 0x",terminator: ""); printBinary(sig)

    var verify:Bool
    if prehash {
        var hm=[UInt8](repeating: 0,count: 64)
        prehash_sha3(m,&hm)
        verify=ed448.EDDSA.VERIFY(true,q, nil ,hm, sig);
    } else {
        verify=ed448.EDDSA.VERIFY(false,q, nil ,m, sig);
    }
    if verify {
        print("Signature is verified\n");
    } else {
        print("Signature is NOT verified\n");
    }
}

var RAW=[UInt8](repeating: 0,count: 100)
var rng=RAND()

rng.clean();
for i in 0 ..< 100 {RAW[i]=UInt8(i&0xff)}
rng.seed(100,RAW)

let prehash=false

TestEDDSA_ed25519(&rng,prehash)
TestEDDSA_ed448(&rng,prehash)

