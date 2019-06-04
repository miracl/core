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
//  bls.swift
//
//  Created by Michael Scott on 03/12/2018.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Boneh-Lynn-Shacham Signature API Functions */

import Foundation
import amcl

public struct BLS256
{
    static public let BFS=Int(CONFIG_BIG.MODBYTES)
    static public let BGS=Int(CONFIG_BIG.MODBYTES)
    static let BLS_OK:Int = 0
    static let BLS_FAIL:Int = -1
    
    static var G2_TAB=[FP16]()
    
    static public func initit() {
        let G=ECP8.generator()
        G2_TAB=PAIR256.precomp(G)
    }

/* hash a message to an ECP point, using SHA3 */

    private static func bls_hashit(_ m: String) -> ECP
    {
        var sh=SHA3(SHA3.SHAKE256)
        var hm=[UInt8](repeating: 0,count: BFS)  
        let t=[UInt8](m.utf8)     
        for i in 0..<t.count {sh.process(t[i])}
        sh.shake(&hm,BFS)
        let P=ECP.mapit(hm) 
        return P
    }

/* generate key pair, private key S, public key W */

    @discardableResult static public func KeyPairGenerate(_ rng: inout RAND,_ S:inout [UInt8],_ W:inout [UInt8]) -> Int
    {
        var G=ECP8.generator()
        let q=BIG(ROM.CURVE_Order)
        let s=BIG.randtrunc(q,16*CONFIG_CURVE.AESKEY,&rng)
        s.toBytes(&S)      
        G=PAIR256.G2mul(G,s) 
        G.toBytes(&W)       
        return BLS_OK
    }

/* Sign message m using private key S to produce signature SIG */

    @discardableResult static public func sign(_ SIG:inout [UInt8],_ m: String,_ S: [UInt8]) -> Int
    {
        var D=bls_hashit(m)
        let s=BIG.fromBytes(S)
        D=PAIR256.G1mul(D,s)         
        D.toBytes(&SIG,true)
        return BLS_OK
    }

/* Verify signature given message m, the signature SIG, and the public key W */

    static public func verify(_ SIG: [UInt8],_ m: String,_ W: [UInt8]) -> Int
    {
        let HM=bls_hashit(m)     
        var D=ECP.fromBytes(SIG)  
        //let G=ECP8.generator()
        let PK=ECP8.fromBytes(W)
        D.neg()

// Use new multi-pairing mechanism 
        var r=PAIR256.initmp()
        //PAIR256.another(&r,G,D)
        PAIR256.another_pc(&r,G2_TAB,D)
        PAIR256.another(&r,PK,HM)
        var v=PAIR256.miller(&r)

//.. or alternatively
//        var v=PAIR256.ate2(G,D,PK,HM)

        v=PAIR256.fexp(v)
    
        if v.isunity() {
            return BLS_OK
        } else {
            return BLS_FAIL
        }
    }
}
 