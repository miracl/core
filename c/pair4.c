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

/* CORE BLS Curve pairing functions */

//#define HAS_MAIN

#include "pair4_ZZZ.h"

// Point doubling for pairings
static void PAIR_ZZZ_double(ECP4_ZZZ *A, FP4_YYY *AA, FP4_YYY *BB, FP4_YYY *CC)
{
    FP4_YYY YY;
    FP4_YYY_copy(CC, &(A->x));  //FP4 XX=new FP4(A.getx());  //X
    FP4_YYY_copy(&YY, &(A->y)); //FP4 YY=new FP4(A.gety());  //Y
    FP4_YYY_copy(BB, &(A->z));  //FP4 ZZ=new FP4(A.getz());  //Z

    FP4_YYY_copy(AA, &YY);      //FP4 YZ=new FP4(YY);        //Y
    FP4_YYY_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP4_YYY_sqr(CC, CC);        //XX.sqr();                //X^2
    FP4_YYY_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP4_YYY_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP4_YYY_add(AA, AA, AA);
    FP4_YYY_neg(AA, AA);
    FP4_YYY_norm(AA);           // -2YZ
    FP4_YYY_times_i(AA);        // -2YZi

    FP4_YYY_imul(BB, BB, 3 * CURVE_B_I_ZZZ); //3Bz^2
    FP4_YYY_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_YYY_times_i(&YY);                   // Y^2.i
    FP4_YYY_times_i(CC);                    // 3X^2.i
#endif

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_YYY_times_i(BB);                    // 3Bz^2.i
#endif

    FP4_YYY_sub(BB, BB, &YY);
    FP4_YYY_norm(BB);

    ECP4_ZZZ_dbl(A);            //A.dbl();
}

// Point addition for pairings
static void PAIR_ZZZ_add(ECP4_ZZZ *A, ECP4_ZZZ *B, FP4_YYY *AA, FP4_YYY *BB, FP4_YYY *CC)
{
    FP4_YYY T1;
    FP4_YYY_copy(AA, &(A->x));      //FP4 X1=new FP4(A.getx());    // X1
    FP4_YYY_copy(CC, &(A->y));      //FP4 Y1=new FP4(A.gety());    // Y1
    FP4_YYY_copy(&T1, &(A->z));     //FP4 T1=new FP4(A.getz());    // Z1

    FP4_YYY_copy(BB, &T1);          //FP4 T2=new FP4(A.getz());    // Z1

    FP4_YYY_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP4_YYY_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP4_YYY_sub(AA, AA, BB);        //X1.sub(T2);
    FP4_YYY_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP4_YYY_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP4_YYY_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP4_YYY_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_YYY_times_i(AA);
    FP4_YYY_norm(AA);
#endif

    FP4_YYY_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP4_YYY_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP4_YYY_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP4_YYY_sub(BB, BB, &T1);       //T2.sub(T1);
    FP4_YYY_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP4_YYY_neg(CC, CC);            //Y1.neg();
    FP4_YYY_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP4_ZZZ_add(A, B);             //A.add(B);
}

/* Line function */
static void PAIR_ZZZ_line(FP24_YYY *v, ECP4_ZZZ *A, ECP4_ZZZ *B, FP_YYY *Qx, FP_YYY *Qy)
{
    FP4_YYY AA, BB, CC;
    FP8_YYY a, b, c;

    if (A == B)
        PAIR_ZZZ_double(A, &AA, &BB, &CC);
    else
        PAIR_ZZZ_add(A, B, &AA, &BB, &CC);

    FP4_YYY_qmul(&CC, &CC, Qx);
    FP4_YYY_qmul(&AA, &AA, Qy);

    FP8_YYY_from_FP4s(&a, &AA, &BB);
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_YYY_from_FP4(&b, &CC);
    FP8_YYY_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP8_YYY_zero(&b);
    FP8_YYY_from_FP4H(&c, &CC);
#endif

    FP24_YYY_from_FP8s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}


/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int PAIR_ZZZ_nbits(BIG_XXX n3, BIG_XXX n)
{
    BIG_XXX x;
    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

    BIG_XXX_copy(n, x);
    BIG_XXX_norm(n);
    BIG_XXX_pmul(n3, n, 3);
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
void PAIR_ZZZ_initmp(FP24_YYY r[])
{
    int i;
    for (i = ATE_BITS_ZZZ - 1; i >= 0; i--)
        FP24_YYY_one(&r[i]);
    return;
}

/* basic Miller loop */
void PAIR_ZZZ_miller(FP24_YYY *res, FP24_YYY r[])
{
    int i;
    FP24_YYY_one(res);
    for (i = ATE_BITS_ZZZ - 1; i >= 1; i--)
    {
        FP24_YYY_sqr(res, res);
        FP24_YYY_ssmul(res, &r[i]);
        FP24_YYY_zero(&r[i]);
    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(res, res);
#endif
    FP24_YYY_ssmul(res, &r[0]);
    FP24_YYY_zero(&r[0]);
    return;
}

// Store precomputed line details in an FP4
static void PAIR_ZZZ_pack(FP8_YYY *T, FP4_YYY* AA, FP4_YYY* BB, FP4_YYY *CC)
{
    FP4_YYY I, A, B;
    FP4_YYY_inv(&I, CC, NULL);
    FP4_YYY_mul(&A, AA, &I);
    FP4_YYY_mul(&B, BB, &I);
    FP8_YYY_from_FP4s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void PAIR_ZZZ_unpack(FP24_YYY *v, FP8_YYY* T, FP_YYY *Qx, FP_YYY *Qy)
{
    FP8_YYY a, b, c;
    FP4_YYY t;
    FP8_YYY_copy(&a, T);
    FP4_YYY_qmul(&a.a, &a.a, Qy);
    FP4_YYY_from_FP(&t, Qx);

#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_YYY_from_FP4(&b, &t);
    FP8_YYY_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP8_YYY_zero(&b);
    FP8_YYY_from_FP4H(&c, &t);
#endif

    FP24_YYY_from_FP8s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}

// Precompute table of line functions for fixed G2 value
void PAIR_ZZZ_precomp(FP8_YYY T[], ECP4_ZZZ* GV)
{
    int i, j, nb, bt;
    BIG_XXX n, n3;
    FP4_YYY AA, BB, CC;
    ECP4_ZZZ A, G, NG;

    ECP4_ZZZ_copy(&A, GV);
    ECP4_ZZZ_copy(&G, GV);
    ECP4_ZZZ_copy(&NG, GV);
    ECP4_ZZZ_neg(&NG);

    nb = PAIR_ZZZ_nbits(n3, n);
    j = 0;

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_ZZZ_double(&A, &AA, &BB, &CC);
        PAIR_ZZZ_pack(&T[j++], &AA, &BB, &CC);

        bt = BIG_XXX_bit(n3, i) - BIG_XXX_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_ZZZ_add(&A, &G, &AA, &BB, &CC);
            PAIR_ZZZ_pack(&T[j++], &AA, &BB, &CC);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_add(&A, &NG, &AA, &BB, &CC);
            PAIR_ZZZ_pack(&T[j++], &AA, &BB, &CC);
        }
    }
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void PAIR_ZZZ_another_pc(FP24_YYY r[], FP8_YYY T[], ECP_ZZZ *QV)
{
    int i, j, nb, bt;
    BIG_XXX x, n, n3;
    FP24_YYY lv, lv2;
    ECP_ZZZ Q;
    FP_YYY Qx, Qy;

    if (ECP_ZZZ_isinf(QV)) return;

    nb = PAIR_ZZZ_nbits(n3, n);

    ECP_ZZZ_copy(&Q, QV);
    ECP_ZZZ_affine(&Q);

    FP_YYY_copy(&Qx, &(Q.x));
    FP_YYY_copy(&Qy, &(Q.y));

    j = 0;
    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_ZZZ_unpack(&lv, &T[j++], &Qx, &Qy);

        bt = BIG_XXX_bit(n3, i) - BIG_XXX_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_ZZZ_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP24_YYY_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP24_YYY_smul(&lv, &lv2);
        }
        FP24_YYY_ssmul(&r[i], &lv);
    }
}

/* Accumulate another set of line functions for n-pairing */
void PAIR_ZZZ_another(FP24_YYY r[], ECP4_ZZZ* PV, ECP_ZZZ* QV)
{
    int i, j, nb, bt;
    BIG_XXX x, n, n3;
    FP24_YYY lv, lv2;
    ECP4_ZZZ A, NP, P;
    ECP_ZZZ Q;
    FP_YYY Qx, Qy;

    if (ECP_ZZZ_isinf(QV)) return;

    nb = PAIR_ZZZ_nbits(n3, n);

    ECP4_ZZZ_copy(&P, PV);
    ECP_ZZZ_copy(&Q, QV);

    ECP4_ZZZ_affine(&P);
    ECP_ZZZ_affine(&Q);

    FP_YYY_copy(&Qx, &(Q.x));
    FP_YYY_copy(&Qy, &(Q.y));

    ECP4_ZZZ_copy(&A, &P);
    ECP4_ZZZ_copy(&NP, &P); ECP4_ZZZ_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_ZZZ_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_XXX_bit(n3, i) - BIG_XXX_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_ZZZ_line(&lv2, &A, &P, &Qx, &Qy);
            FP24_YYY_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_line(&lv2, &A, &NP, &Qx, &Qy);
            FP24_YYY_smul(&lv, &lv2);
        }
        FP24_YYY_ssmul(&r[i], &lv);
    }
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_ZZZ_ate(FP24_YYY *r, ECP4_ZZZ *P1, ECP_ZZZ *Q1)
{
    BIG_XXX x, n, n3;
    FP_YYY Qx, Qy;
    int i, nb, bt;
    ECP4_ZZZ A, NP, P;
    ECP_ZZZ Q;
    FP24_YYY lv, lv2;

    FP24_YYY_one(r);

    if (ECP_ZZZ_isinf(Q1)) return;

    nb = PAIR_ZZZ_nbits(n3, n);

    ECP4_ZZZ_copy(&P, P1);
    ECP_ZZZ_copy(&Q, Q1);

    ECP4_ZZZ_affine(&P);
    ECP_ZZZ_affine(&Q);


    FP_YYY_copy(&Qx, &(Q.x));
    FP_YYY_copy(&Qy, &(Q.y));

    ECP4_ZZZ_copy(&A, &P);
    ECP4_ZZZ_copy(&NP, &P); ECP4_ZZZ_neg(&NP);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP24_YYY_sqr(r, r);
        PAIR_ZZZ_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_XXX_bit(n3, i) - BIG_XXX_bit(n, i); // BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_ZZZ_line(&lv2, &A, &P, &Qx, &Qy);
            FP24_YYY_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_line(&lv2, &A, &NP, &Qx, &Qy);
            FP24_YYY_smul(&lv, &lv2);
        }
        FP24_YYY_ssmul(r, &lv);
    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(r, r);
#endif

}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_ZZZ_double_ate(FP24_YYY *r, ECP4_ZZZ *P1, ECP_ZZZ *Q1, ECP4_ZZZ *R1, ECP_ZZZ *S1)
{
    BIG_XXX x, n, n3;
    FP_YYY Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP4_ZZZ A, B, NP, NR, P, R;
    ECP_ZZZ Q, S;
    FP24_YYY lv, lv2;

    if (ECP_ZZZ_isinf(Q1))
    {
        PAIR_ZZZ_ate(r, R1, S1);
        return;
    }
    if (ECP_ZZZ_isinf(S1))
    {
        PAIR_ZZZ_ate(r, P1, Q1);
        return;
    }
    nb = PAIR_ZZZ_nbits(n3, n);

    ECP4_ZZZ_copy(&P, P1);
    ECP_ZZZ_copy(&Q, Q1);

    ECP4_ZZZ_affine(&P);
    ECP_ZZZ_affine(&Q);

    ECP4_ZZZ_copy(&R, R1);
    ECP_ZZZ_copy(&S, S1);

    ECP4_ZZZ_affine(&R);
    ECP_ZZZ_affine(&S);

    FP_YYY_copy(&Qx, &(Q.x));
    FP_YYY_copy(&Qy, &(Q.y));

    FP_YYY_copy(&Sx, &(S.x));
    FP_YYY_copy(&Sy, &(S.y));

    ECP4_ZZZ_copy(&A, &P);
    ECP4_ZZZ_copy(&B, &R);
    ECP4_ZZZ_copy(&NP, &P); ECP4_ZZZ_neg(&NP);
    ECP4_ZZZ_copy(&NR, &R); ECP4_ZZZ_neg(&NR);

    FP24_YYY_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP24_YYY_sqr(r, r);
        PAIR_ZZZ_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_ZZZ_line(&lv2, &B, &B, &Sx, &Sy);
        FP24_YYY_smul(&lv, &lv2);
        FP24_YYY_ssmul(r, &lv);

        bt = BIG_XXX_bit(n3, i) - BIG_XXX_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_ZZZ_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_ZZZ_line(&lv2, &B, &R, &Sx, &Sy);
            FP24_YYY_smul(&lv, &lv2);
            FP24_YYY_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_ZZZ_line(&lv2, &B, &NR, &Sx, &Sy);
            FP24_YYY_smul(&lv, &lv2);
            FP24_YYY_ssmul(r, &lv);
        }
    }



#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(r, r);
#endif

}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */

void PAIR_ZZZ_fexp(FP24_YYY *r)
{
    FP2_YYY X;
    BIG_XXX x;
    FP_YYY a, b;
    FP24_YYY t0, t1, t2; //, t3, t4, t5, t6, t7; // could lose one of these - r=t3

    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);
    FP_YYY_rcopy(&a, Fra_YYY);
    FP_YYY_rcopy(&b, Frb_YYY);
    FP2_YYY_from_FPs(&X, &a, &b);

    /* Easy part of final exp - r^(p^12-1)(p^4+1)*/

    FP24_YYY_inv(&t0, r);
    FP24_YYY_conj(r, r);

    FP24_YYY_mul(r, &t0);
    FP24_YYY_copy(&t0, r);

    FP24_YYY_frob(r, &X, 4);

    FP24_YYY_mul(r, &t0);

// See https://eprint.iacr.org/2020/875.pdf
    FP24_YYY_usqr(&t2,r);
    FP24_YYY_mul(&t2,r);     // t2=r^3

    FP24_YYY_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t1, &t1);
#endif
    FP24_YYY_conj(&t0,r);    // t0=r^-1
    FP24_YYY_copy(r,&t1);
    FP24_YYY_mul(r,&t0);    // r=r^(x-1)

    FP24_YYY_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t1, &t1);
#endif
    FP24_YYY_conj(&t0,r);    // t0=r^-1
    FP24_YYY_copy(r,&t1);
    FP24_YYY_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP24_YYY_pow(&t1,r,x);  // t1=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t1, &t1);
#endif
    FP24_YYY_copy(&t0,r);   
    FP24_YYY_frob(&t0,&X,1); // t0=r^p
    FP24_YYY_copy(r,&t1);
    FP24_YYY_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2)
    FP24_YYY_pow(&t1,r,x);  
    FP24_YYY_pow(&t1,&t1,x); // t1=r^x^2
    FP24_YYY_copy(&t0,r);    
    FP24_YYY_frob(&t0,&X,2);   // t0=r^p^2
    FP24_YYY_mul(&t1,&t0);   // t1=r^x^2.r^p^2
    FP24_YYY_copy(r,&t1);

// ^(x^4+p^4-1)
    FP24_YYY_pow(&t1,r,x);  
    FP24_YYY_pow(&t1,&t1,x); 
    FP24_YYY_pow(&t1,&t1,x); 
    FP24_YYY_pow(&t1,&t1,x); // t1=r^x^4
    FP24_YYY_copy(&t0,r);    
    FP24_YYY_frob(&t0,&X,4); // t0=r^p^4
    FP24_YYY_mul(&t1,&t0);   // t1=r^x^4.r^p^4
    FP24_YYY_conj(&t0,r);    // t0=r^-1
    FP24_YYY_copy(r,&t1);    
    FP24_YYY_mul(r,&t0);     // r=r^x^4.r^p^4.r^-1

    FP24_YYY_mul(r,&t2);    
    FP24_YYY_reduce(r);

/*
// Ghamman & Fouotsa Method - (completely garbled in  https://eprint.iacr.org/2016/130)

    FP24_YYY_usqr(&t7, r);          // t7=f^2
    FP24_YYY_pow(&t1, &t7, x);      // t1=t7^u

    BIG_XXX_fshr(x, 1);
    FP24_YYY_pow(&t2, &t1, x);      // t2=t1^(u/2)
    BIG_XXX_fshl(x, 1); // x must be even

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t1, &t1);
#endif

    FP24_YYY_conj(&t3, &t1);    // t3=1/t1
    FP24_YYY_mul(&t2, &t3);     // t2=t1*t3
    FP24_YYY_mul(&t2, r);       // t2=t2*f


    FP24_YYY_pow(&t3, &t2, x);      // t3=t2^u
    FP24_YYY_pow(&t4, &t3, x);      // t4=t3^u
    FP24_YYY_pow(&t5, &t4, x);      // t5=t4^u

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t3, &t3);
    FP24_YYY_conj(&t5, &t5);
#endif

    FP24_YYY_frob(&t3, &X, 6);
    FP24_YYY_frob(&t4, &X, 5);

    FP24_YYY_mul(&t3, &t4);     // t3=t3.t4


    FP24_YYY_pow(&t6, &t5, x);      // t6=t5^u
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t6, &t6);
#endif


    FP24_YYY_frob(&t5, &X, 4);
    FP24_YYY_mul(&t3, &t5); // ??


    FP24_YYY_conj(&t0, &t2);        // t0=1/t2
    FP24_YYY_mul(&t6, &t0);     // t6=t6*t0

    FP24_YYY_copy(&t5, &t6);
    FP24_YYY_frob(&t5, &X, 3);

    FP24_YYY_mul(&t3, &t5);     // t3=t3*t5
    FP24_YYY_pow(&t5, &t6, x);  // t5=t6^x
    FP24_YYY_pow(&t6, &t5, x);  // t6=t5^x

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t5, &t5);
#endif

    FP24_YYY_copy(&t0, &t5);
    FP24_YYY_frob(&t0, &X, 2);
    FP24_YYY_mul(&t3, &t0);     // t3=t3*t0
    FP24_YYY_copy(&t0, &t6);    //
    FP24_YYY_frob(&t0, &X, 1);

    FP24_YYY_mul(&t3, &t0);     // t3=t3*t0
    FP24_YYY_pow(&t5, &t6, x);  // t5=t6*x

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t5, &t5);
#endif

    FP24_YYY_frob(&t2, &X, 7);

    FP24_YYY_mul(&t5, &t7);     // t5=t5*t7
    FP24_YYY_mul(&t3, &t2);     // t3=t3*t2
    FP24_YYY_mul(&t3, &t5);     // t3=t3*t5

    FP24_YYY_mul(r, &t3);
    FP24_YYY_reduce(r);
*/
}

#ifdef USE_GLV_ZZZ
/* GLV method */
static void glv(BIG_XXX u[2], BIG_XXX ee)
{
    int bd;
    BIG_XXX q,x,x2;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);

// -(x^4).P = (Beta.x,y)

    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

    BIG_XXX_smul(x2, x, x);
    BIG_XXX_smul(x, x2, x2);
    bd=BIG_XXX_nbits(q)-BIG_XXX_nbits(x); // fixed x^4

    BIG_XXX_copy(u[0], ee);
    BIG_XXX_ctmod(u[0], x, bd);
    BIG_XXX_copy(u[1], ee);
    BIG_XXX_ctsdiv(u[1], x, bd);
    BIG_XXX_sub(u[1], q, u[1]);

    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void gs(BIG_XXX u[8], BIG_XXX ee)
{
    int i,bd;
    BIG_XXX q,x,w;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);

    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);
    BIG_XXX_copy(w, ee);
    bd=BIG_XXX_nbits(q)-BIG_XXX_nbits(x); // fixed

    for (i = 0; i < 7; i++)
    {
        BIG_XXX_copy(u[i], w);
        BIG_XXX_ctmod(u[i], x, bd);
        BIG_XXX_ctsdiv(w, x, bd);
    }
    BIG_XXX_copy(u[7], w);

    /*  */
#if SIGN_OF_X_ZZZ==NEGATIVEX
    BIG_XXX_modneg(u[1], u[1], q);
    BIG_XXX_modneg(u[3], u[3], q);
    BIG_XXX_modneg(u[5], u[5], q);
    BIG_XXX_modneg(u[7], u[7], q);
#endif
    return;
}

/* Multiply P by e in group G1 */
void PAIR_ZZZ_G1mul(ECP_ZZZ *P, BIG_XXX e)
{
    BIG_XXX ee,q;
#ifdef USE_GLV_ZZZ   /* Note this method is patented */
    int np, nn;
    ECP_ZZZ Q;
    FP_YYY cru;
    BIG_XXX t;
    BIG_XXX u[2];
    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);

    glv(u, ee);

    ECP_ZZZ_copy(&Q, P); ECP_ZZZ_affine(&Q);
    FP_YYY_rcopy(&cru, CRu_YYY);
    FP_YYY_mul(&(Q.x), &(Q.x), &cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np = BIG_XXX_nbits(u[0]);
    BIG_XXX_modneg(t, u[0], q);
    nn = BIG_XXX_nbits(t);
    if (nn < np)
    {
        BIG_XXX_copy(u[0], t);
        ECP_ZZZ_neg(P);
    }

    np = BIG_XXX_nbits(u[1]);
    BIG_XXX_modneg(t, u[1], q);
    nn = BIG_XXX_nbits(t);
    if (nn < np)
    {
        BIG_XXX_copy(u[1], t);
        ECP_ZZZ_neg(&Q);
    }
    BIG_XXX_norm(u[0]);
    BIG_XXX_norm(u[1]);
    ECP_ZZZ_mul2(P, &Q, u[0], u[1]);

#else
    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);
    ECP_ZZZ_clmul(P, ee, q);
#endif
}

/* Multiply P by e in group G2 */
void PAIR_ZZZ_G2mul(ECP4_ZZZ *P, BIG_XXX e)
{
    BIG_XXX ee,q;
#ifdef USE_GS_G2_ZZZ   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP4_ZZZ Q[8];
    FP2_YYY X[3];
    BIG_XXX x, y, u[8];

    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);

    ECP4_ZZZ_frob_constants(X);
    gs(u, ee);

    ECP4_ZZZ_copy(&Q[0], P);
    for (i = 1; i < 8; i++)
    {
        ECP4_ZZZ_copy(&Q[i], &Q[i - 1]);
        ECP4_ZZZ_frob(&Q[i], X, 1);
    }

    for (i = 0; i < 8; i++)
    {
        np = BIG_XXX_nbits(u[i]);
        BIG_XXX_modneg(x, u[i], q);
        nn = BIG_XXX_nbits(x);
        if (nn < np)
        {
            BIG_XXX_copy(u[i], x);
            ECP4_ZZZ_neg(&Q[i]);
        }
        BIG_XXX_norm(u[i]);
    }

    ECP4_ZZZ_mul8(P, Q, u);

#else
    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);
    ECP4_ZZZ_mul(P, ee);
#endif
}

/* f=f^e */
void PAIR_ZZZ_GTpow(FP24_YYY *f, BIG_XXX e)
{
    BIG_XXX ee,q;
#ifdef USE_GS_GT_ZZZ   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see FP8.c */
    int i, np, nn;
    FP24_YYY g[8];
    FP2_YYY X;
    BIG_XXX t;
    FP_YYY fx, fy;
    BIG_XXX u[8];

    FP_YYY_rcopy(&fx, Fra_YYY);
    FP_YYY_rcopy(&fy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &fx, &fy);

    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);
    gs(u, ee);

    FP24_YYY_copy(&g[0], f);
    for (i = 1; i < 8; i++)
    {
        FP24_YYY_copy(&g[i], &g[i - 1]);
        FP24_YYY_frob(&g[i], &X, 1);
    }

    for (i = 0; i < 8; i++)
    {
        np = BIG_XXX_nbits(u[i]);
        BIG_XXX_modneg(t, u[i], q);
        nn = BIG_XXX_nbits(t);
        if (nn < np)
        {
            BIG_XXX_copy(u[i], t);
            FP24_YYY_conj(&g[i], &g[i]);
        }
        BIG_XXX_norm(u[i]);
    }
    FP24_YYY_pow8(f, g, u);

#else
    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);
    FP24_YYY_pow(f, f, ee);
#endif
}

/* test G1 group membership */

int PAIR_ZZZ_G1member(ECP_ZZZ *P)
{
    ECP_ZZZ W,T;
    BIG_XXX x;
    FP_YYY cru;
    if (ECP_ZZZ_isinf(P)) return 0;

    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);
    ECP_ZZZ_copy(&W,P);
    ECP_ZZZ_copy(&T,P);
    ECP_ZZZ_mul(&T,x); 
    if (ECP_ZZZ_equals(P,&T)) return 0;    // P is of low order       
    ECP_ZZZ_mul(&T,x);
    ECP_ZZZ_mul(&T,x); ECP_ZZZ_mul(&T,x);
    ECP_ZZZ_neg(&T);

    FP_YYY_rcopy(&cru, CRu_YYY);
    FP_YYY_mul(&(W.x), &(W.x), &cru);
    if (!ECP_ZZZ_equals(&W,&T)) return 0;  // check that Endomorphism works

// Not needed
//    ECP_ZZZ_add(&W,P);
//    FP_YYY_mul(&(T.x), &(T.x), &cru);
//    ECP_ZZZ_add(&W,&T);
//    if (!ECP_ZZZ_isinf(&W)) return 0;      // use it to check order

/*	BIG_XXX q;
	ECP_ZZZ W;
    if (ECP_ZZZ_isinf(P)) return 0;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
	ECP_ZZZ_copy(&W,P);
	ECP_ZZZ_mul(&W,q);
	if (!ECP_ZZZ_isinf(&W)) return 0; */
	return 1;
}

/* test G2 group membership */

int PAIR_ZZZ_G2member(ECP4_ZZZ *P)
{
    ECP4_ZZZ W,T;
    BIG_XXX x;
    FP2_YYY X[3];

    if (ECP4_ZZZ_isinf(P)) return 0;
    ECP4_ZZZ_frob_constants(X);
    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

    ECP4_ZZZ_copy(&W,P);
    ECP4_ZZZ_frob(&W, X, 1);

    ECP4_ZZZ_copy(&T,P);
    ECP4_ZZZ_mul(&T,x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP4_ZZZ_neg(&T);
#endif
/*
    ECP4_ZZZ_copy(&R,&W);
    ECP4_ZZZ_frob(&R,X,1);    // R=\psi^2(P)
    ECP4_ZZZ_sub(&W,&R);
    ECP4_ZZZ_copy(&R,&T);    // R=xP
    ECP4_ZZZ_frob(&R,X,1);
    ECP4_ZZZ_add(&W,&R);     // W=\psi(P)-\psi^2(P)+\psi(xP)
*/
    if (ECP4_ZZZ_equals(&W,&T)) return 1;
    return 0;
/*
	BIG_XXX q;
	ECP4_ZZZ W;
    if (ECP4_ZZZ_isinf(P)) return 0;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
	ECP4_ZZZ_copy(&W,P);
	ECP4_ZZZ_mul(&W,q);
	if (!ECP4_ZZZ_isinf(&W)) return 0;
	return 1; */
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^8}=m^{p^4} */
int PAIR_ZZZ_GTcyclotomic(FP24_YYY *m)
{
	FP_YYY fx,fy;
	FP2_YYY X;
	FP24_YYY r,w;
	if (FP24_YYY_isunity(m)) return 0;
	FP24_YYY_conj(&r,m);
	FP24_YYY_mul(&r,m);
	if (!FP24_YYY_isunity(&r)) return 0;

	FP_YYY_rcopy(&fx,Fra_YYY);
	FP_YYY_rcopy(&fy,Frb_YYY);
	FP2_YYY_from_FPs(&X,&fx,&fy);

	FP24_YYY_copy(&r,m); FP24_YYY_frob(&r,&X,4); 
	FP24_YYY_copy(&w,&r); FP24_YYY_frob(&w,&X,4); 
	FP24_YYY_mul(&w,m);

	if (!FP24_YYY_equals(&w,&r)) return 0;
    return 1;
}


/* test for full GT group membership */
int PAIR_ZZZ_GTmember(FP24_YYY *m)
{
    BIG_XXX x;
    FP2_YYY X;
    FP_YYY fx, fy;
    FP24_YYY r,t;
    if (!PAIR_ZZZ_GTcyclotomic(m)) return 0;

    FP_YYY_rcopy(&fx, Fra_YYY);
    FP_YYY_rcopy(&fy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &fx, &fy);
    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

    FP24_YYY_copy(&r,m);
    FP24_YYY_frob(&r, &X, 1);

    FP24_YYY_pow(&t,m,x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_YYY_conj(&t,&t);
#endif

    if (FP24_YYY_equals(&r,&t)) return 1;
    return 0;

/*
	BIG_XXX q;
    FP24_YYY r;
    if (!PAIR_ZZZ_GTcyclotomic(m)) return 0;

    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
	FP24_YYY_pow(&r,m,q);
	if (!FP24_YYY_isunity(&r)) return 0;
	return 1;
*/
}


#ifdef HAS_MAIN

using namespace std;
using namespace ZZZ;


// g++ -O2 pair4_BLS24.cpp ecp4_BLS24.cpp fp24_BLS24.cpp fp8_BLS24.cpp fp4_BLS24.cpp fp2_BLS24.cpp ecp_BLS24.cpp fp_BLS24.cpp big_XXX.cpp rom_curve_BLS24.cpp rom_field_BLS24.cpp rand.cpp hash.cpp oct.cpp -o pair4_BLS24.exe

int main()
{
    int i;
    char byt[32];
    csprng rng;
    BIG_XXX xa, xb, ya, yb, w, a, b, t1, q, u[2], v[4], m, r, xx, x2, x4, p;
    ECP4_ZZZ P, G;
    ECP_ZZZ Q, R;
    FP24 g, gp;
    FP8_YYY t, c, cp, cpm1, cpm2;
    FP4_YYY X, Y;
    FP2_YYY x, y, f, Aa, Bb;
    FP_YYY cru;

    for (i = 0; i < 32; i++)
        byt[i] = i + 9;
    RAND_seed(&rng, 32, byt);

    BIG_XXX_rcopy(r, CURVE_Order);
    BIG_XXX_rcopy(p, Modulus);


    BIG_XXX_rcopy(xa, CURVE_Gx);
    BIG_XXX_rcopy(ya, CURVE_Gy);

    ECP_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_output(&Q);
    printf("\n");

    ECP4_ZZZ_generator(&P);

    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    BIG_XXX_rcopy(a, Fra);
    BIG_XXX_rcopy(b, Frb);
    FP2_YYY from_BIGs(&f, a, b);

    PAIR_ate(&g, &P, &Q);

    printf("gb= ");
    FP24_output(&g);
    printf("\n");
    PAIR_fexp(&g);

    printf("g= ");
    FP24_output(&g);
    printf("\n");

    ECP_copy(&R, &Q);
    ECP4_ZZZ_copy(&G, &P);

    ECP4_ZZZ_dbl(&G);
    ECP_dbl(&R);
    ECP_affine(&R);

    PAIR_ate(&g, &G, &Q);
    PAIR_fexp(&g);

    printf("g1= ");
    FP24_output(&g);
    printf("\n");

    PAIR_ate(&g, &P, &R);
    PAIR_fexp(&g);

    printf("g2= ");
    FP24_output(&g);
    printf("\n");


    PAIR_G1mul(&Q, r);
    printf("rQ= "); ECP_output(&Q); printf("\n");

    PAIR_G2mul(&P, r);
    printf("rP= "); ECP4_ZZZ_output(&P); printf("\n");

    BIG_XXX_randomnum(w, r, &rng);

    FP24_copy(&gp, &g);

    PAIR_GTpow(&g, w);

    FP24_trace(&t, &g);

    printf("g^r=  "); FP8_output(&t); printf("\n");

    FP24_compow(&t, &gp, w, r);

    printf("t(g)= "); FP8_output(&t); printf("\n");

}

#endif
