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
//  eddsa.swift
//
//  Created by Michael Scott on 25/04/2023.
//

//import Foundation
//import Darwin
import core

/* EDDSA API Functions */

public struct EDDSA
{
    static let INVALID_PUBLIC_KEY:Int = -2
    static let ERROR:Int = -3
    //static let INVALID:Int = -4
    static public let EFS=Int(CONFIG_BIG.MODBYTES);
    static public let EGS=Int(CONFIG_BIG.MODBYTES);
    static public let SHA512=64

// Transform a point multiplier to RFC7748 form
    static func rfc7748(_ r: inout BIG)
    {
        var lg=0
        var t=BIG(1)
        var c=ROM.CURVE_Cof_I
        while c != 1 {
            lg+=1
            c/=2
        }
        let n=UInt(8*EGS-lg+1)
        r.mod2m(n)
        t.shl(n)
        r.add(t)
        c=r.lastbits(UInt(lg))
        r.dec(c)
    }

// reverse first n bytes of buff - for little endian
    static func reverse(_ n: Int,_ buff:inout [UInt8]) {
        for i in 0 ..< n/2 { 
            let ch = buff[i]
            buff[i] = buff[n - i - 1] 
            buff[n - i - 1] = ch
        }    
    }

// dom - domain function
    static func dom(_ ds: String,_ ph: Bool,_ cl: Int,_ domain:inout [UInt8]) -> Int {
        let dsb=[UInt8]( (ds).utf8 )
        let n=dsb.count;
        for i in 0 ..< n {
            domain[i]=dsb[i]
        }
        if ph {
            domain[n]=1
        } else { 
            domain[n]=0
        }
        domain[n+1]=UInt8(cl)
        return n+2
    }

    @discardableResult static func h(_ s: [UInt8],_ digest:inout [UInt8]) -> Int {
        if CONFIG_CURVE.AESKEY<=16 { // for ed25519?
            var sh=HASH512()
            for i in 0 ..< s.count {
                sh.process(s[i])
            }
            let hs = sh.hash();
            for i in 0 ..< 64 {
                digest[i]=hs[i]
            }
            return 64;
        } else {                       // for ed448?
            var sh=SHA3(SHA3.SHAKE256)
            for i in 0 ..< s.count {
                sh.process(s[i])
            }
            sh.shake(&digest,2*s.count)
            return 2*s.count
        }
    }

    static func h2(_ ph: Bool,_ ctx: [UInt8]?,_ r: [UInt8],_ q: [UInt8],_ m: [UInt8]) -> DBIG {
        let b=q.count;
        var cl=0;
        if ctx != nil {
            cl=ctx!.count
        }
        var domain=[UInt8](repeating: 0,count: 64)
       
        if CONFIG_CURVE.AESKEY<=16 { // Ed25519??
            var sh=HASH512()
            if ph || cl>0 {                   // if not prehash and no context, omit dom2()
                let dl=dom("SigXxx no Xxx collisions",ph,cl,&domain)
                for i in 0..<dl {
                    sh.process(domain[i])
                }
                if ctx != nil {
                    for i in 0..<cl {
                        sh.process(ctx![i])
                    }
                }
            }
            for i in 0..<b {
                sh.process(r[i])
            }
            for i in 0..<b {
                sh.process(q[i])
            }
            for i in 0..<m.count {
                sh.process(m[i])
            }
            var h=sh.hash()
            reverse(64,&h)
            return DBIG.fromBytes(h)
        } else {                       // for ed448?
            let dl=dom("SigXxx",ph,cl,&domain)
            var h = [UInt8](repeating: 0,count: 128)
            var sh=SHA3(SHA3.SHAKE256)
            for i in 0..<dl {

                sh.process(domain[i])
            }   
            if ctx != nil {
                for i in 0..<cl {
                    sh.process(ctx![i])
                }
            }

            for i in 0..<b { 
                sh.process(r[i])
            }
            for i in 0..<b {
                sh.process(q[i])
            }
            for i in 0..<m.count { 
                sh.process(m[i])
            }

            sh.shake(&h,2*b);
            var arr=Array(h[0 ..< 2*b])
            reverse(2*b,&arr);
            return DBIG.fromBytes(arr);
        }
    }

    static func getr(_ ph: Bool,_ b: Int,_ digest: [UInt8],_ ctx: [UInt8]?,_ m: [UInt8]) -> DBIG {
        var cl=0;
        if ctx != nil {
            cl=ctx!.count
        }
        var domain=[UInt8](repeating: 0,count: 64)

        if CONFIG_CURVE.AESKEY<=16 { // Ed25519??
            var sh=HASH512()
            if ph || cl>0 {                   // if not prehash and no context, omit dom2()
                let dl=dom("SigXxx no Xxx collisions",ph,cl,&domain)
                for i in 0..<dl {
                    sh.process(domain[i])
                }
                if ctx != nil {
                    for i in 0..<cl {
                        sh.process(ctx![i])
                    }
                }
            }
       
            for i in b..<2*b {
                sh.process(digest[i])
            }
            for i in 0..<m.count {
                sh.process(m[i])
            }
            var h=sh.hash()
            reverse(64,&h)
            return DBIG.fromBytes(h)
        } else {                       // for ed448?
            let dl=dom("SigXxx",ph,cl,&domain)
            var h = [UInt8](repeating: 0,count: 128)
            var sh=SHA3(SHA3.SHAKE256)
            for i in 0..<dl {
                sh.process(domain[i])
            }
            if ctx != nil {
                for i in 0..<cl {
                    sh.process(ctx![i])
                }
            }
            for i in b ..< 2*b {
                sh.process(digest[i])
            }
            for i in 0..<m.count { 
                sh.process(m[i])
            }
            sh.shake(&h,2*b);
            var arr=Array(h[0 ..< 2*b])
            reverse(2*b,&arr);
            return DBIG.fromBytes(arr);
        }
    }

// encode integer (little endian)
    @discardableResult static func encode_int(_ x: BIG,_ w:inout [UInt8]) -> Int {
        var index=0
        if 8*EFS==CONFIG_FIELD.MODBITS {
            index=1 // extra byte needed for compression
        }
        let b=EFS+index;

        w[0]=0;
        x.tobytearray(&w,index);
        reverse(b,&w);
        return b;
    }

    static func encode(_ P: ECP,_ w:inout [UInt8]) {
        var index=0
        if 8*EFS==CONFIG_FIELD.MODBITS {
            index=1 // extra byte needed for compression
        }
        let b=EFS+index;

        let x=P.getX();
        let y=P.getY();
        encode_int(y,&w);
        w[b-1]|=UInt8(x.parity()<<7);
    }

// get sign
    static func getsign(_ x: [UInt8]) -> Int{
        var index=0
        if 8*EFS==CONFIG_FIELD.MODBITS {
            index=1 // extra byte needed for compression
        }
        let b=EFS+index;

        if (x[b-1]&0x80) != 0 {
            return 1;
        } else { 
            return 0;
        }
    }

// decode integer (little endian)
    static func decode_int(_ strip_sign: Bool,_ ei: [UInt8]) -> BIG {
        var index=0
        if 8*EFS==CONFIG_FIELD.MODBITS {
            index=1 // extra byte needed for compression
        }
        let b=EFS+index;

        var r = [UInt8](repeating: 0,count: EFS+1)

        for i in 0..<b {
            r[i]=ei[i];
        }
        reverse(b,&r);

        if strip_sign {
            r[0]&=0x7f;
        }
        return BIG.frombytearray(r,index);
    }

// decode compressed point
    static func decode(_ w: [UInt8]) -> ECP {
        let sign=getsign(w) // lsb of x
        let y=decode_int(true,w)
        let one = FP(1)
        var hint:FP?=FP()
        var x=FP(y); x.sqr() 
        var d=FP(x) 
        x.sub(one)
        x.norm()
        var t = FP(BIG(ROM.CURVE_B))
        d.mul(t)
        if CONFIG_CURVE.CURVE_A == 1 {
            d.sub(one)
        }
        if CONFIG_CURVE.CURVE_A == -1 {
            d.add(one)
        }
        d.norm()
// inverse square root trick for sqrt(x/d)
        t.copy(x)
        t.sqr()
        x.mul(t)
        x.mul(d)
        if x.qr(&hint) != 1 {
            return ECP()
        }
        d.copy(x.sqrt(hint))
        x.inverse(hint)
        x.mul(d)
        x.mul(t)
        x.reduce()
        if x.redc().parity() != sign {
            x.neg()
        }
        x.norm()
        return ECP(x.redc(),y)
    }

    static func key_pair_regenerate(_ d: [UInt8],_ q:inout [UInt8])
    {
        var index=0
        if 8*EFS==CONFIG_FIELD.MODBITS {
            index=1 // extra byte needed for compression
        }
        let b=EFS+index;
        var G=ECP.generator();
        
        var digest = [UInt8](repeating: 0,count: 128)
        h(d,&digest)
// reverse bytes for little endian
        reverse(b,&digest);
        var s=BIG.frombytearray(digest,index);
        rfc7748(&s);
        G.copy(G.mul(s));
        encode(G,&q);
    }

/* Calculate a public/private EC GF(p) key pair w,s where W=s.G mod EC(p),
 * where s is the secret key and W is the public key
 * and G is fixed generator.
 * If RNG is NULL then the private key is provided externally in s
 * otherwise it is generated randomly internally */
    @discardableResult static public func KEY_PAIR_GENERATE(_ rng: inout RAND?,_ d:inout [UInt8],_ q:inout [UInt8]) -> Int
    {
        var index=0
        if 8*EFS==CONFIG_FIELD.MODBITS {
            index=1 // extra byte needed for compression
        }
        let b=EFS+index;
        var G=ECP.generator();


        if rng != nil {
            for i in 0 ..< b {
                d[i]=rng!.getByte();
            }
        }
        
        var digest = [UInt8](repeating: 0,count: 128)
        h(d,&digest)
// reverse bytes for little endian
        reverse(b,&digest);
        var s=BIG.frombytearray(digest,index);
        rfc7748(&s);
        G.copy(G.mul(s));
        encode(G,&q);
        return 0;
    }

// Generate a signature using key pair (d,q) on message m
// Set ph=true if message has already been pre-hashed
// if ph=false, then context should be NULL for ed25519. However RFC8032 mode ed25519ctx is supported by supplying a non-NULL or non-empty context

    @discardableResult static public func SIGNATURE(_ ph: Bool,_ d: [UInt8],_ ctx: [UInt8]?,_ m: [UInt8],_ sig:inout [UInt8]) ->Int {
        var digest = [UInt8](repeating: 0,count: 128)
        h(d,&digest) // hash of private key
        var res = 0
        var index=0
        if 8*EFS==CONFIG_FIELD.MODBITS {
            index=1 // extra byte needed for compression
        }
        let b=EFS+index

        var q = [UInt8](repeating: 0,count: EFS+1)
        key_pair_regenerate(d,&q)
        let qs=Array(q[0 ..< b])

        let r=BIG(ROM.CURVE_Order)

        if d.count != qs.count || d.count != b {
            res=INVALID_PUBLIC_KEY
        }
        if res==0 {
            var t = [UInt8](repeating: 0,count: EFS+1)
            var dr=getr(ph,b,digest,ctx!,m)
            let sr=dr.mod(r)
            let R=ECP.generator().mul(sr)
            encode(R,&t)
            for i in 0..<b {
                sig[i]=t[i]
            }
            reverse(b,&digest)
            var s=BIG.frombytearray(digest,index)
            rfc7748(&s)
            dr=h2(ph,ctx!,sig,qs,m)
            let sd=dr.mod(r)
            encode_int(BIG.modadd(sr,BIG.modmul(s,sd,r),r),&t);
            for i in 0..<b {
                sig[b+i]=t[i]
            }
        }
        return res;
    }

    static public func VERIFY(_ ph: Bool,_ q: [UInt8],_ ctx: [UInt8]?,_ m: [UInt8],_ sig: [UInt8]) -> Bool {
        var index=0
        if 8*EFS==CONFIG_FIELD.MODBITS {
            index=1 // extra byte needed for compression
        }
        let b=EFS+index

        var lg=0;
        var c=ROM.CURVE_Cof_I
        while c != 1 {
            lg+=1
            c/=2
        }
        var s = [UInt8](repeating: 0,count: EFS+1)
        let r=BIG(ROM.CURVE_Order)
        var R=decode(sig);
        if R.is_infinity() {
            return false;
        }
        for i in 0..<b {
            s[i]=sig[b+i]
        }        
        let t=decode_int(false,s)

        if BIG.comp(t,r)>=0 {
            return false
        }   
        var du=h2(ph,ctx!,sig,q,m)
        let su=du.mod(r)

        var G=ECP.generator()
        var QD=decode(q); 
        if QD.is_infinity() {
            return false;
        }
        QD.neg();
        for _ in 0..<lg { // use cofactor 2^c
            G.dbl(); QD.dbl(); R.dbl();
        }

        if !G.mul2(t,QD,su).equals(R) {
            return false;
        }
        return true;
    }

}
