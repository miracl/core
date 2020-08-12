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

/* Test driver and function exerciser for Boneh-Lynn-Shacham BLS Signature API Functions */

/* To reverse the groups G1 and G2, edit BLS*.go

Swap G1 <-> G2
Swap ECP <-> ECPn
Disable G2 precomputation
Switch G1/G2 parameter order in pairing function calls

Swap G1S and G2S in this program

*/

//
//  TestBLS.swift
//
//  Created by Michael Scott on 02/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Test Boneh-Lynn-Shacham signature API */

import Foundation
import core // comment out for Xcode
import bn254
import bls12383
import bls24479
import bls48556

public func printBinary(_ array: [UInt8])
{
    for i in 0 ..< array.count
    {
        let h=String(format:"%02x",array[i])
        print("\(h)", terminator: "")
    }
    print(" ");
}


public func TestBLS_bn254(_ rng: inout RAND)
{
    let BGS=bn254.BLS.BGS
    let BFS=bn254.BLS.BFS
    let G1S=BFS+1    // Group 1 Size - compressed
    let G2S=2*BFS+1;     // Group 2 Size - compressed
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)
    var IKM=[UInt8](repeating: 0,count: 32)

    for i in 0..<32 {
        //IKM[i]=UInt8(i+1)
        IKM[i]=rng.getByte()
    }

    var res=bn254.BLS.initit()
    if res != 0 {
        print("Failed to Initialize\n")
        return
    }

    res=bn254.BLS.KeyPairGenerate(IKM,&S,&W)
    if res != 0 {
        print("Failed to Generate Keys\n")
        return
    }

    print("Testing bn254 curve\n")
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)

    bn254.BLS.core_sign(&SIG,[UInt8](mess.utf8),S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    res=bn254.BLS.core_verify(SIG,[UInt8](mess.utf8),W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }

}

public func TestBLS_bls12383(_ rng: inout RAND)
{
    let BGS=bls12383.BLS.BGS
    let BFS=bls12383.BLS.BFS
    let G1S=BFS+1    // Group 1 Size - compressed
    let G2S=2*BFS+1;     // Group 2 Size - compressed
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)
    var IKM=[UInt8](repeating: 0,count: 32)

    for i in 0..<32 {
        //IKM[i]=UInt8(i+1)
        IKM[i]=rng.getByte()
    }

    var res=bls12383.BLS.initit()
    if res != 0 {
        print("Failed to Initialize\n")
        return
    }
    res=bls12383.BLS.KeyPairGenerate(IKM,&S,&W)
    if res != 0 {
        print("Failed to Generate Keys\n")
        return
    }
    print("Testing bls12383 curve\n")    
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)    

    bls12383.BLS.core_sign(&SIG,[UInt8](mess.utf8),S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    res=bls12383.BLS.core_verify(SIG,[UInt8](mess.utf8),W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }

}

public func TestBLS_bls24479(_ rng: inout RAND)
{
    let BGS=bls24479.BLS192.BGS
    let BFS=bls24479.BLS192.BFS
    let G1S=BFS+1    // Group 1 Size  - compressed
    let G2S=4*BFS+1;     // Group 2 Size - compressed
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)
    var IKM=[UInt8](repeating: 0,count: 48)

    for i in 0..<48 {
        //IKM[i]=UInt8(i+1)
        IKM[i]=rng.getByte()
    }

    var res=bls24479.BLS192.initit()
    if res != 0 {
        print("Failed to Initialize\n")
        return
    }
    res=bls24479.BLS192.KeyPairGenerate(IKM,&S,&W)
    if res != 0 {
        print("Failed to Generate Keys\n")
        return
    }
    print("Testing bls24479 curve\n")      
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)

    bls24479.BLS192.core_sign(&SIG,[UInt8](mess.utf8),S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    res=bls24479.BLS192.core_verify(SIG,[UInt8](mess.utf8),W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }
}

public func TestBLS_bls48556(_ rng: inout RAND)
{
    let BGS=bls48556.BLS256.BGS
    let BFS=bls48556.BLS256.BFS
    let G1S=BFS+1    // Group 1 Size - compressed
    let G2S=8*BFS+1;     // Group 2 Size - compressed
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)
    var IKM=[UInt8](repeating: 0,count: 64)

    for i in 0..<64 {
        //IKM[i]=UInt8(i+1)
        IKM[i]=rng.getByte()
    }

    var res=bls48556.BLS256.initit()
    if res != 0 {
        print("Failed to Initialize\n")
        return
    }
    res=bls48556.BLS256.KeyPairGenerate(IKM,&S,&W)
    if res != 0 {
        print("Failed to Generate Keys\n")
        return
    }
    print("Testing bls48556 curve\n")      
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)
    
    bls48556.BLS256.core_sign(&SIG,[UInt8](mess.utf8),S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    res=bls48556.BLS256.core_verify(SIG,[UInt8](mess.utf8),W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }
}

print("Testing Boneh-Lynn-Shacham signature code\n");


var RAW=[UInt8](repeating: 0,count: 100)
var rng=RAND()
    
rng.clean();
for i in 0 ..< 100 {RAW[i]=UInt8(i&0xff)}
rng.seed(100,RAW)

TestBLS_bn254(&rng)
TestBLS_bls12383(&rng)
TestBLS_bls24479(&rng)
TestBLS_bls48556(&rng)
