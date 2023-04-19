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

#if CURVE_A_ZZZ == -3    

    FP_neg(v, x);
    FP_norm(v);
    FP_imul(v, v, -CURVE_A_ZZZ);
    FP_norm(v);
    FP_add(v, &t, v);
#else
    FP_copy(v, &t);
#endif
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
#if CURVE_A_ZZZ == 1
    FP_sub(v, v, &one);
#endif
#if CURVE_A_ZZZ == -1
    FP_add(v, v, &one);
    FP_norm(v);
    FP_neg(v, v);
#endif
    FP_norm(v);
    FP_inv(&t, &t, NULL);
    FP_mul(v, v, &t);
    FP_reduce(v);
#endif

#if CURVETYPE_ZZZ==MONTGOMERY
    /* x^3+Ax^2+x */
    FP x2, x3;
    FP_sqr(&x2, x);
    FP_mul(&x3, &x2, x);
    FP_copy(v, x);
    FP_imul(&x2, &x2, CURVE_A_ZZZ);
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
    ECP W;
    ECP_copy(&W, P);
    ECP_affine(&W);
    if (ECP_isinf(&W)) return -1;
    FP_redc(y, &(W.y));
    FP_redc(x, &(W.x));
    return FP_sign(&(W.y));
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
    FP_nres(&rhs, x);

    ECP_rhs(&rhs, &rhs);

    if (!FP_qr(&rhs,&hint))
    {
        ECP_inf(P);
        return 0;
    }

    FP_nres(&(P->x), x);
    FP_sqrt(&(P->y), &rhs, &hint);

    if (FP_sign(&(P->y))!=s)
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

    FP_inv(&iz, &(P->z), NULL);
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
    BIG_toBytes(&(W->val[0]), x);
#else
    BIG x, y;
    bool alt=false;
    ECP_affine(P);
    ECP_get(x, y, P);

#if (MBITS-1)%8 <= 4
#ifdef ALLOW_ALT_COMPRESS_ZZZ
    alt=true;
#endif
#endif

    if (alt)
    {
        BIG_toBytes(&(W->val[0]), x);
        if (compress)
        {
            W->len = MODBYTES_XXX;
            W->val[0]|=0x80;
            if (FP_islarger(&(P->y))==1) W->val[0]|=0x20;
        } else {
            W->len = 2 * MODBYTES_XXX;
            BIG_toBytes(&(W->val[MODBYTES_XXX]), y);
        }
    } else {
        BIG_toBytes(&(W->val[1]), x);
        if (compress)
        {
            W->val[0] = 0x02;
            if (FP_sign(&(P->y)) == 1) W->val[0] = 0x03;
            W->len = MODBYTES_XXX + 1;
        } else {
            W->val[0] = 0x04;
            W->len = 2 * MODBYTES_XXX + 1;
            BIG_toBytes(&(W->val[MODBYTES_XXX + 1]), y);
        }
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
    bool alt=false;
    int sgn,cmp,typ = W->val[0];

#if (MBITS-1)%8 <= 4
#ifdef ALLOW_ALT_COMPRESS_ZZZ
    alt=true;
#endif
#endif

    if (alt)
    {
        W->val[0]&=0x1f;
        BIG_fromBytes(x, &(W->val[0]));
        W->val[0]=typ;
        if ((typ&0x80)==0)
        {
            BIG_fromBytes(y, &(W->val[MODBYTES_XXX]));
            if (ECP_set(P, x, y)) return 1;
            return 0;
        } else {
            if (!ECP_setx(P,x,0)) return 0;
            sgn=(typ&0x20)>>5;
            cmp=FP_islarger(&(P->y));
            if ((sgn==1 && cmp!=1) || (sgn==0 && cmp==1)) ECP_neg(P);
            return 1;
        }

    } else {
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

#if CURVE_A_ZZZ == 0

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
#else

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
#endif
#endif

#if CURVETYPE_ZZZ==EDWARDS
    /* Not using square for multiplication swap, as (1) it needs more adds, and (2) it triggers more reductions */

    FP C, D, H, J;
    FP_sqr(&C, &(P->x));                    //C.sqr();
    FP_mul(&(P->x), &(P->x), &(P->y));      //x.mul(y);
    FP_add(&(P->x), &(P->x), &(P->x));      //x.add(x);
    FP_norm(&(P->x));                       //x.norm();

    FP_sqr(&D, &(P->y));                        //D.sqr();

#if CURVE_A_ZZZ == -1    
    FP_neg(&C, &C);             //  C.neg();
#endif
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

    FP_imul(&A, &C, (CURVE_A_ZZZ + 2) / 4); //A.imul((ROM.CURVE_A+2)/4);

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

#if CURVE_A_ZZZ == 0
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
#else

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
#endif

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

#if CURVE_A_ZZZ == 1
    FP_sub(&E, &D, &C);     //E.sub(C);
#endif
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

#if CURVE_A_ZZZ == 1
    FP_norm(&E);            //E.norm();
    FP_mul(&C, &E, &G);     //C.mul(G);
#endif

#if CURVE_A_ZZZ == -1
    FP_norm(&C);            //C.norm();
    FP_mul(&C, &C, &G);     //C.mul(G);
#endif
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

// Point multiplication, multiplies a point P by a scalar e
// This code has no inherent awareness of the order of the curve, or the order of the point.
// The order of the curve will be h.r, where h is a cofactor, and r is a large prime
// Typically P will be of order r (but not always), and typically e will be less than r (but not always)
// A problem can arise if a secret e is a few bits less than r, as the leading zeros in e will leak via a timing attack
// The secret e may however be greater than r (see RFC7748 which combines elimination of a small cofactor h with the point multiplication, using an e>r)
// Our solution is to use as a multiplier an e, whose length in bits is that of the logical OR of e and r, hence allowing e>r while forcing inclusion of leading zeros if e<r. 
// The point multiplication methods used will process leading zeros correctly.

// So this function leaks information about the length of e...
void ZZZ::ECP_mul(ECP *P,BIG e)
{
    ECP_clmul(P,e,e);
}

// .. but this one does not (typically set maxe=r)
// Set P=e*P 
void ZZZ::ECP_clmul(ECP *P, BIG e, BIG maxe)
{
    BIG cm;
    BIG_or(cm,e,maxe);
    int max=BIG_nbits(cm);
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
    ECP_affine(&D);

    nb = max;
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

    nb = 1 + (max + 3) / 4;
    /* convert exponent to signed 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_lastbits(t, 5) - 16;
        BIG_dec(t, w[i]);
        BIG_norm(t);
        BIG_fshr(t, 4);
    }
    w[nb] = BIG_lastbits(t, 5);

    ECP_select(P, W, w[nb]);
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

// Generic multi-multiplication, fixed 4-bit window, P=Sigma e_i*X_i
// m point doublings
void ZZZ::ECP_muln(ECP *P,int n,ECP X[],BIG e[])
{
    int i,j,k,nb;
    BIG t,mt;
    ECP S,R,B[16];
    ECP_inf(P);

    BIG_copy(mt,e[0]); BIG_norm(mt);
    for (i=1;i<n;i++)
    { // find biggest
        BIG_copy(t,e[i]); BIG_norm(t);
        k=BIG_comp(t,mt);
        BIG_cmove(mt,t,(k+1)/2);
    }
    nb=(BIG_nbits(mt)+3)/4;
    for (int i=nb-1;i>=0;i--)
    { // Pippenger's algorithm
        for (j=0;j<16;j++)
            ECP_inf(&B[j]);
        for (j=0;j<n;j++)
        { 
            BIG_copy(mt,e[j]); BIG_norm(mt);
            BIG_shr(mt,4*i);
            k=BIG_lastbits(mt,4);
            ECP_add(&B[k],&X[j]);  // side channel risky
        }
        ECP_inf(&R); ECP_inf(&S);
        for (j=15;j>=1;j--)
        {
            ECP_add(&R,&B[j]);
            ECP_add(&S,&R);
        }
        for (j=0;j<4;j++)
            ECP_dbl(P);
        ECP_add(P,&S);
    }
}

void ZZZ::ECP_mul2(ECP *P, ECP *Q, BIG e, BIG f)
{
    ECP_clmul2(P,Q,e,f,e);
}

/* Set P=eP+fQ double multiplication */
/* constant time - as useful for GLV method in pairings */
/* SU=456 */

void ZZZ::ECP_clmul2(ECP *P, ECP *Q, BIG e, BIG f, BIG maxe)
{
    BIG cm;
    BIG te, tf, mt;
    ECP S, T, W[8], C;
    sign8 w[1 + (NLEN_XXX * BASEBITS_XXX + 1) / 2];
    int i, a, b, s, ns, nb;

    BIG_copy(cm,maxe); BIG_or(cm,cm,e); BIG_or(cm,cm,f);
    int max=BIG_nbits(cm);

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

    //BIG_add(mt, te, tf);
    //BIG_norm(mt);
    nb = 1 + (max + 1) / 2;

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

    //ECP_copy(P, &W[(w[nb] - 1) / 2]);
    ECP_select(P, W, w[nb]);
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
    int qres;
    BIG a;
    FP X1,X2,w,N,t,one,A,D,hint;
    //BIG_zero(a); BIG_inc(a,CURVE_A); BIG_norm(a); FP_nres(&A,a);
    FP_from_int(&A,CURVE_A_ZZZ);
    FP_copy(&t,h);
    FP_sqr(&t,&t);   // t^2

    if (PM1D2_YYY == 2)
         FP_add(&t,&t,&t);  // 2t^2
    if (PM1D2_YYY == 1)
        FP_neg(&t,&t);      // -t^2
    if (PM1D2_YYY > 2)
        FP_imul(&t,&t,QNRI_YYY); // precomputed QNR
    FP_norm(&t);  // z.t^2

    FP_one(&one);
    FP_add(&D,&t,&one); FP_norm(&D);  // Denominator D=1+z.t^2

    FP_copy(&X1,&A);
    FP_neg(&X1,&X1);  FP_norm(&X1);  // X1 = -A/D
    FP_copy(&X2,&X1);
    FP_mul(&X2,&X2,&t);             // X2 = -At/D

    FP_sqr(&w,&X1); FP_mul(&N,&w,&X1);  // w=X1^2, N=X1^3
    FP_mul(&w,&w,&A); FP_mul(&w,&w,&D); FP_add(&N,&N,&w);  // N = X1^3+ADX1^2
    FP_sqr(&t,&D);
    FP_mul(&t,&t,&X1);  
    FP_add(&N,&N,&t);  // N=X1^3+ADX1^2+D^2X1  // Numerator of gx =  N/D^3
    FP_norm(&N);

    FP_mul(&t,&N,&D);                   // N.D
    qres=FP_qr(&t,&hint);  // *** exp
    FP_inv(&w,&t,&hint);
    FP_mul(&D,&w,&N);     // 1/D
    FP_mul(&X1,&X1,&D);    // get X1
    FP_mul(&X2,&X2,&D);    // get X2
    FP_cmove(&X1,&X2,1-qres);
    FP_redc(a,&X1);

    ECP_set(P,a);
    return;
#endif
#if CURVETYPE_ZZZ==EDWARDS
// Elligator 2 - map to Montgomery, place point, map back
    int qres,ne,rfc,qnr;
    BIG x,y;
    FP X1,X2,t,w,one,A,w1,w2,B,Y,K,D,hint;
    FP_one(&one);

#if MODTYPE_YYY != GENERALISED_MERSENNE
// its NOT ed448!
// Figure out the Montgomery curve parameters

    FP_rcopy(&B,CURVE_B);

#if CURVE_A_ZZZ == 1
    FP_add(&A,&B,&one);  // A=B+1  // A = a+d
    FP_sub(&B,&B,&one);  // B=B-1  // B = -a+d
#else
    FP_sub(&A,&B,&one);  // A=B-1
    FP_add(&B,&B,&one);  // B=B+1
#endif
    FP_norm(&A); FP_norm(&B);

    FP_div2(&A,&A);    // (A+B)/2     // (a+d)/2  = J/K
    FP_div2(&B,&B);    // (B-A)/2     // (-a+d)/2
    FP_div2(&B,&B);    // (B-A)/4     // (-a+d)/4 = -1/K

    FP_neg(&K,&B); FP_norm(&K);
    //FP_inv(&K,&K,NULL);    // K
    FP_invsqrt(&K,&w1,&K);                      // *** return K, w1=sqrt(1/K) - - could be precalculated!

    rfc=RIADZ_YYY;
    if (rfc)
    { // RFC7748 method applies
        FP_mul(&A,&A,&K);   // = J
        FP_mul(&K,&K,&w1);
//        FP_sqrt(&K,&K,NULL);
    } else { // generic method
        FP_sqr(&B,&B);
    }
#else
    FP_from_int(&A,156326);
    rfc=1;
#endif
// Map to this Montgomery curve X^2=X^3+AX^2+BX

    FP_copy(&t,h); 
    FP_sqr(&t,&t);   // t^2

    if (PM1D2_YYY == 2)
    {
        FP_add(&t,&t,&t);  // 2t^2
        qnr=2;
    }
    if (PM1D2_YYY == 1)
    {
        FP_neg(&t,&t);      // -t^2
        qnr=-1;
    }
    if (PM1D2_YYY > 2)
    {
        FP_imul(&t,&t,QNRI_YYY);  // precomputed QNR
        qnr=QNRI_YYY;
    }
    FP_norm(&t);
    FP_add(&D,&t,&one); FP_norm(&D);  // Denominator=(1+z.u^2)

    FP_copy(&X1,&A);
    FP_neg(&X1,&X1);   FP_norm(&X1);  // X1=-(J/K).inv(1+z.u^2)
    FP_mul(&X2,&X1,&t);  // X2=X1*z.u^2

// Figure out RHS of Montgomery curve in rational form gx1/d^3

    FP_sqr(&w,&X1); FP_mul(&w1,&w,&X1);  // w=X1^2, w1=X1^3
    FP_mul(&w,&w,&A); FP_mul(&w,&w,&D); FP_add(&w1,&w1,&w);  // w1 = X1^3+ADX1^2
    FP_sqr(&w2,&D);
    if (!rfc)
    {
        FP_mul(&w,&X1,&B);
        FP_mul(&w2,&w2,&w); //
        FP_add(&w1,&w1,&w2);   // w1=X1^3+ADX1^2+BD^2X1
    } else {
        FP_mul(&w2,&w2,&X1);  
        FP_add(&w1,&w1,&w2);  // w1=X1^3+ADX1^2+D^2X1  // was &X1
    }
    FP_norm(&w1);

    FP_mul(&B,&w1,&D);     // gx1=num/den^3 - is_qr num*den (same as num/den, same as num/den^3)
    qres=FP_qr(&B,&hint);  // ***
    FP_inv(&w,&B,&hint);
    FP_mul(&D,&w,&w1);     // 1/D
    FP_mul(&X1,&X1,&D);    // get X1
    FP_mul(&X2,&X2,&D);    // get X2
    FP_sqr(&D,&D);

    FP_imul(&w1,&B,qnr);       // now for gx2 = Z.u^2.gx1
    FP_rcopy(&w,CURVE_HTPC);   // qnr^C3  
    FP_mul(&w,&w,&hint);    // modify hint for gx2
    FP_mul(&w2,&D,h);

    FP_cmove(&X1,&X2,1-qres);  // pick correct one
    FP_cmove(&B,&w1,1-qres);
    FP_cmove(&hint,&w,1-qres);
    FP_cmove(&D,&w2,1-qres);
     
    FP_sqrt(&Y,&B,&hint);   // sqrt(num*den)
    FP_mul(&Y,&Y,&D);       // sqrt(num/den^3)

// correct sign of Y
    FP_neg(&w,&Y); FP_norm(&w);
    FP_cmove(&Y,&w,qres^FP_sign(&Y));

    if (!rfc)
    {
        FP_mul(&X1,&X1,&K);
        FP_mul(&Y,&Y,&K);
    }

#if MODTYPE_YYY == GENERALISED_MERSENNE
// Ed448 isogeny
    FP_sqr(&t,&X1);  // t=u^2
    FP_add(&w,&t,&one); // w=u^2+1
    FP_norm(&w);
    FP_sub(&t,&t,&one); // t=u^2-1
    FP_norm(&t);
    FP_mul(&w1,&t,&Y);  // w1=v(u^2-1)
    FP_add(&w1,&w1,&w1);
    FP_add(&X2,&w1,&w1);
    FP_norm(&X2);       // w1=4v(u^2-1)
    FP_sqr(&t,&t);      // t=(u^2-1)^2
    FP_sqr(&Y,&Y);      // v^2
    FP_add(&Y,&Y,&Y);
    FP_add(&Y,&Y,&Y);
    FP_norm(&Y);        // 4v^2
    FP_add(&B,&t,&Y);  // w2=(u^2-1)^2+4v^2
    FP_norm(&B);                                   // X1=w1/w2 - X2=w1, B=w2

    FP_sub(&w2,&Y,&t);   // w2=4v^2-(u^2-1)^2
    FP_norm(&w2);
    FP_mul(&w2,&w2,&X1); // w2=u(4v^2-(u^2-1)^2)
    FP_mul(&t,&t,&X1);   // t=u(u^2-1)^2
    FP_div2(&Y,&Y);      // 2v^2
    FP_mul(&w1,&Y,&w);  // w1=2v^2(u^2+1)
    FP_sub(&w1,&t,&w1);  // w1=u(u^2-1)^2 - 2v^2(u^2+1)
    FP_norm(&w1);                                   // Y=w2/w1

    FP_mul(&t,&X2,&w1);    // output in projective to avoid inversion
    FP_copy(&(P->x),&t);
    FP_mul(&t,&w2,&B);
    FP_copy(&(P->y),&t);
    FP_mul(&t,&w1,&B);
    FP_copy(&(P->z),&t);

    return;

#else
    FP_add(&w1,&X1,&one); FP_norm(&w1); // (s+1)
    FP_sub(&w2,&X1,&one); FP_norm(&w2); // (s-1)
    FP_mul(&t,&w1,&Y);
    FP_mul(&X1,&X1,&w1);

    if (rfc)
        FP_mul(&X1,&X1,&K);

    FP_mul(&Y,&Y,&w2);      // output in projective to avoid inversion
    FP_copy(&(P->x),&X1);
    FP_copy(&(P->y),&Y);
    FP_copy(&(P->z),&t);
    return;
#endif

#endif

#if CURVETYPE_ZZZ==WEIERSTRASS
// SSWU or SVDW method
    int sgn,ne;
    BIG a,x,y;
    FP X1,X2,X3,t,w,one,A,B,Y,D;
    FP D2,hint,GX1;

#if HTC_ISO_ZZZ != 0
// Map to point on isogenous curve
    int i,k,isox,isoy,iso=HTC_ISO_ZZZ;
    FP xnum,xden,ynum,yden;
    BIG z;
    FP_rcopy(&A,CURVE_Ad);
    FP_rcopy(&B,CURVE_Bd);
#else
    FP_from_int(&A,CURVE_A_ZZZ);
    FP_rcopy(&B,CURVE_B);
#endif

    FP_one(&one);
    FP_copy(&t,h);
    sgn=FP_sign(&t);

#if CURVE_A_ZZZ != 0 || HTC_ISO_ZZZ != 0

        FP_sqr(&t,&t);
        FP_imul(&t,&t,RIADZ_YYY);  // Z from hash-to-point draft standard
        FP_add(&w,&t,&one);     // w=Zt^2+1
        FP_norm(&w);

        FP_mul(&w,&w,&t);      // w=Z^2*t^4+Zt^2
        FP_mul(&D,&A,&w);      // A=Aw
                               
        FP_add(&w,&w,&one); FP_norm(&w);
        FP_mul(&w,&w,&B);
        FP_neg(&w,&w);          // -B(w+1)
        FP_norm(&w);

        FP_copy(&X2,&w);        // Numerators
        FP_mul(&X3,&t,&X2);

// x^3+Ad^2x+Bd^3
        FP_sqr(&GX1,&X2);
        FP_sqr(&D2,&D); FP_mul(&w,&A,&D2); FP_add(&GX1,&GX1,&w); FP_norm(&GX1); FP_mul(&GX1,&GX1,&X2); FP_mul(&D2,&D2,&D); FP_mul(&w,&B,&D2); FP_add(&GX1,&GX1,&w); FP_norm(&GX1);

        FP_mul(&w,&GX1,&D);
        int qr=FP_qr(&w,&hint);   // qr(ad) - only exp happens here
        FP_inv(&D,&w,&hint);     // d=1/(ad)
        FP_mul(&D,&D,&GX1);     // 1/d
        FP_mul(&X2,&X2,&D);     // X2/=D
        FP_mul(&X3,&X3,&D);     // X3/=D
        FP_mul(&t,&t,h);        // t=Z.u^3
        FP_sqr(&D2,&D);

        FP_mul(&D,&D2,&t);
        FP_imul(&t,&w,RIADZ_YYY);
        FP_rcopy(&X1,CURVE_HTPC);     
        FP_mul(&X1,&X1,&hint); // modify hint

        FP_cmove(&X2,&X3,1-qr); 
        FP_cmove(&D2,&D,1-qr);
        FP_cmove(&w,&t,1-qr);
        FP_cmove(&hint,&X1,1-qr);

        FP_sqrt(&Y,&w,&hint);  // first candidate if X2 is correct
        FP_mul(&Y,&Y,&D2);

        ne=FP_sign(&Y)^sgn;
        FP_neg(&w,&Y); FP_norm(&w);
        FP_cmove(&Y,&w,ne);

#if HTC_ISO_ZZZ != 0

// (X2,Y) is on isogenous curve
        k=0;
        isox=iso;
        isoy=3*(iso-1)/2;

// xnum
        FP_rcopy(&xnum,PC[k++]);
        for (i=0;i<isox;i++)
        {
            FP_mul(&xnum,&xnum,&X2); 
            FP_rcopy(&w,PC[k++]);
            FP_add(&xnum,&xnum,&w); FP_norm(&xnum);
        }

// xden
        FP_copy(&xden,&X2);
        FP_rcopy(&w,PC[k++]);
        FP_add(&xden,&xden,&w); FP_norm(&xden);
 
        for (i=0;i<isox-2;i++)
        {
            FP_mul(&xden,&xden,&X2);
            FP_rcopy(&w,PC[k++]);
            FP_add(&xden,&xden,&w); FP_norm(&xden);
        }

// ynum
        FP_rcopy(&ynum,PC[k++]);
        for (i=0;i<isoy;i++)
        {
            FP_mul(&ynum,&ynum,&X2); 
            FP_rcopy(&w,PC[k++]);
            FP_add(&ynum,&ynum,&w); FP_norm(&ynum);
        }

// yden 
        FP_copy(&yden,&X2);
        FP_rcopy(&w,PC[k++]);
        FP_add(&yden,&yden,&w); FP_norm(&yden);
      
        for (i=0;i<isoy-1;i++)
        {
            FP_mul(&yden,&yden,&X2); 
            FP_rcopy(&w,PC[k++]);
            FP_add(&yden,&yden,&w); FP_norm(&yden);
        }

        FP_mul(&ynum,&ynum,&Y);

// return in projective coordinates
        FP_mul(&t,&xnum,&yden);
        FP_copy(&(P->x),&t);

        FP_mul(&t,&ynum,&xden);
        FP_copy(&(P->y),&t);

        FP_mul(&t,&xden,&yden);
        FP_copy(&(P->z),&t);
        return;
#else

        FP_redc(x,&X2);
        FP_redc(y,&Y);
        ECP_set(P,x,y);
        return;
#endif
#else 
// SVDW - Shallue and van de Woestijne
        FP_from_int(&Y,RIADZ_YYY);
        ECP_rhs(&A,&Y);  // A=g(Z)
        FP_rcopy(&B,SQRTm3);
        FP_imul(&B,&B,RIADZ_YYY); // B=Z*sqrt(-3)

        FP_sqr(&t,&t);
        FP_mul(&Y,&A,&t);   // Y=tv1=u^2*g(Z)
        FP_add(&t,&one,&Y); FP_norm(&t); // t=tv2=1+tv1
        FP_sub(&Y,&one,&Y); FP_norm(&Y); // Y=tv1=1-tv1
        FP_mul(&D,&t,&Y);
        FP_mul(&D,&D,&B);

        FP_copy(&w,&A);
        FP_tpo(&D,&w);   // D=tv3=inv0(tv1*tv2*Z*sqrt(-3)) and w=sqrt(g(Z))   // ***

        FP_mul(&w,&w,&B);  // w=tv4=Z.sqrt(-3).sqrt(g(Z))
        if (FP_sign(&w)==1)
        { // depends only on sign of constant RIADZ
            FP_neg(&w,&w);
            FP_norm(&w);
        }
        FP_mul(&w,&w,&B);  // Z.sqrt(-3)
        FP_mul(&w,&w,h);    // u
        FP_mul(&w,&w,&Y);   // tv1
        FP_mul(&w,&w,&D);  // tv3   // tv5=u*tv1*tv3*tv4*Z*sqrt(-3)

        FP_from_int(&X1,RIADZ_YYY);
        FP_copy(&X3,&X1);
        FP_neg(&X1,&X1); FP_norm(&X1); FP_div2(&X1,&X1); // -Z/2
        FP_copy(&X2,&X1);
        FP_sub(&X1,&X1,&w); FP_norm(&X1);
        FP_add(&X2,&X2,&w); FP_norm(&X2);
        FP_add(&A,&A,&A);
        FP_add(&A,&A,&A);
        FP_norm(&A);      // 4*g(Z)
        FP_sqr(&t,&t);
        FP_mul(&t,&t,&D);
        FP_sqr(&t,&t);    // (tv2^2*tv3)^2
        FP_mul(&A,&A,&t); // 4*g(Z)*(tv2^2*tv3)^2
        FP_add(&X3,&X3,&A); FP_norm(&X3);

        ECP_rhs(&w,&X2);
        FP_cmove(&X3,&X2,FP_qr(&w,NULL));                           // ***
        ECP_rhs(&w,&X1);
        FP_cmove(&X3,&X1,FP_qr(&w,NULL));                           // ***
        ECP_rhs(&w,&X3);
        FP_sqrt(&Y,&w,NULL);                                        // ***

        ne=FP_sign(&Y)^sgn;
        FP_neg(&w,&Y); FP_norm(&w);
        FP_cmove(&Y,&w,ne);

        FP_redc(x,&X3);
        FP_redc(y,&Y);
        ECP_set(P,x,y);
        return;
#endif

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
