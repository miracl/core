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

/* AMCL BLS Curve pairing functions */

//#define HAS_MAIN

#include "pair256_ZZZ.h"


// Point doubling for pairings
static void PAIR_ZZZ_double(ECP8_ZZZ *A,FP8_YYY *AA,FP8_YYY *BB,FP8_YYY *CC)
{
	FP8_YYY YY;   
 	FP8_YYY_copy(CC,&(A->x));	//FP8 XX=new FP8(A.getx());  //X
	FP8_YYY_copy(&YY,&(A->y));	//FP8 YY=new FP8(A.gety());  //Y
	FP8_YYY_copy(BB,&(A->z));	//FP8 ZZ=new FP8(A.getz());  //Z

	FP8_YYY_copy(AA,&YY);		//FP8 YZ=new FP8(YY);        //Y 
	FP8_YYY_mul(AA,AA,BB);		//YZ.mul(ZZ);                //YZ
	FP8_YYY_sqr(CC,CC);			//XX.sqr();	               //X^2
	FP8_YYY_sqr(&YY,&YY);		//YY.sqr();	               //Y^2
	FP8_YYY_sqr(BB,BB);			//ZZ.sqr();			       //Z^2

	FP8_YYY_add(AA,AA,AA);		
	FP8_YYY_neg(AA,AA);
	FP8_YYY_norm(AA);			// -2YZ
	FP8_YYY_times_i(AA);		// -2YZi

	FP8_YYY_imul(BB,BB,3*CURVE_B_I_ZZZ);	//3Bz^2 
	FP8_YYY_imul(CC,CC,3);				//3X^2

#if SEXTIC_TWIST_ZZZ==D_TYPE
	FP8_YYY_times_i(&YY);					// Y^2.i
	FP8_YYY_times_i(CC);					// 3X^2.i
#endif

#if SEXTIC_TWIST_ZZZ==M_TYPE
	FP8_YYY_times_i(BB);					// 3Bz^2.i
#endif

	FP8_YYY_sub(BB,BB,&YY);
	FP8_YYY_norm(BB);

	ECP8_ZZZ_dbl(A);			//A.dbl();
}

// Point addition for pairings
static void PAIR_ZZZ_add(ECP8_ZZZ *A,ECP8_ZZZ *B,FP8_YYY *AA,FP8_YYY *BB,FP8_YYY *CC)
{
	FP8_YYY T1;
	FP8_YYY_copy(AA,&(A->x));		//FP8 X1=new FP8(A.getx());    // X1
	FP8_YYY_copy(CC,&(A->y));		//FP8 Y1=new FP8(A.gety());    // Y1
	FP8_YYY_copy(&T1,&(A->z));		//FP8 T1=new FP8(A.getz());    // Z1
			
	FP8_YYY_copy(BB,&T1);			//FP8 T2=new FP8(A.getz());    // Z1

	FP8_YYY_mul(&T1,&T1,&(B->y));	//T1.mul(B.gety());    // T1=Z1.Y2 
	FP8_YYY_mul(BB,BB,&(B->x));		//T2.mul(B.getx());    // T2=Z1.X2

	FP8_YYY_sub(AA,AA,BB);			//X1.sub(T2); 
	FP8_YYY_norm(AA);				//X1.norm();  // X1=X1-Z1.X2
	FP8_YYY_sub(CC,CC,&T1);			//Y1.sub(T1); 
	FP8_YYY_norm(CC);				//Y1.norm();  // Y1=Y1-Z1.Y2

	FP8_YYY_copy(&T1,AA);			//T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_ZZZ==M_TYPE
	FP8_YYY_times_i(AA);
	FP8_YYY_norm(AA);
#endif

	FP8_YYY_mul(&T1,&T1,&(B->y));	//T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

	FP8_YYY_copy(BB,CC);			//T2.copy(Y1);            // T2=Y1-Z1.Y2
	FP8_YYY_mul(BB,BB,&(B->x));		//T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
	FP8_YYY_sub(BB,BB,&T1);			//T2.sub(T1); 
	FP8_YYY_norm(BB);				//T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2
  
	FP8_YYY_neg(CC,CC);				//Y1.neg(); 
	FP8_YYY_norm(CC);				//Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

	ECP8_ZZZ_add(A,B);				//A.add(B);
}


/* Line function */
static void PAIR_ZZZ_line(FP48_YYY *v,ECP8_ZZZ *A,ECP8_ZZZ *B,FP_YYY *Qx,FP_YYY *Qy)
{
	FP8_YYY AA,BB,CC;
    FP16_YYY a,b,c;

	if (A==B)
		PAIR_ZZZ_double(A,&AA,&BB,&CC);
	else
		PAIR_ZZZ_add(A,B,&AA,&BB,&CC);

	FP8_YYY_tmul(&CC,&CC,Qx);		
	FP8_YYY_tmul(&AA,&AA,Qy);		

	FP16_YYY_from_FP8s(&a,&AA,&BB);	
#if SEXTIC_TWIST_ZZZ==D_TYPE
	FP16_YYY_from_FP8(&b,&CC);		
	FP16_YYY_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
	FP16_YYY_zero(&b);
	FP16_YYY_from_FP8H(&c,&CC);		
#endif

    FP48_YYY_from_FP16s(v,&a,&b,&c);
	v->type=FP_SPARSER;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int PAIR_ZZZ_nbits(BIG_XXX n3,BIG_XXX n)
{
	BIG_XXX x;
    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);

    BIG_XXX_copy(n,x);
    BIG_XXX_norm(n);
	BIG_XXX_pmul(n3,n,3);
	BIG_XXX_norm(n3);

    return BIG_XXX_nbits(n3);
}

/*
	For multi-pairing, product of n pairings
	1. Declare FP24 array of length number of bits in Ate parameter
	2. Initialise this array by calling PAIR_initmp()
	3. Accumulate each pairing by calling PAIR_another() n times
	4. Call PAIR_miller()
	5. Call final exponentiation PAIR_fexp()
*/

/* prepare for multi-pairing */
void PAIR_ZZZ_initmp(FP48_YYY r[])
{
	int i;
	for (i=ATE_BITS_ZZZ-1; i>=0; i--)
		FP48_YYY_one(&r[i]);
	return;
}

// Store precomputed line details in an FP4
static void PAIR_ZZZ_pack(FP16_YYY *T,FP8_YYY* AA,FP8_YYY* BB,FP8_YYY *CC)
{
	FP8_YYY I,A,B;
	FP8_YYY_inv(&I,CC);
	FP8_YYY_mul(&A,AA,&I);
	FP8_YYY_mul(&B,BB,&I);
	FP16_YYY_from_FP8s(T,&A,&B);
}

// Unpack G2 line function details and include G1
static void PAIR_ZZZ_unpack(FP48_YYY *v,FP16_YYY* T,FP_YYY *Qx,FP_YYY *Qy)
{
    FP16_YYY a,b,c;
	FP8_YYY t;
	FP16_YYY_copy(&a,T);
	FP8_YYY_tmul(&a.a,&a.a,Qy);
	FP8_YYY_from_FP(&t,Qx);
	
#if SEXTIC_TWIST_ZZZ==D_TYPE
	FP16_YYY_from_FP8(&b,&t);		
	FP16_YYY_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
	FP16_YYY_zero(&b);
	FP16_YYY_from_FP8H(&c,&t);		
#endif

    FP48_YYY_from_FP16s(v,&a,&b,&c);
	v->type=FP_SPARSEST;
}

/* basic Miller loop */
void PAIR_ZZZ_miller(FP48_YYY *res,FP48_YYY r[])
{
	int i;
    FP48_YYY_one(res);
	for (i=ATE_BITS_ZZZ-1; i>=1; i--)
	{
		FP48_YYY_sqr(res,res);
		FP48_YYY_ssmul(res,&r[i]);
		FP48_YYY_zero(&r[i]);
	}

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP48_YYY_conj(res,res);
#endif
	FP48_YYY_ssmul(res,&r[0]);
	FP48_YYY_zero(&r[0]);
	return;
}

// Precompute table of line functions for fixed G2 value
void PAIR_ZZZ_precomp(FP16_YYY T[],ECP8_ZZZ* GV)
{
	int i,j,nb,bt;
	BIG_XXX n,n3;
	FP8_YYY AA,BB,CC;
	ECP8_ZZZ A,G,NG;

	ECP8_ZZZ_copy(&A,GV);
	ECP8_ZZZ_copy(&G,GV);
	ECP8_ZZZ_copy(&NG,GV);
	ECP8_ZZZ_neg(&NG);

	nb=PAIR_ZZZ_nbits(n3,n);
	j=0;

	for (i=nb-2; i>=1; i--)
	{
		PAIR_ZZZ_double(&A,&AA,&BB,&CC);
		PAIR_ZZZ_pack(&T[j++],&AA,&BB,&CC);

		bt=BIG_XXX_bit(n3,i)-BIG_XXX_bit(n,i); // bt=BIG_bit(n,i);
		if (bt==1)
		{
			PAIR_ZZZ_add(&A,&G,&AA,&BB,&CC);
			PAIR_ZZZ_pack(&T[j++],&AA,&BB,&CC);
		}
		if (bt==-1)
		{
			PAIR_ZZZ_add(&A,&NG,&AA,&BB,&CC);
			PAIR_ZZZ_pack(&T[j++],&AA,&BB,&CC);
		}
	}
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void PAIR_ZZZ_another_pc(FP48_YYY r[],FP16_YYY T[],ECP_ZZZ *QV)
{
    int i,j,nb,bt;
	BIG_XXX x,n,n3;
    FP48_YYY lv,lv2;
	ECP_ZZZ Q;
	FP_YYY Qx,Qy;

	nb=PAIR_ZZZ_nbits(n3,n);

	ECP_ZZZ_copy(&Q,QV);
	ECP_ZZZ_affine(&Q);

	FP_YYY_copy(&Qx,&(Q.x));
	FP_YYY_copy(&Qy,&(Q.y));

	j=0;
	for (i=nb-2; i>=1; i--)
	{
		PAIR_ZZZ_unpack(&lv,&T[j++],&Qx,&Qy);

		bt=BIG_XXX_bit(n3,i)-BIG_XXX_bit(n,i); // bt=BIG_bit(n,i);
		if (bt==1)
		{
			PAIR_ZZZ_unpack(&lv2,&T[j++],&Qx,&Qy);
			FP48_YYY_smul(&lv,&lv2);
		}
		if (bt==-1)
		{
			PAIR_ZZZ_unpack(&lv2,&T[j++],&Qx,&Qy);
			FP48_YYY_smul(&lv,&lv2);
		}
		FP48_YYY_ssmul(&r[i],&lv);
	}
}



/* Accumulate another set of line functions for n-pairing */
void PAIR_ZZZ_another(FP48_YYY r[],ECP8_ZZZ* PV,ECP_ZZZ* QV)
{
    int i,j,nb,bt;
	BIG_XXX x,n,n3;
    FP48_YYY lv,lv2;
    ECP8_ZZZ A,NP,P;
	ECP_ZZZ Q;
	FP_YYY Qx,Qy;

	nb=PAIR_ZZZ_nbits(n3,n);

	ECP8_ZZZ_copy(&P,PV);
	ECP_ZZZ_copy(&Q,QV);

	ECP8_ZZZ_affine(&P);
	ECP_ZZZ_affine(&Q);

	FP_YYY_copy(&Qx,&(Q.x));
	FP_YYY_copy(&Qy,&(Q.y));

	ECP8_ZZZ_copy(&A,&P);
	ECP8_ZZZ_copy(&NP,&P); ECP8_ZZZ_neg(&NP);

	for (i=nb-2; i>=1; i--)
	{
		PAIR_ZZZ_line(&lv,&A,&A,&Qx,&Qy);

		bt=BIG_XXX_bit(n3,i)-BIG_XXX_bit(n,i); // bt=BIG_bit(n,i);
		if (bt==1)
		{
			PAIR_ZZZ_line(&lv2,&A,&P,&Qx,&Qy);
			FP48_YYY_smul(&lv,&lv2);
		}
		if (bt==-1)
		{
			PAIR_ZZZ_line(&lv2,&A,&NP,&Qx,&Qy);
			FP48_YYY_smul(&lv,&lv2);
		}
		FP48_YYY_ssmul(&r[i],&lv);
	}
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_ZZZ_ate(FP48_YYY *r,ECP8_ZZZ *P1,ECP_ZZZ *Q1)
{
    BIG_XXX x,n,n3;
	FP_YYY Qx,Qy;
    int i,nb,bt;
    ECP8_ZZZ A,NP,P;
	ECP_ZZZ Q;
    FP48_YYY lv,lv2;

	nb=PAIR_ZZZ_nbits(n3,n);

	ECP8_ZZZ_copy(&P,P1);
	ECP_ZZZ_copy(&Q,Q1);

	ECP8_ZZZ_affine(&P);
	ECP_ZZZ_affine(&Q);


    FP_YYY_copy(&Qx,&(Q.x));
    FP_YYY_copy(&Qy,&(Q.y));

    ECP8_ZZZ_copy(&A,&P);
	ECP8_ZZZ_copy(&NP,&P); ECP8_ZZZ_neg(&NP);

    FP48_YYY_one(r);

    /* Main Miller Loop */
    for (i=nb-2; i>=1; i--)
    {
		FP48_YYY_sqr(r,r);
        PAIR_ZZZ_line(&lv,&A,&A,&Qx,&Qy);

		bt= BIG_XXX_bit(n3,i)-BIG_XXX_bit(n,i);  // BIG_bit(n,i); 
        if (bt==1)
        {
            PAIR_ZZZ_line(&lv2,&A,&P,&Qx,&Qy);
            FP48_YYY_smul(&lv,&lv2);
        }
		if (bt==-1)
		{
            PAIR_ZZZ_line(&lv2,&A,&NP,&Qx,&Qy);
            FP48_YYY_smul(&lv,&lv2);
		}
        FP48_YYY_ssmul(r,&lv);
    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP48_YYY_conj(r,r);
#endif

}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_ZZZ_double_ate(FP48_YYY *r,ECP8_ZZZ *P1,ECP_ZZZ *Q1,ECP8_ZZZ *R1,ECP_ZZZ *S1)
{
    BIG_XXX x,n,n3;
	FP_YYY Qx,Qy,Sx,Sy;
    int i,nb,bt;
    ECP8_ZZZ A,B,NP,NR,P,R;
	ECP_ZZZ Q,S;
    FP48_YYY lv,lv2;

	nb=PAIR_ZZZ_nbits(n3,n);

	ECP8_ZZZ_copy(&P,P1);
	ECP_ZZZ_copy(&Q,Q1);

	ECP8_ZZZ_affine(&P);
	ECP_ZZZ_affine(&Q);

	ECP8_ZZZ_copy(&R,R1);
	ECP_ZZZ_copy(&S,S1);

	ECP8_ZZZ_affine(&R);
	ECP_ZZZ_affine(&S);

    FP_YYY_copy(&Qx,&(Q.x));
    FP_YYY_copy(&Qy,&(Q.y));

    FP_YYY_copy(&Sx,&(S.x));
    FP_YYY_copy(&Sy,&(S.y));

    ECP8_ZZZ_copy(&A,&P);
    ECP8_ZZZ_copy(&B,&R);
	ECP8_ZZZ_copy(&NP,&P); ECP8_ZZZ_neg(&NP);
	ECP8_ZZZ_copy(&NR,&R); ECP8_ZZZ_neg(&NR);

    FP48_YYY_one(r);

    /* Main Miller Loop */
    for (i=nb-2; i>=1; i--)
    {
        FP48_YYY_sqr(r,r);
        PAIR_ZZZ_line(&lv,&A,&A,&Qx,&Qy);
		PAIR_ZZZ_line(&lv2,&B,&B,&Sx,&Sy);
        FP48_YYY_smul(&lv,&lv2);
        FP48_YYY_ssmul(r,&lv);

		bt=BIG_XXX_bit(n3,i)-BIG_XXX_bit(n,i); // bt=BIG_bit(n,i);
        if (bt==1)
        {
            PAIR_ZZZ_line(&lv,&A,&P,&Qx,&Qy);
            PAIR_ZZZ_line(&lv2,&B,&R,&Sx,&Sy);
			FP48_YYY_smul(&lv,&lv2);
            FP48_YYY_ssmul(r,&lv);
        }
		if (bt==-1)
		{
            PAIR_ZZZ_line(&lv,&A,&NP,&Qx,&Qy);
            PAIR_ZZZ_line(&lv2,&B,&NR,&Sx,&Sy);
            FP48_YYY_smul(&lv,&lv2);
            FP48_YYY_ssmul(r,&lv);
		}
	}

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP48_YYY_conj(r,r);
#endif

}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */

void PAIR_ZZZ_fexp(FP48_YYY *r)
{
    FP2_YYY X;
    BIG_XXX x;
	FP_YYY a,b;
    FP48_YYY t1,t2,t3,t7;  

    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);
    FP_YYY_rcopy(&a,Fra_YYY);
    FP_YYY_rcopy(&b,Frb_YYY);
    FP2_YYY_from_FPs(&X,&a,&b);

    /* Easy part of final exp - r^(p^24-1)(p^8+1)*/

    FP48_YYY_inv(&t7,r);
    FP48_YYY_conj(r,r);

    FP48_YYY_mul(r,&t7);
    FP48_YYY_copy(&t7,r);

    FP48_YYY_frob(r,&X,8);

    FP48_YYY_mul(r,&t7);

	if (FP48_YYY_isunity(r))
	{
		FP48_YYY_zero(r);
		return;
	}

// f^e0.f^e1^p.f^e2^p^2.. .. f^e14^p^14.f^e15^p^15

	FP48_YYY_usqr(&t7,r);			// t7=f^2

	if (BIG_XXX_parity(x)==1)
	{
		FP48_YYY_pow(&t2,r,x);		// f^u
		FP48_YYY_usqr(&t1,&t2);		// f^(2u)
		FP48_YYY_pow(&t2,&t2,x);	// f^u^2
	} else {
		FP48_YYY_pow(&t1,&t7,x);		// t1=f^2u
		BIG_XXX_fshr(x,1);
		FP48_YYY_pow(&t2,&t1,x);		// t2=f^2u^(u/2) =  f^u^2
		BIG_XXX_fshl(x,1);				// x must be even
	}

#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif

	FP48_YYY_conj(&t3,&t1);		// t3=f^-2u
	FP48_YYY_mul(&t2,&t3);		// t2=f^u^2.f^-2u
	FP48_YYY_mul(&t2,r);		// t2=f^u^2.f^-2u.f = f^(u^2-2u+1) = f^e15

	FP48_YYY_mul(r,&t7);		// f^3

	FP48_YYY_pow(&t1,&t2,x);	// f^e15^u = f^(u.e15) = f^(u^3-2u^2+u) = f^(e14)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,14);	// f^(u^3-2u^2+u)^p^14
	FP48_YYY_mul(r,&t3);		// f^3.f^(u^3-2u^2+u)^p^14

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e14) = f^(u^4-2u^3+u^2) =  f^(e13)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,13);	// f^(e13)^p^13
	FP48_YYY_mul(r,&t3);		// f^3.f^(u^3-2u^2+u)^p^14.f^(u^4-2u^3+u^2)^p^13

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e13)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,12);	// f^(e12)^p^12
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e12)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,11);	// f^(e11)^p^11
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e11)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,10);	// f^(e10)^p^10
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e10)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,9);	// f^(e9)^p^9
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e9)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,8);	// f^(e8)^p^8
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e8)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_conj(&t3,&t2);
	FP48_YYY_mul(&t1,&t3);  // f^(u.e8).f^-e15
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,7);	// f^(e7)^p^7
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e7)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,6);	// f^(e6)^p^6
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e6)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,5);	// f^(e5)^p^5
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e5)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,4);	// f^(e4)^p^4
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e4)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,3);	// f^(e3)^p^3
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e3)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,2);	// f^(e2)^p^2
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e2)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_copy(&t3,&t1);
	FP48_YYY_frob(&t3,&X,1);	// f^(e1)^p^1
	FP48_YYY_mul(r,&t3);		

	FP48_YYY_pow(&t1,&t1,x);	// f^(u.e1)
#if SIGN_OF_X_ZZZ==NEGATIVEX
	FP48_YYY_conj(&t1,&t1);
#endif
	FP48_YYY_mul(r,&t1);		// r.f^e0		

	FP48_YYY_frob(&t2,&X,15);	// f^(e15.p^15)
	FP48_YYY_mul(r,&t2);


	FP48_YYY_reduce(r);

}

#ifdef USE_GLV_ZZZ
/* GLV method */
static void glv(BIG_XXX u[2],BIG_XXX e)
{

// -(x^8).P = (Beta.x,y)

    BIG_XXX x,x2,q;
    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);
    
	BIG_XXX_smul(x2,x,x);
	BIG_XXX_smul(x,x2,x2);
	BIG_XXX_smul(x2,x,x);

    BIG_XXX_copy(u[0],e);
    BIG_XXX_mod(u[0],x2);
    BIG_XXX_copy(u[1],e);
    BIG_XXX_sdiv(u[1],x2);

    BIG_XXX_rcopy(q,CURVE_Order_ZZZ);
    BIG_XXX_sub(u[1],q,u[1]);


    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void gs(BIG_XXX u[16],BIG_XXX e)
{
    int i;

    BIG_XXX x,w,q;
	BIG_XXX_rcopy(q,CURVE_Order_ZZZ);
    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);
    BIG_XXX_copy(w,e);

    for (i=0; i<15; i++)
    {
        BIG_XXX_copy(u[i],w);
        BIG_XXX_mod(u[i],x);
        BIG_XXX_sdiv(w,x);
    }
	BIG_XXX_copy(u[15],w);

/*  */
#if SIGN_OF_X_ZZZ==NEGATIVEX
	BIG_XXX_modneg(u[1],u[1],q);
	BIG_XXX_modneg(u[3],u[3],q);
	BIG_XXX_modneg(u[5],u[5],q);
	BIG_XXX_modneg(u[7],u[7],q);
	BIG_XXX_modneg(u[9],u[9],q);
	BIG_XXX_modneg(u[11],u[11],q);
	BIG_XXX_modneg(u[13],u[13],q);
	BIG_XXX_modneg(u[15],u[15],q);
#endif


    return;
}

/* Multiply P by e in group G1 */
void PAIR_ZZZ_G1mul(ECP_ZZZ *P,BIG_XXX e)
{
#ifdef USE_GLV_ZZZ   /* Note this method is patented */
    int np,nn;
    ECP_ZZZ Q;
	FP_YYY cru;
    BIG_XXX t,q;
    BIG_XXX u[2];

    BIG_XXX_rcopy(q,CURVE_Order_ZZZ);
    glv(u,e);

    ECP_ZZZ_copy(&Q,P); ECP_ZZZ_affine(&Q);
    FP_YYY_rcopy(&cru,CURVE_Cru_ZZZ);
    FP_YYY_mul(&(Q.x),&(Q.x),&cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np=BIG_XXX_nbits(u[0]);
    BIG_XXX_modneg(t,u[0],q);
    nn=BIG_XXX_nbits(t);
    if (nn<np)
    {
        BIG_XXX_copy(u[0],t);
        ECP_ZZZ_neg(P);
    }

    np=BIG_XXX_nbits(u[1]);
    BIG_XXX_modneg(t,u[1],q);
    nn=BIG_XXX_nbits(t);
    if (nn<np)
    {
        BIG_XXX_copy(u[1],t);
        ECP_ZZZ_neg(&Q);
    }
    BIG_XXX_norm(u[0]);
    BIG_XXX_norm(u[1]);    
    ECP_ZZZ_mul2(P,&Q,u[0],u[1]);

#else
    ECP_ZZZ_mul(P,e);
#endif
}

/* Multiply P by e in group G2 */
void PAIR_ZZZ_G2mul(ECP8_ZZZ *P,BIG_XXX e)
{
#ifdef USE_GS_G2_ZZZ   /* Well I didn't patent it :) */
    int i,np,nn;
    ECP8_ZZZ Q[16];
    FP2_YYY X[3];
    BIG_XXX x,y,u[16];

	ECP8_ZZZ_frob_constants(X);

    BIG_XXX_rcopy(y,CURVE_Order_ZZZ);
    gs(u,e);

    ECP8_ZZZ_copy(&Q[0],P);
    for (i=1; i<16; i++)
    {
        ECP8_ZZZ_copy(&Q[i],&Q[i-1]);
        ECP8_ZZZ_frob(&Q[i],X,1);
    }

    for (i=0; i<16; i++)
    {
        np=BIG_XXX_nbits(u[i]);
        BIG_XXX_modneg(x,u[i],y);
        nn=BIG_XXX_nbits(x);
        if (nn<np)
        {
            BIG_XXX_copy(u[i],x);
            ECP8_ZZZ_neg(&Q[i]);
        }
        BIG_XXX_norm(u[i]);  
    }

    ECP8_ZZZ_mul16(P,Q,u);

#else
    ECP8_ZZZ_mul(P,e);
#endif
}

/* f=f^e */
void PAIR_ZZZ_GTpow(FP48_YYY *f,BIG_XXX e)
{
#ifdef USE_GS_GT_ZZZ   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see FP16.cpp */
    int i,np,nn;
    FP48_YYY g[16];
    FP2_YYY X;
    BIG_XXX t,q;
	FP_YYY fx,fy;
    BIG_XXX u[16];

    FP_YYY_rcopy(&fx,Fra_YYY);
    FP_YYY_rcopy(&fy,Frb_YYY);
    FP2_YYY_from_FPs(&X,&fx,&fy);

    BIG_XXX_rcopy(q,CURVE_Order_ZZZ);
    gs(u,e);

    FP48_YYY_copy(&g[0],f);
    for (i=1; i<16; i++)
    {
        FP48_YYY_copy(&g[i],&g[i-1]);
        FP48_YYY_frob(&g[i],&X,1);
    }

    for (i=0; i<16; i++)
    {
        np=BIG_XXX_nbits(u[i]);
        BIG_XXX_modneg(t,u[i],q);
        nn=BIG_XXX_nbits(t);
        if (nn<np)
        {
            BIG_XXX_copy(u[i],t);
            FP48_YYY_conj(&g[i],&g[i]);
        }
        BIG_XXX_norm(u[i]);
    }
    FP48_YYY_pow16(f,g,u);

#else
    FP48_YYY_pow(f,f,e);
#endif
}

#ifdef HAS_MAIN

using namespace std;
using namespace ZZZ;


// g++ -O2 pair256_BLS48.cpp ecp8_BLS48.cpp fp48_BLS48.cpp fp16_BLS48.cpp fp8_BLS48.cpp fp4_BLS48.cpp fp2_BLS48.cpp ecp_BLS48.cpp fp_BLS48.cpp big_B560_29.cpp rom_curve_BLS48.cpp rom_field_BLS48.cpp rand.cpp hash.cpp oct.cpp -o pair256_BLS48.exe

int main()
{
    int i;
    char byt[32];
    csprng rng;
    BIG xa,xb,ya,yb,w,a,b,t1,q,u[2],v[4],m,r,xx,x2,x4,p;
    ECP8 P,G;
    ECP Q,R;
    FP48 g,gp;
    FP16 t,c,cp,cpm1,cpm2;
	FP8 X,Y;
    FP2 x,y,f,Aa,Bb;
	FP cru;

	for (i=0;i<32;i++)
		byt[i]=i+9;
	RAND_seed(&rng,32,byt);

	BIG_rcopy(r,CURVE_Order);
	BIG_rcopy(p,Modulus);


    BIG_rcopy(xa,CURVE_Gx_ZZZ);
    BIG_rcopy(ya,CURVE_Gy_ZZZ);

    ECP_ZZZ_set(&Q,xa,ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_ZZZ_output(&Q);
    printf("\n");

	ECP8_generator(&P);

    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    BIG_rcopy(a,Fra);
    BIG_rcopy(b,Frb);
    FP2_from_BIGs(&f,a,b);

    PAIR_ZZZ_ate(&g,&P,&Q);

	printf("gb= ");
    FP48_output(&g);
    printf("\n");
    PAIR_ZZZ_fexp(&g);

    printf("g= ");
    FP48_output(&g);
    printf("\n");

	ECP_ZZZ_copy(&R,&Q);
	ECP8_copy(&G,&P);

	ECP8_dbl(&G);
	ECP_dbl(&R);
	ECP_affine(&R);

    PAIR_ZZZ_ate(&g,&G,&Q);
    PAIR_ZZZ_fexp(&g);

    printf("g1= ");
    FP48_output(&g);
    printf("\n");

    PAIR_ZZZ_ate(&g,&P,&R);
    PAIR_ZZZ_fexp(&g);

    printf("g2= ");
    FP48_output(&g);
    printf("\n");


	PAIR_ZZZ_G1mul(&Q,r);
	printf("rQ= ");ECP_output(&Q); printf("\n");

	PAIR_ZZZ_G2mul(&P,r);
	printf("rP= ");ECP8_output(&P); printf("\n");

	PAIR_ZZZ_GTpow(&g,r);
	printf("g^r= ");FP48_output(&g); printf("\n");


	BIG_randomnum(w,r,&rng);

	FP48_copy(&gp,&g);

	PAIR_ZZZ_GTpow(&g,w);

	FP48_trace(&t,&g);

	printf("g^r=  ");FP16_output(&t); printf("\n");

	FP48_compow(&t,&gp,w,r);

	printf("t(g)= "); FP16_output(&t); printf("\n");

}

#endif
