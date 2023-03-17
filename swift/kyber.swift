/*
 * Copyright (c) 2012-2023 MIRACL UK Ltd.
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
//  kyber.swift
//
//  Created by Michael Scott on 15/03/2023
//

/* Kyber API */

//import Foundation
//import core

public struct KYBER
{ 
    static public let KY_LGN:Int  = 8
    static public let KY_DEGREE:Int = 256 //(1 << KY_LGN)
    static public let KY_PRIME:Int32 = 0xD01

    static public let KY_ONE:Int32 = 0x549    // R mod Q
    static public let KY_QINV:Int32 = 62209   // q^(-1) mod 2^16

    static public let SECRET_CPA_SIZE_512:Int = (2*(KY_DEGREE*3)/2)
    static public let PUBLIC_SIZE_512:Int = (32+2*(KY_DEGREE*3)/2)
    static public let CIPHERTEXT_SIZE_512:Int = ((10*2+4)*KY_DEGREE/8)
    static public let SECRET_CCA_SIZE_512:Int = (SECRET_CPA_SIZE_512+PUBLIC_SIZE_512+64)
    static public let SHARED_SECRET_512:Int = 32

    static public let SECRET_CPA_SIZE_768:Int = (3*(KY_DEGREE*3)/2)
    static public let PUBLIC_SIZE_768:Int = (32+3*(KY_DEGREE*3)/2)
    static public let CIPHERTEXT_SIZE_768:Int = ((10*3+4)*KY_DEGREE/8)
    static public let SECRET_CCA_SIZE_768:Int = (SECRET_CPA_SIZE_768+PUBLIC_SIZE_768+64)
    static public let SHARED_SECRET_768:Int = 32

    static public let SECRET_CPA_SIZE_1024:Int = (4*(KY_DEGREE*3)/2)
    static public let PUBLIC_SIZE_1024:Int = (32+4*(KY_DEGREE*3)/2)
    static public let CIPHERTEXT_SIZE_1024:Int = ((11*4+5)*KY_DEGREE/8)
    static public let SECRET_CCA_SIZE_1024:Int = (SECRET_CPA_SIZE_1024+PUBLIC_SIZE_1024+64)
    static public let SHARED_SECRET_1024:Int = 32

    static public let KY_MAXK:Int = 4

// parameters for each security level
// K,eta1,eta2,du,dv,shared secret
    static public let PARAMS_512:[Int] = [2,3,2,10,4,32]
    static public let PARAMS_768:[Int] = [3,2,2,10,4,32]
    static public let PARAMS_1024:[Int] = [4,2,2,11,5,32]

// Translated from public domain reference implementation code - taken from https://github.com/pq-crystals/kyber 

    static private let zetas:[Int16] = [
        -1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
       -171,   622,  1577,   182,   962, -1202, -1474,  1468,
        573, -1325,   264,   383,  -829,  1458, -1602,  -130,
       -681,  1017,   732,   608, -1542,   411,  -205, -1571,
       1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
        516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
       -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
       -398,   961, -1508,  -725,   448, -1065,   677, -1275,
      -1103,   430,   555,   843, -1251,   871,  1550,   105,
        422,   587,   177,  -235,  -291,  -460,  1574,  1653,
       -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
      -1590,   644,  -872,   349,   418,   329,  -156,   -75,
        817,  1097,   603,   610,  1322, -1285, -1465,   384,
      -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
      -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
       -108,  -308,   996,   991,   958, -1460,  1522,  1628
    ]

    static func montgomery_reduce(_ a:Int32) -> Int16 {
        var t:Int16
        t = Int16(a*KY_QINV)
        t = Int16((a - Int32(t)*KY_PRIME) >> 16)
        return t
    }

    static func barrett_reduce(_ a:Int16) -> Int16 {
        var t:Int16
        let v = Int16(((1<<26) + KY_PRIME/2)/KY_PRIME)
        let vv = Int32(v)
        let aa = Int32(a)

        t  = Int16((vv*aa + 0x2000000) >> 26)
        t *= Int16(KY_PRIME)
        return Int16(a - t);
    }

    static func fqmul(_ a:Int16,_ b:Int16) -> Int16 {
        return montgomery_reduce(Int32(a)*Int32(b));
    }

    static func ntt(_ r:inout [Int16]) {
        var k = 1
        var len = 128
        while len>=2 {
            var start = 0
            while start<256 {
                let zeta = zetas[k]; k+=1
                for j in start..<start+len {
                    let t = fqmul(zeta, r[j + len])
                    r[j + len] = Int16(r[j] - t)
                    r[j] = Int16(r[j] + t)
                }
                start += 2*len
            }
            len >>= 1
        }
    }

    static func invntt(_ r:inout [Int16]) {
        let f:Int16 = 1441 // mont^2/128

        var k = 127
        var len = 2
        while len <= 128 {
            var start=0
            while start<256 {
                let zeta = zetas[k]; k-=1
                for j in start..<start+len {
                    let t = r[j]
                    r[j] = barrett_reduce(Int16(t + r[j + len]))
                    r[j + len] = Int16(r[j + len] - t)
                    r[j + len] = fqmul(zeta, r[j + len])
                }
                start += 2*len
            }
            len <<= 1
        }

        for j in 0..<256 {
            r[j] = fqmul(r[j], f)
        }
    }

    static func basemul(_ index:Int,_ r:inout [Int16], _ a:[Int16],_ b:[Int16],_ zeta:Int16) {
        let i=index
        let j=index+1
        r[i]  = fqmul(a[j], b[j])
        r[i]  = fqmul(r[i], zeta)
        r[i] += fqmul(a[i], b[i])
        r[j]  = fqmul(a[i], b[j])
        r[j] += fqmul(a[j], b[i])
    }

    static func poly_reduce(_ r:inout [Int16]) {
        for i in 0..<KY_DEGREE {
            r[i] = barrett_reduce(r[i])
        }
    }

    static func poly_ntt(_ r:inout [Int16]) {
        ntt(&r)
        poly_reduce(&r)
    }

    static func poly_invntt(_ r:inout [Int16]) {
        invntt(&r)
    }

// Note r must be distinct from a and b
    static func poly_mul(_ r:inout [Int16],_ a:[Int16],_ b:[Int16]) {
        for i in 0..<KY_DEGREE/4  {
            basemul(4*i,&r,a,b,zetas[64+i])
            basemul(4*i+2,&r,a,b,Int16(-zetas[64+i]))
        }
    }

    static func poly_tomont(_ r:inout [Int16]) {
        let f = Int32(KY_ONE)
        for i in 0..<KY_DEGREE {
            r[i] = montgomery_reduce(Int32(r[i])*f);
        }
    }

// End of public domain reference code use 

// copy polynomial
    static func poly_copy(_ p1:inout [Int16],_ p2:[Int16]) {
        for i in 0..<KY_DEGREE {
            p1[i] = p2[i]
        }
    }    

// zero polynomial
    static func poly_zero(_ p1:inout [Int16]) {
        for i in 0..<KY_DEGREE {
            p1[i] = 0
        }
    }

// add polynomials
    static func poly_add(_ p1:inout [Int16],_ p2:[Int16],_ p3:[Int16]) {
        for i in 0..<KY_DEGREE {
            p1[i] = Int16(p2[i] + p3[i])
        }
    }

// subtract polynomials
    static func poly_sub(_ p1:inout [Int16],_ p2:[Int16],_ p3:[Int16]) {
        for i in 0..<KY_DEGREE {
            p1[i] = Int16(p2[i] - p3[i])
        }
    }

// Generate A[i][j] from rho
    static func ExpandAij(_ rho:[UInt8],_ Aij:inout [Int16],_ i:Int,_ j:Int) {
        var sh=SHA3(SHA3.SHAKE128)
        var buff=[UInt8](repeating: 0,count: 640) // should be plenty (?)
        for m in 0..<32 {
            sh.process(rho[m])
        }

        sh.process(UInt8(j&0xff))
        sh.process(UInt8(i&0xff))
        sh.shake(&buff,640)
        var i=0; var j=0
        while j<KY_DEGREE {
            let t1=Int(buff[i])&0xff;  // convert byte to unsigned
            let t2=Int(buff[i+1])&0xff
            let t3=Int(buff[i+2])&0xff
            let d1=(t1+256*(t2&0x0f))
            let d2=(t2/16+16*t3)
            if d1<KY_PRIME {
                Aij[j]=Int16(d1); j+=1
            }
            if d2<KY_PRIME && j<KY_DEGREE {
                Aij[j]=Int16(d2); j+=1
            }
            i+=3
        }
    }

// get n-th bit from byte array
    static func getbit(_ b:[UInt8],_ n:Int) -> Int {
        let wd=n/8;
        let bt=n%8;
        return Int((b[wd]>>bt)&1)
    }

// centered binomial distribution
    static func CBD(_ bts:[UInt8],_ eta:Int,_ f:inout [Int16]) {
        for i in 0..<KY_DEGREE {
            var a=0
            var b=0
            for j in 0..<eta {
                a+=getbit(bts,2*i*eta+j)
                b+=getbit(bts,2*i*eta+eta+j)
            }
            f[i]=Int16(a-b)
        }
    }

// extract ab bits into word from dense byte stream
    static func nextword(_ ab:Int,_ t:[UInt8],_ position:inout [Int]) -> Int16 {
        var ptr=position[0] // index in array
        var bts=position[1] // bit index in byte
        var r=Int16(t[ptr]>>bts)
        let mask=Int16((1<<ab)-1)

        var i=0
        var gotbits=8-bts // bits left in current byte
        while gotbits<ab {
            i+=1
            let w=Int16(t[ptr+i])
            r|=w<<gotbits
            gotbits+=8
        }
        bts+=ab
        while bts>=8 {
            bts-=8
            ptr+=1
        }
        let w=Int16(r&mask)
        position[0]=ptr
        position[1]=bts
        return w
    }

// array t has ab active bits per word
// extract bytes from array of words
// if max!=0 then -max<=t[i]<=+max
    static func nextbyte16(_ ab:Int,_ t:[Int16],_ position:inout [Int]) -> UInt8 {
        var ptr=position[0] // index in array
        var bts=position[1] // bit index in word
        var left=ab-bts // number of bits left in this word
        var i=0
        let k=ptr%256

        var w=t[k]; w+=(w>>15)&Int16(KY_PRIME)
        var r=Int16(w>>bts)
        while left<8 {
            i+=1
            var w=t[k+i]; w+=(w>>15)&Int16(KY_PRIME)
            r|=w<<left
            left+=ab
        }

        bts+=8
        while bts>=ab {
            bts-=ab
            ptr+=1
        }
        position[0]=ptr
        position[1]=bts
        return UInt8(r&0xff)       
    }

// encode polynomial vector with coefficients of length L, into packed bytes
// pos indicates current position in vector t
// pptr indicates which vector in the matrix
    static func encode(_ t:[Int16],_ pos:inout [Int],_ L:Int,_ pack:inout [UInt8],_ pptr:Int) {
        let k=(KY_DEGREE*L)/8;  // compressed length
        for n in 0..<k {
            pack[n+pptr*k]=nextbyte16(L,t,&pos)
        }
    }

    static func chk_encode(_ t:[Int16],_ pos:inout [Int],_ L:Int,_ pack:[UInt8],_ pptr:Int) -> UInt8  {
        let k=(KY_DEGREE*L)/8
        var diff:UInt8=0
        for n in 0..<k {
            let m=nextbyte16(L,t,&pos)
            diff|=(m^pack[n+pptr*k])
        }    
        return diff
    }

// decode packed bytes into polynomial vector, with coefficients of length L
// pos indicates current position in byte array pack
    static func decode(_ pack:[UInt8],_ L:Int,_ t:inout [Int16],_ pos:inout [Int]) {
        for i in 0..<KY_DEGREE {
            t[i]=nextword(L,pack,&pos)
        }
    }

// compress polynomial coefficents in place, for polynomial vector of length len
    static func compress(_ t:inout [Int16],_ d:Int) {
        let twod=Int32(1<<d)
        for i in 0..<KY_DEGREE {
            t[i]+=(t[i]>>15)&Int16(KY_PRIME)
            t[i] = Int16(((twod*Int32(t[i])+KY_PRIME/2)/KY_PRIME)&(twod-1))
        }
    }

// decompress polynomial coefficents in place, for polynomial vector of length len
    static func decompress(_ t:inout [Int16],_ d:Int) {
        let twod1=Int32(1<<(d-1));
        for i in 0..<KY_DEGREE {
            t[i]=Int16((KY_PRIME*Int32(t[i])+twod1)>>d)
        }
    }

// input entropy, output key pair
    static func CPA_keypair(_ params:[Int],_ tau:[UInt8],_ sk:inout [UInt8],_ pk:inout [UInt8]) {
        var sh=SHA3(SHA3.HASH512)
        var rho=[UInt8](repeating: 0,count: 32)
        var sigma=[UInt8](repeating: 0,count: 33)
        var buff=[UInt8](repeating: 0,count: 256)

        let ck=params[0]
        var r = [Int16](repeating: 0,count: KY_DEGREE)
        var w = [Int16](repeating: 0,count: KY_DEGREE)
        var Aij = [Int16](repeating: 0,count: KY_DEGREE)

        var s = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)
        var e = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)
        var p = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)

        let eta1=params[1];
        let public_key_size=32+ck*(KY_DEGREE*3)/2;
        //let secret_cpa_key_size=ck*(KY_DEGREE*3)/2;
        for i in 0..<32 { 
            sh.process(tau[i])
        }
        let bf = sh.hash()
        for i in 0..<32 {
            rho[i]=bf[i]
            sigma[i]=bf[i+32]
        }
        sigma[32]=0   // N 
// create s
        for i in 0..<ck {
            sh=SHA3(SHA3.SHAKE256)
            for j in 0..<33 {
                sh.process(sigma[j])
            }
            sh.shake(&buff,64*eta1)
            CBD(buff,eta1,&s[i])
            sigma[32]+=1
        }
// create e
        for i in 0..<ck {
            sh=SHA3(SHA3.SHAKE256)
            for j in 0..<33 {
                sh.process(sigma[j])
            }
            sh.shake(&buff,64*eta1)
            CBD(buff,eta1,&e[i])
            sigma[32]+=1
        } 

        for k in 0..<ck {
            poly_ntt(&s[k])
            poly_ntt(&e[k])
        }  

        for i in 0..<ck {
            ExpandAij(rho,&Aij,i,0)
            poly_mul(&r,Aij,s[0])

            for j in 1..<ck {
                ExpandAij(rho,&Aij,i,j)
                poly_mul(&w,s[j],Aij)
                poly_add(&r,r,w)
            }
            poly_reduce(&r)
            poly_tomont(&r)
            poly_add(&p[i],r,e[i])
            poly_reduce(&p[i])
        }  

        var pos=[Int](repeating: 0,count: 2)
        pos[0]=0; pos[1]=0  
        for i in 0..<ck { 
            encode(s[i],&pos,12,&sk,i)
        } 
        pos[0]=0; pos[1]=0
        for i in 0..<ck {
            encode(p[i],&pos,12,&pk,i)
        }
        for i in 0..<32 {
            pk[public_key_size-32+i]=rho[i]
        }
    }
// input 64 random bytes, output secret and public keys
    static func CCA_keypair(_ params:[Int],_ randbytes64:[UInt8],_ sk:inout [UInt8],_ pk:inout [UInt8]) {
        var sh=SHA3(SHA3.SHAKE256)
        let sks=(params[0]*(KY_DEGREE*3)/2);
        let pks=(32+params[0]*(KY_DEGREE*3)/2);
        CPA_keypair(params,randbytes64,&sk,&pk);
        for i in 0..<pks {
            sk[sks+i]=pk[i]
        }

        for i in 0..<pks {
            sh.process(pk[i])
        }
        let h=sh.hash();
        for i in 0..<32 {
            sk[sks+pks+i]=h[i]
        }
        for i in 0..<32 {
            sk[sks+pks+32+i]=randbytes64[32+i]
        }
    }

    static func CPA_base_encrypt(_ params:[Int],_ coins:[UInt8],_ pk:[UInt8],_ ss:[UInt8],_ u:inout [[Int16]],_ v:inout [Int16]) {

        var sigma=[UInt8](repeating: 0,count: 33)
        var buff=[UInt8](repeating: 0,count: 256)
        var rho=[UInt8](repeating: 0,count: 32)

        let ck=params[0]
        var r = [Int16](repeating: 0,count: KY_DEGREE)
        var w = [Int16](repeating: 0,count: KY_DEGREE)
        var Aij = [Int16](repeating: 0,count: KY_DEGREE)

        var q = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)
        var p = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)

        let eta1=params[1]
        let eta2=params[2]
        let du=params[3]
        let dv=params[4]
        let public_key_size=32+ck*(KY_DEGREE*3)/2

        for i in 0..<32 {
            sigma[i]=coins[i]
        }
        sigma[32]=0

        for i in 0..<32 {
            rho[i]=pk[public_key_size-32+i]
        }

// create q
        for i in 0..<ck {
            var sh = SHA3(SHA3.SHAKE256);
            for j in 0..<33 {
                sh.process(sigma[j])
            }
            sh.shake(&buff,64*eta1)
            CBD(buff,eta1,&q[i])
            sigma[32]+=1
        }
// create e1
        for i in 0..<ck {
            var sh = SHA3(SHA3.SHAKE256)
            for j in 0..<33 {
                sh.process(sigma[j])
            }
            sh.shake(&buff,64*eta2)
            CBD(buff,eta1,&u[i])          // e1
            sigma[32]+=1
        }
        for i in 0..<ck {
            poly_ntt(&q[i])
        }
    
        for i in 0..<ck {
            ExpandAij(rho,&Aij,0,i)
            poly_mul(&r,Aij,q[0])
            for j in 1..<ck {
                ExpandAij(rho,&Aij,j,i)
                poly_mul(&w,q[j],Aij)
                poly_add(&r,r,w)
            }
            poly_reduce(&r)
            poly_invntt(&r)
            poly_add(&u[i],u[i],r)
            poly_reduce(&u[i])
        }

        var pos=[Int](repeating: 0,count: 2)
        pos[0]=0; pos[1]=0
        for i in 0..<ck {
            decode(pk,12,&p[i],&pos)
        }
    
        poly_mul(&v,p[0],q[0])

        for i in 1..<ck {
            poly_mul(&r,p[i],q[i])
            poly_add(&v,v,r)
        }    
        poly_invntt(&v)        

// create e2
        var sh = SHA3(SHA3.SHAKE256)
        for j in 0..<33 {
            sh.process(sigma[j])
        }
        sh.shake(&buff,64*eta2)
        CBD(buff,eta1,&w)  // e2

        poly_add(&v,v,w)
        pos[0]=0; pos[1]=0
        decode(ss,1,&r,&pos)
        decompress(&r,1)

        poly_add(&v,v,r)
        poly_reduce(&v)

        for i in 0..<ck {
            compress(&u[i],du)
        }
        compress(&v,dv)    
    }

// Given input of entropy, public key and shared secret is an input, outputs ciphertext
    static func CPA_encrypt(_ params:[Int],_ coins:[UInt8],_ pk:[UInt8],_ ss:[UInt8],_ ct:inout [UInt8]) {
        let ck=params[0]
        var v = [Int16](repeating: 0,count: KY_DEGREE)
        var u = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)

        let du=params[3]
        let dv=params[4]
        let ciphertext_size=(du*ck+dv)*KY_DEGREE/8

        CPA_base_encrypt(params,coins,pk,ss,&u,&v)
        var pos=[Int](repeating: 0,count: 2)
        pos[0]=0; pos[1]=0

        for i in 0..<ck {
            encode(u[i],&pos,du,&ct,i)
        }

        let len=(dv*KY_DEGREE/8); // last part of CT

        var last = [UInt8](repeating: 0,count: len)

        encode(v,&pos,dv,&last,0);
        for i in 0..<len {
            ct[ciphertext_size-len+i]=last[i]
        }
    }
// Re-encrypt and check that ct is OK (if so return is zero)
    static func CPA_check_encrypt(_ params:[Int],_ coins:[UInt8],_ pk:[UInt8],_ ss:[UInt8],_ ct:[UInt8]) -> UInt8 {
        let ck=params[0]
        var v = [Int16](repeating: 0,count: KY_DEGREE)
        var u = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)

        let du=params[3]
        let dv=params[4]

        let ciphertext_size=(du*ck+dv)*KY_DEGREE/8;
        
        CPA_base_encrypt(params,coins,pk,ss,&u,&v);
        var pos=[Int](repeating: 0,count: 2)
        pos[0]=0; pos[1]=0
        var d1:UInt8=0;
        for i in 0..<ck {
            d1|=chk_encode(u[i],&pos,du,ct,i)
        }

        let len=(dv*KY_DEGREE/8); // last part of CT
        var last = [UInt8](repeating: 0,count: len)

        for i in 0..<len {
            last[i]=ct[ciphertext_size-len+i]
        }

        let d2=chk_encode(v,&pos,dv,last,0)

        if (d1|d2)==0 {
            return UInt8(0)
        } else {
            return UInt8(0xff)
        }
    }

    static func CCA_encrypt(_ params:[Int],_ randbytes32:[UInt8],_ pk:[UInt8],_ ss:inout [UInt8],_ ct:inout [UInt8]) {
        var coins=[UInt8](repeating: 0,count: 32)

        let ck=params[0]
        let du=params[3]
        let dv=params[4]
        let shared_secret_size=params[5]
        let public_key_size=32+ck*(KY_DEGREE*3)/2
        let ciphertext_size=(du*ck+dv)*KY_DEGREE/8

        var sh = SHA3(SHA3.HASH256)
        for i in 0..<32 {
            sh.process(randbytes32[i])
        }
        let hm = sh.hash()

        sh = SHA3(SHA3.HASH256)
        for i in 0..<public_key_size {
            sh.process(pk[i])
        }
        var h = sh.hash()

        sh = SHA3(SHA3.HASH512)
        for i in 0..<32 {
            sh.process(hm[i])
        }
        for i in 0..<32 {
            sh.process(h[i])
        }
        let g = sh.hash()

        for i in 0..<32 {
            coins[i]=g[i+32]
        }

        CPA_encrypt(params,coins,pk,hm,&ct);

        sh = SHA3(SHA3.HASH256)
        for i in 0..<ciphertext_size {
            sh.process(ct[i])
        }
        h = sh.hash()

        sh = SHA3(SHA3.SHAKE256)
        for i in 0..<32 {
            sh.process(g[i])
        }
        for i in 0..<32 {
            sh.process(h[i])
        }

        sh.shake(&ss,shared_secret_size)
    }

	static func CPA_decrypt(_ params:[Int],_ SK:[UInt8],_ CT:[UInt8],_ SS:inout [UInt8]) {
		let ck=params[0];
        var w = [Int16](repeating: 0,count: KY_DEGREE)
        var v = [Int16](repeating: 0,count: KY_DEGREE)
        var r = [Int16](repeating: 0,count: KY_DEGREE)

        var u = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)
        var s = [[Int16]](repeating: [Int16](repeating: 0,count: KY_DEGREE), count: ck)

		let du=params[3]
		let dv=params[4]
		//let shared_secret_size=params[5]

        var pos=[Int](repeating: 0,count: 2)
        pos[0]=0; pos[1]=0

        for i in 0..<ck {
			decode(CT,du,&u[i],&pos)
        }
		decode(CT,dv,&v,&pos);
		for i in 0..<ck {
			decompress(&u[i],du)
        }
		decompress(&v,dv)
		pos[0]=0; pos[1]=0
        for i in 0..<ck {
			decode(SK,12,&s[i],&pos);
        }

		poly_ntt(&u[0]);
		poly_mul(&w,u[0],s[0]);
        for i in 1..<ck {
			poly_ntt(&u[i])
			poly_mul(&r,u[i],s[i])
			poly_add(&w,w,r)
		}
		poly_reduce(&w)
		poly_invntt(&w)
		poly_sub(&v,v,w)
		compress(&v,1)
		pos[0]=0; pos[1]=0
		encode(v,&pos,1,&SS,0); 
	}

	static func CCA_decrypt(_ params:[Int],_ SK:[UInt8],_ CT:[UInt8],_ SS:inout [UInt8]) {
		let ck=params[0]
		let du=params[3]
		let dv=params[4]
		let secret_cpa_key_size=ck*(KY_DEGREE*3)/2
		let public_key_size=32+ck*(KY_DEGREE*3)/2
		let shared_secret_size=params[5]
		let ciphertext_size=(du*ck+dv)*KY_DEGREE/8

        var h = [UInt8](repeating: 0,count: 32)
        var z = [UInt8](repeating: 0,count: 32)
        var m = [UInt8](repeating: 0,count: 32)
        var coins = [UInt8](repeating: 0,count: 32)
        
        var PK = [UInt8](repeating: 0,count: public_key_size)

		for i in 0..<public_key_size{
			PK[i]=SK[secret_cpa_key_size+i]
        }

		for i in 0..<32 {
			h[i]=SK[secret_cpa_key_size+public_key_size+i]
        }
		for i in 0..<32 {
			z[i]=SK[secret_cpa_key_size+public_key_size+32+i]
        }
		CPA_decrypt(params,SK,CT,&m)

		var sh = SHA3(SHA3.HASH512)
		for i in 0..<32 {
			sh.process(m[i])
        }
		for i in 0..<32 {
			sh.process(h[i])
        }
		var g = sh.hash()

		for i in 0..<32 {
			coins[i]=g[i+32]
        }

		let mask=CPA_check_encrypt(params,coins,PK,m,CT)
		for i in 0..<32 {
			g[i]^=(g[i]^z[i])&mask               // substitute z for Kb on failure
        }

		sh = SHA3(SHA3.HASH256)
		for i in 0..<ciphertext_size {
			sh.process(CT[i])
        }
		h=sh.hash()

		sh = SHA3(SHA3.SHAKE256)
		for i in 0..<32 {
			sh.process(g[i])
        }
		for i in 0..<32 {
			sh.process(h[i])
        }
		sh.shake(&SS,shared_secret_size)
	}

	public static func keypair512(_ r64:[UInt8],_ SK:inout [UInt8],_ PK:inout [UInt8]) {
		CCA_keypair(PARAMS_512,r64,&SK,&PK)
	}

	public static func encrypt512(_ r32:[UInt8],_ PK:[UInt8],_ SS:inout [UInt8],_ CT:inout [UInt8]) {
		CCA_encrypt(PARAMS_512,r32,PK,&SS,&CT)
	}

	public static func decrypt512(_ SK:[UInt8],_ CT:[UInt8],_ SS:inout [UInt8]) {
		CCA_decrypt(PARAMS_512,SK,CT,&SS)
	}

	public static func keypair768(_ r64:[UInt8],_ SK:inout [UInt8],_ PK:inout [UInt8]) {
		CCA_keypair(PARAMS_768,r64,&SK,&PK)
	}

	public static func encrypt768(_ r32:[UInt8],_ PK:[UInt8],_ SS:inout [UInt8],_ CT:inout [UInt8]) {
		CCA_encrypt(PARAMS_768,r32,PK,&SS,&CT)
	}

	public static func decrypt768(_ SK:[UInt8],_ CT:[UInt8],_ SS:inout [UInt8]) {
		CCA_decrypt(PARAMS_768,SK,CT,&SS)
	}

	public static func keypair1024(_ r64:[UInt8],_ SK:inout [UInt8],_ PK:inout [UInt8]) {
		CCA_keypair(PARAMS_1024,r64,&SK,&PK)
	}

	public static func encrypt1024(_ r32:[UInt8],_ PK:[UInt8],_ SS:inout [UInt8],_ CT:inout [UInt8]) {
		CCA_encrypt(PARAMS_1024,r32,PK,&SS,&CT)
	}

	public static func decrypt1024(_ SK:[UInt8],_ CT:[UInt8],_ SS:inout [UInt8]) {
		CCA_decrypt(PARAMS_1024,SK,CT,&SS)
	} 
}
