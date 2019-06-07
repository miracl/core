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
//  ecdh.swift
//
//  Created by Michael Scott on 30/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

//import Foundation
//import Darwin
import amcl

/* Elliptic Curve API high-level functions  */

public struct ECDH
{
    static let INVALID_PUBLIC_KEY:Int = -2
    static let ERROR:Int = -3
    static let INVALID:Int = -4
    static public let EFS=Int(CONFIG_BIG.MODBYTES);
    static public let EGS=Int(CONFIG_BIG.MODBYTES);
    static public let SHA256=32
    static public let SHA384=48
    static public let SHA512=64

    /* Convert Integer to n-byte array */
    private static func inttoBytes(_ n: Int,_ len:Int) -> [UInt8]
    {
        var b=[UInt8](repeating: 0,count: len)
        var nn=n
    
        var i=len;
        while (nn>0 && i>0)
        {
            i -= 1;
            b[i]=UInt8(nn&0xff);
            nn /= 256;
        }
        return b;
    }
    
    private static func hashit(_ sha: Int,_ A:[UInt8],_ n: Int32,_ B:[UInt8]?,_ pad:Int) -> [UInt8]
    {
        var R=[UInt8]()
        if sha==SHA256
        {
            var H=HASH256()
            H.process_array(A); if n>0 {H.process_num(n)}
                if B != nil {H.process_array(B!)}
            R=H.hash()
        }
        if sha==SHA384
        {
            var H=HASH384()
            H.process_array(A); if n>0 {H.process_num(n)}
            if B != nil {H.process_array(B!)}
            R=H.hash()
        }
        if sha==SHA512
        {
            var H=HASH512()
            H.process_array(A); if n>0 {H.process_num(n)}
            if B != nil {H.process_array(B!)}
            R=H.hash()
        }
        if R.isEmpty || pad==0 {return R}
        var W=[UInt8](repeating: 0,count: pad)
        if pad<=sha
        {
            for i in 0 ..< pad {W[i]=R[i]}
        }
        else
        {
            for i in 0 ..< sha {W[i+pad-sha]=R[i]}
        }
        return W
    }
    
    /* Key Derivation Functions */
    /* Input octet Z */
    /* Output key of length olen */
    static public func KDF1(_ sha: Int,_ Z: [UInt8],_ olen:Int) -> [UInt8]
    {
    /* NOTE: the parameter olen is the length of the output K in bytes */
        let hlen=sha
        var K=[UInt8](repeating: 0,count: olen)
        var k=0;
    
        var cthreshold=olen/hlen; if (olen%hlen) != 0 {cthreshold += 1}
    
        for counter in 0 ..< cthreshold
        {
            let B=hashit(sha,Z,Int32(counter),nil,0)
            if k+hlen>olen {for i in 0 ..< olen%hlen {K[k]=B[i]; k+=1}}
            else {for i in 0 ..< hlen {K[k]=B[i]; k+=1}}
        }
        return K;
    }
    
    static public func KDF2(_ sha:Int,_ Z:[UInt8],_ P:[UInt8]?,_ olen:Int) -> [UInt8]
    {
    /* NOTE: the parameter olen is the length of the output k in bytes */
        let hlen=sha
        var K=[UInt8](repeating: 0,count: olen)
        var k=0;
    
        var cthreshold=olen/hlen; if (olen%hlen) != 0 {cthreshold += 1}
    
        for counter in 1...cthreshold
        {
            let B=hashit(sha,Z,Int32(counter),P,0)
            if k+hlen>olen {for i in 0 ..< olen%hlen {K[k]=B[i]; k+=1}}
            else {for i in 0 ..< hlen {K[k]=B[i]; k+=1}}
        }
        return K;
    }
   
    /* Password based Key Derivation Function */
    /* Input password p, salt s, and repeat count */
    /* Output key of length olen */
    static public func PBKDF2(_ sha:Int,_ Pass:[UInt8],_ Salt:[UInt8],_ rep:Int,_ olen:Int) -> [UInt8]
    {
        var d=olen/sha;
        if (olen%sha) != 0 {d+=1}
        var F=[UInt8](repeating: 0,count: sha)
        var U=[UInt8](repeating: 0,count: sha)
        var S=[UInt8](repeating: 0,count: Salt.count+4)
    
        var K=[UInt8](repeating: 0,count: d*sha)
        
        var opt=0;
    
        for i in 1...d
        {
            for j in 0 ..< Salt.count {S[j]=Salt[j]}
            var N=ECDH.inttoBytes(i,4);
            for j in 0 ..< 4 {S[Salt.count+j]=N[j]}
            
            ECDH.HMAC(sha,S,Pass,&F);
             
            for j in 0 ..< sha {U[j]=F[j]}
            for _ in 2...rep
            {
                ECDH.HMAC(sha,U,Pass,&U);
                for k in 0 ..< sha {F[k]^=U[k]}
            }
            for j in 0 ..< sha {K[opt]=F[j]; opt+=1}
        }
        var key=[UInt8](repeating: 0,count: olen)
        for i in 0 ..< olen {key[i]=K[i]}
        return key;
    }
    
    /* Calculate HMAC of m using key k. HMAC is tag of length olen */
    @discardableResult static public func HMAC(_ sha:Int,_ M:[UInt8],_ K:[UInt8],_ tag:inout [UInt8]) -> Int
    {
    /* Input is from an octet m        *
    * olen is requested output length in bytes. k is the key  *
    * The output is the calculated tag */
        var b=64
        if sha>32 {b=128}
        
        var K0=[UInt8](repeating: 0,count: b)
        let olen=tag.count;
        var B=[UInt8]();
        
        if olen<4 {return 0}
    
        if (K.count > b)
        {
            B=hashit(sha,K,0,nil,0)
            for i in 0 ..< sha {K0[i]=B[i]}
        }
        else
        {
            for i in 0 ..< K.count {K0[i]=K[i]}
        }
        for i in 0 ..< b {K0[i]^=0x36}
        
        B=hashit(sha,K0,0,M,0)
    
        for i in 0 ..< b {K0[i]^=0x6a}
        B=hashit(sha,K0,0,B,olen)
        
        for i in 0 ..< olen {tag[i]=B[i]}
    
        return 1;
    }
    /* AES encryption/decryption. Encrypt byte array M using key K and returns ciphertext */
    static public func AES_CBC_IV0_ENCRYPT(_ K:[UInt8],_ M:[UInt8]) -> [UInt8]
    { /* AES CBC encryption, with Null IV and key K */
    /* Input is from an octet string M, output is to an octet string C */
    /* Input is padded as necessary to make up a full final block */
        var a=AES();
        var buff=[UInt8](repeating: 0,count: 16)
        let clen=16+(M.count/16)*16;
    
        var C=[UInt8](repeating: 0,count: clen)
    
        a.init_it(AES.CBC,K.count,K,nil)
    
        var ipt=0; var opt=0;
        var fin=false;
        var i:Int=0
        while true
        {
            i=0
            while i<16
            {
                if (ipt<M.count) {buff[i]=M[ipt]; ipt+=1}
				else {fin=true; break;}
                i+=1
            }
            if fin {break}
            a.encrypt(&buff);
            for j in 0 ..< 16
                {C[opt]=buff[j]; opt+=1}
        }
    
    /* last block, filled up to i-th index */
    
        let padlen=16-i;
        for j in i ..< 16 {buff[j]=UInt8(padlen&0xff)}
    
        a.encrypt(&buff);
    
        for j in 0 ..< 16
            {C[opt]=buff[j]; opt+=1}
        a.end();
        return C;
    }
    
    /* returns plaintext if all consistent, else returns null string */
    static public func AES_CBC_IV0_DECRYPT(_ K:[UInt8],_ C:[UInt8]) -> [UInt8]
    { /* padding is removed */
        var a=AES();
        
        var buff=[UInt8](repeating: 0,count: 16)
        var MM=[UInt8](repeating: 0,count: C.count)
    
        var ipt=0; var opt=0;
    
        a.init_it(AES.CBC,K.count,K,nil);
    
        if C.count==0 {return [UInt8]()}
        var ch=C[ipt]; ipt+=1
    
        var fin=false;
        var i:Int=0
        while true
        {
            i=0
            while i<16
            {
				buff[i]=ch;
				if ipt>=C.count {fin=true; break;}
                else {ch=C[ipt]; ipt+=1}
                i+=1
            }
            a.decrypt(&buff);
            if fin {break}
            for j in 0 ..< 16
                {MM[opt]=buff[j]; opt+=1}
        }
    
        a.end();
        var bad=false;
        let padlen:Int=Int(buff[15]);
        if i != 15 || padlen<1 || padlen>16 {bad=true}
        if padlen>=2 && padlen<=16
        {
            for j in 16-padlen ..< 16 {if buff[j] != buff[15] {bad=true}}
        }
        if !bad
        {
            for j in 0 ..< 16-padlen
                {MM[opt]=buff[j]; opt+=1}
        }
    
        if bad {return [UInt8]()}
    
        var M=[UInt8](repeating: 0,count: opt)
        for j in 0 ..< opt {M[j]=MM[j]}
    
        return M;
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
            var k=BIG(1); k.shl((nb+4)/2)
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
    @discardableResult static public func ECPSVDP_DH(_ S:[UInt8],_ WD:[UInt8],_ Z:inout [UInt8]) -> Int
    {
        var res=0
        var T=[UInt8](repeating: 0,count: ECDH.EFS)
    
        var s=BIG.fromBytes(S)
    
        var W=ECP.fromBytes(WD)
        if W.is_infinity() {res=ECDH.ERROR}
    
        if (res==0)
        {
            let r=BIG(ROM.CURVE_Order)
            s.mod(r)
    
            W=W.mul(s);
            if W.is_infinity() {res=ERROR}
            else
            {
                W.getX().toBytes(&T);
                for i in 0 ..< ECDH.EFS {Z[i]=T[i]}
            }
        }
        return res;
    }
    /* IEEE ECDSA Signature, C and D are signature on F using private key S */
    static public func ECPSP_DSA(_ sha:Int,_ RNG: inout RAND,_ S:[UInt8],_ F:[UInt8],_ C:inout [UInt8],_ D:inout [UInt8]) -> Int
    {
        var T=[UInt8](repeating: 0,count: ECDH.EFS)
        let B=hashit(sha,F,0,nil,Int(CONFIG_BIG.MODBYTES))
    
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
        let B=hashit(sha,F,0,nil,Int(CONFIG_BIG.MODBYTES))
    
        let G=ECP.generator();
        let r=BIG(ROM.CURVE_Order)
    
        let c=BIG.fromBytes(C)
        var d=BIG.fromBytes(D)
        var f=BIG.fromBytes(B)
    
        if c.iszilch() || BIG.comp(c,r)>=0 || d.iszilch() || BIG.comp(d,r)>=0
            {res=ECDH.INVALID}
    
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
                if P.is_infinity() {res=INVALID}
				else
				{
                    d=P.getX();
                    d.mod(r);
                    if (BIG.comp(d,c) != 0) {res=ECDH.INVALID}
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
        if ECDH.ECPSVDP_DH(U,W,&Z) != 0 {return [UInt8]()}
    
        for i in 0 ..< 2*ECDH.EFS+1 {VZ[i]=V[i]}
        for i in 0 ..< ECDH.EFS {VZ[2*ECDH.EFS+1+i]=Z[i]}
    
    
        var K=KDF2(sha,VZ,P1,2*CONFIG_CURVE.AESKEY)
    
        for i in 0 ..< CONFIG_CURVE.AESKEY {K1[i]=K[i]; K2[i]=K[CONFIG_CURVE.AESKEY+i];}
    
        var C=AES_CBC_IV0_ENCRYPT(K1,M)
    
        var L2=inttoBytes(P2.count,8)
    
        var AC=[UInt8](repeating: 0,count: C.count+P2.count+8)
        
        for i in 0 ..< C.count {AC[i]=C[i]}
        for i in 0 ..< P2.count {AC[C.count+i]=P2[i]}
        for i in 0 ..< 8 {AC[C.count+P2.count+i]=L2[i]}
    
        ECDH.HMAC(sha,AC,K2,&T)
    
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
    
        if ECPSVDP_DH(U,V,&Z) != 0 {return [UInt8]()}
    
        for i in 0 ..< 2*ECDH.EFS+1 {VZ[i]=V[i]}
        for i in 0 ..< ECDH.EFS {VZ[2*EFS+1+i]=Z[i]}
    
        var K=KDF2(sha,VZ,P1,2*CONFIG_CURVE.AESKEY)
    
        for i in 0 ..< CONFIG_CURVE.AESKEY {K1[i]=K[i]; K2[i]=K[CONFIG_CURVE.AESKEY+i]}
    
        let M=ECDH.AES_CBC_IV0_DECRYPT(K1,C)
    
        if M.count==0 {return M}
    
        var L2=inttoBytes(P2.count,8)
    
        var AC=[UInt8](repeating: 0,count: C.count+P2.count+8)
    
        for i in 0 ..< C.count {AC[i]=C[i]}
        for i in 0 ..< P2.count {AC[C.count+i]=P2[i]}
        for i in 0 ..< 8 {AC[C.count+P2.count+i]=L2[i]}
    
        ECDH.HMAC(sha,AC,K2,&TAG)
    
        if !ncomp(T,TAG,T.count) {return [UInt8]()}	
    
        return M;
    }
    
}
