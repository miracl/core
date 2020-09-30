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
//  fp.swift
//
//  Created by Michael Scott on 20/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//  Small Finite Field arithmetic
//  CORE mod p functions
//

import core

public struct FP {
    var x:BIG
    var xes:Int32
    static let p=BIG(ROM.Modulus)
    static let r2modp=BIG(ROM.R2modp)

/* convert to Montgomery n-residue form */
    mutating func nres()
    {
        if CONFIG_FIELD.MODTYPE != CONFIG_FIELD.PSEUDO_MERSENNE && CONFIG_FIELD.MODTYPE != CONFIG_FIELD.GENERALISED_MERSENNE
        {
            var d=BIG.mul(x,FP.r2modp);
            x.copy(FP.mod(&d))
            xes=2
        } else {
            let md=BIG(ROM.Modulus)
            x.mod(md)
            xes=1
        }
    }
/* convert back to regular form */
    func redc() -> BIG
    {
        if CONFIG_FIELD.MODTYPE != CONFIG_FIELD.PSEUDO_MERSENNE && CONFIG_FIELD.MODTYPE != CONFIG_FIELD.GENERALISED_MERSENNE
        {
            var d=DBIG(x)
            return FP.mod(&d)
        }
        else
        {
            let r=BIG(x)
            return r;
        }
    }

    /* reduce a DBIG to a BIG using the appropriate form of the modulus */
    static func mod(_ d: inout DBIG) -> BIG
    {

        if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.PSEUDO_MERSENNE
        {
            var t=d.split(CONFIG_FIELD.MODBITS)
            let b=BIG(d)
            let v=t.pmul(Int(ROM.MConst))

            t.add(b)
            t.norm()


            let tw=t.w[CONFIG_BIG.NLEN-1]
            t.w[CONFIG_BIG.NLEN-1] &= CONFIG_FIELD.TMASK
            t.inc(Int(ROM.MConst*((tw>>Chunk(CONFIG_FIELD.TBITS))+(v<<Chunk(CONFIG_BIG.BASEBITS-CONFIG_FIELD.TBITS)))))

            t.norm()
            return t

        }
        if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.MONTGOMERY_FRIENDLY
        {
            for i in 0 ..< CONFIG_BIG.NLEN {
                let (top,bot)=BIG.muladd(d.w[i],ROM.MConst-1,d.w[i],d.w[CONFIG_BIG.NLEN+i-1])
                d.w[CONFIG_BIG.NLEN+i]+=top; d.w[CONFIG_BIG.NLEN+i-1]=bot
            }

            var b=BIG(0);

            for i in 0 ..< CONFIG_BIG.NLEN
            {
                b.w[i]=d.w[CONFIG_BIG.NLEN+i]
            }
            b.norm()
            return b;
        }
        if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.GENERALISED_MERSENNE
        { // GoldiLocks Only
            let t=d.split(CONFIG_FIELD.MODBITS)
            let RM2=CONFIG_FIELD.MODBITS/2
            var b=BIG(d)
            b.add(t)
            var dd=DBIG(t)
            dd.shl(RM2)

            var tt=dd.split(CONFIG_FIELD.MODBITS)
            let lo=BIG(dd)
            b.add(tt)
            b.add(lo)
            b.norm()
            tt.shl(RM2)
            b.add(tt)

            let carry=b.w[CONFIG_BIG.NLEN-1]>>Chunk(CONFIG_FIELD.TBITS)
            b.w[CONFIG_BIG.NLEN-1]&=CONFIG_FIELD.TMASK
            b.w[0]+=carry

            let ix=Int(224/CONFIG_BIG.BASEBITS)
            b.w[ix]+=carry<<Chunk(224%CONFIG_BIG.BASEBITS)
            b.norm()
            return b;
        }
        if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.NOT_SPECIAL
        {
            let md=BIG(ROM.Modulus)

            return BIG.monty(md,ROM.MConst,&d)
        }
        return BIG(0)
    }


    init()
    {
        x=BIG()
        xes=1
    }
    init(_ a: Int)
    {
        if a<0 {
            var m=BIG(ROM.Modulus)
            m.inc(a); m.norm();
            x=BIG(m)
        } else {
            x=BIG(a)
        }
        xes=1
        nres()
    }
    public init(_ a: BIG)
    {
        x=BIG(a)
        xes=1
        nres()
    }
    init(_ a: FP)
    {
        x=BIG(a.x)
        xes=a.xes
    }

    public init(_ rng: inout RAND)
    {
       let m = BIG(ROM.Modulus) 
       x = BIG.randomnum(m,&rng)
       xes=1
    }

    /* convert to string */
    public func toString() -> String
    {
        let s=redc().toString()
        return s
    }

    func toRawString() -> String
    {
        let s=x.toRawString()
        return s
    }

/* reduce this mod Modulus */
    mutating func reduce()
    {

        var m=BIG(FP.p)
        var r=BIG(FP.p)
        var sb:Int

        x.norm()

	   if xes>16 {
		  let q=FP.quo(x,m)
		  let carry=r.pmul(q)
		  r.w[CONFIG_BIG.NLEN-1]+=carry<<Chunk(CONFIG_BIG.BASEBITS); // correction - put any carry out back in again
		  x.sub(r)
		  x.norm()
		  sb=2
	   } else {
		  sb=FP.logb2(UInt32(xes-Int32(1)))
	   }
        m.fshl(sb)

        while sb>0 {
            let sr=BIG.ssn(&r,x,&m)
            x.cmove(r,1-sr)
            sb -= 1
        }

        xes=1
    }

/* test this=0? */
    func iszilch() -> Bool
    {
        var z=FP(self)
        z.reduce()
        return z.x.iszilch()
    }

    func islarger() -> Int
    {
        if iszilch() {
            return 0
        }
        var sx=BIG(ROM.Modulus)
        let fx=redc()
        sx.sub(fx); sx.norm()
        return BIG.comp(fx,sx)
    }

    func toBytes(_ b:inout [UInt8]) 
    {
        redc().toBytes(&b)
    }

    static func fromBytes(_ b: [UInt8]) -> FP
    {
        let t=BIG.fromBytes(b)
        return FP(t)
    }

/* test this=0? */
    func isunity() -> Bool
    {
        var z=FP(self)
        z.reduce()
        return z.redc().isunity()
    }

    func sign() -> Int
    {
        if CONFIG_FIELD.BIG_ENDIAN_SIGN {
            var m = BIG(ROM.Modulus)
            m.dec(1)
            m.fshr(1)
            var n = FP(self)
            n.reduce()
            let w=n.redc()
            let cp=BIG.comp(w,m)
            return ((cp+1)&2)>>1
        } else {
            var z=FP(self)
            z.reduce()
            return z.redc().parity()
        }
    }

/* copy from FP b */
    mutating func copy(_ b: FP)
    {
        x.copy(b.x)
        xes=b.xes
    }

/* set this=0 */
    mutating func zero()
    {
        x.zero();
        xes=1;
    }

/* set this=1 */
    mutating func one()
    {
        x.one(); nres()
    }

/* normalise this */
    mutating func norm()
    {
        x.norm();
    }
/* swap FPs depending on d */
    mutating func cswap(_ b: inout FP,_ d: Int)
    {
        var c=Int32(d)
        x.cswap(&(b.x),d)
        c = ~(c-1)
        let t=c&(xes^b.xes)
        xes^=t
        b.xes^=t
    }

/* copy FPs depending on d */
    mutating func cmove(_ b: FP,_ d:Int)
    {
        let c=Int32(-d)
        x.cmove(b.x,d)
        xes^=(xes^b.xes)&c
    }
/* this*=b mod Modulus */
    mutating func mul(_ b: FP)
    {

        if Int64(xes)*Int64(b.xes) > Int64(CONFIG_FIELD.FEXCESS) {reduce()}

        var d=BIG.mul(x,b.x)
        x.copy(FP.mod(&d))
        xes=2
    }
    static func logb2(_ w: UInt32) -> Int
    {
        var v = w
        v |= (v >> 1)
        v |= (v >> 2)
        v |= (v >> 4)
        v |= (v >> 8)
        v |= (v >> 16)

        v = v - ((v >> 1) & 0x55555555)
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333)
        let r = Int((   ((v + (v >> 4)) & 0xF0F0F0F)   &* 0x1010101) >> 24)
        return (r)
    }

// find appoximation to quotient of a/m
// Out by at most 2.
// Note that MAXXES is bounded to be 2-bits less than half a word
    static func quo(_ n: BIG,_ m: BIG) -> Int
    {
        let hb=UInt(CONFIG_BIG.CHUNK)/2
        if CONFIG_FIELD.TBITS < hb {
		  let sh=Chunk(hb-CONFIG_FIELD.TBITS);
		  let num=((n.w[CONFIG_BIG.NLEN-1]<<sh))|(n.w[CONFIG_BIG.NLEN-2]>>(Chunk(CONFIG_BIG.BASEBITS)-sh));
		  let den=((m.w[CONFIG_BIG.NLEN-1]<<sh))|(m.w[CONFIG_BIG.NLEN-2]>>(Chunk(CONFIG_BIG.BASEBITS)-sh));
		  return Int(num/(den+1));
	   } else {
		  let num=n.w[CONFIG_BIG.NLEN-1];
		  let den=m.w[CONFIG_BIG.NLEN-1];
		  return Int(num/(den+1));
	   }
    }

    /* this = -this mod Modulus */
    mutating func neg()
    {
        var m=BIG(FP.p)
        let sb=FP.logb2(UInt32(xes-Int32(1)))
        m.fshl(sb)
        x.rsub(m)
        xes=(1<<Int32(sb))+1
        if xes>CONFIG_FIELD.FEXCESS {reduce()}
    }
    /* this*=c mod Modulus, where c is a small int */
    mutating func imul(_ c: Int)
    {
        var cc=c
    //    norm();
        var s=false
        if (cc<0)
        {
            cc = -cc
            s=true
        }

        if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.PSEUDO_MERSENNE || CONFIG_FIELD.MODTYPE==CONFIG_FIELD.GENERALISED_MERSENNE
        {
            var d=x.pxmul(cc)
            x.copy(FP.mod(&d))
            xes=2
        }
        else {
            if xes*Int32(cc)<CONFIG_FIELD.FEXCESS
            {
                x.pmul(cc)
                xes*=Int32(cc);
            }
            else {
                let n=FP(cc)
                self.mul(n)
            }
        }

        if s {neg();  norm()}

    }

/* this*=this mod Modulus */
    mutating func sqr()
    {
        if Int64(xes)*Int64(xes) > Int64(CONFIG_FIELD.FEXCESS) {reduce()}
        var d=BIG.sqr(x);
        x.copy(FP.mod(&d));
        xes=2
    }

    /* this+=b */
    mutating func add(_ b: FP)
    {
        x.add(b.x);
        xes+=b.xes
        if xes>CONFIG_FIELD.FEXCESS {reduce()}
    }
/* this-=b */
    mutating func sub(_ b: FP)
    {
        var n=FP(b)
        n.neg()
        self.add(n)
    }
/* this=b-this */
    mutating func rsub(_ b: FP)
    {
        self.neg();
        self.add(b)
    }
/* this/=2 mod Modulus */
    mutating func div2()
    {
        let pr=x.parity()
        var w=BIG(x)
        x.fshr(1)
        w.add(FP.p); w.norm()
        w.fshr(1)
        x.cmove(w,pr)

/*
    //    x.norm()
        if (x.parity()==0)
            {x.fshr(1)}
        else
        {
            x.add(FP.p)
            x.norm()
            x.fshr(1)
        } */
    }

/* return jacobi symbol (this/Modulus) */
    func jacobi() -> Int
    {
        var w=redc()
        return w.jacobi(FP.p)
    }

/* return TRUE if this==a */
    func equals(_ a: FP) -> Bool
    {
        var f=FP(self)
        var s=FP(a)
        f.reduce()
        s.reduce()
        if (BIG.comp(f.x,s.x)==0) {return true}
        return false;
    }


/* return this^e mod Modulus */
    mutating func pow(_ e: BIG) -> FP
    {
        var tb=[FP]()
        let n=1+(CONFIG_BIG.NLEN*Int(CONFIG_BIG.BASEBITS)+3)/4
        var w=[Int8](repeating: 0,count: n)
        norm()
        var t=BIG(e); t.norm()
        let nb=1+(t.nbits()+3)/4

        for i in 0 ..< nb  {
            let lsbs=t.lastbits(4)
            t.dec(lsbs)
            t.norm()
            w[i]=Int8(lsbs)
            t.fshr(4);
        }
        tb.append(FP(1))
        tb.append(FP(self))
        for i in 2 ..< 16 {
            tb.append(FP(tb[i-1]))
            tb[i].mul(self)
        }
        var r=FP(tb[Int(w[nb-1])])
        for i in (0...nb-2).reversed() {
            r.sqr()
            r.sqr()
            r.sqr()
            r.sqr()
            r.mul(tb[Int(w[i])])
        }
        r.reduce()
        return r
    }

// See eprint paper https://eprint.iacr.org/2018/1038
// return this^(p-3)/4 or this^(p-5)/8
    mutating func fpow() -> FP
    {
        let ac: [Int] = [1, 2, 3, 6, 12, 15, 30, 60, 120, 240, 255]
        var xp=[FP]()
// phase 1
        xp.append(FP(self))
        xp.append(FP(self)); xp[1].sqr()
        xp.append(FP(xp[1])); xp[2].mul(self)
        xp.append(FP(xp[2])); xp[3].sqr()
        xp.append(FP(xp[3])); xp[4].sqr()
        xp.append(FP(xp[4])); xp[5].mul(xp[2])
        xp.append(FP(xp[5])); xp[6].sqr()
        xp.append(FP(xp[6])); xp[7].sqr()
        xp.append(FP(xp[7])); xp[8].sqr()
        xp.append(FP(xp[8])); xp[9].sqr()
        xp.append(FP(xp[9])); xp[10].mul(xp[5])

        var n=Int(CONFIG_FIELD.MODBITS)
        var c: Int
        if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.GENERALISED_MERSENNE {   // Goldilocks ONLY
            n=n/2
        }

        let e=Int(CONFIG_FIELD.PM1D2)
        n = n-(e+1)
        c=((Int(ROM.MConst))+(1<<e)+1)/(1<<(e+1));

        var nd=0
        while c%2 == 0 {
            c /= 2
            n -= 1
            nd += 1
        }

        var bw=0; var w=1; while w<c {w*=2; bw+=1}
        var k=w-c

        var i=10; var key=FP()

        if k != 0 {
            while ac[i]>k {i-=1}
            key.copy(xp[i])
            k-=ac[i]
        }
        while k != 0 {
            i-=1
            if ac[i]>k {continue}
            key.mul(xp[i])
            k-=ac[i]
        }

// phase 2
        xp[1].copy(xp[2])
        xp[2].copy(xp[5])
        xp[3].copy(xp[10])

        var j=3; var m=8
        let nw=n-bw
        var t=FP()

        while 2*m<nw {
            t.copy(xp[j]); j+=1
            for _ in 0..<m {t.sqr()}
            xp[j].copy(xp[j-1])
            xp[j].mul(t)
            m*=2
        }

        var lo=nw-m
        var r=FP(xp[j])

        while lo != 0 {
            m/=2; j-=1
            if lo<m {continue}
            lo-=m
            t.copy(r)
            for _ in 0..<m {t.sqr()}
            r.copy(t)
            r.mul(xp[j])
        }

        if bw != 0 {
            for _ in 0..<bw {r.sqr()}
            r.mul(key)
        }
        if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.GENERALISED_MERSENNE {   // Goldilocks ONLY
            key.copy(r)
            r.sqr()
            r.mul(self)
            for _ in 0..<n+1 {r.sqr()}
            r.mul(key)
        }
        while nd > 0 {
            r.sqr()
            nd -= 1
        }
        return r
    }

    /* Pseudo_inverse square root */
    mutating func progen()
    {
        if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.PSEUDO_MERSENNE || CONFIG_FIELD.MODTYPE==CONFIG_FIELD.GENERALISED_MERSENNE {
            copy(fpow())
            return
        }
        let e=CONFIG_FIELD.PM1D2
        var m=BIG(ROM.Modulus)
        m.dec(1)
        m.shr(e)
        m.dec(1)
        m.fshr(1)

        copy(pow(m))
    }

/* this=1/this mod Modulus */
    mutating func inverse(_ hint: FP?)
    {
        let e=CONFIG_FIELD.PM1D2
        norm()
        var s=(self);
        for _ in 0..<(e-1) {
            s.sqr()
            s.mul(self)
        }
        if hint != nil {
            copy(hint!)
        } else {
            progen()
        }
        for _ in 0...e {
            sqr();
        }
        mul(s)
        reduce()
    }
/* Test for Quadratic Residue */
    func qr(_ hint:inout FP?) -> Int
    {
        let e=CONFIG_FIELD.PM1D2
        var r=FP(self);
        r.progen()
        if hint != nil {
            hint!.copy(r)
        }

        r.sqr()
        r.mul(self)
        for _ in 0..<(e-1) {
            r.sqr()
        }

        return r.isunity() ? 1:0
    }

// Calculate both inverse and square root of x, return QR
    @discardableResult func invsqrt(_ i:inout FP,_ s:inout FP) -> Int
    {
        var h:FP?=FP()
        let qres=qr(&h)
        s.copy(sqrt(h))
        i.copy(self)
        i.inverse(h)
        return qres
    }

// Two for the price of One  - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
    @discardableResult static func tpo(_ i:inout FP,_ s:inout FP) -> Int
    {
        var w = FP(s)
        var t = FP(i)
        w.mul(i)
        t.mul(w)
        let qres=t.invsqrt(&i,&s)
        i.mul(w)
        s.mul(i)
        return qres
    }

/* return sqrt(this) mod Modulus */
    func sqrt(_ hint: FP?) -> FP
    {
        let e=CONFIG_FIELD.PM1D2
        var g=FP(self);

        if hint != nil {
            g.copy(hint!)
        } else {
            g.progen()
        }
        let m=BIG(ROM.ROI)

        var v=FP(m)
        var t=FP(g)
        t.sqr()
        t.mul(self)

        var r=FP(self)
        r.mul(g)
        var b=FP(t)

        for k in stride(from: e, to: 1, by: -1) //(int k=e;k>1;k--)
        {
            for _ in 1..<k-1 {
                b.sqr()
            }
            let u=b.isunity() ? 0 : 1
            g.copy(r); g.mul(v)
            r.cmove(g,u)
            v.sqr()
            g.copy(t); g.mul(v)
            t.cmove(g,u)
            b.copy(t)
        }

        let sgn=r.sign()
        var nr=FP(r)
        nr.neg(); nr.norm()
        r.cmove(nr,sgn)

        return r

    }

/* return sqrt(this) mod Modulus
    mutating func sqrt() -> FP
    {
        reduce();
        if (CONFIG_FIELD.PM1D2==2)
        {
            var v: FP
            var i=FP(self); i.x.shl(1)
            if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.PSEUDO_MERSENNE  || CONFIG_FIELD.MODTYPE==CONFIG_FIELD.GENERALISED_MERSENNE {
                v=i.fpow()
            } else {
                var b=BIG(FP.p)
                b.dec(5); b.norm(); b.shr(3)
                v=i.pow(b)
            }
            i.mul(v); i.mul(v)
            i.x.dec(1)
            var r=FP(self)
            r.mul(v); r.mul(i)
            r.reduce()
            return r
        }
        else
        {
           if CONFIG_FIELD.MODTYPE==CONFIG_FIELD.PSEUDO_MERSENNE  || CONFIG_FIELD.MODTYPE==CONFIG_FIELD.GENERALISED_MERSENNE {
                var r=fpow()
                r.mul(self)
                return r
            } else {
                var b=BIG(FP.p)
                b.inc(1); b.norm(); b.shr(2)
                return pow(b)
            }
        }
    }  */
}
