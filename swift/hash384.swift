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
//  hash384.swift - Implementation of SHA-384
//
//  Created by Michael Scott on 29/03/2016.
//  Copyright © 2016 Michael Scott. All rights reserved.
//

import Foundation

public struct HASH384{

    private var length=[UInt64](repeating: 0,count: 2)
    private var h=[UInt64](repeating: 0,count: 8)
    private var w=[UInt64](repeating: 0,count: 80)
    static let H0:UInt64=0xcbbb9d5dc1059ed8
    static let H1:UInt64=0x629a292a367cd507
    static let H2:UInt64=0x9159015a3070dd17
    static let H3:UInt64=0x152fecd8f70e5939
    static let H4:UInt64=0x67332667ffc00b31
    static let H5:UInt64=0x8eb44a8768581511
    static let H6:UInt64=0xdb0c2e0d64f98fa7
    static let H7:UInt64=0x47b5481dbefa4fa4
    
    static let len:Int=48
    
    static let K:[UInt64]=[
        0x428a2f98d728ae22,0x7137449123ef65cd,0xb5c0fbcfec4d3b2f,0xe9b5dba58189dbbc,
        0x3956c25bf348b538,0x59f111f1b605d019,0x923f82a4af194f9b,0xab1c5ed5da6d8118,
        0xd807aa98a3030242,0x12835b0145706fbe,0x243185be4ee4b28c,0x550c7dc3d5ffb4e2,
        0x72be5d74f27b896f,0x80deb1fe3b1696b1,0x9bdc06a725c71235,0xc19bf174cf692694,
        0xe49b69c19ef14ad2,0xefbe4786384f25e3,0x0fc19dc68b8cd5b5,0x240ca1cc77ac9c65,
        0x2de92c6f592b0275,0x4a7484aa6ea6e483,0x5cb0a9dcbd41fbd4,0x76f988da831153b5,
        0x983e5152ee66dfab,0xa831c66d2db43210,0xb00327c898fb213f,0xbf597fc7beef0ee4,
        0xc6e00bf33da88fc2,0xd5a79147930aa725,0x06ca6351e003826f,0x142929670a0e6e70,
        0x27b70a8546d22ffc,0x2e1b21385c26c926,0x4d2c6dfc5ac42aed,0x53380d139d95b3df,
        0x650a73548baf63de,0x766a0abb3c77b2a8,0x81c2c92e47edaee6,0x92722c851482353b,
        0xa2bfe8a14cf10364,0xa81a664bbc423001,0xc24b8b70d0f89791,0xc76c51a30654be30,
        0xd192e819d6ef5218,0xd69906245565a910,0xf40e35855771202a,0x106aa07032bbd1b8,
        0x19a4c116b8d2d0c8,0x1e376c085141ab53,0x2748774cdf8eeb99,0x34b0bcb5e19b48a8,
        0x391c0cb3c5c95a63,0x4ed8aa4ae3418acb,0x5b9cca4f7763e373,0x682e6ff3d6b2b8a3,
        0x748f82ee5defb2fc,0x78a5636f43172f60,0x84c87814a1f0ab72,0x8cc702081a6439ec,
        0x90befffa23631e28,0xa4506cebde82bde9,0xbef9a3f7b2c67915,0xc67178f2e372532b,
        0xca273eceea26619c,0xd186b8c721c0c207,0xeada7dd6cde0eb1e,0xf57d4f7fee6ed178,
        0x06f067aa72176fba,0x0a637dc5a2c898a6,0x113f9804bef90dae,0x1b710b35131c471b,
        0x28db77f523047d84,0x32caab7b40c72493,0x3c9ebe0a15c9bebc,0x431d67c49c100d4c,
        0x4cc5d4becb3e42b6,0x597f299cfc657e2a,0x5fcb6fab3ad6faec,0x6c44198c4a475817]

    private static func S(_ n: UInt32,_ x: UInt64) -> UInt64
    {
        return ((x>>UInt64(n))|(x<<(64-UInt64(n))))
    }
    
    private static func R(_ n: UInt32,_ x: UInt64) -> UInt64
    {
        return (x>>UInt64(n))
    }
    
    private static func Ch(_ x: UInt64,_ y: UInt64,_ z:UInt64) -> UInt64
    {
        return ((x&y)^(~(x)&z))
    }
    
    private static func Maj(_ x: UInt64,_ y: UInt64,_ z:UInt64) -> UInt64
    {
        return ((x&y)^(x&z)^(y&z))
    }
    
    private static func Sig0(_ x: UInt64) -> UInt64
    {
        return (S(28,x)^S(34,x)^S(39,x))
    }
    
    private static func Sig1(_ x: UInt64) -> UInt64
    {
        return (S(14,x)^S(18,x)^S(41,x))
    }
    
    private static func theta0(_ x: UInt64) -> UInt64
    {
        return (S(1,x)^S(8,x)^R(7,x))
    }
    
    private static func theta1(_ x: UInt64) -> UInt64
    {
        return (S(19,x)^S(61,x)^R(6,x))
    }
    
    private mutating func transform()
    { /* basic transformation step */
        var a,b,c,d,e,f,g,hh,t1,t2 :UInt64
        for j in 16 ..< 80
        {
            w[j]=HASH384.theta1(w[j-2])&+w[j-7]&+HASH384.theta0(w[j-15])&+w[j-16]
        }
        a=h[0]; b=h[1]; c=h[2]; d=h[3]
        e=h[4]; f=h[5]; g=h[6]; hh=h[7]
        
        for j in 0 ..< 80
        { /* 64 times - mush it up */
            t1=hh&+HASH384.Sig1(e)&+HASH384.Ch(e,f,g)&+HASH384.K[j]&+w[j]
            t2=HASH384.Sig0(a)&+HASH384.Maj(a,b,c)
            hh=g; g=f; f=e;
            e=d&+t1;
            d=c;
            c=b;
            b=a;
            a=t1&+t2;
        }
        h[0]=h[0]&+a; h[1]=h[1]&+b; h[2]=h[2]&+c; h[3]=h[3]&+d
        h[4]=h[4]&+e; h[5]=h[5]&+f; h[6]=h[6]&+g; h[7]=h[7]&+hh;
    }
    
    /* Re-Initialise Hash function */
    mutating func init_it()
    { /* initialise */
        for i in 0 ..< 80 {w[i]=0}
        length[0]=0; length[1]=0
        h[0]=HASH384.H0;
        h[1]=HASH384.H1;
        h[2]=HASH384.H2;
        h[3]=HASH384.H3;
        h[4]=HASH384.H4;
        h[5]=HASH384.H5;
        h[6]=HASH384.H6;
        h[7]=HASH384.H7;
    }
    
    public init()
    {
        init_it()
    }
    
    /* process a single byte */
    public mutating func process(_ byt: UInt8)
    { /* process the next message byte */
        let cnt=Int((length[0]/64)%16)
        w[cnt]<<=8;
        w[cnt]|=(UInt64(byt)&0xFF);
        length[0]+=8;
        if (length[0]==0) { length[1] += 1; length[0]=0 }
        if ((length[0]%1024)==0) {transform()}
    }
    
    /* process an array of bytes */
    public mutating func process_array(_ b: [UInt8])
    {
        for i in 0 ..< b.count {process((b[i]))}
    }
    
    /* process a 32-bit integer */
    public mutating func process_num(_ n:Int32)
    {
        process(UInt8((n>>24)&0xff))
        process(UInt8((n>>16)&0xff))
        process(UInt8((n>>8)&0xff))
        process(UInt8(n&0xff))
    }
    
    /* Generate 48-byte Hash */
    public mutating func hash() -> [UInt8]
    { /* pad message and finish - supply digest */
        var digest=[UInt8](repeating: 0,count: 48)
        
        let len0=length[0]
        let len1=length[1]
        process(0x80);
        while ((length[0]%1024) != 896) {process(0)}
        w[14]=len1
        w[15]=len0;
        transform()
        for i in 0 ..< HASH384.len
        { /* convert to bytes */
            digest[i]=UInt8((h[i/8]>>(8*(7-UInt64(i)%8))) & 0xff);
        }
        init_it();
        return digest;
    }
    
}
