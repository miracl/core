/*
	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at
	
	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
*/

//
//  TestBLS.swift
//
//  Created by Michael Scott on 02/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Test Boneh-Lynn-Shacham signature API */

import Foundation
import amcl // comment out for Xcode
import bn254
import bls383
import bls24
import bls48

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
    let G1S=BFS+1    // Group 1 Size
    let G2S=4*BFS;     // Group 2 Size
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)

    bn254.BLS.initit()

    bn254.BLS.KeyPairGenerate(&rng,&S,&W)
    print("Testing bn254 curve\n")
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)

    bn254.BLS.sign(&SIG,mess,S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    let res=bn254.BLS.verify(SIG,mess,W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }

}

public func TestBLS_bls383(_ rng: inout RAND)
{
    let BGS=bls383.BLS.BGS
    let BFS=bls383.BLS.BFS
    let G1S=BFS+1    // Group 1 Size
    let G2S=4*BFS;     // Group 2 Size
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)

    bls383.BLS.initit()

    bls383.BLS.KeyPairGenerate(&rng,&S,&W)
    print("Testing bls383 curve\n")    
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)    

    bls383.BLS.sign(&SIG,mess,S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    let res=bls383.BLS.verify(SIG,mess,W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }

}

public func TestBLS_bls24(_ rng: inout RAND)
{
    let BGS=bls24.BLS192.BGS
    let BFS=bls24.BLS192.BFS
    let G1S=BFS+1    // Group 1 Size
    let G2S=8*BFS;     // Group 2 Size
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)

    bls24.BLS192.initit()

    bls24.BLS192.KeyPairGenerate(&rng,&S,&W)
    print("Testing bls24 curve\n")      
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)

    bls24.BLS192.sign(&SIG,mess,S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    let res=bls24.BLS192.verify(SIG,mess,W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }
}

public func TestBLS_bls48(_ rng: inout RAND)
{
    let BGS=bls48.BLS256.BGS
    let BFS=bls48.BLS256.BFS
    let G1S=BFS+1    // Group 1 Size
    let G2S=16*BFS;     // Group 2 Size
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)

    bls48.BLS256.initit()

    bls48.BLS256.KeyPairGenerate(&rng,&S,&W)
    print("Testing bls48 curve\n")      
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)
    
    bls48.BLS256.sign(&SIG,mess,S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    let res=bls48.BLS256.verify(SIG,mess,W)
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
TestBLS_bls383(&rng)
TestBLS_bls24(&rng)
TestBLS_bls48(&rng)
