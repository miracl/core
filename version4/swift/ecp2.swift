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
//  ecp2.swift
//
//  Created by Michael Scott on 07/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* AMCL Weierstrass elliptic curve functions over FP2 */

public struct ECP2 {
    private var x:FP2
    private var y:FP2
    private var z:FP2
    
    /* Constructor - set self=O */
    init()
    {
        x=FP2()
        y=FP2(1)
        z=FP2()
    }
    /* Test self=O? */
    public func is_infinity() -> Bool
    {
        return x.iszilch() && z.iszilch()
    }
    /* copy self=P */
    mutating public func copy(_ P:ECP2)
    {
        x.copy(P.x)
        y.copy(P.y)
        z.copy(P.z)
    }
    /* set self=O */
    mutating func inf() {
        x.zero()
        y.one()
        z.zero()
    }
    /* Conditional move of Q to P dependant on d */
    mutating func cmove(_ Q:ECP2,_ d:Int)
    {
        x.cmove(Q.x,d);
        y.cmove(Q.y,d);
        z.cmove(Q.z,d);
    }
    
    /* return 1 if b==c, no branching */
    private static func teq(_ b:Int32,_ c:Int32) -> Int
    {
        var x=b^c
        x-=1  // if x=0, x now -1
        return Int((x>>31)&1)
    }
    /* Constant time select from pre-computed table */
    mutating func select(_ W:[ECP2],_ b:Int32)
    {
        var MP=ECP2()
        let m=b>>31
        var babs=(b^m)-m
        
        babs=(babs-1)/2
    
        cmove(W[0],ECP2.teq(babs,0)) // conditional move
        cmove(W[1],ECP2.teq(babs,1))
        cmove(W[2],ECP2.teq(babs,2))
        cmove(W[3],ECP2.teq(babs,3))
        cmove(W[4],ECP2.teq(babs,4))
        cmove(W[5],ECP2.teq(babs,5))
        cmove(W[6],ECP2.teq(babs,6))
        cmove(W[7],ECP2.teq(babs,7))
    
        MP.copy(self)
        MP.neg()
        cmove(MP,Int(m&1))
    }
 
    /* Test if P == Q */
    func equals(_ Q:ECP2) -> Bool
    {
    
        var a=FP2(x)  
        var b=FP2(Q.x)
        a.mul(Q.z); b.mul(z) 
        if !a.equals(b) {return false}
        a.copy(y); a.mul(Q.z)
        b.copy(Q.y); b.mul(z)
        if !a.equals(b) {return false}
    
        return true;
    }
    /* set self=-self */
    mutating func neg()
    {
        y.norm(); y.neg(); y.norm()
        return
    }
    /* set to Affine - (x,y,z) to (x,y) */
    mutating func affine() {
        if is_infinity() {return}
        let one=FP2(1)
        if z.equals(one) {
            x.reduce(); y.reduce()
            return
        }
        z.inverse()
    
        x.mul(z); x.reduce()
        y.mul(z); y.reduce()
        z.copy(one)
    }
    /* extract affine x as FP2 */
    func getX() -> FP2
    {
        var W=ECP2(); W.copy(self)
        W.affine()
        return W.x
    }
    /* extract affine y as FP2 */
    func getY() -> FP2
    {
        var W=ECP2(); W.copy(self)
        W.affine()
        return W.y
    }
    /* extract projective x */
    func getx() -> FP2
    {
        return x
    }
    /* extract projective y */
    func gety() -> FP2
    {
        return y
    }
    /* extract projective z */
    func getz() -> FP2
    {
        return z
    }
    /* convert to byte array */
    func toBytes(_ b:inout [UInt8])
    {
        let RM=Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM)
        var W=ECP2(); W.copy(self)

        W.affine();
        W.x.getA().toBytes(&t)
        for i in 0 ..< RM
            {b[i]=t[i]}
        W.x.getB().toBytes(&t);
        for i in 0 ..< RM
            {b[i+RM]=t[i]}
    
        W.y.getA().toBytes(&t);
        for i in 0 ..< RM
            {b[i+2*RM]=t[i]}
        W.y.getB().toBytes(&t);
        for i in 0 ..< RM
            {b[i+3*RM]=t[i]}
    }
    /* convert from byte array to point */
    static func fromBytes(_ b:[UInt8]) -> ECP2
    {
        let RM=Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM)

    
        for i in 0 ..< RM {t[i]=b[i]}
        var ra=BIG.fromBytes(t);
        for i in 0 ..< RM {t[i]=b[i+RM]}
        var rb=BIG.fromBytes(t);
        let rx=FP2(ra,rb)
    
        for i in 0 ..< RM {t[i]=b[i+2*RM]}
        ra=BIG.fromBytes(t)
        for i in 0 ..< RM {t[i]=b[i+3*RM]}
        rb=BIG.fromBytes(t)
        let ry=FP2(ra,rb)
    
        return ECP2(rx,ry)
    }
/* convert self to hex string */
    func toString() -> String
    {
        var W=ECP2(); W.copy(self)
        if W.is_infinity() {return "infinity"}
        W.affine()
        return "("+W.x.toString()+","+W.y.toString()+")"
    }
    
/* Calculate RHS of twisted curve equation x^3+B/i */
    static func RHS(_ x:FP2) -> FP2
    {
        var r=FP2(x)
        r.sqr()
        var b=FP2(BIG(ROM.CURVE_B))
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {
            b.div_ip()
        }
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {
            b.norm()
            b.mul_ip()
            b.norm()
        }
        r.mul(x)
        r.add(b)
    
        r.reduce()
        return r
    }
/* construct self from (x,y) - but set to O if not on curve */
    public init(_ ix:FP2,_ iy:FP2)
    {
        x=FP2(ix)
        y=FP2(iy)
        z=FP2(1)
        x.norm()
        let rhs=ECP2.RHS(x)
        var y2=FP2(y)
        y2.sqr()
        if !y2.equals(rhs) {inf()}
    }
    /* construct this from x - but set to O if not on curve */
    init(_ ix:FP2)
    {
        x=FP2(ix)
        y=FP2(1)
        z=FP2(1)
        x.norm()
        var rhs=ECP2.RHS(x)
        if rhs.sqrt()
        {
            y.copy(rhs);
        }
        else {inf()}
    }
    
    /* this+=this */
    @discardableResult mutating func dbl() -> Int
    {
        if y.iszilch()
        {
            inf();
            return -1;
        }
    
        var iy=FP2(y)
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {       
            iy.mul_ip(); iy.norm()
        }

        var t0=FP2(y) 
        t0.sqr();
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {           
            t0.mul_ip() 
        }  
        var t1=FP2(iy)  
        t1.mul(z)
        var t2=FP2(z)
        t2.sqr()

        z.copy(t0)
        z.add(t0); z.norm() 
        z.add(z)
        z.add(z) 
        z.norm()  

        t2.imul(3*ROM.CURVE_B_I) 
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {
            t2.mul_ip()
            t2.norm()   
        }
        var x3=FP2(t2)
        x3.mul(z) 

        var y3=FP2(t0)   

        y3.add(t2); y3.norm()
        z.mul(t1)
        t1.copy(t2); t1.add(t2); t2.add(t1); t2.norm()  
        t0.sub(t2); t0.norm()                           //y^2-9bz^2
        y3.mul(t0); y3.add(x3)                          //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2
        t1.copy(x); t1.mul(iy)                     //
        x.copy(t0); x.norm(); x.mul(t1); x.add(x)       //(y^2-9bz^2)xy2

        x.norm() 
        y.copy(y3); y.norm()
        return 1
    }
/* this+=Q - return 0 for add, 1 for double, -1 for O */
    @discardableResult mutating func add(_ Q:ECP2) -> Int
    {

        let b=3*ROM.CURVE_B_I
        var t0=FP2(x)
        t0.mul(Q.x)         // x.Q.x
        var t1=FP2(y)
        t1.mul(Q.y)         // y.Q.y

        var t2=FP2(z)
        t2.mul(Q.z)
        var t3=FP2(x)
        t3.add(y); t3.norm()          //t3=X1+Y1
        var t4=FP2(Q.x)            
        t4.add(Q.y); t4.norm()         //t4=X2+Y2
        t3.mul(t4)                     //t3=(X1+Y1)(X2+Y2)
        t4.copy(t0); t4.add(t1)        //t4=X1.X2+Y1.Y2

        t3.sub(t4); t3.norm(); 
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {
            t3.mul_ip();  t3.norm()         //t3=(X1+Y1)(X2+Y2)-(X1.X2+Y1.Y2) = X1.Y2+X2.Y1
        }
        t4.copy(y)                    
        t4.add(z); t4.norm()           //t4=Y1+Z1
        var x3=FP2(Q.y)
        x3.add(Q.z); x3.norm()         //x3=Y2+Z2

        t4.mul(x3)                     //t4=(Y1+Z1)(Y2+Z2)
        x3.copy(t1)                    //
        x3.add(t2)                     //X3=Y1.Y2+Z1.Z2
    
        t4.sub(x3); t4.norm(); 
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {  
            t4.mul_ip(); t4.norm()          //t4=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1
        }
        x3.copy(x); x3.add(z); x3.norm()   // x3=X1+Z1
        var y3=FP2(Q.x)                
        y3.add(Q.z); y3.norm()             // y3=X2+Z2
        x3.mul(y3)                         // x3=(X1+Z1)(X2+Z2)
        y3.copy(t0)
        y3.add(t2)                         // y3=X1.X2+Z1+Z2
        y3.rsub(x3); y3.norm()             // y3=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {  
            t0.mul_ip(); t0.norm() // x.Q.x
            t1.mul_ip(); t1.norm() // y.Q.y
        }
        x3.copy(t0); x3.add(t0) 
        t0.add(x3); t0.norm()
        t2.imul(b)
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {
            t2.mul_ip(); t2.norm()
        }  
        var z3=FP2(t1); z3.add(t2); z3.norm()
        t1.sub(t2); t1.norm()
        y3.imul(b)
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {          
            y3.mul_ip()
            y3.norm()
        }
        x3.copy(y3); x3.mul(t4); t2.copy(t3); t2.mul(t1); x3.rsub(t2)
        y3.mul(t0); t1.mul(z3); y3.add(t1)
        t0.mul(t3); z3.mul(t4); z3.add(t0)

        x.copy(x3); x.norm()
        y.copy(y3); y.norm()
        z.copy(z3); z.norm()    

        return 0
    }

    /* set self-=Q */
    @discardableResult mutating func sub(_ Q:ECP2) -> Int
    {
        var NQ=ECP2(); NQ.copy(Q)
        NQ.neg()
        let D=add(NQ)
        return D
    }
/* set self*=q, where q is Modulus, using Frobenius */
    mutating func frob(_ X:FP2)
    {
        var X2=FP2(X)
        X2.sqr()
        x.conj()
        y.conj()
        z.conj()
        z.reduce()
        x.mul(X2)
        y.mul(X2)
        y.mul(X)
    }
    
    /* P*=e */
    func mul(_ e:BIG) -> ECP2
    {
    /* fixed size windows */
        var mt=BIG()
        var t=BIG()
        var P=ECP2()
        var Q=ECP2()
        var C=ECP2()
        
        var W=[ECP2]();
        for _ in 0 ..< 8 {W.append(ECP2())}
        
        var w=[Int8](repeating: 0,count: 1+(CONFIG_BIG.NLEN*Int(CONFIG_BIG.BASEBITS)+3)/4)
    
        if is_infinity() {return ECP2()}
    
    /* precompute table */
        Q.copy(self)
        Q.dbl()
        W[0].copy(self)
    
        for i in 1 ..< 8
        {
            W[i].copy(W[i-1])
            W[i].add(Q)
        }
    
    /* make exponent odd - add 2P if even, P if odd */
        t.copy(e)
        let s=t.parity()
        t.inc(1); t.norm(); let ns=t.parity(); mt.copy(t); mt.inc(1); mt.norm()
        t.cmove(mt,s)
        Q.cmove(self,ns)
        C.copy(Q)
    
        let nb=1+(t.nbits()+3)/4
    /* convert exponent to signed 4-bit window */
        for i in 0 ..< nb
        {
            w[i]=Int8(t.lastbits(5)-16)
            t.dec(Int(w[i])); t.norm()
            t.fshr(4)
        }
        w[nb]=Int8(t.lastbits(5))
    
        P.copy(W[Int(w[nb]-1)/2])
        for i in (0...nb-1).reversed()
        {
            Q.select(W,Int32(w[i]))
            P.dbl()
            P.dbl()
            P.dbl()
            P.dbl()
            P.add(Q)
        }
        P.sub(C);
        P.affine()
        return P;
    }
    
    /* P=u0.Q0+u1*Q1+u2*Q2+u3*Q3 */
    // Bos & Costello https://eprint.iacr.org/2013/458.pdf
    // Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
    // Side channel attack secure 

    static func mul4(_ Q:[ECP2],_ u:[BIG]) -> ECP2
    {
        var W=ECP2()
        var P=ECP2()
        
        var T=[ECP2]();
        for _ in 0 ..< 8 {T.append(ECP2())}
    
        var mt=BIG()
        var t=[BIG]()
    
        var w=[Int8](repeating: 0,count: CONFIG_BIG.NLEN*Int(CONFIG_BIG.BASEBITS)+1)
        var s=[Int8](repeating: 0,count: CONFIG_BIG.NLEN*Int(CONFIG_BIG.BASEBITS)+1)
    
        for i in 0 ..< 4
        {
            t.append(BIG(u[i]))
            t[i].norm()
        }

    // precompute table 

        T[0].copy(Q[0])  // Q[0]
        T[1].copy(T[0]); T[1].add(Q[1])  // Q[0]+Q[1]
        T[2].copy(T[0]); T[2].add(Q[2])  // Q[0]+Q[2]
        T[3].copy(T[1]); T[3].add(Q[2])  // Q[0]+Q[1]+Q[2]
        T[4].copy(T[0]); T[4].add(Q[3])  // Q[0]+Q[3]
        T[5].copy(T[1]); T[5].add(Q[3])  // Q[0]+Q[1]+Q[3]
        T[6].copy(T[2]); T[6].add(Q[3])  // Q[0]+Q[2]+Q[3]
        T[7].copy(T[3]); T[7].add(Q[3])  // Q[0]+Q[1]+Q[2]+Q[3]

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
        P.select(T,Int32(2*w[nb-1]+1));
        for i in (0 ..< nb-1).reversed() {
            P.dbl()
            W.select(T,Int32(2*w[i]+s[i]))
            P.add(W)
        }    

        W.copy(P)  
        W.sub(Q[0])
        P.cmove(W,pb) 
        P.affine()
        return P
    }

     // needed for SOK
    static func mapit(_ h:[UInt8]) -> ECP2
    {
        let q=BIG(ROM.Modulus)
        var x=BIG.fromBytes(h)
        let one=BIG(1)
        var Q=ECP2()
        x.mod(q);
        while (true)
        {
            let X=FP2(one,x)
            Q=ECP2(X)
            if !Q.is_infinity() {break}
            x.inc(1); x.norm()
        }
    // Fast Hashing to G2 - Fuentes-Castaneda, Knapp and Rodriguez-Henriquez
        let Fra=BIG(ROM.Fra)
        let Frb=BIG(ROM.Frb)
        var X=FP2(Fra,Frb)
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE { 
            X.inverse()
            X.norm()
        }         
        x=BIG(ROM.CURVE_Bnx)
    
        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            var T=Q.mul(x)
            if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
                T.neg()
            }
            var K=ECP2(); K.copy(T)
            K.dbl(); K.add(T)
    
            K.frob(X)
            Q.frob(X); Q.frob(X); Q.frob(X)
            Q.add(T); Q.add(K)
            T.frob(X); T.frob(X)
            Q.add(T)
        }
        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BLS {
            var xQ=Q.mul(x);
            var x2Q=xQ.mul(x);

            if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
                xQ.neg()
            }

            x2Q.sub(xQ)
            x2Q.sub(Q)

            xQ.sub(Q)
            xQ.frob(X)

            Q.dbl()
            Q.frob(X)
            Q.frob(X)

            Q.add(x2Q)
            Q.add(xQ)
        }        
        Q.affine()
        return Q
    }   
    
    static public func generator() -> ECP2
    {
        return ECP2(FP2(BIG(ROM.CURVE_Pxa),BIG(ROM.CURVE_Pxb)),FP2(BIG(ROM.CURVE_Pya),BIG(ROM.CURVE_Pyb)))
    }

}
