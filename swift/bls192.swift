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
//  bls.swift
//
//  Created by Michael Scott on 03/12/2018.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Boneh-Lynn-Shacham signature 192-bit API Functions */

/* Loosely (for now) following https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-bls-signature-02 */

// Minimal-signature-size variant

import Foundation
import core

public struct BLS192
{
    static public let BFS=Int(CONFIG_BIG.MODBYTES)
    static public let BGS=Int(CONFIG_BIG.MODBYTES)
    static let BLS_OK:Int = 0
    static let BLS_FAIL:Int = -1

    static var G2_TAB=[FP8]()

    static func ceil(_ a: Int,_ b:Int) -> Int {
        return (((a)-1)/(b)+1)
    }

/* output u \in F_p */
    static func hash_to_field(_ hf: Int,_ hlen: Int,_ DST: [UInt8],_ M: [UInt8],_ ctr:Int) -> [FP] {
        let q=BIG(ROM.Modulus)
        let L = ceil(q.nbits()+CONFIG_CURVE.AESKEY*8,8)
        var u=[FP]()
        var fd=[UInt8](repeating: 0,count: L)
        var dx:DBIG

        let OKM=HMAC.XMD_Expand(hf,hlen,L*ctr,DST,M)
        for i in 0..<ctr {
            for j in 0..<L {
                fd[j]=OKM[i*L+j]
            }
            dx=DBIG.fromBytes(fd)
            u.append(FP(dx.mod(q)))
        }

        return u
    }

/* hash a message to an ECP point, using SHA2, random oracle method */
    static public func bls_hash_to_point(_ M: [UInt8]) -> ECP
    {
        let dst = "BLS_SIG_ZZZG1_XMD:SHA-384_SVDW_RO_NUL_".uppercased()
        let u=hash_to_field(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,[UInt8](dst.utf8),M,2)

        var P=ECP.map2point(u[0])
        let P1=ECP.map2point(u[1])
        P.add(P1)
        P.cfp()
        P.affine()
        return P
    }

    static public func initit() -> Int
    {
        let G=ECP4.generator()
        if G.is_infinity() {return BLS_FAIL}
        G2_TAB=PAIR4.precomp(G)
        return BLS_OK
    }

/* generate key pair, private key S, public key W */
    @discardableResult static public func KeyPairGenerate(_ IKM: [UInt8],_ S:inout [UInt8],_ W:inout [UInt8]) -> Int
    {
        let r = BIG(ROM.CURVE_Order)
        let L = ceil(3*ceil(r.nbits(),8),2)
        let LEN=HMAC.inttoBytes(L, 2)
        var AIKM=[UInt8]()
        AIKM.append(contentsOf: IKM)
        AIKM.append(0)

        var G = ECP4.generator()
        if G.is_infinity() {return BLS_FAIL}
        let salt = "BLS-SIG-KEYGEN-SALT-"

        let PRK=HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,[UInt8](salt.utf8),AIKM)
        let OKM=HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,PRK,LEN)

        var dx = DBIG.fromBytes(OKM)
        let s = dx.mod(r)
        s.toBytes(&S)
// SkToPk
        G=PAIR4.G2mul(G,s)
        G.toBytes(&W,true)
        return BLS_OK
    }

/* Sign message m using private key S to produce signature SIG */
    @discardableResult static public func core_sign(_ SIG:inout [UInt8],_ M: [UInt8],_ S: [UInt8]) -> Int
    {
        var D=bls_hash_to_point(M)
        let s=BIG.fromBytes(S)
        D=PAIR4.G1mul(D,s)
        D.toBytes(&SIG,true)
        return BLS_OK
    }

/* Verify signature given message m, the signature SIG, and the public key W */

    static public func core_verify(_ SIG: [UInt8],_ M: [UInt8],_ W: [UInt8]) -> Int
    {
        let HM=bls_hash_to_point(M)

        var D=ECP.fromBytes(SIG)
        if !PAIR4.G1member(D) {return BLS_FAIL}
        D.neg()

        let PK=ECP4.fromBytes(W)
        if !PAIR4.G2member(PK) {return BLS_FAIL}

// Use new multi-pairing mechanism
        var r=PAIR4.initmp()
        //PAIR4.another(&r,G,D)
        PAIR4.another_pc(&r,G2_TAB,D)
        PAIR4.another(&r,PK,HM)
        var v=PAIR4.miller(&r)

//.. or alternatively
//        var v=PAIR4.ate2(G,D,PK,HM)

        v=PAIR4.fexp(v)

        if v.isunity() {
            return BLS_OK
        } else {
            return BLS_FAIL
        }
    }
}
