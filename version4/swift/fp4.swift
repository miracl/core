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
//  fp4.swift
//
//  Created by Michael Scott on 07/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Finite Field arithmetic  Fp^4 functions */

/* FP4 elements are of the form a+ib, where i is sqrt(-1+sqrt(-1))  */

public struct FP4 {
    private var a:FP2
    private var b:FP2

    /* constructors */

    init()
    {
        a=FP2()
        b=FP2()        
    }

    init(_ c:Int)
    {
        a=FP2(c)
        b=FP2()
    }
    
    init(_ x:FP4)
    {
        a=FP2(x.a)
        b=FP2(x.b)
    }
    
    init(_ c:FP2,_ d:FP2)
    {
        a=FP2(c)
        b=FP2(d)
    }
    
    init(_ c:FP2)
    {
        a=FP2(c)
        b=FP2()
    }

    init(_ c:FP)
    {
        a=FP2(c)
        b=FP2()
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

    mutating func cmove(_ g:FP4,_ d:Int)
    {
        a.cmove(g.a,d)
        b.cmove(g.b,d)
    }

    /* test this==1 ? */
    func isunity() -> Bool
    {
    let one=FP2(1);
    return a.equals(one) && b.iszilch()
    }
    
    /* test is w real? That is in a+ib test b is zero */
    func isreal() -> Bool
    {
        return b.iszilch();
    }
    /* extract real part a */
    func real() -> FP2
    {
        return a;
    }
    
    func geta() -> FP2
    {
        return a;
    }
    /* extract imaginary part b */
    func getb() -> FP2
    {
    return b;
    }
    /* test self=x? */
    func equals(_ x:FP4) -> Bool
    {
        return a.equals(x.a) && b.equals(x.b)
    }
    mutating func set_fp2s(_ c: FP2,_ d: FP2)
    {
        a.copy(c)
        b.copy(d)
    }
    mutating func set_fp2(_ c: FP2)
    {
        a.copy(c)
        b.zero()
    }    
    mutating func set_fp2h(_ c: FP2)
    {
        b.copy(c)
        a.zero()
    }      
    /* copy self=x */
    mutating func copy(_ x:FP4)
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
        var m=FP2(a)
        var t=FP2()
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

    mutating func adds(_ x: FP2)
    {
        a.add(x)
    }

    /* self+=x */
    mutating func add(_ x:FP4)
    {
        a.add(x.a)
        b.add(x.b)
    }
    /* self-=x */
    mutating func sub(_ x:FP4)
    {
        var m=FP4(x)
        m.neg()
        add(m)
    }
    
    /* self-=x */
    mutating func rsub(_ x: FP4) {
        neg()
        add(x)
    }    

    /* self*=s where s is FP2 */
    mutating func pmul(_ s:FP2)
    {
        a.mul(s)
        b.mul(s)
    }

    /* self*=s where s is FP */
    mutating func qmul(_ s:FP) {
        a.pmul(s)
        b.pmul(s)
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
        var t1=FP2(a)
        var t2=FP2(b)
        var t3=FP2(a)
    
        t3.mul(b)
        t1.add(b)
        t2.mul_ip()
    
        t2.add(a)

        t1.norm(); t2.norm()
        a.copy(t1)
    
        a.mul(t2)
    
        t2.copy(t3)
        t2.mul_ip()
        t2.add(t3); t2.norm()

        t2.neg()
        a.add(t2)
    
        b.copy(t3)
        b.add(t3)
    
        norm()
    }
    /* self*=y */
    mutating func mul(_ y:FP4)
    {
        var t1=FP2(a)
        var t2=FP2(b)
        var t3=FP2()
        var t4=FP2(b)
    
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

        t2.mul_ip()
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
        //norm();
    
        var t1=FP2(a)
        var t2=FP2(b)
    
        t1.sqr()
        t2.sqr()
        t2.mul_ip(); t2.norm()
        t1.sub(t2)
        t1.inverse()
        a.mul(t1)
        t1.neg(); t1.norm()
        b.mul(t1)
    }
    
    /* self*=i where i = sqrt(-1+sqrt(-1)) */
    mutating func times_i()
    {
        var t=FP2(b)
        b.copy(a)
        t.mul_ip()
        a.copy(t)
        norm()
	if CONFIG_FIELD.TOWER==CONFIG_FIELD.POSITOWER {
		neg()
		norm()
	}
    }
    
    /* self=self^p using Frobenius */
    mutating func frob(_ f:FP2)
    {
        a.conj()
        b.conj()
        b.mul(f)
    }
    /* self=self^e */
    func pow(_ e:BIG) -> FP4
    {
        var w=FP4(self)
        w.norm()        
        var z=BIG(e)
        var r=FP4(1)
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
    mutating func xtr_A(_ w:FP4,_ y:FP4,_ z:FP4)
    {
        var r=FP4(w)
        var t=FP4(w)
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
    /* XTR xtr_d function */
    mutating func xtr_D()
    {
        var w=FP4(self)
        sqr(); w.conj()
        w.add(w); w.norm();
        sub(w)
        reduce()
    }
    /* r=x^n using XTR method on traces of FP12s */
    func xtr_pow(_ n:BIG) -> FP4
    {
        var a=FP4(3)
        var b=FP4(self)
        var c=FP4(b)
        c.xtr_D()
        var t=FP4(0)
        var r=FP4(0)
        var sf=FP4(self)
    
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
    
    /* r=ck^a.cl^n using XTR double exponentiation method on traces of FP12s. See Stam thesis. */
    func xtr_pow2(_ ck:FP4,_ ckml:FP4,_ ckm2l:FP4,_ a:BIG,_ b:BIG) -> FP4
    {

        var e=BIG(a)
        var d=BIG(b)
        var w=BIG(0)
        e.norm(); d.norm()
    
        var cu=FP4(ck)  // can probably be passed in w/o copying
        var cv=FP4(self)
        var cumv=FP4(ckml)
        var cum2v=FP4(ckm2l)
        var r=FP4(0)
        var t=FP4(0)
    
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
    
    /* self/=2 */
    mutating func div2() {
        a.div2()
        b.div2()
    }

    mutating func div_i() {
        var u=FP2(a)
        let v=FP2(b)
        u.div_ip()
        a.copy(v)
        b.copy(u)
	if CONFIG_FIELD.TOWER==CONFIG_FIELD.POSITOWER {
		neg()
		norm()
	}
    }   

/* sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b)/2)+ib/(2*sqrt(a+sqrt(a*a-n*b*b)/2)) */
/* returns true if this is QR */
    mutating func sqrt() -> Bool {
        if iszilch() {return true}

        var aa=FP2(a)
        var s=FP2(b)
        var t=FP2(a)

        if s.iszilch() {
            if t.sqrt() {
                a.copy(t)
                b.zero()
            } else {
                t.div_ip()
                _=t.sqrt()
                b.copy(t)
                a.zero()
            }
            return true
        }
        s.sqr()
        a.sqr()
        s.mul_ip()
        s.norm()
        aa.sub(s)

        s.copy(aa)
        if !s.sqrt() {
            return false
        }

        aa.copy(t); aa.add(s); aa.norm(); aa.div2()

        if !aa.sqrt() {
            aa.copy(t); aa.sub(s); aa.norm(); aa.div2()
            if !a.sqrt() {
                return false
            }
        }
        t.copy(b)
        s.copy(aa); s.add(aa)
        s.inverse()

        t.mul(s)
        a.copy(aa)
        b.copy(t)

        return true
    }
}
