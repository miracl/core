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

/* CORE BN Curve pairing functions */

//#define HAS_MAIN

#include "pair_ZZZ.h"

using namespace XXX;
using namespace YYY;

namespace ZZZ {
static void PAIR_line(FP12 *, ECP2 *, ECP2 *, FP *, FP *);
static void PAIR_double(ECP2 *, FP2 *, FP2 *, FP2 *);
static void PAIR_add(ECP2 *, ECP2 *B, FP2 *, FP2 *, FP2 *);
static void PAIR_pack(FP4 *, FP2 *, FP2 *, FP2 *);
static void PAIR_unpack(FP12 *, FP4 *, FP *, FP *);
static void glv(BIG u[2], BIG);
static void gs(BIG u[4], BIG);
}

// Point doubling for pairings
static void ZZZ::PAIR_double(ECP2 *A, FP2 *AA, FP2 *BB, FP2 *CC)
{
    FP2 YY;
    FP2_copy(CC, &(A->x));  //FP2 XX=new FP2(A.getx());  //X
    FP2_copy(&YY, &(A->y)); //FP2 YY=new FP2(A.gety());  //Y
    FP2_copy(BB, &(A->z));  //FP2 ZZ=new FP2(A.getz());  //Z

    FP2_copy(AA, &YY);      //FP2 YZ=new FP2(YY);        //Y
    FP2_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP2_sqr(CC, CC);        //XX.sqr();                //X^2
    FP2_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP2_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP2_add(AA, AA, AA);
    FP2_neg(AA, AA);
    FP2_norm(AA);           // -2YZ
    FP2_mul_ip(AA);
    FP2_norm(AA);           // -2YZi

    FP2_imul(BB, BB, 3 * CURVE_B_I); //3Bz^2
    FP2_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP2_mul_ip(&YY);                    // Y^2.i
    FP2_norm(&YY);
    FP2_mul_ip(CC);                 // 3X^2.i
    FP2_norm(CC);
#endif

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_mul_ip(BB);                 // 3Bz^2.i
    FP2_norm(BB);
#endif

    FP2_sub(BB, BB, &YY);
    FP2_norm(BB);

    ECP2_dbl(A);            //A.dbl();
}

// Point addition for pairings
static void ZZZ::PAIR_add(ECP2 *A, ECP2 *B, FP2 *AA, FP2 *BB, FP2 *CC)
{
    FP2 T1;
    FP2_copy(AA, &(A->x));      //FP2 X1=new FP2(A.getx());    // X1
    FP2_copy(CC, &(A->y));      //FP2 Y1=new FP2(A.gety());    // Y1
    FP2_copy(&T1, &(A->z));     //FP2 T1=new FP2(A.getz());    // Z1

    FP2_copy(BB, &T1);          //FP2 T2=new FP2(A.getz());    // Z1

    FP2_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP2_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP2_sub(AA, AA, BB);        //X1.sub(T2);
    FP2_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP2_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP2_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP2_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_mul_ip(AA);
    FP2_norm(AA);
#endif

    FP2_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP2_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP2_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP2_sub(BB, BB, &T1);       //T2.sub(T1);
    FP2_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP2_neg(CC, CC);            //Y1.neg();
    FP2_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP2_add(A, B);             //A.add(B);
}

/* Line function */
static void ZZZ::PAIR_line(FP12 *v, ECP2 *A, ECP2 *B, FP *Qx, FP *Qy)
{
    FP2 AA, BB, CC;
    FP4 a, b, c;

    if (A == B)
        PAIR_double(A, &AA, &BB, &CC);
    else
        PAIR_add(A, B, &AA, &BB, &CC);

    FP2_pmul(&CC, &CC, Qx);
    FP2_pmul(&AA, &AA, Qy);

    FP4_from_FP2s(&a, &AA, &BB);
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_from_FP2(&b, &CC);
    FP4_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_zero(&b);
    FP4_from_FP2H(&c, &CC);
#endif

    FP12_from_FP4s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int ZZZ::PAIR_nbits(BIG n3, BIG n)
{
    BIG x;
    BIG_rcopy(x, CURVE_Bnx);

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    BIG_pmul(n, x, 6);
#if SIGN_OF_X_ZZZ==POSITIVEX
    BIG_inc(n, 2);
#else
    BIG_dec(n, 2);
#endif

#else
    BIG_copy(n, x);
#endif

    BIG_norm(n);
    BIG_pmul(n3, n, 3);
    BIG_norm(n3);

    return BIG_nbits(n3);
}

/*
    For multi-pairing, product of n pairings
    1. Declare FP12 array of length number of bits in Ate parameter
    2. Initialise this array by calling PAIR_initmp()
    3. Accumulate each pairing by calling PAIR_another() n times
    4. Call PAIR_miller()
    5. Call final exponentiation PAIR_fexp()
*/

/* prepare for multi-pairing */
void ZZZ::PAIR_initmp(FP12 r[])
{
    int i;
    for (i = ATE_BITS_ZZZ - 1; i >= 0; i--)
        FP12_one(&r[i]);
    return;
}

/* basic Miller loop */
void ZZZ::PAIR_miller(FP12 *res, FP12 r[])
{
    int i;
    FP12_one(res);
    for (i = ATE_BITS_ZZZ - 1; i >= 1; i--)
    {
        FP12_sqr(res, res);
        FP12_ssmul(res, &r[i]);
        FP12_zero(&r[i]);
    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_conj(res, res);
#endif
    FP12_ssmul(res, &r[0]);
    FP12_zero(&r[0]);
    return;
}

// Store precomputed line details in an FP4
static void ZZZ::PAIR_pack(FP4 *T, FP2* AA, FP2* BB, FP2 *CC)
{
    FP2 I, A, B;
    FP2_inv(&I, CC, NULL);
    FP2_mul(&A, AA, &I);
    FP2_mul(&B, BB, &I);
    FP4_from_FP2s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void ZZZ::PAIR_unpack(FP12 *v, FP4* T, FP *Qx, FP *Qy)
{
    FP4 a, b, c;
    FP2 t;
    FP4_copy(&a, T);
    FP2_pmul(&a.a, &a.a, Qy);
    FP2_from_FP(&t, Qx);

#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_from_FP2(&b, &t);
    FP4_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_zero(&b);
    FP4_from_FP2H(&c, &t);
#endif

    FP12_from_FP4s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}

// Precompute table of line functions for fixed G2 value
void ZZZ::PAIR_precomp(FP4 T[], ECP2* GV)
{
    int i, j, nb, bt;
    BIG n, n3;
    FP2 AA, BB, CC;
    ECP2 A, G, NG;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    ECP2 K;
    FP2 X;
    FP Qx, Qy;
    FP_rcopy(&Qx, Fra);
    FP_rcopy(&Qy, Frb);
    FP2_from_FPs(&X, &Qx, &Qy);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_inv(&X, &X, NULL);
    FP2_norm(&X);
#endif
#endif

    ECP2_copy(&A, GV);
    ECP2_copy(&G, GV);
    ECP2_copy(&NG, GV);
    ECP2_neg(&NG);

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
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_neg(&A);
#endif

    ECP2_copy(&K, &G);
    ECP2_frob(&K, &X);
    PAIR_add(&A, &K, &AA, &BB, &CC);
    PAIR_pack(&T[j++], &AA, &BB, &CC);
    ECP2_frob(&K, &X);
    ECP2_neg(&K);
    PAIR_add(&A, &K, &AA, &BB, &CC);
    PAIR_pack(&T[j++], &AA, &BB, &CC);

#endif

}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void ZZZ::PAIR_another_pc(FP12 r[], FP4 T[], ECP *QV)
{
    int i, j, nb, bt;
    BIG n, n3;
    FP12 lv, lv2;
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
            FP12_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP12_smul(&lv, &lv2);
        }
        FP12_ssmul(&r[i], &lv);
    }

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    PAIR_unpack(&lv, &T[j++], &Qx, &Qy);
    PAIR_unpack(&lv2, &T[j++], &Qx, &Qy);
    FP12_smul(&lv, &lv2);
    FP12_ssmul(&r[0], &lv);
#endif
}

/* Accumulate another set of line functions for n-pairing */
void ZZZ::PAIR_another(FP12 r[], ECP2* PV, ECP* QV)
{
    int i, nb, bt;
    BIG n, n3;
    FP12 lv, lv2;
    ECP2 A, NP, P;
    ECP Q;
    FP Qx, Qy;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    ECP2 K;
    FP2 X;
    FP_rcopy(&Qx, Fra);
    FP_rcopy(&Qy, Frb);
    FP2_from_FPs(&X, &Qx, &Qy);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_inv(&X, &X, NULL);
    FP2_norm(&X);
#endif
#endif

    if (ECP_isinf(QV)) return;

    nb = PAIR_nbits(n3, n);

    ECP2_copy(&P, PV);
    ECP_copy(&Q, QV);

    ECP2_affine(&P);
    ECP_affine(&Q);

    FP_copy(&Qx, &(Q.x));
    FP_copy(&Qy, &(Q.y));

    ECP2_copy(&A, &P);
    ECP2_copy(&NP, &P); ECP2_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_bit(n3, i) - BIG_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_line(&lv2, &A, &P, &Qx, &Qy);
            FP12_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_line(&lv2, &A, &NP, &Qx, &Qy);
            FP12_smul(&lv, &lv2);
        }
        FP12_ssmul(&r[i], &lv);
    }

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_neg(&A);
#endif

    ECP2_copy(&K, &P);
    ECP2_frob(&K, &X);
    PAIR_line(&lv, &A, &K, &Qx, &Qy);
    ECP2_frob(&K, &X);
    ECP2_neg(&K);
    PAIR_line(&lv2, &A, &K, &Qx, &Qy);
    FP12_smul(&lv, &lv2);
    FP12_ssmul(&r[0], &lv);

#endif
}

/* Optimal single R-ate pairing r=e(P,Q) */
void ZZZ::PAIR_ate(FP12 *r, ECP2 *P1, ECP *Q1)
{
    BIG n, n3;
    FP Qx, Qy;
    int i, nb, bt;
    ECP2 A, NP, P;
    ECP Q;
    FP12 lv, lv2;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    ECP2 KA;
    FP2 X;

    FP_rcopy(&Qx, Fra);
    FP_rcopy(&Qy, Frb);
    FP2_from_FPs(&X, &Qx, &Qy);

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_inv(&X, &X, NULL);
    FP2_norm(&X);
#endif

#endif

    FP12_one(r);
    if (ECP_isinf(Q1)) return;

    nb = PAIR_nbits(n3, n);

    ECP2_copy(&P, P1);
    ECP_copy(&Q, Q1);

    ECP2_affine(&P);
    ECP_affine(&Q);

    FP_copy(&Qx, &(Q.x));
    FP_copy(&Qy, &(Q.y));

    ECP2_copy(&A, &P);
    ECP2_copy(&NP, &P); ECP2_neg(&NP);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP12_sqr(r, r);
        PAIR_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_bit(n3, i) - BIG_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_line(&lv2, &A, &P, &Qx, &Qy);
            FP12_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_line(&lv2, &A, &NP, &Qx, &Qy);
            FP12_smul(&lv, &lv2);
        }
        FP12_ssmul(r, &lv);
    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_conj(r, r);
#endif

    /* R-ate fixup required for BN curves */
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_neg(&A);
#endif

    ECP2_copy(&KA, &P);
    ECP2_frob(&KA, &X);
    PAIR_line(&lv, &A, &KA, &Qx, &Qy);
    ECP2_frob(&KA, &X);
    ECP2_neg(&KA);
    PAIR_line(&lv2, &A, &KA, &Qx, &Qy);
    FP12_smul(&lv, &lv2);
    FP12_ssmul(r, &lv);
#endif
}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void ZZZ::PAIR_double_ate(FP12 *r, ECP2 *P1, ECP *Q1, ECP2 *R1, ECP *S1)
{

    BIG n, n3;
    FP Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP2 A, B, NP, NR, P, R;
    ECP Q, S;
    FP12 lv, lv2;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    ECP2 K;
    FP2 X;

    FP_rcopy(&Qx, Fra);
    FP_rcopy(&Qy, Frb);
    FP2_from_FPs(&X, &Qx, &Qy);

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_inv(&X, &X, NULL);
    FP2_norm(&X);
#endif

#endif

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

    ECP2_copy(&P, P1);
    ECP_copy(&Q, Q1);

    ECP2_affine(&P);
    ECP_affine(&Q);

    ECP2_copy(&R, R1);
    ECP_copy(&S, S1);

    ECP2_affine(&R);
    ECP_affine(&S);

    FP_copy(&Qx, &(Q.x));
    FP_copy(&Qy, &(Q.y));

    FP_copy(&Sx, &(S.x));
    FP_copy(&Sy, &(S.y));

    ECP2_copy(&A, &P);
    ECP2_copy(&B, &R);

    ECP2_copy(&NP, &P); ECP2_neg(&NP);
    ECP2_copy(&NR, &R); ECP2_neg(&NR);

    FP12_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP12_sqr(r, r);
        PAIR_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_line(&lv2, &B, &B, &Sx, &Sy);
        FP12_smul(&lv, &lv2);
        FP12_ssmul(r, &lv);

        bt = BIG_bit(n3, i) - BIG_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_line(&lv2, &B, &R, &Sx, &Sy);
            FP12_smul(&lv, &lv2);
            FP12_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_line(&lv2, &B, &NR, &Sx, &Sy);
            FP12_smul(&lv, &lv2);
            FP12_ssmul(r, &lv);
        }
    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_conj(r, r);
#endif
    /* R-ate fixup required for BN curves */
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_neg(&A);
    ECP2_neg(&B);
#endif

    ECP2_copy(&K, &P);
    ECP2_frob(&K, &X);
    PAIR_line(&lv, &A, &K, &Qx, &Qy);
    ECP2_frob(&K, &X);
    ECP2_neg(&K);
    PAIR_line(&lv2, &A, &K, &Qx, &Qy);
    FP12_smul(&lv, &lv2);
    FP12_ssmul(r, &lv);

    ECP2_copy(&K, &R);
    ECP2_frob(&K, &X);
    PAIR_line(&lv, &B, &K, &Sx, &Sy);
    ECP2_frob(&K, &X);
    ECP2_neg(&K);
    PAIR_line(&lv2, &B, &K, &Sx, &Sy);
    FP12_smul(&lv, &lv2);
    FP12_ssmul(r, &lv);
#endif
}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
void ZZZ::PAIR_fexp(FP12 *r)
{
    FP2 X;
    BIG x;
    FP a, b;
    FP12 t0, y0, y1;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    FP12 y2, y3;
#endif
    BIG_rcopy(x, CURVE_Bnx);
    FP_rcopy(&a, Fra);
    FP_rcopy(&b, Frb);
    FP2_from_FPs(&X, &a, &b);

    /* Easy part of final exp */

    FP12_inv(&t0, r);
    FP12_conj(r, r);

    FP12_mul(r, &t0);
    FP12_copy(&t0, r);

    FP12_frob(r, &X);
    FP12_frob(r, &X);
    FP12_mul(r, &t0);

    /* Hard part of final exp - see Duquesne & Ghamman eprint 2015/192.pdf */
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    FP12_pow(&t0, r, x); // t0=f^-u
#if SIGN_OF_X_ZZZ==POSITIVEX
    FP12_conj(&t0, &t0);
#endif
    FP12_usqr(&y3, &t0); // y3=t0^2
    FP12_copy(&y0, &t0);
    FP12_mul(&y0, &y3); // y0=t0*y3
    FP12_copy(&y2, &y3);
    FP12_frob(&y2, &X); // y2=y3^p
    FP12_mul(&y2, &y3); //y2=y2*y3
    FP12_usqr(&y2, &y2); //y2=y2^2
    FP12_mul(&y2, &y3); // y2=y2*y3

    FP12_pow(&t0, &y0, x); //t0=y0^-u
#if SIGN_OF_X_ZZZ==POSITIVEX
    FP12_conj(&t0, &t0);
#endif
    FP12_conj(&y0, r);    //y0=~r
    FP12_copy(&y1, &t0);
    FP12_frob(&y1, &X);
    FP12_frob(&y1, &X); //y1=t0^p^2
    FP12_mul(&y1, &y0); // y1=y0*y1
    FP12_conj(&t0, &t0); // t0=~t0
    FP12_copy(&y3, &t0);
    FP12_frob(&y3, &X); //y3=t0^p
    FP12_mul(&y3, &t0); // y3=t0*y3
    FP12_usqr(&t0, &t0); // t0=t0^2
    FP12_mul(&y1, &t0); // y1=t0*y1

    FP12_pow(&t0, &y3, x); // t0=y3^-u
#if SIGN_OF_X_ZZZ==POSITIVEX
    FP12_conj(&t0, &t0);
#endif
    FP12_usqr(&t0, &t0); //t0=t0^2
    FP12_conj(&t0, &t0); //t0=~t0
    FP12_mul(&y3, &t0); // y3=t0*y3

    FP12_frob(r, &X);
    FP12_copy(&y0, r);
    FP12_frob(r, &X);
    FP12_mul(&y0, r);
    FP12_frob(r, &X);
    FP12_mul(&y0, r);

    FP12_usqr(r, &y3); //r=y3^2
    FP12_mul(r, &y2);  //r=y2*r
    FP12_copy(&y3, r);
    FP12_mul(&y3, &y0); // y3=r*y0
    FP12_mul(r, &y1); // r=r*y1
    FP12_usqr(r, r); // r=r^2
    FP12_mul(r, &y3); // r=r*y3
    FP12_reduce(r);
#else
 
// See https://eprint.iacr.org/2020/875.pdf
    FP12_usqr(&y1,r);
    FP12_mul(&y1,r);     // y1=r^3

    FP12_pow(&y0,r,x);   // y0=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_conj(&y0, &y0);
#endif
    FP12_conj(&t0,r);    // t0=r^-1
    FP12_copy(r,&y0);
    FP12_mul(r,&t0);    // r=r^(x-1)

    FP12_pow(&y0,r,x);   // y0=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_conj(&y0, &y0);
#endif
    FP12_conj(&t0,r);    // t0=r^-1
    FP12_copy(r,&y0);
    FP12_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP12_pow(&y0,r,x);  // y0=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_conj(&y0, &y0);
#endif
    FP12_copy(&t0,r);   
    FP12_frob(&t0,&X); // t0=r^p
    FP12_copy(r,&y0);
    FP12_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2-1)
    FP12_pow(&y0,r,x);  
    FP12_pow(&y0,&y0,x); // y0=r^x^2
    FP12_copy(&t0,r);    
    FP12_frob(&t0,&X);
    FP12_frob(&t0,&X);   // t0=r^p^2
    FP12_mul(&y0,&t0);   // y0=r^x^2.r^p^2
    FP12_conj(&t0,r);    // t0=r^-1
    FP12_copy(r,&y0);    // 
    FP12_mul(r,&t0);     // r=r^x^2.r^p^2.r^-1

    FP12_mul(r,&y1);    
    FP12_reduce(r);
#endif
}

#ifdef USE_GLV_ZZZ
/* GLV method */
static void ZZZ::glv(BIG u[2], BIG ee)
{
    BIG q;
    BIG_rcopy(q, CURVE_Order);
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    int i, j;
    BIG v[2], t;
    DBIG d;

    for (i = 0; i < 2; i++)
    {
        BIG_rcopy(t, CURVE_W[i]);
        BIG_mul(d, t, ee);
        BIG_ctddiv(v[i],d,q,BIG_nbits(t));
        BIG_zero(u[i]);
    }
    BIG_copy(u[0], ee);
    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
        {
            BIG_rcopy(t, CURVE_SB[j][i]);
            BIG_modmul(t, v[j], t, q);
            BIG_add(u[i], u[i], q);
            BIG_sub(u[i], u[i], t);
            BIG_ctmod(u[i],q,1);
        }

    //BIG x, x2;
    //BIG_rcopy(x, CURVE_Bnx);
    //BIG_smul(x2, x, x);
    //BIG_imul(x2,x2,6);
    //printf("bits(6x^2) = %d \n",BIG_nbits(x2));

#else
// -(x^2).P = (Beta.x,y)
    int bd;
    BIG x, x2;
    BIG_rcopy(x, CURVE_Bnx);

    BIG_smul(x2, x, x);
    bd=BIG_nbits(q)-BIG_nbits(x2); // fixed
    BIG_copy(u[0], ee);
    BIG_ctmod(u[0], x2, bd);
    BIG_copy(u[1], ee);
    BIG_ctsdiv(u[1], x2, bd);

    BIG_sub(u[1], q, u[1]);

#endif

    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void ZZZ::gs(BIG u[4], BIG ee)
{
    int i;
    BIG q;
    BIG_rcopy(q, CURVE_Order);

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    int j;
    BIG v[4], t;
    DBIG d;

    for (i = 0; i < 4; i++)
    {
        BIG_rcopy(t, CURVE_WB[i]);
        BIG_mul(d, t, ee);
        BIG_ctddiv(v[i],d,q,BIG_nbits(t));
        BIG_zero(u[i]);
    }

    BIG_copy(u[0], ee);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            BIG_rcopy(t, CURVE_BB[j][i]);
            BIG_modmul(t, v[j], t, q);
            BIG_add(u[i], u[i], q);
            BIG_sub(u[i], u[i], t);
            BIG_ctmod(u[i],q,1);
        }

    //BIG x;
    //BIG_rcopy(x, CURVE_Bnx);
    //BIG_imul(x,x,3);
    //printf("bits(3x) = %d \n",BIG_nbits(x));

#else
    int bd;
    BIG x, w;

    BIG_rcopy(x, CURVE_Bnx);
    BIG_copy(w, ee);
    bd=BIG_nbits(q)-BIG_nbits(x); // fixed

    for (i = 0; i < 3; i++)
    {
        BIG_copy(u[i], w);
        BIG_ctmod(u[i],x,bd);
        BIG_ctsdiv(w,x,bd);
    }
    BIG_copy(u[3], w);

    /*  */
#if SIGN_OF_X_ZZZ==NEGATIVEX
    BIG_modneg(u[1], u[1], q);
    BIG_modneg(u[3], u[3], q);
#endif

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

    //printf("nbits(q) = %d\n",BIG_nbits(q));
    //printf("nbits(q)/2 = %d\n",BIG_nbits(q)/2);
    //printf("u[0] %d = ",BIG_nbits(u[0]));BIG_output(u[0]); printf("\n");
    //printf("u[1] %d = ",BIG_nbits(u[1]));BIG_output(u[1]); printf("\n");
#else
    ECP_clmul(P, ee, q);
#endif
}

/* Multiply P by e in group G2 */
void ZZZ::PAIR_G2mul(ECP2 *P, BIG e)
{
    BIG ee,q;
    BIG_copy(ee,e);
    BIG_rcopy(q, CURVE_Order);
    BIG_mod(ee,q);
#ifdef USE_GS_G2_ZZZ   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP2 Q[4];
    FP2 X;
    FP fx, fy;
    BIG x, u[4];

    FP_rcopy(&fx, Fra);
    FP_rcopy(&fy, Frb);
    FP2_from_FPs(&X, &fx, &fy);

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_inv(&X, &X, NULL);
    FP2_norm(&X);
#endif

    gs(u, ee);

    ECP2_copy(&Q[0], P);
    for (i = 1; i < 4; i++)
    {
        ECP2_copy(&Q[i], &Q[i - 1]);
        ECP2_frob(&Q[i], &X);
    }

    for (i = 0; i < 4; i++)
    {
        np = BIG_nbits(u[i]);
        BIG_modneg(x, u[i], q);
        nn = BIG_nbits(x);
        if (nn < np)
        {
            BIG_copy(u[i], x);
            ECP2_neg(&Q[i]);
        }
        BIG_norm(u[i]);
    }
    ECP2_mul4(P, Q, u);

    //printf("nbits(q) = %d\n",BIG_nbits(q));
    //printf("nbits(q)/4 = %d\n",BIG_nbits(q)/4);
    //printf("u[0] %d = ",BIG_nbits(u[0]));BIG_output(u[0]); printf("\n");
    //printf("u[1] %d = ",BIG_nbits(u[1]));BIG_output(u[1]); printf("\n");
    //printf("u[2] %d = ",BIG_nbits(u[2]));BIG_output(u[2]); printf("\n");
    //printf("u[3] %d = ",BIG_nbits(u[3]));BIG_output(u[3]); printf("\n");


#else
    ECP2_mul(P, ee);
#endif
}

/* f=f^e */
void ZZZ::PAIR_GTpow(FP12 *f, BIG e)
{
    BIG ee,q;
    BIG_copy(ee,e);
    BIG_rcopy(q, CURVE_Order);
    BIG_mod(ee,q);
#ifdef USE_GS_GT_ZZZ   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see fp4.c */
    int i, np, nn;
    FP12 g[4];
    FP2 X;
    BIG t;
    FP fx, fy;
    BIG u[4];

    FP_rcopy(&fx, Fra);
    FP_rcopy(&fy, Frb);
    FP2_from_FPs(&X, &fx, &fy);

    gs(u, ee);

    FP12_copy(&g[0], f);
    for (i = 1; i < 4; i++)
    {
        FP12_copy(&g[i], &g[i - 1]);
        FP12_frob(&g[i], &X);
    }

    for (i = 0; i < 4; i++)
    {
        np = BIG_nbits(u[i]);
        BIG_modneg(t, u[i], q);
        nn = BIG_nbits(t);
        if (nn < np)
        {
            BIG_copy(u[i], t);
            FP12_conj(&g[i], &g[i]);
        }
        BIG_norm(u[i]);
    }
    FP12_pow4(f, g, u);

#else
    FP12_pow(f, f, ee);
#endif
}

/* test G1 group membership */

int ZZZ::PAIR_G1member(ECP *P)
{
    ECP W,T;
    BIG x;
    FP cru;
    if (ECP_isinf(P)) return 0;
#if PAIRING_FRIENDLY_ZZZ!=BN_CURVE
    BIG_rcopy(x, CURVE_Bnx);
    ECP_copy(&W,P);
    ECP_copy(&T,P);
    ECP_mul(&T,x);
    if (ECP_equals(P,&T)) return 0;    // P is of low order
    ECP_mul(&T,x);
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
    BIG_rcopy(q, CURVE_Order);
	ECP_copy(&W,P);
	ECP_mul(&W,q);
	if (!ECP_isinf(&W)) return 0; */
#endif
	return 1; 
}

/* test G2 group membership */
int ZZZ::PAIR_G2member(ECP2 *P)
{
    ECP2 W,T;
    BIG x;
    FP2 X;
    FP fx, fy;

    if (ECP2_isinf(P)) return 0;
    FP_rcopy(&fx, Fra);
    FP_rcopy(&fy, Frb);
    FP2_from_FPs(&X, &fx, &fy);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_inv(&X, &X, NULL);
    FP2_norm(&X);
#endif
    BIG_rcopy(x, CURVE_Bnx);

    ECP2_copy(&T,P);
    ECP2_mul(&T,x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_neg(&T);
#endif

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
//https://eprint.iacr.org/2022/348.pdf
    ECP2_copy(&W,&T);
    ECP2_frob(&W,&X); // W=\psi(xP)
    ECP2_add(&T,P); // T=xP+P
    ECP2_add(&T,&W); // T=xP+P+\psi(xP)
    ECP2_frob(&W,&X); // W=\psi^2(xP)
    ECP2_add(&T,&W); // T=xp+P+\psi(xP)+\psi^2(xP)
    ECP2_frob(&W,&X); // W=\psi^3(xP)
    ECP2_dbl(&W); // W=\psi^3(2xP)
#else
//https://eprint.iacr.org/2021/1130
    ECP2_copy(&W,P);
    ECP2_frob(&W, &X);    // W=\psi(P)    
#endif

    if (ECP2_equals(&W,&T)) return 1;
    return 0;
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^4}=m^{p^2} */
int ZZZ::PAIR_GTcyclotomic(FP12 *m)
{
	FP fx,fy;
	FP2 X;
	FP12 r,w;
	if (FP12_isunity(m)) return 0;
	FP12_conj(&r,m);
	FP12_mul(&r,m);
	if (!FP12_isunity(&r)) return 0;

	FP_rcopy(&fx,Fra);
	FP_rcopy(&fy,Frb);
	FP2_from_FPs(&X,&fx,&fy);

	FP12_copy(&r,m); FP12_frob(&r,&X); FP12_frob(&r,&X);
	FP12_copy(&w,&r); FP12_frob(&w,&X); FP12_frob(&w,&X);
	FP12_mul(&w,m);

	if (!FP12_equals(&w,&r)) return 0;
    return 1;
}

/* test for full GT group membership */
int ZZZ::PAIR_GTmember(FP12 *m)
{
    BIG x;
    FP2 X;
    FP fx, fy;
    FP12 r,t;
    if (!PAIR_GTcyclotomic(m)) return 0;

    FP_rcopy(&fx, Fra);
    FP_rcopy(&fy, Frb);
    FP2_from_FPs(&X, &fx, &fy);
    BIG_rcopy(x, CURVE_Bnx);

    FP12_pow(&t,m,x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_conj(&t,&t);
#endif

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
//https://eprint.iacr.org/2022/348.pdf
    FP12_copy(&r,&t);
    FP12_frob(&r,&X); // r=(m^x)^p
    FP12_mul(&t,m); // t=(m^x).m
    FP12_mul(&t,&r); // t=(m^x).m.(m^x)^p
    FP12_frob(&r,&X); // r=(m^x)^p^2
    FP12_mul(&t,&r); // t=(m^x).m.(m^x)^p.(m^x)^p^2
    FP12_frob(&r,&X); // r=(m^x)^p^3
    FP12_usqr(&r,&r); // r=(m^2x)^p^3
#else
//https://eprint.iacr.org/2021/1130
    FP12_copy(&r,m);
    FP12_frob(&r, &X);  
#endif

    if (FP12_equals(&r,&t)) return 1;
    return 0;
}

#ifdef HAS_MAIN

int main()
{
    int i;
    char byt[32];
    csprng rng;
    BIG xa, xb, ya, yb, w, a, b, t1, q, u[2], v[4], m, r;
    ECP2 P, G;
    ECP Q, R;
    FP12 g, gp;
    FP4 t, c, cp, cpm1, cpm2;
    FP2 x, y, X;


    BIG_rcopy(a, CURVE_Fra);
    BIG_rcopy(b, CURVE_Frb);
    FP2_from_BIGs(&X, a, b);

    BIG_rcopy(xa, CURVE_Gx);
    BIG_rcopy(ya, CURVE_Gy);

    ECP_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_output(&Q);
    printf("\n");

    BIG_rcopy(xa, CURVE_Pxa);
    BIG_rcopy(xb, CURVE_Pxb);
    BIG_rcopy(ya, CURVE_Pya);
    BIG_rcopy(yb, CURVE_Pyb);

    FP2_from_BIGs(&x, xa, xb);
    FP2_from_BIGs(&y, ya, yb);

    ECP2_set(&P, &x, &y);
    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    printf("P= ");
    ECP2_output(&P);
    printf("\n");

    for (i = 0; i < 1000; i++ )
    {
        PAIR_ate(&g, &P, &Q);
        PAIR_fexp(&g);
    }
    printf("g= ");
    FP12_output(&g);
    printf("\n");

}

#endif
