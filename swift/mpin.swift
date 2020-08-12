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
//  mpin.swift
//
//  Created by Michael Scott on 08/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* MPIN 128-bit API Functions */

import Foundation
import core

public struct MPIN
{
    static public let EFS=Int(CONFIG_BIG.MODBYTES)
    static public let EGS=Int(CONFIG_BIG.MODBYTES)
    static let INVALID_POINT:Int = -14
    static let BAD_PARAMS:Int = -11
    static let WRONG_ORDER:Int = -18
    static public let BAD_PIN:Int = -19

    /* Configure your PIN here */

    static let MAXPIN:Int32 = 10000  // PIN less than this
    static let PBLEN:Int32 = 14      // Number of bits in PIN

    static func ceil(_ a: Int,_ b:Int) -> Int {
        return (((a)-1)/(b)+1)
    }

    static public func ENCODE_TO_CURVE(_ DST: [UInt8],_ ID: [UInt8],_ HCID:inout [UInt8])
    {
        let q=BIG(ROM.Modulus)
        let k=q.nbits()
        let r=BIG(ROM.CURVE_Order)
        let m=r.nbits()
        let L=ceil(k+ceil(m,2),8)
        let OKM=HMAC.XMD_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,DST,ID)
        var fd=[UInt8](repeating: 0,count: L)
        for j in 0..<L {
            fd[j]=OKM[j]
        }
		var dx=DBIG.fromBytes(fd)
		let u=FP(dx.mod(q))
        var P=ECP.map2point(u)
        P.cfp()
        P.affine()
        P.toBytes(&HCID,false)
    }

    // create random secret S
    @discardableResult static public func RANDOM_GENERATE(_ rng: inout RAND,_ S:inout [UInt8]) -> Int
    {
        let r=BIG(ROM.CURVE_Order)
        let s=BIG.randtrunc(r,16*CONFIG_CURVE.AESKEY,&rng)
        s.toBytes(&S);
        return 0;
    }

    // Extract PIN from TOKEN for identity CID
    static public func EXTRACT_PIN(_ CID:[UInt8],_ pin:Int32,_ TOKEN:inout [UInt8]) -> Int
    {
        var P=ECP.fromBytes(TOKEN)
        if P.is_infinity() {return INVALID_POINT}
        var R=ECP.fromBytes(CID)
        if R.is_infinity() {return INVALID_POINT}
        R=R.pinmul(pin%MAXPIN,PBLEN)
        P.sub(R)
        P.toBytes(&TOKEN,false)
        return 0
    }


    // Implement step 2 on client side of MPin protocol
    static public func CLIENT_2(_ X:[UInt8],_ Y:[UInt8],_ SEC:inout [UInt8]) -> Int
    {
        let r=BIG(ROM.CURVE_Order)
        var P=ECP.fromBytes(SEC)
        if P.is_infinity() {return INVALID_POINT}

        var px=BIG.fromBytes(X)
        let py=BIG.fromBytes(Y)
        px.add(py)
        px.mod(r)

        P=PAIR.G1mul(P,px)
        P.neg()
        P.toBytes(&SEC,false);
        return 0
    }

    @discardableResult static public func GET_CLIENT_SECRET(_ S:inout [UInt8],_ IDHTC:[UInt8],_ CST:inout [UInt8]) -> Int
    {
        let s=BIG.fromBytes(S)
        let P=ECP.fromBytes(IDHTC);
        if P.is_infinity() {return INVALID_POINT}
        PAIR.G1mul(P,s).toBytes(&CST,false)
        return 0
    }

    // Implement step 1 on client side of MPin protocol
    static public func CLIENT_1(_ CID:[UInt8],_ rng: inout RAND?,_ X:inout [UInt8],_ pin:Int32,_ TOKEN:[UInt8],_ SEC:inout [UInt8],_ xID:inout [UInt8]) -> Int
    {
        let r=BIG(ROM.CURVE_Order)
        var x:BIG
        if rng != nil
        {
            x=BIG.randtrunc(r,16*CONFIG_CURVE.AESKEY,&rng!)
            x.toBytes(&X);
        }
        else
        {
            x=BIG.fromBytes(X);
        }

        var P=ECP.fromBytes(CID);
        if P.is_infinity() {return INVALID_POINT}

        var T=ECP.fromBytes(TOKEN);
        if T.is_infinity() {return INVALID_POINT}

        let W=P.pinmul(pin%MPIN.MAXPIN,MPIN.PBLEN)
        T.add(W)

		P=PAIR.G1mul(P,x)
		P.toBytes(&xID,false)
          
        T.toBytes(&SEC,false);
        return 0;
    }
    // Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret
    @discardableResult static public func GET_SERVER_SECRET(_ S:[UInt8],_ SST:inout [UInt8]) -> Int
    {
        var Q=ECP2.generator();
        let s=BIG.fromBytes(S)
        Q=PAIR.G2mul(Q,s)
        Q.toBytes(&SST, false)
        return 0
    }

    // Implement step 2 of MPin protocol on server side
    static public func SERVER(_ HID:[UInt8]?,_ Y:[UInt8],_ SST:[UInt8],_ xID:[UInt8]?,_ mSEC:[UInt8]) -> Int
    {
        let Q=ECP2.generator();
        let sQ=ECP2.fromBytes(SST)
        if sQ.is_infinity() {return INVALID_POINT}
 
        if xID==nil {return MPIN.BAD_PARAMS}
        var R=ECP.fromBytes(xID!)
        
        if R.is_infinity() {return INVALID_POINT}
        let y=BIG.fromBytes(Y)

        if HID==nil {return MPIN.BAD_PARAMS}
        var P=ECP.fromBytes(HID!)
        
        if P.is_infinity() {return INVALID_POINT}

        P=PAIR.G1mul(P,y)
        P.add(R);
        R=ECP.fromBytes(mSEC)
        if R.is_infinity() {return MPIN.INVALID_POINT}

        var g=PAIR.ate2(Q,R,sQ,P)
        g=PAIR.fexp(g)

        if !g.isunity()
        {
            return MPIN.BAD_PIN;
        }
        return 0
    }
}


