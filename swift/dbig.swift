
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
//  dbig.swift
//
//  Created by Michael Scott on 13/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

public struct DBIG{
    var w=[Chunk](repeating: 0,count: CONFIG_BIG.DNLEN)
    //var w=ContiguousArray<Chunk>(repeating: 0,count: CONFIG_BIG.DNLEN)
    init() {
        for i in 0 ..< CONFIG_BIG.DNLEN {w[i]=0}
    }
    init(_ x: Int)
    {
        w[0]=Chunk(x);
        for i in 1 ..< CONFIG_BIG.DNLEN {w[i]=0}
    }
    init(_ x: BIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i]=x.w[i]}
        w[CONFIG_BIG.NLEN-1]=x.w[CONFIG_BIG.NLEN-1]&CONFIG_BIG.BMASK
        w[CONFIG_BIG.NLEN]=x.w[CONFIG_BIG.NLEN-1]>>Chunk(CONFIG_BIG.BASEBITS)
        for i in CONFIG_BIG.NLEN+1 ..< CONFIG_BIG.DNLEN {w[i]=0}
    }
    init(_ x: DBIG)
    {
        for i in 0 ..< CONFIG_BIG.DNLEN {w[i]=x.w[i]}
    }
    init(_ x: [Chunk])
    {
        for i in 0 ..< CONFIG_BIG.DNLEN {w[i]=x[i]}
    }

    @discardableResult mutating func cmove(_ g: DBIG,_ d: Int)  -> Chunk
    {
        let b = Chunk(-d)
        var s=Chunk(0)
        let r=w[0] ^ g.w[1]
        var ra=r &+ r; ra >>= 1
        for i in 0 ..< CONFIG_BIG.DNLEN
        {
            var t=(w[i]^g.w[i])&b
            t^=r
            let e=w[i]^t; s ^= e
            w[i]=e^ra
        }
        return s
    }

/* Copy from another DBIG */
    mutating func copy(_ x: DBIG)
    {
        for i in 0 ..< CONFIG_BIG.DNLEN {w[i] = x.w[i]}
    }

    mutating func ucopy(_ x: BIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i] = 0}
        for i in CONFIG_BIG.NLEN ..< CONFIG_BIG.DNLEN {w[i] = x.w[i-CONFIG_BIG.NLEN]}
    }

    /* this+=x */
    mutating func add(_ x: DBIG)
    {
        for i in 0 ..< CONFIG_BIG.DNLEN
        {
            w[i]+=x.w[i]
        }
    }

    /* this-=x */
    mutating func sub(_ x: DBIG)
    {
        for i in 0 ..< CONFIG_BIG.DNLEN
        {
            w[i]-=x.w[i]
        }
    }

    /* this-=x */
    mutating func rsub(_ x: DBIG)
    {
        for i in 0 ..< CONFIG_BIG.DNLEN
        {
            w[i]=x.w[i]-w[i]
        }
    }
    /* general shift left */
    mutating func shl(_ k: UInt)
    {
        let n=k%CONFIG_BIG.BASEBITS
        let m=Int(k/CONFIG_BIG.BASEBITS)
        w[CONFIG_BIG.DNLEN-1]=((w[CONFIG_BIG.DNLEN-1-m]<<Chunk(n)))|(w[CONFIG_BIG.DNLEN-m-2]>>Chunk(CONFIG_BIG.BASEBITS-n))
        for i in (m+1...CONFIG_BIG.DNLEN-2).reversed()
        {
            w[i]=((w[i-m]<<Chunk(n))&CONFIG_BIG.BMASK)|(w[i-m-1]>>Chunk(CONFIG_BIG.BASEBITS-n))
        }
        w[m]=(w[0]<<Chunk(n))&CONFIG_BIG.BMASK
        for i in 0 ..< m {w[i]=0}
    }
    /* general shift right */
    mutating func shr(_ k: UInt)
    {
        let n=k%CONFIG_BIG.BASEBITS
        let m=Int(k/CONFIG_BIG.BASEBITS)
        for i in 0 ..< CONFIG_BIG.DNLEN-m-1
        {
            w[i]=(w[m+i]>>Chunk(n))|((w[m+i+1]<<Chunk(CONFIG_BIG.BASEBITS-n))&CONFIG_BIG.BMASK)
        }
        w[CONFIG_BIG.DNLEN - m - 1]=w[CONFIG_BIG.DNLEN-1]>>Chunk(n)
        for i in CONFIG_BIG.DNLEN - m ..< CONFIG_BIG.DNLEN {w[i]=0}
    }
    /* Compare a and b, return 0 if a==b, -1 if a<b, +1 if a>b. Inputs must be normalised */
    static func comp(_ a: DBIG,_ b: DBIG) -> Int
    {
        var gt = Chunk(0)
        var eq = Chunk(1)
        for i in (0...CONFIG_BIG.DNLEN-1).reversed()
        {
            gt |= ((b.w[i]-a.w[i]) >> CONFIG_BIG.BASEBITS) & eq
            eq &= ((b.w[i]^a.w[i])-1) >> CONFIG_BIG.BASEBITS
        }
        return Int(gt+gt+eq-1)
    }


/* convert from byte array to DBIG */
    static public func fromBytes(_ b: [UInt8]) -> DBIG
    {
        var m=DBIG();

        for i in 0 ..< b.count
        {
            m.shl(8)
            m.w[0]+=Chunk(b[i])&0xff
        }
        return m;
    }


    /* normalise BIG - force all digits < 2^BASEBITS */
    mutating func norm()
    {
        var carry:Chunk=0
        for i in 0 ..< CONFIG_BIG.DNLEN-1
        {
            let d=w[i]+carry
            w[i]=d&CONFIG_BIG.BMASK
            carry=d>>Chunk(CONFIG_BIG.BASEBITS)
        }
        w[CONFIG_BIG.DNLEN-1]+=carry
    }

    mutating public func ctmod(_ m: BIG,_ bd: UInt) -> BIG
    {
        var k=bd
        norm()
        var c=DBIG(m)
        var r=DBIG()

        c.shl(k)

        while true {
		    r.copy(self)
		    r.sub(c)
		    r.norm()
		    cmove(r,Int(1-((r.w[CONFIG_BIG.DNLEN-1]>>Chunk(CONFIG_BIG.CHUNK-1))&1)))
            if k==0 {break}
            k -= 1
            c.shr(1)
        }
        return BIG(self)
    }

    /* reduces this DBIG mod a BIG, and returns the BIG */
    mutating public func mod(_ m: BIG) -> BIG
    {
        var k=nbits()-m.nbits()
        if k<0 {k=0}
        return ctmod(m,UInt(k))
    }

    mutating func ctdiv(_ m: BIG,_ bd: UInt) -> BIG
    {
        var k=bd
        var c=DBIG(m)
        var a=BIG(0)
        var e=BIG(1)
        var r=BIG()
        var dr=DBIG(0)
        norm()

        c.shl(k)
        e.shl(k)

        while true {
		    dr.copy(self)
		    dr.sub(c)
		    dr.norm()
		    let d=Int(1-((dr.w[CONFIG_BIG.DNLEN-1]>>Chunk(CONFIG_BIG.CHUNK-1))&1))
		    cmove(dr,d)
		    r.copy(a)
		    r.add(e)
		    r.norm()
		    a.cmove(r,d)
            if k==0 {break}
            k -= 1
            c.shr(1)
            e.shr(1)
        }
        return a
    }
    /* return this/c */
    mutating func div(_ m:BIG) -> BIG
    {
        var k=nbits()-m.nbits()
        if k<0 {k=0}
        return ctdiv(m,UInt(k))
    }

    /* split DBIG at position n, return higher half, keep lower half */
    mutating func split(_ n: UInt) -> BIG
    {
        var t=BIG(0)
        let m=n%CONFIG_BIG.BASEBITS
        var carry=w[CONFIG_BIG.DNLEN-1]<<Chunk(CONFIG_BIG.BASEBITS-m)

        for i in (CONFIG_BIG.NLEN-1...CONFIG_BIG.DNLEN-2).reversed()
        {
            let nw=(w[i]>>Chunk(m))|carry;
            carry=(w[i]<<Chunk(CONFIG_BIG.BASEBITS-m))&CONFIG_BIG.BMASK;
            t.set(i-CONFIG_BIG.NLEN+1,nw);
        }
        w[CONFIG_BIG.NLEN-1]&=((1<<Chunk(m))-1);
        return t;
    }
    /* return number of bits */
    func nbits() -> Int
    {
        var k=(CONFIG_BIG.DNLEN-1)
        var t=DBIG(self)
        t.norm()
        while k>=0 && t.w[k]==0 {k -= 1}
        if k<0 {return 0}
        var bts=Int(CONFIG_BIG.BASEBITS)*k
        var c=t.w[k];
        while c != 0 {c/=2; bts+=1}
        return bts
    }
    /* Convert to Hex String */
    func toString() -> String
    {
        _ = DBIG()
        var s:String=""
        var len=nbits()
        if len%4 == 0 {len/=4}
        else {len/=4; len += 1}

        for i in (0...len-1).reversed()
        {
            var b = DBIG(self)
            b.shr(UInt(i*4))
            let n=String(b.w[0]&15,radix:16,uppercase:false)
            s+=n
        }

        return s
    }

}
