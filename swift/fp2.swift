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
//  fp2.swift
//
//  Created by Michael Scott on 07/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Finite Field arithmetic  Fp^2 functions */

/* FP2 elements are of the form a+ib, where i is sqrt(-1) */

import core

public struct FP2
{
    private var a:FP
    private var b:FP

    /* Constructors */

    init()
    {
        a=FP()
        b=FP()
    }

    init(_ c: Int)
    {
        a=FP(c)
        b=FP()
    }
    
    init(_ c: Int,_ d: Int)
    {
        a=FP(c)
        b=FP(d)
    }

    init(_ x:FP2)
    {
        a=FP(x.a)
        b=FP(x.b)
    }

    public init(_ c:FP,_ d:FP)
    {
        a=FP(c)
        b=FP(d)
    }
    
    public init(_ c:BIG,_ d:BIG)
    {
        a=FP(c)
        b=FP(d)
    }

    init(_ c:FP)
    {
        a=FP(c)
        b=FP()
    }
    
    init(_ c:BIG)
    {
        a=FP(c)
        b=FP()
    }

    public init(_ rng: inout RAND)
    {
        a=FP(&rng)
        b=FP(&rng)
    }

    /* test this=0 ? */
    func iszilch() -> Bool
    {
        return (a.iszilch() && b.iszilch())
    }
    
    func islarger() -> Int
    {
        if iszilch() {
            return 0
        }
        let cmp=b.islarger()
        if cmp != 0 {
            return cmp
        }
        return a.islarger()
    }

    func toBytes(_ bf:inout [UInt8])
    {
        let RM=Int(CONFIG_BIG.MODBYTES)
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

    static func fromBytes(_ bf: [UInt8]) -> FP2
    {
        let RM=Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM) 
	    for i in 0 ..< RM {
            t[i]=bf[i]
        }
        let tb=FP.fromBytes(t)
	    for i in 0 ..< RM {
            t[i]=bf[i+RM]
	    }
        let ta=FP.fromBytes(t)
	    return FP2(ta,tb)
    }

    mutating func cmove(_ g:FP2,_ d:Int)
    {
        a.cmove(g.a,d)
        b.cmove(g.b,d)
    }

    /* test this=1 ? */
    func isunity() -> Bool
    {
        let one=FP(1)
        return (a.equals(one) && b.iszilch())
    }
    
    /* test this=x */
    func equals(_ x:FP2) -> Bool
    {
        return (a.equals(x.a) && b.equals(x.b));
    }
    
    
    /* reduce components mod Modulus */
    mutating func reduce()
    {
        a.reduce()
        b.reduce()
    }
    
    /* normalise components of w */
    mutating func norm()
    {
        a.norm()
        b.norm()
    }
    
    /* extract a */
    func getA() -> BIG
    {
        return a.redc()
    }
    
    /* extract b */
    func getB() -> BIG
    {
        return b.redc()
    }

/* extract a */
    func geta() -> FP
    {
        return a
    }
    
    /* extract b */
    func getb() -> FP
    {
        return b
    }

    /* copy self=x */
    mutating func copy(_ x:FP2)
    {
        a.copy(x.a)
        b.copy(x.b)
    }
    
    /* set self=0 */
    mutating func zero()
    {
        a.zero()
        b.zero()
    }
    
    /* set self=1 */
    mutating func one()
    {
        a.one()
        b.zero()
    }
    
    func sign() -> Int
    {
        var p1=a.sign()
        var p2=b.sign()
        if CONFIG_FIELD.BIG_ENDIAN_SIGN {
            let u=b.iszilch() ? 1 : 0
	        p2^=(p1^p2)&u
	        return p2
        } else {
            let u=a.iszilch() ? 1 : 0
	        p1^=(p1^p2)&u
	        return p1
        }
    }

    /* negate self mod Modulus */
    mutating func neg()
    {
        var m=FP(a)
        var t=FP()
    
        m.add(b)
        m.neg()
        t.copy(m); t.add(b)
        b.copy(m)
        b.add(a)
        a.copy(t)
    }
    
    /* set to a-ib */
    mutating func conj()
    {
        b.neg(); b.norm()
    }

    /* self+=a */
    mutating func add(_ x:FP2)
    {
        a.add(x.a)
        b.add(x.b)
    }
    
    /* self-=a */
    mutating func sub(_ x:FP2)
    {
        var m=FP2(x)
        m.neg()
        add(m)
    }

    /* self=a-self */
    mutating func rsub(_ x:FP2)
    {
        self.neg()
        self.add(x)
    }

    /* self*=s, where s is an FP */
    mutating func pmul(_ s:FP)
    {
        a.mul(s)
        b.mul(s)
    }
    
    /* self*=i, where i is an int */
    mutating func imul(_ c:Int)
    {
        a.imul(c);
        b.imul(c);
    }
    
    /* self*=self */
    mutating func sqr()
    {
        var w1=FP(a)
        var w3=FP(a)
        var mb=FP(b)

        w1.add(b)

        w3.add(a)
        w3.norm()
        b.mul(w3)

        mb.neg()
        a.add(mb)

        a.norm()
        w1.norm()

        a.mul(w1)

    }
    /* self*=y */
    mutating func mul(_ y:FP2)
    { 
        if Int64(a.xes+b.xes)*Int64(y.a.xes+y.b.xes) > Int64(CONFIG_FIELD.FEXCESS)
        {
            if a.xes>1 {a.reduce()}
            if b.xes>1 {b.reduce()}       
        }

        var pR=DBIG(0)
        pR.ucopy(FP.p)

        var C=BIG(a.x)
        var D=BIG(y.a.x)

        var A=BIG.mul(a.x,y.a.x)
        var B=BIG.mul(b.x,y.b.x)

        C.add(b.x); C.norm()
        D.add(y.b.x); D.norm()

        var E=BIG.mul(C,D)
        var F=DBIG(A); F.add(B);
        B.rsub(pR);

        A.add(B); A.norm()
        E.sub(F); E.norm()

        a.x.copy(FP.mod(&A)); a.xes=3
        b.x.copy(FP.mod(&E)); b.xes=2
    
    }
 /*
    mutating func pow(_ e:BIG)
    {
        var w=FP2(self)       
        var z=BIG(e)
        var r=FP2(1)
        while (true)
        {
            let bt=z.parity()
            z.fshr(1)
            if bt==1 {r.mul(w)}
            if z.iszilch() {break}
            w.sqr()
        }
        r.reduce()
        copy(r)
    }*/

    func qr(_ h:inout FP?) -> Int
    {
    //    var pNIL:FP?=nil
        var c=FP2(self) 
        c.conj()
        c.mul(self)
        return c.geta().qr(&h)
    }

    /* sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b)/2)+ib/(2*sqrt(a+sqrt(a*a-n*b*b)/2)) */
    /* returns true if this is QR */
    mutating func sqrt(_ h: FP?)
    {
        if iszilch() {return}
        var w1=FP(b)
        var w2=FP(a)
        var w3=FP(a)
        var w4=FP()
        var hint:FP?=FP()

        w1.sqr(); w2.sqr(); w1.add(w2); w1.norm()
        
        w1=w1.sqrt(h)

        w2.copy(a); w2.add(w1); w2.norm(); w2.div2()

        w1.copy(b); w1.div2()
        let qr=w2.qr(&hint)

// tweak hint
        w3.copy(hint!); w3.neg(); w3.norm()
        w4.copy(w2); w4.neg(); w4.norm()

        w2.cmove(w4,1-qr)
        hint!.cmove(w3,1-qr)

        a.copy(w2.sqrt(hint))
        w3.copy(w2); w3.inverse(hint)
        w3.mul(a)
        b.copy(w3); b.mul(w1)
        w4.copy(a)

        a.cmove(b,1-qr)
        b.cmove(w4,1-qr)

/*
        a.copy(w2.sqrt(hint))
        w3.copy(w2); w3.inverse(hint)
        w3.mul(a)
        b.copy(w3); b.mul(w1)

        hint!.neg(); hint!.norm()
        w2.neg(); w2.norm()

        w4.copy(w2.sqrt(hint))
        w3.copy(w2); w3.inverse(hint)
        w3.mul(w4)
        w3.mul(w1)

        a.cmove(w3,1-qr)
        b.cmove(w4,1-qr)
*/
        let sgn=self.sign()
        var nr=FP2(self)
        nr.neg(); nr.norm()
        self.cmove(nr,sgn)    
    }
    /* output to hex string */
    public func toString() -> String
    {
        return ("["+a.toString()+","+b.toString()+"]")
    }
    
    func toRawString() -> String
    {
        return ("["+a.toRawString()+","+b.toRawString()+"]")
    }

    /* self=1/self */
    mutating func inverse(_ h: FP?)
    {
        //let pNIL:FP?=nil
        norm();
        var w1=FP(a)
        var w2=FP(b)
    
        w1.sqr()
        w2.sqr()
        w1.add(w2)
        w1.inverse(h)
        a.mul(w1)
        w1.neg(); w1.norm()
        b.mul(w1)
    }

    /* self/=2 */
    mutating func div2()
    {
        a.div2();
        b.div2();
    }
    
    /* self*=sqrt(-1) */
    mutating func times_i()
    {
        let z=FP(a)
        a.copy(b); a.neg()
        b.copy(z)
    }

    /* w*=(1+sqrt(-1)) */
    /* where X*2-(1+sqrt(-1)) is irreducible for FP4, assumes p=3 mod 8 */
    mutating func mul_ip()
    {
        var t=FP2(self)
        var i=CONFIG_FIELD.QNRI
        times_i()
        while i>0 {
            t.add(t)
            t.norm()
            i-=1
        }
        add(t)
	if CONFIG_FIELD.TOWER==CONFIG_FIELD.POSITOWER {
		norm()
		neg()
	}
    }
    /* w/=(1+sqrt(-1)) */
    mutating func div_ip()
    {
        var z=FP2(1<<CONFIG_FIELD.QNRI,1)
        let pNIL:FP?=nil
        z.inverse(pNIL)
        norm()
        mul(z)
	if CONFIG_FIELD.TOWER==CONFIG_FIELD.POSITOWER {
		neg()
		norm()
	}
    }

}
