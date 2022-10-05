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

// Point doubling for pairings
static void PAIR_ZZZ_double(ECP2_ZZZ *A, FP2_YYY *AA, FP2_YYY *BB, FP2_YYY *CC)
{
    FP2_YYY YY;
    FP2_YYY_copy(CC, &(A->x));  //FP2 XX=new FP2(A.getx());  //X
    FP2_YYY_copy(&YY, &(A->y)); //FP2 YY=new FP2(A.gety());  //Y
    FP2_YYY_copy(BB, &(A->z));  //FP2 ZZ=new FP2(A.getz());  //Z

    FP2_YYY_copy(AA, &YY);      //FP2 YZ=new FP2(YY);        //Y
    FP2_YYY_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP2_YYY_sqr(CC, CC);        //XX.sqr();                //X^2
    FP2_YYY_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP2_YYY_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP2_YYY_add(AA, AA, AA);
    FP2_YYY_neg(AA, AA);
    FP2_YYY_norm(AA);           // -2YZ
    FP2_YYY_mul_ip(AA);
    FP2_YYY_norm(AA);           // -2YZi

    FP2_YYY_imul(BB, BB, 3 * CURVE_B_I_ZZZ); //3Bz^2
    FP2_YYY_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP2_YYY_mul_ip(&YY);                    // Y^2.i
    FP2_YYY_norm(&YY);
    FP2_YYY_mul_ip(CC);                 // 3X^2.i
    FP2_YYY_norm(CC);
#endif

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_YYY_mul_ip(BB);                 // 3Bz^2.i
    FP2_YYY_norm(BB);
#endif

    FP2_YYY_sub(BB, BB, &YY);
    FP2_YYY_norm(BB);

    ECP2_ZZZ_dbl(A);            //A.dbl();
}

// Point addition for pairings
static void PAIR_ZZZ_add(ECP2_ZZZ *A, ECP2_ZZZ *B, FP2_YYY *AA, FP2_YYY *BB, FP2_YYY *CC)
{
    FP2_YYY T1;
    FP2_YYY_copy(AA, &(A->x));      //FP2 X1=new FP2(A.getx());    // X1
    FP2_YYY_copy(CC, &(A->y));      //FP2 Y1=new FP2(A.gety());    // Y1
    FP2_YYY_copy(&T1, &(A->z));     //FP2 T1=new FP2(A.getz());    // Z1

    FP2_YYY_copy(BB, &T1);          //FP2 T2=new FP2(A.getz());    // Z1

    FP2_YYY_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP2_YYY_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP2_YYY_sub(AA, AA, BB);        //X1.sub(T2);
    FP2_YYY_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP2_YYY_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP2_YYY_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP2_YYY_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_YYY_mul_ip(AA);
    FP2_YYY_norm(AA);
#endif

    FP2_YYY_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP2_YYY_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP2_YYY_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP2_YYY_sub(BB, BB, &T1);       //T2.sub(T1);
    FP2_YYY_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP2_YYY_neg(CC, CC);            //Y1.neg();
    FP2_YYY_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP2_ZZZ_add(A, B);             //A.add(B);
}

/* Line function */
static void PAIR_ZZZ_line(FP12_YYY *v, ECP2_ZZZ *A, ECP2_ZZZ *B, FP_YYY *Qx, FP_YYY *Qy)
{
    FP2_YYY AA, BB, CC;
    FP4_YYY a, b, c;

    if (A == B)
        PAIR_ZZZ_double(A, &AA, &BB, &CC);
    else
        PAIR_ZZZ_add(A, B, &AA, &BB, &CC);

    FP2_YYY_pmul(&CC, &CC, Qx);
    FP2_YYY_pmul(&AA, &AA, Qy);

    FP4_YYY_from_FP2s(&a, &AA, &BB);
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_YYY_from_FP2(&b, &CC);
    FP4_YYY_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_YYY_zero(&b);
    FP4_YYY_from_FP2H(&c, &CC);
#endif

    FP12_YYY_from_FP4s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int PAIR_ZZZ_nbits(BIG_XXX n3, BIG_XXX n)
{
    BIG_XXX x;
    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    BIG_XXX_pmul(n, x, 6);
#if SIGN_OF_X_ZZZ==POSITIVEX
    BIG_XXX_inc(n, 2);
#else
    BIG_XXX_dec(n, 2);
#endif

#else
    BIG_XXX_copy(n, x);
#endif

    BIG_XXX_norm(n);
    BIG_XXX_pmul(n3, n, 3);
    BIG_XXX_norm(n3);

    return BIG_XXX_nbits(n3);
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
void PAIR_ZZZ_initmp(FP12_YYY r[])
{
    int i;
    for (i = ATE_BITS_ZZZ - 1; i >= 0; i--)
        FP12_YYY_one(&r[i]);
    return;
}

// Store precomputed line details in an FP4
static void PAIR_ZZZ_pack(FP4_YYY *T, FP2_YYY* AA, FP2_YYY* BB, FP2_YYY *CC)
{
    FP2_YYY I, A, B;
    FP2_YYY_inv(&I, CC, NULL);
    FP2_YYY_mul(&A, AA, &I);
    FP2_YYY_mul(&B, BB, &I);
    FP4_YYY_from_FP2s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void PAIR_ZZZ_unpack(FP12_YYY *v, FP4_YYY* T, FP_YYY *Qx, FP_YYY *Qy)
{
    FP4_YYY a, b, c;
    FP2_YYY t;
    FP4_YYY_copy(&a, T);
    FP2_YYY_pmul(&a.a, &a.a, Qy);
    FP2_YYY_from_FP(&t, Qx);

#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_YYY_from_FP2(&b, &t);
    FP4_YYY_zero(&c);
#endif
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_YYY_zero(&b);
    FP4_YYY_from_FP2H(&c, &t);
#endif

    FP12_YYY_from_FP4s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}


/* basic Miller loop */
void PAIR_ZZZ_miller(FP12_YYY *res, FP12_YYY r[])
{
    int i;
    FP12_YYY_one(res);
    for (i = ATE_BITS_ZZZ - 1; i >= 1; i--)
    {
        FP12_YYY_sqr(res, res);
        FP12_YYY_ssmul(res, &r[i]);
        FP12_YYY_zero(&r[i]);
    }

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(res, res);
#endif
    FP12_YYY_ssmul(res, &r[0]);
    FP12_YYY_zero(&r[0]);
    return;
}


// Precompute table of line functions for fixed G2 value
void PAIR_ZZZ_precomp(FP4_YYY T[], ECP2_ZZZ* GV)
{
    int i, j, nb, bt;
    BIG_XXX n, n3;
    FP2_YYY AA, BB, CC;
    ECP2_ZZZ A, G, NG;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    ECP2_ZZZ K;
    FP2_YYY X;
    FP_YYY Qx, Qy;
    FP_YYY_rcopy(&Qx, Fra_YYY);
    FP_YYY_rcopy(&Qy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &Qx, &Qy);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_YYY_inv(&X, &X, NULL);
    FP2_YYY_norm(&X);
#endif
#endif

    ECP2_ZZZ_copy(&A, GV);
    ECP2_ZZZ_copy(&G, GV);
    ECP2_ZZZ_copy(&NG, GV);
    ECP2_ZZZ_neg(&NG);

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
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_ZZZ_neg(&A);
#endif

    ECP2_ZZZ_copy(&K, &G);
    ECP2_ZZZ_frob(&K, &X);
    PAIR_ZZZ_add(&A, &K, &AA, &BB, &CC);
    PAIR_ZZZ_pack(&T[j++], &AA, &BB, &CC);
    ECP2_ZZZ_frob(&K, &X);
    ECP2_ZZZ_neg(&K);
    PAIR_ZZZ_add(&A, &K, &AA, &BB, &CC);
    PAIR_ZZZ_pack(&T[j++], &AA, &BB, &CC);

#endif
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void PAIR_ZZZ_another_pc(FP12_YYY r[], FP4_YYY T[], ECP_ZZZ *QV)
{
    int i, j, nb, bt;
    BIG_XXX x, n, n3;
    FP12_YYY lv, lv2;
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
            FP12_YYY_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP12_YYY_smul(&lv, &lv2);
        }
        FP12_YYY_ssmul(&r[i], &lv);
    }

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    PAIR_ZZZ_unpack(&lv, &T[j++], &Qx, &Qy);
    PAIR_ZZZ_unpack(&lv2, &T[j++], &Qx, &Qy);
    FP12_YYY_smul(&lv, &lv2);
    FP12_YYY_ssmul(&r[0], &lv);
#endif
}

/* Accumulate another set of line functions for n-pairing */
void PAIR_ZZZ_another(FP12_YYY r[], ECP2_ZZZ* PV, ECP_ZZZ* QV)
{
    int i, j, nb, bt;
    BIG_XXX x, n, n3;
    FP12_YYY lv, lv2;
    ECP2_ZZZ A, NP, P;
    ECP_ZZZ Q;
    FP_YYY Qx, Qy;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    ECP2_ZZZ K;
    FP2_YYY X;
    FP_YYY_rcopy(&Qx, Fra_YYY);
    FP_YYY_rcopy(&Qy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &Qx, &Qy);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_YYY_inv(&X, &X, NULL);
    FP2_YYY_norm(&X);
#endif
#endif

    if (ECP_ZZZ_isinf(QV)) return;

    nb = PAIR_ZZZ_nbits(n3, n);

    ECP2_ZZZ_copy(&P, PV);
    ECP_ZZZ_copy(&Q, QV);

    ECP2_ZZZ_affine(&P);
    ECP_ZZZ_affine(&Q);

    FP_YYY_copy(&Qx, &(Q.x));
    FP_YYY_copy(&Qy, &(Q.y));

    ECP2_ZZZ_copy(&A, &P);
    ECP2_ZZZ_copy(&NP, &P); ECP2_ZZZ_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_ZZZ_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_XXX_bit(n3, i) - BIG_XXX_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_ZZZ_line(&lv2, &A, &P, &Qx, &Qy);
            FP12_YYY_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_line(&lv2, &A, &NP, &Qx, &Qy);
            FP12_YYY_smul(&lv, &lv2);
        }
        FP12_YYY_ssmul(&r[i], &lv);
    }

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_ZZZ_neg(&A);
#endif

    ECP2_ZZZ_copy(&K, &P);
    ECP2_ZZZ_frob(&K, &X);
    PAIR_ZZZ_line(&lv, &A, &K, &Qx, &Qy);
    ECP2_ZZZ_frob(&K, &X);
    ECP2_ZZZ_neg(&K);
    PAIR_ZZZ_line(&lv2, &A, &K, &Qx, &Qy);
    FP12_YYY_smul(&lv, &lv2);
    FP12_YYY_ssmul(&r[0], &lv);

#endif
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_ZZZ_ate(FP12_YYY *r, ECP2_ZZZ *P1, ECP_ZZZ *Q1)
{

    BIG_XXX x, n, n3;
    FP_YYY Qx, Qy;
    int i, nb, bt;
    ECP2_ZZZ A, NP, P;
    ECP_ZZZ Q;
    FP12_YYY lv, lv2;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    ECP2_ZZZ KA;
    FP2_YYY X;

    FP_YYY_rcopy(&Qx, Fra_YYY);
    FP_YYY_rcopy(&Qy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &Qx, &Qy);

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_YYY_inv(&X, &X, NULL);
    FP2_YYY_norm(&X);
#endif
#endif

    FP12_YYY_one(r);
    if (ECP_ZZZ_isinf(Q1)) return;

    nb = PAIR_ZZZ_nbits(n3, n);

    ECP2_ZZZ_copy(&P, P1);
    ECP_ZZZ_copy(&Q, Q1);

    ECP2_ZZZ_affine(&P);
    ECP_ZZZ_affine(&Q);

    FP_YYY_copy(&Qx, &(Q.x));
    FP_YYY_copy(&Qy, &(Q.y));

    ECP2_ZZZ_copy(&A, &P);
    ECP2_ZZZ_copy(&NP, &P); ECP2_ZZZ_neg(&NP);



    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--) //0
    {
        FP12_YYY_sqr(r, r);
        PAIR_ZZZ_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_XXX_bit(n3, i) - BIG_XXX_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_ZZZ_line(&lv2, &A, &P, &Qx, &Qy);
            FP12_YYY_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_line(&lv2, &A, &NP, &Qx, &Qy);
            FP12_YYY_smul(&lv, &lv2);
        }
        FP12_YYY_ssmul(r, &lv);

    }


#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(r, r);
#endif

    /* R-ate fixup required for BN curves */
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_ZZZ_neg(&A);
#endif

    ECP2_ZZZ_copy(&KA, &P);
    ECP2_ZZZ_frob(&KA, &X);
    PAIR_ZZZ_line(&lv, &A, &KA, &Qx, &Qy);
    ECP2_ZZZ_frob(&KA, &X);
    ECP2_ZZZ_neg(&KA);
    PAIR_ZZZ_line(&lv2, &A, &KA, &Qx, &Qy);
    FP12_YYY_smul(&lv, &lv2);
    FP12_YYY_ssmul(r, &lv);
#endif
}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_ZZZ_double_ate(FP12_YYY *r, ECP2_ZZZ *P1, ECP_ZZZ *Q1, ECP2_ZZZ *R1, ECP_ZZZ *S1)
{
    BIG_XXX x, n, n3;
    FP_YYY Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP2_ZZZ A, B, NP, NR, P, R;
    ECP_ZZZ Q, S;
    FP12_YYY lv, lv2;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    FP2_YYY X;
    ECP2_ZZZ K;

    FP_YYY_rcopy(&Qx, Fra_YYY);
    FP_YYY_rcopy(&Qy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &Qx, &Qy);

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_YYY_inv(&X, &X, NULL);
    FP2_YYY_norm(&X);
#endif
#endif

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

    ECP2_ZZZ_copy(&P, P1);
    ECP_ZZZ_copy(&Q, Q1);

    ECP2_ZZZ_affine(&P);
    ECP_ZZZ_affine(&Q);

    ECP2_ZZZ_copy(&R, R1);
    ECP_ZZZ_copy(&S, S1);

    ECP2_ZZZ_affine(&R);
    ECP_ZZZ_affine(&S);

    FP_YYY_copy(&Qx, &(Q.x));
    FP_YYY_copy(&Qy, &(Q.y));

    FP_YYY_copy(&Sx, &(S.x));
    FP_YYY_copy(&Sy, &(S.y));

    ECP2_ZZZ_copy(&A, &P);
    ECP2_ZZZ_copy(&B, &R);

    ECP2_ZZZ_copy(&NP, &P); ECP2_ZZZ_neg(&NP);
    ECP2_ZZZ_copy(&NR, &R); ECP2_ZZZ_neg(&NR);

    FP12_YYY_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP12_YYY_sqr(r, r);
        PAIR_ZZZ_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_ZZZ_line(&lv2, &B, &B, &Sx, &Sy);
        FP12_YYY_smul(&lv, &lv2);
        FP12_YYY_ssmul(r, &lv);

        bt = BIG_XXX_bit(n3, i) - BIG_XXX_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_ZZZ_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_ZZZ_line(&lv2, &B, &R, &Sx, &Sy);
            FP12_YYY_smul(&lv, &lv2);
            FP12_YYY_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_ZZZ_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_ZZZ_line(&lv2, &B, &NR, &Sx, &Sy);
            FP12_YYY_smul(&lv, &lv2);
            FP12_YYY_ssmul(r, &lv);
        }

    }


    /* R-ate fixup required for BN curves */

#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(r, r);
#endif

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_ZZZ_neg(&A);
    ECP2_ZZZ_neg(&B);
#endif

    ECP2_ZZZ_copy(&K, &P);
    ECP2_ZZZ_frob(&K, &X);
    PAIR_ZZZ_line(&lv, &A, &K, &Qx, &Qy);
    ECP2_ZZZ_frob(&K, &X);
    ECP2_ZZZ_neg(&K);
    PAIR_ZZZ_line(&lv2, &A, &K, &Qx, &Qy);
    FP12_YYY_smul(&lv, &lv2);
    FP12_YYY_ssmul(r, &lv);

    ECP2_ZZZ_copy(&K, &R);
    ECP2_ZZZ_frob(&K, &X);
    PAIR_ZZZ_line(&lv, &B, &K, &Sx, &Sy);
    ECP2_ZZZ_frob(&K, &X);
    ECP2_ZZZ_neg(&K);
    PAIR_ZZZ_line(&lv2, &B, &K, &Sx, &Sy);
    FP12_YYY_smul(&lv, &lv2);
    FP12_YYY_ssmul(r, &lv);
#endif
}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
void PAIR_ZZZ_fexp(FP12_YYY *r)
{
    FP2_YYY X;
    BIG_XXX x;
    FP_YYY a, b;
    FP12_YYY t0, y0, y1;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    FP12_YYY y2, y3;
#endif

    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);
    FP_YYY_rcopy(&a, Fra_YYY);
    FP_YYY_rcopy(&b, Frb_YYY);
    FP2_YYY_from_FPs(&X, &a, &b);

    /* Easy part of final exp */

    FP12_YYY_inv(&t0, r);
    FP12_YYY_conj(r, r);

    FP12_YYY_mul(r, &t0);
    FP12_YYY_copy(&t0, r);

    FP12_YYY_frob(r, &X);
    FP12_YYY_frob(r, &X);
    FP12_YYY_mul(r, &t0);

    /* Hard part of final exp - see Duquesne & Ghamman eprint 2015/192.pdf */
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    FP12_YYY_pow(&t0, r, x); // t0=f^-u
#if SIGN_OF_X_ZZZ==POSITIVEX
    FP12_YYY_conj(&t0, &t0);
#endif
    FP12_YYY_usqr(&y3, &t0); // y3=t0^2
    FP12_YYY_copy(&y0, &t0);
    FP12_YYY_mul(&y0, &y3); // y0=t0*y3
    FP12_YYY_copy(&y2, &y3);
    FP12_YYY_frob(&y2, &X); // y2=y3^p
    FP12_YYY_mul(&y2, &y3); //y2=y2*y3
    FP12_YYY_usqr(&y2, &y2); //y2=y2^2
    FP12_YYY_mul(&y2, &y3); // y2=y2*y3

    FP12_YYY_pow(&t0, &y0, x); //t0=y0^-u
#if SIGN_OF_X_ZZZ==POSITIVEX
    FP12_YYY_conj(&t0, &t0);
#endif
    FP12_YYY_conj(&y0, r);    //y0=~r
    FP12_YYY_copy(&y1, &t0);
    FP12_YYY_frob(&y1, &X);
    FP12_YYY_frob(&y1, &X); //y1=t0^p^2
    FP12_YYY_mul(&y1, &y0); // y1=y0*y1
    FP12_YYY_conj(&t0, &t0); // t0=~t0
    FP12_YYY_copy(&y3, &t0);
    FP12_YYY_frob(&y3, &X); //y3=t0^p
    FP12_YYY_mul(&y3, &t0); // y3=t0*y3
    FP12_YYY_usqr(&t0, &t0); // t0=t0^2
    FP12_YYY_mul(&y1, &t0); // y1=t0*y1

    FP12_YYY_pow(&t0, &y3, x); // t0=y3^-u
#if SIGN_OF_X_ZZZ==POSITIVEX
    FP12_YYY_conj(&t0, &t0);
#endif
    FP12_YYY_usqr(&t0, &t0); //t0=t0^2
    FP12_YYY_conj(&t0, &t0); //t0=~t0
    FP12_YYY_mul(&y3, &t0); // y3=t0*y3

    FP12_YYY_frob(r, &X);
    FP12_YYY_copy(&y0, r);
    FP12_YYY_frob(r, &X);
    FP12_YYY_mul(&y0, r);
    FP12_YYY_frob(r, &X);
    FP12_YYY_mul(&y0, r);

    FP12_YYY_usqr(r, &y3); //r=y3^2
    FP12_YYY_mul(r, &y2);  //r=y2*r
    FP12_YYY_copy(&y3, r);
    FP12_YYY_mul(&y3, &y0); // y3=r*y0
    FP12_YYY_mul(r, &y1); // r=r*y1
    FP12_YYY_usqr(r, r); // r=r^2
    FP12_YYY_mul(r, &y3); // r=r*y3
    FP12_YYY_reduce(r);
#else

// See https://eprint.iacr.org/2020/875.pdf
    FP12_YYY_usqr(&y1,r);
    FP12_YYY_mul(&y1,r);     // y1=r^3

    FP12_YYY_pow(&y0,r,x);   // y0=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&y0, &y0);
#endif
    FP12_YYY_conj(&t0,r);    // t0=r^-1
    FP12_YYY_copy(r,&y0);
    FP12_YYY_mul(r,&t0);    // r=r^(x-1)

    FP12_YYY_pow(&y0,r,x);   // y0=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&y0, &y0);
#endif
    FP12_YYY_conj(&t0,r);    // t0=r^-1
    FP12_YYY_copy(r,&y0);
    FP12_YYY_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP12_YYY_pow(&y0,r,x);  // y0=r^x
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&y0, &y0);
#endif
    FP12_YYY_copy(&t0,r);   
    FP12_YYY_frob(&t0,&X); // t0=r^p
    FP12_YYY_copy(r,&y0);
    FP12_YYY_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2-1)
    FP12_YYY_pow(&y0,r,x);  
    FP12_YYY_pow(&y0,&y0,x); // y0=r^x^2
    FP12_YYY_copy(&t0,r);    
    FP12_YYY_frob(&t0,&X);
    FP12_YYY_frob(&t0,&X);   // t0=r^p^2
    FP12_YYY_mul(&y0,&t0);   // y0=r^x^2.r^p^2
    FP12_YYY_conj(&t0,r);    // t0=r^-1
    FP12_YYY_copy(r,&y0);    // 
    FP12_YYY_mul(r,&t0);     // r=r^x^2.r^p^2.r^-1

    FP12_YYY_mul(r,&y1);    
    FP12_YYY_reduce(r);

// Ghamman & Fouotsa Method
/*
    FP12_YYY_usqr(&y0, r);
    FP12_YYY_pow(&y1, &y0, x);
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&y1, &y1);
#endif

    BIG_XXX_fshr(x, 1);
    FP12_YYY_pow(&y2, &y1, x);
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&y2, &y2);
#endif

    BIG_XXX_fshl(x, 1); // x must be even
    FP12_YYY_conj(&y3, r);
    FP12_YYY_mul(&y1, &y3);

    FP12_YYY_conj(&y1, &y1);
    FP12_YYY_mul(&y1, &y2);

    FP12_YYY_pow(&y2, &y1, x);
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&y2, &y2);
#endif

    FP12_YYY_pow(&y3, &y2, x);
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&y3, &y3);
#endif
    FP12_YYY_conj(&y1, &y1);
    FP12_YYY_mul(&y3, &y1);

    FP12_YYY_conj(&y1, &y1);
    FP12_YYY_frob(&y1, &X);
    FP12_YYY_frob(&y1, &X);
    FP12_YYY_frob(&y1, &X);
    FP12_YYY_frob(&y2, &X);
    FP12_YYY_frob(&y2, &X);
    FP12_YYY_mul(&y1, &y2);

    FP12_YYY_pow(&y2, &y3, x);
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&y2, &y2);
#endif
    FP12_YYY_mul(&y2, &y0);
    FP12_YYY_mul(&y2, r);

    FP12_YYY_mul(&y1, &y2);
    FP12_YYY_copy(&y2, &y3);
    FP12_YYY_frob(&y2, &X);
    FP12_YYY_mul(&y1, &y2);
    FP12_YYY_copy(r, &y1);
    FP12_YYY_reduce(r);
*/
#endif
}

#ifdef USE_GLV_ZZZ
/* GLV method */
static void glv(BIG_XXX u[2], BIG_XXX ee)
{
    BIG_XXX q;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    int i, j;
    BIG_XXX v[2], t;
    DBIG_XXX d;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);

    for (i = 0; i < 2; i++)
    {
        BIG_XXX_rcopy(t, CURVE_W_ZZZ[i]);
        BIG_XXX_mul(d, t, ee);
        BIG_XXX_ctddiv(v[i],d,q,BIG_XXX_nbits(t));
        BIG_XXX_zero(u[i]);
    }
    BIG_XXX_copy(u[0], ee);
    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
        {
            BIG_XXX_rcopy(t, CURVE_SB_ZZZ[j][i]);
            BIG_XXX_modmul(t, v[j], t, q);
            BIG_XXX_add(u[i], u[i], q);
            BIG_XXX_sub(u[i], u[i], t);
            BIG_XXX_ctmod(u[i],q,1);
        }

#else
// -(x^2).P = (Beta.x,y)
    int bd;
    BIG_XXX x, x2;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

    BIG_XXX_smul(x2, x, x);
    bd=BIG_XXX_nbits(q)-BIG_XXX_nbits(x2); // fixed
    BIG_XXX_copy(u[0], ee);
    BIG_XXX_ctmod(u[0], x2, bd);
    BIG_XXX_copy(u[1], ee);
    BIG_XXX_ctsdiv(u[1], x2, bd);

    BIG_XXX_sub(u[1], q, u[1]);

#endif

    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void gs(BIG_XXX u[4], BIG_XXX ee)
{
    int i;
    BIG_XXX q;
#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    int j;
    BIG_XXX v[4], t;
    DBIG_XXX d;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);

    for (i = 0; i < 4; i++)
    {
        BIG_XXX_rcopy(t, CURVE_WB_ZZZ[i]);
        BIG_XXX_mul(d, t, ee);
        BIG_XXX_ctddiv(v[i],d,q,BIG_XXX_nbits(t));
        BIG_XXX_zero(u[i]);
    }

    BIG_XXX_copy(u[0], ee);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            BIG_XXX_rcopy(t, CURVE_BB_ZZZ[j][i]);
            BIG_XXX_modmul(t, v[j], t, q);
            BIG_XXX_add(u[i], u[i], q);
            BIG_XXX_sub(u[i], u[i], t);
            BIG_XXX_ctmod(u[i],q,1);
        }
#else
    int bd;
    BIG_XXX x, w;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);

    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);
    BIG_XXX_copy(w, ee);
    bd=BIG_XXX_nbits(q)-BIG_XXX_nbits(x); // fixed

    for (i = 0; i < 3; i++)
    {
        BIG_XXX_copy(u[i], w);
        BIG_XXX_ctmod(u[i],x,bd);
        BIG_XXX_ctsdiv(w,x,bd);
    }
    BIG_XXX_copy(u[3], w);

    /*  */
#if SIGN_OF_X_ZZZ==NEGATIVEX
    BIG_XXX_modneg(u[1], u[1], q);
    BIG_XXX_modneg(u[3], u[3], q);
#endif

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
void PAIR_ZZZ_G2mul(ECP2_ZZZ *P, BIG_XXX e)
{
    BIG_XXX ee,q;
#ifdef USE_GS_G2_ZZZ   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP2_ZZZ Q[4];
    FP2_YYY X;
    FP_YYY fx, fy;
    BIG_XXX x, u[4];

    FP_YYY_rcopy(&fx, Fra_YYY);
    FP_YYY_rcopy(&fy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &fx, &fy);

#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_YYY_inv(&X, &X, NULL);
    FP2_YYY_norm(&X);
#endif

    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);

    gs(u, ee);

    ECP2_ZZZ_copy(&Q[0], P);
    for (i = 1; i < 4; i++)
    {
        ECP2_ZZZ_copy(&Q[i], &Q[i - 1]);
        ECP2_ZZZ_frob(&Q[i], &X);
    }

    for (i = 0; i < 4; i++)
    {
        np = BIG_XXX_nbits(u[i]);
        BIG_XXX_modneg(x, u[i], q);
        nn = BIG_XXX_nbits(x);
        if (nn < np)
        {
            BIG_XXX_copy(u[i], x);
            ECP2_ZZZ_neg(&Q[i]);
        }
        BIG_XXX_norm(u[i]);
    }

    ECP2_ZZZ_mul4(P, Q, u);

#else
    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);
    ECP2_ZZZ_mul(P, ee);
#endif
}

/* f=f^e */
void PAIR_ZZZ_GTpow(FP12_YYY *f, BIG_XXX e)
{
    BIG_XXX ee,q;
#ifdef USE_GS_GT_ZZZ   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see fp4.c */
    int i, np, nn;
    FP12_YYY g[4];
    FP2_YYY X;
    BIG_XXX t;
    FP_YYY fx, fy;
    BIG_XXX u[4];

    FP_YYY_rcopy(&fx, Fra_YYY);
    FP_YYY_rcopy(&fy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &fx, &fy);

    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);

    gs(u, ee);

    FP12_YYY_copy(&g[0], f);
    for (i = 1; i < 4; i++)
    {
        FP12_YYY_copy(&g[i], &g[i - 1]);
        FP12_YYY_frob(&g[i], &X);
    }

    for (i = 0; i < 4; i++)
    {
        np = BIG_XXX_nbits(u[i]);
        BIG_XXX_modneg(t, u[i], q);
        nn = BIG_XXX_nbits(t);
        if (nn < np)
        {
            BIG_XXX_copy(u[i], t);
            FP12_YYY_conj(&g[i], &g[i]);
        }
        BIG_XXX_norm(u[i]);
    }
    FP12_YYY_pow4(f, g, u);

#else
    BIG_XXX_copy(ee,e);
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    BIG_XXX_mod(ee,q);
    FP12_YYY_pow(f, f, ee);
#endif
}


/* test G1 group membership */

int PAIR_ZZZ_G1member(ECP_ZZZ *P)
{
    ECP_ZZZ W,T;
    BIG_XXX x;
    FP_YYY cru;
    if (ECP_ZZZ_isinf(P)) return 0;
#if PAIRING_FRIENDLY_ZZZ!=BN_CURVE
    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);
    ECP_ZZZ_copy(&W,P);
    ECP_ZZZ_copy(&T,P);
    ECP_ZZZ_mul(&T,x);
    if (ECP_ZZZ_equals(P,&T)) return 0;    // P is of low order   
    ECP_ZZZ_mul(&T,x);
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
#if PAIRING_FRIENDLY_ZZZ!=BN_CURVE
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
	ECP_ZZZ_copy(&W,P);
	ECP_ZZZ_mul(&W,q);
	if (!ECP_ZZZ_isinf(&W)) return 0; */
#endif 
	return 1;
}

/* test G2 group membership */

int PAIR_ZZZ_G2member(ECP2_ZZZ *P)
{
    ECP2_ZZZ W,T;
    BIG_XXX x;
    FP2_YYY X;
    FP_YYY fx, fy;

    if (ECP2_ZZZ_isinf(P)) return 0;
    FP_YYY_rcopy(&fx, Fra_YYY);
    FP_YYY_rcopy(&fy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &fx, &fy);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP2_YYY_inv(&X, &X, NULL);
    FP2_YYY_norm(&X);
#endif
    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

    ECP2_ZZZ_copy(&T,P);
    ECP2_ZZZ_mul(&T,x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP2_ZZZ_neg(&T);
#endif

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
//https://eprint.iacr.org/2022/348.pdf
    ECP2_ZZZ_copy(&W,&T);
    ECP2_ZZZ_frob(&W,&X); // W=\psi(xP)
    ECP2_ZZZ_add(&T,P); // T=xP+P
    ECP2_ZZZ_add(&T,&W); // T=xP+P+\psi(xP)
    ECP2_ZZZ_frob(&W,&X); // W=\psi^2(xP)
    ECP2_ZZZ_add(&T,&W); // T=xp+P+\psi(xP)+\psi^2(xP)
    ECP2_ZZZ_frob(&W,&X); // W=\psi^3(xP)
    ECP2_ZZZ_dbl(&W); // W=\psi^3(2xP)
#else
//https://eprint.iacr.org/2021/1130
    ECP2_ZZZ_copy(&W,P);
    ECP2_ZZZ_frob(&W, &X);    // W=\psi(P)    
#endif

    if (ECP2_ZZZ_equals(&W,&T)) return 1;
    return 0;
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^4}=m^{p^2} */
int PAIR_ZZZ_GTcyclotomic(FP12_YYY *m)
{
	FP_YYY fx,fy;
	FP2_YYY X;
	FP12_YYY r,w;
	if (FP12_YYY_isunity(m)) return 0;
	FP12_YYY_conj(&r,m);
	FP12_YYY_mul(&r,m);
	if (!FP12_YYY_isunity(&r)) return 0;

	FP_YYY_rcopy(&fx,Fra_YYY);
	FP_YYY_rcopy(&fy,Frb_YYY);
	FP2_YYY_from_FPs(&X,&fx,&fy);

	FP12_YYY_copy(&r,m); FP12_YYY_frob(&r,&X); FP12_YYY_frob(&r,&X);
	FP12_YYY_copy(&w,&r); FP12_YYY_frob(&w,&X); FP12_YYY_frob(&w,&X);
	FP12_YYY_mul(&w,m);

	if (!FP12_YYY_equals(&w,&r)) return 0;
    return 1;
}

/* test for full GT group membership */
int PAIR_ZZZ_GTmember(FP12_YYY *m)
{
    BIG_XXX x;
    FP2_YYY X;
    FP_YYY fx, fy;
    FP12_YYY r,t;
    if (!PAIR_ZZZ_GTcyclotomic(m)) return 0;

    FP_YYY_rcopy(&fx, Fra_YYY);
    FP_YYY_rcopy(&fy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &fx, &fy);
    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

    FP12_YYY_copy(&r,m);
    FP12_YYY_frob(&r, &X);

    FP12_YYY_pow(&t,m,x);

#if PAIRING_FRIENDLY_ZZZ==BN_CURVE
    FP12_YYY_pow(&t,&t,x);
    BIG_XXX_zero(x); BIG_XXX_inc(x,6);
    FP12_YYY_pow(&t,&t,x);
#else
#if SIGN_OF_X_ZZZ==NEGATIVEX
    FP12_YYY_conj(&t,&t);
#endif
#endif

    if (FP12_YYY_equals(&r,&t)) return 1;
    return 0;

/*	BIG_XXX q;
	FP12_YYY r;
    if (!PAIR_ZZZ_GTcyclotomic(m)) return 0;

    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);
    FP12_YYY_pow(&r,m,q);
	if (!FP12_YYY_isunity(&r)) return 0;
	return 1;
*/
}

#ifdef HAS_MAIN

int main()
{
    int i;
    char byt[32];
    csprng rng;
    BIG_XXX xa, xb, ya, yb, w, a, b, t1, q, u[2], v[4], m, r;
    ECP2_ZZZ P, G;
    ECP_ZZZ Q, R;
    FP12_YYY g, gp;
    FP4_YYY t, c, cp, cpm1, cpm2;
    FP2_YYY x, y, X;


    BIG_XXX_rcopy(a, CURVE_Fra);
    BIG_XXX_rcopy(b, CURVE_Frb);
    FP2_YYY_from_BIGs(&X, a, b);

    BIG_XXX_rcopy(xa, CURVE_Gx);
    BIG_XXX_rcopy(ya, CURVE_Gy);

    ECP_ZZZ_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_ZZZ_output(&Q);
    printf("\n");

    BIG_XXX_rcopy(xa, CURVE_Pxa);
    BIG_XXX_rcopy(xb, CURVE_Pxb);
    BIG_XXX_rcopy(ya, CURVE_Pya);
    BIG_XXX_rcopy(yb, CURVE_Pyb);

    FP2_YYY_from_BIGs(&x, xa, xb);
    FP2_YYY_from_BIGs(&y, ya, yb);

    ECP2_ZZZ_set(&P, &x, &y);
    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    printf("P= ");
    ECP2_ZZZ_output(&P);
    printf("\n");

    for (i = 0; i < 1000; i++ )
    {
        PAIR_ZZZ_ate(&g, &P, &Q);
        PAIR_ZZZ_fexp(&g);
    }
    printf("g= ");
    FP12_YYY_output(&g);
    printf("\n");
}

#endif
