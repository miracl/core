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

/* MPIN API Functions */

package org.apache.milagro.amcl.XXX;

import java.util.Date;

import org.apache.milagro.amcl.RAND;
import org.apache.milagro.amcl.HASH256;
import org.apache.milagro.amcl.HASH384;
import org.apache.milagro.amcl.HASH512;
import org.apache.milagro.amcl.AES;

public class MPIN256
{
	public static final int EFS=CONFIG_BIG.MODBYTES;
	public static final int EGS=CONFIG_BIG.MODBYTES;
	public static final int INVALID_POINT=-14;
	public static final int BAD_PARAMS=-11;
	public static final int WRONG_ORDER=-18;
	public static final int BAD_PIN=-19;

/* Configure your PIN here */

	public static final int MAXPIN=10000;  /* PIN less than this */
	public static final int PBLEN=14;      /* Number of bits in PIN */
	public static final int TS=10;         /* 10 for 4 digit PIN, 14 for 6-digit PIN - 2^TS/TS approx = sqrt(MAXPIN) */
	public static final int TRAP=200;      /* 200 for 4 digit PIN, 2000 for 6-digit PIN  - approx 2*sqrt(MAXPIN) */

/* Hash number (optional) and string to array size of Bigs */

	public static byte[] hashit(int sha,int n,byte[] B,int len)
	{
		byte[] R=null;

		if (sha==CONFIG_CURVE.SHA256)
		{
			HASH256 H=new HASH256();
			if (n>0) H.process_num(n);

			H.process_array(B);
			R=H.hash();
		}
		if (sha==CONFIG_CURVE.SHA384)
		{
			HASH384 H=new HASH384();
			if (n>0) H.process_num(n);
			H.process_array(B);
			R=H.hash();
		}
		if (sha==CONFIG_CURVE.SHA512)
		{
			HASH512 H=new HASH512();
			if (n>0) H.process_num(n);
			H.process_array(B);
			R=H.hash();
		}
		if (R==null) return null;
		byte[] W=new byte[len];

		if (sha>=len)
			for (int i=0;i<len;i++) W[i]=R[i];
		else
		{
			for (int i=0;i<sha;i++) W[i+len-sha]=R[i];
            for (int i=0;i<len-sha;i++) W[i]=0;
		}
		return W;
	}

	/* return time in slots since epoch */
	public static int today() {
		Date date=new Date();
		return (int) (date.getTime()/(1000*60*1440));
	}

	public static byte[] HASH_ID(int sha,byte[] ID,int len)
	{
		return hashit(sha,0,ID,len);
	}

/* Hash the M-Pin transcript - new */

	public static byte[] HASH_ALL(int sha,byte[] HID,byte[] xID,byte[] xCID,byte[] SEC,byte[] Y,byte[] R,byte[] W,int len)
	{
		int i,ilen,tlen=0;

		ilen=HID.length+SEC.length+Y.length+R.length+W.length;
		if (xCID!=null) ilen+=xCID.length;
		else ilen+=xID.length;

		byte[] T = new byte[ilen];

		for (i=0;i<HID.length;i++) T[i]=HID[i];
		tlen+=HID.length;
		if (xCID!=null)
		{
			for (i=0;i<xCID.length;i++) T[i+tlen]=xCID[i];
			tlen+=xCID.length;
		}	
		else
		{
			for (i=0;i<xID.length;i++) T[i+tlen]=xID[i];
			tlen+=xID.length;
		}	
		for (i=0;i<SEC.length;i++) T[i+tlen]=SEC[i];
		tlen+=SEC.length;		
		for (i=0;i<Y.length;i++) T[i+tlen]=Y[i];
		tlen+=Y.length;	
		for (i=0;i<R.length;i++) T[i+tlen]=R[i];
		tlen+=R.length;		
		for (i=0;i<W.length;i++) T[i+tlen]=W[i];
		tlen+=W.length;		

		return hashit(sha,0,T,len);
	}

/* return time since epoch */
	public static int GET_TIME() {
		Date date=new Date();
		return (int) (date.getTime()/1000);
	}

	public static byte[] mpin_hash(int sha,FP16 c,ECP U)
	{
		byte[] w=new byte[EFS];
		byte[] t=new byte[18*EFS];
		byte[] h=null;
		c.geta().geta().geta().getA().toBytes(w); for (int i=0;i<EFS;i++) t[i]=w[i];
		c.geta().geta().geta().getB().toBytes(w); for (int i=EFS;i<2*EFS;i++) t[i]=w[i-EFS];
		c.geta().geta().getb().getA().toBytes(w); for (int i=2*EFS;i<3*EFS;i++) t[i]=w[i-2*EFS];
		c.geta().geta().getb().getB().toBytes(w); for (int i=3*EFS;i<4*EFS;i++) t[i]=w[i-3*EFS];
		c.geta().getb().geta().getA().toBytes(w); for (int i=4*EFS;i<5*EFS;i++) t[i]=w[i-4*EFS];
		c.geta().getb().geta().getB().toBytes(w); for (int i=5*EFS;i<6*EFS;i++) t[i]=w[i-5*EFS];
		c.geta().getb().getb().getA().toBytes(w); for (int i=6*EFS;i<7*EFS;i++) t[i]=w[i-6*EFS];
		c.geta().getb().getb().getB().toBytes(w); for (int i=7*EFS;i<8*EFS;i++) t[i]=w[i-7*EFS];

		c.getb().geta().geta().getA().toBytes(w); for (int i=8*EFS;i<9*EFS;i++) t[i]=w[i-8*EFS];
		c.getb().geta().geta().getB().toBytes(w); for (int i=9*EFS;i<10*EFS;i++) t[i]=w[i-9*EFS];
		c.getb().geta().getb().getA().toBytes(w); for (int i=10*EFS;i<11*EFS;i++) t[i]=w[i-10*EFS];
		c.getb().geta().getb().getB().toBytes(w); for (int i=11*EFS;i<12*EFS;i++) t[i]=w[i-11*EFS];
		c.getb().getb().geta().getA().toBytes(w); for (int i=12*EFS;i<13*EFS;i++) t[i]=w[i-12*EFS];
		c.getb().getb().geta().getB().toBytes(w); for (int i=13*EFS;i<14*EFS;i++) t[i]=w[i-13*EFS];
		c.getb().getb().getb().getA().toBytes(w); for (int i=14*EFS;i<15*EFS;i++) t[i]=w[i-14*EFS];
		c.getb().getb().getb().getB().toBytes(w); for (int i=15*EFS;i<16*EFS;i++) t[i]=w[i-15*EFS];
	
		

		U.getX().toBytes(w); for (int i=16*EFS;i<17*EFS;i++) t[i]=w[i-16*EFS];
		U.getY().toBytes(w); for (int i=17*EFS;i<18*EFS;i++) t[i]=w[i-17*EFS];
		
		if (sha==CONFIG_CURVE.SHA256)
		{
			HASH256 H=new HASH256();
			H.process_array(t);
			h=H.hash();
		}
		if (sha==CONFIG_CURVE.SHA384)
		{
			HASH384 H=new HASH384();
			H.process_array(t);
			h=H.hash();
		}
		if (sha==CONFIG_CURVE.SHA512)
		{
			HASH512 H=new HASH512();
			H.process_array(t);
			h=H.hash();
		}
		if (h==null) return null;
		byte[] R=new byte[CONFIG_CURVE.AESKEY];
		for (int i=0;i<CONFIG_CURVE.AESKEY;i++) R[i]=h[i];
		return R;
	}

/* these next two functions help to implement elligator squared - http://eprint.iacr.org/2014/043 */
/* maps a random u to a point on the curve */
	public static ECP map(BIG u,int cb)
	{
		ECP P;
		BIG x=new BIG(u);
		BIG p=new BIG(ROM.Modulus);
		x.mod(p);
		while (true)
		{
			P=new ECP(x,cb);
			if (!P.is_infinity()) break;
			x.inc(1);  x.norm();
		}
		return P;
	}

/* returns u derived from P. Random value in range 1 to return value should then be added to u */
	public static int unmap(BIG u,ECP P)
	{
		int s=P.getS();
		ECP R;
		int r=0;
		BIG x=P.getX();
		u.copy(x);
		while (true)
		{
			u.dec(1); u.norm();
			r++;
			R=new ECP(u,s);
			if (!R.is_infinity()) break;
		}
		return r;
	}



/* these next two functions implement elligator squared - http://eprint.iacr.org/2014/043 */
/* Elliptic curve point E in format (0x04,x,y} is converted to form {0x0-,u,v} */
/* Note that u and v are indistinguisible from random strings */
	public static int ENCODING(RAND rng,byte[] E)
	{
		int rn,m,su,sv;
		byte[] T=new byte[EFS];

		for (int i=0;i<EFS;i++) T[i]=E[i+1];
		BIG u=BIG.fromBytes(T);
		for (int i=0;i<EFS;i++) T[i]=E[i+EFS+1];
		BIG v=BIG.fromBytes(T);
		
		ECP P=new ECP(u,v);
		if (P.is_infinity()) return INVALID_POINT;

		BIG p=new BIG(ROM.Modulus);
		u=BIG.randomnum(p,rng);

		su=rng.getByte();  su%=2;
		
		ECP W=map(u,su);
		P.sub(W); 
		sv=P.getS();
		rn=unmap(v,P);
		m=rng.getByte(); m%=rn;
		v.inc(m+1);
		E[0]=(byte)(su+2*sv);
		u.toBytes(T);
		for (int i=0;i<EFS;i++) E[i+1]=T[i];
		v.toBytes(T);
		for (int i=0;i<EFS;i++) E[i+EFS+1]=T[i];		
		
		return 0;
	}

	public static int DECODING(byte[] D)
	{
		int su,sv;
		byte[] T=new byte[EFS];

		if ((D[0]&0x04)!=0) return INVALID_POINT;

		for (int i=0;i<EFS;i++) T[i]=D[i+1];
		BIG u=BIG.fromBytes(T);
		for (int i=0;i<EFS;i++) T[i]=D[i+EFS+1];
		BIG v=BIG.fromBytes(T);

		su=D[0]&1;
		sv=(D[0]>>1)&1;
		ECP W=map(u,su);
		ECP P=map(v,sv);
		P.add(W); 
		u=P.getX();
		v=P.getY();
		D[0]=0x04;
		u.toBytes(T);
		for (int i=0;i<EFS;i++) D[i+1]=T[i];
		v.toBytes(T);
		for (int i=0;i<EFS;i++) D[i+EFS+1]=T[i];		
		
		return 0;
	}

/* R=R1+R2 in group G1 */
	public static int RECOMBINE_G1(byte[] R1,byte[] R2,byte[] R)
	{
		ECP P=ECP.fromBytes(R1);
		ECP Q=ECP.fromBytes(R2);

		if (P.is_infinity() || Q.is_infinity()) return INVALID_POINT;

		P.add(Q); 

		P.toBytes(R,false);
		return 0;
	}

/* W=W1+W2 in group G2 */
	public static int RECOMBINE_G2(byte[] W1,byte[] W2,byte[] W)
	{
		ECP8 P=ECP8.fromBytes(W1);
		ECP8 Q=ECP8.fromBytes(W2);

		if (P.is_infinity() || Q.is_infinity()) return INVALID_POINT;

		P.add(Q); 
	
		P.toBytes(W);
		return 0;
	}
	
/* create random secret S */
	public static int RANDOM_GENERATE(RAND rng,byte[] S)
	{
		BIG s;
		BIG r=new BIG(ROM.CURVE_Order);
		s=BIG.randtrunc(r,16*CONFIG_CURVE.AESKEY,rng);
		s.toBytes(S);
		return 0;
	}

/* Extract PIN from TOKEN for identity CID */
	public static int EXTRACT_PIN(int sha,byte[] CID,int pin,byte[] TOKEN)
	{
		ECP P=ECP.fromBytes(TOKEN);
		if (P.is_infinity()) return INVALID_POINT;
		byte[] h=hashit(sha,0,CID,EFS);
		ECP R=ECP.mapit(h);


		pin%=MAXPIN;

		R=R.pinmul(pin,PBLEN);
		P.sub(R);

		P.toBytes(TOKEN,false);

		return 0;
	}

/* Implement step 2 on client side of MPin protocol */
	public static int CLIENT_2(byte[] X,byte[] Y,byte[] SEC)
	{
		BIG r=new BIG(ROM.CURVE_Order);
		ECP P=ECP.fromBytes(SEC);
		if (P.is_infinity()) return INVALID_POINT;

		BIG px=BIG.fromBytes(X);
		BIG py=BIG.fromBytes(Y);
		px.add(py);
		px.mod(r);

		P=PAIR256.G1mul(P,px);
		P.neg();
		P.toBytes(SEC,false);
		return 0;
	}

/* Implement step 1 on client side of MPin protocol */
	public static int CLIENT_1(int sha,int date,byte[] CLIENT_ID,RAND rng,byte[] X,int pin,byte[] TOKEN,byte[] SEC,byte[] xID,byte[] xCID,byte[] PERMIT)
	{
		BIG r=new BIG(ROM.CURVE_Order);
		BIG x;
		if (rng!=null)
		{
			x=BIG.randtrunc(r,16*CONFIG_CURVE.AESKEY,rng);
			x.toBytes(X);
		}
		else
		{
			x=BIG.fromBytes(X);
		}
		ECP P,T,W;
		BIG px;

		byte[] h=hashit(sha,0,CLIENT_ID,EFS);
		P=ECP.mapit(h);
	
		T=ECP.fromBytes(TOKEN);
		if (T.is_infinity()) return INVALID_POINT;

		pin%=MAXPIN;
		W=P.pinmul(pin,PBLEN);
		T.add(W);
		if (date!=0)
		{
			W=ECP.fromBytes(PERMIT);
			if (W.is_infinity()) return INVALID_POINT;
			T.add(W);
			h=hashit(sha,date,h,EFS);
			W=ECP.mapit(h);
			if (xID!=null)
			{
				P=PAIR256.G1mul(P,x);
				P.toBytes(xID,false);
				W=PAIR256.G1mul(W,x);
				P.add(W);
			}
			else
			{
				P.add(W);
				P=PAIR256.G1mul(P,x);
			}
			if (xCID!=null) P.toBytes(xCID,false);
		}
		else
		{
			if (xID!=null)
			{
				P=PAIR256.G1mul(P,x);
				P.toBytes(xID,false);
			}
		}

		//T.affine();
		T.toBytes(SEC,false);
		return 0;
	}

/* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
	public static int GET_SERVER_SECRET(byte[] S,byte[] SST)
	{
		ECP8 Q=ECP8.generator();
		BIG s=BIG.fromBytes(S);
		Q=PAIR256.G2mul(Q,s);
		Q.toBytes(SST);
		return 0;
	}

/*
 W=x*H(G);
 if RNG == NULL then X is passed in 
 if RNG != NULL the X is passed out 
 if type=0 W=x*G where G is point on the curve, else W=x*M(G), where M(G) is mapping of octet G to point on the curve
*/
	public static int GET_G1_MULTIPLE(RAND rng, int type,byte[] X,byte[] G,byte[] W)
	{
		BIG x;
		BIG r=new BIG(ROM.CURVE_Order);
		if (rng!=null)
		{
			x=BIG.randtrunc(r,16*CONFIG_CURVE.AESKEY,rng);
			x.toBytes(X);
		}
		else
		{
			x=BIG.fromBytes(X);
		}
		ECP P;
		if (type==0)
		{
			P=ECP.fromBytes(G);
			if (P.is_infinity()) return INVALID_POINT;
		}
		else
			P=ECP.mapit(G);

		PAIR256.G1mul(P,x).toBytes(W,false);
		return 0;
	}

/* Client secret CST=S*H(CID) where CID is client ID and S is master secret */
/* CID is hashed externally */
	public static int GET_CLIENT_SECRET(byte[] S,byte[] CID,byte[] CST)
	{
		return GET_G1_MULTIPLE(null,1,S,CID,CST);
	}

/* Time Permit CTT=S*(date|H(CID)) where S is master secret */
	public static int GET_CLIENT_PERMIT(int sha,int date,byte[] S,byte[] CID,byte[] CTT)
	{
		byte[] h=hashit(sha,date,CID,EFS);
		ECP P=ECP.mapit(h);

		BIG s=BIG.fromBytes(S);
		ECP OP=PAIR256.G1mul(P,s);

		OP.toBytes(CTT,false);
		return 0;
	}

/* Outputs H(CID) and H(T|H(CID)) for time permits. If no time permits set HID=HTID */
	public static void SERVER_1(int sha,int date,byte[] CID,byte[] HID,byte[] HTID)
	{
		byte[] h=hashit(sha,0,CID,EFS);
		ECP R,P=ECP.mapit(h);

		P.toBytes(HID,false);   // new
		if (date!=0)
		{
			h=hashit(sha,date,h,EFS);
			R=ECP.mapit(h);
			P.add(R); 
			P.toBytes(HTID,false);
		}
	}

/* Implement step 2 of MPin protocol on server side */
	public static int SERVER_2(int date,byte[] HID,byte[] HTID,byte[] Y,byte[] SST,byte[] xID,byte[] xCID,byte[] mSEC,byte[] E,byte[] F)
	{
		BIG q=new BIG(ROM.Modulus);
		ECP8 Q=ECP8.generator();

		ECP8 sQ=ECP8.fromBytes(SST);
		if (sQ.is_infinity()) return INVALID_POINT;	

		ECP R;
		if (date!=0)
			R=ECP.fromBytes(xCID);
		else 
		{
			if (xID==null) return BAD_PARAMS;
			R=ECP.fromBytes(xID);
		}
		if (R.is_infinity()) return INVALID_POINT;

		BIG y=BIG.fromBytes(Y);
		ECP P;
		if (date!=0) P=ECP.fromBytes(HTID);
		else 
		{
			if (HID==null) return BAD_PARAMS;
			P=ECP.fromBytes(HID);
		}
	
		if (P.is_infinity()) return INVALID_POINT;

		P=PAIR256.G1mul(P,y);
		P.add(R); 
		R=ECP.fromBytes(mSEC);
		if (R.is_infinity()) return INVALID_POINT;

		FP48 g;

		g=PAIR256.ate2(Q,R,sQ,P);
		g=PAIR256.fexp(g);

		if (!g.isunity())
		{
			if (HID!=null && xID!=null && E!=null && F!=null)
			{
				g.toBytes(E);
				if (date!=0)
				{
					P=ECP.fromBytes(HID);
					if (P.is_infinity()) return INVALID_POINT;
					R=ECP.fromBytes(xID);
					if (R.is_infinity()) return INVALID_POINT;

					P=PAIR256.G1mul(P,y);
					P.add(R); 
				}
				g=PAIR256.ate(Q,P);
				g=PAIR256.fexp(g);
				g.toBytes(F);
			}
			return BAD_PIN;
		}

		return 0;
	}

/* Pollards kangaroos used to return PIN error */
	public static int KANGAROO(byte[] E,byte[] F)
	{
		FP48 ge=FP48.fromBytes(E);
		FP48 gf=FP48.fromBytes(F);
		int[] distance = new int[TS];
		FP48 t=new FP48(gf);
		FP48[] table=new FP48[TS];
		int i,j,m,s,dn,dm,res,steps;

		s=1;
		for (m=0;m<TS;m++)
		{
			distance[m]=s;
			table[m]=new FP48(t);
			s*=2;
			t.usqr();
		}
		t.one();
		dn=0;
		for (j=0;j<TRAP;j++)
		{
			i=t.geta().geta().geta().geta().getA().lastbits(20)%TS;
			t.mul(table[i]);
			dn+=distance[i];
		}
		gf.copy(t); gf.conj();
		steps=0; dm=0;
		res=0;
		while (dm-dn<MAXPIN)
		{
			steps++;
			if (steps>4*TRAP) break;
			i=ge.geta().geta().geta().geta().getA().lastbits(20)%TS;
			ge.mul(table[i]);
			dm+=distance[i];
			if (ge.equals(t))
			{
				res=dm-dn;
				break;
			}
			if (ge.equals(gf))
			{
				res=dn-dm;
				break;
			}

		}
		if (steps>4*TRAP || dm-dn>=MAXPIN) {res=0; }    // Trap Failed  - probable invalid token
		return res;
	}

/* Functions to support M-Pin Full */

	public static int PRECOMPUTE(byte[] TOKEN,byte[] CID,byte[] G1,byte[] G2)
	{
		ECP P,T;
		FP48 g;

		T=ECP.fromBytes(TOKEN);
		if (T.is_infinity()) return INVALID_POINT; 

		P=ECP.mapit(CID);

		ECP8 Q=ECP8.generator();

		g=PAIR256.ate(Q,T);
		g=PAIR256.fexp(g);
		g.toBytes(G1);

		g=PAIR256.ate(Q,P);
		g=PAIR256.fexp(g);
		g.toBytes(G2);

		return 0;
	}



/* calculate common key on client side */
/* wCID = w.(A+AT) */
	public static int CLIENT_KEY(int sha,byte[] G1,byte[] G2,int pin,byte[] R,byte[] X,byte[] H,byte[] wCID,byte[] CK)
	{
		byte[] t;

		FP48 g1=FP48.fromBytes(G1);
		FP48 g2=FP48.fromBytes(G2);
		BIG z=BIG.fromBytes(R);
		BIG x=BIG.fromBytes(X);
		BIG h=BIG.fromBytes(H);

		ECP W=ECP.fromBytes(wCID);
		if (W.is_infinity()) return INVALID_POINT; 

		W=PAIR256.G1mul(W,x);

		BIG r=new BIG(ROM.CURVE_Order);

		z.add(h);	//new
		z.mod(r);

		g2.pinpow(pin,PBLEN);
		g1.mul(g2);

		FP16 c=g1.compow(z,r);

		t=mpin_hash(sha,c,W);

		for (int i=0;i<CONFIG_CURVE.AESKEY;i++) CK[i]=t[i];

		return 0;
	}

/* calculate common key on server side */
/* Z=r.A - no time permits involved */

	public static int SERVER_KEY(int sha,byte[] Z,byte[] SST,byte[] W,byte[] H,byte[] HID,byte[] xID,byte[] xCID,byte[] SK)
	{
		byte[] t;

		ECP8 sQ=ECP8.fromBytes(SST);
		if (sQ.is_infinity()) return INVALID_POINT; 
		ECP R=ECP.fromBytes(Z);
		if (R.is_infinity()) return INVALID_POINT; 
		ECP A=ECP.fromBytes(HID);
		if (A.is_infinity()) return INVALID_POINT; 

		ECP U;
		if (xCID!=null)
			U=ECP.fromBytes(xCID);
		else
			U=ECP.fromBytes(xID);
		if (U.is_infinity()) return INVALID_POINT; 

		BIG w=BIG.fromBytes(W);
		BIG h=BIG.fromBytes(H);
		A=PAIR256.G1mul(A,h);	// new
		R.add(A); 

		U=PAIR256.G1mul(U,w);
		FP48 g=PAIR256.ate(sQ,R);
		g=PAIR256.fexp(g);

		FP16 c=g.trace();

		t=mpin_hash(sha,c,U);

		for (int i=0;i<CONFIG_CURVE.AESKEY;i++) SK[i]=t[i];

		return 0;
	}

/* Generate Y = H(epoch, xCID/xID) */
	public static void GET_Y(int sha,int TimeValue,byte[] xCID,byte[] Y)
	{
		byte[] h = hashit(sha,TimeValue,xCID,EFS);
		BIG y = BIG.fromBytes(h);
		BIG q=new BIG(ROM.CURVE_Order);
		y.mod(q);
		y.toBytes(Y);
	}
        
/* One pass MPIN Client */
	public static int CLIENT(int sha,int date,byte[] CLIENT_ID,RAND RNG,byte[] X,int pin,byte[] TOKEN,byte[] SEC,byte[] xID,byte[] xCID,byte[] PERMIT, int TimeValue, byte[] Y)
	{
		int rtn=0;
        
		byte[] pID;
		if (date == 0)
			pID = xID;
		else
			pID = xCID;
          
		rtn = CLIENT_1(sha,date,CLIENT_ID,RNG,X,pin,TOKEN,SEC,xID,xCID,PERMIT);
		if (rtn != 0)
			return rtn;
        
		GET_Y(sha,TimeValue,pID,Y);
        
		rtn = CLIENT_2(X,Y,SEC);
		if (rtn != 0)
		return rtn;
        
		return 0;
	}
        
/* One pass MPIN Server */
	public static int SERVER(int sha,int date,byte[] HID,byte[] HTID,byte[] Y,byte[] SST,byte[] xID,byte[] xCID,byte[] SEC,byte[] E,byte[] F,byte[] CID, int TimeValue)
	{
		int rtn=0;
        
		byte[] pID;
		if (date == 0)
			pID = xID;
		else
			pID = xCID;
          
		SERVER_1(sha,date,CID,HID,HTID);
        
		GET_Y(sha,TimeValue,pID,Y);
          
		rtn = SERVER_2(date,HID,HTID,Y,SST,xID,xCID,SEC,E,F);
		if (rtn != 0)
			return rtn;
        
		return 0;
	}
}
