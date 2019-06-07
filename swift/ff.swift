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
//  ff.swift
//
//  Created by Michael Scott on 24/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Large Finite Field arithmetic */
/* AMCL mod p functions */

import amcl

final public class FF {

    var v = [BIG]()
    var length:Int=1

/* calculate Field Excess */
    static func EXCESS(_ a: BIG) -> Chunk
    {
        return ((a.w[CONFIG_BIG.NLEN-1] & CONFIG_FF.P_OMASK)>>Chunk(CONFIG_FF.P_MBITS%CONFIG_BIG.BASEBITS))+1
    }

#if D32
    static func pexceed(_ a: BIG,_ b : BIG) -> Bool
    {
        let ea=FF.EXCESS(a)
        let eb=FF.EXCESS(b)
        if (DChunk(ea)+1)*(DChunk(eb)+1) > DChunk(CONFIG_FF.P_FEXCESS) {return true}
        return false;
    }
    static func sexceed(_ a: BIG) -> Bool
    {
        let ea=FF.EXCESS(a)
        if (DChunk(ea)+1)*(DChunk(ea)+1) > DChunk(CONFIG_FF.P_FEXCESS) {return true}
        return false;
    }
#endif
#if D64
    static func pexceed(_ a: BIG,_ b : BIG) -> Bool
    {
        let ea=FF.EXCESS(a)
        let eb=FF.EXCESS(b)
        if (ea+1) > CONFIG_FF.P_FEXCESS/(eb+1) {return true}
        return false;
    }
    static func sexceed(_ a: BIG) -> Bool
    {
        let ea=FF.EXCESS(a)
        if (ea+1) > CONFIG_FF.P_FEXCESS/(ea+1) {return true}
        return false;
    }
#endif
    

    /* Constructors */
    init(_ n: Int)
    {
        for _ in 0 ..< n
        {
            v.append(BIG(0));
        }
        length=n;
    }
    
    init(_ x: [[Chunk]],n: Int)
    {
        for i in 0 ..< n
        {
            v.append(BIG(x[i]))
        }
        length=n;
    }
    
    func getlen() -> Int
    {
        return length;
    }
    
    /* set to zero */
    func zero()
    {
        for i in 0 ..< length
        {
            v[i].zero();
        }
    }
    
    /* set to integer */
    func set(_ m: Int)
    {
        zero();
        v[0].set(0,Chunk(m));
    }
    
    /* copy from FF b */
    func copy(_ b: FF)
    {
        for i in 0 ..< length
        {
            v[i].copy(b.v[i]);
        }
    }
    
    /* x=y<<n */
    func dsucopy(_ b: FF)
    {
        for i in 0 ..< b.length
        {
            v[b.length+i].copy(b.v[i]);
            v[i].zero();
        }
    }
    /* x=y */
    func dscopy(_ b: FF)
    {
        for i in 0 ..< b.length
        {
            v[i].copy(b.v[i]);
            v[b.length+i].zero();
        }
    }
    /* x=y>>n */
    func sducopy(_ b: FF)
    {
        for i in 0 ..< length
        {
            v[i].copy(b.v[length+i]);
        }
    }
    func one()
    {
        v[0].one();
        for i in 1 ..< length
        {
            v[i].zero();
        }
    }
    /* test equals 0 */
    func iszilch() -> Bool
    {
        for i in 0 ..< length
        {
            if (!v[i].iszilch()) {return false}
        }
        return true;
    }
    /* shift right by BIGBITS-bit words */
    func shrw(_ n: Int)
    {
        for i in 0 ..< n
        {
            v[i].copy(v[i+n]);
            v[i+n].zero();
        }
    }
    
    /* shift left by BIGBITS-bit words */
    func shlw(_ n: Int)
    {
        for i in 0 ..< n
        {
        v[n+i].copy(v[i]);
        v[i].zero();
        }
    }
    
    /* extract last bit */
    func parity() -> Int
    {
        return v[0].parity()
    }
    
    func lastbits(_ m: Int) ->Int
    {
        return v[0].lastbits(UInt(m));
    }
    
    /* compare x and y - must be normalised, and of same length */
    static func comp(_ a: FF,_ b:FF) -> Int
    {
        for i in (0...a.length-1).reversed()
       // for var i=a.length-1;i>=0;i--
        {
            let j=BIG.comp(a.v[i],b.v[i])
            if j != 0 {return j}
        }
        return 0;
    }
    /* recursive add */
    func radd(_ vp: Int,_ x:FF,_ xp:Int,_ y:FF,_ yp:Int,_ n: Int)
    {
        for i in 0 ..< n
        {
            v[vp+i].copy(x.v[xp+i])
            v[vp+i].add(y.v[yp+i])
        }
    }
    /* recursive inc */
    func rinc(_ vp: Int,_ y: FF,_ yp: Int,_ n:Int)
    {
        for i in 0 ..< n
        {
            v[vp+i].add(y.v[yp+i])
        }
    }
    /* recursive add */
    func rsub(_ vp: Int,_ x:FF,_ xp:Int,_ y:FF,_ yp:Int,_ n: Int)
    {
        for i in 0 ..< n
        {
            v[vp+i].copy(x.v[xp+i])
            v[vp+i].sub(y.v[yp+i])
        }
    }
    /* recursive inc */
    func rdec(_ vp: Int,_ y: FF,_ yp: Int,_ n:Int)
    {
        for i in 0 ..< n
        {
            v[vp+i].sub(y.v[yp+i])
        }
    }
    /* simple add */
    func add(_ b: FF)
    {
        for i in 0 ..< length
            {v[i].add(b.v[i])}
    }
    
    /* simple sub */
    func sub(_ b: FF)
    {
        for i in 0 ..< length
            {v[i].sub(b.v[i])}
    }
    /* reverse sub */
    func revsub(_ b: FF)
    {
        for i in 0 ..< length
            {v[i].rsub(b.v[i])}
    }
    /* normalise - but hold any overflow in top part unless n<0 */
    private func rnorm(_ vp: Int,_ n: Int)
    {
        var trunc=false;
        var nn=n
   
        if (nn<0)
        { /* -v n signals to do truncation */
            nn = -nn
            trunc=true;
        }
        for i in 0 ..< nn-1
        {
            let carry=v[vp+i].norm()
            v[vp+i].xortop(carry<<Chunk(CONFIG_FF.P_TBITS))
            v[vp+i+1].w[0]+=carry
        }
        let carry=v[vp+nn-1].norm()
        if (trunc)
            {v[vp+nn-1].xortop(carry<<Chunk(CONFIG_FF.P_TBITS))}
    }
    
    func norm()
    {
        rnorm(0,length)
    }
    
    /* increment/decrement by a small integer */
    func inc(_ m: Int)
    {
        v[0].inc(m)
        norm()
    }
    
    func dec(_ m: Int)
    {
        v[0].dec(m)
        norm()
    }
    
    /* shift left by one bit */
    func shl()
    {
        var delay_carry=0;
        for i in 0 ..< length-1
        {
            let carry=v[i].fshl(1)
            v[i].inc(delay_carry);
            v[i].xortop(Chunk(carry)<<Chunk(CONFIG_FF.P_TBITS));
            delay_carry=carry;
        }
        v[length-1].fshl(1)
        v[length-1].inc(delay_carry)
    }
    
    /* shift right by one bit */
    func shr()
    {
        for i in (1..<length).reversed()
        {
            let carry=v[i].fshr(1);
            v[i-1].ortop(Chunk(carry)<<Chunk(CONFIG_FF.P_TBITS));
        }
        v[0].fshr(1);
    }
    
    /* Convert to Hex String */
    func toString() -> String
    {
        norm();
        var s="";
        for i in (0...length-1).reversed()
        {
            s+=v[i].toString();
        }
        return s;
    }
    
    /* Convert FFs to/from byte arrays */
    func toBytes(_ b: inout [UInt8])
    {
        for i in 0 ..< length
        {
            v[i].tobytearray(&b,(length-i-1)*Int(CONFIG_BIG.MODBYTES))
        }
    }
    static func fromBytes(_ x: FF,_ b:[UInt8])
    {
        for i in 0 ..< x.length
        {
            x.v[i]=BIG.frombytearray(b,(x.length-i-1)*Int(CONFIG_BIG.MODBYTES))
        }
    }
    
    /* in-place swapping using xor - side channel resistant - lengths must be the same */
    private static func cswap(_ a: FF,_ b:FF,_ d:Int)
    {
        for i in 0 ..< a.length
        {
            a.v[i].cswap(&(b.v[i]),d)
        }
    }
    /* z=x*y, t is workspace */
    private func karmul(_ vp: Int,_ x: FF,_ xp: Int,_ y:FF,_ yp: Int,_ t:FF,_ tp:Int,_ n:Int)
    {
        if (n==1)
        {
            x.v[xp].norm(); y.v[yp].norm()
            var d=BIG.mul(x.v[xp],y.v[yp])
            v[vp+1]=d.split(8*CONFIG_BIG.MODBYTES)
            v[vp].copy(d)
            return
        }
        let nd2=n/2
        radd(vp,x,xp,x,xp+nd2,nd2)
        rnorm(vp,nd2)
        radd(vp+nd2,y,yp,y,yp+nd2,nd2)
        rnorm(vp+nd2,nd2)
        
        t.karmul(tp,self,vp,self,vp+nd2,t,tp+n,nd2)
        karmul(vp,x,xp,y,yp,t,tp+n,nd2)
        karmul(vp+n,x,xp+nd2,y,yp+nd2,t,tp+n,nd2)
        t.rdec(tp,self,vp,n)
        t.rdec(tp,self,vp+n,n)
        rinc(vp+nd2,t,tp,n)
        rnorm(vp,2*n)
    }
    
    private func karsqr(_ vp: Int,_ x: FF,_ xp:Int,_ t:FF,_ tp:Int,_ n:Int)
    {
        if (n==1)
        {
            x.v[xp].norm()
            var d=BIG.sqr(x.v[xp])
            v[vp+1].copy(d.split(8*CONFIG_BIG.MODBYTES))
            v[vp].copy(d);
            return;
        }
    
        let nd2=n/2
        karsqr(vp,x,xp,t,tp+n,nd2)
        karsqr(vp+n,x,xp+nd2,t,tp+n,nd2)
        t.karmul(tp,x,xp,x,xp+nd2,t,tp+n,nd2)
        rinc(vp+nd2,t,tp,n)
        rinc(vp+nd2,t,tp,n)
        rnorm(vp+nd2,n)
    }
    private func karmul_lower(_ vp:Int,_ x:FF,_ xp:Int,_ y:FF,_ yp:Int,_ t:FF,_ tp:Int,_ n: Int)
    { /* Calculates Least Significant bottom half of x*y */
        if (n==1)
        { /* only calculate bottom half of product */
            v[vp].copy(BIG.smul(x.v[xp],y.v[yp]))
            return
        }
        let nd2=n/2
    
        karmul(vp,x,xp,y,yp,t,tp+n,nd2)
        t.karmul_lower(tp,x,xp+nd2,y,yp,t,tp+n,nd2);
        rinc(vp+nd2,t,tp,nd2);
        t.karmul_lower(tp,x,xp,y,yp+nd2,t,tp+n,nd2);
        rinc(vp+nd2,t,tp,nd2);
        rnorm(vp+nd2,-nd2);  /* truncate it */
    }
    
    private func karmul_upper(_ x: FF,_ y:FF,_ t:FF,_ n:Int)
    { /* Calculates Most Significant upper half of x*y, given lower part */
        let nd2=n/2;
        radd(n,x,0,x,nd2,nd2);
        radd(n+nd2,y,0,y,nd2,nd2);
	rnorm(n,nd2);
	rnorm(n+nd2,nd2);
    
        t.karmul(0,self,n+nd2,self,n,t,n,nd2);  /* t = (a0+a1)(b0+b1) */
        karmul(n,x,nd2,y,nd2,t,n,nd2);          /* z[n]= a1*b1 */
    /* z[0-nd2]=l(a0b0) z[nd2-n]= h(a0b0)+l(t)-l(a0b0)-l(a1b1) */
        t.rdec(0,self,n,n);                     /* t=t-a1b1  */
        rinc(nd2,self,0,nd2);                   /* z[nd2-n]+=l(a0b0) = h(a0b0)+l(t)-l(a1b1)  */
        rdec(nd2,t,0,nd2);                      /* z[nd2-n]=h(a0b0)+l(t)-l(a1b1)-l(t-a1b1)=h(a0b0) */
        rnorm(0,-n);                            /* a0b0 now in z - truncate it */
        t.rdec(0,self,0,n);                     /* (a0+a1)(b0+b1) - a0b0 */
        rinc(nd2,t,0,n);
    
        rnorm(nd2,n);
    }
    /* z=x*y. Assumes x and y are of same length. */
    static func mul(_ x: FF,_ y:FF) -> FF
    {
        let n=x.length
        let z=FF(2*n)
        let t=FF(2*n)
        z.karmul(0,x,0,y,0,t,0,n)
        return z
    }
    
    /* z=x^2 */
    static func sqr(_ x: FF) -> FF
    {
        let n=x.length
        let z=FF(2*n)
        let t=FF(2*n)
        z.karsqr(0,x,0,t,0,n)
        return z
    }
    /* return low part of product self*y */
    func lmul(_ y: FF)
    {
        let n=length;
        let t=FF(2*n);
        let x=FF(n); x.copy(self);
        karmul_lower(0,x,0,y,0,t,0,n);
    }
    
    /* Set b=b mod c */
    func mod(_ c: FF)
    {
        var k=0
    
        norm()
        if (FF.comp(self,c)<0)
            {return}
        repeat
        {
            c.shl()
            k+=1
        } while (FF.comp(self,c)>=0)
    
        while (k>0)
        {
            c.shr();
            if (FF.comp(self,c)>=0)
            {
				sub(c)
				norm()
            }
            k-=1
        }
    }
    
    /* return This mod modulus, N is modulus, ND is Montgomery Constant */
    func reduce(_ N: FF,_ ND:FF) -> FF
    { /* fast karatsuba Montgomery reduction */
        let n=N.length
        let t=FF(2*n)
        let r=FF(n)
        let m=FF(n)
    
        r.sducopy(self)
        m.karmul_lower(0,self,0,ND,0,t,0,n)
        karmul_upper(N,m,t,n)
        m.sducopy(self)
    
        r.add(N);
        r.sub(m);
        r.norm();
    
        return r;
    }
    /* Set r=this mod b */
    /* this is of length - 2*n */
    /* r,b is of length - n */
    func dmod(_ b: FF) -> FF
    {
        let n=b.length
        let m=FF(2*n)
        let x=FF(2*n)
        let r=FF(n)
    
        x.copy(self)
        x.norm()
        m.dsucopy(b)
        var k=Int(CONFIG_BIG.BIGBITS)*n
    
        while (FF.comp(x,m)>=0)
        {
		x.sub(m);
		x.norm();
        }

        while (k>0)
        {
            m.shr()
    
            if (FF.comp(x,m)>=0)
            {
				x.sub(m);
				x.norm();
            }
            k -= 1;
        }
    
        r.copy(x);
        r.mod(b);
        return r;
    }
    /* Set return=1/this mod p. Binary method - a<p on entry */
    
    func invmodp(_ p: FF)
    {
        let n=p.length;
    
        let u=FF(n)
        let v=FF(n)
        let x1=FF(n)
        let x2=FF(n)
        let t=FF(n)
        let one=FF(n)
    
        one.one()
        u.copy(self)
        v.copy(p)
        x1.copy(one)
        x2.zero()
    
    // reduce n in here as well!
        while (FF.comp(u,one) != 0 && FF.comp(v,one) != 0)
        {
            while (u.parity()==0)
            {
                u.shr()
                if (x1.parity() != 0)
                {
                    x1.add(p)
                    x1.norm()
                }
                x1.shr()
            }
            while (v.parity()==0)
            {
                v.shr()
                if (x2.parity() != 0)
                {
                    x2.add(p)
                    x2.norm()
                }
                x2.shr();
            }
            if (FF.comp(u,v)>=0)
            {
                u.sub(v)
                u.norm()
                if (FF.comp(x1,x2)>=0) {x1.sub(x2)}
                else
                {
                    t.copy(p)
                    t.sub(x2)
                    x1.add(t)
                }
                x1.norm()
            }
            else
            {
                v.sub(u)
                v.norm()
                if (FF.comp(x2,x1)>=0) {x2.sub(x1)}
                else
                {
                    t.copy(p)
                    t.sub(x1)
                    x2.add(t)
                }
                x2.norm()
            }
        }
        if FF.comp(u,one)==0
            {copy(x1)}
        else
            {copy(x2)}
    }
    
    /* nresidue mod m */
    func nres(_ m: FF)
    {
        let n=m.length
        if (n==1) {
                var d=DBIG(v[0])
                d.shl(UInt(CONFIG_BIG.NLEN)*CONFIG_BIG.BASEBITS)
                v[0].copy(d.mod(m.v[0]))
            } else {
                let d=FF(2*n)
                d.dsucopy(self)
                copy(d.dmod(m))
            }
    }
    
    func redc(_ m: FF,_ ND:FF)
    {
        let n=m.length
        if (n==1) {
                var d=DBIG(v[0])
                v[0].copy(BIG.monty(m.v[0],(Chunk(1)<<Chunk(CONFIG_BIG.BASEBITS))-ND.v[0].w[0],&d))
            } else {
                let d=FF(2*n)
                mod(m)
                d.dscopy(self)
                copy(d.reduce(m,ND))
                mod(m)
            }
    }
    private func mod2m(_ m: Int)
    {
        for i in m ..< length
            {v[i].zero()}
    }
    /* U=1/a mod 2^m - Arazi & Qi */
    private func invmod2m() -> FF
    {
        let n=length;
    
        let b=FF(n);
        let c=FF(n);
        let U=FF(n);
    
        U.zero();
        U.v[0].copy(v[0]);
        U.v[0].invmod2m();
    
        var i=1
        while (i<n)
        {
            b.copy(self); b.mod2m(i);
            let t=FF.mul(U,b); t.shrw(i); b.copy(t);
            c.copy(self); c.shrw(i); c.mod2m(i);
            c.lmul(U); c.mod2m(i);
    
            b.add(c); b.norm();
            b.lmul(U); b.mod2m(i);
    
            c.one(); c.shlw(i); b.revsub(c); b.norm();
            b.shlw(i);
            U.add(b);
            i<<=1
        }
        U.norm();
        return U;
    }
    
    func random(_ rng: inout RAND)
    {
        let n=length;
        for i in 0 ..< n
        {
            v[i].copy(BIG.random(&rng));
        }
    /* make sure top bit is 1 */
        while (v[n-1].nbits()<Int(CONFIG_BIG.MODBYTES)*8) {v[n-1].copy(BIG.random(&rng))}
    }
    /* generate random x */
    func randomnum(_ p: FF,_ rng: inout RAND)
    {
        let n=length;
        let d=FF(2*n);
    
        for i in 0 ..< 2*n
        {
            d.v[i].copy(BIG.random(&rng));
        }
        copy(d.dmod(p));
    }
    /* this*=y mod p */
    func modmul(_ y: FF,_ p:FF,_ nd: FF)
    {
        if FF.pexceed(v[length-1],y.v[y.length-1]) {mod(p)}
        let n=p.length
        if (n==1) {
                var d=BIG.mul(v[0],y.v[0])
                v[0].copy(BIG.monty(p.v[0],(Chunk(1)<<Chunk(CONFIG_BIG.BASEBITS))-nd.v[0].w[0],&d))
            } else {
                let d=FF.mul(self,y);
                copy(d.reduce(p,nd));
            }
    }
    
    /* this*=y mod p */
    func modsqr(_ p: FF,_ nd:FF)
    {
        if FF.sexceed(v[length-1]) {mod(p)}
        let n=p.length
        if (n==1) {
                var d=BIG.sqr(v[0])
                v[0].copy(BIG.monty(p.v[0],(Chunk(1)<<Chunk(CONFIG_BIG.BASEBITS))-nd.v[0].w[0],&d))
            } else {
                let d=FF.sqr(self);
                copy(d.reduce(p,nd));
            }
    }
    
    /* self=self^e mod p using side-channel resistant Montgomery Ladder, for large e */
    func skpow(_ e: FF,_ p:FF)
    {
        let n=p.length
        let R0=FF(n)
        let R1=FF(n)
        let ND=p.invmod2m()
    
        mod(p)
        R0.one()
        R1.copy(self)
        R0.nres(p)
        R1.nres(p)
    
        for i in (0...8*Int(CONFIG_BIG.MODBYTES)*n-1).reversed()
        {
            let b=Int(e.v[i/Int(CONFIG_BIG.BIGBITS)].bit(UInt(i%Int(CONFIG_BIG.BIGBITS))))
            copy(R0)
            modmul(R1,p,ND)
    
            FF.cswap(R0,R1,b)
            R0.modsqr(p,ND)
    
            R1.copy(self)
            FF.cswap(R0,R1,b)
    
        }
    
        copy(R0)
        redc(p,ND)
    }
    
    /* this =this^e mod p using side-channel resistant Montgomery Ladder, for short e */
    func skpow(_ e: BIG,_ p:FF)
    {
        let n=p.length
        let R0=FF(n)
        let R1=FF(n)
        let ND=p.invmod2m()
    
        mod(p)
        R0.one()
        R1.copy(self)
        R0.nres(p)
        R1.nres(p)
    
        for i in (0...8*Int(CONFIG_BIG.MODBYTES)-1).reversed()
        {
            let b=(e.bit(UInt(i)))
            copy(R0)
            modmul(R1,p,ND)
    
            FF.cswap(R0,R1,b)
            R0.modsqr(p,ND)
    
            R1.copy(self)
            FF.cswap(R0,R1,b)
        }
        copy(R0)
        redc(p,ND)
    }
    
    /* raise to an integer power - right-to-left method */
    func power(_ e:Int,_ p:FF)
    {
        let n=p.length
        var f=true
        let w=FF(n)
        let ND=p.invmod2m()
        var ee=e;
    
        w.copy(self)
        w.nres(p)
    
        if (ee==2)
        {
            copy(w)
            modsqr(p,ND)
        }
        else
        {
            while true
            {
                if (ee%2==1)
                {
                    if (f) {copy(w)}
                    else {modmul(w,p,ND)}
                    f=false;
                }
                ee>>=1;
                if (ee==0) {break}
                w.modsqr(p,ND)
            }
        }
        redc(p,ND)
    }
    
    /* this=this^e mod p, faster but not side channel resistant */
    func pow(_ e: FF,_ p:FF)
    {
        let n=p.length
        let w=FF(n)
        let ND=p.invmod2m()
    
        w.copy(self);
        one();
        nres(p);
        w.nres(p);
        for i in (0...8*Int(CONFIG_BIG.MODBYTES)*n-1).reversed()
        {
            modsqr(p,ND)
            let b=e.v[i/Int(CONFIG_BIG.BIGBITS)].bit(UInt(i%Int(CONFIG_BIG.BIGBITS)))
            if (b==1) {modmul(w,p,ND)}
        }
        redc(p,ND);
    }
    /* double exponentiation r=x^e.y^f mod p */
    func pow2(_ e: BIG,_ y:FF,_ f:BIG,_ p:FF)
    {
        let n=p.length
        let xn=FF(n)
        let yn=FF(n)
        let xy=FF(n)
        let ND=p.invmod2m()
    
        xn.copy(self)
        yn.copy(y)
        xn.nres(p)
        yn.nres(p)
        xy.copy(xn); xy.modmul(yn,p,ND)
        one()
        nres(p)
    
        for i in (0...8*Int(CONFIG_BIG.MODBYTES)-1).reversed()
        {
            let eb=e.bit(UInt(i))
            let fb=f.bit(UInt(i))
            modsqr(p,ND)
            if (eb==1)
            {
                if (fb==1) {modmul(xy,p,ND)}
                else {modmul(xn,p,ND)}
            }
            else
            {
                if (fb==1) {modmul(yn,p,ND)}
            }
        }
        redc(p,ND)
    }
    static func igcd(_ x:Int,_ y:Int) -> Int
    { /* integer GCD, returns GCD of x and y */
        var xx=x;
        var yy=y;
        if (yy==0) {return xx}
        while true
        {
            let r=xx%yy; if r==0 {break}
            xx=yy; yy=r;
        }
        return yy;
    }
   
    /* quick and dirty check for common factor with n */
    func cfactor(_ s: Int) -> Bool
    {
        let n=length;
        let x=FF(n);
        let y=FF(n);
        y.set(s);
        
        x.copy(self);
        x.norm();
        
        repeat
        {
            x.sub(y);
            x.norm();
            while ( (!x.iszilch()) && x.parity()==0) {x.shr()}
        } while (FF.comp(x,y)>0);
        let g=x.v[0].get(0);
        let r=FF.igcd(s,Int(g));
        
        if (r>1) {return true}
        return false;
    }
 
    /* Miller-Rabin test for primality. Slow. */
    static func prime(_ p: FF,_ rng: inout RAND) -> Bool
    {
        var s=0
        let n=p.length
        var loop:Bool

        let d=FF(n)
        let x=FF(n)
        let unity=FF(n)
        let nm1=FF(n)
    
        let sf:Int=4849845; /* 3*5*.. *19 */
        p.norm();
        if (p.cfactor(sf)) {return false}
        unity.one();
        nm1.copy(p);
        nm1.sub(unity);
        nm1.norm();
        d.copy(nm1);
        
        while (d.parity()==0)
        {
            d.shr();
            s += 1;
        }
        
        if (s==0) {return false}
        for _ in 0 ..< 10
        {
            x.randomnum(p,&rng)
            x.pow(d,p)
            if (FF.comp(x,unity)==0 || FF.comp(x,nm1)==0) {continue}
            loop=false
            for _ in 1 ..< s
            {
				x.power(2,p);
                if (FF.comp(x,unity)==0) {return false}
				if (FF.comp(x,nm1)==0) {loop=true; break;}
            }
            if (loop) {continue}
            return false;
        }
        return true;
    }

}
