
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
//  dbig.swift
//
//  Created by Michael Scott on 13/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

    struct DBIG{
    var w=[Chunk](repeating: 0,count: CONFIG_BIG.DNLEN)
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

    mutating func cmove(_ g: DBIG,_ d: Int)
    {
        let b = Chunk(-d)
    
        for i in 0 ..< CONFIG_BIG.DNLEN
        {
            w[i]^=(w[i]^g.w[i])&b;
        }
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
        for i in (0...CONFIG_BIG.DNLEN-1).reversed()
        {
            if (a.w[i]==b.w[i]) {continue}
            if (a.w[i]>b.w[i]) {return 1}
            else  {return -1}
        }
        return 0;
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
    /* reduces this DBIG mod a BIG, and returns the BIG */
    mutating func mod(_ c: BIG) -> BIG
    {
        var k:Int=0
        norm()
        var m=DBIG(c)
        var r=DBIG(0)
    
        if DBIG.comp(self,m)<0 {return BIG(self)}
    
        repeat
        {
            m.shl(1)
            k += 1
        }
        while (DBIG.comp(self,m)>=0);
    
        while (k>0)
        {
            m.shr(1)

		r.copy(self)
		r.sub(m)
		r.norm()
		cmove(r,Int(1-((r.w[CONFIG_BIG.DNLEN-1]>>Chunk(CONFIG_BIG.CHUNK-1))&1)))

            k -= 1
        }
        return BIG(self)
    }
    /* return this/c */
    mutating func div(_ c:BIG) -> BIG
    {
        var k:Int=0
        var m=DBIG(c)
        var a=BIG(0)
        var e=BIG(1)
        var r=BIG(0)
        var dr=DBIG(0)

        norm()
    
        while (DBIG.comp(self,m)>=0)
        {
            e.fshl(1)
            m.shl(1)
            k += 1
        }
    
        while (k>0)
        {
            m.shr(1)
            e.shr(1)

		dr.copy(self)
		dr.sub(m)
		dr.norm()
		let d=Int(1-((dr.w[CONFIG_BIG.DNLEN-1]>>Chunk(CONFIG_BIG.CHUNK-1))&1))
		cmove(dr,d)
		r.copy(a)
		r.add(e)
		r.norm()
		a.cmove(r,d)

            k -= 1
        }
        return a
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
        var t=BIG(self)        
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
