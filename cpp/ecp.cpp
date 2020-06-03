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

/* CORE Elliptic Curve Functions */
/* SU=m, SU is Stack Usage (Weierstrass Curves) */

//#define HAS_MAIN

#include "ecp_ZZZ.h"

using namespace XXX;
using namespace YYY;

/* test for P=O point-at-infinity */
int ZZZ::ECP_isinf(ECP *P)
{

#if CURVETYPE_ZZZ==EDWARDS
    return (FP_iszilch(&(P->x)) && FP_equals(&(P->y), &(P->z)));
#endif
#if CURVETYPE_ZZZ==WEIERSTRASS
    return (FP_iszilch(&(P->x)) && FP_iszilch(&(P->z)));
#endif
#if CURVETYPE_ZZZ==MONTGOMERY
    return FP_iszilch(&(P->z));
#endif

}

/* Conditional swap of P and Q dependant on d */
static void ECP_cswap(ZZZ::ECP *P, ZZZ::ECP *Q, int d)
{
    FP_cswap(&(P->x), &(Q->x), d);
#if CURVETYPE_ZZZ!=MONTGOMERY
    FP_cswap(&(P->y), &(Q->y), d);
#endif
    FP_cswap(&(P->z), &(Q->z), d);

}

#if CURVETYPE_ZZZ!=MONTGOMERY
/* Conditional move Q to P dependant on d */
static void ECP_cmove(ZZZ::ECP *P, ZZZ::ECP *Q, int d)
{
    FP_cmove(&(P->x), &(Q->x), d);
#if CURVETYPE_ZZZ!=MONTGOMERY
    FP_cmove(&(P->y), &(Q->y), d);
#endif
    FP_cmove(&(P->z), &(Q->z), d);

}

/* return 1 if b==c, no branching */
static int teq(sign32 b, sign32 c)
{
    sign32 x = b ^ c;
    x -= 1; // if x=0, x now -1
    return (int)((x >> 31) & 1);
}
#endif // CURVETYPE_ZZZ!=MONTGOMERY

#if CURVETYPE_ZZZ!=MONTGOMERY
/* Constant time select from pre-computed table */
static void ECP_select(ZZZ::ECP *P, ZZZ::ECP W[], sign32 b)
{
    ZZZ::ECP MP;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    ECP_cmove(P, &W[0], teq(babs, 0)); // conditional move
    ECP_cmove(P, &W[1], teq(babs, 1));
    ECP_cmove(P, &W[2], teq(babs, 2));
    ECP_cmove(P, &W[3], teq(babs, 3));
    ECP_cmove(P, &W[4], teq(babs, 4));
    ECP_cmove(P, &W[5], teq(babs, 5));
    ECP_cmove(P, &W[6], teq(babs, 6));
    ECP_cmove(P, &W[7], teq(babs, 7));

    ECP_copy(&MP, P);
    ECP_neg(&MP);  // minus P
    ECP_cmove(P, &MP, (int)(m & 1));
}
#endif

/* Test P == Q */
/* SU=168 */
int ZZZ::ECP_equals(ECP *P, ECP *Q)
{
    FP a, b;

    FP_mul(&a, &(P->x), &(Q->z));
    FP_mul(&b, &(Q->x), &(P->z));
    if (!FP_equals(&a, &b)) return 0;

#if CURVETYPE_ZZZ!=MONTGOMERY
    FP_mul(&a, &(P->y), &(Q->z));
    FP_mul(&b, &(Q->y), &(P->z));
    if (!FP_equals(&a, &b)) return 0;
#endif

    return 1;

}

/* Set P=Q */
/* SU=16 */
void ZZZ::ECP_copy(ECP *P, ECP *Q)
{
    FP_copy(&(P->x), &(Q->x));
#if CURVETYPE_ZZZ!=MONTGOMERY
    FP_copy(&(P->y), &(Q->y));
#endif
    FP_copy(&(P->z), &(Q->z));
}

/* Set P=-Q */
#if CURVETYPE_ZZZ!=MONTGOMERY
/* SU=8 */
void ZZZ::ECP_neg(ECP *P)
{
#if CURVETYPE_ZZZ==WEIERSTRASS
    FP_neg(&(P->y), &(P->y));
    FP_norm(&(P->y));
#else
    FP_neg(&(P->x), &(P->x));
    FP_norm(&(P->x));
#endif

}
#endif

/* Set P=O */
void ZZZ::ECP_inf(ECP *P)
{
    FP_zero(&(P->x));
#if CURVETYPE_ZZZ!=MONTGOMERY
    FP_one(&(P->y));
#endif
#if CURVETYPE_ZZZ!=EDWARDS
    FP_zero(&(P->z));
#else
    FP_one(&(P->z));
#endif
}

/* Calculate right Hand Side of curve equation y^2=RHS */
/* SU=56 */
void ZZZ::ECP_rhs(FP *v, FP *x)
{
#if CURVETYPE_ZZZ==WEIERSTRASS
    /* x^3+Ax+B */
    FP t;
    FP_sqr(&t, x);
    FP_mul(&t, &t, x);

    if (CURVE_A == -3)
    {
        FP_neg(v, x);
        FP_norm(v);
        FP_imul(v, v, -CURVE_A);
        FP_norm(v);
        FP_add(v, &t, v);
    }
    else FP_copy(v, &t);

    FP_rcopy(&t, CURVE_B);

    FP_add(v, &t, v);
    FP_reduce(v);
#endif

#if CURVETYPE_ZZZ==EDWARDS
    /* (Ax^2-1)/(Bx^2-1) */
    FP t, one;
    FP_sqr(v, x);
    FP_one(&one);
    FP_rcopy(&t, CURVE_B);

    FP_mul(&t, v, &t);
    FP_sub(&t, &t, &one);
    FP_norm(&t);
    if (CURVE_A == 1) FP_sub(v, v, &one);

    if (CURVE_A == -1)
    {
        FP_add(v, v, &one);
        FP_norm(v);
        FP_neg(v, v);
    }
    FP_norm(v);
    FP_inv(&t, &t);
    FP_mul(v, v, &t);
    FP_reduce(v);
#endif

#if CURVETYPE_ZZZ==MONTGOMERY
    /* x^3+Ax^2+x */
    FP x2, x3;
    FP_sqr(&x2, x);
    FP_mul(&x3, &x2, x);
    FP_copy(v, x);
    FP_imul(&x2, &x2, CURVE_A);
    FP_add(v, v, &x2);
    FP_add(v, v, &x3);
    FP_reduce(v);
#endif
}

/* Set P=(x,y) */

#if CURVETYPE_ZZZ==MONTGOMERY

/* Set P=(x,{y}) */

int ZZZ::ECP_set(ECP *P, BIG x)
{
    FP rhs;
    FP_nres(&rhs, x);

    ECP_rhs(&rhs, &rhs);

    if (!FP_qr(&rhs,NULL))
    {
        ECP_inf(P);
        return 0;
    }

    FP_nres(&(P->x), x);
    FP_one(&(P->z));
    return 1;
}

/* Extract x coordinate as BIG */
int ZZZ::ECP_get(BIG x, ECP *P)
{
    ECP W;
    ECP_copy(&W, P);
    ECP_affine(&W);
    if (ECP_isinf(&W)) return -1;
    FP_redc(x, &(W.x));
    return 0;
}


#else
/* Extract (x,y) and return sign of y. If x and y are the same return only x */
/* SU=16 */
int ZZZ::ECP_get(BIG x, BIG y, ECP *P)
{
    int s;
    ECP W;
    ECP_copy(&W, P);
    ECP_affine(&W);
    if (ECP_isinf(&W)) return -1;

    FP_redc(y, &(W.y));
    s = BIG_parity(y);

    FP_redc(x, &(W.x));

    return s;
}

/* Set P=(x,{y}) */
/* SU=96 */
int ZZZ::ECP_set(ECP *P, BIG x, BIG y)
{
    FP rhs, y2;

    FP_nres(&y2, y);
    FP_sqr(&y2, &y2);
    FP_reduce(&y2);

    FP_nres(&rhs, x);
    ECP_rhs(&rhs, &rhs);

    if (!FP_equals(&y2, &rhs))
    {
        ECP_inf(P);
        return 0;
    }

    FP_nres(&(P->x), x);
    FP_nres(&(P->y), y);
    FP_one(&(P->z));
    return 1;
}

/* Set P=(x,y), where y is calculated from x with sign s */
/* SU=136 */
int ZZZ::ECP_setx(ECP *P, BIG x, int s)
{
    FP rhs,hint;
    BIG t;

    FP_nres(&rhs, x);

    ECP_rhs(&rhs, &rhs);

    if (!FP_qr(&rhs,&hint))
    {
        ECP_inf(P);
        return 0;
    }

    FP_nres(&(P->x), x);
    FP_sqrt(&(P->y), &rhs, &hint);

    FP_redc(t, &(P->y));

    if (BIG_parity(t) != s)
        FP_neg(&(P->y), &(P->y));
    FP_reduce(&(P->y));
    FP_one(&(P->z));
    return 1;
}

#endif

/* Convert P to Affine, from (x,y,z) to (x,y) */
/* SU=160 */
void ZZZ::ECP_affine(ECP *P)
{
    FP one, iz;
    if (ECP_isinf(P)) return;
    FP_one(&one);
    if (FP_equals(&(P->z), &one)) return;

    FP_inv(&iz, &(P->z));
    FP_mul(&(P->x), &(P->x), &iz);

#if CURVETYPE_ZZZ==EDWARDS || CURVETYPE_ZZZ==WEIERSTRASS

    FP_mul(&(P->y), &(P->y), &iz);
    FP_reduce(&(P->y));

#endif

    FP_reduce(&(P->x));
    FP_copy(&(P->z), &one);
}

/* SU=120 */
void ZZZ::ECP_outputxyz(ECP *P)
{
    BIG x, z;
    if (ECP_isinf(P))
    {
        printf("Infinity\n");
        return;
    }
    FP_reduce(&(P->x));
    FP_redc(x, &(P->x));
    FP_reduce(&(P->z));
    FP_redc(z, &(P->z));

#if CURVETYPE_ZZZ!=MONTGOMERY
    BIG y;
    FP_reduce(&(P->y));
    FP_redc(y, &(P->y));
    printf("(");
    BIG_output(x);
    printf(",");
    BIG_output(y);
    printf(",");
    BIG_output(z);
    printf(")\n");

#else
    printf("(");
    BIG_output(x);
    printf(",");
    BIG_output(z);
    printf(")\n");
#endif
}

/* SU=16 */
/* Output point P */
void ZZZ::ECP_output(ECP *P)
{
    BIG x, y;
    if (ECP_isinf(P))
    {
        printf("Infinity\n");
        return;
    }
    ECP_affine(P);
#if CURVETYPE_ZZZ!=MONTGOMERY
    FP_reduce(&(P->x));
    FP_reduce(&(P->y));
    FP_redc(x, &(P->x));
    FP_redc(y, &(P->y));
    printf("(");
    BIG_output(x);
    printf(",");
    BIG_output(y);
    printf(")\n");
#else
    FP_reduce(&(P->x));
    FP_redc(x, &(P->x));
    printf("(");
    BIG_output(x);
    printf(")\n");
#endif
}

/* SU=16 */
/* Output point P */
void ZZZ::ECP_rawoutput(ECP *P)
{
    BIG x, y, z;

#if CURVETYPE_ZZZ!=MONTGOMERY
    FP_redc(x, &(P->x));
    FP_redc(y, &(P->y));
    FP_redc(z, &(P->z));
    printf("(");
    BIG_output(x);
    printf(",");
    BIG_output(y);
    printf(",");
    BIG_output(z);
    printf(")\n");
#else
    FP_redc(x, &(P->x));
    FP_redc(z, &(P->z));
    printf("(");
    BIG_output(x);
    printf(",");
    BIG_output(z);
    printf(")\n");
#endif
}

/* SU=88 */
/* Convert P to octet string, compressing if desired */
void ZZZ::ECP_toOctet(octet *W, ECP *P, bool compress)
{
#if CURVETYPE_ZZZ==MONTGOMERY
    BIG x;
    ECP_get(x, P);
    W->len = MODBYTES_XXX; // + 1;
    //W->val[0] = 0x06;
    BIG_toBytes(&(W->val[0]), x);
#else
    BIG x, y;
    ECP_get(x, y, P);
    if (compress)
    {
        W->val[0] = 0x02;
        if (BIG_parity(y) == 1) W->val[0] = 0x03;
        W->len = MODBYTES_XXX + 1;
        BIG_toBytes(&(W->val[1]), x);
    }
    else
    {
        W->val[0] = 0x04;
        W->len = 2 * MODBYTES_XXX + 1;
        BIG_toBytes(&(W->val[1]), x);
        BIG_toBytes(&(W->val[MODBYTES_XXX + 1]), y);
    }
#endif
}

/* SU=88 */
/* Restore P from octet string */
int ZZZ::ECP_fromOctet(ECP *P, octet *W)
{
#if CURVETYPE_ZZZ==MONTGOMERY
    BIG x;
    BIG_fromBytes(x, &(W->val[0]));
    if (ECP_set(P, x)) return 1;
    return 0;
#else
    BIG x, y;
    int typ = W->val[0];
    BIG_fromBytes(x, &(W->val[1]));

    if (typ == 0x04)
    {
        BIG_fromBytes(y, &(W->val[MODBYTES_XXX + 1]));
        if (ECP_set(P, x, y)) return 1;
    }
    if (typ == 0x02 || typ == 0x03)
    {
        if (ECP_setx(P, x, typ & 1)) return 1;
    }
    return 0;
#endif
}


/* Set P=2P */
/* SU=272 */
void ZZZ::ECP_dbl(ECP *P)
{
#if CURVETYPE_ZZZ==WEIERSTRASS
    FP t0, t1, t2, t3, x3, y3, z3, b;

//    if (ECP_isinf(P)) return;

    if (CURVE_A == 0)
    {   // 2S + 6M
        FP_sqr(&t0, &(P->y));                   //t0.sqr();
        FP_mul(&t1, &(P->y), &(P->z));          //t1.mul(z);

        FP_sqr(&t2, &(P->z));                   //t2.sqr();

        FP_add(&(P->z), &t0, &t0);          //z.add(t0);
        FP_norm(&(P->z));                   //z.norm();
        FP_add(&(P->z), &(P->z), &(P->z));  //z.add(z);
        FP_add(&(P->z), &(P->z), &(P->z));  //z.add(z);
        FP_norm(&(P->z));                   //z.norm();

        FP_imul(&t2, &t2, 3 * CURVE_B_I);   //t2.imul(3*ROM.CURVE_B_I);
        FP_mul(&x3, &t2, &(P->z));          //x3.mul(z);

        FP_add(&y3, &t0, &t2);              //y3.add(t2);
        FP_norm(&y3);                       //y3.norm();
        FP_mul(&(P->z), &(P->z), &t1);      //z.mul(t1);

        FP_add(&t1, &t2, &t2);              //t1.add(t2);
        FP_add(&t2, &t2, &t1);              //t2.add(t1);
        FP_sub(&t0, &t0, &t2);              //t0.sub(t2);
        FP_norm(&t0);                       //t0.norm();
        FP_mul(&y3, &y3, &t0);              //y3.mul(t0);
        FP_add(&y3, &y3, &x3);              //y3.add(x3);

        FP_mul(&t1, &(P->x), &(P->y));          //t1.mul(y);
        FP_norm(&t0);                   //x.norm();
        FP_mul(&(P->x), &t0, &t1);      //x.mul(t1);
        FP_add(&(P->x), &(P->x), &(P->x));  //x.add(x);
        FP_norm(&(P->x));                   //x.norm();
        FP_copy(&(P->y), &y3);              //y.copy(y3);
        FP_norm(&(P->y));                   //y.norm();
    }
    else // its -3
    {   // 3S+10M

        if (CURVE_B_I == 0)                 //if (ROM.CURVE_B_I==0)
            FP_rcopy(&b, CURVE_B);          //b.copy(new FP(new BIG(ROM.CURVE_B)));

        FP_sqr(&t0, &(P->x));                   //t0.sqr();  //1    x^2
        FP_sqr(&t1, &(P->y));                   //t1.sqr();  //2    y^2
        FP_sqr(&t2, &(P->z));                   //t2.sqr();  //3

        FP_mul(&t3, &(P->x), &(P->y));          //t3.mul(y); //4
        FP_add(&t3, &t3, &t3);              //t3.add(t3);
        FP_norm(&t3);                       //t3.norm();//5

        FP_mul(&z3, &(P->z), &(P->x));          //z3.mul(x);   //6
        FP_add(&z3, &z3, &z3);              //z3.add(z3);
        FP_norm(&z3);                       //z3.norm();//7

        if (CURVE_B_I == 0)                     //if (ROM.CURVE_B_I==0)
            FP_mul(&y3, &t2, &b);           //y3.mul(b); //8
        else
            FP_imul(&y3, &t2, CURVE_B_I);   //y3.imul(ROM.CURVE_B_I);

        FP_sub(&y3, &y3, &z3);              //y3.sub(z3); //y3.norm(); //9  ***
        FP_add(&x3, &y3, &y3);              //x3.add(y3);
        FP_norm(&x3);                       //x3.norm();//10

        FP_add(&y3, &y3, &x3);              //y3.add(x3); //y3.norm();//11
        FP_sub(&x3, &t1, &y3);              //x3.sub(y3);
        FP_norm(&x3);                       //x3.norm();//12
        FP_add(&y3, &y3, &t1);              //y3.add(t1);
        FP_norm(&y3);                       //y3.norm();//13
        FP_mul(&y3, &y3, &x3);              //y3.mul(x3); //14
        FP_mul(&x3, &x3, &t3);              //x3.mul(t3); //15
        FP_add(&t3, &t2, &t2);              //t3.add(t2);  //16
        FP_add(&t2, &t2, &t3);              //t2.add(t3); //17

        if (CURVE_B_I == 0)                 //if (ROM.CURVE_B_I==0)
            FP_mul(&z3, &z3, &b);           //z3.mul(b); //18
        else
            FP_imul(&z3, &z3, CURVE_B_I); //z3.imul(ROM.CURVE_B_I);

        FP_sub(&z3, &z3, &t2);              //z3.sub(t2); //z3.norm();//19
        FP_sub(&z3, &z3, &t0);              //z3.sub(t0);
        FP_norm(&z3);                       //z3.norm();//20  ***
        FP_add(&t3, &z3, &z3);              //t3.add(z3); //t3.norm();//21

        FP_add(&z3, &z3, &t3);              //z3.add(t3);
        FP_norm(&z3);                       //z3.norm(); //22
        FP_add(&t3, &t0, &t0);              //t3.add(t0); //t3.norm(); //23
        FP_add(&t0, &t0, &t3);              //t0.add(t3); //t0.norm();//24
        FP_sub(&t0, &t0, &t2);              //t0.sub(t2);
        FP_norm(&t0);                       //t0.norm();//25

        FP_mul(&t0, &t0, &z3);              //t0.mul(z3);//26
        FP_add(&y3, &y3, &t0);              //y3.add(t0); //y3.norm();//27
        FP_mul(&t0, &(P->y), &(P->z));          //t0.mul(z);//28
        FP_add(&t0, &t0, &t0);              //t0.add(t0);
        FP_norm(&t0);                       //t0.norm(); //29
        FP_mul(&z3, &z3, &t0);              //z3.mul(t0);//30
        FP_sub(&(P->x), &x3, &z3);              //x3.sub(z3); //x3.norm();//31
        FP_add(&t0, &t0, &t0);              //t0.add(t0);
        FP_norm(&t0);                       //t0.norm();//32
        FP_add(&t1, &t1, &t1);              //t1.add(t1);
        FP_norm(&t1);                       //t1.norm();//33
        FP_mul(&(P->z), &t0, &t1);              //z3.mul(t1);//34

        FP_norm(&(P->x));                   //x.norm();
        FP_copy(&(P->y), &y3);              //y.copy(y3);
        FP_norm(&(P->y));                   //y.norm();
        FP_norm(&(P->z));                   //z.norm();
    }
#endif

#if CURVETYPE_ZZZ==EDWARDS
    /* Not using square for multiplication swap, as (1) it needs more adds, and (2) it triggers more reductions */

    FP C, D, H, J;

    FP_sqr(&C, &(P->x));                    //C.sqr();
    FP_mul(&(P->x), &(P->x), &(P->y));      //x.mul(y);
    FP_add(&(P->x), &(P->x), &(P->x));      //x.add(x);
    FP_norm(&(P->x));                       //x.norm();

    FP_sqr(&D, &(P->y));                        //D.sqr();

    if (CURVE_A == -1)              //if (ROM.CURVE_A==-1)
        FP_neg(&C, &C);             //  C.neg();

    FP_add(&(P->y), &C, &D);    //y.add(D);
    FP_norm(&(P->y));               //y.norm();
    FP_sqr(&H, &(P->z));                //H.sqr();
    FP_add(&H, &H, &H);             //H.add(H);

    FP_sub(&J, &(P->y), &H);            //J.sub(H);
    FP_norm(&J);                    //J.norm();

    FP_mul(&(P->x), &(P->x), &J);   //x.mul(J);
    FP_sub(&C, &C, &D);             //C.sub(D);
    FP_norm(&C);                    //C.norm();
    FP_mul(&(P->z), &(P->y), &J);   //z.mul(J);
    FP_mul(&(P->y), &(P->y), &C);   //y.mul(C);


#endif

#if CURVETYPE_ZZZ==MONTGOMERY
    FP A, B, AA, BB, C;

    FP_add(&A, &(P->x), &(P->z));   //A.add(z);
    FP_norm(&A);                    //A.norm();
    FP_sqr(&AA, &A);                //AA.sqr();
    FP_sub(&B, &(P->x), &(P->z));   //B.sub(z);
    FP_norm(&B);                    //B.norm();
    FP_sqr(&BB, &B);                //BB.sqr();
    FP_sub(&C, &AA, &BB);           //C.sub(BB);
    FP_norm(&C);                    //C.norm();
    FP_mul(&(P->x), &AA, &BB);      //x.mul(BB);

    FP_imul(&A, &C, (CURVE_A + 2) / 4); //A.imul((ROM.CURVE_A+2)/4);

    FP_add(&BB, &BB, &A);           //BB.add(A);
    FP_norm(&BB);                   //BB.norm();
    FP_mul(&(P->z), &BB, &C);   //z.mul(C);

#endif
}

#if CURVETYPE_ZZZ==MONTGOMERY

/* Set P+=Q. W is difference between P and Q and is affine */
void ZZZ::ECP_add(ECP *P, ECP *Q, ECP *W)
{
    FP A, B, C, D, DA, CB;

    FP_add(&A, &(P->x), &(P->z)); //A.add(z);
    FP_sub(&B, &(P->x), &(P->z)); //B.sub(z);

    FP_add(&C, &(Q->x), &(Q->z)); //C.add(Q.z);
    FP_sub(&D, &(Q->x), &(Q->z)); //D.sub(Q.z);

    FP_norm(&A);            //A.norm();
    FP_norm(&D);            //D.norm();
    FP_mul(&DA, &D, &A);    //DA.mul(A);

    FP_norm(&C);            //C.norm();
    FP_norm(&B);            //B.norm();
    FP_mul(&CB, &C, &B);    //CB.mul(B);

    FP_add(&A, &DA, &CB);   //A.add(CB);
    FP_norm(&A);            //A.norm();
    FP_sqr(&(P->x), &A);    //A.sqr();
    FP_sub(&B, &DA, &CB);   //B.sub(CB);
    FP_norm(&B);            //B.norm();
    FP_sqr(&B, &B);         //B.sqr();

    FP_mul(&(P->z), &(W->x), &B); //z.mul(B);
}

#else

/* Set P+=Q */
/* SU=248 */
void ZZZ::ECP_add(ECP *P, ECP *Q)
{
#if CURVETYPE_ZZZ==WEIERSTRASS

    int b3;
    FP t0, t1, t2, t3, t4, x3, y3, z3, b;

    if (CURVE_A == 0)
    {
        b3 = 3 * CURVE_B_I;             //int b=3*ROM.CURVE_B_I;
        FP_mul(&t0, &(P->x), &(Q->x));      //t0.mul(Q.x);
        FP_mul(&t1, &(P->y), &(Q->y));      //t1.mul(Q.y);
        FP_mul(&t2, &(P->z), &(Q->z));      //t2.mul(Q.z);
        FP_add(&t3, &(P->x), &(P->y));      //t3.add(y);
        FP_norm(&t3);                   //t3.norm();
        FP_add(&t4, &(Q->x), &(Q->y));      //t4.add(Q.y);
        FP_norm(&t4);                   //t4.norm();
        FP_mul(&t3, &t3, &t4);          //t3.mul(t4);
        FP_add(&t4, &t0, &t1);          //t4.add(t1);

        FP_sub(&t3, &t3, &t4);          //t3.sub(t4);
        FP_norm(&t3);                   //t3.norm();
        FP_add(&t4, &(P->y), &(P->z));      //t4.add(z);
        FP_norm(&t4);                   //t4.norm();
        FP_add(&x3, &(Q->y), &(Q->z));      //x3.add(Q.z);
        FP_norm(&x3);                   //x3.norm();

        FP_mul(&t4, &t4, &x3);          //t4.mul(x3);
        FP_add(&x3, &t1, &t2);          //x3.add(t2);

        FP_sub(&t4, &t4, &x3);          //t4.sub(x3);
        FP_norm(&t4);                   //t4.norm();
        FP_add(&x3, &(P->x), &(P->z));      //x3.add(z);
        FP_norm(&x3);                   //x3.norm();
        FP_add(&y3, &(Q->x), &(Q->z));      //y3.add(Q.z);
        FP_norm(&y3);                   //y3.norm();
        FP_mul(&x3, &x3, &y3);          //x3.mul(y3);
        FP_add(&y3, &t0, &t2);          //y3.add(t2);
        FP_sub(&y3, &x3, &y3);          //y3.rsub(x3);
        FP_norm(&y3);                   //y3.norm();
        FP_add(&x3, &t0, &t0);          //x3.add(t0);
        FP_add(&t0, &t0, &x3);          //t0.add(x3);
        FP_norm(&t0);                   //t0.norm();
        FP_imul(&t2, &t2, b3);          //t2.imul(b);

        FP_add(&z3, &t1, &t2);          //z3.add(t2);
        FP_norm(&z3);                   //z3.norm();
        FP_sub(&t1, &t1, &t2);          //t1.sub(t2);
        FP_norm(&t1);                   //t1.norm();
        FP_imul(&y3, &y3, b3);          //y3.imul(b);

        FP_mul(&x3, &y3, &t4);          //x3.mul(t4);
        FP_mul(&t2, &t3, &t1);          //t2.mul(t1);
        FP_sub(&(P->x), &t2, &x3);          //x3.rsub(t2);
        FP_mul(&y3, &y3, &t0);          //y3.mul(t0);
        FP_mul(&t1, &t1, &z3);          //t1.mul(z3);
        FP_add(&(P->y), &y3, &t1);          //y3.add(t1);
        FP_mul(&t0, &t0, &t3);          //t0.mul(t3);
        FP_mul(&z3, &z3, &t4);          //z3.mul(t4);
        FP_add(&(P->z), &z3, &t0);          //z3.add(t0);

        FP_norm(&(P->x));               //x.norm();
        FP_norm(&(P->y));               //y.norm();
        FP_norm(&(P->z));               //z.norm();
    }
    else
    {

        if (CURVE_B_I == 0)             //if (ROM.CURVE_B_I==0)
            FP_rcopy(&b, CURVE_B);  //b.copy(new FP(new BIG(ROM.CURVE_B)));

        FP_mul(&t0, &(P->x), &(Q->x));      //t0.mul(Q.x); //1
        FP_mul(&t1, &(P->y), &(Q->y));      //t1.mul(Q.y); //2
        FP_mul(&t2, &(P->z), &(Q->z));      //t2.mul(Q.z); //3

        FP_add(&t3, &(P->x), &(P->y));      //t3.add(y);
        FP_norm(&t3);                   //t3.norm(); //4
        FP_add(&t4, &(Q->x), &(Q->y));      //t4.add(Q.y);
        FP_norm(&t4);                   //t4.norm();//5
        FP_mul(&t3, &t3, &t4);          //t3.mul(t4);//6
        FP_add(&t4, &t0, &t1);          //t4.add(t1); //t4.norm(); //7
        FP_sub(&t3, &t3, &t4);          //t3.sub(t4);
        FP_norm(&t3);                   //t3.norm(); //8
        FP_add(&t4, &(P->y), &(P->z));      //t4.add(z);
        FP_norm(&t4);                   //t4.norm();//9
        FP_add(&x3, &(Q->y), &(Q->z));      //x3.add(Q.z);
        FP_norm(&x3);                   //x3.norm();//10
        FP_mul(&t4, &t4, &x3);          //t4.mul(x3); //11
        FP_add(&x3, &t1, &t2);          //x3.add(t2); //x3.norm();//12

        FP_sub(&t4, &t4, &x3);          //t4.sub(x3);
        FP_norm(&t4);                   //t4.norm();//13
        FP_add(&x3, &(P->x), &(P->z));      //x3.add(z);
        FP_norm(&x3);                   //x3.norm(); //14
        FP_add(&y3, &(Q->x), &(Q->z));      //y3.add(Q.z);
        FP_norm(&y3);                   //y3.norm();//15

        FP_mul(&x3, &x3, &y3);          //x3.mul(y3); //16
        FP_add(&y3, &t0, &t2);          //y3.add(t2); //y3.norm();//17

        FP_sub(&y3, &x3, &y3);          //y3.rsub(x3);
        FP_norm(&y3);                   //y3.norm(); //18

        if (CURVE_B_I == 0)             //if (ROM.CURVE_B_I==0)
            FP_mul(&z3, &t2, &b);       //z3.mul(b); //18
        else
            FP_imul(&z3, &t2, CURVE_B_I); //z3.imul(ROM.CURVE_B_I);

        FP_sub(&x3, &y3, &z3);          //x3.sub(z3);
        FP_norm(&x3);                   //x3.norm(); //20
        FP_add(&z3, &x3, &x3);          //z3.add(x3); //z3.norm(); //21

        FP_add(&x3, &x3, &z3);          //x3.add(z3); //x3.norm(); //22
        FP_sub(&z3, &t1, &x3);          //z3.sub(x3);
        FP_norm(&z3);                   //z3.norm(); //23
        FP_add(&x3, &x3, &t1);          //x3.add(t1);
        FP_norm(&x3);                   //x3.norm(); //24

        if (CURVE_B_I == 0)             //if (ROM.CURVE_B_I==0)
            FP_mul(&y3, &y3, &b);       //y3.mul(b); //18
        else
            FP_imul(&y3, &y3, CURVE_B_I); //y3.imul(ROM.CURVE_B_I);

        FP_add(&t1, &t2, &t2);          //t1.add(t2); //t1.norm();//26
        FP_add(&t2, &t2, &t1);          //t2.add(t1); //t2.norm();//27

        FP_sub(&y3, &y3, &t2);          //y3.sub(t2); //y3.norm(); //28

        FP_sub(&y3, &y3, &t0);          //y3.sub(t0);
        FP_norm(&y3);                   //y3.norm(); //29
        FP_add(&t1, &y3, &y3);          //t1.add(y3); //t1.norm();//30
        FP_add(&y3, &y3, &t1);          //y3.add(t1);
        FP_norm(&y3);                   //y3.norm(); //31

        FP_add(&t1, &t0, &t0);          //t1.add(t0); //t1.norm(); //32
        FP_add(&t0, &t0, &t1);          //t0.add(t1); //t0.norm();//33
        FP_sub(&t0, &t0, &t2);          //t0.sub(t2);
        FP_norm(&t0);                   //t0.norm();//34
        FP_mul(&t1, &t4, &y3);          //t1.mul(y3);//35
        FP_mul(&t2, &t0, &y3);          //t2.mul(y3);//36
        FP_mul(&y3, &x3, &z3);          //y3.mul(z3);//37
        FP_add(&(P->y), &y3, &t2);          //y3.add(t2); //y3.norm();//38
        FP_mul(&x3, &x3, &t3);          //x3.mul(t3);//39
        FP_sub(&(P->x), &x3, &t1);          //x3.sub(t1);//40
        FP_mul(&z3, &z3, &t4);          //z3.mul(t4);//41

        FP_mul(&t1, &t3, &t0);          //t1.mul(t0);//42
        FP_add(&(P->z), &z3, &t1);          //z3.add(t1);
        FP_norm(&(P->x));               //x.norm();
        FP_norm(&(P->y));               //y.norm();
        FP_norm(&(P->z));               //z.norm();
    }

#else
    FP A, B, C, D, E, F, G, b;

    FP_mul(&A, &(P->z), &(Q->z));   //A.mul(Q.z);
    FP_sqr(&B, &A);                 //B.sqr();
    FP_mul(&C, &(P->x), &(Q->x));   //C.mul(Q.x);
    FP_mul(&D, &(P->y), &(Q->y));   //D.mul(Q.y);

    FP_mul(&E, &C, &D);             //E.mul(D);

    if (CURVE_B_I == 0)         //if (ROM.CURVE_B_I==0)
    {
        FP_rcopy(&b, CURVE_B);  //FP b=new FP(new BIG(ROM.CURVE_B));
        FP_mul(&E, &E, &b);         //E.mul(b);
    }
    else
        FP_imul(&E, &E, CURVE_B_I); //E.imul(ROM.CURVE_B_I);

    FP_sub(&F, &B, &E);         //F.sub(E);
    FP_add(&G, &B, &E);         //G.add(E);

    if (CURVE_A == 1)           //if (ROM.CURVE_A==1)
    {
        FP_sub(&E, &D, &C);     //E.sub(C);
    }
    FP_add(&C, &C, &D);         //C.add(D);

    FP_add(&B, &(P->x), &(P->y));   //B.add(y);
    FP_add(&D, &(Q->x), &(Q->y));   //D.add(Q.y);
    FP_norm(&B);                //B.norm();
    FP_norm(&D);                //D.norm();
    FP_mul(&B, &B, &D);         //B.mul(D);
    FP_sub(&B, &B, &C);         //B.sub(C);
    FP_norm(&B);                //B.norm();
    FP_norm(&F);                //F.norm();
    FP_mul(&B, &B, &F);         //B.mul(F);
    FP_mul(&(P->x), &A, &B); //x.mul(B);
    FP_norm(&G);                //G.norm();

    if (CURVE_A == 1)           //if (ROM.CURVE_A==1)
    {
        FP_norm(&E);            //E.norm();
        FP_mul(&C, &E, &G);     //C.mul(G);
    }
    if (CURVE_A == -1)          //if (ROM.CURVE_A==-1)
    {
        FP_norm(&C);            //C.norm();
        FP_mul(&C, &C, &G);     //C.mul(G);
    }
    FP_mul(&(P->y), &A, &C); //y.mul(C);
    FP_mul(&(P->z), &F, &G); //z.mul(G);

#endif
}

/* Set P-=Q */
/* SU=16 */
void  ZZZ::ECP_sub(ECP *P, ECP *Q)
{
    ECP NQ;
    ECP_copy(&NQ, Q);
    ECP_neg(&NQ);
    ECP_add(P, &NQ);
}

#endif

#if CURVETYPE_ZZZ!=MONTGOMERY
/* constant time multiply by small integer of length bts - use ladder */
void ZZZ::ECP_pinmul(ECP *P, int e, int bts)
{
    int i, b;
    ECP R0, R1;

    ECP_affine(P);
    ECP_inf(&R0);
    ECP_copy(&R1, P);

    for (i = bts - 1; i >= 0; i--)
    {
        b = (e >> i) & 1;
        ECP_copy(P, &R1);
        ECP_add(P, &R0);
        ECP_cswap(&R0, &R1, b);
        ECP_copy(&R1, P);
        ECP_dbl(&R0);
        ECP_cswap(&R0, &R1, b);
    }
    ECP_copy(P, &R0);
}
#endif

/* Set P=r*P */
/* SU=424 */
void ZZZ::ECP_mul(ECP *P, BIG e)
{
#if CURVETYPE_ZZZ==MONTGOMERY
    /* Montgomery ladder */
    int nb, i, b;
    ECP R0, R1, D;
    if (ECP_isinf(P)) return;
    if (BIG_iszilch(e))
    {
        ECP_inf(P);
        return;
    }

    ECP_copy(&R0, P);
    ECP_copy(&R1, P);
    ECP_dbl(&R1);

    ECP_copy(&D, P);

    nb = BIG_nbits(e);
    for (i = nb - 2; i >= 0; i--)
    {
        b = BIG_bit(e, i);
        ECP_copy(P, &R1);
        ECP_add(P, &R0, &D);
        ECP_cswap(&R0, &R1, b);
        ECP_copy(&R1, P);
        ECP_dbl(&R0);

        ECP_cswap(&R0, &R1, b);
    }

    ECP_copy(P, &R0);

#else
    /* fixed size windows */
    int i, nb, s, ns;
    BIG mt, t;
    ECP Q, W[8], C;
    sign8 w[1 + (NLEN_XXX * BASEBITS_XXX + 3) / 4];

    if (ECP_isinf(P)) return;
    if (BIG_iszilch(e))
    {
        ECP_inf(P);
        return;
    }

    /* precompute table */

    ECP_copy(&Q, P);
    ECP_dbl(&Q);

    ECP_copy(&W[0], P);

    for (i = 1; i < 8; i++)
    {
        ECP_copy(&W[i], &W[i - 1]);
        ECP_add(&W[i], &Q);
    }

    /* make exponent odd - add 2P if even, P if odd */
    BIG_copy(t, e);
    s = BIG_parity(t);
    BIG_inc(t, 1);
    BIG_norm(t);
    ns = BIG_parity(t);
    BIG_copy(mt, t);
    BIG_inc(mt, 1);
    BIG_norm(mt);
    BIG_cmove(t, mt, s);
    ECP_cmove(&Q, P, ns);
    ECP_copy(&C, &Q);

    nb = 1 + (BIG_nbits(t) + 3) / 4;

    /* convert exponent to signed 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_lastbits(t, 5) - 16;
        BIG_dec(t, w[i]);
        BIG_norm(t);
        BIG_fshr(t, 4);
    }
    w[nb] = BIG_lastbits(t, 5);

    ECP_copy(P, &W[(w[nb] - 1) / 2]);
    for (i = nb - 1; i >= 0; i--)
    {
        ECP_select(&Q, W, w[i]);
        ECP_dbl(P);
        ECP_dbl(P);
        ECP_dbl(P);
        ECP_dbl(P);
        ECP_add(P, &Q);
    }
    ECP_sub(P, &C); /* apply correction */
#endif
}

#if CURVETYPE_ZZZ!=MONTGOMERY
/* Set P=eP+fQ double multiplication */
/* constant time - as useful for GLV method in pairings */
/* SU=456 */

void ZZZ::ECP_mul2(ECP *P, ECP *Q, BIG e, BIG f)
{
    BIG te, tf, mt;
    ECP S, T, W[8], C;
    sign8 w[1 + (NLEN_XXX * BASEBITS_XXX + 1) / 2];
    int i, a, b, s, ns, nb;

    BIG_copy(te, e);
    BIG_copy(tf, f);

    /* precompute table */
    ECP_copy(&W[1], P);
    ECP_sub(&W[1], Q); /* P+Q */
    ECP_copy(&W[2], P);
    ECP_add(&W[2], Q); /* P-Q */
    ECP_copy(&S, Q);
    ECP_dbl(&S);  /* S=2Q */
    ECP_copy(&W[0], &W[1]);
    ECP_sub(&W[0], &S);
    ECP_copy(&W[3], &W[2]);
    ECP_add(&W[3], &S);
    ECP_copy(&T, P);
    ECP_dbl(&T); /* T=2P */
    ECP_copy(&W[5], &W[1]);
    ECP_add(&W[5], &T);
    ECP_copy(&W[6], &W[2]);
    ECP_add(&W[6], &T);
    ECP_copy(&W[4], &W[5]);
    ECP_sub(&W[4], &S);
    ECP_copy(&W[7], &W[6]);
    ECP_add(&W[7], &S);

    /* if multiplier is odd, add 2, else add 1 to multiplier, and add 2P or P to correction */

    s = BIG_parity(te);
    BIG_inc(te, 1);
    BIG_norm(te);
    ns = BIG_parity(te);
    BIG_copy(mt, te);
    BIG_inc(mt, 1);
    BIG_norm(mt);
    BIG_cmove(te, mt, s);
    ECP_cmove(&T, P, ns);
    ECP_copy(&C, &T);

    s = BIG_parity(tf);
    BIG_inc(tf, 1);
    BIG_norm(tf);
    ns = BIG_parity(tf);
    BIG_copy(mt, tf);
    BIG_inc(mt, 1);
    BIG_norm(mt);
    BIG_cmove(tf, mt, s);
    ECP_cmove(&S, Q, ns);
    ECP_add(&C, &S);

    BIG_add(mt, te, tf);
    BIG_norm(mt);
    nb = 1 + (BIG_nbits(mt) + 1) / 2;

    /* convert exponent to signed 2-bit window */
    for (i = 0; i < nb; i++)
    {
        a = BIG_lastbits(te, 3) - 4;
        BIG_dec(te, a);
        BIG_norm(te);
        BIG_fshr(te, 2);
        b = BIG_lastbits(tf, 3) - 4;
        BIG_dec(tf, b);
        BIG_norm(tf);
        BIG_fshr(tf, 2);
        w[i] = 4 * a + b;
    }
    w[nb] = (4 * BIG_lastbits(te, 3) + BIG_lastbits(tf, 3));

    ECP_copy(P, &W[(w[nb] - 1) / 2]);
    for (i = nb - 1; i >= 0; i--)
    {
        ECP_select(&T, W, w[i]);
        ECP_dbl(P);
        ECP_dbl(P);
        ECP_add(P, &T);
    }
    ECP_sub(P, &C); /* apply correction */
}

#endif

void ZZZ::ECP_cfp(ECP *P)
{   /* multiply point by curves cofactor */
    BIG c;
    int cf = CURVE_Cof_I;
    if (cf == 1) return;
    if (cf == 4)
    {
        ECP_dbl(P);
        ECP_dbl(P);
        return;
    }
    if (cf == 8)
    {
        ECP_dbl(P);
        ECP_dbl(P);
        ECP_dbl(P);
        return;
    }
    BIG_rcopy(c, CURVE_Cof);
    ECP_mul(P, c);
    return;
}

/* Constant time Map to Point */
void ZZZ::ECP_map2point(ECP *P,FP *h)
{
#if CURVETYPE_ZZZ==MONTGOMERY
// Elligator 2
    BIG a;
    FP X1,X2,t,one,A;
    //BIG_zero(a); BIG_inc(a,CURVE_A); BIG_norm(a); FP_nres(&A,a);
    FP_from_int(&A,CURVE_A);
    FP_copy(&t,h);
    FP_sqr(&t,&t);   // t^2

    if (PM1D2_YYY == 2)
         FP_add(&t,&t,&t);  // 2t^2
    if (PM1D2_YYY == 1)
        FP_neg(&t,&t);      // -t^2
    if (PM1D2_YYY > 2)
        FP_imul(&t,&t,QNRI_YYY); // precomputed QNR

    FP_one(&one);
    FP_add(&t,&t,&one); FP_norm(&t);
    FP_inv(&t,&t);      // 1/(1+z.t^2)
    FP_mul(&X1,&t,&A);
    FP_neg(&X1,&X1);    // X1 = -A* [1/(1+z.t^2)]
    FP_copy(&X2,&X1);
    FP_add(&X2,&X2,&A); FP_norm(&X2);
    FP_neg(&X2,&X2);    // X2= -(X1+A)

    ECP_rhs(&t,&X2);

    FP_cmove(&X1,&X2,FP_qr(&t,NULL));
    FP_redc(a,&X1);

    ECP_set(P,a);
    return;
#endif
#if CURVETYPE_ZZZ==EDWARDS
// Elligator 2 - map to Montgomery, place point, map back
    int qres,sgn,ne,rfc;
    BIG x,y;
    FP X1,X2,t,one,A,w1,w2,B,Y,NY,K;
    FP_one(&one);

#if MODTYPE_YYY != GENERALISED_MERSENNE
// its NOT goldilocks!
// Figure out the Montgomery curve parameters

    FP_rcopy(&B,CURVE_B);

    if (CURVE_A==1)
    {
        FP_add(&A,&B,&one);  // A=B+1  // A = a+d
        FP_sub(&B,&B,&one);  // B=B-1  // B = -a+d

    } else {
        FP_sub(&A,&B,&one);  // A=B-1
        FP_add(&B,&B,&one);  // B=B+1
    }
    FP_norm(&A); FP_norm(&B);

    FP_div2(&A,&A);    // (A+B)/2     // (a+d)/2  = J/K
    FP_div2(&B,&B);    // (B-A)/2     // (-a+d)/2
    FP_div2(&B,&B);    // (B-A)/4     // (-a+d)/4 = -1/K

    FP_neg(&K,&B);
    FP_inv(&K,&K);    // K

    rfc=RIADZ_YYY;
    if (rfc)
    { // RFC7748 method applies
        FP_mul(&A,&A,&K);   // = J
        FP_sqrt(&K,&K,NULL);
    } else { // generic method
        FP_sqr(&B,&B);
    }
#else
    FP_from_int(&A,156326);
    rfc=1;
#endif
// Map to this Montgomery curve X^2=X^3+AX^2+BX

    FP_copy(&t,h); sgn=FP_sign(&t);
    FP_sqr(&t,&t);   // t^2

    if (PM1D2_YYY == 2)
         FP_add(&t,&t,&t);  // 2t^2
    if (PM1D2_YYY == 1)
        FP_neg(&t,&t);      // -t^2
    if (PM1D2_YYY > 2)
        FP_imul(&t,&t,QNRI_YYY);  // precomputed QNR

    FP_add(&t,&t,&one); FP_norm(&t);
    FP_inv(&t,&t);      // 1/(1+z.t^2)

    FP_mul(&X1,&t,&A);
    FP_neg(&X1,&X1);    // -(J/K).inv(1+z.t^2)

    FP_copy(&X2,&X1);
    FP_add(&X2,&X2,&A); FP_norm(&X2);
    FP_neg(&X2,&X2);    // -(X1+(J/K))

// Figure out RHS of Montgomery curve for both X values

    FP_norm(&X1);
    FP_sqr(&t,&X1); FP_mul(&w1,&t,&X1);  // t=X1^2, w1=X1^3
    FP_mul(&t,&t,&A); FP_add(&w1,&w1,&t);
    if (!rfc)
    {
        FP_mul(&t,&X1,&B);
        FP_add(&w1,&w1,&t);   // w1=X1^3+AX1^2+BX1
    } else {
        FP_add(&w1,&w1,&X1);
    }
    FP_norm(&w1);

    FP_norm(&X2);
    FP_sqr(&t,&X2); FP_mul(&w2,&t,&X2);  // t=X2^2, w2=X2^3
    FP_mul(&t,&t,&A); FP_add(&w2,&w2,&t);
    if (!rfc)
    {
        FP_mul(&t,&X2,&B);
        FP_add(&w2,&w2,&t);   // w2=X2^3+AX2^2+BX2
    } else {
        FP_add(&w2,&w2,&X2);
    }
    FP_norm(&w2);

    qres=FP_qr(&w2,NULL);
    FP_cmove(&X1,&X2,qres);
    FP_cmove(&w1,&w2,qres);

// Map back to Edwards curve - X1 is now the right X

    FP_sqrt(&Y,&w1,NULL);   // This is only sign ambiguity

// this is now in...
    FP_neg(&NY,&Y); FP_norm(&NY);
    FP_cmove(&Y,&NY,1-qres);

    if (!rfc)
    {
        FP_mul(&X1,&X1,&K);
        FP_mul(&Y,&Y,&K);
    }

// this is now out
//    ne=sgn^FP_sign(&Y);
//    FP_neg(&NY,&Y); FP_norm(&NY);
//    FP_cmove(&Y,&NY,ne);


#if MODTYPE_YYY == GENERALISED_MERSENNE
// GOLDILOCKS isogeny
    FP_sqr(&t,&X1);  // t=u^2
    FP_add(&NY,&t,&one); // NY=u^2+1
    FP_norm(&NY);
    FP_sub(&t,&t,&one); // t=u^2-1
    FP_norm(&t);
    FP_mul(&w1,&t,&Y);  // w1=v(u^2-1)
    FP_add(&w1,&w1,&w1);
    FP_add(&w1,&w1,&w1);
    FP_norm(&w1);       // w1=4v(u^2-1)
    FP_sqr(&t,&t);      // t=(u^2-1)^2
    FP_sqr(&Y,&Y);      // v^2
    FP_add(&Y,&Y,&Y);
    FP_add(&Y,&Y,&Y);
    FP_norm(&Y);        // 4v^2
    FP_add(&w2,&t,&Y);  // w2=(u^2-1)^2+4v^2
    FP_norm(&w2);
    FP_inv(&w2,&w2);
    FP_mul(&w1,&w1,&w2); // w1=4v(u^2-1)/[(u^2-1)^2+4v^2]

    FP_sub(&w2,&Y,&t);   // w2=4v^2-(u^2-1)^2
    FP_norm(&w2);
    FP_mul(&w2,&w2,&X1); // w2=u(4v^2-(u^2-1)^2)
    FP_mul(&t,&t,&X1);   // t=u(u^2-1)^2
    FP_copy(&X1,&w1);    // X=4v(u^2-1)/[(u^2-1)^2+4v^2]
    FP_div2(&Y,&Y);      // 2v^2
    FP_mul(&w1,&Y,&NY);  // w1=2v^2(u^2+1)
    FP_sub(&w1,&t,&w1);  // w1=u(u^2-1)^2 - 2v^2(u^2+1)
    FP_norm(&w1);
    FP_inv(&w1,&w1);
    FP_mul(&Y,&w2,&w1);  // Y=u(4v^2-(u^2-1)*2) / u(u^2-1)^2 - 2v^2(u^2+1)

#else

    FP_add(&w1,&X1,&one); FP_norm(&w1); // (s+1)
    FP_sub(&w2,&X1,&one); FP_norm(&w2); // (s-1)
    FP_mul(&t,&w1,&Y);
    FP_inv(&t,&t);
    FP_mul(&X1,&X1,&w1);
    FP_mul(&X1,&X1,&t);

    if (rfc)
        FP_mul(&X1,&X1,&K);

    FP_mul(&Y,&Y,&w2);
    FP_mul(&Y,&Y,&t);
#endif
    FP_redc(x,&X1);
    FP_redc(y,&Y);
    ECP_set(P,x,y);
    return;

#endif

#if CURVETYPE_ZZZ==WEIERSTRASS
    int sgn,ne;
    BIG a,x,y;
    FP X1,X2,X3,t,w,one,A,B,Y,NY,j;
    FP_rcopy(&B,CURVE_B);
    FP_one(&one);
    FP_copy(&t,h);
    sgn=FP_sign(&t);
    if (CURVE_A!=0)
    {
// SWU method.
        FP_from_int(&A,CURVE_A);
        FP_sqr(&t,&t);
    //    if (PM1D2_YYY == 2)
    //        FP_add(&t,&t,&t);  // t2=2*t*t 2 is QNR
    //    else
    //        FP_neg(&t,&t);     // t2=-t^2 -1 is QNR

        FP_imul(&t,&t,RIADZ_YYY);  // Z from hash-to-point draft standard

    //    FP_norm(&t);           // t=Zt^2
        FP_add(&w,&t,&one);    // w=Zt^2+1
        FP_norm(&w);
        FP_mul(&w,&w,&t);    // w=Z^2*t^4+Zt^2
        FP_mul(&A,&A,&w);     // A=Aw
        FP_inv(&A,&A);
        FP_add(&w,&w,&one); FP_norm(&w);
        FP_mul(&w,&w,&B);
        FP_neg(&w,&w);      // -B(w+1)
        FP_norm(&w);
        FP_mul(&X2,&w,&A);   // -B(w+1)/Aw
        FP_mul(&X3,&t,&X2);
        ECP_rhs(&w,&X3);
        FP_cmove(&X2,&X3,FP_qr(&w,NULL));
        ECP_rhs(&w,&X2);
        FP_sqrt(&Y,&w,NULL);
        FP_redc(x,&X2);

    } else {
// Shallue and van de Woestijne
        FP_from_int(&Y,RIADZ_YYY);
        ECP_rhs(&A,&Y);  // A=g(Z)
        FP_sqr(&t,&t);
        FP_mul(&Y,&A,&t);   // tv1=u^2*g(Z)
        FP_add(&t,&one,&Y); FP_norm(&t); // tv2=1+tv1
        FP_sub(&Y,&one,&Y); FP_norm(&Y); // tv1=1-tv1
        FP_mul(&NY,&t,&Y);
        FP_inv(&NY,&NY);     // tv3=inv0(tv1*tv2)
        FP_neg(&A,&A); FP_norm(&A);     // -g(Z)
        FP_imul(&w,&A,3);    // -3*g(Z);
        FP_sqrt(&w,&w,NULL);
        FP_imul(&w,&w,RIADZ_YYY); // tv4=Z*sqrt(-3g(Z))
        FP_mul(&w,&w,h);
        FP_mul(&w,&w,&Y);
        FP_mul(&w,&w,&NY);     // tv5=u*tv1*tv3*tv4
        FP_from_int(&X1,RIADZ_YYY);
        FP_copy(&X3,&X1);
        FP_neg(&X1,&X1); FP_norm(&X1); FP_div2(&X1,&X1); // -Z/2
        FP_copy(&X2,&X1);
        FP_sub(&X1,&X1,&w); FP_norm(&X1);
        FP_add(&X2,&X2,&w); FP_norm(&X2);
        FP_add(&A,&A,&A);
        FP_add(&A,&A,&A);
        FP_norm(&A);      // -4*g(Z)
        FP_sqr(&t,&t);
        FP_mul(&t,&t,&NY);
        FP_sqr(&t,&t);    // (tv2^2*tv3)^2
        FP_mul(&A,&A,&t); // -4*g(Z)*(tv2^2*tv3)^2
        FP_from_int(&t,RIADZ_YYY*RIADZ_YYY*3);
        FP_inv(&t,&t);
        FP_mul(&A,&A,&t);
        FP_add(&X3,&X3,&A); FP_norm(&X3);

        ECP_rhs(&w,&X2);
        FP_cmove(&X3,&X2,FP_qr(&w,NULL));
        ECP_rhs(&w,&X1);
        FP_cmove(&X3,&X1,FP_qr(&w,NULL));
        ECP_rhs(&w,&X3);
        FP_sqrt(&Y,&w,NULL);
        FP_redc(x,&X3);
/*
        FP_from_int(&A,-3);
        FP_sqrt(&w,&A,NULL);      // w=sqrt(-3)
        FP_sub(&j,&w,&one);  FP_norm(&j);
        FP_div2(&j,&j);        // j=(w-1)/2
        FP_mul(&w,&w,&t);     // w=s.t
        FP_add(&B,&B,&one);
        FP_sqr(&Y,&t);
        FP_add(&B,&B,&Y);    // t^2+b+1
        FP_norm(&B);
        FP_inv(&B,&B);
        FP_mul(&w,&w,&B);
        FP_mul(&t,&t,&w);
        FP_sub(&X1,&j,&t); FP_norm(&X1);
        FP_neg(&X2,&X1);
        FP_sub(&X2,&X2,&one);
        FP_norm(&X2);
        FP_sqr(&w,&w); FP_inv(&w,&w);
        FP_add(&X3,&w,&one); FP_norm(&X3);

        ECP_rhs(&w,&X2);
        FP_cmove(&X1,&X2,FP_qr(&w,NULL));
        ECP_rhs(&w,&X3);
        FP_cmove(&X1,&X3,FP_qr(&w,NULL));
        ECP_rhs(&w,&X1);
        FP_sqrt(&Y,&w,NULL);
        FP_redc(x,&X1); */
    }
    ne=FP_sign(&Y)^sgn;
    FP_neg(&NY,&Y); FP_norm(&NY);
    FP_cmove(&Y,&NY,ne);

    FP_redc(y,&Y);
    ECP_set(P,x,y);
#endif
}

/* Hunt and Peck a BIG to a curve point */
/*
void ZZZ::ECP_hap2point(ECP *P,BIG h)
{
    BIG x;
    BIG_copy(x,h);
	for (;;)
	{
#if CURVETYPE_ZZZ!=MONTGOMERY
		ECP_setx(P,x,0);
#else
		ECP_set(P,x);
#endif
		BIG_inc(x,1); BIG_norm(x);
		if (!ECP_isinf(P)) break;
	}
}
*/
/* Map octet to point */
/*
void ZZZ::ECP_mapit(ECP *P, octet *W)
{
    BIG q, x;
    DBIG dx;
    BIG_rcopy(q, Modulus);

    BIG_dfromBytesLen(dx,W->val,W->len);
    BIG_dmod(x,dx,q);

    ECP_hap2point(P,x);
    ECP_cfp(P);
}
*/
int ZZZ::ECP_generator(ECP *G)
{
    BIG x, y;
    BIG_rcopy(x, CURVE_Gx);
#if CURVETYPE_ZZZ!=MONTGOMERY
    BIG_rcopy(y, CURVE_Gy);
    return ECP_set(G, x, y);
#else
    return ECP_set(G, x);
#endif
}

#ifdef HAS_MAIN

using namespace ZZZ;

int main()
{
    int i;
    ECP G, P;
    csprng RNG;
    BIG r, s, x, y, b, m, w, q;
    BIG_rcopy(x, CURVE_Gx);
#if CURVETYPE_ZZZ!=MONTGOMERY
    BIG_rcopy(y, CURVE_Gy);
#endif
    BIG_rcopy(m, Modulus);

    printf("x= ");
    BIG_output(x);
    printf("\n");
#if CURVETYPE_ZZZ!=MONTGOMERY
    printf("y= ");
    BIG_output(y);
    printf("\n");
#endif
    RNG_seed(&RNG, 3, "abc");

#if CURVETYPE_ZZZ!=MONTGOMERY
    ECP_set(&G, x, y);
#else
    ECP_set(&G, x);
#endif
    if (ECP_isinf(&G)) printf("Failed to set - point not on curve\n");
    else printf("set success\n");

    ECP_output(&G);

    BIG_rcopy(r, CURVE_Order); //BIG_dec(r,7);
    printf("r= ");
    BIG_output(r);
    printf("\n");

    ECP_copy(&P, &G);

    ECP_mul(&P, r);

    ECP_output(&P);
    BIG_randomnum(w, &RNG);
    BIG_mod(w, r);

    ECP_copy(&P, &G);
    ECP_mul(&P, w);

    ECP_output(&P);

    return 0;
}

#endif
