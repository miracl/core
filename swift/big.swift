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
//  big.swift
//
//  Created by Michael Scott on 12/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//  BIG number class
//

import core

public struct BIG{
    var w=[Chunk](repeating: 0,count: CONFIG_BIG.NLEN)
    //var w: ContiguousArray<Chunk> = ContiguousArray(repeating: 0,count: CONFIG_BIG.NLEN)
    //var w: Array<Chunk> = Array(repeating: 0,count: CONFIG_BIG.NLEN)

/* Constructors */
    init() {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i]=0}
    }
    init(_ x: Int)
    {
        w[0]=Chunk(x);
        for i in 1 ..< CONFIG_BIG.NLEN {w[i]=0}
    }
    init(_ x: BIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i]=x.w[i]}
    }
    init(_ x: DBIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i]=x.w[i]}
    }
    public init(_ x: [Chunk])
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i]=x[i]}
    }
    func get(_ i: Int) -> Chunk
    {
        return w[i]
    }
    mutating func set(_ i: Int,_ x: Chunk)
    {
        w[i]=x
    }
    mutating func xortop(_ x: Chunk)
    {
        w[CONFIG_BIG.NLEN-1]^=x
    }
    mutating func ortop(_ x: Chunk)
    {
        w[CONFIG_BIG.NLEN-1]|=x
    }

#if D32

    static func muladd(_ a: Chunk,_ b: Chunk,_ c: Chunk,_ r: Chunk) -> (Chunk,Chunk)
    {
        let prod:DChunk = DChunk(a)*DChunk(b)+DChunk(c)+DChunk(r)
        let bot=Chunk(prod&DChunk(CONFIG_BIG.BMASK))
        let top=Chunk(prod>>DChunk(CONFIG_BIG.BASEBITS))
        return (top,bot)
    }
#endif
#if D64

    static func muladd(_ a: Chunk,_ b: Chunk,_ c: Chunk,_ r: Chunk) -> (Chunk,Chunk)
    {
        let (tp,bt)=a.multipliedFullWidth(by: b)
        var bot = Chunk(bt&UInt64(CONFIG_BIG.BMASK))
        var top = (tp << Chunk(64-CONFIG_BIG.BASEBITS)) | Chunk(bt >> CONFIG_BIG.BASEBITS)

        bot+=c; bot+=r
        let carry=bot>>Chunk(CONFIG_BIG.BASEBITS)
        bot &= CONFIG_BIG.BMASK
        top+=carry
        return (top,bot)
    }

#endif
    /* test for zero */
    func iszilch() -> Bool
    {
        var d=Chunk(0)
        for i in 0 ..< CONFIG_BIG.NLEN {
            d |= w[i]
        }
        return ((1 & ((d-1)>>CONFIG_BIG.BASEBITS)) != 0)
    }
/* set to zero */
    mutating func zero()
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i] = 0}
    }
/* set to one */
    mutating func one()
    {
        w[0]=1
        for i in 1 ..< CONFIG_BIG.NLEN {w[i]=0}
    }
/* Test for equal to one */
    func isunity() -> Bool
    {
        var d=Chunk(0)
        for i in 1 ..< CONFIG_BIG.NLEN {
            d |= w[i]
        }
        return ((1 & ((d-1)>>CONFIG_BIG.BASEBITS) & (((w[0]^1)-1)>>CONFIG_BIG.BASEBITS)) != 0)
    }
/* Copy from another BIG */
    mutating func copy(_ x: BIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i] = x.w[i]}
    }
    mutating func copy(_ x: DBIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i] = x.w[i]}
    }
/* Conditional swap of two bigs depending on d using XOR - no branches */
    @discardableResult mutating func cswap(_ b: inout BIG,_ d: Int) -> Chunk
    {
        let c = Chunk(-d)
        var s=Chunk(0)
        let r=w[0] ^ b.w[1]
        var ra=r &+ r; ra >>= 1
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            var t=c&(w[i]^b.w[i])
            t^=r
            var e=w[i]^t; s ^= e
            w[i]=e^ra
            e=b.w[i]^t; s ^= e
            b.w[i]=e^ra
        }
        return s
    }
    @discardableResult mutating func cmove(_ g: BIG,_ d: Int) -> Chunk
    {
        let b=Chunk(-d)
        var s=Chunk(0)
        let r=w[0] ^ g.w[1]
        var ra=r &+ r; ra >>= 1
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            var t=(w[i]^g.w[i])&b
            t^=r
            let e=w[i]^t; s ^= e
            w[i]=e^ra
        }
        return s
    }
/* normalise BIG - force all digits < 2^CONFIG_BIG.BASEBITS */
    @discardableResult mutating func norm() -> Chunk
    {
        var carry=Chunk(0);
        for i in 0 ..< CONFIG_BIG.NLEN-1
        {
            let d=w[i]+carry
            w[i]=d&CONFIG_BIG.BMASK
            carry=d>>Chunk(CONFIG_BIG.BASEBITS)
        }
        w[CONFIG_BIG.NLEN-1]+=carry
        return (w[CONFIG_BIG.NLEN-1]>>Chunk((8*CONFIG_BIG.MODBYTES)%CONFIG_BIG.BASEBITS))
    }
/* Shift right by less than a word */
    @discardableResult mutating func fshr(_ k: UInt) -> Int
    {
        let kw=Chunk(k);
        let r=w[0]&((Chunk(1)<<kw)-1)
        for i in 0 ..< CONFIG_BIG.NLEN-1
        {
            w[i]=(w[i]>>kw)|((w[i+1]<<(Chunk(CONFIG_BIG.BASEBITS)-kw))&CONFIG_BIG.BMASK)
        }
        w[CONFIG_BIG.NLEN-1]>>=kw;
        return Int(r)
    }
/* general shift right */
    mutating func shr(_ k: UInt)
    {
        let n=k%CONFIG_BIG.BASEBITS
        let m=Int(k/CONFIG_BIG.BASEBITS)
        for i in 0 ..< CONFIG_BIG.NLEN-m-1
        {
            w[i]=(w[m+i]>>Chunk(n))|((w[m+i+1]<<Chunk(CONFIG_BIG.BASEBITS-n))&CONFIG_BIG.BMASK)
        }
        w[CONFIG_BIG.NLEN - m - 1]=w[CONFIG_BIG.NLEN-1]>>Chunk(n)
        for i in CONFIG_BIG.NLEN - m ..< CONFIG_BIG.NLEN {w[i]=0}
    }
/* Shift right by less than a word */
    @discardableResult mutating func fshl(_ k: Int) -> Int
    {
        let kw=Chunk(k)
        w[CONFIG_BIG.NLEN-1]=((w[CONFIG_BIG.NLEN-1]<<kw))|(w[CONFIG_BIG.NLEN-2]>>(Chunk(CONFIG_BIG.BASEBITS)-kw))
        for i in (1...CONFIG_BIG.NLEN-2).reversed()
        {
            w[i]=((w[i]<<kw)&CONFIG_BIG.BMASK)|(w[i-1]>>(Chunk(CONFIG_BIG.BASEBITS)-kw))
        }
        w[0]=(w[0]<<kw)&CONFIG_BIG.BMASK
        return Int(w[CONFIG_BIG.NLEN-1]>>Chunk((8*CONFIG_BIG.MODBYTES)%CONFIG_BIG.BASEBITS))
    }
/* general shift left */
    mutating func shl(_ k: UInt)
    {
        let n=k%CONFIG_BIG.BASEBITS
        let m=Int(k/CONFIG_BIG.BASEBITS)
        w[CONFIG_BIG.NLEN-1]=(w[CONFIG_BIG.NLEN-1-m]<<Chunk(n))
        if CONFIG_BIG.NLEN>=m+2 {w[CONFIG_BIG.NLEN-1]|=(w[CONFIG_BIG.NLEN-m-2]>>Chunk(CONFIG_BIG.BASEBITS-n))}
        if m+1 <= CONFIG_BIG.NLEN-2 {
            for i in (m+1...CONFIG_BIG.NLEN-2).reversed()
            {
                w[i]=((w[i-m]<<Chunk(n))&CONFIG_BIG.BMASK)|(w[i-m-1]>>Chunk(CONFIG_BIG.BASEBITS-n))
            }
        }
        w[m]=(w[0]<<Chunk(n))&CONFIG_BIG.BMASK
        for i in 0 ..< m {w[i]=0}
    }
/* return number of bits */
    public func nbits() -> Int
    {
        var k=(CONFIG_BIG.NLEN-1)
        var t=BIG(self)
        t.norm()
        while k>=0 && t.w[k]==0 {k -= 1}
        if k<0 {return 0}
        var bts=Int(CONFIG_BIG.BASEBITS)*k
        var c=t.w[k];
        while c != 0 {c/=2; bts += 1}
        return bts
    }
    func toRawString() -> String
    {
        var s:String="("
        for i in 0 ..< CONFIG_BIG.NLEN-1
        {
            let n=String(w[i],radix:16,uppercase:false)
            s+=n
            s+=","

        }
        let n=String(w[CONFIG_BIG.NLEN-1],radix:16,uppercase:false)
        s+=n
        s+=")"
        return s
    }
/* Convert to Hex String */
    func toString() -> String
    {
        _ = BIG()
        var s:String=""
        var len=nbits()
        if len%4 == 0 {len/=4}
        else {len/=4; len += 1}
        if len<2*Int(CONFIG_BIG.MODBYTES) {len=2*Int(CONFIG_BIG.MODBYTES)}

        for i in (0...len-1).reversed()
        {
            var b = BIG(self)
            b.shr(UInt(i*4))
            let n=String(b.w[0]&15,radix:16,uppercase:false)
            s+=n
        }

        return s
    }
/* return this+x */
    func plus(_ x: BIG) -> BIG
    {
        var s=BIG()
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            s.w[i]=w[i]+x.w[i]
        }
        s.norm()
        return s
    }
/* this+=x */
    mutating func add(_ x: BIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            w[i]+=x.w[i]
        }
    }

/* this|=x */
    mutating func or(_ x: BIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            w[i]|=x.w[i]
        }
    }

/* this+=x, where x is int */
    mutating func inc(_ x: Int) {
        norm();
        w[0]+=Chunk(x);
    }
/* return this.x */
   	func minus(_ x: BIG) -> BIG
    {
        var d=BIG();
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            d.w[i]=w[i]-x.w[i];
        }
        return d;
    }
/* this-=x */
    mutating func sub(_ x: BIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            w[i]-=x.w[i]
        }
    }
/* reverse subtract this=x-this */
    mutating func rsub(_ x: BIG)
    {
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            w[i]=x.w[i]-w[i]
        }
    }
/* this-=x where x is int */
    mutating func dec(_ x: Int) {
        norm();
        w[0]-=Chunk(x);
    }
/* this*=x, where x is small int<NEXCESS */
    mutating func imul(_ c: Int)
    {
        for i in 0 ..< CONFIG_BIG.NLEN {w[i]*=Chunk(c)}
    }
/* convert this BIG to byte array */
    func tobytearray(_ b: inout [UInt8],_ n: Int)
    {
        //norm();
        var c=BIG(self);
        c.norm()
        for i in (0...Int(CONFIG_BIG.MODBYTES)-1).reversed()
        {
            b[i+n]=UInt8(c.w[0]&0xff);
            c.fshr(8);
        }
    }
/* convert from byte array to BIG */
    static func frombytearray(_ b: [UInt8],_ n: Int) -> BIG
    {
        var m=BIG();

        for i in 0 ..< Int(CONFIG_BIG.MODBYTES)
        {
            m.fshl(8)
            m.w[0]+=Chunk(b[i+n])&0xff
        }
        return m;
    }
    func toBytes(_ b: inout [UInt8])
    {
        tobytearray(&b,0)
    }
    static func fromBytes(_ b: [UInt8]) -> BIG
    {
        return frombytearray(b,0)
    }

/* this*=x, where x is >NEXCESS */
    @discardableResult mutating func pmul(_ c: Int) -> Chunk
    {
        var carry=Chunk(0);
        //norm();
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            let ak=w[i]
            let (top,bot)=BIG.muladd(ak,Chunk(c),carry,Chunk(0))
            carry=top; w[i]=bot;
        }
        return carry;
    }
/* this*=c and catch overflow in DBIG */
    mutating func pxmul(_ c: Int) -> DBIG
    {
        var m=DBIG()
        var carry=Chunk(0)
        for j in 0 ..< CONFIG_BIG.NLEN
        {
            let (top,bot)=BIG.muladd(w[j],Chunk(c),carry,m.w[j])
            carry=top; m.w[j]=bot
        }
        m.w[CONFIG_BIG.NLEN]=carry
        return m;
    }
/* divide by 3 */
    mutating func div3() -> Chunk
    {
        var carry=Chunk(0)
        norm();
        let base=Chunk(1<<CONFIG_BIG.BASEBITS);
        for i in (0...CONFIG_BIG.NLEN-1).reversed()
        {
            let ak=(carry*base+w[i]);
            w[i]=ak/3;
            carry=ak%3;
        }
        return carry;
    }
/* return a*b where result fits in a BIG */
    static func smul(_ a: BIG,_ b: BIG) -> BIG
    {
        var c=BIG()
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            var carry=Chunk(0)
            for j in 0 ..< CONFIG_BIG.NLEN
            {
                if (i+j<CONFIG_BIG.NLEN) {
                    let (top,bot)=BIG.muladd(a.w[i],b.w[j],carry,c.w[i+j])
                    carry=top; c.w[i+j]=bot
                }
            }
        }
        return c;
    }
/* Compare a and b, return 0 if a==b, -1 if a<b, +1 if a>b. Inputs must be normalised */
    static func comp(_ a: BIG,_ b: BIG) -> Int
    {
        var gt = Chunk(0)
        var eq = Chunk(1)
        for i in (0...CONFIG_BIG.NLEN-1).reversed()
        {
            gt |= ((b.w[i]-a.w[i]) >> CONFIG_BIG.BASEBITS) & eq
            eq &= ((b.w[i]^a.w[i])-1) >> CONFIG_BIG.BASEBITS
        }
        return Int(gt+gt+eq-1);
    }
/* set x = x mod 2^m */
    mutating func mod2m(_ m: UInt)
    {
        let wd=Int(m/CONFIG_BIG.BASEBITS)
        let bt=m%CONFIG_BIG.BASEBITS
        let msk=Chunk(1<<bt)-1;
        w[wd]&=msk;
        for i in wd+1 ..< CONFIG_BIG.NLEN {w[i]=0}
    }
/* Arazi and Qi inversion mod 256 */
    static func invmod256(_ a: Int) -> Int
    {
        var t1:Int=0
        var c=(a>>1)&1
        t1+=c
        t1&=1
        t1=2-t1
        t1<<=1
        var U=t1+1

    // i=2
        var b=a&3
        t1=U*b; t1>>=2
        c=(a>>2)&3
        var t2=(U*c)&3
        t1+=t2
        t1*=U; t1&=3
        t1=4-t1
        t1<<=2
        U+=t1

    // i=4
        b=a&15
        t1=U*b; t1>>=4
        c=(a>>4)&15
        t2=(U*c)&15
        t1+=t2
        t1*=U; t1&=15
        t1=16-t1
        t1<<=4
        U+=t1

        return U
    }
/* return parity */
    func parity() -> Int
    {
        return Int(w[0]%2)
    }

/* return n-th bit */
    func bit(_ n: UInt) -> Int
    {
        return Int( (w[Int(n/CONFIG_BIG.BASEBITS)]&(1<<Chunk(n%CONFIG_BIG.BASEBITS))) >>  Chunk(n%CONFIG_BIG.BASEBITS)            );
        //if ((w[Int(n/CONFIG_BIG.BASEBITS)]&(1<<Chunk(n%CONFIG_BIG.BASEBITS)))>0) {return 1;}
        //else {return 0;}
    }

    /* return n last bits */
    mutating func lastbits(_ n: UInt) -> Int
    {
        let msk=(Chunk(1)<<Chunk(n))-1;
        norm();
        return Int((w[0])&msk)
    }
/* a=1/a mod 2^256. This is very fast! */
    mutating func invmod2m()
    {
        var U=BIG()
        var b=BIG()
        var c=BIG()

        U.inc(BIG.invmod256(lastbits(8)))

        var i=UInt(8)
        while (i<CONFIG_BIG.BIGBITS)
        {
            U.norm();
            b.copy(self)
            b.mod2m(i)
            var t1=BIG.smul(U,b)
            t1.shr(i)
            c.copy(self)
            c.shr(i)
            c.mod2m(i)

            var t2=BIG.smul(U,c)
            t2.mod2m(i)
            t1.add(t2); t1.norm()
            b=BIG.smul(t1,U)
            t1.copy(b)
            t1.mod2m(i)

            t2.one(); t2.shl(i); t1.rsub(t2); t1.norm()
            t1.shl(i)
            U.add(t1)
            i<<=1
        }
        U.mod2m(CONFIG_BIG.BIGBITS)
        self.copy(U)
        self.norm()
    }

// Set self=self mod m in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m

    mutating func ctmod(_ m: BIG,_ bd: UInt)
    {
        var k=bd;
        var r=BIG()
        var c=BIG(m)
        norm()

        c.shl(k);
        while true {
            r.copy(self)
            r.sub(c)
            r.norm()
            cmove(r,Int(1-((r.w[CONFIG_BIG.NLEN-1]>>Chunk(CONFIG_BIG.CHUNK-1))&1)))
            if k==0 {break}
            c.fshr(1)
            k -= 1
        }
    }

    /* reduce this mod m */
    mutating func mod(_ m: BIG)
    {
        var k=nbits()-m.nbits()
        if k<0 {k=0}
        ctmod(m,UInt(k))
/*
        var k=0
        var m=BIG(m1)
        var r=BIG(0)
        norm()
        if (BIG.comp(self,m)<0) {return}
        repeat
        {
            m.fshl(1)
            k += 1
        } while (BIG.comp(self,m)>=0)

        while (k>0)
        {
            m.fshr(1)

            r.copy(self)
            r.sub(m)
            r.norm()
            cmove(r,Int(1-((r.w[CONFIG_BIG.NLEN-1]>>Chunk(CONFIG_BIG.CHUNK-1))&1)))

            k -= 1
        }  */
    }

    mutating func ctdiv(_ m: BIG,_ bd:UInt)
    {
        var k=bd;
        norm()
        var e=BIG(1)
        var a=BIG(self)
        var r=BIG()
        var c=BIG(m)
        zero()

        c.shl(k)
        e.shl(k)

        while true {
            r.copy(a)
            r.sub(c)
            r.norm()
            let d=Int(1-((r.w[CONFIG_BIG.NLEN-1]>>Chunk(CONFIG_BIG.CHUNK-1))&1))
            a.cmove(r,d)
            r.copy(self)
            r.add(e)
            r.norm()
            cmove(r,d)

            if k==0 {break}
            c.fshr(1)
            e.fshr(1)
            k -= 1;
        }
    }

    /* divide this by m */
    mutating func div(_ m: BIG)
    {
        var k=nbits()-m.nbits()
        if k<0 {k=0}
        ctdiv(m,UInt(k))
/*
        var k=0
        norm()
        var e=BIG(1)
        var b=BIG(self)
        var r=BIG(0)
        var m=BIG(m1)
        zero()

        while (BIG.comp(b,m)>=0)
        {
            e.fshl(1)
            m.fshl(1)
            k += 1
        }

        while (k>0)
        {
            m.fshr(1)
            e.fshr(1)

            r.copy(b)
            r.sub(m)
            r.norm()
            let d=Int(1-((r.w[CONFIG_BIG.NLEN-1]>>Chunk(CONFIG_BIG.CHUNK-1))&1))
            b.cmove(r,d)
            r.copy(self)
            r.add(e)
            r.norm()
            cmove(r,d)

            k -= 1
        } */
    }
    /* get 8*CONFIG_BIG.MODBYTES size random number */
    static public func random(_ rng: inout RAND) -> BIG
    {
        var m=BIG();
        var j:Int=0
        var r:UInt8=0
        /* generate random BIG */
        for _ in 0 ..< Int(8*CONFIG_BIG.MODBYTES)
        {
            if (j==0) {r=rng.getByte()}
            else {r>>=1}

            let b=Chunk(r&1)
            m.shl(1); m.w[0]+=b
            j += 1; j&=7
        }
        return m
    }

    /* Create random BIG in portable way, one bit at a time, less than q */
    static public func randomnum(_ q: BIG,_ rng: inout RAND) -> BIG
    {
        var d=DBIG(0);
        var j:Int=0
        var r:UInt8=0

        for _ in 0 ..< 2*q.nbits()
        {
            if (j==0) {r=rng.getByte()}
            else {r>>=1}

            let b=Chunk(r&1)
            d.shl(1); d.w[0]+=b
            j += 1; j&=7
        }
 
        let m=d.mod(q)
        return m
    }

    static public func randtrunc(_ q: BIG,_ trunc: Int,_ rng: inout RAND) -> BIG
    {
        var m=BIG.randomnum(q,&rng)
        if q.nbits() > trunc {
            m.mod2m(UInt(trunc))
        }
        return m
    }

    /* Jacobi Symbol (this/p). Returns 0, 1 or -1 */
    mutating func jacobi(_ p: BIG) -> Int
    {
        var n8:Int
        var k:Int
        var m:Int=0;
        var t=BIG()
        var x=BIG()
        var n=BIG()
        let zilch=BIG()
        let one=BIG(1)
        if (p.parity()==0 || BIG.comp(self,zilch)==0 || BIG.comp(p,one)<=0) {return 0}
        norm()
        x.copy(self)
        n.copy(p)
        x.mod(p)

        while (BIG.comp(n,one)>0)
        {
            if (BIG.comp(x,zilch)==0) {return 0}
            n8=n.lastbits(3)
            k=0
            while (x.parity()==0)
            {
				k += 1
				x.shr(1)
            }
            if (k%2==1) {m+=((n8*n8-1)/8)}
            let w=Int(x.lastbits(2)-1)
            m+=(n8-1)*w/4
            t.copy(n)
            t.mod(x)
            n.copy(x)
            x.copy(t)
            m%=2

        }
        if (m==0) {return 1}
        else {return -1}
    }
    /* this=1/this mod p. Binary method */
    mutating func invmodp(_ p: BIG)
    {
        mod(p)
	    if (iszilch()) {return}
        var u=BIG(self)
        var v=BIG(p)
        var x1=BIG(1)
        var x2=BIG()
        var t=BIG()
        let one=BIG(1)

        while ((BIG.comp(u,one) != 0 ) && (BIG.comp(v,one) != 0 ))
        {
            while (u.parity()==0)
            {
                u.fshr(1)
                t.copy(x1)
                t.add(p)
                x1.cmove(t,x1.parity())
                x1.norm()
                x1.fshr(1)
            }
            while (v.parity()==0)
            {
                v.fshr(1)
                t.copy(x2)
                t.add(p)
                x2.cmove(t,x2.parity())
                x2.norm()
                x2.fshr(1)
            }
            if (BIG.comp(u,v)>=0)
            {
                u.sub(v)
                u.norm()
                t.copy(x1)
                t.add(p)
                x1.cmove(t,(BIG.comp(x1,x2)>>1)&1)
                x1.sub(x2)
                x1.norm()
            }
            else
            {
                v.sub(u)
                v.norm()
                t.copy(x2)
                t.add(p)
                x2.cmove(t,(BIG.comp(x2,x1)>>1)&1)
                x2.sub(x1)
                x2.norm()
            }
        }
        copy(x1)
        cmove(x2,BIG.comp(u,one)&1)
    }
    /* return a*b as DBIG */
#if D32
    static func mul(_ a: BIG,_ b:BIG) -> DBIG
    {
        var t:DChunk
        var co:DChunk
        var c=DBIG()
        let RM:DChunk=DChunk(CONFIG_BIG.BMASK);
        let RB:DChunk=DChunk(CONFIG_BIG.BASEBITS)

        var d=[DChunk](repeating: 0,count: CONFIG_BIG.NLEN)
        var s:DChunk
        for i in 0 ..< CONFIG_BIG.NLEN
        {
            d[i]=DChunk(a.w[i])*DChunk(b.w[i]);
        }
        s=d[0]
        t=s; c.w[0]=Chunk(t&RM); co=t>>RB
        for k in 1 ..< CONFIG_BIG.NLEN
        {
            s+=d[k]; t=co+s;
            for i in 1+k/2...k
                {t+=DChunk(a.w[i]-a.w[k-i])*DChunk(b.w[k-i]-b.w[i])}
            c.w[k]=Chunk(t&RM); co=t>>RB
        }
        for k in CONFIG_BIG.NLEN ..< 2*CONFIG_BIG.NLEN-1
        {
            s-=d[k-CONFIG_BIG.NLEN]; t=co+s;

            var i=1+k/2
            while i<CONFIG_BIG.NLEN
            {
                t+=DChunk(a.w[i]-a.w[k-i])*DChunk(b.w[k-i]-b.w[i])
                i+=1
            }

            c.w[k]=Chunk(t&RM); co=t>>RB
        }
        c.w[2*CONFIG_BIG.NLEN-1]=Chunk(co);

        return c
    }

    /* return a^2 as DBIG */
    static func sqr(_ a: BIG) -> DBIG
    {
        var t:DChunk
        var co:DChunk
        var c=DBIG()
        let RM:DChunk=DChunk(CONFIG_BIG.BMASK);
        let RB:DChunk=DChunk(CONFIG_BIG.BASEBITS)

        t=DChunk(a.w[0])*DChunk(a.w[0])
        c.w[0]=Chunk(t&RM); co=t>>RB
        var j:Int
        j=1
        while j<CONFIG_BIG.NLEN-1
        {
            t=DChunk(a.w[j])*DChunk(a.w[0]); for  i in 1 ..< (j+1)/2 {t+=DChunk(a.w[j-i])*DChunk(a.w[i])}; t+=t;  t+=co
            c.w[j]=Chunk(t&RM); co=t>>RB
            j+=1
            t=DChunk(a.w[j])*DChunk(a.w[0]); for  i in 1 ..< (j+1)/2 {t+=DChunk(a.w[j-i])*DChunk(a.w[i])}; t+=t; t+=co; t+=DChunk(a.w[j/2])*DChunk(a.w[j/2])
            c.w[j]=Chunk(t&RM); co=t>>RB
            j+=1
        }

        j=CONFIG_BIG.NLEN-1+(CONFIG_BIG.NLEN%2)
        while j<CONFIG_BIG.DNLEN-3
        {
            t=DChunk(a.w[CONFIG_BIG.NLEN-1])*DChunk(a.w[j-CONFIG_BIG.NLEN+1]); for i in j-CONFIG_BIG.NLEN+2 ..< (j+1)/2 {t+=DChunk(a.w[j-i])*DChunk(a.w[i])}; t+=t; t+=co
            c.w[j]=Chunk(t&RM); co=t>>RB
            j+=1;
            t=DChunk(a.w[CONFIG_BIG.NLEN-1])*DChunk(a.w[j-CONFIG_BIG.NLEN+1]); for i in j-CONFIG_BIG.NLEN+2 ..< (j+1)/2 {t+=DChunk(a.w[j-i])*DChunk(a.w[i])}; t+=t; t+=co; t+=DChunk(a.w[j/2])*DChunk(a.w[j/2])
            c.w[j]=Chunk(t&RM); co=t>>RB
            j+=1;
        }

        t=DChunk(a.w[CONFIG_BIG.NLEN-2])*DChunk(a.w[CONFIG_BIG.NLEN-1])
        t+=t; t+=co;
        c.w[CONFIG_BIG.DNLEN-3]=Chunk(t&RM); co=t>>RB

        t=DChunk(a.w[CONFIG_BIG.NLEN-1])*DChunk(a.w[CONFIG_BIG.NLEN-1])+co
        c.w[CONFIG_BIG.DNLEN-2]=Chunk(t&RM); co=t>>RB
        c.w[CONFIG_BIG.DNLEN-1]=Chunk(co)

        return c
    }
    static func monty(_ md:BIG,_ mc:Chunk,_ d: inout DBIG) -> BIG
    {
        let RM:DChunk=DChunk(CONFIG_BIG.BMASK)
        let RB:DChunk=DChunk(CONFIG_BIG.BASEBITS)


        var t:DChunk
        var s:DChunk
        var c:DChunk
        var dd=[DChunk](repeating: 0,count: CONFIG_BIG.NLEN)
        var v=[Chunk](repeating: 0,count: CONFIG_BIG.NLEN)
        var b=BIG(0)

        t=DChunk(d.w[0]); v[0]=(Chunk(t&RM)&*mc)&CONFIG_BIG.BMASK; t+=DChunk(v[0])*DChunk(md.w[0]); c=DChunk(d.w[1])+(t>>RB); s=0
        for k in 1 ..< CONFIG_BIG.NLEN
        {
            t=c+s+DChunk(v[0])*DChunk(md.w[k])
            var i=1+k/2
            while i<k
            {
                t+=DChunk(v[k-i]-v[i])*DChunk(md.w[i]-md.w[k-i])
                i+=1
            }
            v[k]=(Chunk(t&RM)&*mc)&CONFIG_BIG.BMASK; t+=DChunk(v[k])*DChunk(md.w[0]); c=DChunk(d.w[k+1])+(t>>RB)
            dd[k]=DChunk(v[k])*DChunk(md.w[k]); s+=dd[k]
        }
        for k in CONFIG_BIG.NLEN ..< 2*CONFIG_BIG.NLEN-1
        {
            t=c+s
            var i=1+k/2
            while i<CONFIG_BIG.NLEN
            {
                t+=DChunk(v[k-i]-v[i])*DChunk(md.w[i]-md.w[k-i])
                i+=1
            }
            b.w[k-CONFIG_BIG.NLEN]=Chunk(t&RM); c=DChunk(d.w[k+1])+(t>>RB); s-=dd[k-CONFIG_BIG.NLEN+1]
        }
        b.w[CONFIG_BIG.NLEN-1]=Chunk(c&RM)
        return b;
    }
#endif
#if D64
    static func mul(_ a: BIG,_ b:BIG) -> DBIG
    {
        var c=DBIG()
        var carry:Chunk

        //let aw = UnsafePointer(a.w)
        //let bw = UnsafePointer(b.w)


        for i in 0 ..< CONFIG_BIG.NLEN {
            carry=0
            for j in 0..<CONFIG_BIG.NLEN {
                let (top,bot)=BIG.muladd(a.w[i],b.w[j],carry,c.w[i+j])
                carry=top; c.w[i+j]=bot
            }
            c.w[CONFIG_BIG.NLEN+i]=carry
        }
        return c
    }
    static func sqr(_ a: BIG) -> DBIG
    {
        var c=DBIG()
        var carry:Chunk

        //let aw = UnsafePointer(a.w)

        for i in 0 ..< CONFIG_BIG.NLEN {
            carry=0
            for j in i+1 ..< CONFIG_BIG.NLEN {
                let (top,bot)=BIG.muladd(2*a.w[i],a.w[j],carry,c.w[i+j])
                carry=top; c.w[i+j]=bot
            }
            c.w[CONFIG_BIG.NLEN+i]=carry
        }
        for i in 0 ..< CONFIG_BIG.NLEN {
            let (top,bot)=BIG.muladd(a.w[i],a.w[i],Chunk(0),c.w[2*i])
            c.w[2*i]=bot
            c.w[2*i+1]+=top
        }
        c.norm()
        return c
    }
    static func monty(_ md:BIG,_ mc:Chunk,_ d: inout DBIG) -> BIG
    {
        var b=BIG()
        var carry:Chunk
        var m:Chunk
        for i in 0 ..< CONFIG_BIG.NLEN {
            if mc == -1 {
                m=(-d.w[i])&CONFIG_BIG.BMASK
            } else {
                if mc == 1 {
                    m=d.w[i]
                } else {
                    m=(mc&*d.w[i])&CONFIG_BIG.BMASK;
                }
            }
            carry=0
            for j in 0 ..< CONFIG_BIG.NLEN {
                let (top,bot)=BIG.muladd(m,md.w[j],carry,d.w[i+j])
                carry=top; d.w[i+j]=bot
            }
            d.w[CONFIG_BIG.NLEN+i]+=carry
        }
        for i in 0 ..< CONFIG_BIG.NLEN {
            b.w[i]=d.w[CONFIG_BIG.NLEN+i]
        }
        b.norm();
        return b
    }
#endif

/* Optimized combined shift, subtract and norm */
    static func ssn(_ r: inout BIG,_ a :BIG,_ m: inout BIG) -> Int
    {
        let n=CONFIG_BIG.NLEN-1
        m.w[0]=(m.w[0]>>1)|((m.w[1]<<Chunk(CONFIG_BIG.BASEBITS-1))&CONFIG_BIG.BMASK)
        r.w[0]=a.w[0]-m.w[0]
        var carry=r.w[0]>>Chunk(CONFIG_BIG.BASEBITS)
        r.w[0] &= CONFIG_BIG.BMASK
        for i in 1 ..< n {
            m.w[i]=(m.w[i]>>1)|((m.w[i+1]<<Chunk(CONFIG_BIG.BASEBITS-1))&CONFIG_BIG.BMASK)
            r.w[i]=a.w[i]-m.w[i]+carry
            carry=r.w[i]>>Chunk(CONFIG_BIG.BASEBITS)
            r.w[i] &= CONFIG_BIG.BMASK
        }
	   m.w[n]>>=1
	   r.w[n]=a.w[n]-m.w[n]+carry
	   return Int((r.w[n]>>Chunk(CONFIG_BIG.CHUNK-1))&Chunk(1))
    }

    /* return a*b mod m */
    static func modmul(_ a1: BIG,_ b1 :BIG,_ m: BIG) -> BIG
    {
        var a=BIG(a1); var b=BIG(b1)
        a.mod(m)
        b.mod(m)
        var d=mul(a,b)
        return d.ctmod(m,UInt(m.nbits()))
    }

    /* return a^2 mod m */
    static func modsqr(_ a1: BIG,_ m: BIG) -> BIG
    {
        var a=BIG(a1)
        a.mod(m)
        var d=sqr(a)
        return d.ctmod(m,UInt(m.nbits()))
    }

    /* return -a mod m */
    static func modneg(_ a1: BIG,_ m: BIG) -> BIG
    {
        var a=BIG(a1)
        a.mod(m)
	    a.rsub(m)
	    a.norm()
        return a
    }

    /* return a+b mod m */
    static func modadd(_ a1: BIG,_ b1 :BIG,_ m: BIG) -> BIG
    {
        var a=BIG(a1); var b=BIG(b1)
        a.mod(m)
        b.mod(m)
        a.add(b); a.norm()
        a.ctmod(m,1)
        return a
    }

    /* return this^e mod m */
    mutating func powmod(_ e1: BIG,_ m: BIG) -> BIG
    {
        norm();
        var e=BIG(e1)
        e.norm();
        var a=BIG(1)
        var z=BIG(e)
        var s=BIG(self)
        while (true)
        {
            let bt=z.parity();
            z.fshr(1)
            if bt==1 {a=BIG.modmul(a,s,m)}
            if (z.iszilch()) {break}
            s=BIG.modsqr(s,m)
        }
        return a
    }
}
