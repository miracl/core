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

/* AMCL Weierstrass elliptic curve functions over FP8 */

package org.apache.milagro.amcl.XXX;

public final class ECP8 {
	private FP8 x;
	private FP8 y;
	private FP8 z;

/* Constructor - set this=O */
	public ECP8() {
		x=new FP8();
		y=new FP8(1);
		z=new FP8();
	}

    public ECP8(ECP8 e) {
        this.x = new FP8(e.x);
        this.y = new FP8(e.y);
        this.z = new FP8(e.z);
    }

/* Test this=O? */
	public boolean is_infinity() {
		return (x.iszilch() && z.iszilch());
	}
/* copy this=P */
	public void copy(ECP8 P)
	{
		x.copy(P.x);
		y.copy(P.y);
		z.copy(P.z);
	}
/* set this=O */
	public void inf() {
		x.zero();
		y.one();
		z.zero();
	}

/* Conditional move of Q to P dependant on d */
	public void cmove(ECP8 Q,int d)
	{
		x.cmove(Q.x,d);
		y.cmove(Q.y,d);
		z.cmove(Q.z,d);
	}

/* return 1 if b==c, no branching */
	public static int teq(int b,int c)
	{
		int x=b^c;
		x-=1;  // if x=0, x now -1
		return ((x>>31)&1);
	}

/* Constant time select from pre-computed table */
	public void select(ECP8 W[],int b)
	{
		ECP8 MP=new ECP8(); 
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

/* Test if P == Q */
	public boolean equals(ECP8 Q) {

		FP8 a=new FP8(x);                            // *****
		FP8 b=new FP8(Q.x);
		a.mul(Q.z); 
		b.mul(z); 
		if (!a.equals(b)) return false;

		a.copy(y); a.mul(Q.z); 
		b.copy(Q.y); b.mul(z); 
		if (!a.equals(b)) return false;

		return true;
	}

/* set this=-this */
	public void neg() {
		y.norm();
		y.neg(); y.norm();
		return;
	}
/* set to Affine - (x,y,z) to (x,y) */
	public void affine() {
		if (is_infinity()) return;
		FP8 one=new FP8(1);
		if (z.equals(one))
		{
			x.reduce();
			y.reduce();
			return;
		}
		z.inverse();

		x.mul(z); x.reduce();               // *****
		y.mul(z); y.reduce();
		z.copy(one);
	}

/* extract affine x as FP8 */
	public FP8 getX()
	{
		ECP8 W= new ECP8(this);
		W.affine();
		return W.x;
	}
/* extract affine y as FP8 */
	public FP8 getY()
	{
		ECP8 W= new ECP8(this);
		W.affine();
		return W.y;
	}
/* extract projective x */
	public FP8 getx()
	{
		return x;
	}
/* extract projective y */
	public FP8 gety()
	{
		return y;
	}
/* extract projective z */
	public FP8 getz()
	{
		return z;
	}

/* convert to byte array */
	public void toBytes(byte[] b)
	{
		byte[] t=new byte[CONFIG_BIG.MODBYTES];
		ECP8 W=new ECP8(this);
		W.affine();
		int MB=CONFIG_BIG.MODBYTES;

		W.x.geta().geta().getA().toBytes(t);
		for (int i=0;i<MB;i++) { b[i]=t[i];}
		W.x.geta().geta().getB().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+MB]=t[i];}
		W.x.geta().getb().getA().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+2*MB]=t[i];}
		W.x.geta().getb().getB().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+3*MB]=t[i];}

		W.x.getb().geta().getA().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+4*MB]=t[i];}
		W.x.getb().geta().getB().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+5*MB]=t[i];}
		W.x.getb().getb().getA().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+6*MB]=t[i];}
		W.x.getb().getb().getB().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+7*MB]=t[i];}


		W.y.geta().geta().getA().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+8*MB]=t[i];}
		W.y.geta().geta().getB().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+9*MB]=t[i];}
		W.y.geta().getb().getA().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+10*MB]=t[i];}
		W.y.geta().getb().getB().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+11*MB]=t[i];}
	
		W.y.getb().geta().getA().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+12*MB]=t[i];}
		W.y.getb().geta().getB().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+13*MB]=t[i];}
		W.y.getb().getb().getA().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+14*MB]=t[i];}
		W.y.getb().getb().getB().toBytes(t);
		for (int i=0;i<MB;i++) { b[i+15*MB]=t[i];}

	}

/* convert from byte array to point */
	public static ECP8 fromBytes(byte[] b)
	{
		byte[] t=new byte[CONFIG_BIG.MODBYTES];
		BIG ra;
		BIG rb;
		int MB=CONFIG_BIG.MODBYTES;

		for (int i=0;i<MB;i++) {t[i]=b[i];}
		ra=BIG.fromBytes(t);
		for (int i=0;i<MB;i++) {t[i]=b[i+MB];}
		rb=BIG.fromBytes(t);

		FP2 ra4=new FP2(ra,rb);

		for (int i=0;i<MB;i++) {t[i]=b[i+2*MB];}
		ra=BIG.fromBytes(t);
		for (int i=0;i<MB;i++) {t[i]=b[i+3*MB];}
		rb=BIG.fromBytes(t);

		FP2 rb4=new FP2(ra,rb);

		FP4 ra8=new FP4(ra4,rb4);

		for (int i=0;i<MB;i++) {t[i]=b[i+4*MB];}
		ra=BIG.fromBytes(t);
		for (int i=0;i<MB;i++) {t[i]=b[i+5*MB];}
		rb=BIG.fromBytes(t);

		ra4=new FP2(ra,rb);

		for (int i=0;i<MB;i++) {t[i]=b[i+6*MB];}
		ra=BIG.fromBytes(t);
		for (int i=0;i<MB;i++) {t[i]=b[i+7*MB];}
		rb=BIG.fromBytes(t);

		rb4=new FP2(ra,rb);

		FP4 rb8=new FP4(ra4,rb4);

		FP8 rx=new FP8(ra8,rb8);



		for (int i=0;i<MB;i++) {t[i]=b[i+8*MB];}
		ra=BIG.fromBytes(t);
		for (int i=0;i<MB;i++) {t[i]=b[i+9*MB];}
		rb=BIG.fromBytes(t);

		ra4=new FP2(ra,rb);

		for (int i=0;i<MB;i++) {t[i]=b[i+10*MB];}
		ra=BIG.fromBytes(t);
		for (int i=0;i<MB;i++) {t[i]=b[i+11*MB];}
		rb=BIG.fromBytes(t);

		rb4=new FP2(ra,rb);

		ra8=new FP4(ra4,rb4);

		for (int i=0;i<MB;i++) {t[i]=b[i+12*MB];}
		ra=BIG.fromBytes(t);
		for (int i=0;i<MB;i++) {t[i]=b[i+13*MB];}
		rb=BIG.fromBytes(t);

		ra4=new FP2(ra,rb);

		for (int i=0;i<MB;i++) {t[i]=b[i+14*MB];}
		ra=BIG.fromBytes(t);
		for (int i=0;i<MB;i++) {t[i]=b[i+15*MB];}
		rb=BIG.fromBytes(t);

		rb4=new FP2(ra,rb);

		rb8=new FP4(ra4,rb4);

		FP8 ry=new FP8(ra8,rb8);

		return new ECP8(rx,ry);
	}

/* convert this to hex string */
	public String toString() {
		ECP8 W=new ECP8(this);		
		W.affine();
		if (W.is_infinity()) return "infinity";
		return "("+W.x.toString()+","+W.y.toString()+")";
	}

/* Calculate RHS of twisted curve equation x^3+B/i */
	public static FP8 RHS(FP8 x) {
		FP8 r=new FP8(x);
		r.sqr();
		FP8 b=new FP8(new FP4(new FP2(new BIG(ROM.CURVE_B))));

		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.D_TYPE)
		{
			b.div_i();
		}
		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.M_TYPE)
		{
			b.times_i();
		}


		r.mul(x);
		r.add(b);

		r.reduce();
		return r;
	}

/* construct this from (x,y) - but set to O if not on curve */
	public ECP8(FP8 ix,FP8 iy) {
		x=new FP8(ix);
		y=new FP8(iy);
		z=new FP8(1);
		x.norm();
		FP8 rhs=RHS(x);
		FP8 y2=new FP8(y);
		y2.sqr();
		if (!y2.equals(rhs)) inf();
	}

/* construct this from x - but set to O if not on curve */
	public ECP8(FP8 ix) {
		x=new FP8(ix);
		y=new FP8(1);
		z=new FP8(1);
		x.norm();
		FP8 rhs=RHS(x);
		if (rhs.sqrt()) 
		{
			y.copy(rhs);
		}
		else {inf();}
	}

/* this+=this */
	public int dbl() {
//		if (INF) return -1;      

		FP8 iy=new FP8(y);
		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.D_TYPE)
		{
			iy.times_i(); 
		}
		FP8 t0=new FP8(y);                  //***** Change 
		t0.sqr();            
		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.D_TYPE)
		{		
			t0.times_i();
		}
		FP8 t1=new FP8(iy);  
		t1.mul(z);
		FP8 t2=new FP8(z);
		t2.sqr();

		z.copy(t0);
		z.add(t0); z.norm(); 
		z.add(z); 
		z.add(z); 
		z.norm();  

		t2.imul(3*ROM.CURVE_B_I); 
		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.M_TYPE)
		{
			t2.times_i();
		}

		FP8 x3=new FP8(t2);
		x3.mul(z); 

		FP8 y3=new FP8(t0);   

		y3.add(t2); y3.norm();
		z.mul(t1);
		t1.copy(t2); t1.add(t2); t2.add(t1); t2.norm();  
		t0.sub(t2); t0.norm();                           //y^2-9bz^2
		y3.mul(t0); y3.add(x3);                          //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2
		t1.copy(x); t1.mul(iy);						//
		x.copy(t0); x.norm(); x.mul(t1); x.add(x);       //(y^2-9bz^2)xy2

		x.norm(); 
		y.copy(y3); y.norm();

		return 1;
	}

/* this+=Q - return 0 for add, 1 for double, -1 for O */
	public int add(ECP8 Q) {

		int b=3*ROM.CURVE_B_I;
		FP8 t0=new FP8(x);
		t0.mul(Q.x);         // x.Q.x
		FP8 t1=new FP8(y);
		t1.mul(Q.y);		 // y.Q.y

		FP8 t2=new FP8(z);
		t2.mul(Q.z);
		FP8 t3=new FP8(x);
		t3.add(y); t3.norm();          //t3=X1+Y1
		FP8 t4=new FP8(Q.x);            
		t4.add(Q.y); t4.norm();			//t4=X2+Y2
		t3.mul(t4);						//t3=(X1+Y1)(X2+Y2)
		t4.copy(t0); t4.add(t1);		//t4=X1.X2+Y1.Y2

		t3.sub(t4); t3.norm(); 
		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.D_TYPE)
		{		
			t3.times_i();        //t3=(X1+Y1)(X2+Y2)-(X1.X2+Y1.Y2) = X1.Y2+X2.Y1
		}
		t4.copy(y);                    
		t4.add(z); t4.norm();			//t4=Y1+Z1
		FP8 x3=new FP8(Q.y);
		x3.add(Q.z); x3.norm();			//x3=Y2+Z2

		t4.mul(x3);						//t4=(Y1+Z1)(Y2+Z2)
		x3.copy(t1);					//
		x3.add(t2);						//X3=Y1.Y2+Z1.Z2
	
		t4.sub(x3); t4.norm(); 
		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.D_TYPE)
		{	
			t4.times_i();          //t4=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1
		}
		x3.copy(x); x3.add(z); x3.norm();	// x3=X1+Z1
		FP8 y3=new FP8(Q.x);				
		y3.add(Q.z); y3.norm();				// y3=X2+Z2
		x3.mul(y3);							// x3=(X1+Z1)(X2+Z2)
		y3.copy(t0);
		y3.add(t2);							// y3=X1.X2+Z1+Z2
		y3.rsub(x3); y3.norm();				// y3=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1

		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.D_TYPE)
		{
			t0.times_i();  // x.Q.x
			t1.times_i();  // y.Q.y
		}
		x3.copy(t0); x3.add(t0); 
		t0.add(x3); t0.norm();
		t2.imul(b); 	
		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.M_TYPE)
		{
			t2.times_i();
		}
		FP8 z3=new FP8(t1); z3.add(t2); z3.norm();
		t1.sub(t2); t1.norm(); 
		y3.imul(b); 
		if (CONFIG_CURVE.SEXTIC_TWIST==CONFIG_CURVE.M_TYPE)
		{
			y3.times_i(); 
		}
		x3.copy(y3); x3.mul(t4); t2.copy(t3); t2.mul(t1); x3.rsub(t2);
		y3.mul(t0); t1.mul(z3); y3.add(t1);
		t0.mul(t3); z3.mul(t4); z3.add(t0);

		x.copy(x3); x.norm(); 
		y.copy(y3); y.norm();
		z.copy(z3); z.norm();

		return 0;
	}

/* set this-=Q */
	public int sub(ECP8 Q) {
		ECP8 NQ=new ECP8(Q);
		NQ.neg();
		int D=add(NQ);
		return D;
	}

	public static FP2[] frob_constants() {
			BIG Fra=new BIG(ROM.Fra);
			BIG Frb=new BIG(ROM.Frb);
			FP2 X=new FP2(Fra,Frb);

			FP2 F0=new FP2(X); F0.sqr();
			FP2 F2=new FP2(F0);
			F2.mul_ip(); F2.norm();
			FP2 F1=new FP2(F2); F1.sqr();
			F2.mul(F1);

			F2.mul_ip(); F2.norm();
			F1.copy(X);
			if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE)
			{
				F1.mul_ip();  F1.norm();
				F1.inverse();
				F0.copy(F1); F0.sqr();
	
				F1.mul(F0);
			}
			if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE)
			{
				F0.copy(F1); F0.sqr();
				F1.mul(F0);
				F0.mul_ip();   F0.norm();
				F1.mul_ip();   F1.norm();
				F1.mul_ip();   F1.norm();
			}

			FP2[] F={F0,F1,F2};
			return F; 
	}

/* set this*=q, where q is Modulus, using Frobenius */
	public void frob(FP2 F[],int n)
	{
		for (int i=0;i<n;i++) {
			x.frob(F[2]);
			if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
				x.qmul(F[0]);
				x.times_i2();
			}
			if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
				x.qmul(F[0]);
				x.times_i2();
			}

			y.frob(F[2]);
			if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
				y.qmul(F[1]);
				y.times_i2();
				y.times_i();
			}
			if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
				y.qmul(F[1]);
				y.times_i();
			}

			z.frob(F[2]);
		}
	}

/* P*=e */
	public ECP8 mul(BIG e)
	{
/* fixed size windows */
		int i,b,nb,m,s,ns;
		BIG mt=new BIG();
		BIG t=new BIG();
		ECP8 P=new ECP8();
		ECP8 Q=new ECP8();
		ECP8 C=new ECP8();
		ECP8[] W=new ECP8[8];
		byte[] w=new byte[1+(BIG.NLEN*CONFIG_BIG.BASEBITS+3)/4];

		if (is_infinity()) return new ECP8();

/* precompute table */
		Q.copy(this);
		Q.dbl();
		W[0]=new ECP8();
		W[0].copy(this);

		for (i=1;i<8;i++)
		{
			W[i]=new ECP8();
			W[i].copy(W[i-1]);
			W[i].add(Q);
		}

/* make exponent odd - add 2P if even, P if odd */
		t.copy(e);
		s=t.parity();
		t.inc(1); t.norm(); ns=t.parity(); mt.copy(t); mt.inc(1); mt.norm();
		t.cmove(mt,s);
		Q.cmove(this,ns);
		C.copy(Q);

		nb=1+(t.nbits()+3)/4;
/* convert exponent to signed 4-bit window */
		for (i=0;i<nb;i++)
		{
			w[i]=(byte)(t.lastbits(5)-16);
			t.dec(w[i]); t.norm();
			t.fshr(4);	
		}
		w[nb]=(byte)t.lastbits(5);
	
		P.copy(W[(w[nb]-1)/2]);  
		for (i=nb-1;i>=0;i--)
		{
			Q.select(W,w[i]);
			P.dbl();
			P.dbl();
			P.dbl();
			P.dbl();
			P.add(Q);
		}
		P.sub(C);
		P.affine();
		return P;
	}

/* P=u0.Q0+u1*Q1+u2*Q2+u3*Q3... */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
// Side channel attack secure 

	public static ECP8 mul16(ECP8[] Q,BIG[] u)
	{
		int i,j,k,nb,pb1,pb2,pb3,pb4;
		ECP8 W=new ECP8();
		ECP8 P=new ECP8();
		ECP8[] T1=new ECP8[8];
		ECP8[] T2=new ECP8[8];
		ECP8[] T3=new ECP8[8];
		ECP8[] T4=new ECP8[8];

		BIG mt=new BIG();
		BIG[] t=new BIG[16];

		byte[] w1=new byte[BIG.NLEN*CONFIG_BIG.BASEBITS+1];
		byte[] s1=new byte[BIG.NLEN*CONFIG_BIG.BASEBITS+1];
		byte[] w2=new byte[BIG.NLEN*CONFIG_BIG.BASEBITS+1];
		byte[] s2=new byte[BIG.NLEN*CONFIG_BIG.BASEBITS+1];
		byte[] w3=new byte[BIG.NLEN*CONFIG_BIG.BASEBITS+1];
		byte[] s3=new byte[BIG.NLEN*CONFIG_BIG.BASEBITS+1];
		byte[] w4=new byte[BIG.NLEN*CONFIG_BIG.BASEBITS+1];
		byte[] s4=new byte[BIG.NLEN*CONFIG_BIG.BASEBITS+1];

		for (i=0;i<16;i++)
		{
			t[i]=new BIG(u[i]);
			//Q[i].affine();
			t[i].norm();
		}

        T1[0] = new ECP8(); T1[0].copy(Q[0]);  // Q[0]
        T1[1] = new ECP8(); T1[1].copy(T1[0]); T1[1].add(Q[1]);  // Q[0]+Q[1]
        T1[2] = new ECP8(); T1[2].copy(T1[0]); T1[2].add(Q[2]);  // Q[0]+Q[2]
        T1[3] = new ECP8(); T1[3].copy(T1[1]); T1[3].add(Q[2]);  // Q[0]+Q[1]+Q[2]
        T1[4] = new ECP8(); T1[4].copy(T1[0]); T1[4].add(Q[3]);  // Q[0]+Q[3]
        T1[5] = new ECP8(); T1[5].copy(T1[1]); T1[5].add(Q[3]);  // Q[0]+Q[1]+Q[3]
        T1[6] = new ECP8(); T1[6].copy(T1[2]); T1[6].add(Q[3]);  // Q[0]+Q[2]+Q[3]
        T1[7] = new ECP8(); T1[7].copy(T1[3]); T1[7].add(Q[3]);  // Q[0]+Q[1]+Q[2]+Q[3]

        T2[0] = new ECP8(); T2[0].copy(Q[4]);  // Q[0]
        T2[1] = new ECP8(); T2[1].copy(T2[0]); T2[1].add(Q[5]);  // Q[0]+Q[1]
        T2[2] = new ECP8(); T2[2].copy(T2[0]); T2[2].add(Q[6]);  // Q[0]+Q[2]
        T2[3] = new ECP8(); T2[3].copy(T2[1]); T2[3].add(Q[6]);  // Q[0]+Q[1]+Q[2]
        T2[4] = new ECP8(); T2[4].copy(T2[0]); T2[4].add(Q[7]);  // Q[0]+Q[3]
        T2[5] = new ECP8(); T2[5].copy(T2[1]); T2[5].add(Q[7]);  // Q[0]+Q[1]+Q[3]
        T2[6] = new ECP8(); T2[6].copy(T2[2]); T2[6].add(Q[7]);  // Q[0]+Q[2]+Q[3]
        T2[7] = new ECP8(); T2[7].copy(T2[3]); T2[7].add(Q[7]);  // Q[0]+Q[1]+Q[2]+Q[3]

        T3[0] = new ECP8(); T3[0].copy(Q[8]);  // Q[0]
        T3[1] = new ECP8(); T3[1].copy(T3[0]); T3[1].add(Q[9]);  // Q[0]+Q[1]
        T3[2] = new ECP8(); T3[2].copy(T3[0]); T3[2].add(Q[10]);  // Q[0]+Q[2]
        T3[3] = new ECP8(); T3[3].copy(T3[1]); T3[3].add(Q[10]);  // Q[0]+Q[1]+Q[2]
        T3[4] = new ECP8(); T3[4].copy(T3[0]); T3[4].add(Q[11]);  // Q[0]+Q[3]
        T3[5] = new ECP8(); T3[5].copy(T3[1]); T3[5].add(Q[11]);  // Q[0]+Q[1]+Q[3]
        T3[6] = new ECP8(); T3[6].copy(T3[2]); T3[6].add(Q[11]);  // Q[0]+Q[2]+Q[3]
        T3[7] = new ECP8(); T3[7].copy(T3[3]); T3[7].add(Q[11]);  // Q[0]+Q[1]+Q[2]+Q[3]

        T4[0] = new ECP8(); T4[0].copy(Q[12]);  // Q[0]
        T4[1] = new ECP8(); T4[1].copy(T4[0]); T4[1].add(Q[13]);  // Q[0]+Q[1]
        T4[2] = new ECP8(); T4[2].copy(T4[0]); T4[2].add(Q[14]);  // Q[0]+Q[2]
        T4[3] = new ECP8(); T4[3].copy(T4[1]); T4[3].add(Q[14]);  // Q[0]+Q[1]+Q[2]
        T4[4] = new ECP8(); T4[4].copy(T4[0]); T4[4].add(Q[15]);  // Q[0]+Q[3]
        T4[5] = new ECP8(); T4[5].copy(T4[1]); T4[5].add(Q[15]);  // Q[0]+Q[1]+Q[3]
        T4[6] = new ECP8(); T4[6].copy(T4[2]); T4[6].add(Q[15]);  // Q[0]+Q[2]+Q[3]
        T4[7] = new ECP8(); T4[7].copy(T4[3]); T4[7].add(Q[15]);  // Q[0]+Q[1]+Q[2]+Q[3]

    // Make it odd
        pb1=1-t[0].parity();
        t[0].inc(pb1);

        pb2=1-t[4].parity();
        t[4].inc(pb2);

        pb3=1-t[8].parity();
        t[8].inc(pb3);

        pb4=1-t[12].parity();
        t[12].inc(pb4);

    // Number of bits
        mt.zero();
        for (i=0;i<16;i++) {
			t[i].norm();
            mt.or(t[i]);
        }
        nb=1+mt.nbits();

    // Sign pivot 
        s1[nb-1]=1;
		s2[nb-1]=1;
        s3[nb-1]=1;
		s4[nb-1]=1;
        for (i=0;i<nb-1;i++) {
            t[0].fshr(1);
            s1[i]=(byte)(2*t[0].parity()-1);
            t[4].fshr(1);
            s2[i]=(byte)(2*t[4].parity()-1);
            t[8].fshr(1);
            s3[i]=(byte)(2*t[8].parity()-1);
            t[12].fshr(1);
            s4[i]=(byte)(2*t[12].parity()-1);
 
        }

    // Recoded exponent
        for (i=0; i<nb; i++) {
            w1[i]=0;
            k=1;
            for (j=1; j<4; j++) {
                byte bt=(byte)(s1[i]*t[j].parity());
                t[j].fshr(1);
                t[j].dec((int)(bt)>>1);
                t[j].norm();
                w1[i]+=bt*(byte)k;
                k*=2;
            }

            w2[i]=0;
            k=1;
            for (j=5; j<8; j++) {
                byte bt=(byte)(s2[i]*t[j].parity());
                t[j].fshr(1);
                t[j].dec((int)(bt)>>1);
                t[j].norm();
                w2[i]+=bt*(byte)k;
                k*=2;
            }

            w3[i]=0;
            k=1;
            for (j=9; j<12; j++) {
                byte bt=(byte)(s3[i]*t[j].parity());
                t[j].fshr(1);
                t[j].dec((int)(bt)>>1);
                t[j].norm();
                w3[i]+=bt*(byte)k;
                k*=2;
            }

            w4[i]=0;
            k=1;
            for (j=13; j<16; j++) {
                byte bt=(byte)(s4[i]*t[j].parity());
                t[j].fshr(1);
                t[j].dec((int)(bt)>>1);
                t[j].norm();
                w4[i]+=bt*(byte)k;
                k*=2;
            }

        } 

    // Main loop
        P.select(T1,(int)(2*w1[nb-1]+1));  
		W.select(T2,(int)(2*w2[nb-1]+1)); 
		P.add(W);
		W.select(T3,(int)(2*w3[nb-1]+1));
		P.add(W);
		W.select(T4,(int)(2*w4[nb-1]+1));
		P.add(W);
        for (i=nb-2;i>=0;i--) {
            P.dbl();
            W.select(T1,(int)(2*w1[i]+s1[i]));
            P.add(W);
            W.select(T2,(int)(2*w2[i]+s2[i]));
            P.add(W);
            W.select(T3,(int)(2*w3[i]+s3[i]));
            P.add(W);
            W.select(T4,(int)(2*w4[i]+s4[i]));
            P.add(W);

        }

    // apply correction
        W.copy(P);   
        W.sub(Q[0]);
        P.cmove(W,pb1);   

        W.copy(P);   
        W.sub(Q[4]);
        P.cmove(W,pb2);  

        W.copy(P);   
        W.sub(Q[8]);
        P.cmove(W,pb3);   

        W.copy(P);   
        W.sub(Q[12]);
        P.cmove(W,pb4);  

		P.affine();
		return P;
	}        

/* needed for SOK */
	public static ECP8 mapit(byte[] h)
	{
		BIG q=new BIG(ROM.Modulus);
		BIG x=BIG.fromBytes(h);
		BIG one=new BIG(1);
		FP8 X;
		FP2 X2;
		FP4 X4;
		ECP8 Q;
		x.mod(q);
		while (true)
		{
			X2=new FP2(one,x);
			X4=new FP4(X2);
			X=new FP8(X4);
			Q=new ECP8(X);
			if (!Q.is_infinity()) break;
			x.inc(1); x.norm();
		}

		FP2[] F=ECP8.frob_constants();
		x=new BIG(ROM.CURVE_Bnx);

/* Efficient hash maps to G2 on BLS curves - Budroni, Pintore */

		ECP8 xQ=Q.mul(x);
		ECP8 x2Q=xQ.mul(x);
		ECP8 x3Q=x2Q.mul(x);
		ECP8 x4Q=x3Q.mul(x);
		ECP8 x5Q=x4Q.mul(x);
		ECP8 x6Q=x5Q.mul(x);
		ECP8 x7Q=x6Q.mul(x);
		ECP8 x8Q=x7Q.mul(x);

		if (CONFIG_CURVE.SIGN_OF_X==CONFIG_CURVE.NEGATIVEX)
		{
			xQ.neg();
			x3Q.neg();
			x5Q.neg();
			x7Q.neg();
		}	

		x8Q.sub(x7Q);
		x8Q.sub(Q);

		x7Q.sub(x6Q);
		x7Q.frob(F,1);

		x6Q.sub(x5Q);
		x6Q.frob(F,2);

		x5Q.sub(x4Q);
		x5Q.frob(F,3);

		x4Q.sub(x3Q);
		x4Q.frob(F,4);

		x3Q.sub(x2Q);
		x3Q.frob(F,5);

		x2Q.sub(xQ);
		x2Q.frob(F,6);

		xQ.sub(Q);
		xQ.frob(F,7);

		Q.dbl();
		Q.frob(F,8);

		Q.add(x8Q);
		Q.add(x7Q);
		Q.add(x6Q);
		Q.add(x5Q);

		Q.add(x4Q);
		Q.add(x3Q);
		Q.add(x2Q);
		Q.add(xQ);

		Q.affine();
		return Q;
	}

	public static ECP8 generator()
	{
		return new ECP8(
			new FP8(
				new FP4(
					new FP2(
						new BIG(ROM.CURVE_Pxaaa),new BIG(ROM.CURVE_Pxaab)),
					new FP2(
						new BIG(ROM.CURVE_Pxaba),new BIG(ROM.CURVE_Pxabb))),
				new FP4(
					new FP2(
						new BIG(ROM.CURVE_Pxbaa),new BIG(ROM.CURVE_Pxbab)),
					new FP2(
						new BIG(ROM.CURVE_Pxbba),new BIG(ROM.CURVE_Pxbbb)))),
			new FP8(
				new FP4(
					new FP2(
						new BIG(ROM.CURVE_Pyaaa),new BIG(ROM.CURVE_Pyaab)),
					new FP2(
						new BIG(ROM.CURVE_Pyaba),new BIG(ROM.CURVE_Pyabb))),
				new FP4(
					new FP2(
						new BIG(ROM.CURVE_Pybaa),new BIG(ROM.CURVE_Pybab)),
					new FP2(
						new BIG(ROM.CURVE_Pybba),new BIG(ROM.CURVE_Pybbb)))));

	}

}
