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

using namespace XXX;
using namespace YYY;

namespace ZZZ {
static void PAIR_line(FP24 *, ECP4 *, ECP4 *, FP *, FP *);
static void PAIR_double(ECP4 *, FP4 *, FP4 *, FP4 *);
static void PAIR_add(ECP4 *, ECP4 *B, FP4 *, FP4 *, FP4 *);
static void PAIR_pack(FP8 *, FP4 *, FP4 *, FP4 *);
static void PAIR_unpack(FP24 *, FP8 *, FP *, FP *);
static void glv(BIG u[2], BIG);
static void gs(BIG u[4], BIG);
}


// Point doubling for pairings
static void ZZZ::PAIR_double(ECP4 *A, FP4 *AA, FP4 *BB, FP4 *CC)
{
    FP4 YY;
    FP4_copy(CC, &(A->x));  //FP4 XX=new FP4(A.getx());  //X
    FP4_copy(&YY, &(A->y)); //FP4 YY=new FP4(A.gety());  //Y
    FP4_copy(BB, &(A->z));  //FP4 ZZ=new FP4(A.getz());  //Z

    FP4_copy(AA, &YY);      //FP4 YZ=new FP4(YY);        //Y
    FP4_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP4_sqr(CC, CC);        //XX.sqr();                //X^2
    FP4_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP4_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP4_add(AA, AA, AA);
    FP4_neg(AA, AA);
    FP4_norm(AA);           // -2YZ
    FP4_times_i(AA);        // -2YZi

    FP4_imul(BB, BB, 3 * CURVE_B_I); //3Bz^2
    FP4_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_times_i(&YY);                   // Y^2.i
    FP4_times_i(CC);                    // 3X^2.i
#endif

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_times_i(BB);                    // 3Bz^2.i
#endif

    FP4_sub(BB, BB, &YY);
    FP4_norm(BB);

    ECP4_dbl(A);            //A.dbl();

}

// Point addition for pairings
static void ZZZ::PAIR_add(ECP4 *A, ECP4 *B, FP4 *AA, FP4 *BB, FP4 *CC)
{
    FP4 T1;
    FP4_copy(AA, &(A->x));      //FP4 X1=new FP4(A.getx());    // X1
    FP4_copy(CC, &(A->y));      //FP4 Y1=new FP4(A.gety());    // Y1
    FP4_copy(&T1, &(A->z));     //FP4 T1=new FP4(A.getz());    // Z1

    FP4_copy(BB, &T1);          //FP4 T2=new FP4(A.getz());    // Z1

    FP4_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP4_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP4_sub(AA, AA, BB);        //X1.sub(T2);
    FP4_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP4_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP4_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP4_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_times_i(AA);
    FP4_norm(AA);
#endif

    FP4_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP4_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP4_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP4_sub(BB, BB, &T1);       //T2.sub(T1);
    FP4_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP4_neg(CC, CC);            //Y1.neg();
    FP4_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP4_add(A, B);             //A.add(B);
}

/* Line function */
static void ZZZ::PAIR_line(FP24 *v, ECP4 *A, ECP4 *B, FP *Qx, FP *Qy)
{
    FP4 AA, BB, CC;
    FP8 a, b, c;

    if (A == B)
        PAIR_double(A, &AA, &BB, &CC);
    else
        PAIR_add(A, B, &AA, &BB, &CC);

    FP4_qmul(&CC, &CC, Qx);
    FP4_qmul(&AA, &AA, Qy);

    FP8_from_FP4s(&a, &AA, &BB);
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_from_FP4(&b, &CC);
    FP8_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP8_zero(&b);
    FP8_from_FP4H(&c, &CC);
#endif

    FP24_from_FP8s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int ZZZ::PAIR_nbits(BIG n3, BIG n)
{
    BIG x;
    BIG_rcopy(x, CURVE_Bnx);

    BIG_copy(n, x);
    BIG_norm(n);
    BIG_pmul(n3, n, 3);
    BIG_norm(n3);

    return BIG_nbits(n3);
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
void ZZZ::PAIR_initmp(FP24 r[])
{
    int i;
    for (i = ATE_BITS_ZZZ - 1; i >= 0; i--)
        FP24_one(&r[i]);
    return;
}

/* basic Miller loop */
void ZZZ::PAIR_miller(FP24 *res, FP24 r[])
{
    int i;
    FP24_one(res);
    for (i = ATE_BITS_ZZZ - 1; i >= 1; i--)
    {
        FP24_sqr(res, res);
        FP24_ssmul(res, &r[i]);
        FP24_zero(&r[i]);
    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(res, res);
#endif
    FP24_ssmul(res, &r[0]);
    FP24_zero(&r[0]);
    return;
}


// Store precomputed line details in an FP4
static void ZZZ::PAIR_pack(FP8 *T, FP4* AA, FP4* BB, FP4 *CC)
{
    FP4 I, A, B;
    FP4_inv(&I, CC, NULL);
    FP4_mul(&A, AA, &I);
    FP4_mul(&B, BB, &I);
    FP8_from_FP4s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void ZZZ::PAIR_unpack(FP24 *v, FP8* T, FP *Qx, FP *Qy)
{
    FP8 a, b, c;
    FP4 t;
    FP8_copy(&a, T);
    FP4_qmul(&a.a, &a.a, Qy);
    FP4_from_FP(&t, Qx);

#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_from_FP4(&b, &t);
    FP8_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP8_zero(&b);
    FP8_from_FP4H(&c, &t);
#endif

    FP24_from_FP8s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}


// Precompute table of line functions for fixed G2 value
void ZZZ::PAIR_precomp(FP8 T[], ECP4* GV)
{
    int i, j, nb, bt;
    BIG n, n3;
    FP4 AA, BB, CC;
    ECP4 A, G, NG;

    ECP4_copy(&A, GV);
    ECP4_copy(&G, GV);
    ECP4_copy(&NG, GV);
    ECP4_neg(&NG);

    nb = PAIR_nbits(n3, n);
    j = 0;

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_double(&A, &AA, &BB, &CC);
        PAIR_pack(&T[j++], &AA, &BB, &CC);

        bt = BIG_bit(n3, i) - BIG_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_add(&A, &G, &AA, &BB, &CC);
            PAIR_pack(&T[j++], &AA, &BB, &CC);
        }
        if (bt == -1)
        {
            PAIR_add(&A, &NG, &AA, &BB, &CC);
            PAIR_pack(&T[j++], &AA, &BB, &CC);
        }
    }
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void ZZZ::PAIR_another_pc(FP24 r[], FP8 T[], ECP *QV)
{
    int i, j, nb, bt;
    BIG x, n, n3;
    FP24 lv, lv2;
    ECP Q;
    FP Qx, Qy;

    if (ECP_isinf(QV)) return;

    nb = PAIR_nbits(n3, n);

    ECP_copy(&Q, QV);
    ECP_affine(&Q);

    FP_copy(&Qx, &(Q.x));
    FP_copy(&Qy, &(Q.y));

    j = 0;
    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_unpack(&lv, &T[j++], &Qx, &Qy);

        bt = BIG_bit(n3, i) - BIG_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP24_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP24_smul(&lv, &lv2);
        }
        FP24_ssmul(&r[i], &lv);
    }
}

/* Accumulate another set of line functions for n-pairing */
void ZZZ::PAIR_another(FP24 r[], ECP4* PV, ECP* QV)
{
    int i, j, nb, bt;
    BIG x, n, n3;
    FP24 lv, lv2;
    ECP4 A, NP, P;
    ECP Q;
    FP Qx, Qy;

    if (ECP_isinf(QV)) return;

    nb = PAIR_nbits(n3, n);

    ECP4_copy(&P, PV);
    ECP_copy(&Q, QV);

    ECP4_affine(&P);
    ECP_affine(&Q);

    FP_copy(&Qx, &(Q.x));
    FP_copy(&Qy, &(Q.y));

    ECP4_copy(&A, &P);
    ECP4_copy(&NP, &P); ECP4_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_bit(n3, i) - BIG_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_line(&lv2, &A, &P, &Qx, &Qy);
            FP24_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_line(&lv2, &A, &NP, &Qx, &Qy);
            FP24_smul(&lv, &lv2);
        }
        FP24_ssmul(&r[i], &lv);
    }
}

/* Optimal R-ate pairing r=e(P,Q) */
void ZZZ::PAIR_ate(FP24 *r, ECP4 *P1, ECP *Q1)
{
    BIG x, n, n3;
    FP Qx, Qy;
    int i, nb, bt;
    ECP4 A, NP, P;
    ECP Q;
    FP24 lv, lv2;

    FP24_one(r);
    if (ECP_isinf(Q1)) return;

    nb = PAIR_nbits(n3, n);

    ECP4_copy(&P, P1);
    ECP_copy(&Q, Q1);

    ECP4_affine(&P);
    ECP_affine(&Q);

    FP_copy(&Qx, &(Q.x));
    FP_copy(&Qy, &(Q.y));

    ECP4_copy(&A, &P);
    ECP4_copy(&NP, &P); ECP4_neg(&NP);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP24_sqr(r, r);
        PAIR_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_bit(n3, i) - BIG_bit(n, i); // BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_line(&lv2, &A, &P, &Qx, &Qy);
            FP24_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_line(&lv2, &A, &NP, &Qx, &Qy);
            FP24_smul(&lv, &lv2);
        }
        FP24_ssmul(r, &lv);

    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(r, r);
#endif

}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void ZZZ::PAIR_double_ate(FP24 *r, ECP4 *P1, ECP *Q1, ECP4 *R1, ECP *S1)
{
    BIG x, n, n3;
    FP Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP4 A, B, NP, NR, P, R;
    ECP Q, S;
    FP24 lv, lv2;

    if (ECP_isinf(Q1))
    {
        PAIR_ate(r, R1, S1);
        return;
    }
    if (ECP_isinf(S1))
    {
        PAIR_ate(r, P1, Q1);
        return;
    }
    nb = PAIR_nbits(n3, n);

    ECP4_copy(&P, P1);
    ECP_copy(&Q, Q1);

    ECP4_affine(&P);
    ECP_affine(&Q);

    ECP4_copy(&R, R1);
    ECP_copy(&S, S1);

    ECP4_affine(&R);
    ECP_affine(&S);

    FP_copy(&Qx, &(Q.x));
    FP_copy(&Qy, &(Q.y));

    FP_copy(&Sx, &(S.x));
    FP_copy(&Sy, &(S.y));

    ECP4_copy(&A, &P);
    ECP4_copy(&B, &R);

    ECP4_copy(&NP, &P); ECP4_neg(&NP);
    ECP4_copy(&NR, &R); ECP4_neg(&NR);


    FP24_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP24_sqr(r, r);
        PAIR_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_line(&lv2, &B, &B, &Sx, &Sy);
        FP24_smul(&lv, &lv2);
        FP24_ssmul(r, &lv);

        bt = BIG_bit(n3, i) - BIG_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_line(&lv2, &B, &R, &Sx, &Sy);
            FP24_smul(&lv, &lv2);
            FP24_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_line(&lv2, &B, &NR, &Sx, &Sy);
            FP24_smul(&lv, &lv2);
            FP24_ssmul(r, &lv);
        }
    }



#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(r, r);
#endif

}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */

void ZZZ::PAIR_fexp(FP24 *r)
{
    FP2 X;
    BIG x;
    FP a, b;
    FP24 t0, t1, t2; //, t3, t4, t5, t6, t7; // could lose one of these - r=t3

    BIG_rcopy(x, CURVE_Bnx);
    FP_rcopy(&a, Fra);
    FP_rcopy(&b, Frb);
    FP2_from_FPs(&X, &a, &b);

    /* Easy part of final exp - r^(p^12-1)(p^4+1)*/

    FP24_inv(&t0, r);
    FP24_conj(r, r);

    FP24_mul(r, &t0);
    FP24_copy(&t0, r);

    FP24_frob(r, &X, 4);

    FP24_mul(r, &t0);

// See https://eprint.iacr.org/2020/875.pdf
    FP24_usqr(&t2,r);
    FP24_mul(&t2,r);     // t2=r^3

    FP24_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t1, &t1);
#endif
    FP24_conj(&t0,r);    // t0=r^-1
    FP24_copy(r,&t1);
    FP24_mul(r,&t0);    // r=r^(x-1)

    FP24_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t1, &t1);
#endif
    FP24_conj(&t0,r);    // t0=r^-1
    FP24_copy(r,&t1);
    FP24_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP24_pow(&t1,r,x);  // t1=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t1, &t1);
#endif
    FP24_copy(&t0,r);   
    FP24_frob(&t0,&X,1); // t0=r^p
    FP24_copy(r,&t1);
    FP24_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2)
    FP24_pow(&t1,r,x);  
    FP24_pow(&t1,&t1,x); // t1=r^x^2
    FP24_copy(&t0,r);    
    FP24_frob(&t0,&X,2);   // t0=r^p^2
    FP24_mul(&t1,&t0);   // t1=r^x^2.r^p^2
    FP24_copy(r,&t1);

// ^(x^4+p^4-1)
    FP24_pow(&t1,r,x);  
    FP24_pow(&t1,&t1,x); 
    FP24_pow(&t1,&t1,x); 
    FP24_pow(&t1,&t1,x); // t1=r^x^4
    FP24_copy(&t0,r);    
    FP24_frob(&t0,&X,4); // t0=r^p^4
    FP24_mul(&t1,&t0);   // t1=r^x^4.r^p^4
    FP24_conj(&t0,r);    // t0=r^-1
    FP24_copy(r,&t1);    
    FP24_mul(r,&t0);     // r=r^x^4.r^p^4.r^-1

    FP24_mul(r,&t2);    
    FP24_reduce(r);

/*
// Ghamman & Fouotsa Method - (completely garbled in  https://eprint.iacr.org/2016/130)

    FP24_usqr(&t7, r);          // t7=f^2
    FP24_pow(&t1, &t7, x);      // t1=t7^u

    BIG_fshr(x, 1);
    FP24_pow(&t2, &t1, x);      // t2=t1^(u/2)
    BIG_fshl(x, 1); // x must be even

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t1, &t1);
#endif

    FP24_conj(&t3, &t1);    // t3=1/t1
    FP24_mul(&t2, &t3);     // t2=t1*t3
    FP24_mul(&t2, r);       // t2=t2*f


    FP24_pow(&t3, &t2, x);      // t3=t2^u
    FP24_pow(&t4, &t3, x);      // t4=t3^u
    FP24_pow(&t5, &t4, x);      // t5=t4^u

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t3, &t3);
    FP24_conj(&t5, &t5);
#endif

    FP24_frob(&t3, &X, 6);
    FP24_frob(&t4, &X, 5);

    FP24_mul(&t3, &t4);     // t3=t3.t4


    FP24_pow(&t6, &t5, x);      // t6=t5^u
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t6, &t6);
#endif


    FP24_frob(&t5, &X, 4);
    FP24_mul(&t3, &t5); // ??


    FP24_conj(&t0, &t2);        // t0=1/t2
    FP24_mul(&t6, &t0);     // t6=t6*t0

    FP24_copy(&t5, &t6);
    FP24_frob(&t5, &X, 3);

    FP24_mul(&t3, &t5);     // t3=t3*t5
    FP24_pow(&t5, &t6, x);  // t5=t6^x
    FP24_pow(&t6, &t5, x);  // t6=t5^x

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t5, &t5);
#endif

    FP24_copy(&t0, &t5);
    FP24_frob(&t0, &X, 2);
    FP24_mul(&t3, &t0);     // t3=t3*t0
    FP24_copy(&t0, &t6);    //
    FP24_frob(&t0, &X, 1);

    FP24_mul(&t3, &t0);     // t3=t3*t0
    FP24_pow(&t5, &t6, x);  // t5=t6*x

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t5, &t5);
#endif

    FP24_frob(&t2, &X, 7);

    FP24_mul(&t5, &t7);     // t5=t5*t7
    FP24_mul(&t3, &t2);     // t3=t3*t2
    FP24_mul(&t3, &t5);     // t3=t3*t5

    FP24_mul(r, &t3);
    FP24_reduce(r);
*/
}

#ifdef USE_GLV_ZZZ
/* GLV method */
static void ZZZ::glv(BIG u[2], BIG ee)
{
    int bd;
    BIG q,x,x2;
    BIG_rcopy(q, CURVE_Order);

// -(x^4).P = (Beta.x,y)

    BIG_rcopy(x, CURVE_Bnx);

    BIG_smul(x2, x, x);
    BIG_smul(x, x2, x2);
    bd=BIG_nbits(q)-BIG_nbits(x); // fixed x^4

    BIG_copy(u[0], ee);
    BIG_ctmod(u[0], x, bd);
    BIG_copy(u[1], ee);
    BIG_ctsdiv(u[1], x, bd);
 
    BIG_sub(u[1], q, u[1]);


    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void ZZZ::gs(BIG u[8], BIG ee)
{
    int i,bd;
    BIG q,x,w;
    BIG_rcopy(q, CURVE_Order);

    BIG_rcopy(x, CURVE_Bnx);
    BIG_copy(w, ee);
    bd=BIG_nbits(q)-BIG_nbits(x); // fixed

    for (i = 0; i < 7; i++)
    {
        BIG_copy(u[i], w);
        BIG_ctmod(u[i], x, bd);
        BIG_ctsdiv(w, x, bd);
    }
    BIG_copy(u[7], w);

    /*  */
#if SIGN_OF_X_ZZZ==NEGATIVEX
    BIG_modneg(u[1], u[1], q);
    BIG_modneg(u[3], u[3], q);
    BIG_modneg(u[5], u[5], q);
    BIG_modneg(u[7], u[7], q);
#endif

    return;
}

/* Multiply P by e in group G1 */
void ZZZ::PAIR_G1mul(ECP *P, BIG e)
{
    BIG ee,q;
    BIG_copy(ee,e);
    BIG_rcopy(q, CURVE_Order);
    BIG_mod(ee,q);
#ifdef USE_GLV_ZZZ   /* Note this method is patented */
    int np, nn;
    ECP Q;
    FP cru;
    BIG t;
    BIG u[2];

    glv(u, ee);

    ECP_copy(&Q, P); ECP_affine(&Q);
    FP_rcopy(&cru, CRu);
    FP_mul(&(Q.x), &(Q.x), &cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np = BIG_nbits(u[0]);
    BIG_modneg(t, u[0], q);
    nn = BIG_nbits(t);
    if (nn < np)
    {
        BIG_copy(u[0], t);
        ECP_neg(P);
    }

    np = BIG_nbits(u[1]);
    BIG_modneg(t, u[1], q);
    nn = BIG_nbits(t);
    if (nn < np)
    {
        BIG_copy(u[1], t);
        ECP_neg(&Q);
    }
    BIG_norm(u[0]);
    BIG_norm(u[1]);
    ECP_mul2(P, &Q, u[0], u[1]);

#else
    ECP_clmul(P, ee, q);
#endif
}

/* Multiply P by e in group G2 */
void ZZZ::PAIR_G2mul(ECP4 *P, BIG e)
{
    BIG ee,q;
    BIG_copy(ee,e);
    BIG_rcopy(q, CURVE_Order);
    BIG_mod(ee,q);
#ifdef USE_GS_G2_ZZZ   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP4 Q[8];
    FP2 X[3];
    BIG x, u[8];

    ECP4_frob_constants(X);

    gs(u, ee);

    ECP4_copy(&Q[0], P);
    for (i = 1; i < 8; i++)
    {
        ECP4_copy(&Q[i], &Q[i - 1]);
        ECP4_frob(&Q[i], X, 1);
    }

    for (i = 0; i < 8; i++)
    {
        np = BIG_nbits(u[i]);
        BIG_modneg(x, u[i], q);
        nn = BIG_nbits(x);
        if (nn < np)
        {
            BIG_copy(u[i], x);
            ECP4_neg(&Q[i]);
        }
        BIG_norm(u[i]);
    }

    ECP4_mul8(P, Q, u);

#else
    ECP4_mul(P, ee);
#endif
}

/* f=f^e */
void ZZZ::PAIR_GTpow(FP24 *f, BIG e)
{
    BIG ee,q;
    BIG_copy(ee,e);
    BIG_rcopy(q, CURVE_Order);
    BIG_mod(ee,q);
#ifdef USE_GS_GT_ZZZ   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see FP8.c */
    int i, np, nn;
    FP24 g[8];
    FP2 X;
    BIG t;
    FP fx, fy;
    BIG u[8];

    FP_rcopy(&fx, Fra);
    FP_rcopy(&fy, Frb);
    FP2_from_FPs(&X, &fx, &fy);

    gs(u, ee);

    FP24_copy(&g[0], f);
    for (i = 1; i < 8; i++)
    {
        FP24_copy(&g[i], &g[i - 1]);
        FP24_frob(&g[i], &X, 1);
    }

    for (i = 0; i < 8; i++)
    {
        np = BIG_nbits(u[i]);
        BIG_modneg(t, u[i], q);
        nn = BIG_nbits(t);
        if (nn < np)
        {
            BIG_copy(u[i], t);
            FP24_conj(&g[i], &g[i]);
        }
        BIG_norm(u[i]);
    }
    FP24_pow8(f, g, u);

#else
    FP24_pow(f, f, ee);
#endif
}

/* test G1 group membership */

int ZZZ::PAIR_G1member(ECP *P)
{
    ECP W,T;
    BIG x;
    FP cru;
    if (ECP_isinf(P)) return 0;

    BIG_rcopy(x, CURVE_Bnx);
    ECP_copy(&W,P);
    ECP_copy(&T,P);
    ECP_mul(&T,x); 
    if (ECP_equals(P,&T)) return 0;    // P is of low order    
    ECP_mul(&T,x);
    ECP_mul(&T,x); ECP_mul(&T,x);
    ECP_neg(&T);

    FP_rcopy(&cru, CRu);
    FP_mul(&(W.x), &(W.x), &cru);
    if (!ECP_equals(&W,&T)) return 0;  // check that Endomorphism works

// Not needed
//    ECP_add(&W,P);
//    FP_mul(&(T.x), &(T.x), &cru);
//    ECP_add(&W,&T);
//    if (!ECP_isinf(&W)) return 0;      // use it to check order

/*
	BIG q;
	ECP W;
    if (ECP_isinf(P)) return 0;
    BIG_rcopy(q, CURVE_Order);
	ECP_copy(&W,P);
	ECP_mul(&W,q);
	if (!ECP_isinf(&W)) return 0; */
	return 1;
}

/* test G2 group membership */

int ZZZ::PAIR_G2member(ECP4 *P)
{
    ECP4 W,T;
    BIG x;
    FP2 X[3];

    if (ECP4_isinf(P)) return 0;
    ECP4_frob_constants(X);
    BIG_rcopy(x, CURVE_Bnx);

    ECP4_copy(&W,P);
    ECP4_frob(&W, X, 1);

    ECP4_copy(&T,P);
    ECP4_mul(&T,x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP4_neg(&T);
#endif
/*
    ECP4_copy(&R,&W);
    ECP4_frob(&R,X,1);    // R=\psi^2(P)
    ECP4_sub(&W,&R);
    ECP4_copy(&R,&T);    // R=xP
    ECP4_frob(&R,X,1);
    ECP4_add(&W,&R);     // W=\psi(P)-\psi^2(P)+\psi(xP)
*/
    if (ECP4_equals(&W,&T)) return 1;
    return 0;

/*
	BIG q;
	ECP4 W;
    if (ECP4_isinf(P)) return 0;
    BIG_rcopy(q, CURVE_Order);
	ECP4_copy(&W,P);
	ECP4_mul(&W,q);
	if (!ECP4_isinf(&W)) return 0;
	return 1; */
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^8}=m^{p^4} */
int ZZZ::PAIR_GTcyclotomic(FP24 *m)
{
	FP fx,fy;
	FP2 X;
	FP24 r,w;
	if (FP24_isunity(m)) return 0;
	FP24_conj(&r,m);
	FP24_mul(&r,m);
	if (!FP24_isunity(&r)) return 0;

	FP_rcopy(&fx,Fra);
	FP_rcopy(&fy,Frb);
	FP2_from_FPs(&X,&fx,&fy);

	FP24_copy(&r,m); FP24_frob(&r,&X,4); 
	FP24_copy(&w,&r); FP24_frob(&w,&X,4); 
	FP24_mul(&w,m);

	if (!FP24_equals(&w,&r)) return 0;
    return 1;
}
/* test for full GT group membership */

int ZZZ::PAIR_GTmember(FP24 *m)
{
    BIG x;
    FP2 X;
    FP fx, fy;
    FP24 r,t;
    if (!PAIR_GTcyclotomic(m)) return 0;

    FP_rcopy(&fx, Fra);
    FP_rcopy(&fy, Frb);
    FP2_from_FPs(&X, &fx, &fy);
    BIG_rcopy(x, CURVE_Bnx);

    FP24_copy(&r,m);
    FP24_frob(&r, &X, 1);

    FP24_pow(&t,m,x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP24_conj(&t,&t);
#endif

    if (FP24_equals(&r,&t)) return 1;
    return 0;
/*
    BIG_rcopy(q, CURVE_Order);
    FP24_pow(&r, m, q);
	if (!FP24_isunity(&r)) return 0;
	return 1; */
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
    BIG xa, xb, ya, yb, w, a, b, t1, q, u[2], v[4], m, r, xx, x2, x4, p;
    ECP4 P, G;
    ECP Q, R;
    FP24 g, gp;
    FP8 t, c, cp, cpm1, cpm2;
    FP4 X, Y;
    FP2 x, y, f, Aa, Bb;
    FP cru;

    for (i = 0; i < 32; i++)
        byt[i] = i + 9;
    RAND_seed(&rng, 32, byt);

    BIG_rcopy(r, CURVE_Order);
    BIG_rcopy(p, Modulus);


    BIG_rcopy(xa, CURVE_Gx);
    BIG_rcopy(ya, CURVE_Gy);

    ECP_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_output(&Q);
    printf("\n");

    ECP4_generator(&P);

    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    BIG_rcopy(a, Fra);
    BIG_rcopy(b, Frb);
    FP2_from_BIGs(&f, a, b);

    PAIR_ate(&g, &P, &Q);

    printf("gb= ");
    FP24_output(&g);
    printf("\n");
    PAIR_fexp(&g);

    printf("g= ");
    FP24_output(&g);
    printf("\n");

    ECP_copy(&R, &Q);
    ECP4_copy(&G, &P);

    ECP4_dbl(&G);
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
    printf("rP= "); ECP4_output(&P); printf("\n");

    BIG_randomnum(w, r, &rng);

    FP24_copy(&gp, &g);

    PAIR_GTpow(&g, w);

    FP24_trace(&t, &g);

    printf("g^r=  "); FP8_output(&t); printf("\n");

    FP24_compow(&t, &gp, w, r);

    printf("t(g)= "); FP8_output(&t); printf("\n");

}

#endif
