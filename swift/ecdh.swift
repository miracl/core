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
//  ecdh.swift
//
//  Created by Michael Scott on 30/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

//import Foundation
//import Darwin
import core

/* ECDH/ECIES/ECDSA API Functions */

public struct ECDH
{
    static let INVALID_PUBLIC_KEY:Int = -2
    static let ERROR:Int = -3
    //static let INVALID:Int = -4
    static public let EFS=Int(CONFIG_BIG.MODBYTES);
    static public let EGS=Int(CONFIG_BIG.MODBYTES);
    static public let SHA256=32
    static public let SHA384=48
    static public let SHA512=64

    /* return true if S is in ranger 0 < S < order , else return false */
    static public func IN_RANGE(_ S: [UInt8]) -> Bool
    {
       let r=BIG(ROM.CURVE_Order)
       let s=BIG.fromBytes(S)
       if s.iszilch() {
            return false
       }
       if BIG.comp(s,r) >= 0 {
            return false
       }
       return true
    }

    /* Calculate a public/private EC GF(p) key pair W,S where W=S.G mod EC(p),
    * where S is the secret key and W is the public key
    * and G is fixed generator.
    * If RNG is NULL then the private key is provided externally in S
    * otherwise it is generated randomly internally */
    @discardableResult  static public func KEY_PAIR_GENERATE(_ RNG: inout RAND?,_ S:inout [UInt8],_ W:inout [UInt8]) -> Int
    {
        let res=0;
        var s:BIG
        var G:ECP

        G=ECP.generator();

        let r=BIG(ROM.CURVE_Order)

        if (RNG==nil)
        {
            s=BIG.fromBytes(S)
            s.mod(r)
        }
        else
        {
            s=BIG.randtrunc(r,16*CONFIG_CURVE.AESKEY,&RNG!)
        }

        s.toBytes(&S)

        let WP=G.mul(s)
        WP.toBytes(&W,false)  // To use point compression on public keys, change to true

        return res;
    }

    /* validate public key */
    static public func PUBLIC_KEY_VALIDATE(_ W:[UInt8]) -> Int
    {
        var WP=ECP.fromBytes(W);
        var res=0;

        let r=BIG(ROM.CURVE_Order)

        if WP.is_infinity() {res=INVALID_PUBLIC_KEY}
        if res==0
        {
            let q=BIG(ROM.Modulus)
            let nb=UInt(q.nbits())
            var k=BIG(1); 
            k.shl((nb+4)/2)
            k.add(q)
            k.div(r)
            while k.parity()==0 {
                k.shr(1)
                WP.dbl()
            }
            if !k.isunity() {WP=WP.mul(k)}
            if WP.is_infinity() {res=INVALID_PUBLIC_KEY}

        }
        return res;
    }
    /* IEEE-1363 Diffie-Hellman online calculation Z=S.WD */
    // type = 0 is just x coordinate output
    // type = 1 for standard compressed output
    // type = 2 for standard uncompress output 04|x|y
    @discardableResult static public func ECPSVDP_DH(_ S:[UInt8],_ WD:[UInt8],_ Z:inout [UInt8],_ type:Int) -> Int
    {
        var res=0
        var s=BIG.fromBytes(S)

        var W=ECP.fromBytes(WD)
        if W.is_infinity() {res=ECDH.ERROR}

        if (res==0)
        {
            let r=BIG(ROM.CURVE_Order)
            s.mod(r)

            W=W.mul(s);
            if W.is_infinity() {res=ECDH.ERROR}
            else
            {
                if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY {
                    if type>0 {
                        if type==1 {
                            W.toBytes(&Z,true)
                        } else {
                            W.toBytes(&Z,false)
                        }
                    } else {
                        W.getX().toBytes(&Z)
                    }
                    return res
                } else {
                    W.getX().toBytes(&Z)
                }
            }
        }
        return res
    }
    /* IEEE ECDSA Signature, C and D are signature on F using private key S */
    static public func ECPSP_DSA(_ sha:Int,_ RNG: inout RAND,_ S:[UInt8],_ F:[UInt8],_ C:inout [UInt8],_ D:inout [UInt8]) -> Int
    {
        var T=[UInt8](repeating: 0,count: ECDH.EFS)
        let B=HMAC.GPhashit(HMAC.MC_SHA2,sha,Int(CONFIG_BIG.MODBYTES), 0,F,-1,nil)

        let G=ECP.generator();
        let r=BIG(ROM.CURVE_Order)

        let s=BIG.fromBytes(S)
        let f=BIG.fromBytes(B)

        var c=BIG(0)
        var d=BIG(0)
        var V=ECP()

        repeat {
            var u=BIG.randomnum(r,&RNG);
            let w=BIG.randomnum(r,&RNG);  /* side channel masking */

            V.copy(G)
            V=V.mul(u)
            let vx=V.getX()
            c.copy(vx)
            c.mod(r)
            if c.iszilch() {continue}
            u.copy(BIG.modmul(u,w,r))
            u.invmodp(r)
            d.copy(BIG.modmul(s,c,r))
            d.add(f)
            d.copy(BIG.modmul(d,w,r))
            d.copy(BIG.modmul(u,d,r))
        } while d.iszilch()

        c.toBytes(&T)
        for i in 0 ..< ECDH.EFS {C[i]=T[i]}
        d.toBytes(&T)
        for i in 0 ..< ECDH.EFS {D[i]=T[i]}
        return 0;
    }

    /* IEEE1363 ECDSA Signature Verification. Signature C and D on F is verified using public key W */
    static public func ECPVP_DSA(_ sha:Int,_ W:[UInt8],_ F:[UInt8],_ C:[UInt8],_ D:[UInt8]) -> Int
    {
        var res=0
        let B=HMAC.GPhashit(HMAC.MC_SHA2,sha,Int(CONFIG_BIG.MODBYTES),0,F,-1,nil)

        let G=ECP.generator();
        let r=BIG(ROM.CURVE_Order)

        let c=BIG.fromBytes(C)
        var d=BIG.fromBytes(D)
        var f=BIG.fromBytes(B)

        if c.iszilch() || BIG.comp(c,r)>=0 || d.iszilch() || BIG.comp(d,r)>=0
            {res=ECDH.ERROR}

        if res==0
        {
            d.invmodp(r);
            f.copy(BIG.modmul(f,d,r))
            let h2=BIG.modmul(c,d,r)

            let WP=ECP.fromBytes(W)
            if WP.is_infinity() {res=ECDH.ERROR}
            else
            {
				var P=ECP();
				P.copy(WP);
				P=P.mul2(h2,G,f);
                if P.is_infinity() {res=ECDH.ERROR}
				else
				{
                    d=P.getX();
                    d.mod(r);
                    if (BIG.comp(d,c) != 0) {res=ECDH.ERROR}
				}
            }
        }

        return res;
    }

    /* IEEE1363 ECIES encryption. Encryption of plaintext M uses public key W and produces ciphertext V,C,T */
    static public func ECIES_ENCRYPT(_ sha:Int,_ P1:[UInt8],_ P2:[UInt8],_ RNG: inout RAND?,_ W:[UInt8],_ M:[UInt8],_ V:inout [UInt8],_ T:inout [UInt8]) -> [UInt8]
    {
        var Z=[UInt8](repeating: 0,count: ECDH.EFS)
        var VZ=[UInt8](repeating: 0,count: 3*ECDH.EFS+1)
        var K1=[UInt8](repeating: 0,count: CONFIG_CURVE.AESKEY)
        var K2=[UInt8](repeating: 0,count: CONFIG_CURVE.AESKEY)
        var U=[UInt8](repeating: 0,count: ECDH.EGS)


        if ECDH.KEY_PAIR_GENERATE(&RNG,&U,&V) != 0 {return [UInt8]()}
        if ECDH.ECPSVDP_DH(U,W,&Z,0) != 0 {return [UInt8]()}

        for i in 0 ..< 2*ECDH.EFS+1 {VZ[i]=V[i]}
        for i in 0 ..< ECDH.EFS {VZ[2*ECDH.EFS+1+i]=Z[i]}


        let K=HMAC.KDF2(HMAC.MC_SHA2,sha,VZ,P1,2*CONFIG_CURVE.AESKEY)

        for i in 0 ..< CONFIG_CURVE.AESKEY {K1[i]=K[i]; K2[i]=K[CONFIG_CURVE.AESKEY+i];}

        let C=AES.CBC_IV0_ENCRYPT(K1,M)

        let L2=HMAC.inttoBytes(P2.count,8)

        var AC=[UInt8](repeating: 0,count: C.count+P2.count+8)

        for i in 0 ..< C.count {AC[i]=C[i]}
        for i in 0 ..< P2.count {AC[C.count+i]=P2[i]}
        for i in 0 ..< 8 {AC[C.count+P2.count+i]=L2[i]}

        HMAC.HMAC1(HMAC.MC_SHA2,sha,&T,T.count,K2,AC)

        return C
    }

/* constant time n-byte compare */
    static func ncomp(_ T1:[UInt8],_ T2:[UInt8],_ n:Int) -> Bool {
	var res=0
	for i in 0 ..< n {
		res|=Int(T1[i]^T2[i])
	}
	if res==0 {return true}
	return false
    }

    /* IEEE1363 ECIES decryption. Decryption of ciphertext V,C,T using private key U outputs plaintext M */
    static public func ECIES_DECRYPT(_ sha:Int,_ P1:[UInt8],_ P2:[UInt8],_ V:[UInt8],_ C:[UInt8],_ T:[UInt8],_ U:[UInt8]) -> [UInt8]
    {
        var Z=[UInt8](repeating: 0,count: ECDH.EFS)
        var VZ=[UInt8](repeating: 0,count: 3*ECDH.EFS+1)
        var K1=[UInt8](repeating: 0,count: CONFIG_CURVE.AESKEY)
        var K2=[UInt8](repeating: 0,count: CONFIG_CURVE.AESKEY)

        var TAG=[UInt8](repeating: 0,count: T.count)

        if ECPSVDP_DH(U,V,&Z,0) != 0 {return [UInt8]()}

        for i in 0 ..< 2*ECDH.EFS+1 {VZ[i]=V[i]}
        for i in 0 ..< ECDH.EFS {VZ[2*EFS+1+i]=Z[i]}

        let K=HMAC.KDF2(HMAC.MC_SHA2,sha,VZ,P1,2*CONFIG_CURVE.AESKEY)

        for i in 0 ..< CONFIG_CURVE.AESKEY {K1[i]=K[i]; K2[i]=K[CONFIG_CURVE.AESKEY+i]}

        let M=AES.CBC_IV0_DECRYPT(K1,C)

        if M.count==0 {return M}

        let L2=HMAC.inttoBytes(P2.count,8)

        var AC=[UInt8](repeating: 0,count: C.count+P2.count+8)

        for i in 0 ..< C.count {AC[i]=C[i]}
        for i in 0 ..< P2.count {AC[C.count+i]=P2[i]}
        for i in 0 ..< 8 {AC[C.count+P2.count+i]=L2[i]}

        HMAC.HMAC1(HMAC.MC_SHA2,sha,&TAG,TAG.count,K2,AC)

        if !ncomp(T,TAG,T.count) {return [UInt8]()}

        return M;
    }

}
