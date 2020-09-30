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
//  fp12.swift
//
//  Created by Michael Scott on 07/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* CORE Fp^12 functions */
/* FP12 elements are of the form a+i.b+i^2.c */

public struct FP12
{
    static public let ZERO:Int=0
    static public let ONE:Int=1
    static public let SPARSEST:Int=2    
    static public let SPARSER:Int=3
    static public let SPARSE:Int=4
    static public let DENSE:Int=5

    private var a:FP4
    private var b:FP4
    private var c:FP4
    private var stype:Int
    
    /* reduce all components of this mod Modulus */
    mutating func reduce()
    {
        a.reduce()
        b.reduce()
        c.reduce()
    }
    /* normalise all components of this */
    mutating func norm()
    {
        a.norm()
        b.norm()
        c.norm()
    }

    mutating func settype(_ t:Int)
    {
        stype=t
    }

    mutating func gettype() -> Int
    {
        return stype
    }

    /* Constructors */
    init(_ d:FP4)
    {
        a=FP4(d)
        b=FP4()
        c=FP4()
        stype=FP12.SPARSEST
    }
    
    init()
    {
        a=FP4()
        b=FP4()
        c=FP4()
        stype=FP12.ZERO   
    }

    init(_ d:Int)
    {
        a=FP4(d)
        b=FP4()
        c=FP4()
        if (d==1) {stype=FP12.ONE}
        else {stype=FP12.SPARSEST}
    }
    
    init(_ d:FP4,_ e:FP4,_ f:FP4)
    {
        a=FP4(d)
        b=FP4(e)
        c=FP4(f)
        stype=FP12.DENSE
    }
    
    init(_ x:FP12)
    {
        a=FP4(x.a)
        b=FP4(x.b)
        c=FP4(x.c)
        stype=x.stype
    }
    /* test x==0 ? */
    func iszilch() -> Bool
    {
        return a.iszilch() && b.iszilch() && c.iszilch()
    }

    mutating func cmove(_ g:FP12,_ d:Int)
    {
        a.cmove(g.a,d)
        b.cmove(g.b,d)
        c.cmove(g.c,d)
        let u = ~(d-1)
        stype^=(stype^g.stype)&u   
    }

    /* return 1 if b==c, no branching */
    private static func teq(_ b:Int32,_ c:Int32) -> Int
    {
        var x=b^c
        x-=1  // if x=0, x now -1
        return Int((x>>31)&1)
    }
    /* Constant time select from pre-computed table */
    mutating func select(_ g:[FP12],_ b:Int32)
    {

        let m=b>>31
        var babs=(b^m)-m
        
        babs=(babs-1)/2
    
        cmove(g[0],FP12.teq(babs,0)) // conditional move
        cmove(g[1],FP12.teq(babs,1))
        cmove(g[2],FP12.teq(babs,2))
        cmove(g[3],FP12.teq(babs,3))
        cmove(g[4],FP12.teq(babs,4))
        cmove(g[5],FP12.teq(babs,5))
        cmove(g[6],FP12.teq(babs,6))
        cmove(g[7],FP12.teq(babs,7))
    
        var invf=FP12(self)
        invf.conj()
        cmove(invf,Int(m&1))
    }

    /* test x==1 ? */
    public func isunity() -> Bool
    {
        let one=FP4(1)
        return a.equals(one) && b.iszilch() && c.iszilch()
    }
    /* return 1 if x==y, else 0 */
    public func equals(_ x:FP12) -> Bool
    {
        return a.equals(x.a) && b.equals(x.b) && c.equals(x.c)
    }
    /* extract a from self */
    func geta() -> FP4
    {
        return a
    }
    /* extract b */
    func getb()  -> FP4
    {
        return b
    }
    /* extract c */
    func getc() -> FP4
    {
        return c
    }
    /* copy self=x */
    public mutating func copy(_ x:FP12)
    {
        a.copy(x.a)
        b.copy(x.b)
        c.copy(x.c)
        stype=x.stype
    }
    /* set self=1 */
    mutating func one()
    {
        a.one()
        b.zero()
        c.zero()
        stype=FP12.ONE
    }

    /* set self=0 */
    mutating func zero()
    {
        a.zero()
        b.zero()
        c.zero()
        stype=FP12.ZERO
    }
    /* self=conj(self) */
    mutating func conj()
    {
        a.conj()
        b.nconj()
        c.conj()
    }
    /* Granger-Scott Unitary Squaring */
    mutating func usqr()
    {
        var A=FP4(a)
        var B=FP4(c)
        var C=FP4(b)
        var D=FP4()
    
        a.sqr()
        D.copy(a); D.add(a)
        a.add(D)
    
        a.norm()
        A.nconj()
    
        A.add(A)
        a.add(A)
        B.sqr()
        B.times_i()
    
        D.copy(B); D.add(B)
        B.add(D)
        B.norm()
    
        C.sqr()
        D.copy(C); D.add(C)
        C.add(D)
        C.norm()
    
        b.conj()
        b.add(b)
        c.nconj()
    
        c.add(c)
        b.add(B)
        c.add(C)
        stype=FP12.DENSE
        reduce()
    
    }
    /* Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */
    mutating func sqr()
    {
        if (stype==FP12.ONE) {return}
        var A=FP4(a)
        var B=FP4(b)
        var C=FP4(c)
        var D=FP4(a)
    
        A.sqr()
        B.mul(c)
        B.add(B); B.norm()
        C.sqr()
        D.mul(b)
        D.add(D)

        c.add(a)
        c.add(b); c.norm()
        c.sqr()
    
        a.copy(A)
    
        A.add(B)
        A.norm()
        A.add(C)
        A.add(D)
        A.norm()
    
        A.neg()
        B.times_i()
        C.times_i()
    
        a.add(B)

        b.copy(C); b.add(D)
        c.add(A)
        if (stype==FP12.SPARSER || stype==FP12.SPARSEST) {
            stype=FP12.SPARSE
        } else {
            stype=FP12.DENSE
        }
        norm()
    }
    
    /* FP12 full multiplication this=this*y */
    mutating func mul(_ y:FP12)
    {
        var z0=FP4(a)
        var z1=FP4()
        var z2=FP4(b)
        var z3=FP4()
        var t0=FP4(a)
        var t1=FP4(y.a)
    
        z0.mul(y.a)
        z2.mul(y.b)
    
        t0.add(b)
        t1.add(y.b)

        t0.norm(); t1.norm()
    
        z1.copy(t0); z1.mul(t1)
        t0.copy(b); t0.add(c)
    
        t1.copy(y.b); t1.add(y.c)

        t0.norm(); t1.norm()

        z3.copy(t0); z3.mul(t1)
    
        t0.copy(z0); t0.neg()
        t1.copy(z2); t1.neg()
    
        z1.add(t0)
        b.copy(z1); b.add(t1)
    
        z3.add(t1)
        z2.add(t0)
    
        t0.copy(a); t0.add(c)
        t1.copy(y.a); t1.add(y.c)

        t0.norm(); t1.norm()

        t0.mul(t1)
        z2.add(t0)
    
        t0.copy(c); t0.mul(y.c)
        t1.copy(t0); t1.neg()
    
        c.copy(z2); c.add(t1)
        z3.add(t1)
        t0.times_i()
        b.add(t0)
    
        z3.norm()
        z3.times_i()
        a.copy(z0); a.add(z3)
        stype=FP12.DENSE
        norm()
    }
    
/* FP12 full multiplication w=w*y */
/* Supports sparse multiplicands */
/* Usually w is denser than y */
    mutating func ssmul(_ y:FP12)
    {
        if stype==FP12.ONE {
            copy(y)
            return
        }
        if y.stype==FP12.ONE {
            return
        }
        if y.stype>=FP12.SPARSE {
            var z0=FP4(a)
            var z1=FP4()
            var z2=FP4()
            var z3=FP4()
            z0.mul(y.a)

            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {  
                if y.stype==FP12.SPARSE || stype==FP12.SPARSE {
                    var ga=FP2(0)
                    var gb=FP2(0)

                    gb.copy(b.getb())
                    gb.mul(y.b.getb())
                    ga.zero()
                    if y.stype != FP12.SPARSE {
                        ga.copy(b.getb())
                        ga.mul(y.b.geta())
                    }
                    if stype != FP12.SPARSE {
                        ga.copy(b.geta())
                        ga.mul(y.b.getb())
                    }
                    z2.set_fp2s(ga,gb)
                    z2.times_i()
                } else {
                    z2.copy(b)
                    z2.mul(y.b)
                }
            } else {
                z2.copy(b)
                z2.mul(y.b)
            }
            var t0=FP4(a)
            var t1=FP4(y.a)
            t0.add(b); t0.norm()
            t1.add(y.b); t1.norm()

            z1.copy(t0); z1.mul(t1)
            t0.copy(b); t0.add(c); t0.norm()
            t1.copy(y.b); t1.add(y.c); t1.norm()

            z3.copy(t0); z3.mul(t1)

            t0.copy(z0); t0.neg()
            t1.copy(z2); t1.neg()

            z1.add(t0)
            b.copy(z1); b.add(t1)

            z3.add(t1)
            z2.add(t0)

            t0.copy(a); t0.add(c); t0.norm()
            t1.copy(y.a); t1.add(y.c); t1.norm()
    
            t0.mul(t1)
            z2.add(t0)

            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {  
                if y.stype==FP12.SPARSE || stype==FP12.SPARSE {
                    var ga=FP2(0)
                    var gb=FP2(0)

                    ga.copy(c.geta())
                    ga.mul(y.c.geta())
                    gb.zero()
                    if y.stype != FP12.SPARSE {
                        gb.copy(c.geta())
                        gb.mul(y.c.getb())
                    }
                    if stype != FP12.SPARSE {
                        gb.copy(c.getb())
                        gb.mul(y.c.geta())
                    }
                    t0.set_fp2s(ga,gb)
                } else {
                    t0.copy(c)
                    t0.mul(y.c)
                }
            } else {
                t0.copy(c)
                t0.mul(y.c)
            }
            t1.copy(t0); t1.neg()

            c.copy(z2); c.add(t1)
            z3.add(t1)
            t0.times_i()
            b.add(t0)
            z3.norm()
            z3.times_i()
            a.copy(z0); a.add(z3);
        } else {
            if stype==FP12.SPARSER || stype==FP12.SPARSEST {
                smul(y)
                return
            }
            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {  // dense by sparser - 13m 
                var z0=FP4(a)
                var z2=FP4(b)
                var z3=FP4(b)
                var t0=FP4()
                var t1=FP4(y.a)
                z0.mul(y.a)

                if y.stype==FP12.SPARSEST {
                    z2.qmul(y.b.geta().geta())
                } else {                 
                    z2.pmul(y.b.geta())
                }
                b.add(a)
                t1.adds(y.b.geta())

                t1.norm()
                b.norm()
                b.mul(t1)
                z3.add(c)
                z3.norm()
                if y.stype==FP12.SPARSEST {
                    z3.qmul(y.b.geta().geta())
                } else {                    
                    z3.pmul(y.b.geta())
                }

                t0.copy(z0); t0.neg()
                t1.copy(z2); t1.neg()

                b.add(t0)

                b.add(t1)
                z3.add(t1)
                z2.add(t0)

                t0.copy(a); t0.add(c); t0.norm()
                z3.norm()
                t0.mul(y.a)
                c.copy(z2); c.add(t0)

                z3.times_i()
                a.copy(z0); a.add(z3)
            }
            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {
                var z0=FP4(a)
                var z1=FP4()
                var z2=FP4()
                var z3=FP4()
                var t0=FP4(a)
                var t1=FP4()
        
                z0.mul(y.a)
                t0.add(b); t0.norm()

                z1.copy(t0); z1.mul(y.a)
                t0.copy(b); t0.add(c)
                t0.norm()

                z3.copy(t0)
                if y.stype==FP12.SPARSEST {
                    z3.qmul(y.c.getb().geta())
                } else {                 
                    z3.pmul(y.c.getb())
                }
                z3.times_i()

                t0.copy(z0); t0.neg()
                z1.add(t0)
                b.copy(z1)
                z2.copy(t0)

                t0.copy(a); t0.add(c); t0.norm()
                t1.copy(y.a); t1.add(y.c); t1.norm()

                t0.mul(t1)
                z2.add(t0)
                t0.copy(c)
                if y.stype==FP12.SPARSEST {
                    t0.qmul(y.c.getb().geta())
                } else {             
                    t0.pmul(y.c.getb())
                }
                t0.times_i()
                t1.copy(t0); t1.neg()

                c.copy(z2); c.add(t1)
                z3.add(t1)
                t0.times_i()
                b.add(t0)
                z3.norm()
                z3.times_i()
                a.copy(z0); a.add(z3)
            }   
        }
        stype=FP12.DENSE
        norm()    
    }

    /* Special case of multiplication arises from special form of ATE pairing line function */
    mutating func smul(_ y:FP12)
    {
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {  
            var w1=FP2(a.geta())
            var w2=FP2(a.getb())
            var w3:FP2
            
            w1.mul(y.a.geta())
            w2.mul(y.a.getb())


            if y.stype==FP12.SPARSEST || stype==FP12.SPARSEST {
                if y.stype==FP12.SPARSEST && stype==FP12.SPARSEST {
                    var t=FP(b.geta().geta())
                    t.mul(y.b.geta().geta())
                    w3=FP2(t)
                } else {
                    if y.stype != FP12.SPARSEST {
                        w3=FP2(y.b.geta())
                        w3.pmul(b.geta().geta())
                    } else {
                        w3=FP2(b.geta())
                        w3.pmul(y.b.geta().geta())
                    }
                }            
            } else {
                w3=FP2(b.geta())
                w3.mul(y.b.geta())
            }
            var ta=FP2(a.geta())
            var tb=FP2(y.a.geta())
            ta.add(a.getb()); ta.norm()
            tb.add(y.a.getb()); tb.norm()
            var tc=FP2(ta)
            tc.mul(tb)
            var t=FP2(w1)
            t.add(w2)
            t.neg()
            tc.add(t)

            ta.copy(a.geta()); ta.add(b.geta()); ta.norm()
            tb.copy(y.a.geta()); tb.add(y.b.geta()); tb.norm()
            var td=FP2(ta)
            td.mul(tb)
            t.copy(w1)
            t.add(w3)
            t.neg()
            td.add(t)

            ta.copy(a.getb()); ta.add(b.geta()); ta.norm()
            tb.copy(y.a.getb()); tb.add(y.b.geta()); tb.norm()
            var te=FP2(ta)
            te.mul(tb)
            t.copy(w2)
            t.add(w3)
            t.neg()
            te.add(t)

            w2.mul_ip()
            w1.add(w2)

            a.set_fp2s(w1,tc)
            b.set_fp2s(td,te)
            c.set_fp2(w3)

            a.norm()
            b.norm()
    } else {
            var w1=FP2(a.geta())
            var w2=FP2(a.getb())
            var w3:FP2
 
            w1.mul(y.a.geta())
            w2.mul(y.a.getb())

            if y.stype==FP12.SPARSEST || stype==FP12.SPARSEST {
                if y.stype==FP12.SPARSEST && stype==FP12.SPARSEST {
                    var t=FP(c.getb().geta())
                    t.mul(y.c.getb().geta())
                    w3=FP2(t)
                } else {
                    if y.stype != FP12.SPARSEST {
                        w3=FP2(y.c.getb())
                        w3.pmul(c.getb().geta())
                    } else {
                        w3=FP2(c.getb())
                        w3.pmul(y.c.getb().geta())
                    }
                }    
            } else {
                w3=FP2(c.getb())
                w3.mul(y.c.getb())
            }
            var ta=FP2(a.geta())
            var tb=FP2(y.a.geta())
            ta.add(a.getb()); ta.norm()
            tb.add(y.a.getb()); tb.norm()
            var tc=FP2(ta)
            tc.mul(tb)
            var t=FP2(w1)
            t.add(w2)
            t.neg()
            tc.add(t)

            ta.copy(a.geta()); ta.add(c.getb()); ta.norm()
            tb.copy(y.a.geta()); tb.add(y.c.getb()); tb.norm()
            var td=FP2(ta)
            td.mul(tb)
            t.copy(w1)
            t.add(w3)
            t.neg()
            td.add(t)

            ta.copy(a.getb()); ta.add(c.getb()); ta.norm()
            tb.copy(y.a.getb()); tb.add(y.c.getb()); tb.norm()
            var te=FP2(ta)
            te.mul(tb)
            t.copy(w2)
            t.add(w3)
            t.neg()
            te.add(t)

            w2.mul_ip()
            w1.add(w2)
            a.set_fp2s(w1,tc);

            w3.mul_ip()
            w3.norm()
            b.set_fp2h(w3);

            te.norm()
            te.mul_ip()
            c.set_fp2s(te,td)

            a.norm()
            c.norm()

        }
        stype=FP12.SPARSE
    }
    /* self=1/self */
    mutating func inverse()
    {
        var f0=FP4(a)
        var f1=FP4(b)
        var f2=FP4(a)
        var f3=FP4()
        let pNIL:FP?=nil  
        
        norm()
        f0.sqr()
        f1.mul(c)
        f1.times_i()
        f0.sub(f1); f0.norm()
    
        f1.copy(c); f1.sqr()
        f1.times_i()
        f2.mul(b)
        f1.sub(f2); f1.norm()
    
        f2.copy(b); f2.sqr()
        f3.copy(a); f3.mul(c)
        f2.sub(f3); f2.norm()
    
        f3.copy(b); f3.mul(f2)
        f3.times_i()
        a.mul(f0)
        f3.add(a)
        c.mul(f1)
        c.times_i()
    
        f3.add(c); f3.norm()
        f3.inverse(pNIL)
        a.copy(f0); a.mul(f3)
        b.copy(f1); b.mul(f3)
        c.copy(f2); c.mul(f3)
        stype=FP12.DENSE        
    }
    
    /* self=self^p using Frobenius */
    mutating func frob(_ f:FP2)
    {
        var f2=FP2(f)
        var f3=FP2(f)
    
        f2.sqr()
        f3.mul(f2)
    
        a.frob(f3)
        b.frob(f3)
        c.frob(f3)
    
        b.pmul(f)
        c.pmul(f2)
        stype=FP12.DENSE        
    }
    
    /* trace function */
    func trace() -> FP4
    {
        var t=FP4()
        t.copy(a)
        t.imul(3)
        t.reduce()
        return t
    }
    /* convert from byte array to FP12 */
    static func fromBytes(_ w:[UInt8]) -> FP12
    {
        let RM=4*Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM)
    
	    for i in 0 ..< RM {
		    t[i]=w[i]
	    }
        let c=FP4.fromBytes(t)
	    for i in 0 ..< RM {
		    t[i]=w[i+RM]
	    }
        let b=FP4.fromBytes(t)
	    for i in 0 ..< RM {
		    t[i]=w[i+2*RM]
	    }
        let a=FP4.fromBytes(t)
	    return FP12(a,b,c)
    }
    
    /* convert this to byte array */
    func toBytes(_ w:inout [UInt8])
    {
        let RM=4*Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM)
        c.toBytes(&t)
	    for i in 0 ..< RM { 
		    w[i]=t[i]
	    }
        b.toBytes(&t)
	    for i in 0 ..< RM {
		    w[i+RM]=t[i]
	    }
        a.toBytes(&t)
	    for i in 0 ..< RM {
		    w[i+2*RM]=t[i]
	    }
    }
    /* convert to hex string */
    public func toString() -> String
    {
        return ("["+a.toString()+","+b.toString()+","+c.toString()+"]")
    }
    
    /* self=self^e */
    /* Note this is simple square and multiply, so not side-channel safe */
    func pow(_ e:BIG) -> FP12
    {
        var sf = FP12(self)
        sf.norm()
        //e.norm()
        var e1=BIG(e)
        e1.norm()
        var e3=BIG(e1)
        e3.pmul(3)
        e3.norm();

        var w=FP12(sf)
        if e3.iszilch() {
            w.one()
            return w
        }
        let nb=e3.nbits()
 
        for i in (1...nb-2).reversed()
        {
            w.usqr()
            let bt=e3.bit(UInt(i))-e1.bit(UInt(i))
            if bt == 1 {
                w.mul(sf)
            }
            if bt == -1 {
                sf.conj(); w.mul(sf); sf.conj()
            }            
        }
    
        w.reduce()
        return w
    }
    /* constant time powering by small integer of max length bts */
    mutating func pinpow(_ e:Int32,_ bts:Int32)
    {
        var R=[FP12]()
        R.append(FP12(1))
        R.append(FP12(self))

        for i in (0...bts-1).reversed()
        {
            let b=Int((e>>i)&1)
            R[1-b].mul(R[b])
            R[b].usqr()
        }
        copy(R[0])
    }

    public func compow(_ e :BIG,_ r :BIG) -> FP4
    {
        let f=FP2(BIG(ROM.Fra),BIG(ROM.Frb))
        let q=BIG(ROM.Modulus)       

        var g1=FP12(self)
        var g2=FP12(self)

        var m=BIG(q)
        m.mod(r)
    
        var a=BIG(e)
        a.mod(m)
    
        var b=BIG(e)
        b.div(m);
    
        var c=g1.trace()

        if b.iszilch() 
        {
            c=c.xtr_pow(e)
            return c
        }

        g2.frob(f)
        let cp=g2.trace()
        g1.conj()
        g2.mul(g1)
        let cpm1=g2.trace()
        g2.mul(g1)
        let cpm2=g2.trace()
    
        c=c.xtr_pow2(cp,cpm1,cpm2,a,b)

        return c
    }
    

    /* P=u0.Q0+u1*Q1+u2*Q2+u3*Q3 */
    // Bos & Costello https://eprint.iacr.org/2013/458.pdf
    // Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
    // Side channel attack secure 

    static func pow4(_ q:[FP12],_ u:[BIG]) -> FP12
    {
        var g=[FP12]();
        
        for _ in 0 ..< 8 {g.append(FP12(0))}
        
        var r=FP12(0)
        var p=FP12(0)
        
        var t=[BIG]()
        for i in 0 ..< 4 {
            t.append(BIG(u[i]))
            t[i].norm()
        }

        var mt=BIG(0);
        var w=[Int8](repeating: 0,count: CONFIG_BIG.NLEN*Int(CONFIG_BIG.BASEBITS)+1)           
        var s=[Int8](repeating: 0,count: CONFIG_BIG.NLEN*Int(CONFIG_BIG.BASEBITS)+1)   

// precompute table 
        g[0].copy(q[0])   // q[0]
        g[1].copy(g[0]); g[1].mul(q[1])   // q[0].q[1]
        g[2].copy(g[0]); g[2].mul(q[2])   // q[0].q[2]
        g[3].copy(g[1]); g[3].mul(q[2])   // q[0].q[1].q[2]
        g[4].copy(g[0]); g[4].mul(q[3])   // q[0].q[3]
        g[5].copy(g[1]); g[5].mul(q[3])   // q[0].q[1].q[3]
        g[6].copy(g[2]); g[6].mul(q[3])   // q[0].q[2].q[3]
        g[7].copy(g[3]); g[7].mul(q[3])   // q[0].q[1].q[2].q[3]

// Make it odd
        let pb=1-t[0].parity()
        t[0].inc(pb)
        t[0].norm()  

// Number of bits
        mt.zero();
        for i in 0 ..< 4 {
            mt.or(t[i]); 
        }

        let nb=1+mt.nbits()

// Sign pivot 

        s[nb-1]=1
        for i in 0 ..< nb-1 {
            t[0].fshr(1)
            s[i]=2*Int8(t[0].parity())-1
        }

// Recoded exponent
        for i in 0 ..< nb {
            w[i]=0
            var k=1
            for j in 1 ..< 4 {
                let bt=s[i]*Int8(t[j].parity())
                t[j].fshr(1)
                t[j].dec(Int(bt>>1))
                t[j].norm()
                w[i]+=bt*Int8(k)
                k=2*k
            }
        }   

// Main loop
        p.select(g,Int32(2*w[nb-1]+1))
        for i in (0 ..< nb-1).reversed() {
            p.usqr()
            r.select(g,Int32(2*w[i]+s[i]))
            p.mul(r)
        }      

// apply correction
        r.copy(q[0]); r.conj()   
        r.mul(p)
        p.cmove(r,pb)
        p.reduce()
        return p
    }
}
