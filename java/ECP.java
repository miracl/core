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

/* Elliptic Curve Point class */

package org.miracl.core.XXX;

public final class ECP {

	private FP x;
	private FP y;
	private FP z;

/* Constructor - set to O */
	public ECP() {
		x=new FP();
		y=new FP(1);
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS)
		{
			z=new FP(1);
		}
		else
		{
			z=new FP();
		}
	}

    public ECP(ECP e) {
        this.x = new FP(e.x);
        this.y = new FP(e.y);
        this.z = new FP(e.z);
    }

/* test for O point-at-infinity */
	public boolean is_infinity() {
//		if (INF) return true;                            // Edits made
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS)
		{
			return (x.iszilch() && y.equals(z));
		}
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.WEIERSTRASS)
		{
			return (x.iszilch() && z.iszilch());
		}
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
		{
			return z.iszilch();
		}
		return true;
	}
/* Conditional swap of P and Q dependant on d */
	private void cswap(ECP Q,int d)
	{
		x.cswap(Q.x,d);
		if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY) y.cswap(Q.y,d);
		z.cswap(Q.z,d);
	}

/* Conditional move of Q to P dependant on d */
	private void cmove(ECP Q,int d)
	{
		x.cmove(Q.x,d);
		if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY) y.cmove(Q.y,d);
		z.cmove(Q.z,d);
	}

/* return 1 if b==c, no branching */
	private static int teq(int b,int c)
	{
		int x=b^c;
		x-=1;  // if x=0, x now -1
		return ((x>>31)&1);
	}

/* Constant time select from pre-computed table */
	private void select(ECP W[],int b)
	{
		ECP MP=new ECP();
		int m=b>>31;
		int babs=(b^m)-m;

		babs=(babs-1)/2;
		cmove(W[0],teq(babs,0));  // conditional move
		cmove(W[1],teq(babs,1));
		cmove(W[2],teq(babs,2));
		cmove(W[3],teq(babs,3));
		cmove(W[4],teq(babs,4));
		cmove(W[5],teq(babs,5));
		cmove(W[6],teq(babs,6));
		cmove(W[7],teq(babs,7));

		MP.copy(this);
		MP.neg();
		cmove(MP,(int)(m&1));
	}

/* Test P == Q */
	public boolean equals(ECP Q) {

		FP a=new FP();                                        // Edits made
		FP b=new FP();
		a.copy(x); a.mul(Q.z);
		b.copy(Q.x); b.mul(z);
		if (!a.equals(b)) return false;
		if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY)
		{
			a.copy(y); a.mul(Q.z);
			b.copy(Q.y); b.mul(z);
			if (!a.equals(b)) return false;
		}
		return true;
	}

/* this=P */
	public void copy(ECP P)
	{
		x.copy(P.x);
		if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY) y.copy(P.y);
		z.copy(P.z);
	}
/* this=-this */
	public void neg() {
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.WEIERSTRASS)
		{
			y.neg(); y.norm();
		}
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS)
		{
			x.neg(); x.norm();
		}
		return;
	}
/* set this=O */
	public void inf() {
//		INF=true;
		x.zero();
		if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY) y.one();
		if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.EDWARDS) z.zero();
		else z.one();
	}

/* Calculate RHS of curve equation */
	public static FP RHS(FP x) {
		FP r=new FP(x);
		r.sqr();

		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.WEIERSTRASS)
		{ // x^3+Ax+B
			FP b=new FP(new BIG(ROM.CURVE_B));
			r.mul(x);
			if (CONFIG_CURVE.CURVE_A==-3)
			{
				FP cx=new FP(x);
				cx.imul(3);
				cx.neg(); cx.norm();
				r.add(cx);
			}
			r.add(b);
		}
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS)
		{ // (Ax^2-1)/(Bx^2-1)
			FP b=new FP(new BIG(ROM.CURVE_B));

			FP one=new FP(1);
			b.mul(r);
			b.sub(one);
			b.norm();
			if (CONFIG_CURVE.CURVE_A==-1) r.neg();
			r.sub(one); r.norm();
			b.inverse(null);

			r.mul(b);
		}
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
		{ // x^3+Ax^2+x
			FP x3=new FP();
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
	public ECP(BIG ix,BIG iy) {
		x=new FP(ix);
		y=new FP(iy);
		z=new FP(1);
		x.norm();
		FP rhs=RHS(x);

		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
		{
			if (rhs.qr(null)!=1) inf();
		}
		else
		{
			FP y2=new FP(y);
			y2.sqr();
			if (!y2.equals(rhs)) inf();
		}
	}
/* set (x,y) from BIG and a bit */
	public ECP(BIG ix,int s) {
		x=new FP(ix);
		x.norm();
		FP rhs=RHS(x);
        FP hint=new FP();
		y=new FP();
		z=new FP(1);
		if (rhs.qr(hint)==1)
		{
			FP ny=rhs.sqrt(hint);
			if (ny.sign()!=s) {
                ny.neg();
                ny.norm();
            }
			y.copy(ny);
		}
		else inf();
	}

/* set from x - calculate y from curve equation */
	public ECP(BIG ix) {
		x=new FP(ix);
		x.norm();
		FP rhs=RHS(x);
        FP hint=new FP();
		y=new FP();
		z=new FP(1);
		if (rhs.qr(hint)==1)
		{
			if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY) y.copy(rhs.sqrt(hint));
		}
		else inf(); //INF=true;
	}

/* set to affine - from (x,y,z) to (x,y) */
	public void affine() {
		if (is_infinity()) return;	//
		FP one=new FP(1);
		if (z.equals(one)) return;
		z.inverse(null);
		x.mul(z); x.reduce();
		if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY)            // Edits made
		{
			y.mul(z); y.reduce();
		}
		z.copy(one);
	}
/* extract x as a BIG */
	public BIG getX()
	{
		ECP W=new ECP(this);
		W.affine();
		return W.x.redc();
	}
/* extract y as a BIG */
	public BIG getY()
	{
		ECP W=new ECP(this);
		W.affine();
		return W.y.redc();
	}

/* get sign of Y */
	public int getS()
	{
        ECP W=new ECP(this);
		W.affine();
        return W.y.sign();
	}
/* extract x as an FP */
	public FP getx()
	{
		return x;
	}
/* extract y as an FP */
	public FP gety()
	{
		return y;
	}
/* extract z as an FP */
	public FP getz()
	{
		return z;
	}
/* convert to byte array */
	public void toBytes(byte[] b,boolean compress)
	{
		byte[] t=new byte[CONFIG_BIG.MODBYTES];
        boolean alt=false;
		ECP W=new ECP(this);
		W.affine();
		W.x.redc().toBytes(t);

		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
		{
		    for (int i=0;i<CONFIG_BIG.MODBYTES;i++) b[i]=t[i];
			return;
		}

        if ((CONFIG_FIELD.MODBITS-1)%8<=4 && CONFIG_CURVE.ALLOW_ALT_COMPRESS) alt=true;

        if (alt)
        {
            for (int i=0;i<CONFIG_BIG.MODBYTES;i++) b[i]=t[i];
            if (compress)
            {
                b[0]|=0x80;
                if (W.y.islarger()==1) b[0]|=0x20;
            } else {
                W.y.redc().toBytes(t);
                for (int i=0;i<CONFIG_BIG.MODBYTES;i++) b[i+CONFIG_BIG.MODBYTES]=t[i];
            }
        } else {
		    for (int i=0;i<CONFIG_BIG.MODBYTES;i++) b[i+1]=t[i];
		    if (compress)
		    {
			    b[0]=0x02;
			    if (W.y.sign()==1) b[0]=0x03;
			    return;
		    }
		    b[0]=0x04;
		    W.y.redc().toBytes(t);
		    for (int i=0;i<CONFIG_BIG.MODBYTES;i++) b[i+CONFIG_BIG.MODBYTES+1]=t[i];
        }
	}
/* convert from byte array to point */
	public static ECP fromBytes(byte[] b)
	{
		byte[] t=new byte[CONFIG_BIG.MODBYTES];
		BIG p=new BIG(ROM.Modulus);
        boolean alt=false;

		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
		{
		    for (int i=0;i<CONFIG_BIG.MODBYTES;i++) t[i]=b[i];
		    BIG px=BIG.fromBytes(t);
		    if (BIG.comp(px,p)>=0) return new ECP();

			return new ECP(px);
		}

        if ((CONFIG_FIELD.MODBITS-1)%8<=4 && CONFIG_CURVE.ALLOW_ALT_COMPRESS) alt=true;

        if (alt)
        {
            for (int i=0;i<CONFIG_BIG.MODBYTES;i++) t[i]=b[i];
            t[0]&=0x1f;
            BIG px=BIG.fromBytes(t);
            if ((b[0]&0x80)==0)
            {
                for (int i=0;i<CONFIG_BIG.MODBYTES;i++) t[i]=b[i+CONFIG_BIG.MODBYTES];
                BIG py=BIG.fromBytes(t);
                return new ECP(px,py);
            } else {
                int sgn=(b[0]&0x20)>>5;
                ECP P=new ECP(px,0);
                int cmp=P.y.islarger();
                if ((sgn==1 && cmp!=1) || (sgn==0 && cmp==1)) P.neg();
                return P;
            }
        } else {
		    for (int i=0;i<CONFIG_BIG.MODBYTES;i++) t[i]=b[i+1];
		    BIG px=BIG.fromBytes(t);
		    if (BIG.comp(px,p)>=0) return new ECP();
		    if (b[0]==0x04)
		    {
			    for (int i=0;i<CONFIG_BIG.MODBYTES;i++) t[i]=b[i+CONFIG_BIG.MODBYTES+1];
			    BIG py=BIG.fromBytes(t);
			    if (BIG.comp(py,p)>=0) return new ECP();
			    return new ECP(px,py);
		    }

		    if (b[0]==0x02 || b[0]==0x03)
		    {
			    return new ECP(px,(int)(b[0]&1));
		    }
        }
		return new ECP();
	}
/* convert to hex string */
	public String toString() {
		ECP W=new ECP(this);
		W.affine();
		if (W.is_infinity()) return "infinity";
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY) return "("+W.x.redc().toString()+")";
		else return "("+W.x.redc().toString()+","+W.y.redc().toString()+")";
	}

/* convert to hex string */
	public String toRawString() {
		ECP W=new ECP(this);
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY) return "("+W.x.redc().toString()+","+W.z.redc().toString()+")";
		else return "("+W.x.redc().toString()+","+W.y.redc().toString()+","+W.z.redc().toString()+")";
	}

/* this*=2 */
	public void dbl() {

		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.WEIERSTRASS)
		{
			if (CONFIG_CURVE.CURVE_A==0)
			{
				FP t0=new FP(y);                      /*** Change ***/    // Edits made
				t0.sqr();
				FP t1=new FP(y);
				t1.mul(z);
				FP t2=new FP(z);
				t2.sqr();

				z.copy(t0);
				z.add(t0); z.norm();
				z.add(z); z.add(z); z.norm();
				t2.imul(3*ROM.CURVE_B_I);

				FP x3=new FP(t2);
				x3.mul(z);

				FP y3=new FP(t0);
				y3.add(t2); y3.norm();
				z.mul(t1);
				t1.copy(t2); t1.add(t2); t2.add(t1);
				t0.sub(t2); t0.norm(); y3.mul(t0); y3.add(x3);
				t1.copy(x); t1.mul(y);
				x.copy(t0); x.norm(); x.mul(t1); x.add(x);
				x.norm();
				y.copy(y3); y.norm();
			}
			else
			{
				FP t0=new FP(x);
				FP t1=new FP(y);
				FP t2=new FP(z);
				FP t3=new FP(x);
				FP z3=new FP(z);
				FP y3=new FP();
				FP x3=new FP();
				FP b=new FP();

				if (ROM.CURVE_B_I==0)
					b.copy(new FP(new BIG(ROM.CURVE_B)));

				t0.sqr();  //1    x^2
				t1.sqr();  //2    y^2
				t2.sqr();  //3

				t3.mul(y); //4
				t3.add(t3); t3.norm();//5
				z3.mul(x);   //6
				z3.add(z3);  z3.norm();//7
				y3.copy(t2);

				if (ROM.CURVE_B_I==0)
					y3.mul(b); //8
				else
					y3.imul(ROM.CURVE_B_I);

				y3.sub(z3); //y3.norm(); //9  ***
				x3.copy(y3); x3.add(y3); x3.norm();//10

				y3.add(x3); //y3.norm();//11
				x3.copy(t1); x3.sub(y3); x3.norm();//12
				y3.add(t1); y3.norm();//13
				y3.mul(x3); //14
				x3.mul(t3); //15
				t3.copy(t2); t3.add(t2); //t3.norm(); //16
				t2.add(t3); //t2.norm(); //17

				if (ROM.CURVE_B_I==0)
					z3.mul(b); //18
				else
					z3.imul(ROM.CURVE_B_I);

				z3.sub(t2); //z3.norm();//19
				z3.sub(t0); z3.norm();//20  ***
				t3.copy(z3); t3.add(z3); //t3.norm();//21

				z3.add(t3); z3.norm(); //22
				t3.copy(t0); t3.add(t0); //t3.norm(); //23
				t0.add(t3); //t0.norm();//24
				t0.sub(t2); t0.norm();//25

				t0.mul(z3);//26
				y3.add(t0); //y3.norm();//27
				t0.copy(y); t0.mul(z);//28
				t0.add(t0); t0.norm(); //29
				z3.mul(t0);//30
				x3.sub(z3); //x3.norm();//31
				t0.add(t0); t0.norm();//32
				t1.add(t1); t1.norm();//33
				z3.copy(t0); z3.mul(t1);//34

				x.copy(x3); x.norm();
				y.copy(y3); y.norm();
				z.copy(z3); z.norm();
			}
		}
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS)
		{
			FP C=new FP(x);
			FP D=new FP(y);
			FP H=new FP(z);
			FP J=new FP();

			x.mul(y); x.add(x); x.norm();
			C.sqr();
			D.sqr();

			if (CONFIG_CURVE.CURVE_A==-1) C.neg();

			y.copy(C); y.add(D); y.norm();
			H.sqr(); H.add(H);

			z.copy(y);
			J.copy(y);

			J.sub(H); J.norm();
			x.mul(J);

			C.sub(D); C.norm();
			y.mul(C);
			z.mul(J);
		}
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
		{
			FP A=new FP(x);
			FP B=new FP(x);
			FP AA=new FP();
			FP BB=new FP();
			FP C=new FP();

			A.add(z); A.norm();
			AA.copy(A); AA.sqr();
			B.sub(z); B.norm();
			BB.copy(B); BB.sqr();
			C.copy(AA); C.sub(BB); C.norm();
			x.copy(AA); x.mul(BB);

			A.copy(C); A.imul((CONFIG_CURVE.CURVE_A+2)/4);

			BB.add(A); BB.norm();
			z.copy(BB); z.mul(C);
		}
		return;
	}

/* this+=Q */
	public void add(ECP Q) {

		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.WEIERSTRASS)
		{
			if (CONFIG_CURVE.CURVE_A==0)
			{
				int b=3*ROM.CURVE_B_I;
				FP t0=new FP(x);
				t0.mul(Q.x);
				FP t1=new FP(y);
				t1.mul(Q.y);
				FP t2=new FP(z);
				t2.mul(Q.z);
				FP t3=new FP(x);
				t3.add(y); t3.norm();
				FP t4=new FP(Q.x);
				t4.add(Q.y); t4.norm();
				t3.mul(t4);
				t4.copy(t0); t4.add(t1);

				t3.sub(t4); t3.norm();
				t4.copy(y);
				t4.add(z); t4.norm();
				FP x3=new FP(Q.y);
				x3.add(Q.z); x3.norm();

				t4.mul(x3);
				x3.copy(t1);
				x3.add(t2);

				t4.sub(x3); t4.norm();
				x3.copy(x); x3.add(z); x3.norm();
				FP y3=new FP(Q.x);
				y3.add(Q.z); y3.norm();
				x3.mul(y3);
				y3.copy(t0);
				y3.add(t2);
				y3.rsub(x3); y3.norm();
				x3.copy(t0); x3.add(t0);
				t0.add(x3); t0.norm();
				t2.imul(b);

				FP z3=new FP(t1); z3.add(t2); z3.norm();
				t1.sub(t2); t1.norm();
				y3.imul(b);

				x3.copy(y3); x3.mul(t4); t2.copy(t3); t2.mul(t1); x3.rsub(t2);
				y3.mul(t0); t1.mul(z3); y3.add(t1);
				t0.mul(t3); z3.mul(t4); z3.add(t0);

				x.copy(x3); x.norm();
				y.copy(y3); y.norm();
				z.copy(z3); z.norm();
			}
			else
			{
				FP t0=new FP(x);
				FP t1=new FP(y);
				FP t2=new FP(z);
				FP t3=new FP(x);
				FP t4=new FP(Q.x);
				FP z3=new FP();
				FP y3=new FP(Q.x);
				FP x3=new FP(Q.y);
				FP b=new FP();

				if (ROM.CURVE_B_I==0)
					b.copy(new FP(new BIG(ROM.CURVE_B)));

				t0.mul(Q.x); //1
				t1.mul(Q.y); //2
				t2.mul(Q.z); //3

				t3.add(y); t3.norm(); //4
				t4.add(Q.y); t4.norm();//5
				t3.mul(t4);//6
				t4.copy(t0); t4.add(t1); //t4.norm(); //7
				t3.sub(t4); t3.norm(); //8
				t4.copy(y); t4.add(z); t4.norm();//9
				x3.add(Q.z); x3.norm();//10
				t4.mul(x3); //11
				x3.copy(t1); x3.add(t2); //x3.norm();//12

				t4.sub(x3); t4.norm();//13
				x3.copy(x); x3.add(z); x3.norm(); //14
				y3.add(Q.z); y3.norm();//15

				x3.mul(y3); //16
				y3.copy(t0); y3.add(t2); //y3.norm();//17

				y3.rsub(x3); y3.norm(); //18
				z3.copy(t2);


				if (ROM.CURVE_B_I==0)
					z3.mul(b); //18
				else
					z3.imul(ROM.CURVE_B_I);

				x3.copy(y3); x3.sub(z3); x3.norm(); //20
				z3.copy(x3); z3.add(x3); //z3.norm(); //21

				x3.add(z3); //x3.norm(); //22
				z3.copy(t1); z3.sub(x3); z3.norm(); //23
				x3.add(t1); x3.norm(); //24

				if (ROM.CURVE_B_I==0)
					y3.mul(b); //18
				else
					y3.imul(ROM.CURVE_B_I);

				t1.copy(t2); t1.add(t2); //t1.norm();//26
				t2.add(t1); //t2.norm();//27

				y3.sub(t2); //y3.norm(); //28

				y3.sub(t0); y3.norm(); //29
				t1.copy(y3); t1.add(y3); //t1.norm();//30
				y3.add(t1); y3.norm(); //31

				t1.copy(t0); t1.add(t0); //t1.norm(); //32
				t0.add(t1); //t0.norm();//33
				t0.sub(t2); t0.norm();//34
				t1.copy(t4); t1.mul(y3);//35
				t2.copy(t0); t2.mul(y3);//36
				y3.copy(x3); y3.mul(z3);//37
				y3.add(t2); //y3.norm();//38
				x3.mul(t3);//39
				x3.sub(t1);//40
				z3.mul(t4);//41
				t1.copy(t3); t1.mul(t0);//42
				z3.add(t1);
				x.copy(x3); x.norm();
				y.copy(y3); y.norm();
				z.copy(z3); z.norm();
			}
		}
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS)
		{
			FP A=new FP(z);
			FP B=new FP();
			FP C=new FP(x);
			FP D=new FP(y);
			FP E=new FP();
			FP F=new FP();
			FP G=new FP();

			A.mul(Q.z);
			B.copy(A); B.sqr();
			C.mul(Q.x);
			D.mul(Q.y);

			E.copy(C); E.mul(D);

			if (ROM.CURVE_B_I==0)
			{
				FP b=new FP(new BIG(ROM.CURVE_B));
				E.mul(b);
			}
			else
				E.imul(ROM.CURVE_B_I);

			F.copy(B); F.sub(E);
			G.copy(B); G.add(E);

			if (CONFIG_CURVE.CURVE_A==1)
			{
				E.copy(D); E.sub(C);
			}
			C.add(D);

			B.copy(x); B.add(y);
			D.copy(Q.x); D.add(Q.y); B.norm(); D.norm();
			B.mul(D);
			B.sub(C); B.norm(); F.norm();
			B.mul(F);
			x.copy(A); x.mul(B); G.norm();
			if (CONFIG_CURVE.CURVE_A==1)
			{
				E.norm(); C.copy(E); C.mul(G);
			}
			if (CONFIG_CURVE.CURVE_A==-1)
			{
				C.norm(); C.mul(G);
			}
			y.copy(A); y.mul(C);

			z.copy(F);
			z.mul(G);
		}
		return;
	}

/* Differential Add for CONFIG_CURVE.MONTGOMERY curves. this+=Q where W is this-Q and is affine. */
	public void dadd(ECP Q,ECP W) {
		FP A=new FP(x);
		FP B=new FP(x);
		FP C=new FP(Q.x);
		FP D=new FP(Q.x);
		FP DA=new FP();
		FP CB=new FP();

		A.add(z);
		B.sub(z);

		C.add(Q.z);
		D.sub(Q.z);
		A.norm();

		D.norm();
		DA.copy(D); DA.mul(A);

		C.norm();
		B.norm();
		CB.copy(C); CB.mul(B);

		A.copy(DA); A.add(CB);
		A.norm(); A.sqr();
		B.copy(DA); B.sub(CB);
		B.norm(); B.sqr();

		x.copy(A);
		z.copy(W.x); z.mul(B);
	}
/* this-=Q */
	public void sub(ECP Q) {
		ECP NQ=new ECP(Q);
		NQ.neg();
		add(NQ);
	}

/* constant time multiply by small integer of length bts - use ladder */
	public ECP pinmul(int e,int bts) {
		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
			return this.mul(new BIG(e));
		else
		{
			int nb,i,b;
			ECP P=new ECP();
			ECP R0=new ECP();
			ECP R1=new ECP(); R1.copy(this);

			for (i=bts-1;i>=0;i--)
			{
				b=(e>>i)&1;
				P.copy(R1);
				P.add(R0);
				R0.cswap(R1,b);
				R1.copy(P);
				R0.dbl();
				R0.cswap(R1,b);
			}
			P.copy(R0);
			return P;
		}
	}

// Point multiplication, multiplies a point P by a scalar e
// This code has no inherent awareness of the order of the curve, or the order of the point.
// The order of the curve will be h.r, where h is a cofactor, and r is a large prime
// Typically P will be of order r (but not always), and typically e will be less than r (but not always)
// A problem can arise if a secret e is a few bits less than r, as the leading zeros in e will leak via a timing attack
// The secret e may however be greater than r (see RFC7748 which combines elimination of a small cofactor h with the point multiplication, using an e>r)
// Our solution is to use as a multiplier an e, whose length in bits is that of the logical OR of e and r, hence allowing e>r while forcing inclusion of leading zeros if e<r. 
// The point multiplication methods used will process leading zeros correctly.

// So this function leaks information about the length of e...
    public ECP mul(BIG e)
    {
        return clmul(e,e);
    }

// .. but this one does not (typically set maxe=r)
// Set P=e*P 
/* return e.this */
	public ECP clmul(BIG e,BIG maxe) {
		if (e.iszilch() || is_infinity()) return new ECP();
		ECP P=new ECP();
        BIG cm=new BIG(e); cm.or(maxe);
        int max=cm.nbits();

		if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
		{
/* use Ladder */
			int nb,i,b;
			ECP D=new ECP();
			ECP R0=new ECP(); R0.copy(this);
			ECP R1=new ECP(); R1.copy(this);
			R1.dbl();

			D.copy(this); D.affine();
			nb=max;
			for (i=nb-2;i>=0;i--)
			{
				b=e.bit(i);
				P.copy(R1);

				P.dadd(R0,D);
				R0.cswap(R1,b);
				R1.copy(P);
				R0.dbl();
				R0.cswap(R1,b);

			}

			P.copy(R0);
		}
		else
		{
// fixed size windows
			int i,b,nb,m,s,ns;
			BIG mt=new BIG();
			BIG t=new BIG();
			ECP Q=new ECP();
			ECP C=new ECP();
			ECP[] W=new ECP[8];
			byte[] w=new byte[1+(BIG.NLEN*CONFIG_BIG.BASEBITS+3)/4];

// precompute table
			Q.copy(this);

			Q.dbl();
			W[0]=new ECP();
			W[0].copy(this);

			for (i=1;i<8;i++)
			{
				W[i]=new ECP();
				W[i].copy(W[i-1]);
				W[i].add(Q);
			}

// make exponent odd - add 2P if even, P if odd
			t.copy(e);
			s=t.parity();
			t.inc(1); t.norm(); ns=t.parity(); mt.copy(t); mt.inc(1); mt.norm();
			t.cmove(mt,s);
			Q.cmove(this,ns);
			C.copy(Q);

			nb=1+(max+3)/4;

// convert exponent to signed 4-bit window
			for (i=0;i<nb;i++)
			{
				w[i]=(byte)(t.lastbits(5)-16);
				t.dec(w[i]); t.norm();
				t.fshr(4);
			}
			w[nb]=(byte)t.lastbits(5);

			//P.copy(W[(w[nb]-1)/2]);
            P.select(W,w[nb]);
			for (i=nb-1;i>=0;i--)
			{
				Q.select(W,w[i]);
				P.dbl();
				P.dbl();
				P.dbl();
				P.dbl();
				P.add(Q);
			}
			P.sub(C); /* apply correction */
		}
        //P.affine();
		return P;
	}


// Generic multi-multiplication, fixed 4-bit window, P=Sigma e_i*X_i

    public ECP muln(int n,ECP X[],BIG e[]) {
        ECP P=new ECP();
        ECP R=new ECP();
        ECP S=new ECP();
		ECP[] B=new ECP[16];
        BIG t=new BIG();

        for (int i=0;i<16;i++) {
            B[i]=new ECP();
        }
        BIG mt=new BIG(e[0]); mt.norm();
        for (int i=1;i<n;i++)
        { // find biggest
            t.copy(e[i]); t.norm();
            int k=BIG.comp(t,mt);
            mt.cmove(t,(k+1)/2);
        }
        int nb=(mt.nbits()+3)/4;
        for (int i=nb-1;i>=0;i--)
        {
            for (int j=0;j<16;j++) {
                B[j].inf();
            }
            for (int j=0;j<n;j++)
            { 
                mt.copy(e[j]); mt.norm();
                mt.shr(4*i);
                int k=mt.lastbits(4);
                B[k].add(X[j]);
            }
            R.inf(); S.inf();
            for (int j=15;j>=1;j--)
            {
                R.add(B[j]);
                S.add(R);
            }
            for (int j=0;j<4;j++) {
                P.dbl();
            }
            P.add(S);
        }
        return P;
    }

/* Return e.this+f.Q */

	public ECP mul2(BIG e,ECP Q,BIG f) {
		BIG te=new BIG();
		BIG tf=new BIG();
		BIG mt=new BIG();
		ECP S=new ECP();
		ECP T=new ECP();
		ECP C=new ECP();
		ECP[] W=new ECP[8];
		byte[] w=new byte[1+(BIG.NLEN*CONFIG_BIG.BASEBITS+1)/2];
		int i,s,ns,nb;
		byte a,b;

		te.copy(e);
		tf.copy(f);

// precompute table
		W[1]=new ECP(); W[1].copy(this); W[1].sub(Q);
		W[2]=new ECP(); W[2].copy(this); W[2].add(Q);
		S.copy(Q); S.dbl();
		W[0]=new ECP(); W[0].copy(W[1]); W[0].sub(S);
		W[3]=new ECP(); W[3].copy(W[2]); W[3].add(S);
		T.copy(this); T.dbl();
		W[5]=new ECP(); W[5].copy(W[1]); W[5].add(T);
		W[6]=new ECP(); W[6].copy(W[2]); W[6].add(T);
		W[4]=new ECP(); W[4].copy(W[5]); W[4].sub(S);
		W[7]=new ECP(); W[7].copy(W[6]); W[7].add(S);

// if multiplier is odd, add 2, else add 1 to multiplier, and add 2P or P to correction

		s=te.parity();
		te.inc(1); te.norm(); ns=te.parity(); mt.copy(te); mt.inc(1); mt.norm();
		te.cmove(mt,s);
		T.cmove(this,ns);
		C.copy(T);

		s=tf.parity();
		tf.inc(1); tf.norm(); ns=tf.parity(); mt.copy(tf); mt.inc(1); mt.norm();
		tf.cmove(mt,s);
		S.cmove(Q,ns);
		C.add(S);

		mt.copy(te); mt.add(tf); mt.norm();
		nb=1+(mt.nbits()+1)/2;

// convert exponent to signed 2-bit window
		for (i=0;i<nb;i++)
		{
			a=(byte)(te.lastbits(3)-4);
			te.dec(a); te.norm();
			te.fshr(2);
			b=(byte)(tf.lastbits(3)-4);
			tf.dec(b); tf.norm();
			tf.fshr(2);
			w[i]=(byte)(4*a+b);
		}
		w[nb]=(byte)(4*te.lastbits(3)+tf.lastbits(3));
		//S.copy(W[(w[nb]-1)/2]);
        S.select(W,w[nb]);
		for (i=nb-1;i>=0;i--)
		{
			T.select(W,w[i]);
			S.dbl();
			S.dbl();
			S.add(T);
		}
		S.sub(C); /* apply correction */
//        S.affine();
		return S;
	}

// multiply a point by the curves cofactor
	public void cfp()
	{
		int cf=ROM.CURVE_Cof_I;
		if (cf==1) return;
		if (cf==4)
		{
			dbl(); dbl();
			//affine();
			return;
		}
		if (cf==8)
		{
			dbl(); dbl(); dbl();
			//affine();
			return;
		}
		BIG c=new BIG(ROM.CURVE_Cof);
		copy(mul(c));
	}

/* Hunt and Peck a BIG to a curve point
    public static ECP hap2point(BIG h)
    {
        ECP P;
        BIG x=new BIG(h);
		while (true)
		{
			if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY)
				P=new ECP(x,0);
			else
				P=new ECP(x);
			x.inc(1); x.norm();
			if (!P.is_infinity()) break;
		}
        return P;
    } */

/* Constant time Map to Point */
    public static ECP map2point(FP h)
    {
        ECP P;
        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY)
        { // Elligator 2
            FP X1=new FP();
            FP X2=new FP();
            FP t=new FP(h);
            FP w=new FP();
            FP one=new FP(1);
            FP N=new FP();
            FP D=new FP();
            FP hint=new FP();
            FP A=new FP(CONFIG_CURVE.CURVE_A);
            t.sqr();

            if (CONFIG_FIELD.PM1D2 == 2) {
                t.add(t);
            }
            if (CONFIG_FIELD.PM1D2 == 1) {
                t.neg();
            }
            if (CONFIG_FIELD.PM1D2 > 2) {
                t.imul(CONFIG_FIELD.QNRI);
            }
            t.norm();
            D.copy(t); D.add(one); D.norm();

            X1.copy(A);
            X1.neg(); X1.norm();
            X2.copy(X1);
            X2.mul(t);

            w.copy(X1); w.sqr(); N.copy(w); N.mul(X1);
            w.mul(A); w.mul(D); N.add(w); 
            t.copy(D); t.sqr();
            t.mul(X1);
            N.add(t); N.norm();

            t.copy(N); t.mul(D);
            int qres=t.qr(hint);
            w.copy(t); w.inverse(hint);
            D.copy(w); D.mul(N);
            X1.mul(D);
            X2.mul(D);
            X1.cmove(X2,1-qres);

            BIG a=X1.redc();
            P=new ECP(a);
            return P;
        }
        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.EDWARDS)
        { // Elligator 2 - map to Montgomery, place point, map back
            FP X1=new FP();
            FP X2=new FP();
            FP t=new FP(h);
            FP w=new FP();
            FP one=new FP(1);
            FP A=new FP();
            FP w1=new FP();
            FP w2=new FP();
            FP B=new FP(new BIG(ROM.CURVE_B));
            FP Y=new FP();
            FP K=new FP();
            FP D=new FP();
            FP hint=new FP();
        //    FP Y3=new FP();

            int qres,qnr;
            int rfc=0;

            if (CONFIG_FIELD.MODTYPE != CONFIG_FIELD.GENERALISED_MERSENNE )
            {
                A.copy(B);
                if (CONFIG_CURVE.CURVE_A==1) {
                    A.add(one);
                    B.sub(one);
                } else {
                    A.sub(one);
                    B.add(one);
                }
                A.norm(); B.norm();

                A.div2();
                B.div2();
                B.div2();

                K.copy(B);
                K.neg(); K.norm();
 
                //K.inverse(null);
                K.invsqrt(K,w1);

                rfc=CONFIG_FIELD.RIADZ;
                if (rfc==1)
                { // RFC7748
                    A.mul(K);
                    K.mul(w1);
                    //K=K.sqrt(null);
                } else {
                    B.sqr();
                }

            } else {
                A.copy(new FP(156326));
                rfc=1;
            }

            t.sqr();
            if (CONFIG_FIELD.PM1D2 == 2) {
                qnr=2;
                t.add(t);
            }
            if (CONFIG_FIELD.PM1D2 == 1) {
                t.neg();
                qnr=-1;
            }
            if (CONFIG_FIELD.PM1D2 > 2) {
                t.imul(CONFIG_FIELD.QNRI);
                qnr=CONFIG_FIELD.QNRI;
            }
            t.norm();
            D.copy(t); D.add(one); D.norm();
            X1.copy(A);
            X1.neg(); X1.norm();
            X2.copy(X1); X2.mul(t);

// Figure out RHS of Montgomery curve in rational form gx1/d^3

            w.copy(X1); w.sqr(); w1.copy(w); w1.mul(X1);
            w.mul(A); w.mul(D); w1.add(w);
            w2.copy(D); w2.sqr();

            if (rfc==0)
            {
                w.copy(X1); w.mul(B);
                w2.mul(w);
                w1.add(w2);
            } else {
                w2.mul(X1);
                w1.add(w2);
            }
            w1.norm();

            B.copy(w1); B.mul(D);
            qres=B.qr(hint);
            w.copy(B); w.inverse(hint);
            D.copy(w); D.mul(w1);
            X1.mul(D);
            X2.mul(D);
            D.sqr();

            w1.copy(B); w1.imul(qnr);
            w.copy(new FP(new BIG(ROM.CURVE_HTPC)));
            w.mul(hint);
            w2.copy(D); w2.mul(h);

            X1.cmove(X2,1-qres);
            B.cmove(w1,1-qres);
            hint.cmove(w,1-qres);
            D.cmove(w2,1-qres);

            Y.copy(B.sqrt(hint));
            Y.mul(D);

/*
            Y.copy(B.sqrt(hint));
            Y.mul(D);

            B.imul(qnr);
            w.copy(new FP(new BIG(ROM.CURVE_HTPC)));
            hint.mul(w);

            Y3.copy(B.sqrt(hint));
            D.mul(h);
            Y3.mul(D);

            X1.cmove(X2,1-qres);
            Y.cmove(Y3,1-qres);
*/
            w.copy(Y); w.neg(); w.norm();
            Y.cmove(w,qres^Y.sign());

            if (rfc==0)
            {
                X1.mul(K);
                Y.mul(K);
            }

            if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.GENERALISED_MERSENNE )
            { // ED448 isogeny
                t.copy(X1); t.sqr();
                w.copy(t); w.add(one); w.norm();
                t.sub(one); t.norm();
                w1.copy(t); w1.mul(Y);
                w1.add(w1); X2.copy(w1); X2.add(w1); X2.norm();
                t.sqr();
                Y.sqr(); Y.add(Y); Y.add(Y); Y.norm();
                B.copy(t); B.add(Y); B.norm();

                w2.copy(Y); w2.sub(t); w2.norm();
                w2.mul(X1);
                t.mul(X1);
                Y.div2();
                w1.copy(Y); w1.mul(w);
                w1.rsub(t); w1.norm();
 
                t.copy(X2); t.mul(w1);
                P=new ECP();
                P.x.copy(t);
                t.copy(w2); t.mul(B);
                P.y.copy(t);
                t.copy(w1); t.mul(B);
                P.z.copy(t);

                return P;

            } else {
                w1.copy(X1); w1.add(one); w1.norm();
                w2.copy(X1); w2.sub(one); w2.norm();
                t.copy(w1); t.mul(Y);
                X1.mul(w1);
   
                if (rfc==1)
                    X1.mul(K);

                Y.mul(w2);
                P=new ECP();
                P.x.copy(X1);
                P.y.copy(Y);
                P.z.copy(t);

                return P;
            }
        }

        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.WEIERSTRASS)
        {
            FP A=new FP();
            FP B=new FP();
            FP X1=new FP();
            FP X2=new FP();
            FP X3=new FP();
            FP one=new FP(1);
            FP Y=new FP();
            FP D=new FP();
            FP t=new FP(h);
            FP w=new FP();
            FP D2=new FP();
            FP hint=new FP();
            FP GX1=new FP();
          //  FP Y3=new FP();
            int sgn=t.sign();

            if (CONFIG_CURVE.CURVE_A!=0 || CONFIG_CURVE.HTC_ISO!=0)
            { 
                if (CONFIG_CURVE.HTC_ISO!=0)
                {
/* CAHCZS
                    A.copy(new FP(new BIG(ROM.CURVE_Ad)));
                    B.copy(new FP(new BIG(ROM.CURVE_Bd)));
CAHCZF */
                } else {
                    A.copy(new FP(CONFIG_CURVE.CURVE_A));
                    B.copy(new FP(new BIG(ROM.CURVE_B)));
                }
                // SSWU Method
                t.sqr();
                t.imul(CONFIG_FIELD.RIADZ);
                w.copy(t); w.add(one); w.norm();

                w.mul(t); D.copy(A);
                D.mul(w);
               
                w.add(one); w.norm();
                w.mul(B);
                w.neg(); w.norm();

                X2.copy(w); 
                X3.copy(t); X3.mul(X2);
// x^3+Ad^2x+Bd^3
                GX1.copy(X2); GX1.sqr(); D2.copy(D);
                D2.sqr(); w.copy(A); w.mul(D2); GX1.add(w); GX1.norm(); GX1.mul(X2); D2.mul(D);w.copy(B); w.mul(D2); GX1.add(w); GX1.norm();

                w.copy(GX1); w.mul(D);
                int qr=w.qr(hint);
                D.copy(w); D.inverse(hint);
                D.mul(GX1);
                X2.mul(D);
                X3.mul(D);
                t.mul(h);
                D2.copy(D); D2.sqr();

                D.copy(D2); D.mul(t);
                t.copy(w); t.imul(CONFIG_FIELD.RIADZ);
                X1.copy(new FP(new BIG(ROM.CURVE_HTPC)));
                X1.mul(hint);

                X2.cmove(X3,1-qr);
                D2.cmove(D,1-qr);
                w.cmove(t,1-qr);
                hint.cmove(X1,1-qr);

                Y.copy(w.sqrt(hint));
                Y.mul(D2);

/*
                Y.copy(w.sqrt(hint));
                Y.mul(D2);

                D2.mul(t);
                w.imul(CONFIG_FIELD.RIADZ);

                X1.copy(new FP(new BIG(ROM.CURVE_HTPC)));
                hint.mul(X1);
                
                Y3.copy(w.sqrt(hint));
                Y3.mul(D2);

                X2.cmove(X3,1-qr);
                Y.cmove(Y3,1-qr);
*/
                int ne=Y.sign()^sgn;
                w.copy(Y); w.neg(); w.norm();
                Y.cmove(w,ne);

                if (CONFIG_CURVE.HTC_ISO != 0)
                {
/* CAHCZS

                    int k=0;
                    int isox=CONFIG_CURVE.HTC_ISO;
                    int isoy=3*(isox-1)/2;
                // xnum
                    FP xnum=new FP(new BIG(ROM.PC[k++]));
                    for (int i=0;i<isox;i++) {
                        xnum.mul(X2);
                        w.copy(new FP(new BIG(ROM.PC[k++])));
                        xnum.add(w); xnum.norm();
                    }
                // xden
                    FP xden=new FP(X2);
                    w.copy(new FP(new BIG(ROM.PC[k++])));
                    xden.add(w); xden.norm();
                    for (int i=0;i<isox-2;i++) {
                        xden.mul(X2);
                        w.copy(new FP(new BIG(ROM.PC[k++])));
                        xden.add(w); xden.norm();
                    }
                // ynum
                    FP ynum=new FP(new BIG(ROM.PC[k++]));
                    for (int i=0;i<isoy;i++) {
                        ynum.mul(X2);
                        w.copy(new FP(new BIG(ROM.PC[k++])));
                        ynum.add(w); ynum.norm();
                    }
                // yden
                    FP yden=new FP(X2);
                    w.copy(new FP(new BIG(ROM.PC[k++])));
                    yden.add(w); yden.norm();
                    for (int i=0;i<isoy-1;i++) {
                        yden.mul(X2);
                        w.copy(new FP(new BIG(ROM.PC[k++])));
                        yden.add(w); yden.norm();
                    }  
                    ynum.mul(Y);
                    w.copy(xnum); w.mul(yden);
                    P=new ECP();
                    P.x.copy(w);
                    w.copy(ynum); w.mul(xden);
                    P.y.copy(w);
                    w.copy(xden); w.mul(yden);
                    P.z.copy(w);
                    return P;
CAHCZF */
                } else {
                    BIG x=X2.redc();
                    BIG y=Y.redc();
                    P=new ECP(x,y);
                    return P;
                }
            } else {
// Shallue and van de Woestijne
// SQRTm3 not available, so preprocess this out
/* CAISZS
                int Z=CONFIG_FIELD.RIADZ;
                X1.copy(new FP(Z));
                X3.copy(X1);
                A.copy(RHS(X1));
                B.copy(new FP(new BIG(ROM.SQRTm3)));
                B.imul(Z);

                t.sqr();
                Y.copy(A); Y.mul(t);
                t.copy(one); t.add(Y); t.norm();
                Y.rsub(one); Y.norm();
                D.copy(t); D.mul(Y);
                D.mul(B);

                w.copy(A);
                FP.tpo(D,w);                     // ***

                w.mul(B);
                if (w.sign()==1)
                {
                    w.neg();
                    w.norm();
                }

                w.mul(B);
                w.mul(h); w.mul(Y); w.mul(D);

                X1.neg(); X1.norm(); X1.div2();
                X2.copy(X1);
                X1.sub(w); X1.norm();
                X2.add(w); X2.norm();
                A.add(A); A.add(A); A.norm();
                t.sqr(); t.mul(D); t.sqr();
                A.mul(t);
                X3.add(A); X3.norm();

                FP rhs=RHS(X2);
                X3.cmove(X2,rhs.qr(null));    // ***
                rhs.copy(RHS(X1));
                X3.cmove(X1,rhs.qr(null));    // ***
                rhs.copy(RHS(X3));
                Y.copy(rhs.sqrt(null));       // ***

                int ne=Y.sign()^sgn;
                w.copy(Y); w.neg(); w.norm();
                Y.cmove(w,ne);

                BIG x=X3.redc();
                BIG y=Y.redc();
                P=new ECP(x,y);
                return P;

CAISZF */
            }
        }
        return P;
    }

/* Map byte string to curve point
	public static ECP mapit(byte[] h)
	{
		BIG q=new BIG(ROM.Modulus);
        DBIG dx=DBIG.fromBytes(h);
        BIG x=dx.mod(q);
		ECP P=hap2point(x);
		P.cfp();
		return P;
	} */

	public static ECP generator()
	{
		ECP G;
		BIG gx,gy;
		gx=new BIG(ROM.CURVE_Gx);

		if (CONFIG_CURVE.CURVETYPE!=CONFIG_CURVE.MONTGOMERY)
		{
			gy=new BIG(ROM.CURVE_Gy);
			G=new ECP(gx,gy);
		}
		else
			G=new ECP(gx);
		return G;
	}

}

