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
//  pair.swift
//
//  Created by Michael Scott on 07/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* CORE BN Curve Pairing functions */

public struct PAIR {
    
   // Point doubling for pairings
    static private func dbl(_ A: inout ECP2,_ AA: inout FP2,_ BB: inout FP2,_ CC: inout FP2)
    {
        CC.copy(A.getx())  //X
        var YY=FP2(A.gety())  //Y
        BB.copy(A.getz())  //Z
        AA.copy(YY)        //Y 
        AA.mul(BB)                //YZ
        CC.sqr()                  //X^2
        YY.sqr()                  //Y^2
        BB.sqr()                  //Z^2
        
        AA.add(AA)
        AA.neg(); AA.norm()       //-2AA
        AA.mul_ip()
        AA.norm()

        let sb=3*ROM.CURVE_B_I
        BB.imul(sb)
        CC.imul(3)
        if CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.D_TYPE {
            YY.mul_ip()
            YY.norm()
            CC.mul_ip()
            CC.norm()
        }
        if CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.M_TYPE {
            BB.mul_ip()
            BB.norm()
        }
        BB.sub(YY)
        BB.norm()

        A.dbl()

    } 
// Point addition for pairings
    static private func add(_ A: inout ECP2,_ B:ECP2,_ AA: inout FP2,_ BB: inout FP2,_ CC: inout FP2)
    {
        AA.copy(A.getx())    // X1
        CC.copy(A.gety())    // Y1
        var T1=FP2(A.getz())    // Z1
        BB.copy(A.getz())    // Z1
            
        T1.mul(B.gety())    // T1=Z1.Y2 
        BB.mul(B.getx())    // T2=Z1.X2

        AA.sub(BB); AA.norm()  // X1=X1-Z1.X2
        CC.sub(T1); CC.norm()  // Y1=Y1-Z1.Y2

        T1.copy(AA)            // T1=X1-Z1.X2

        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {
            AA.mul_ip()
            AA.norm()
        }              
        T1.mul(B.gety())       // T1=(X1-Z1.X2).Y2

        BB.copy(CC)            // T2=Y1-Z1.Y2
        BB.mul(B.getx())       // T2=(Y1-Z1.Y2).X2
        BB.sub(T1); BB.norm()          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2
        CC.neg(); CC.norm() // Y1=-(Y1-Z1.Y2).Xs

        A.add(B)

    }

 // Line functions
    static private func linedbl(_ A: inout ECP2,_ Qx:FP,_ Qy:FP) -> FP12
    {
        var a:FP4
        var b:FP4
        var c:FP4

        var AA=FP2()  //X
        var BB=FP2()  //Y
        var CC=FP2()  //Z

        dbl(&A,&AA,&BB,&CC)

        CC.pmul(Qx)
        AA.pmul(Qy)

        a=FP4(AA,BB)          // -2YZ.Ys | 3b.Z^2-Y^2 | 3X^2.Xs 
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {             
            b=FP4(CC)            // L(0,1) | L(0,0) | L(1,0)
            c=FP4()
        } else { 
            b=FP4()
            c=FP4(CC); c.times_i()
        }   
        var res=FP12(a,b,c)
        res.settype(FP12.SPARSER)
        return res
    }            

    static private func lineadd(_ A: inout ECP2,_ B: ECP2,_ Qx:FP,_ Qy:FP) -> FP12
    {
        var a:FP4
        var b:FP4
        var c:FP4

        var AA=FP2()  //X
        var BB=FP2()  //Y
        var CC=FP2()  //Z

        add(&A,B,&AA,&BB,&CC)

        CC.pmul(Qx)
        AA.pmul(Qy)

        a=FP4(AA,BB)          // -2YZ.Ys | 3b.Z^2-Y^2 | 3X^2.Xs 
        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {             
            b=FP4(CC)            // L(0,1) | L(0,0) | L(1,0)
            c=FP4()
        } else { 
            b=FP4()
            c=FP4(CC); c.times_i()
        }   
        var res=FP12(a,b,c)
        res.settype(FP12.SPARSER)
        return res
    }       

    static private func lbits(_ n3:inout BIG,_ n:inout BIG) -> Int
    {
        n.copy(BIG(ROM.CURVE_Bnx))
        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            n.pmul(6)
            if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.POSITIVEX {  
                n.inc(2)
            } else {
                n.dec(2)
            }
        }
        n.norm()
        n3.copy(n)
        n3.pmul(3)
        n3.norm()
        return n3.nbits()          
    }

    static public func initmp() -> [FP12]
    {
        var r=[FP12]()
        for _ in (0...CONFIG_CURVE.ATE_BITS-1).reversed() {
            r.append(FP12(1))
        }
        return r
    }

/* basic Miller loop */
    static public func miller(_ r: inout [FP12]) -> FP12 {
        var res=FP12(1)
        for i in (1...CONFIG_CURVE.ATE_BITS-1).reversed() {
            res.sqr()
            res.ssmul(r[i])
            r[i].zero()
        }

        if CONFIG_CURVE.SIGN_OF_X==CONFIG_CURVE.NEGATIVEX {
            res.conj();
        }
        res.ssmul(r[0])
        r[0].zero()
        return res
    }

    static private func pack(_ AA: FP2,_ BB: FP2,_ CC: FP2) -> FP4 {
        var i=FP2(CC)
        let pNIL:FP?=nil
        i.inverse(pNIL)
        var a=FP2(AA)
        var b=FP2(BB)
        a.mul(i)
        b.mul(i)
        return FP4(a,b)
    }

    static private func unpack(_ T: FP4,_ Qx: FP,_ Qy: FP) -> FP12 {
        var b:FP4
        var c:FP4
        let W=FP2(Qx)
        var AA=T.geta()
        let BB=T.getb()
        AA.pmul(Qy)
        let a=FP4(AA,BB)

        if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE {             
            b=FP4(W)            // L(0,1) | L(0,0) | L(1,0)
            c=FP4()
        } else { 
            b=FP4()
            c=FP4(W); c.times_i()
        } 
        var v=FP12(a,b,c)
        v.settype(FP12.SPARSEST)
        return v
    }

    static public func precomp(_ GV: ECP2) -> [FP4] {
        var f=FP2(BIG(ROM.Fra),BIG(ROM.Frb))
        var n = BIG()
        var n3 = BIG()
        var AA=FP2()
        var BB=FP2()
        var CC=FP2()

        let nb=lbits(&n3,&n)    
        var P=ECP2(); P.copy(GV);            

        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE { 
                let pNIL:FP?=nil            
                f.inverse(pNIL)
                f.norm()
            }
        }
        var A=ECP2(); A.copy(P)
        var NP=ECP2(); NP.copy(P); NP.neg()

        var T=[FP4]()
        for i in (1...nb-2).reversed() {     
            dbl(&A,&AA,&BB,&CC)
            T.append(pack(AA,BB,CC))
            let bt=n3.bit(UInt(i))-n.bit(UInt(i))     
            if bt == 1 {
                add(&A,P,&AA,&BB,&CC)
                T.append(pack(AA,BB,CC))
            }
            if bt == -1 {
                add(&A,NP,&AA,&BB,&CC)
                T.append(pack(AA,BB,CC))                
            }            
        }   
/* R-ate fixup required for BN curves */
        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            if CONFIG_CURVE.SIGN_OF_X==CONFIG_CURVE.NEGATIVEX {
                A.neg()
            }
            var K = ECP2()    
            K.copy(P)
            K.frob(f)
            add(&A,K,&AA,&BB,&CC)
            T.append(pack(AA,BB,CC))  
            K.frob(f)
            K.neg()
            add(&A,K,&AA,&BB,&CC)
            T.append(pack(AA,BB,CC)) 
        }         
        return T
    }

    static public func another_pc(_ r: inout [FP12],_ T: [FP4],_ QV: ECP) {
        var n = BIG()
        var n3 = BIG()

	if QV.is_infinity() {
		return
	}


        let nb=lbits(&n3,&n)    
        var Q=ECP(); Q.copy(QV); Q.affine()
        let Qx=FP(Q.getx())
        let Qy=FP(Q.gety())

        var j=0
        for i in (1...nb-2).reversed() {
            var lv=unpack(T[j],Qx,Qy); j+=1
            let bt=n3.bit(UInt(i))-n.bit(UInt(i))
            if bt == 1 {
                let lv2=unpack(T[j],Qx,Qy); j+=1
                lv.smul(lv2)
            }
            if bt == -1 {
                let lv2=unpack(T[j],Qx,Qy); j+=1
                lv.smul(lv2) 
            }    
            r[i].ssmul(lv)
        }  
/* R-ate fixup required for BN curves */
        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            var lv=unpack(T[j],Qx,Qy); j+=1
            let lv2=unpack(T[j],Qx,Qy)
            lv.smul(lv2)
            r[0].ssmul(lv)
        }                           
    }


/* Accumulate another set of line functions for n-pairing */
    static public func another(_ r: inout [FP12],_ P1: ECP2,_ Q1: ECP) {
        var f=FP2(BIG(ROM.Fra),BIG(ROM.Frb))
        var n = BIG()
        var n3 = BIG()
        var K = ECP2()

	if Q1.is_infinity() {
		return
	}

// P is needed in affine form for line function, Q for (Qx,Qy) extraction
        var P=ECP2(); P.copy(P1); P.affine()
        var Q=ECP(); Q.copy(Q1); Q.affine()

        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {  
                let pNIL:FP?=nil   
                f.inverse(pNIL)
                f.norm()
            }
        }

        let Qx=FP(Q.getx())
        let Qy=FP(Q.gety())
    
        var A=ECP2()
        A.copy(P)
        var NP=ECP2()
        NP.copy(P)
        NP.neg()

        let nb=lbits(&n3,&n)

        for i in (1...nb-2).reversed() {
            var lv=linedbl(&A,Qx,Qy)

            let bt=n3.bit(UInt(i))-n.bit(UInt(i))
            if bt == 1 {
                let lv2=lineadd(&A,P,Qx,Qy)
                lv.smul(lv2)
            }
            if bt == -1 {
                let lv2=lineadd(&A,NP,Qx,Qy)
                lv.smul(lv2)
            }
            r[i].ssmul(lv)
        }

/* R-ate fixup required for BN curves */
        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            if CONFIG_CURVE.SIGN_OF_X==CONFIG_CURVE.NEGATIVEX {
                A.neg()
            }
            K.copy(P)
            K.frob(f)
            var lv=lineadd(&A,K,Qx,Qy)
            K.frob(f)
            K.neg()
            let lv2=lineadd(&A,K,Qx,Qy)
            lv.smul(lv2)
            r[0].ssmul(lv)
        } 
    }


    // Optimal R-ate pairing
    static public func ate(_ P1:ECP2,_ Q1:ECP) -> FP12
    {
        var f=FP2(BIG(ROM.Fra),BIG(ROM.Frb))
        var n = BIG();
        var n3 = BIG();
        var K=ECP2()
        
	if Q1.is_infinity() {
		return FP12(1)
	}

        var lv:FP12

        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {  
                let pNIL:FP?=nil   
                f.inverse(pNIL)
                f.norm()
            }
        }

        var P=ECP2(); P.copy(P1); P.affine()
        var Q=ECP(); Q.copy(Q1); Q.affine()


        let Qx=FP(Q.getx())
        let Qy=FP(Q.gety())
    
        var A=ECP2()
        A.copy(P)

        var NP=ECP2()
        NP.copy(P)
        NP.neg()

        var r=FP12(1)
        let nb=lbits(&n3,&n)
    
        for i in (1...nb-2).reversed()
        //for var i=nb-2;i>=1;i--
        {
            r.sqr()            
            lv=linedbl(&A,Qx,Qy)

            let bt=n3.bit(UInt(i))-n.bit(UInt(i))
            if bt == 1 {
                let lv2=lineadd(&A,P,Qx,Qy)
                lv.smul(lv2)
            }
            if bt == -1 {
                let lv2=lineadd(&A,NP,Qx,Qy)
                lv.smul(lv2)
            }
            r.ssmul(lv)        
        }
    
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            r.conj()
         }     

    // R-ate fixup required for BN curves

	   if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
                A.neg()
            }           
            K.copy(P)
            K.frob(f)

            lv=lineadd(&A,K,Qx,Qy)
            K.frob(f)
            K.neg()
            let lv2=lineadd(&A,K,Qx,Qy)
            lv.smul(lv2)
            r.ssmul(lv)
        }
        return r
    }
    // Optimal R-ate double pairing e(P,Q).e(R,S)
    static public func ate2(_ P1:ECP2,_ Q1:ECP,_ R1:ECP2,_ S1:ECP) -> FP12
    {
        var f=FP2(BIG(ROM.Fra),BIG(ROM.Frb))
        var n = BIG();
        var n3 = BIG();
        var K=ECP2()
        var lv:FP12

	if Q1.is_infinity() {
		return PAIR.ate(R1,S1);
	}
	if S1.is_infinity() {
		return PAIR.ate(P1,Q1);
	}

        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {  
                let pNIL:FP?=nil   
                f.inverse(pNIL)
                f.norm()
            }
        }
    
        var P=ECP2(); P.copy(P1); P.affine()
        var Q=ECP(); Q.copy(Q1); Q.affine()
        var R=ECP2(); R.copy(R1); R.affine()
        var S=ECP(); S.copy(S1); S.affine()

        let Qx=FP(Q.getx())
        let Qy=FP(Q.gety())
        let Sx=FP(S.getx())
        let Sy=FP(S.gety())
    
        var A=ECP2()
        var B=ECP2()
        var r=FP12(1)
    
        A.copy(P)
        B.copy(R)
        var NP=ECP2()
        NP.copy(P)
        NP.neg()
        var NR=ECP2()
        NR.copy(R)
        NR.neg()

        let nb=lbits(&n3,&n)
    
        for i in (1...nb-2).reversed()
        {
            r.sqr()            
            lv=linedbl(&A,Qx,Qy)
            var lv2=linedbl(&B,Sx,Sy)
            lv.smul(lv2)
            r.ssmul(lv)
            let bt=n3.bit(UInt(i))-n.bit(UInt(i))

            if bt == 1 {
                lv=lineadd(&A,P,Qx,Qy)
                lv2=lineadd(&B,R,Sx,Sy)
                lv.smul(lv2)
                r.ssmul(lv)
            }

            if bt == -1 {
                lv=lineadd(&A,NP,Qx,Qy)
                lv2=lineadd(&B,NR,Sx,Sy)
                lv.smul(lv2)
                r.ssmul(lv)              
            }            

        }
    
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            r.conj()
         }     

    // R-ate fixup required for BN curves

	   if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
            if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
                A.neg()                
                B.neg()
            }
            K.copy(P)
            K.frob(f)

            lv=lineadd(&A,K,Qx,Qy)
            K.frob(f)
            K.neg()
            var lv2=lineadd(&A,K,Qx,Qy)
            lv.smul(lv2)
            r.ssmul(lv)
    
            K.copy(R)
            K.frob(f)

            lv=lineadd(&B,K,Sx,Sy)
            K.frob(f)
            K.neg()
            lv2=lineadd(&B,K,Sx,Sy)
            lv.smul(lv2)
            r.ssmul(lv)
        }
        return r
    }
    
    // final exponentiation - keep separate for multi-pairings and to avoid thrashing stack
    static public func fexp(_ m:FP12) -> FP12
    {
        let f=FP2(BIG(ROM.Fra),BIG(ROM.Frb));
        let x=BIG(ROM.CURVE_Bnx)
        var r=FP12(m)
    
    // Easy part of final exp
        var lv=FP12(r)
        lv.inverse()
        r.conj()
    
        r.mul(lv)
        lv.copy(r)
        r.frob(f)
        r.frob(f)
        r.mul(lv)

    // Hard part of final exp
	if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
		lv.copy(r)
		lv.frob(f)
		var x0=FP12(lv)
		x0.frob(f)
		lv.mul(r)
		x0.mul(lv)
		x0.frob(f)
		var x1=FP12(r)
		x1.conj()
		var x4=r.pow(x)
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.POSITIVEX {
            x4.conj()
        }
		var x3=FP12(x4)
		x3.frob(f)
    
		var x2=x4.pow(x)
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.POSITIVEX {
            x2.conj()
        }    
		var x5=FP12(x2); x5.conj()
		lv=x2.pow(x)
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.POSITIVEX {
            lv.conj()
        }   
		x2.frob(f)
		r.copy(x2); r.conj()
    
		x4.mul(r)
		x2.frob(f)
    
		r.copy(lv)
		r.frob(f)
		lv.mul(r)
    
		lv.usqr()
		lv.mul(x4)
		lv.mul(x5)
		r.copy(x3)
		r.mul(x5)
		r.mul(lv)
		lv.mul(x2)
		r.usqr()
		r.mul(lv)
		r.usqr()
		lv.copy(r)
		lv.mul(x1)
		r.mul(x0)
		lv.usqr()
		r.mul(lv)
		r.reduce()
	} else {

// See https://eprint.iacr.org/2020/875.pdf
        var y1=FP12(r)
        y1.usqr()
        y1.mul(r) // y1=r^3

        var y0=FP12(r.pow(x))
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            y0.conj()
        }
        var t0=FP12(r); t0.conj()
        r.copy(y0)
        r.mul(t0)

        y0.copy(r.pow(x))
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            y0.conj()
        }
        t0.copy(r); t0.conj()
        r.copy(y0)
        r.mul(t0)

// ^(x+p)
        y0.copy(r.pow(x))
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            y0.conj()
        }
        t0.copy(r)
        t0.frob(f)
        r.copy(y0)
        r.mul(t0)

// ^(x^2+p^2-1)
        y0.copy(r.pow(x))
        y0.copy(y0.pow(x))
        t0.copy(r)
        t0.frob(f); t0.frob(f)
        y0.mul(t0)
        t0.copy(r); t0.conj()
        r.copy(y0)
        r.mul(t0)

        r.mul(y1)
        r.reduce()

/*		var x0=FP12(r)
		var x1=FP12(r)
		lv.copy(r); lv.frob(f)
		var x3=FP12(lv); x3.conj(); x1.mul(x3)
		lv.frob(f); lv.frob(f)
		x1.mul(lv)

		r.copy(r.pow(x)) 
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            r.conj()
        }          
		x3.copy(r); x3.conj(); x1.mul(x3)
		lv.copy(r); lv.frob(f)
		x0.mul(lv)
		lv.frob(f)
		x1.mul(lv)
		lv.frob(f)
		x3.copy(lv); x3.conj(); x0.mul(x3)

		r.copy(r.pow(x))
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            r.conj()
        }  
		x0.mul(r)
		lv.copy(r); lv.frob(f); lv.frob(f)
		x3.copy(lv); x3.conj(); x0.mul(x3)
		lv.frob(f)
		x1.mul(lv)

		r.copy(r.pow(x))
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            r.conj()
        }          
		lv.copy(r); lv.frob(f)
		x3.copy(lv); x3.conj(); x0.mul(x3)
		lv.frob(f)
		x1.mul(lv)

		r.copy(r.pow(x))
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            r.conj()
        }          
		x3.copy(r); x3.conj(); x0.mul(x3)
		lv.copy(r); lv.frob(f)
		x1.mul(lv)

		r.copy(r.pow(x))
        if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
            r.conj()
        }          
		x1.mul(r)

		x0.usqr()
		x0.mul(x1)
		r.copy(x0)
		r.reduce() */
	}
        return r
    }
    
    // GLV method
    static func glv(_ e:BIG) -> [BIG]
    {
	var u=[BIG]();
	if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
/* PFBNS
		var t=BIG(0)
		let q=BIG(ROM.CURVE_Order)
		var v=[BIG]();
		for _ in 0 ..< 2
		{
			u.append(BIG(0))
			v.append(BIG(0))
		}
        
		for i in 0 ..< 2
		{
			t.copy(BIG(ROM.CURVE_W[i]))
			var d=BIG.mul(t,e)
			v[i].copy(d.div(q))
		}
		u[0].copy(e);
		for i in 0 ..< 2
		{
			for j in 0 ..< 2
			{
				t.copy(BIG(ROM.CURVE_SB[j][i]))
				t.copy(BIG.modmul(v[j],t,q))
				u[i].add(q)
				u[i].sub(t)
				u[i].mod(q)
			}
		}
PFBNF */
	} else { // -(x^2).P = (Beta.x,y)
		let q=BIG(ROM.CURVE_Order)
		let x=BIG(ROM.CURVE_Bnx)
		let x2=BIG.smul(x,x)
		u.append(BIG(e))
		u[0].mod(x2)
		u.append(BIG(e))
		u[1].div(x2)
		u[1].rsub(q)

	}
        return u
    }
    // Galbraith & Scott Method
    static func gs(_ e:BIG) -> [BIG]
    {
        var u=[BIG]();
        if CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN {
/* PFBNS
		  var t=BIG(0)
		  let q=BIG(ROM.CURVE_Order)
		  var v=[BIG]();
		  for _ in 0 ..< 4
		  {
			 u.append(BIG(0))
			 v.append(BIG(0))
		  }
        
		  for i in 0 ..< 4
		  {
			 t.copy(BIG(ROM.CURVE_WB[i]))
			 var d=BIG.mul(t,e)
			 v[i].copy(d.div(q))
		  }
		  u[0].copy(e);
		  for i in 0 ..< 4
		  {
			for j in 0 ..< 4
			{
				t.copy(BIG(ROM.CURVE_BB[j][i]))
				t.copy(BIG.modmul(v[j],t,q))
				u[i].add(q)
				u[i].sub(t)
				u[i].mod(q)
			}

		  }
PFBNF */
	} else {
            let q=BIG(ROM.CURVE_Order)        
            let x=BIG(ROM.CURVE_Bnx)
            var w=BIG(e)
            for i in 0 ..< 3
            {
			     u.append(BIG(w))
			     u[i].mod(x)
			     w.div(x)
            }
            u.append(BIG(w))
            if CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX {
                u[1].copy(BIG.modneg(u[1],q))
                u[3].copy(BIG.modneg(u[3],q))                
            }        
        }
        return u
    }	
    
    // Multiply P by e in group G1
    static public func G1mul(_ P:ECP,_ e:BIG) -> ECP
    {
        var R:ECP
        if (CONFIG_CURVE.USE_GLV)
        {
            R=ECP()
            R.copy(P)
            var Q=ECP()
            Q.copy(P); Q.affine()
            let q=BIG(ROM.CURVE_Order)
            let cru=FP(BIG(ROM.CRu))
            var t=BIG(0)
            var u=PAIR.glv(e)
            Q.mulx(cru);
    
            var np=u[0].nbits()
            t.copy(BIG.modneg(u[0],q))
            var nn=t.nbits()
            if (nn<np)
            {
				u[0].copy(t)
				R.neg()
            }
    
            np=u[1].nbits()
            t.copy(BIG.modneg(u[1],q))
            nn=t.nbits()
            if (nn<np)
            {
				u[1].copy(t)
				Q.neg()
            }
            u[0].norm()
            u[1].norm()
            R=R.mul2(u[0],Q,u[1])
        }
        else
        {
            R=P.mul(e)
        }
        return R
    }
    
    // Multiply P by e in group G2
    static public func G2mul(_ P:ECP2,_ e:BIG) -> ECP2
    {
        var R:ECP2
        if (CONFIG_CURVE.USE_GS_G2)
        {
            var Q=[ECP2]()
            var f=FP2(BIG(ROM.Fra),BIG(ROM.Frb));
            let q=BIG(ROM.CURVE_Order);
            var u=PAIR.gs(e);
    
            if CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE {  
                let pNIL:FP?=nil   
                f.inverse(pNIL)
                f.norm()
            }

            var t=BIG(0)
            Q.append(ECP2())
            Q[0].copy(P);
            for i in 1 ..< 4
            {
                Q.append(ECP2()); Q[i].copy(Q[i-1]);
				Q[i].frob(f);
            }
            for i in 0 ..< 4
            {
				let np=u[i].nbits()
				t.copy(BIG.modneg(u[i],q))
				let nn=t.nbits()
				if (nn<np)
				{
                    u[i].copy(t)
                    Q[i].neg()
				}
                u[i].norm()
            }
    
            R=ECP2.mul4(Q,u)
        }
        else
        {
            R=P.mul(e)
        }
        return R;
    }
    // f=f^e
    // Note that this method requires a lot of RAM! Better to use compressed XTR method, see FP4.java
    static public func GTpow(_ d:FP12,_ e:BIG) -> FP12
    {
        var r:FP12
        if (CONFIG_CURVE.USE_GS_GT)
        {
            var g=[FP12]()
            let f=FP2(BIG(ROM.Fra),BIG(ROM.Frb))
            let q=BIG(ROM.CURVE_Order)
            var t=BIG(0)
        
            var u=gs(e)
            g.append(FP12())
            g[0].copy(d);
            for i in 1 ..< 4
            {
                g.append(FP12()); g[i].copy(g[i-1])
				g[i].frob(f)
            }
            for i in 0 ..< 4
            {
				let np=u[i].nbits()
				t.copy(BIG.modneg(u[i],q))
				let nn=t.nbits()
				if (nn<np)
				{
                    u[i].copy(t)
                    g[i].conj()
				}
                u[i].norm()                
            }
            r=FP12.pow4(g,u)
        }
        else
        {
            r=d.pow(e)
        }
        return r
    }

    // test G1 group membership */
    static public func G1member(_ P:ECP) -> Bool
    {
        let q=BIG(ROM.CURVE_Order)
        if P.is_infinity() {return false}
        let W=G1mul(P,q)
        if !W.is_infinity() {return false}
        return true
    }
    // test G2 group membership */
    static public func G2member(_ P:ECP2) -> Bool
    {
        let q=BIG(ROM.CURVE_Order)
        if P.is_infinity() {return false}
        let W=G2mul(P,q)
        if !W.is_infinity() {return false}
        return true
    }

    // test GT group membership
    // Check that m!=1, conj(m)*m==1, and m.m^{p^4}=m^{p^2}

    static public func GTmember(_ m:FP12) -> Bool
    {
        if m.isunity() {return false}
        var r=FP12(m)
        r.conj()
        r.mul(m)
        if !r.isunity() {return false}
    
        let f=FP2(BIG(ROM.Fra),BIG(ROM.Frb))

    
        r.copy(m); r.frob(f); r.frob(f)
        var w=FP12(r); w.frob(f); w.frob(f)
        w.mul(m)
        if !w.equals(r) {return false}

        let q=BIG(ROM.CURVE_Order)
        w.copy(m)
        r.copy(GTpow(w,q))
        if !r.isunity() {return false}
        return true
    }

}

