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
//  ecp.swift
//
//  Created by Michael Scott on 30/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//


public struct ECP {

    private var x:FP
    private var y:FP
    private var z:FP

   /* Constructor - set to O */
    init()
    {
        x=FP()
        y=FP(1)
        if CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS {
	       z=FP(1)
	   } else {
	       z=FP()
	   }
    }

    /* test for O point-at-infinity */
    public func is_infinity() -> Bool
    {
        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS)
        {
            return x.iszilch() && y.equals(z)
        }
        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.WEIERSTRASS)
        {
            return x.iszilch() && z.iszilch()
        }
        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
        {
            return z.iszilch()
        }
        return true
    }

    /* Conditional swap of P and Q dependant on d */
    private mutating func cswap(_ Q: inout ECP,_ d:Int)
    {
        x.cswap(&(Q.x),d)
        if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY {y.cswap(&(Q.y),d)}
        z.cswap(&(Q.z),d)
    }

    /* Conditional move of Q to P dependant on d */
    private mutating func cmove(_ Q: ECP,_ d:Int)
    {
        x.cmove(Q.x,d)
        if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY {y.cmove(Q.y,d)}
        z.cmove(Q.z,d)
    }

    /* return 1 if b==c, no branching */
    private static func teq(_ b: Int32,_ c:Int32) -> Int
    {
        var x=b^c
        x-=1  // if x=0, x now -1
        return Int((x>>31)&1)
    }

    /* self=P */
    public mutating func copy(_ P: ECP)
    {
        x.copy(P.x)
        if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY {y.copy(P.y)}
        z.copy(P.z)
    }
    /* self=-self */
    public mutating func neg() {
        if (CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.WEIERSTRASS)
        {
            y.neg(); y.norm();
        }
        if (CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.EDWARDS)
        {
            x.neg(); x.norm();
        }
        return;
    }

    /* Constant time select from pre-computed table */
    private mutating func select(_ W:[ECP],_ b:Int32)
    {
        var MP=ECP()
        let m=b>>31
        var babs=(b^m)-m

        babs=(babs-1)/2

        cmove(W[0],ECP.teq(babs,0)); // conditional move
        cmove(W[1],ECP.teq(babs,1))
        cmove(W[2],ECP.teq(babs,2))
        cmove(W[3],ECP.teq(babs,3))
        cmove(W[4],ECP.teq(babs,4))
        cmove(W[5],ECP.teq(babs,5))
        cmove(W[6],ECP.teq(babs,6))
        cmove(W[7],ECP.teq(babs,7))

        MP.copy(self)
        MP.neg()
        cmove(MP,Int(m&1))
    }

    /* Test P == Q */
    func equals(_ Q: ECP) -> Bool
    {
        var a=FP()
        var b=FP()
        a.copy(x); a.mul(Q.z)
        b.copy(Q.x); b.mul(z)
        if !a.equals(b) {return false}
        if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY
        {
			a.copy(y); a.mul(Q.z);
			b.copy(Q.y); b.mul(z);
			if !a.equals(b) {return false}
        }
        return true
    }

    mutating func mulx(_ w: FP)
    {
        x.mul(w)
    }

/* set self=O */
    mutating func inf()
    {
    //    INF=true;
        x.zero()
        if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY {y.one()}
        if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.EDWARDS {z.zero()}
        else {z.one()}
    }

    /* Calculate RHS of curve equation */
    static func RHS(_ x: FP) -> FP
    {
        var r=FP(x)
        let pNIL:FP?=nil
        r.sqr()

        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.WEIERSTRASS
        { // x^3+Ax+B
            let b=FP(BIG(ROM.CURVE_B))
            r.mul(x)
            if (CONFIG_CURVE.CURVE_A == -3)
            {
				var cx=FP(x)
				cx.imul(3)
				cx.neg(); cx.norm()
				r.add(cx)
            }
            r.add(b);
        }
        if (CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.EDWARDS)
        { // (Ax^2-1)/(Bx^2-1)
            var b=FP(BIG(ROM.CURVE_B))

            let one=FP(1);
            b.mul(r);
            b.sub(one); b.norm()
            if CONFIG_CURVE.CURVE_A == -1 {r.neg()}
            r.sub(one); r.norm()
            b.inverse(pNIL)
            r.mul(b);
        }
        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY
        { // x^3+Ax^2+x
            var x3=FP()
            x3.copy(r);
            x3.mul(x);
            r.imul(CONFIG_CURVE.CURVE_A);
            r.add(x3);
            r.add(x);
        }
        r.reduce();
        return r;
    }

    /* set (x,y) from two BIGs */
    public init(_ ix: BIG,_ iy: BIG)
    {
        x=FP(ix)
        y=FP(iy)
        z=FP(1)
        var pNIL:FP?=nil

        x.norm()
        let rhs=ECP.RHS(x);

        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY
        {
            if rhs.qr(&pNIL) != 1 {inf()}
        }
        else
        {
            var y2=FP(y)
            y2.sqr()
            if !y2.equals(rhs) {inf()}
        }
    }

    /* set (x,y) from BIG and a bit */
    public init(_ ix: BIG,_ s:Int)
    {
        x=FP(ix)
        x.norm()
        let rhs=ECP.RHS(x)
        y=FP()
        z=FP(1)
        var hint:FP?=FP()
        if rhs.qr(&hint)==1
        {
            var ny=rhs.sqrt(hint)
            if (ny.sign() != s) {ny.neg(); ny.norm()}
            y.copy(ny)
        }
        else {inf()}
    }

    /* set from x - calculate y from curve equation */
    public init(_ ix:BIG)
    {
        x=FP(ix)
        x.norm()
        let rhs=ECP.RHS(x)
        y=FP()
        z=FP(1)
        var hint:FP?=FP()
        if rhs.qr(&hint)==1
        {
            if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY {y.copy(rhs.sqrt(hint))}
        }
        else {inf()}
    }

    /* set to affine - from (x,y,z) to (x,y) */
    mutating public func affine()
    {
        let pNIL:FP?=nil
        if is_infinity() {return}
        let one=FP(1)
        if (z.equals(one)) {
            x.reduce(); y.reduce()
            return
        }
        z.inverse(pNIL)

        x.mul(z); x.reduce()
        if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY
        {
            y.mul(z); y.reduce()

        }
        z.copy(one)
    }
    /* extract x as a BIG */
    func getX() -> BIG
    {
        var W=ECP(); W.copy(self)
        W.affine()
        return W.x.redc()
    }
    /* extract y as a BIG */
    func getY() -> BIG
    {
        var W=ECP(); W.copy(self)
        W.affine();
        return W.y.redc();
    }

    /* get sign of Y */
    func getS() -> Int
    {
        var W=ECP(); W.copy(self)
        W.affine();
        return W.y.sign();
    }
    /* extract x as an FP */
    func getx() -> FP
    {
        return x;
    }
    /* extract y as an FP */
    func gety() -> FP
    {
        return y;
    }
    /* extract z as an FP */
    func getz() -> FP
    {
        return z;
    }
    /* convert to byte array */
    func toBytes(_ b:inout [UInt8],_ compress: Bool)
    {
        let RM=Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM)
        var alt=false
        var W=ECP(); W.copy(self)
        W.affine()
        W.x.redc().toBytes(&t)

        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY {
            for i in 0 ..< RM {b[i]=t[i]}
		    //b[0]=0x06
		    return
        }

        if (CONFIG_FIELD.MODBITS-1)%8 <= 4 && CONFIG_CURVE.ALLOW_ALT_COMPRESS {
            alt=true
        }
        if alt {
            for i in 0 ..< RM {
			    b[i]=t[i]
		    }
            if compress {
                b[0]|=0x80
                if W.y.islarger()==1 {
				    b[0]|=0x20
			    }
            } else {
                W.y.redc().toBytes(&t)
                for i in 0 ..< RM {
				    b[i+RM]=t[i]
			    }
		    }		
        } else {
            for i in 0 ..< RM {b[i+1]=t[i]}
	        if compress {
		        b[0]=0x02
		        if W.y.sign()==1 {b[0]=0x03}
		        return
	        }
	        b[0]=0x04
            W.y.redc().toBytes(&t);
            for i in 0 ..< RM {b[i+RM+1]=t[i]}
        }
    }

    /* convert from byte array to point */
    static func fromBytes(_ b: [UInt8]) -> ECP
    {
        let RM=Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM)
        var alt=false
        let p=BIG(ROM.Modulus);

        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY {
            for i in 0 ..< RM {t[i]=b[i]}
            let px=BIG.fromBytes(t)
            if BIG.comp(px,p)>=0 {return ECP()}
		    return ECP(px)
	    }

        if (CONFIG_FIELD.MODBITS-1)%8 <= 4 && CONFIG_CURVE.ALLOW_ALT_COMPRESS {
            alt=true
        }

        if alt {
            for i in 0 ..< RM {
			    t[i]=b[i]
		    }
            t[0]&=0x1f
            let px=BIG.fromBytes(t)
            if (b[0]&0x80)==0 {
                for i in 0 ..< RM {
				    t[i]=b[i+RM]
			    }
                let py=BIG.fromBytes(t)
                return ECP(px,py)
            } else {
                let sgn=(b[0]&20)>>5
                var P=ECP(px,0)
                let cmp=P.y.islarger()
                if (sgn == 1 && cmp != 1) || (sgn == 0 && cmp == 1) {
				    P.neg()
			    }
                return P
            }
        } else {
            for i in 0 ..< RM {t[i]=b[i+1]}
            let px=BIG.fromBytes(t)
            if BIG.comp(px,p)>=0 {return ECP()}

            if b[0]==0x04 {
                for i in 0 ..< RM {t[i]=b[i+RM+1]}
                let py=BIG.fromBytes(t)
                if BIG.comp(py,p)>=0 {return ECP()}
                return ECP(px,py)
            }

	        if b[0]==0x02 || b[0]==0x03 {
	            return ECP(px,Int(b[0]&1))
	        }
        }
	    return ECP()
    }
    /* convert to hex string */
    public func toString() -> String
    {
        var W=ECP(); W.copy(self)
        if W.is_infinity() {return "infinity"}
        W.affine();
        if CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY {return "("+W.x.redc().toString()+")"}
        else {return "("+W.x.redc().toString()+","+W.y.redc().toString()+")"}
    }

    /* self*=2 */
    mutating func dbl()
    {
        if (CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.WEIERSTRASS)
        {

            if CONFIG_CURVE.CURVE_A == 0
            {
                var t0=FP(y)
                t0.sqr()
                var t1=FP(y)
                t1.mul(z);
                var t2=FP(z)
                t2.sqr()

                z.copy(t0)
                z.add(t0); z.norm()
                z.add(z); z.add(z); z.norm()
                t2.imul(3*ROM.CURVE_B_I)

                var x3=FP(t2)
                x3.mul(z)

                var y3=FP(t0)
                y3.add(t2); y3.norm()
                z.mul(t1)
                t1.copy(t2); t1.add(t2); t2.add(t1)
                t0.sub(t2); t0.norm(); y3.mul(t0); y3.add(x3)
                t1.copy(x); t1.mul(y)
                x.copy(t0); x.norm(); x.mul(t1); x.add(x)
                x.norm()
                y.copy(y3); y.norm()
            }
            else {
                var t0=FP(x)
                var t1=FP(y)
                var t2=FP(z)
                var t3=FP(x)
                var z3=FP(z)
                var y3=FP()
                var x3=FP()
                var b=FP()

                if ROM.CURVE_B_I==0
                {
                    b.copy(FP(BIG(ROM.CURVE_B)))
                }

                t0.sqr()  //1    x^2
                t1.sqr()  //2    y^2
                t2.sqr()  //3

                t3.mul(y) //4
                t3.add(t3); t3.norm()//5
                z3.mul(x)   //6
                z3.add(z3);  z3.norm()//7
                y3.copy(t2)

                if ROM.CURVE_B_I==0 {
                    y3.mul(b) //8
                }
                else {
                    y3.imul(ROM.CURVE_B_I)
                }

                y3.sub(z3)  //9  ***
                x3.copy(y3); x3.add(y3); x3.norm()//10

                y3.add(x3) //11
                x3.copy(t1); x3.sub(y3); x3.norm()//12
                y3.add(t1); y3.norm()//13
                y3.mul(x3) //14
                x3.mul(t3) //15
                t3.copy(t2); t3.add(t2)  //16
                t2.add(t3) //t2.norm(); //17

                if ROM.CURVE_B_I==0 {
                    z3.mul(b) //18
                }
                else {
                    z3.imul(ROM.CURVE_B_I)
                }

                z3.sub(t2) //19
                z3.sub(t0); z3.norm()//20  ***
                t3.copy(z3); t3.add(z3) //21

                z3.add(t3); z3.norm() //22
                t3.copy(t0); t3.add(t0)  //23
                t0.add(t3) //24
                t0.sub(t2); t0.norm()//25

                t0.mul(z3)//26
                y3.add(t0) //27
                t0.copy(y); t0.mul(z)//28
                t0.add(t0); t0.norm() //29
                z3.mul(t0)//30
                x3.sub(z3) //31
                t0.add(t0); t0.norm()//32
                t1.add(t1); t1.norm()//33
                z3.copy(t0); z3.mul(t1)//34

                x.copy(x3); x.norm()
                y.copy(y3); y.norm()
                z.copy(z3); z.norm()
            }
        }
        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.EDWARDS
        {
            var C=FP(x)
            var D=FP(y)
            var H=FP(z)
            var J=FP()

            x.mul(y); x.add(x); x.norm()
            C.sqr()
            D.sqr()
            if CONFIG_CURVE.CURVE_A == -1 {C.neg()}
            y.copy(C); y.add(D); y.norm()
            H.sqr(); H.add(H)
            z.copy(y)
            J.copy(y); J.sub(H); J.norm()
            x.mul(J)
            C.sub(D); C.norm()
            y.mul(C)
            z.mul(J)

        }
        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY
        {
            var A=FP(x)
            var B=FP(x);
            var AA=FP();
            var BB=FP();
            var C=FP();

            A.add(z); A.norm()
            AA.copy(A); AA.sqr()
            B.sub(z); B.norm()
            BB.copy(B); BB.sqr()
            C.copy(AA); C.sub(BB); C.norm()

            x.copy(AA); x.mul(BB)

            A.copy(C); A.imul((CONFIG_CURVE.CURVE_A+2)/4)

            BB.add(A); BB.norm()
            z.copy(BB); z.mul(C)
        }
        return
    }

    /* self+=Q */
    mutating public func add(_ Q:ECP)
    {

        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.WEIERSTRASS
        {

                if CONFIG_CURVE.CURVE_A == 0
                {
                    let b=3*ROM.CURVE_B_I
                    var t0=FP(x)
                    t0.mul(Q.x)
                    var t1=FP(y)
                    t1.mul(Q.y)
                    var t2=FP(z)
                    t2.mul(Q.z)
                    var t3=FP(x)
                    t3.add(y); t3.norm()
                    var t4=FP(Q.x)
                    t4.add(Q.y); t4.norm()
                    t3.mul(t4)
                    t4.copy(t0); t4.add(t1)

                    t3.sub(t4); t3.norm()
                    t4.copy(y)
                    t4.add(z); t4.norm()
                    var x3=FP(Q.y)
                    x3.add(Q.z); x3.norm()

                    t4.mul(x3)
                    x3.copy(t1)
                    x3.add(t2)

                    t4.sub(x3); t4.norm()
                    x3.copy(x); x3.add(z); x3.norm()
                    var y3=FP(Q.x)
                    y3.add(Q.z); y3.norm()
                    x3.mul(y3)
                    y3.copy(t0)
                    y3.add(t2)
                    y3.rsub(x3); y3.norm()
                    x3.copy(t0); x3.add(t0)
                    t0.add(x3); t0.norm()
                    t2.imul(b);

                    var z3=FP(t1); z3.add(t2); z3.norm()
                    t1.sub(t2); t1.norm()
                    y3.imul(b)

                    x3.copy(y3); x3.mul(t4); t2.copy(t3); t2.mul(t1); x3.rsub(t2)
                    y3.mul(t0); t1.mul(z3); y3.add(t1)
                    t0.mul(t3); z3.mul(t4); z3.add(t0)

                    x.copy(x3); x.norm()
                    y.copy(y3); y.norm()
                    z.copy(z3); z.norm()
                }
                else {

                    var t0=FP(x)
                    var t1=FP(y)
                    var t2=FP(z)
                    var t3=FP(x)
                    var t4=FP(Q.x)
                    var z3=FP()
                    var y3=FP(Q.x)
                    var x3=FP(Q.y)
                    var b=FP()

                    if ROM.CURVE_B_I==0
                    {
                        b.copy(FP(BIG(ROM.CURVE_B)))
                    }

                    t0.mul(Q.x) //1
                    t1.mul(Q.y) //2
                    t2.mul(Q.z) //3

                    t3.add(y); t3.norm() //4
                    t4.add(Q.y); t4.norm()//5
                    t3.mul(t4)//6
                    t4.copy(t0); t4.add(t1)  //7
                    t3.sub(t4); t3.norm() //8
                    t4.copy(y); t4.add(z); t4.norm()//9
                    x3.add(Q.z); x3.norm()//10
                    t4.mul(x3) //11
                    x3.copy(t1); x3.add(t2) //12

                    t4.sub(x3); t4.norm()//13
                    x3.copy(x); x3.add(z); x3.norm() //14
                    y3.add(Q.z); y3.norm()//15

                    x3.mul(y3) //16
                    y3.copy(t0); y3.add(t2) //17

                    y3.rsub(x3); y3.norm() //18
                    z3.copy(t2)


                    if ROM.CURVE_B_I==0
                    {
                        z3.mul(b) //18
                    }
                    else {
                        z3.imul(ROM.CURVE_B_I)
                    }

                    x3.copy(y3); x3.sub(z3); x3.norm() //20
                    z3.copy(x3); z3.add(x3)  //21

                    x3.add(z3)  //22
                    z3.copy(t1); z3.sub(x3); z3.norm() //23
                    x3.add(t1); x3.norm() //24

                    if ROM.CURVE_B_I==0
                    {
                        y3.mul(b) //18
                    }
                    else {
                        y3.imul(ROM.CURVE_B_I)
                    }

                    t1.copy(t2); t1.add(t2) //26
                    t2.add(t1) //27

                    y3.sub(t2) //28

                    y3.sub(t0); y3.norm() //29
                    t1.copy(y3); t1.add(y3) //30
                    y3.add(t1); y3.norm() //31

                    t1.copy(t0); t1.add(t0) //32
                    t0.add(t1) //33
                    t0.sub(t2); t0.norm()//34
                    t1.copy(t4); t1.mul(y3)//35
                    t2.copy(t0); t2.mul(y3)//36
                    y3.copy(x3); y3.mul(z3)//37
                    y3.add(t2) //y3.norm();//38
                    x3.mul(t3)//39
                    x3.sub(t1)//40
                    z3.mul(t4)//41
                    t1.copy(t3); t1.mul(t0)//42
                    z3.add(t1)
                    x.copy(x3); x.norm()
                    y.copy(y3); y.norm()
                    z.copy(z3); z.norm()
                }
        }
        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.EDWARDS
        {
            let b=FP(BIG(ROM.CURVE_B))
            var A=FP(z)
            var B=FP()
            var C=FP(x)
            var D=FP(y)
            var E=FP()
            var F=FP()
            var G=FP()

            A.mul(Q.z)
            B.copy(A); B.sqr()
            C.mul(Q.x)
            D.mul(Q.y)

            E.copy(C); E.mul(D); E.mul(b)
            F.copy(B); F.sub(E)
            G.copy(B); G.add(E)

            if CONFIG_CURVE.CURVE_A==1
            {
				E.copy(D); E.sub(C)
            }
            C.add(D)

            B.copy(x); B.add(y)
            D.copy(Q.x); D.add(Q.y)
            B.norm(); D.norm()
            B.mul(D)
            B.sub(C); B.norm(); F.norm()
            B.mul(F)
            x.copy(A); x.mul(B)
            G.norm()
            if CONFIG_CURVE.CURVE_A==1
            {
				E.norm(); C.copy(E); C.mul(G)
            }
            if CONFIG_CURVE.CURVE_A == -1
            {
				C.norm(); C.mul(G)
            }
            y.copy(A); y.mul(C)
            z.copy(F); z.mul(G)

        }
        return;
    }

    /* Differential Add for Montgomery curves. self+=Q where W is self-Q and is affine. */
    mutating func dadd(_ Q:ECP,_ W:ECP)
    {
        var A=FP(x)
        var B=FP(x)
        var C=FP(Q.x)
        var D=FP(Q.x)
        var DA=FP()
        var CB=FP()

        A.add(z)
        B.sub(z)

        C.add(Q.z)
        D.sub(Q.z)
        A.norm()

        D.norm()
        DA.copy(D); DA.mul(A)

        C.norm();
        B.norm();
        CB.copy(C); CB.mul(B)

        A.copy(DA); A.add(CB); A.norm(); A.sqr()
        B.copy(DA); B.sub(CB); B.norm(); B.sqr()

        x.copy(A)
        z.copy(W.x); z.mul(B)

    }
    /* this-=Q */
    mutating func sub(_ Q:ECP)
    {
        var NQ=ECP(); NQ.copy(Q)
        NQ.neg()
        add(NQ)
    }

    /* constant time multiply by small integer of length bts - use ladder */
    func pinmul(_ e:Int32,_ bts:Int32) -> ECP
    {
        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY
            {return self.mul(BIG(Int(e)))}
        else
        {
            var P=ECP()
            var R0=ECP()
            var R1=ECP(); R1.copy(self)

            for i in (0...bts-1).reversed()
            {
				let b=Int(e>>i)&1;
				P.copy(R1);
				P.add(R0);
				R0.cswap(&R1,b);
				R1.copy(P);
				R0.dbl();
				R0.cswap(&R1,b);
            }
            P.copy(R0);
            return P;
        }
    }

    /* return e.self */

    public func mul(_ e:BIG) -> ECP
    {
        if (e.iszilch() || is_infinity()) {return ECP()}

        var P=ECP()
        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY
        {
            /* use Ladder */
            var D=ECP()
            var R0=ECP(); R0.copy(self)
            var R1=ECP(); R1.copy(self)
            R1.dbl()
            D.copy(self); D.affine()
            let nb=e.nbits()

            for i in (0...nb-2).reversed()
            {
				let b=e.bit(UInt(i))
				P.copy(R1)
				P.dadd(R0,D)
				R0.cswap(&R1,b)
				R1.copy(P)
				R0.dbl()
				R0.cswap(&R1,b)
            }
            P.copy(R0)
        }
        else
        {
    // fixed size windows
            var mt=BIG()
            var t=BIG()
            var Q=ECP()
            var C=ECP()
            var W=[ECP]()
            let n=1+(CONFIG_BIG.NLEN*Int(CONFIG_BIG.BASEBITS)+3)/4
            var w=[Int8](repeating: 0,count: n)

    // precompute table
            Q.copy(self)
            Q.dbl()
            W.append(ECP())

            W[0].copy(self)

            for i in 1 ..< 8
            {
                W.append(ECP())
				W[i].copy(W[i-1])
				W[i].add(Q)
            }

    // make exponent odd - add 2P if even, P if odd
            t.copy(e);
            let s=t.parity();
            t.inc(1); t.norm(); let ns=t.parity();
            mt.copy(t); mt.inc(1); mt.norm();
            t.cmove(mt,s);
            Q.cmove(self,ns);
            C.copy(Q);

            let nb=1+(t.nbits()+3)/4;

    // convert exponent to signed 4-bit window
            for i in 0 ..< nb
            {
				w[i]=Int8(t.lastbits(5)-16);
				t.dec(Int(w[i]));
                t.norm();
				t.fshr(4);
            }
            w[nb]=Int8(t.lastbits(5))

            P.copy(W[Int((w[nb])-1)/2]);
            for i in (0...nb-1).reversed()
            {
				Q.select(W,Int32(w[i]));
				P.dbl();
				P.dbl();
				P.dbl();
				P.dbl();
				P.add(Q);
            }
            P.sub(C); /* apply correction */
        }
        return P;
    }

// Generic multi-multiplication, fixed 4-bit window, P=Sigma e_i*X_i
    static public func muln(_ n:Int,_ X:[ECP],_ e:[BIG]) -> ECP
    {
        var mt=BIG()    
        var t=BIG()  
        var P=ECP()
        var S=ECP()
        var R=ECP()
        var B=[ECP]()
        for _ in 0 ..< 16 {B.append(ECP())}

        mt.copy(e[0]); mt.norm()
        for i in 0 ..< n { // find biggest
            t.copy(e[i]); t.norm()
            let k=BIG.comp(t,mt)
            mt.cmove(t,(k+1)/2)
        }
        let nb=(mt.nbits()+3)/4
        for i in (0...nb-1).reversed() {
            for j in 0 ..< 16 {
                B[j].inf()
            }
            for j in 0 ..< n { 
                mt.copy(e[j]); mt.norm()
                mt.shr(UInt(i*4))
                let k=mt.lastbits(4)
                B[k].add(X[j])
            }
            R.inf(); S.inf()
            for j in (0...15).reversed() {
                R.add(B[j])
                S.add(R)
            }
            for _ in 0 ..< 4 {
                P.dbl()
            }
            P.add(S)
        }
        return P
    }

    /* Return e.this+f.Q */

    public func mul2(_ e:BIG,_ Q:ECP,_ f:BIG) -> ECP
    {
        var te=BIG()
        var tf=BIG()
        var mt=BIG()
        var S=ECP()
        var T=ECP()
        var C=ECP()
        var W=[ECP]()
        let n=1+(CONFIG_BIG.NLEN*Int(CONFIG_BIG.BASEBITS)+1)/2
        var w=[Int8](repeating: 0,count: n);

        te.copy(e);
        tf.copy(f);

    // precompute table
        for _ in 0 ..< 8 {W.append(ECP())}
        W[1].copy(self); W[1].sub(Q)
        W[2].copy(self); W[2].add(Q)
        S.copy(Q); S.dbl();
        W[0].copy(W[1]); W[0].sub(S)
        W[3].copy(W[2]); W[3].add(S)
        T.copy(self); T.dbl()
        W[5].copy(W[1]); W[5].add(T)
        W[6].copy(W[2]); W[6].add(T)
        W[4].copy(W[5]); W[4].sub(S)
        W[7].copy(W[6]); W[7].add(S)


    // if multiplier is odd, add 2, else add 1 to multiplier, and add 2P or P to correction

        var s=te.parity()
        te.inc(1); te.norm(); var ns=te.parity(); mt.copy(te); mt.inc(1); mt.norm()
        te.cmove(mt,s)
        T.cmove(self,ns)
        C.copy(T)

        s=tf.parity()
        tf.inc(1); tf.norm(); ns=tf.parity(); mt.copy(tf); mt.inc(1); mt.norm()
        tf.cmove(mt,s)
        S.cmove(Q,ns)
        C.add(S)

        mt.copy(te); mt.add(tf); mt.norm()
        let nb=1+(mt.nbits()+1)/2

    // convert exponent to signed 2-bit window
        for i in 0 ..< nb
        {
            let a=(te.lastbits(3)-4);
            te.dec(a); te.norm();
            te.fshr(2);
            let b=(tf.lastbits(3)-4);
            tf.dec(b); tf.norm();
            tf.fshr(2);
            w[i]=Int8(4*a+b);
        }
        w[nb]=Int8(4*te.lastbits(3)+tf.lastbits(3));
        S.copy(W[Int(w[nb]-1)/2]);
        for i in (0...nb-1).reversed()
        {
            T.select(W,Int32(w[i]));
            S.dbl();
            S.dbl();
            S.add(T);
        }
        S.sub(C); /* apply correction */
        return S;
    }

    mutating public func cfp()
    {

	    let cf=ROM.CURVE_Cof_I;
	    if cf==1 {return}
	    if cf==4 {
		    dbl(); dbl()
		    return
	    }
	    if cf==8 {
		    dbl(); dbl(); dbl()
		    return;
	    }
	    let c=BIG(ROM.CURVE_Cof);
	    copy(mul(c));

    }

/* Hunt and Peck a BIG to a curve point */
    static public func hap2point(_ h: BIG) -> ECP {
        var P=ECP()
        var x=BIG(h)
        while (true) {
            if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY {
                P.copy(ECP(x,0))
            } else {
                P.copy(ECP(x))
            }
            x.inc(1); x.norm();
            if !P.is_infinity() {break}
        }
        return P
    }

/* Constant time Map to Point */
    static public func map2point(_ h: FP) -> ECP {
        var P=ECP()
//        var pNIL:FP?=nil
        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.MONTGOMERY {
// Elligator 2
            var X1=FP()
            var X2=FP()
            var t=FP(h)
            var w=FP()
            let one=FP(1)
            let A=FP(CONFIG_CURVE.CURVE_A)
            var hint:FP?=FP()
            var D=FP()
            var N=FP()

            t.sqr()

            if CONFIG_FIELD.PM1D2 == 2 {
                t.add(t)
            }
            if CONFIG_FIELD.PM1D2 == 1 {
                t.neg()
            }
            if CONFIG_FIELD.PM1D2 > 2 {
                t.imul(CONFIG_FIELD.QNRI)
            }

            t.norm()
            D.copy(t); D.add(one); D.norm()

            X1.copy(A)
            X1.neg(); X1.norm()
            X2.copy(X1)
            X2.mul(t)

            w.copy(X1); w.sqr(); N.copy(w); N.mul(X1)
            w.mul(A); w.mul(D); N.add(w)
            t.copy(D); t.sqr()
            t.mul(X1)
            N.add(t); N.norm()

            t.copy(N); t.mul(D)
            let qres=t.qr(&hint)
            w.copy(t); w.inverse(hint)
            D.copy(w); D.mul(N)
            X1.mul(D)
            X2.mul(D)
            X1.cmove(X2,1-qres)

            let a=X1.redc()
            P.copy(ECP(a))

        }

        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.EDWARDS {
// Elligator 2 - map to Montgomery, place point, map back
            var X1=FP()
            var X2=FP()
            var t=FP(h)
            var w=FP()
            let one=FP(1)
            var A=FP()
            var w1=FP()
            var w2=FP()
            var B=FP(BIG(ROM.CURVE_B))
            var Y=FP()
            var K=FP()
            var D=FP()
            var hint:FP?=FP()
            //var Y3=FP()
            var rfc=0

            if CONFIG_FIELD.MODTYPE != CONFIG_FIELD.GENERALISED_MERSENNE {
                A.copy(B)
                if CONFIG_CURVE.CURVE_A==1 {
                    A.add(one)
                    B.sub(one)
                } else {
                    A.sub(one)
                    B.add(one)
                }
                A.norm(); B.norm()

                A.div2()
                B.div2()
                B.div2()

                K.copy(B)
                K.neg(); K.norm()
                //K.inverse(pNIL)
                K.invsqrt(&K,&w1);

                rfc=CONFIG_FIELD.RIADZ

                if rfc==1 { // RFC7748
                    A.mul(K)
                    K.mul(w1)
                    //K=K.sqrt(pNIL)
                } else {
                    B.sqr()
                }
            } else {
                rfc=1
                A.copy(FP(156326))
            }

            t.sqr()
            var qnr=0
            if CONFIG_FIELD.PM1D2 == 2 {
                t.add(t)
                qnr = 2
            }
            if CONFIG_FIELD.PM1D2 == 1 {
                t.neg()
                qnr = -1
            }
            if CONFIG_FIELD.PM1D2 > 2 {
                t.imul(CONFIG_FIELD.QNRI)
                qnr = CONFIG_FIELD.QNRI
            }
            t.norm()

            D.copy(t); D.add(one); D.norm()
            X1.copy(A)
            X1.neg(); X1.norm()
            X2.copy(X1); X2.mul(t)

// Figure out RHS of Montgomery curve in rational form gx1/d^3

            w.copy(X1); w.sqr(); w1.copy(w); w1.mul(X1)
            w.mul(A); w.mul(D); w1.add(w)
            w2.copy(D); w2.sqr()

            if rfc==0 {
                w.copy(X1); w.mul(B)
                w2.mul(w)
                w1.add(w2)
            } else {
                w2.mul(X1)
                w1.add(w2)
            }
            w1.norm()

            B.copy(w1); B.mul(D)
            let qres=B.qr(&hint)
            w.copy(B); w.inverse(hint)
            D.copy(w); D.mul(w1)
            X1.mul(D)
            X2.mul(D)
            D.sqr()

            w1.copy(B); w1.imul(qnr)
            w.copy(FP(BIG(ROM.CURVE_HTPC)))
            w.mul(hint!)
            w2.copy(D); w2.mul(h)

            X1.cmove(X2,1-qres)
            B.cmove(w1,1-qres)
            hint!.cmove(w,1-qres)
            D.cmove(w2,1-qres)

            Y.copy(B.sqrt(hint))
            Y.mul(D);

/*
            Y.copy(B.sqrt(hint))
            Y.mul(D)

            B.imul(qnr)
            w.copy(FP(BIG(ROM.CURVE_HTPC)))
            hint!.mul(w)

            Y3.copy(B.sqrt(hint))
            D.mul(h)
            Y3.mul(D)

            X1.cmove(X2,1-qres)
            Y.cmove(Y3,1-qres)
*/
            w.copy(Y); w.neg(); w.norm()
            Y.cmove(w,qres^Y.sign())

            if rfc==0 {
                X1.mul(K)
                Y.mul(K)
            }

            if CONFIG_FIELD.MODTYPE == CONFIG_FIELD.GENERALISED_MERSENNE {
				t.copy(X1); t.sqr()
				w.copy(t); w.add(one); w.norm()
				t.sub(one); t.norm()
				w1.copy(t); w1.mul(Y)
				w1.add(w1); X2.copy(w1); X2.add(w1); X2.norm()
				t.sqr()
				Y.sqr(); Y.add(Y); Y.add(Y); Y.norm()
				B.copy(t); B.add(Y); B.norm()

				w2.copy(Y); w2.sub(t); w2.norm()
				w2.mul(X1)
				t.mul(X1)
				Y.div2()
				w1.copy(Y); w1.mul(w)
				w1.rsub(t); w1.norm()
 
				t.copy(X2); t.mul(w1)
				P.x.copy(t)
				t.copy(w2); t.mul(B)
				P.y.copy(t)
				t.copy(w1); t.mul(B)
				P.z.copy(t)

				return P;
            } else {
                w1.copy(X1); w1.add(one); w1.norm()
                w2.copy(X1); w2.sub(one); w2.norm()
                t.copy(w1); t.mul(Y)
                X1.mul(w1)

                if rfc==1 {
                    X1.mul(K)
			    }
                Y.mul(w2)
                P.x.copy(X1)
                P.y.copy(Y)
                P.z.copy(t)

                return P		 
            }
        }

        if CONFIG_CURVE.CURVETYPE == CONFIG_CURVE.WEIERSTRASS {
// swu method
            var A=FP()
            var B=FP()
            var X1=FP()
            var X2=FP()
            var X3=FP()
            let one=FP(1)
            var Y=FP()
            var D=FP()
            var t=FP(h)
            var w=FP()
            var D2=FP()
            var hint:FP?=FP()
            var GX1=FP()
            //var Y3=FP()
            let sgn=t.sign()

            if CONFIG_CURVE.CURVE_A != 0 || CONFIG_CURVE.HTC_ISO != 0
            {
                if CONFIG_CURVE.HTC_ISO != 0 {
/* CAHCZS
                    A.copy(FP(BIG(ROM.CURVE_Ad)))
                    B.copy(FP(BIG(ROM.CURVE_Bd)))
CAHCZF */
                } else {
                    A.copy(FP(CONFIG_CURVE.CURVE_A))
                    B.copy(FP(BIG(ROM.CURVE_B)))
                }
                // SSWU method
                t.sqr()
                t.imul(CONFIG_FIELD.RIADZ)
                w.copy(t); w.add(one); w.norm()

                w.mul(t); D.copy(A)
                D.mul(w)
           
                w.add(one); w.norm()
                w.mul(B)
                w.neg(); w.norm()

                X2.copy(w); 
                X3.copy(t); X3.mul(X2)

// x^3+Ad^2x+Bd^3
                GX1.copy(X2); GX1.sqr(); D2.copy(D)
                D2.sqr(); w.copy(A); w.mul(D2); GX1.add(w); GX1.norm(); GX1.mul(X2); D2.mul(D); w.copy(B); w.mul(D2); GX1.add(w); GX1.norm()

                w.copy(GX1); w.mul(D)
                let qr=w.qr(&hint)
                D.copy(w); D.inverse(hint)
                D.mul(GX1)
                X2.mul(D)
                X3.mul(D)
                t.mul(h)
                D2.copy(D); D2.sqr()

                D.copy(D2); D.mul(t)
                t.copy(w); t.imul(CONFIG_FIELD.RIADZ)
                X1.copy(FP(BIG(ROM.CURVE_HTPC)))
                X1.mul(hint!)

                X2.cmove(X3,1-qr)
                D2.cmove(D,1-qr)
                w.cmove(t,1-qr)
                hint!.cmove(X1,1-qr)

                Y.copy(w.sqrt(hint))
                Y.mul(D2)
/*
                Y.copy(w.sqrt(hint))
                Y.mul(D2)

                D2.mul(t)
                w.imul(CONFIG_FIELD.RIADZ)

                X1.copy(FP(BIG(ROM.CURVE_HTPC)))
                hint!.mul(X1)
                
                Y3.copy(w.sqrt(hint))
                Y3.mul(D2)

                X2.cmove(X3,1-qr)
                Y.cmove(Y3,1-qr)
*/
                let ne=Y.sign()^sgn
                w.copy(Y); w.neg(); w.norm()
                Y.cmove(w,ne)

                if CONFIG_CURVE.HTC_ISO != 0 {
/* CAHCZS
                    var k=0
                    let isox=CONFIG_CURVE.HTC_ISO
                    let isoy=3*(isox-1)/2
                //xnum
                    var xnum=FP(BIG(ROM.PC[k])); k+=1
                    for _ in 0..<isox {
                        xnum.mul(X2)
                        w.copy(FP(BIG(ROM.PC[k]))); k+=1
                        xnum.add(w); xnum.norm()
                    }
                //xden
                    var xden=FP(X2)
                    w.copy(FP(BIG(ROM.PC[k]))); k+=1
                    xden.add(w); xden.norm();
                    for _ in 0..<isox-2 {
                        xden.mul(X2)
                        w.copy(FP(BIG(ROM.PC[k]))); k+=1
                        xden.add(w); xden.norm()
                    }
                //ynum
                    var ynum=FP(BIG(ROM.PC[k])); k+=1
                    for _ in 0..<isoy {
                        ynum.mul(X2)
                        w.copy(FP(BIG(ROM.PC[k]))); k+=1
                        ynum.add(w); ynum.norm()
                    }
                //yden
                    var yden=FP(X2)
                    w.copy(FP(BIG(ROM.PC[k]))); k+=1
                    yden.add(w); yden.norm();
                    for _ in 0..<isoy-1 {
                        yden.mul(X2)
                        w.copy(FP(BIG(ROM.PC[k]))); k+=1
                        yden.add(w); yden.norm()
                    }
                    ynum.mul(Y)
					w.copy(xnum); w.mul(yden)
					P.x.copy(w)
					w.copy(ynum); w.mul(xden)
					P.y.copy(w)
					w.copy(xden); w.mul(yden)
					P.z.copy(w)
					return P
CAHCZF */
                } else {
                    let x=X2.redc() 
                    let y=Y.redc()
                    P.copy(ECP(x,y))     
                    return P
                }
            } else {
// Shallue and van de Woestijne
// SQRTm3 not available, so preprocess this out
/* CAISZS
                var pNIL:FP?=nil
                let Z=CONFIG_FIELD.RIADZ
                X1.copy(FP(Z))
                X3.copy(X1)
                A.copy(RHS(X1))
                B.copy(FP(BIG(ROM.SQRTm3)))
                B.imul(Z)

                t.sqr()
                Y.copy(A); Y.mul(t)
                t.copy(one); t.add(Y); t.norm()
                Y.rsub(one); Y.norm()
                D.copy(t); D.mul(Y)
                D.mul(B)

                var w=FP(A)
                FP.tpo(&D,&w)
                
                w.mul(B)
                if w.sign()==1 {
                    w.neg()
                    w.norm()
                }

                w.mul(B)            
                w.mul(h); w.mul(Y); w.mul(D)

                X1.neg(); X1.norm(); X1.div2()
                X2.copy(X1)
                X1.sub(w); X1.norm()
                X2.add(w); X2.norm()
                A.add(A); A.add(A); A.norm()
                t.sqr(); t.mul(D); t.sqr()
                A.mul(t)
                X3.add(A); X3.norm()

                var rhs=ECP.RHS(X2)
                X3.cmove(X2,rhs.qr(&pNIL))
                rhs.copy(ECP.RHS(X1))
                X3.cmove(X1,rhs.qr(&pNIL))
                rhs.copy(ECP.RHS(X3))
                Y.copy(rhs.sqrt(pNIL))
               
                let ne=Y.sign()^sgn
                w.copy(Y); w.neg(); w.norm()
                Y.cmove(w,ne)

                let x=X3.redc()
                let y=Y.redc()
                P.copy(ECP(x,y))
                return P
CAISZF */
            }
        }
        return P
    }

/* Map byte array to point */
    static func mapit(_ h:[UInt8]) -> ECP
    {
        let q=BIG(ROM.Modulus)
        var dx = DBIG.fromBytes(h)
        let x=dx.mod(q)
        var P=ECP.hap2point(x)
		P.cfp()
        return P
    }

    static public func generator() -> ECP
    {
        let gx=BIG(ROM.CURVE_Gx);
        var G:ECP
        if CONFIG_CURVE.CURVETYPE != CONFIG_CURVE.MONTGOMERY
        {
            let gy=BIG(ROM.CURVE_Gy)
            G=ECP(gx,gy)
        }
        else
            {G=ECP(gx)}
        return G
    }

}
