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
//  Created by Michael Scott on 20/04/2020.
//  Copyright (c) 2020 Michael Scott. All rights reserved.
//

/* Run through some test vectors for hash-to-curve draft standard */

import Foundation
import core // comment out for Xcode
import ed25519
import c25519
import nist256
import goldilocks
import secp256k1
import bls12381

func ceil(_ a: Int,_ b:Int) -> Int {
    return (((a)-1)/(b)+1)
}

public func hash_to_field_ed25519(_ hf: Int,_ hlen:Int,_ DST: [UInt8], _ M:[UInt8],_ ctr: Int) -> [ed25519.FP] {
    var u=[ed25519.FP]()
    let q=ed25519.BIG(ed25519.ROM.Modulus)
    let k=q.nbits()
    let r=ed25519.BIG(ed25519.ROM.CURVE_Order)
    let m=r.nbits()
    let L=ceil(k+ceil(m,2),8)
    let OKM=HMAC.XMD_Expand(hf,hlen,L*ctr,DST,M)
    var fd=[UInt8](repeating: 0,count: L)
    for i in 0..<ctr {
        for j in 0..<L {
            fd[j]=OKM[i*L+j]
        }
		var dx=ed25519.DBIG.fromBytes(fd)
		let w=ed25519.FP(dx.mod(q))
		u.append(w)
    }

    return u
}

public func htp_ed25519(_ mess: String)
{
    print("\nRandom oracle - message= "+mess)
    let M=[UInt8](mess.utf8)
	var DST = [UInt8]("QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_".utf8)
	var u=hash_to_field_ed25519(HMAC.MC_SHA2,64,DST,M,2)

    print("u[0]= "+u[0].toString())
    print("u[1]= "+u[1].toString())

    var P=ed25519.ECP.map2point(u[0])
    print("Q[0]= "+P.toString())
    let P1=ed25519.ECP.map2point(u[1])
    print("Q[1]= "+P1.toString())
    P.add(P1)
    P.cfp()
    P.affine()
    print("P= "+P.toString())

    print("\nNon-Uniform")
    DST = [UInt8]("QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_".utf8)
    u=hash_to_field_ed25519(HMAC.MC_SHA2,64,DST,M,1)
    print("u[0]= "+u[0].toString())
    P=ed25519.ECP.map2point(u[0])
    print("Q= "+P.toString())
    P.cfp()
    P.affine()
    print("P= "+P.toString())
}

public func hash_to_field_c25519(_ hf: Int,_ hlen:Int,_ DST: [UInt8], _ M:[UInt8],_ ctr: Int) -> [c25519.FP] {
    var u=[c25519.FP]()
    let q=c25519.BIG(c25519.ROM.Modulus)
    let k=q.nbits()
    let r=c25519.BIG(c25519.ROM.CURVE_Order)
    let m=r.nbits()
    let L=ceil(k+ceil(m,2),8)
    let OKM=HMAC.XMD_Expand(hf,hlen,L*ctr,DST,M)
    var fd=[UInt8](repeating: 0,count: L)
    for i in 0..<ctr {
        for j in 0..<L {
            fd[j]=OKM[i*L+j]
        }
		var dx=c25519.DBIG.fromBytes(fd)
		let w=c25519.FP(dx.mod(q))
		u.append(w)
    }

    return u
}

public func htp_c25519(_ mess: String)
{
    let M=[UInt8](mess.utf8)
    print("\nNon-Uniform")
    let DST = [UInt8]("QUUX-V01-CS02-with-curve25519_XMD:SHA-512_ELL2_NU_".utf8)
    let u=hash_to_field_c25519(HMAC.MC_SHA2,64,DST,M,1)
    print("u[0]= "+u[0].toString())
    var P=c25519.ECP.map2point(u[0])
    print("Q= "+P.toString())
    P.cfp()
    P.affine()
    print("P= "+P.toString())
}


public func hash_to_field_nist256(_ hf: Int,_ hlen:Int,_ DST: [UInt8], _ M:[UInt8],_ ctr: Int) -> [nist256.FP] {
    var u=[nist256.FP]()
    let q=nist256.BIG(nist256.ROM.Modulus)
    let k=q.nbits()
    let r=nist256.BIG(nist256.ROM.CURVE_Order)
    let m=r.nbits()
    let L=ceil(k+ceil(m,2),8)
    let OKM=HMAC.XMD_Expand(hf,hlen,L*ctr,DST,M)
    var fd=[UInt8](repeating: 0,count: L)
    for i in 0..<ctr {
        for j in 0..<L {
            fd[j]=OKM[i*L+j]
        }
		var dx=nist256.DBIG.fromBytes(fd)
		let w=nist256.FP(dx.mod(q))
		u.append(w)
    }

    return u
}

public func htp_nist256(_ mess: String)
{
    print("\nRandom oracle - message= "+mess)
    let M=[UInt8](mess.utf8)
	var DST = [UInt8]("QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_RO_".utf8)
	var u=hash_to_field_nist256(HMAC.MC_SHA2,nist256.CONFIG_CURVE.HASH_TYPE,DST,M,2)

    print("u[0]= "+u[0].toString())
    print("u[1]= "+u[1].toString())

    var P=nist256.ECP.map2point(u[0])
    print("Q[0]= "+P.toString())
    let P1=nist256.ECP.map2point(u[1])
    print("Q[1]= "+P1.toString())
    P.add(P1)
    P.cfp()
    P.affine()
    print("P= "+P.toString())

    print("\nNon-Uniform");
    DST = [UInt8]("QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_NU_".utf8)
    u=hash_to_field_nist256(HMAC.MC_SHA2,nist256.CONFIG_CURVE.HASH_TYPE,DST,M,1);
    print("u[0]= "+u[0].toString())
    P=nist256.ECP.map2point(u[0])
    print("Q= "+P.toString())
    P.cfp()
    P.affine()
    print("P= "+P.toString())
}

public func hash_to_field_goldilocks(_ hf: Int,_ hlen:Int,_ DST: [UInt8], _ M:[UInt8],_ ctr: Int) -> [goldilocks.FP] {
    var u=[goldilocks.FP]()
    let q=goldilocks.BIG(goldilocks.ROM.Modulus)
    let k=q.nbits()
    let r=goldilocks.BIG(goldilocks.ROM.CURVE_Order)
    let m=r.nbits()
    let L=ceil(k+ceil(m,2),8)
    let OKM=HMAC.XMD_Expand(hf,hlen,L*ctr,DST,M)
    var fd=[UInt8](repeating: 0,count: L)
    for i in 0..<ctr {
        for j in 0..<L {
            fd[j]=OKM[i*L+j]
        }
		var dx=goldilocks.DBIG.fromBytes(fd)
		let w=goldilocks.FP(dx.mod(q))
		u.append(w)
    }

    return u
}

public func htp_goldilocks(_ mess: String)
{
    print("\nRandom oracle - message= "+mess)
    let M=[UInt8](mess.utf8)
	var DST = [UInt8]("QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_RO_".utf8)
	var u=hash_to_field_goldilocks(HMAC.MC_SHA2,goldilocks.CONFIG_CURVE.HASH_TYPE,DST,M,2)

    print("u[0]= "+u[0].toString())
    print("u[1]= "+u[1].toString())

    var P=goldilocks.ECP.map2point(u[0])
    print("Q[0]= "+P.toString())
    let P1=goldilocks.ECP.map2point(u[1])
    print("Q[1]= "+P1.toString())
    P.add(P1)
    P.cfp()
    P.affine()
    print("P= "+P.toString())

    print("\nNon-Uniform");
    DST = [UInt8]("QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_NU_".utf8)
    u=hash_to_field_goldilocks(HMAC.MC_SHA2,goldilocks.CONFIG_CURVE.HASH_TYPE,DST,M,1);
    print("u[0]= "+u[0].toString())
    P=goldilocks.ECP.map2point(u[0])
    print("Q= "+P.toString())
    P.cfp()
    P.affine()
    print("P= "+P.toString())
}

public func hash_to_field_secp256k1(_ hf: Int,_ hlen:Int,_ DST: [UInt8], _ M:[UInt8],_ ctr: Int) -> [secp256k1.FP] {
    var u=[secp256k1.FP]()
    let q=secp256k1.BIG(secp256k1.ROM.Modulus)
    let k=q.nbits()
    let r=secp256k1.BIG(secp256k1.ROM.CURVE_Order)
    let m=r.nbits()
    let L=ceil(k+ceil(m,2),8)
    let OKM=HMAC.XMD_Expand(hf,hlen,L*ctr,DST,M)
    var fd=[UInt8](repeating: 0,count: L)
    for i in 0..<ctr {
        for j in 0..<L {
            fd[j]=OKM[i*L+j]
        }
		var dx=secp256k1.DBIG.fromBytes(fd)
		let w=secp256k1.FP(dx.mod(q))
		u.append(w)
    }

    return u
}

public func htp_secp256k1(_ mess: String)
{
    print("\nRandom oracle - message= "+mess)
    let M=[UInt8](mess.utf8)
	var DST = [UInt8]("QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_RO_".utf8)
	var u=hash_to_field_secp256k1(HMAC.MC_SHA2,secp256k1.CONFIG_CURVE.HASH_TYPE,DST,M,2)

    print("u[0]= "+u[0].toString())
    print("u[1]= "+u[1].toString())

    var P=secp256k1.ECP.map2point(u[0])
    print("Q[0]= "+P.toString())
    let P1=secp256k1.ECP.map2point(u[1])
    print("Q[1]= "+P1.toString())
    P.add(P1)
    P.cfp()
    P.affine()
    print("P= "+P.toString())

    print("\nNon-Uniform");
    DST = [UInt8]("QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_NU_".utf8)
    u=hash_to_field_secp256k1(HMAC.MC_SHA2,secp256k1.CONFIG_CURVE.HASH_TYPE,DST,M,1);
    print("u[0]= "+u[0].toString())
    P=secp256k1.ECP.map2point(u[0])
    print("Q= "+P.toString())
    P.cfp()
    P.affine()
    print("P= "+P.toString())
}

public func hash_to_field_bls12381(_ hf: Int,_ hlen:Int,_ DST: [UInt8], _ M:[UInt8],_ ctr: Int) -> [bls12381.FP] {
    var u=[bls12381.FP]()
    let q=bls12381.BIG(bls12381.ROM.Modulus)
    let k=q.nbits()
    let r=bls12381.BIG(bls12381.ROM.CURVE_Order)
    let m=r.nbits()
    let L=ceil(k+ceil(m,2),8)
    let OKM=HMAC.XMD_Expand(hf,hlen,L*ctr,DST,M)
    var fd=[UInt8](repeating: 0,count: L)
    for i in 0..<ctr {
        for j in 0..<L {
            fd[j]=OKM[i*L+j]
        }
		var dx=bls12381.DBIG.fromBytes(fd)
		let w=bls12381.FP(dx.mod(q))
		u.append(w)
    }

    return u
}

public func htp_bls12381(_ mess: String)
{
    print("\nRandom oracle - message= "+mess)
    let M=[UInt8](mess.utf8)
	var DST = [UInt8]("QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_RO_".utf8)
	var u=hash_to_field_bls12381(HMAC.MC_SHA2,bls12381.CONFIG_CURVE.HASH_TYPE,DST,M,2)

    print("u[0]= "+u[0].toString())
    print("u[1]= "+u[1].toString())

    var P=bls12381.ECP.map2point(u[0])
    print("Q[0]= "+P.toString())
    let P1=bls12381.ECP.map2point(u[1])
    print("Q[1]= "+P1.toString())
    P.add(P1)
    P.cfp()
    P.affine()
    print("P= "+P.toString())

    print("\nNon-Uniform");
    DST = [UInt8]("QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_NU_".utf8)
    u=hash_to_field_bls12381(HMAC.MC_SHA2,bls12381.CONFIG_CURVE.HASH_TYPE,DST,M,1);
    print("u[0]= "+u[0].toString())
    P=bls12381.ECP.map2point(u[0])
    print("Q= "+P.toString())
    P.cfp()
    P.affine()
    print("P= "+P.toString())
}


public func hash_to_field2_bls12381(_ hf: Int,_ hlen:Int,_ DST: [UInt8], _ M:[UInt8],_ ctr: Int) -> [bls12381.FP2] {
    var u=[bls12381.FP2]()
    let q=bls12381.BIG(bls12381.ROM.Modulus)
    let k=q.nbits()
    let r=bls12381.BIG(bls12381.ROM.CURVE_Order)
    let m=r.nbits()
    let L=ceil(k+ceil(m,2),8)
    let OKM=HMAC.XMD_Expand(hf,hlen,2*L*ctr,DST,M)
    var fd=[UInt8](repeating: 0,count: L)
    for i in 0..<ctr {
        for j in 0..<L {
            fd[j]=OKM[2*i*L+j]
        }
		var dx=bls12381.DBIG.fromBytes(fd)
		let w1=bls12381.FP(dx.mod(q))

        for j in 0..<L {
            fd[j]=OKM[(2*i+1)*L+j]
        }
		dx=bls12381.DBIG.fromBytes(fd)
		let w2=bls12381.FP(dx.mod(q))
		u.append(FP2(w1,w2))
    }

    return u
}

public func htp2_bls12381(_ mess: String)
{
    print("\nRandom oracle - message= "+mess)
    let M=[UInt8](mess.utf8)
	var DST = [UInt8]("QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_RO_".utf8)
	var u=hash_to_field2_bls12381(HMAC.MC_SHA2,bls12381.CONFIG_CURVE.HASH_TYPE,DST,M,2)

    print("u[0]= "+u[0].toString())
    print("u[1]= "+u[1].toString())

    var P=bls12381.ECP2.map2point(u[0])
    print("Q[0]= "+P.toString())
    let P1=bls12381.ECP2.map2point(u[1])
    print("Q[1]= "+P1.toString())
    P.add(P1)
    P.cfp()
    P.affine()
    print("P= "+P.toString())

    print("\nNon-Uniform");
    DST = [UInt8]("QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_NU_".utf8)
    u=hash_to_field2_bls12381(HMAC.MC_SHA2,bls12381.CONFIG_CURVE.HASH_TYPE,DST,M,1);
    print("u[0]= "+u[0].toString())
    P=bls12381.ECP2.map2point(u[0])
    print("Q= "+P.toString())
    P.cfp()
    P.affine()
    print("P= "+P.toString())
}


print("\nTesting HTP for curve ed25519\n")
htp_ed25519("")
htp_ed25519("abc")
htp_ed25519("abcdef0123456789")
htp_ed25519("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq")
htp_ed25519("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")

print("\nTesting HTP for curve c25519\n")
htp_c25519("")
htp_c25519("abc")
htp_c25519("abcdef0123456789")
htp_c25519("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq")
htp_c25519("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")

print("\nTesting HTP for curve nist256\n")
htp_nist256("")
htp_nist256("abc")
htp_nist256("abcdef0123456789")
htp_nist256("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq")
htp_nist256("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")

print("\nTesting HTP for curve goldilocks\n")
htp_goldilocks("")
htp_goldilocks("abc")
htp_goldilocks("abcdef0123456789")
htp_goldilocks("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq")
htp_goldilocks("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")

print("\nTesting HTP for curve secp256k1\n")
htp_secp256k1("")
htp_secp256k1("abc")
htp_secp256k1("abcdef0123456789")
htp_secp256k1("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq")
htp_secp256k1("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")

print("\nTesting HTP for curve bls12381_G1\n")
htp_bls12381("")
htp_bls12381("abc")
htp_bls12381("abcdef0123456789")
htp_bls12381("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq")
htp_bls12381("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")

print("\nTesting HTP for curve bls12381_G2\n")
htp2_bls12381("")
htp2_bls12381("abc")
htp2_bls12381("abcdef0123456789")
htp2_bls12381("q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq")
htp2_bls12381("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
