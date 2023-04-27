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

/* EDDSA API Functions */

package org.miracl.core.XXX;
import org.miracl.core.RAND;
import org.miracl.core.SHA3;
import org.miracl.core.HASH512;

public final class EDDSA {
    public static final int INVALID_PUBLIC_KEY = -2;
    public static final int ERROR = -3;

// Transform a point multiplier to RFC7748 form
    private static void RFC7748(BIG r)
    {
        int c,lg=0;
        BIG t=new BIG(1);
        c=ROM.CURVE_Cof_I;
        while (c!=1) {
            lg++;
            c/=2;
        }
        int n=8*CONFIG_BIG.MODBYTES-lg+1;
        r.mod2m(n);
        t.shl(n);
        r.add(t);
        c=r.lastbits(lg);
        r.dec(c);
    }

    // reverse first n bytes of buff - for little endian
    private static void reverse(int n,byte[] buff) {
        for (int i=0;i<n/2;i++) { 
            byte ch = buff[i]; 
            buff[i] = buff[n - i - 1]; 
            buff[n - i - 1] = ch; 
        }    
    }

    // dom - domain function
    private static byte[] dom(String pp,boolean ph,byte cl) {
        byte[] PP = pp.getBytes();
        int len=PP.length;
        byte[] dom = new byte[len+2];
        for (int i=0;i<len;i++ )
            dom[i]=PP[i];
        if (ph) dom[len]=1;
        else dom[len]=0;
        dom[len+1]=cl;
        return dom;
    }

    private static byte[] H(byte[] S) {
        if (CONFIG_CURVE.AESKEY<=16) { // for ed25519?
            HASH512 sh = new HASH512();
            for (int i=0;i<S.length;i++)
                sh.process(S[i]);
            return sh.hash();
        } else {                       // for ed448?
            byte[] digest = new byte[2*S.length];
            SHA3 sh=new SHA3(SHA3.SHAKE256);
            for (int i=0;i<S.length;i++)
                sh.process(S[i]);
            sh.shake(digest,2*S.length);
            return digest;
        }
    }

    private static DBIG H2(boolean ph,byte[] ctx,byte[] R,byte[] Q,byte[] M)  {
        int b=Q.length;
        byte cl;
        if (ctx==null) cl=0;
        else cl=(byte)ctx.length; 
        if (CONFIG_CURVE.AESKEY<=16) { // Ed25519??
            HASH512 sh = new HASH512();
            if (ph || cl>0)
            {                   // if not prehash and no context, omit dom2()
                byte[] domain=dom("SigXxx no Xxx collisions",ph,cl);
                for (int i=0;i<domain.length;i++)
                    sh.process(domain[i]);
                for (int i=0;i<cl;i++)
                    sh.process(ctx[i]);
            }
            for (int i=0;i<b;i++ )
                sh.process(R[i]);
            for (int i=0;i<b;i++ )
                sh.process(Q[i]);
            for (int i=0;i<M.length;i++ )
                sh.process(M[i]);
            byte[] h=sh.hash();
            reverse(64,h);
            return DBIG.fromBytes(h);
       } else {                       // for ed448?
            byte[] domain=dom("SigXxx",ph,cl);
            byte[] h=new byte[2*b];
            SHA3 sh=new SHA3(SHA3.SHAKE256);
            for (int i=0;i<domain.length;i++)
                sh.process(domain[i]);
            for (int i=0;i<cl;i++)
                sh.process(ctx[i]);
            for (int i=0;i<b;i++) 
                sh.process(R[i]);
            for (int i=0;i<b;i++) 
                sh.process(Q[i]);
            for (int i=0;i<M.length;i++) 
                sh.process(M[i]);
            sh.shake(h,2*b);
            reverse(2*b,h);
            return DBIG.fromBytes(h);
        }
    }

    private static DBIG getR(boolean ph,int b,byte[] digest,byte[] ctx,byte[] M) {
        byte[] domain;
        byte cl;
        if (ctx==null) cl=0;
        else cl=(byte)ctx.length;

        if (CONFIG_CURVE.AESKEY<=16) { // Ed25519??
            HASH512 sh = new HASH512();
            if (ph || cl>0)
            {                   // if not prehash and no context, omit dom2()
                domain=dom("SigXxx no Xxx collisions",ph,cl);
                for (int i=0;i<domain.length;i++)
                    sh.process(domain[i]);
                for (int i=0;i<cl;i++)
                    sh.process(ctx[i]);
            }
            for (int i=b;i<2*b;i++ )
                sh.process(digest[i]);
            for (int i=0;i<M.length;i++)
                sh.process(M[i]);
            byte[] h=sh.hash();
            reverse(64,h);
            return DBIG.fromBytes(h);
        } else {                       // for ed448?
            domain=dom("SigXxx",ph,cl);
            SHA3 sh=new SHA3(SHA3.SHAKE256);
            byte[] h=new byte[2*b];
            for (int i=0;i<domain.length;i++)
                sh.process(domain[i]);
            for (int i=0;i<cl;i++)
                sh.process(ctx[i]);
            for (int i=b;i<2*b;i++ )
                sh.process(digest[i]);
            for (int i=0;i<M.length;i++) 
                sh.process(M[i]);
            sh.shake(h,2*b); 
            reverse(2*b,h);
            return DBIG.fromBytes(h);
        }
    }

// encode integer (little endian)
    private static int encode_int(BIG x,byte[] w) {
        int b,index=0;
        if (8*CONFIG_BIG.MODBYTES==CONFIG_FIELD.MODBITS) index=1; // extra byte needed for compression        
        b=CONFIG_BIG.MODBYTES+index;

        w[0]=0;
        x.tobytearray(w,index);
        reverse(b,w);
        return b;
    }

// encode point
    private static void encode(ECP P,byte[] w) {
        int b,index=0;
        if (8*CONFIG_BIG.MODBYTES==CONFIG_FIELD.MODBITS) index=1; // extra byte needed for compression        
        b=CONFIG_BIG.MODBYTES+index;

        BIG x=P.getX();
        BIG y=P.getY();
        encode_int(y,w);
        w[b-1]|=x.parity()<<7;
    }

// get sign
    private static int getsign(byte[] x) {
        int b,index=0;
        if (8*CONFIG_BIG.MODBYTES==CONFIG_FIELD.MODBITS) index=1; // extra byte needed for compression        
        b=CONFIG_BIG.MODBYTES+index;  
        if ((x[b-1]&0x80)!=0)
            return 1;
        else 
            return 0;
    }

// decode integer (little endian)
    private static BIG decode_int(boolean strip_sign,byte[] ei) {
        int b,index=0;

        if (8*CONFIG_BIG.MODBYTES==CONFIG_FIELD.MODBITS) index=1; // extra byte needed for compression        
        b=CONFIG_BIG.MODBYTES+index;

        byte[] r=new byte[b];

        for (int i=0;i<b;i++)
            r[i]=ei[i];
        reverse(b,r);

        if (strip_sign)
            r[0]&=0x7f;

        return BIG.frombytearray(r,index);
    }

// decode compressed point
    private static ECP decode(byte[] W) {
        ECP P=new ECP();
        int sign=getsign(W); // lsb of x
        BIG y=decode_int(true,W);
        FP one=new FP(1);
        FP hint=new FP();
        FP x=new FP(y); x.sqr(); 
        FP d=new FP(x); 
        x.sub(one);
        x.norm();
        FP t=new FP(new BIG(ROM.CURVE_B));
        d.mul(t);
        if (CONFIG_CURVE.CURVE_A==1) d.sub(one);
        if (CONFIG_CURVE.CURVE_A==-1) d.add(one);
        d.norm();
// inverse square root trick for sqrt(x/d)
        t.copy(x);
        t.sqr();
        x.mul(t);
        x.mul(d);
        if (x.qr(hint)!=1)
        {
            P.inf();
            return P;
        }
        d.copy(x.sqrt(hint));
        x.inverse(hint);
        x.mul(d);
        x.mul(t);
        x.reduce();
        if (x.redc().parity()!=sign)
            x.neg();
        x.norm();
        return new ECP(x.redc(),y);
    }

/* Calculate a public/private EC GF(p) key pair. Q=D.G mod EC(p),
 * where D is the secret key and Q is the public key
 * and G is fixed generator.
 * RNG is a cryptographically strong RNG 
 * If RNG==NULL, D is provided externally 
 */
    public static int KEY_PAIR_GENERATE(RAND RNG, byte[] D, byte[] Q) {
        int res = 0;
        int b,index=0;

        if (8*CONFIG_BIG.MODBYTES==CONFIG_FIELD.MODBITS) index=1; // extra byte needed for compression        
        b=CONFIG_BIG.MODBYTES+index;

        ECP G=ECP.generator();

        if (RNG != null) {
            for (int i=0;i<b;i++)
                D[i]=(byte)RNG.getByte();
        }

        byte [] digest=H(D);

// reverse bytes for little endian
        reverse(b,digest);
        BIG s=BIG.frombytearray(digest,index);
        RFC7748(s);
        G.copy(G.mul(s));
        encode(G,Q);
        return res;
    }

// Generate a signature using key pair (D,Q) on message M
// Set ph=true if message has already been pre-hashed
// if ph=false, then context should be NULL for ed25519. However RFC8032 mode ed25519ctx is supported by supplying a non-NULL or non-empty context
    public static int SIGNATURE(boolean ph,byte[] D, byte[] ctx, byte[] M, byte[] SIG) {
        byte [] digest=H(D);   // hash of private key
        int res = 0;
        int b,index=0;
        if (8*CONFIG_BIG.MODBYTES==CONFIG_FIELD.MODBITS) index=1; // extra byte needed for compression        
        b=CONFIG_BIG.MODBYTES+index;
        byte [] S=new byte[b];

        byte[] Q=new byte[b];
        KEY_PAIR_GENERATE(null,D,Q);

        BIG q = new BIG(ROM.CURVE_Order);
        if (D.length!=Q.length || D.length!=b)
            res=INVALID_PUBLIC_KEY;
        if (res==0)
        {
            DBIG dr=getR(ph,b,digest,ctx,M);
            BIG sr=dr.mod(q);
            ECP R=ECP.generator().mul(sr);
            encode(R,S);
            for (int i=0;i<b;i++)
                SIG[i]=S[i];
// reverse bytes for little endian        
            reverse(b,digest);
            BIG s=BIG.frombytearray(digest,index);
            RFC7748(s);
            dr=H2(ph,ctx,SIG,Q,M);
            BIG sd=dr.mod(q);
            encode_int(BIG.modadd(sr,BIG.modmul(s,sd,q),q),S);
            for (int i=0;i<b;i++)
                SIG[b+i]=S[i];
        }
        return res;
    }

// verify a signature using public key Q
// same context (if any) as used for signature
    public static boolean VERIFY(boolean ph,byte[] Q, byte[] ctx,byte[] M, byte[] SIG) {
        int lg=0;
        int c,b,index=0;
        if (8*CONFIG_BIG.MODBYTES==CONFIG_FIELD.MODBITS) index=1; // extra byte needed for compression        
        b=CONFIG_BIG.MODBYTES+index;
        byte[] S=new byte[b];

        c=ROM.CURVE_Cof_I;
        while (c!=1) {
            lg++;
            c/=2;
        }
        BIG q = new BIG(ROM.CURVE_Order);
        ECP R=decode(SIG);
        if (R.is_infinity()) return false;
        for (int i=0;i<b;i++)
            S[i]=SIG[b+i];
        BIG t=decode_int(false,S);
        if (BIG.comp(t,q)>=0) return false;
        DBIG du=H2(ph,ctx,SIG,Q,M);
        BIG su=du.mod(q);

        ECP G=ECP.generator();
        ECP QD=decode(Q); 
        if (QD.is_infinity()) return false;
        QD.neg();
        for (int i=0;i<lg;i++)
        { // use cofactor 2^c
            G.dbl(); QD.dbl(); R.dbl();
        }
        
        if (!G.mul2(t,QD,su).equals(R)) return false;
        return true;
    }
}
