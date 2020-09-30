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
//  ecp2.swift
//
//  Created by Michael Scott on 07/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* CORE Weierstrass elliptic curve functions over FP2 */

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
    mutating public func affine() {
        if is_infinity() {return}
        let one=FP2(1)
        if z.equals(one) {
            x.reduce(); y.reduce()
            return
        }
        let pNIL:FP?=nil
        z.inverse(pNIL)

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
    func toBytes(_ b:inout [UInt8],_ compress: Bool)
    {
        let RM=2*Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM)
        var alt=false
        var W=ECP2(); W.copy(self)
        W.affine()
	    W.x.toBytes(&t)

        if (CONFIG_FIELD.MODBITS-1)%8 <= 4 && CONFIG_CURVE.ALLOW_ALT_COMPRESS {
            alt=true
        }
        if alt {
		    for i in 0 ..< RM {
			    b[i]=t[i]
		    }
            if !compress {
                W.y.toBytes(&t);
                for i in 0 ..< RM {
				    b[i+RM]=t[i]
			    }
            } else {
                b[0]|=0x80
                if W.y.islarger()==1 {
				    b[0]|=0x20
			    }
            }

	    } else {
		    for i in 0 ..< RM {
			    b[i+1]=t[i]
		    }
            if !compress {
                b[0]=0x04
                W.y.toBytes(&t)
	            for i in 0 ..< RM {
			        b[i+RM+1]=t[i]
			    }
            } else {
                b[0]=0x02
                if W.y.sign() == 1 {
                    b[0]=0x03
			    }
            }
	    }
    }
    /* convert from byte array to point */
    static func fromBytes(_ b:[UInt8]) -> ECP2
    {
        let RM=2*Int(CONFIG_BIG.MODBYTES)
        var t=[UInt8](repeating: 0,count: RM)
        var alt=false
        let typ = Int(b[0])

        if (CONFIG_FIELD.MODBITS-1)%8 <= 4 && CONFIG_CURVE.ALLOW_ALT_COMPRESS {
            alt=true
        }

	    if alt {
            for i in 0 ..< RM  {
			    t[i]=b[i]
		    }
            t[0]&=0x1f
            let rx=FP2.fromBytes(t)
            if (b[0]&0x80)==0 {
                for i in 0 ..< RM {
				    t[i]=b[i+RM]
			    }
                let ry=FP2.fromBytes(t)
                return ECP2(rx,ry)
            } else {
                let sgn=(b[0]&0x20)>>5
                var P=ECP2(rx,0)
                let cmp=P.y.islarger()
                if (sgn == 1 && cmp != 1) || (sgn == 0 && cmp == 1) {
				    P.neg()
			    }
                return P;
            }
        } else {
		    for i in 0 ..< RM {
			    t[i]=b[i+1]
		    }
            let rx=FP2.fromBytes(t)
            if typ == 0x04 {
		        for i in 0 ..< RM {
				    t[i]=b[i+RM+1]
			    }
		        let ry=FP2.fromBytes(t)
		        return ECP2(rx,ry)
            } else {
                return ECP2(rx,typ&1)
            }
        }
    }
/* convert self to hex string */
    public func toString() -> String
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
    init(_ ix:FP2,_ s:Int)
    {
        x=FP2(ix)
        y=FP2(1)
        z=FP2(1)
        var hint:FP?=FP()
        x.norm()
        var rhs=ECP2.RHS(x)
 	    if rhs.qr(&hint) == 1 {
		    rhs.sqrt(hint)
		    if rhs.sign() != s {
			    rhs.neg()
		    }
		    rhs.reduce()
		    y.copy(rhs)
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
/* this+=Q  */
    @discardableResult mutating public func add(_ Q:ECP2) -> Int
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
        return P;
    }


    // clear cofactor
    mutating public func cfp()
    {
    // Fast Hashing to G2 - Fuentes-Castaneda, Knapp and Rodriguez-Henriquez
        let Fra=BIG(ROM.Fra)
        let Frb=BIG(ROM.Frb)
        var X=FP2(Fra,Frb)
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {
            let pNIL:FP?=nil
            X.inverse(pNIL)
            X.norm()
        }
        let x=BIG(ROM.CURVE_Bnx)

        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            var T=self.mul(x)
            if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
                T.neg()
            }
            var K=ECP2(); K.copy(T)
            K.dbl(); K.add(T)

            K.frob(X)
            self.frob(X); self.frob(X); self.frob(X)
            self.add(T); self.add(K)
            T.frob(X); T.frob(X)
            self.add(T)
        }
        if CONFIG_CURVE.CURVE_PAIRING_TYPE > CONFIG_CURVE.BN {
            var xQ=self.mul(x);
            var x2Q=xQ.mul(x);

            if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
                xQ.neg()
            }

            x2Q.sub(xQ)
            x2Q.sub(self)

            xQ.sub(self)
            xQ.frob(X)

            self.dbl()
            self.frob(X)
            self.frob(X)

            self.add(x2Q)
            self.add(xQ)
        }
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
        return P
    }

/* Hunt and Peck a BIG to a curve point */
    static public func hap2point(_ h:BIG) -> ECP2
    {
        let one=BIG(1)
        var Q=ECP2()
        var x=BIG(h)
        while (true)
        {
            let X=FP2(one,x)
            Q=ECP2(X,0)
            if !Q.is_infinity() {break}
            x.inc(1); x.norm()
        }
        return Q
    }

/* Constant time Map to Point */
    static public func map2point(_ H:FP2) -> ECP2
    { // Shallue and van de Woestijne
        var Q:ECP2
        var T=FP2(H) /**/
        let sgn=T.sign() /**/
        if CONFIG_CURVE.HTC_ISO_G2 == 0 {
/* CAHCNZS
            var pNIL:FP?=nil
            var NY=FP2(1)
            var Z=FP(CONFIG_FIELD.RIADZG2A)
            var X1=FP2(Z)
            var X3=FP2(X1)
            var A=ECP2.RHS(X1)
            var W=FP2(A)
            if (CONFIG_FIELD.RIADZG2A == -1 && CONFIG_FIELD.RIADZG2B == 0 && CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE && ROM.CURVE_B_I == 4)
            { // special case for BLS12381
                W.copy(FP2(2,1))
            } else {
                W.sqrt(pNIL)
            }
            let s=FP(BIG(ROM.SQRTm3))
            Z.mul(s)

            T.sqr()
            var Y=FP2(A); Y.mul(T)
            T.copy(NY); T.add(Y); T.norm()
            Y.rsub(NY); Y.norm()
            NY.copy(T); NY.mul(Y)
        
            NY.pmul(Z)
            NY.inverse(pNIL)

            W.pmul(Z)
            if W.sign()==1 {
                W.neg()
                W.norm()
            }
            W.pmul(Z)
            W.mul(H); W.mul(Y); W.mul(NY)

            X1.neg(); X1.norm(); X1.div2()
            var X2=FP2(X1)
            X1.sub(W); X1.norm()
            X2.add(W); X2.norm()
            A.add(A); A.add(A); A.norm()
            T.sqr(); T.mul(NY); T.sqr()
            A.mul(T)
            X3.add(A); X3.norm()

            Y.copy(ECP2.RHS(X2))
            X3.cmove(X2,Y.qr(&pNIL))
            Y.copy(ECP2.RHS(X1))
            X3.cmove(X1,Y.qr(&pNIL))
            Y.copy(ECP2.RHS(X3))
            Y.sqrt(pNIL)

            let ne=Y.sign()^sgn
            W.copy(Y); W.neg(); W.norm()
            Y.cmove(W,ne)

            Q=ECP2(X3,Y)
            return Q
CAHCNZF */
        } else {

/* CAHCZS
            let NY=FP2(1)
            Q=ECP2()
            let Ad=FP2(BIG(ROM.CURVE_Adr),BIG(ROM.CURVE_Adi))
            let Bd=FP2(BIG(ROM.CURVE_Bdr),BIG(ROM.CURVE_Bdi))
            let ZZ=FP2(CONFIG_FIELD.RIADZG2A,CONFIG_FIELD.RIADZG2B)
            var hint:FP?=FP()

            T.sqr()
            T.mul(ZZ)
            var W=FP2(T)
            W.add(NY); W.norm()

            W.mul(T)
            var D=FP2(Ad)
            D.mul(W)

            W.add(NY); W.norm()
            W.mul(Bd)
            W.neg(); W.norm()

            var X2=FP2(W)
            var X3=FP2(T)
            X3.mul(X2)

            var GX1=FP2(X2); GX1.sqr()
            var D2=FP2(D); D2.sqr()

            W.copy(Ad); W.mul(D2); GX1.add(W); GX1.norm(); GX1.mul(X2); D2.mul(D); W.copy(Bd); W.mul(D2); GX1.add(W); GX1.norm() // x^3+Ax+b

            W.copy(GX1); W.mul(D)
            let qr=W.qr(&hint)
            D.copy(W); D.inverse(hint)
            D.mul(GX1)
            X2.mul(D)
            X3.mul(D)
            T.mul(H)
            D2.copy(D); D2.sqr()

            D.copy(D2); D.mul(T)
            T.copy(W); T.mul(ZZ)

            var s=FP(BIG(ROM.CURVE_HTPC2))
            s.mul(hint!);

            X2.cmove(X3,1-qr)
            W.cmove(T,1-qr)
            D2.cmove(D,1-qr)
            hint!.cmove(s,1-qr)

            var Y=FP2(W); Y.sqrt(hint)
            Y.mul(D2)

            let ne=Y.sign()^sgn
            W.copy(Y); W.neg(); W.norm()
            Y.cmove(W,ne)

            var k=0
            let isox=CONFIG_CURVE.HTC_ISO_G2
            let isoy=3*(isox-1)/2
        //xnum
            var xnum=FP2(BIG(ROM.PCR[k]),BIG(ROM.PCI[k])); k+=1
            for _ in 0..<isox {
                xnum.mul(X2)
                xnum.add(FP2(BIG(ROM.PCR[k]),BIG(ROM.PCI[k]))); k+=1
                xnum.norm()
            }
        //xden
            var xden=FP2(X2)
            xden.add(FP2(BIG(ROM.PCR[k]),BIG(ROM.PCI[k]))); k+=1
            xden.norm()
            for _ in 0..<isox-2 {
                xden.mul(X2)
                xden.add(FP2(BIG(ROM.PCR[k]),BIG(ROM.PCI[k]))); k+=1
                xden.norm()
            }
        //ynum
            var ynum=FP2(BIG(ROM.PCR[k]),BIG(ROM.PCI[k])); k+=1
            for _ in 0..<isoy {
                ynum.mul(X2)
                ynum.add(FP2(BIG(ROM.PCR[k]),BIG(ROM.PCI[k]))); k+=1
                ynum.norm()
            }
        //yden
            var yden=FP2(X2)
            yden.add(FP2(BIG(ROM.PCR[k]),BIG(ROM.PCI[k]))); k+=1
            yden.norm()
            for _ in 0..<isoy-1 {
                yden.mul(X2)
                yden.add(FP2(BIG(ROM.PCR[k]),BIG(ROM.PCI[k]))); k+=1
                yden.norm()
            }
            ynum.mul(Y)
            T.copy(xnum); T.mul(yden)
            Q.x.copy(T)
            T.copy(ynum); T.mul(xden)
            Q.y.copy(T)
            T.copy(xden); T.mul(yden)
            Q.z.copy(T)
            return Q
CAHCZF */
        }
        return ECP2()
    }

/* Map byte array to point */
	static public func mapit(_ h:[UInt8]) -> ECP2
	{
		let q=BIG(ROM.Modulus)
		var dx=DBIG.fromBytes(h)
        let x=dx.mod(q)

		var Q=ECP2.hap2point(x);
		Q.cfp()
        return Q
    }

    static public func generator() -> ECP2
    {
        return ECP2(FP2(BIG(ROM.CURVE_Pxa),BIG(ROM.CURVE_Pxb)),FP2(BIG(ROM.CURVE_Pya),BIG(ROM.CURVE_Pyb)))
    }

}
