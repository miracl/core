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
//  fp16.swift
//
//  Created by Michael Scott on 07/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Finite Field arithmetic  Fp^16 functions */

/* FP16 elements are of the form a+ib, where i is sqrt(sqrt(sqrt(-1+sqrt(-1)))  */

public struct FP16 {
    private var a:FP8
    private var b:FP8

    /* constructors */
    init()
    {
        a=FP8()
        b=FP8()
    }
    init(_ c:Int)
    {
        a=FP8(c)
        b=FP8()
    }

    init(_ x:FP16)
    {
        a=FP8(x.a)
        b=FP8(x.b)
    }

    init(_ c:FP8,_ d:FP8)
    {
        a=FP8(c)
        b=FP8(d)
    }

    init(_ c:FP8)
    {
        a=FP8(c)
        b=FP8()
    }
    /* reduce all components of this mod Modulus */
    mutating func reduce()
    {
        a.reduce()
        b.reduce()
    }
    /* normalise all components of this mod Modulus */
    mutating func norm()
    {
        a.norm()
        b.norm()
    }
    /* test this==0 ? */
    func iszilch() -> Bool
    {
        return a.iszilch() && b.iszilch()
    }

    func toBytes(_ bf:inout [UInt8])
    {
        let RM=8*Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM) 
	    b.toBytes(&t)
	    for i in 0 ..< RM {
	    	bf[i]=t[i]
	    }
	    a.toBytes(&t)
	    for i in 0 ..< RM {
		    bf[i+RM]=t[i]
	    }        
    }

    static func fromBytes(_ bf: [UInt8]) -> FP16
    {
        let RM=8*Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM) 
	    for i in 0 ..< RM {
            t[i]=bf[i]
        }
        let tb=FP8.fromBytes(t)
	    for i in 0 ..< RM {
            t[i]=bf[i+RM]
	    }
        let ta=FP8.fromBytes(t)
	    return FP16(ta,tb)
    }

    mutating func cmove(_ g:FP16,_ d:Int)
    {
        a.cmove(g.a,d)
        b.cmove(g.b,d)
    }


    /* test this==1 ? */
    func isunity() -> Bool
    {
        let one=FP8(1);
        return a.equals(one) && b.iszilch()
    }

    /* test is w real? That is in a+ib test b is zero */
    func isreal() -> Bool
    {
        return b.iszilch();
    }
    /* extract real part a */
    func real() -> FP8
    {
        return a;
    }

    func geta() -> FP8
    {
        return a;
    }
    /* extract imaginary part b */
    func getb() -> FP8
    {
        return b;
    }
    mutating func set_fp8s(_ c: FP8,_ d: FP8)
    {
        a.copy(c)
        b.copy(d)
    }
    mutating func set_fp8(_ c: FP8)
    {
        a.copy(c)
        b.zero()
    }
    mutating func set_fp8h(_ c: FP8)
    {
        b.copy(c)
        a.zero()
    }
    /* test self=x? */
    func equals(_ x:FP16) -> Bool
    {
        return a.equals(x.a) && b.equals(x.b)
    }
    /* copy self=x */
    mutating func copy(_ x:FP16)
    {
        a.copy(x.a)
        b.copy(x.b)
    }

        /* set this=0 */
    mutating func zero()
    {
        a.zero()
        b.zero()
    }
    /* set this=1 */
    mutating func one()
    {
        a.one()
        b.zero()
    }
    /* set self=-self */
    mutating func neg()
    {
        norm()
        var m=FP8(a)
        var t=FP8()
        m.add(b)
        m.neg()
        t.copy(m); t.add(b)
        b.copy(m)
        b.add(a)
        a.copy(t)
        norm()
    }
    /* self=conjugate(self) */
    mutating func conj()
    {
        b.neg(); norm()
    }
    /* this=-conjugate(this) */
    mutating func nconj()
    {
        a.neg(); norm()
    }
    /* self+=x */

    mutating func adds(_ x: FP8)
    {
        a.add(x)
    }

    mutating func add(_ x:FP16)
    {
        a.add(x.a)
        b.add(x.b)
    }
    /* self-=x */
    mutating func sub(_ x:FP16)
    {
        var m=FP16(x)
        m.neg()
        add(m)
    }

    /* self-=x */
    mutating func rsub(_ x: FP16) {
        neg()
        add(x)
    }

    /* self*=s where s is FP8 */
    mutating func pmul(_ s:FP8)
    {
        a.mul(s)
        b.mul(s)
    }

    /* self*=s where s is FP2 */
    mutating func qmul(_ s:FP2) {
        a.qmul(s)
        b.qmul(s)
    }

    /* self*=s where s is FP */
    mutating func tmul(_ s:FP) {
        a.tmul(s)
        b.tmul(s)
    }

    /* self*=c where c is int */
    mutating func imul(_ c:Int)
    {
        a.imul(c)
        b.imul(c)
    }

    /* self*=self */
    mutating func sqr()
    {
        var t1=FP8(a)
        var t2=FP8(b)
        var t3=FP8(a)

        t3.mul(b)
        t1.add(b)
        t2.times_i()

        t2.add(a)

        t1.norm(); t2.norm()
        a.copy(t1)

        a.mul(t2)

        t2.copy(t3)
        t2.times_i()
        t2.add(t3); t2.norm()

        t2.neg()
        a.add(t2)

        b.copy(t3)
        b.add(t3)

        norm()
    }

    /* self*=y */
    mutating func mul(_ y:FP16)
    {
        var t1=FP8(a)
        var t2=FP8(b)
        var t3=FP8()
        var t4=FP8(b)

        t1.mul(y.a)
        t2.mul(y.b)
        t3.copy(y.b)
        t3.add(y.a)
        t4.add(a)

        t3.norm(); t4.norm()

        t4.mul(t3)

        t3.copy(t1)
        t3.neg()
        t4.add(t3)
        t4.norm()

        t3.copy(t2)
        t3.neg()
        b.copy(t4)
        b.add(t3)

        t2.times_i()
        a.copy(t2)
        a.add(t1)

        norm();
    }
    /* convert this to hex string */
    func toString() -> String
    {
        return ("["+a.toString()+","+b.toString()+"]")
    }

   func toRawString() -> String
    {
        return ("["+a.toRawString()+","+b.toRawString()+"]")
    }
    /* self=1/self */
    mutating func inverse()
    {
        var t1=FP8(a)
        var t2=FP8(b)
        let pNIL:FP?=nil

        t1.sqr()
        t2.sqr()
        t2.times_i(); t2.norm()
        t1.sub(t2); t1.norm()
        t1.inverse(pNIL)
        a.mul(t1)
        t1.neg(); t1.norm()
        b.mul(t1)
    }

    /* self*=i where i = sqrt(sqrt(-1+sqrt(-1))) */
    mutating func times_i()
    {
        var s=FP8(b)
        let t=FP8(a)
        s.times_i()
        a.copy(s)
        b.copy(t)
        norm()
    }

    mutating func times_i2() {
        a.times_i()
        b.times_i()
    }

    mutating func times_i4() {
        a.times_i2()
        b.times_i2()
    }

    /* self=self^p using Frobenius */
    mutating func frob(_ f:FP2)
    {
        var ff=FP2(f); ff.sqr(); ff.norm()

        a.frob(ff)
        b.frob(ff)
        b.qmul(f)
        b.times_i()
    }

    /* self=self^e */
    func pow(_ e:BIG) -> FP16
    {
        var w=FP16(self)
        w.norm()
        var z=BIG(e)
        var r=FP16(1)
        z.norm()
        while (true)
        {
            let bt=z.parity()
            z.fshr(1)
            if bt==1 {r.mul(w)}
            if z.iszilch() {break}
            w.sqr()
        }
        r.reduce()
        return r
    }

    /* XTR xtr_a function */
/*
    mutating func xtr_A(_ w:FP16,_ y:FP16,_ z:FP16)
    {
        var r=FP16(w)
        var t=FP16(w)
        r.sub(y); r.norm()
        r.pmul(a)
        t.add(y); t.norm()
        t.pmul(b)
        t.times_i()

        copy(r)
        add(t)
        add(z)

        norm()
    }
*/
    /* XTR xtr_d function */
/*
    mutating func xtr_D()
    {
        var w=FP16(self)
        sqr(); w.conj()
        w.add(w); w.norm();
        sub(w)
        reduce()
    }
*/
    /* r=x^n using XTR method on traces of FP48s */
/*
    func xtr_pow(_ n:BIG) -> FP16
    {
        var sf=FP16(self)
        sf.norm()
        var a=FP16(3)
        var b=FP16(sf)
        var c=FP16(b)
        c.xtr_D()
        var t=FP16(0)
        var r=FP16(0)

        let par=n.parity()
        var v=BIG(n); v.norm(); v.fshr(1)
        if par==0 {v.dec(1); v.norm()}

        let nb=v.nbits()
        var i=nb-1
        while i>=0
        {
            if (v.bit(UInt(i)) != 1)
            {
                t.copy(b)
                sf.conj()
                c.conj()
                b.xtr_A(a,sf,c)
                sf.conj()
                c.copy(t)
                c.xtr_D()
                a.xtr_D()
            }
            else
            {
                t.copy(a); t.conj()
                a.copy(b)
                a.xtr_D()
                b.xtr_A(c,sf,t)
                c.xtr_D()
            }
            i-=1
        }
        if par==0 {r.copy(c)}
        else {r.copy(b)}
        r.reduce()
        return r
    }
*/
    /* r=ck^a.cl^n using XTR double exponentiation method on traces of FP48s. See Stam thesis. */
/*
    func xtr_pow2(_ ck:FP16,_ ckml:FP16,_ ckm2l:FP16,_ a:BIG,_ b:BIG) -> FP16
    {

        var e=BIG(a)
        var d=BIG(b)
        var w=BIG(0)
        e.norm(); d.norm()

        var cu=FP16(ck)  // can probably be passed in w/o copying
        var cv=FP16(self)
        var cumv=FP16(ckml)
        var cum2v=FP16(ckm2l)
        var r=FP16(0)
        var t=FP16(0)

        var f2:Int=0
        while d.parity()==0 && e.parity()==0
        {
            d.fshr(1);
            e.fshr(1);
            f2 += 1;
        }

        while (BIG.comp(d,e) != 0)
        {
            if BIG.comp(d,e)>0
            {
                w.copy(e); w.imul(4); w.norm()
                if BIG.comp(d,w)<=0
                {
                    w.copy(d); d.copy(e)
                    e.rsub(w); e.norm()

                    t.copy(cv)
                    t.xtr_A(cu,cumv,cum2v)
                    cum2v.copy(cumv)
                    cum2v.conj()
                    cumv.copy(cv)
                    cv.copy(cu)
                    cu.copy(t)

                }
                else if d.parity()==0
                {
                    d.fshr(1)
                    r.copy(cum2v); r.conj()
                    t.copy(cumv)
                    t.xtr_A(cu,cv,r)
                    cum2v.copy(cumv)
                    cum2v.xtr_D()
                    cumv.copy(t)
                    cu.xtr_D()
                }
                else if e.parity()==1
                {
                    d.sub(e); d.norm()
                    d.fshr(1)
                    t.copy(cv)
                    t.xtr_A(cu,cumv,cum2v)
                    cu.xtr_D()
                    cum2v.copy(cv)
                    cum2v.xtr_D()
                    cum2v.conj()
                    cv.copy(t)
                }
                else
                {
                    w.copy(d)
                    d.copy(e); d.fshr(1)
                    e.copy(w)
                    t.copy(cumv)
                    t.xtr_D()
                    cumv.copy(cum2v); cumv.conj()
                    cum2v.copy(t); cum2v.conj()
                    t.copy(cv)
                    t.xtr_D()
                    cv.copy(cu)
                    cu.copy(t)
                }
            }
            if BIG.comp(d,e)<0
            {
                w.copy(d); w.imul(4); w.norm()
                if BIG.comp(e,w)<=0
                {
                    e.sub(d); e.norm()
                    t.copy(cv)
                    t.xtr_A(cu,cumv,cum2v)
                    cum2v.copy(cumv)
                    cumv.copy(cu)
                    cu.copy(t)
                }
                else if e.parity()==0
                {
                    w.copy(d)
                    d.copy(e); d.fshr(1)
                    e.copy(w)
                    t.copy(cumv)
                    t.xtr_D()
                    cumv.copy(cum2v); cumv.conj()
                    cum2v.copy(t); cum2v.conj()
                    t.copy(cv)
                    t.xtr_D()
                    cv.copy(cu)
                    cu.copy(t)
                }
                else if d.parity()==1
                {
                    w.copy(e)
                    e.copy(d)
                    w.sub(d); w.norm()
                    d.copy(w); d.fshr(1)
                    t.copy(cv)
                    t.xtr_A(cu,cumv,cum2v)
                    cumv.conj()
                    cum2v.copy(cu)
                    cum2v.xtr_D()
                    cum2v.conj()
                    cu.copy(cv)
                    cu.xtr_D()
                    cv.copy(t)
                }
                else
                {
                    d.fshr(1)
                    r.copy(cum2v); r.conj()
                    t.copy(cumv)
                    t.xtr_A(cu,cv,r)
                    cum2v.copy(cumv)
                    cum2v.xtr_D()
                    cumv.copy(t)
                    cu.xtr_D()
                }
            }
        }
        r.copy(cv)
        r.xtr_A(cu,cumv,cum2v)
        for _ in 0 ..< f2
            {r.xtr_D()}
        r=r.xtr_pow(d)
        return r
    }
*/
}
