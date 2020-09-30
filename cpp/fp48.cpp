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

/* CORE Fp^12 functions */
/* SU=m, m is Stack Usage (no lazy )*/
/* FP48 elements are of the form a+i.b+i^2.c */

#include "fp48_YYY.h"
#include "config_curve_ZZZ.h"

using namespace XXX;

/* return 1 if b==c, no branching */
static int teq(sign32 b, sign32 c)
{
    sign32 x = b ^ c;
    x -= 1; // if x=0, x now -1
    return (int)((x >> 31) & 1);
}


/* Constant time select from pre-computed table */
static void FP48_select(YYY::FP48 *f, YYY::FP48 g[], sign32 b)
{
    YYY::FP48 invf;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    FP48_cmove(f, &g[0], teq(babs, 0)); // conditional move
    FP48_cmove(f, &g[1], teq(babs, 1));
    FP48_cmove(f, &g[2], teq(babs, 2));
    FP48_cmove(f, &g[3], teq(babs, 3));
    FP48_cmove(f, &g[4], teq(babs, 4));
    FP48_cmove(f, &g[5], teq(babs, 5));
    FP48_cmove(f, &g[6], teq(babs, 6));
    FP48_cmove(f, &g[7], teq(babs, 7));

    FP48_copy(&invf, f);
    FP48_conj(&invf, &invf); // 1/f
    FP48_cmove(f, &invf, (int)(m & 1));
}


/* test x==0 ? */
/* SU= 8 */
int YYY::FP48_iszilch(FP48 *x)
{
    if (FP16_iszilch(&(x->a)) && FP16_iszilch(&(x->b)) && FP16_iszilch(&(x->c))) return 1;
    return 0;
}

/* test x==1 ? */
/* SU= 8 */
int YYY::FP48_isunity(FP48 *x)
{
    if (FP16_isunity(&(x->a)) && FP16_iszilch(&(x->b)) && FP16_iszilch(&(x->c))) return 1;
    return 0;
}

/* FP48 copy w=x */
/* SU= 16 */
void YYY::FP48_copy(FP48 *w, FP48 *x)
{
    if (x == w) return;
    FP16_copy(&(w->a), &(x->a));
    FP16_copy(&(w->b), &(x->b));
    FP16_copy(&(w->c), &(x->c));
    w->type = x->type;
}

/* FP48 w=1 */
/* SU= 8 */
void YYY::FP48_one(FP48 *w)
{
    FP16_one(&(w->a));
    FP16_zero(&(w->b));
    FP16_zero(&(w->c));
    w->type = FP_UNITY;
}

void YYY::FP48_zero(FP48 *w)
{
    FP16_zero(&(w->a));
    FP16_zero(&(w->b));
    FP16_zero(&(w->c));
    w->type = FP_ZILCH;
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int YYY::FP48_equals(FP48 *x, FP48 *y)
{
    if (FP16_equals(&(x->a), &(y->a)) && FP16_equals(&(x->b), &(y->b)) && FP16_equals(&(x->c), &(y->c)))
        return 1;
    return 0;
}

/* Set w=conj(x) */
/* SU= 8 */
void YYY::FP48_conj(FP48 *w, FP48 *x)
{
    FP48_copy(w, x);
    FP16_conj(&(w->a), &(w->a));
    FP16_nconj(&(w->b), &(w->b));
    FP16_conj(&(w->c), &(w->c));
}

/* Create FP48 from FP16 */
/* SU= 8 */
void YYY::FP48_from_FP16(FP48 *w, FP16 *a)
{
    FP16_copy(&(w->a), a);
    FP16_zero(&(w->b));
    FP16_zero(&(w->c));
    w->type = FP_SPARSEST;
}

/* Create FP48 from 3 FP16's */
/* SU= 16 */
void YYY::FP48_from_FP16s(FP48 *w, FP16 *a, FP16 *b, FP16 *c)
{
    FP16_copy(&(w->a), a);
    FP16_copy(&(w->b), b);
    FP16_copy(&(w->c), c);
    w->type = FP_DENSE;
}

/* Granger-Scott Unitary Squaring. This does not benefit from lazy reduction */
/* SU= 600 */
void YYY::FP48_usqr(FP48 *w, FP48 *x)
{
    FP16 A, B, C, D;

    FP16_copy(&A, &(x->a));

    FP16_sqr(&(w->a), &(x->a));
    FP16_add(&D, &(w->a), &(w->a));
    FP16_add(&(w->a), &D, &(w->a));

    FP16_norm(&(w->a));
    FP16_nconj(&A, &A);

    FP16_add(&A, &A, &A);
    FP16_add(&(w->a), &(w->a), &A);
    FP16_sqr(&B, &(x->c));
    FP16_times_i(&B);

    FP16_add(&D, &B, &B);
    FP16_add(&B, &B, &D);
    FP16_norm(&B);

    FP16_sqr(&C, &(x->b));

    FP16_add(&D, &C, &C);
    FP16_add(&C, &C, &D);

    FP16_norm(&C);
    FP16_conj(&(w->b), &(x->b));
    FP16_add(&(w->b), &(w->b), &(w->b));
    FP16_nconj(&(w->c), &(x->c));

    FP16_add(&(w->c), &(w->c), &(w->c));
    FP16_add(&(w->b), &B, &(w->b));
    FP16_add(&(w->c), &C, &(w->c));
    w->type = FP_DENSE;

    FP48_reduce(w);     /* reduce here as in pow function repeated squarings would trigger multiple reductions */
}

/* FP48 squaring w=x^2 */
/* SU= 600 */
void YYY::FP48_sqr(FP48 *w, FP48 *x)
{
    /* Use Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */

    FP16 A, B, C, D;

    if (x->type <= FP_UNITY)
    {
        FP48_copy(w, x);
        return;
    }

    FP16_sqr(&A, &(x->a));
    FP16_mul(&B, &(x->b), &(x->c));
    FP16_add(&B, &B, &B);
    FP16_norm(&B);
    FP16_sqr(&C, &(x->c));

    FP16_mul(&D, &(x->a), &(x->b));
    FP16_add(&D, &D, &D);

    FP16_add(&(w->c), &(x->a), &(x->c));
    FP16_add(&(w->c), &(x->b), &(w->c));
    FP16_norm(&(w->c));

    FP16_sqr(&(w->c), &(w->c));

    FP16_copy(&(w->a), &A);
    FP16_add(&A, &A, &B);

    FP16_norm(&A);

    FP16_add(&A, &A, &C);
    FP16_add(&A, &A, &D);

    FP16_norm(&A);

    FP16_neg(&A, &A);
    FP16_times_i(&B);
    FP16_times_i(&C);

    FP16_add(&(w->a), &(w->a), &B);
    FP16_add(&(w->b), &C, &D);
    FP16_add(&(w->c), &(w->c), &A);

    if (x->type == FP_SPARSER || x->type == FP_SPARSEST)
        w->type = FP_SPARSE;
    else
        w->type = FP_DENSE;

    FP48_norm(w);
}

/* FP48 full multiplication w=w*y */


/* SU= 896 */
/* FP48 full multiplication w=w*y */
void YYY::FP48_mul(FP48 *w, FP48 *y)
{
    FP16 z0, z1, z2, z3, t0, t1;

    FP16_mul(&z0, &(w->a), &(y->a));
    FP16_mul(&z2, &(w->b), &(y->b)); //

    FP16_add(&t0, &(w->a), &(w->b));
    FP16_add(&t1, &(y->a), &(y->b)); //

    FP16_norm(&t0);
    FP16_norm(&t1);

    FP16_mul(&z1, &t0, &t1);
    FP16_add(&t0, &(w->b), &(w->c));
    FP16_add(&t1, &(y->b), &(y->c)); //

    FP16_norm(&t0);
    FP16_norm(&t1);

    FP16_mul(&z3, &t0, &t1);

    FP16_neg(&t0, &z0);
    FP16_neg(&t1, &z2);

    FP16_add(&z1, &z1, &t0); // z1=z1-z0
    FP16_add(&(w->b), &z1, &t1); // z1=z1-z2
    FP16_add(&z3, &z3, &t1);      // z3=z3-z2
    FP16_add(&z2, &z2, &t0);      // z2=z2-z0

    FP16_add(&t0, &(w->a), &(w->c));
    FP16_add(&t1, &(y->a), &(y->c));

    FP16_norm(&t0);
    FP16_norm(&t1);

    FP16_mul(&t0, &t1, &t0);
    FP16_add(&z2, &z2, &t0);

    FP16_mul(&t0, &(w->c), &(y->c));
    FP16_neg(&t1, &t0);

    FP16_add(&(w->c), &z2, &t1);
    FP16_add(&z3, &z3, &t1);
    FP16_times_i(&t0);
    FP16_add(&(w->b), &(w->b), &t0);
    FP16_norm(&z3);
    FP16_times_i(&z3);
    FP16_add(&(w->a), &z0, &z3);

    FP48_norm(w);
    w->type = FP_DENSE;
}

/* FP48 full multiplication w=w*y */
/* Supports sparse multiplicands */
/* Usually w is denser than y */
void YYY::FP48_ssmul(FP48 *w, FP48 *y)
{
    FP16 z0, z1, z2, z3, t0, t1;
    if (w->type == FP_UNITY)
    {
        FP48_copy(w, y);
        return;
    }
    if (y->type == FP_UNITY)
        return;

//  FP48_mul(w,y); return;

    if (y->type >= FP_SPARSE)
    {
        FP16_mul(&z0, &(w->a), &(y->a)); // xa.ya   always 11x11

#if SEXTIC_TWIST_ZZZ == M_TYPE
        if (y->type == FP_SPARSE || w->type == FP_SPARSE)
        {
            FP8_mul(&z2.b, &(w->b).b, &(y->b).b);
            FP8_zero(&z2.a);
            if (y->type != FP_SPARSE)
                FP8_mul(&z2.a, &(w->b).b, &(y->b).a);
            if (w->type != FP_SPARSE)
                FP8_mul(&z2.a, &(w->b).a, &(y->b).b);
            FP16_times_i(&z2);
        }
        else
#endif
            FP16_mul(&z2, &(w->b), &(y->b)); // xb.yb  could be 00x00 or 01x01 or or 10x10 or 11x00 or 11x10 or 11x01 or 11x11

        FP16_add(&t0, &(w->a), &(w->b)); // (xa+xb)
        FP16_add(&t1, &(y->a), &(y->b)); // (ya+yb)

        FP16_norm(&t0);
        FP16_norm(&t1);

        FP16_mul(&z1, &t0, &t1); // (xa+xb)(ya+yb)  always 11x11
        FP16_add(&t0, &(w->b), &(w->c)); // (xb+xc)
        FP16_add(&t1, &(y->b), &(y->c)); // (yb+yc)

        FP16_norm(&t0);
        FP16_norm(&t1);

        FP16_mul(&z3, &t0, &t1); // (xb+xc)(yb+yc)   could be anything...
        FP16_neg(&t0, &z0);     // -(xa.ya)
        FP16_neg(&t1, &z2);     // -(xb.yb)

        FP16_add(&z1, &z1, &t0);
        FP16_add(&(w->b), &z1, &t1); // /wb = (xa+xb)(ya+yb) -(xa.ya) -(xb.yb)                        = xa.yb + xb.ya

        FP16_add(&z3, &z3, &t1);      // (xb+xc)(yb+yc) -(xb.yb)
        FP16_add(&z2, &z2, &t0);      // (xb.yb) - (xa.ya)

        FP16_add(&t0, &(w->a), &(w->c)); // (xa+xc)
        FP16_add(&t1, &(y->a), &(y->c)); // (ya+yc)

        FP16_norm(&t0);
        FP16_norm(&t1);

        FP16_mul(&t0, &t1, &t0); // (xa+xc)(ya+yc)    always 11x11
        FP16_add(&z2, &z2, &t0); // (xb.yb) - (xa.ya) + (xa+xc)(ya+yc)

#if SEXTIC_TWIST_ZZZ == D_TYPE
        if (y->type == FP_SPARSE || w->type == FP_SPARSE)
        {
            FP8_mul(&t0.a, &(w->c).a, &(y->c).a);
            FP8_zero(&t0.b);
            if (y->type != FP_SPARSE)
                FP8_mul(&t0.b, &(w->c).a, &(y->c).b);
            if (w->type != FP_SPARSE)
                FP8_mul(&t0.b, &(w->c).b, &(y->c).a);
        }
        else
#endif
            FP16_mul(&t0, &(w->c), &(y->c)); // (xc.yc)  could be anything

        FP16_neg(&t1, &t0);           // -(xc.yc)

        FP16_add(&(w->c), &z2, &t1);    // wc = (xb.yb) - (xa.ya) + (xa+xc)(ya+yc) - (xc.yc)    =  xb.yb + xc.ya + xa.yc
        FP16_add(&z3, &z3, &t1);        // (xb+xc)(yb+yc) -(xb.yb) - (xc.yc)                    =  xb.yc + xc.yb
        FP16_times_i(&t0);              // i.(xc.yc)
        FP16_add(&(w->b), &(w->b), &t0); // wb = (xa+xb)(ya+yb) -(xa.ya) -(xb.yb) +i(xc.yc)
        FP16_norm(&z3);
        FP16_times_i(&z3);              // i[(xb+xc)(yb+yc) -(xb.yb) - (xc.yc)]                 = i(xb.yc + xc.yb)
        FP16_add(&(w->a), &z0, &z3);    // wa = xa.ya + i(xb.yc + xc.yb)
    } else {
        if (w->type == FP_SPARSER || w->type == FP_SPARSEST)
        {
            FP48_smul(w, y);
            return;
        }
// dense by sparser - 13m
#if SEXTIC_TWIST_ZZZ == D_TYPE
        FP16_copy(&z3, &(w->b));
        FP16_mul(&z0, &(w->a), &(y->a));

        if (y->type == FP_SPARSEST)
            FP16_tmul(&z2, &(w->b), &(y->b).a.a.a.a);
        else
            FP16_pmul(&z2, &(w->b), &(y->b).a);

        FP16_add(&(w->b), &(w->a), &(w->b));
        FP16_copy(&t1, &(y->a));
        FP8_add(&t1.a, &t1.a, &(y->b).a);

        FP16_norm(&t1);
        FP16_norm(&(w->b));

        FP16_mul(&(w->b), &(w->b), &t1);
        FP16_add(&z3, &z3, &(w->c));
        FP16_norm(&z3);

        if (y->type == FP_SPARSEST)
            FP16_tmul(&z3, &z3, &(y->b).a.a.a.a);
        else
            FP16_pmul(&z3, &z3, &(y->b).a);

        FP16_neg(&t0, &z0);
        FP16_neg(&t1, &z2);

        FP16_add(&(w->b), &(w->b), &t0); // z1=z1-z0
        FP16_add(&(w->b), &(w->b), &t1); // z1=z1-z2

        FP16_add(&z3, &z3, &t1);      // z3=z3-z2
        FP16_add(&z2, &z2, &t0);      // z2=z2-z0

        FP16_add(&t0, &(w->a), &(w->c));
        FP16_norm(&t0);
        FP16_norm(&z3);

        FP16_mul(&t0, &(y->a), &t0);
        FP16_add(&(w->c), &z2, &t0);

        FP16_times_i(&z3);
        FP16_add(&(w->a), &z0, &z3);
#endif
#if SEXTIC_TWIST_ZZZ == M_TYPE
        FP16_mul(&z0, &(w->a), &(y->a));
        FP16_add(&t0, &(w->a), &(w->b));
        FP16_norm(&t0);

        FP16_mul(&z1, &t0, &(y->a));
        FP16_add(&t0, &(w->b), &(w->c));
        FP16_norm(&t0);

        if (y->type == FP_SPARSEST)
            FP16_tmul(&z3, &t0, &(y->c).b.a.a.a);
        else
            FP16_pmul(&z3, &t0, &(y->c).b);

        FP16_times_i(&z3);

        FP16_neg(&t0, &z0);
        FP16_add(&z1, &z1, &t0); // z1=z1-z0

        FP16_copy(&(w->b), &z1);
        FP16_copy(&z2, &t0);

        FP16_add(&t0, &(w->a), &(w->c));
        FP16_add(&t1, &(y->a), &(y->c));

        FP16_norm(&t0);
        FP16_norm(&t1);

        FP16_mul(&t0, &t1, &t0);
        FP16_add(&z2, &z2, &t0);

        if (y->type == FP_SPARSEST)
            FP16_tmul(&t0, &(w->c), &(y->c).b.a.a.a);
        else
            FP16_pmul(&t0, &(w->c), &(y->c).b);

        FP16_times_i(&t0);
        FP16_neg(&t1, &t0);
        FP16_times_i(&t0);

        FP16_add(&(w->c), &z2, &t1);
        FP16_add(&z3, &z3, &t1);

        FP16_add(&(w->b), &(w->b), &t0);
        FP16_norm(&z3);
        FP16_times_i(&z3);
        FP16_add(&(w->a), &z0, &z3);
#endif
    }
    w->type = FP_DENSE;
    FP48_norm(w);
}

/* FP48 multiplication w=w*y */
/* catering for special case that arises from special form of ATE pairing line function */
/* w and y are both sparser line functions - cost = 6m */
void YYY::FP48_smul(FP48 *w, FP48 *y)
{
    FP8 w1, w2, w3, ta, tb, tc, td, te, t;

//  FP48_mul(w,y); return;

#if SEXTIC_TWIST_ZZZ == D_TYPE
    FP8_mul(&w1, &(w->a).a, &(y->a).a); // A1.A2
    FP8_mul(&w2, &(w->a).b, &(y->a).b); // B1.B2

    if (y->type == FP_SPARSEST || w->type == FP_SPARSEST)
    {
        if (y->type == FP_SPARSEST && w->type == FP_SPARSEST)
        {
            FP8_zero(&w3);
            FP_mul(&w3.a.a.a, &(w->b).a.a.a.a, &(y->b).a.a.a.a);
        } else {
            if (y->type != FP_SPARSEST)
                FP8_tmul(&w3, &(y->b).a, &(w->b).a.a.a.a);
            if (w->type != FP_SPARSEST)
                FP8_tmul(&w3, &(w->b).a, &(y->b).a.a.a.a);
        }
    }
    else
        FP8_mul(&w3, &(w->b).a, &(y->b).a); // C1.C2

    FP8_add(&ta, &(w->a).a, &(w->a).b); // A1+B1
    FP8_add(&tb, &(y->a).a, &(y->a).b); // A2+B2
    FP8_norm(&ta);
    FP8_norm(&tb);
    FP8_mul(&tc, &ta, &tb);         // (A1+B1)(A2+B2)
    FP8_add(&t, &w1, &w2);
    FP8_neg(&t, &t);
    FP8_add(&tc, &tc, &t);          // (A1+B1)(A2+B2)-A1.A2-B1*B2 =  (A1.B2+A2.B1)

    FP8_add(&ta, &(w->a).a, &(w->b).a); // A1+C1
    FP8_add(&tb, &(y->a).a, &(y->b).a); // A2+C2
    FP8_norm(&ta);
    FP8_norm(&tb);
    FP8_mul(&td, &ta, &tb);         // (A1+C1)(A2+C2)
    FP8_add(&t, &w1, &w3);
    FP8_neg(&t, &t);
    FP8_add(&td, &td, &t);          // (A1+C1)(A2+C2)-A1.A2-C1*C2 =  (A1.C2+A2.C1)

    FP8_add(&ta, &(w->a).b, &(w->b).a); // B1+C1
    FP8_add(&tb, &(y->a).b, &(y->b).a); // B2+C2
    FP8_norm(&ta);
    FP8_norm(&tb);
    FP8_mul(&te, &ta, &tb);         // (B1+C1)(B2+C2)
    FP8_add(&t, &w2, &w3);
    FP8_neg(&t, &t);
    FP8_add(&te, &te, &t);          // (B1+C1)(B2+C2)-B1.B2-C1*C2 =  (B1.C2+B2.C1)

    FP8_times_i(&w2);
    FP8_add(&w1, &w1, &w2);
    FP16_from_FP8s(&(w->a), &w1, &tc);
    FP16_from_FP8s(&(w->b), &td, &te); // only norm these 2
    FP16_from_FP8(&(w->c), &w3);

    FP16_norm(&(w->a));
    FP16_norm(&(w->b));
#endif
#if SEXTIC_TWIST_ZZZ == M_TYPE
    FP8_mul(&w1, &(w->a).a, &(y->a).a); // A1.A2
    FP8_mul(&w2, &(w->a).b, &(y->a).b); // B1.B2

    if (y->type == FP_SPARSEST || w->type == FP_SPARSEST)
    {
        if (y->type == FP_SPARSEST && w->type == FP_SPARSEST)
        {
            FP8_zero(&w3);
            FP_mul(&w3.a.a.a, &(w->c).b.a.a.a, &(y->c).b.a.a.a);
        } else {
            if (y->type != FP_SPARSEST)
                FP8_tmul(&w3, &(y->c).b, &(w->c).b.a.a.a);
            if (w->type != FP_SPARSEST)
                FP8_tmul(&w3, &(w->c).b, &(y->c).b.a.a.a);
        }
    }
    else
        FP8_mul(&w3, &(w->c).b, &(y->c).b); // F1.F2

    FP8_add(&ta, &(w->a).a, &(w->a).b); // A1+B1
    FP8_add(&tb, &(y->a).a, &(y->a).b); // A2+B2
    FP8_norm(&ta);
    FP8_norm(&tb);
    FP8_mul(&tc, &ta, &tb);         // (A1+B1)(A2+B2)
    FP8_add(&t, &w1, &w2);
    FP8_neg(&t, &t);
    FP8_add(&tc, &tc, &t);          // (A1+B1)(A2+B2)-A1.A2-B1*B2 =  (A1.B2+A2.B1)

    FP8_add(&ta, &(w->a).a, &(w->c).b); // A1+F1
    FP8_add(&tb, &(y->a).a, &(y->c).b); // A2+F2
    FP8_norm(&ta);
    FP8_norm(&tb);
    FP8_mul(&td, &ta, &tb);         // (A1+F1)(A2+F2)
    FP8_add(&t, &w1, &w3);
    FP8_neg(&t, &t);
    FP8_add(&td, &td, &t);          // (A1+F1)(A2+F2)-A1.A2-F1*F2 =  (A1.F2+A2.F1)

    FP8_add(&ta, &(w->a).b, &(w->c).b); // B1+F1
    FP8_add(&tb, &(y->a).b, &(y->c).b); // B2+F2
    FP8_norm(&ta);
    FP8_norm(&tb);
    FP8_mul(&te, &ta, &tb);         // (B1+F1)(B2+F2)
    FP8_add(&t, &w2, &w3);
    FP8_neg(&t, &t);
    FP8_add(&te, &te, &t);          // (B1+F1)(B2+F2)-B1.B2-F1*F2 =  (B1.F2+B2.F1)

    FP8_times_i(&w2);
    FP8_add(&w1, &w1, &w2);
    FP16_from_FP8s(&(w->a), &w1, &tc);

    FP8_times_i(&w3);
    FP8_norm(&w3);
    FP16_from_FP8H(&(w->b), &w3);

    FP8_norm(&te);
    FP8_times_i(&te);
    FP16_from_FP8s(&(w->c), &te, &td);

    FP16_norm(&(w->a));
    FP16_norm(&(w->c));
#endif

    w->type = FP_SPARSE;
}

/* Set w=1/x */
/* SU= 600 */
void YYY::FP48_inv(FP48 *w, FP48 *x)
{
    FP16 f0, f1, f2, f3;

    FP16_sqr(&f0, &(x->a));
    FP16_mul(&f1, &(x->b), &(x->c));
    FP16_times_i(&f1);
    FP16_sub(&f0, &f0, &f1); /* y.a */
    FP16_norm(&f0);

    FP16_sqr(&f1, &(x->c));
    FP16_times_i(&f1);
    FP16_mul(&f2, &(x->a), &(x->b));
    FP16_sub(&f1, &f1, &f2); /* y.b */
    FP16_norm(&f1);

    FP16_sqr(&f2, &(x->b));
    FP16_mul(&f3, &(x->a), &(x->c));
    FP16_sub(&f2, &f2, &f3); /* y.c */
    FP16_norm(&f2);

    FP16_mul(&f3, &(x->b), &f2);
    FP16_times_i(&f3);
    FP16_mul(&(w->a), &f0, &(x->a));
    FP16_add(&f3, &(w->a), &f3);
    FP16_mul(&(w->c), &f1, &(x->c));
    FP16_times_i(&(w->c));

    FP16_add(&f3, &(w->c), &f3);
    FP16_norm(&f3);

    FP16_inv(&f3, &f3);
    FP16_mul(&(w->a), &f0, &f3);
    FP16_mul(&(w->b), &f1, &f3);
    FP16_mul(&(w->c), &f2, &f3);
    w->type = FP_DENSE;
}

/* constant time powering by small integer of max length bts */

void YYY::FP48_pinpow(FP48 *r, int e, int bts)
{
    int i, b;
    FP48 R[2];

    FP48_one(&R[0]);
    FP48_copy(&R[1], r);

    for (i = bts - 1; i >= 0; i--)
    {
        b = (e >> i) & 1;
        FP48_mul(&R[1 - b], &R[b]);
        FP48_usqr(&R[b], &R[b]);
    }
    FP48_copy(r, &R[0]);
}

/* Compressed powering of unitary elements y=x^(e mod r) */
/*
void YYY::FP48_compow(FP16 *c, FP48 *x, BIG e, BIG r)
{
    FP48 g1, g2;
    FP16 cp, cpm1, cpm2;
    FP2 f;
    BIG q, a, b, m;

    BIG_rcopy(a, Fra);
    BIG_rcopy(b, Frb);
    FP2_from_BIGs(&f, a, b);

    BIG_rcopy(q, Modulus);

    FP48_copy(&g1, x);
    FP48_copy(&g2, x);

    BIG_copy(m, q);
    BIG_mod(m, r);

    BIG_copy(a, e);
    BIG_mod(a, m);

    BIG_copy(b, e);
    BIG_sdiv(b, m);

    FP48_trace(c, &g1);

    if (BIG_iszilch(b))
    {
        FP16_xtr_pow(c, c, e);
        return;
    }

    FP48_frob(&g2, &f, 1);
    FP48_trace(&cp, &g2);
    FP48_conj(&g1, &g1);
    FP48_mul(&g2, &g1);
    FP48_trace(&cpm1, &g2);
    FP48_mul(&g2, &g1);

    FP48_trace(&cpm2, &g2);

    FP16_xtr_pow2(c, &cp, c, &cpm1, &cpm2, a, b);

}
*/
/* Note this is simple square and multiply, so not side-channel safe */

void YYY::FP48_pow(FP48 *r, FP48 *a, BIG b)
{
    FP48 w, sf;
    BIG b1, b3;
    int i, nb, bt;
    BIG_copy(b1, b);
    BIG_norm(b1);
    BIG_pmul(b3, b1, 3);
    BIG_norm(b3);

    FP48_copy(&sf, a);
    FP48_norm(&sf);
    FP48_copy(&w, &sf);

    if (BIG_iszilch(b3))
    {
        FP48_one(r);
        return;
    }

    nb = BIG_nbits(b3);
    for (i = nb - 2; i >= 1; i--)
    {
        FP48_usqr(&w, &w);
        bt = BIG_bit(b3, i) - BIG_bit(b1, i);
        if (bt == 1)
            FP48_mul(&w, &sf);
        if (bt == -1)
        {
            FP48_conj(&sf, &sf);
            FP48_mul(&w, &sf);
            FP48_conj(&sf, &sf);
        }
    }

    FP48_copy(r, &w);
    FP48_reduce(r);
}

/* p=q0^u0.q1^u1.q2^u2.q3^u3... */
/* Side channel attack secure */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf

void YYY::FP48_pow16(FP48 *p, FP48 *q, BIG u[16])
{
    int i, j, k, nb, pb1, pb2, pb3, pb4, bt;
    FP48 g1[8], g2[8], g3[8], g4[8], r;
    BIG t[16], mt;
    sign8 w1[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s1[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 w2[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s2[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 w3[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s3[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 w4[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s4[NLEN_XXX * BASEBITS_XXX + 1];

    for (i = 0; i < 16; i++)
        BIG_copy(t[i], u[i]);

// Precomputed tables
    FP48_copy(&g1[0], &q[0]); // q[0]
    FP48_copy(&g1[1], &g1[0]);
    FP48_mul(&g1[1], &q[1]); // q[0].q[1]
    FP48_copy(&g1[2], &g1[0]);
    FP48_mul(&g1[2], &q[2]); // q[0].q[2]
    FP48_copy(&g1[3], &g1[1]);
    FP48_mul(&g1[3], &q[2]); // q[0].q[1].q[2]
    FP48_copy(&g1[4], &g1[0]);
    FP48_mul(&g1[4], &q[3]); // q[0].q[3]
    FP48_copy(&g1[5], &g1[1]);
    FP48_mul(&g1[5], &q[3]); // q[0].q[1].q[3]
    FP48_copy(&g1[6], &g1[2]);
    FP48_mul(&g1[6], &q[3]); // q[0].q[2].q[3]
    FP48_copy(&g1[7], &g1[3]);
    FP48_mul(&g1[7], &q[3]); // q[0].q[1].q[2].q[3]

    FP48_copy(&g2[0], &q[4]); // q[0]
    FP48_copy(&g2[1], &g2[0]);
    FP48_mul(&g2[1], &q[5]); // q[0].q[1]
    FP48_copy(&g2[2], &g2[0]);
    FP48_mul(&g2[2], &q[6]); // q[0].q[2]
    FP48_copy(&g2[3], &g2[1]);
    FP48_mul(&g2[3], &q[6]); // q[0].q[1].q[2]
    FP48_copy(&g2[4], &g2[0]);
    FP48_mul(&g2[4], &q[7]); // q[0].q[3]
    FP48_copy(&g2[5], &g2[1]);
    FP48_mul(&g2[5], &q[7]); // q[0].q[1].q[3]
    FP48_copy(&g2[6], &g2[2]);
    FP48_mul(&g2[6], &q[7]); // q[0].q[2].q[3]
    FP48_copy(&g2[7], &g2[3]);
    FP48_mul(&g2[7], &q[7]); // q[0].q[1].q[2].q[3]

    FP48_copy(&g3[0], &q[8]); // q[0]
    FP48_copy(&g3[1], &g3[0]);
    FP48_mul(&g3[1], &q[9]); // q[0].q[1]
    FP48_copy(&g3[2], &g3[0]);
    FP48_mul(&g3[2], &q[10]);   // q[0].q[2]
    FP48_copy(&g3[3], &g3[1]);
    FP48_mul(&g3[3], &q[10]);   // q[0].q[1].q[2]
    FP48_copy(&g3[4], &g3[0]);
    FP48_mul(&g3[4], &q[11]); // q[0].q[3]
    FP48_copy(&g3[5], &g3[1]);
    FP48_mul(&g3[5], &q[11]);   // q[0].q[1].q[3]
    FP48_copy(&g3[6], &g3[2]);
    FP48_mul(&g3[6], &q[11]);   // q[0].q[2].q[3]
    FP48_copy(&g3[7], &g3[3]);
    FP48_mul(&g3[7], &q[11]);   // q[0].q[1].q[2].q[3]

    FP48_copy(&g4[0], &q[12]); // q[0]
    FP48_copy(&g4[1], &g4[0]);
    FP48_mul(&g4[1], &q[13]);   // q[0].q[1]
    FP48_copy(&g4[2], &g4[0]);
    FP48_mul(&g4[2], &q[14]);   // q[0].q[2]
    FP48_copy(&g4[3], &g4[1]);
    FP48_mul(&g4[3], &q[14]);   // q[0].q[1].q[2]
    FP48_copy(&g4[4], &g4[0]);
    FP48_mul(&g4[4], &q[15]); // q[0].q[3]
    FP48_copy(&g4[5], &g4[1]);
    FP48_mul(&g4[5], &q[15]);   // q[0].q[1].q[3]
    FP48_copy(&g4[6], &g4[2]);
    FP48_mul(&g4[6], &q[15]);   // q[0].q[2].q[3]
    FP48_copy(&g4[7], &g4[3]);
    FP48_mul(&g4[7], &q[15]);   // q[0].q[1].q[2].q[3]

// Make them odd
    pb1 = 1 - BIG_parity(t[0]);
    BIG_inc(t[0], pb1);
    BIG_norm(t[0]);

    pb2 = 1 - BIG_parity(t[4]);
    BIG_inc(t[4], pb2);
    BIG_norm(t[4]);

    pb3 = 1 - BIG_parity(t[8]);
    BIG_inc(t[8], pb3);
    BIG_norm(t[8]);

    pb4 = 1 - BIG_parity(t[12]);
    BIG_inc(t[12], pb4);
    BIG_norm(t[12]);

// Number of bits
    BIG_zero(mt);
    for (i = 0; i < 16; i++)
    {
        BIG_or(mt, mt, t[i]);
    }
    nb = 1 + BIG_nbits(mt);

// Sign pivot
    s1[nb - 1] = 1;
    s2[nb - 1] = 1;
    s3[nb - 1] = 1;
    s4[nb - 1] = 1;
    for (i = 0; i < nb - 1; i++)
    {
        BIG_fshr(t[0], 1);
        s1[i] = 2 * BIG_parity(t[0]) - 1;
        BIG_fshr(t[4], 1);
        s2[i] = 2 * BIG_parity(t[4]) - 1;
        BIG_fshr(t[8], 1);
        s3[i] = 2 * BIG_parity(t[8]) - 1;
        BIG_fshr(t[12], 1);
        s4[i] = 2 * BIG_parity(t[12]) - 1;
    }

// Recoded exponents
    for (i = 0; i < nb; i++)
    {
        w1[i] = 0;
        k = 1;
        for (j = 1; j < 4; j++)
        {
            bt = s1[i] * BIG_parity(t[j]);
            BIG_fshr(t[j], 1);

            BIG_dec(t[j], (bt >> 1));
            BIG_norm(t[j]);
            w1[i] += bt * k;
            k *= 2;
        }

        w2[i] = 0;
        k = 1;
        for (j = 5; j < 8; j++)
        {
            bt = s2[i] * BIG_parity(t[j]);
            BIG_fshr(t[j], 1);

            BIG_dec(t[j], (bt >> 1));
            BIG_norm(t[j]);
            w2[i] += bt * k;
            k *= 2;
        }

        w3[i] = 0;
        k = 1;
        for (j = 9; j < 12; j++)
        {
            bt = s3[i] * BIG_parity(t[j]);
            BIG_fshr(t[j], 1);

            BIG_dec(t[j], (bt >> 1));
            BIG_norm(t[j]);
            w3[i] += bt * k;
            k *= 2;
        }

        w4[i] = 0;
        k = 1;
        for (j = 13; j < 16; j++)
        {
            bt = s4[i] * BIG_parity(t[j]);
            BIG_fshr(t[j], 1);

            BIG_dec(t[j], (bt >> 1));
            BIG_norm(t[j]);
            w4[i] += bt * k;
            k *= 2;
        }
    }

// Main loop
    FP48_select(p, g1, 2 * w1[nb - 1] + 1);
    FP48_select(&r, g2, 2 * w2[nb - 1] + 1);
    FP48_mul(p, &r);
    FP48_select(&r, g3, 2 * w3[nb - 1] + 1);
    FP48_mul(p, &r);
    FP48_select(&r, g4, 2 * w4[nb - 1] + 1);
    FP48_mul(p, &r);
    for (i = nb - 2; i >= 0; i--)
    {
        FP48_usqr(p, p);
        FP48_select(&r, g1, 2 * w1[i] + s1[i]);
        FP48_mul(p, &r);
        FP48_select(&r, g2, 2 * w2[i] + s2[i]);
        FP48_mul(p, &r);
        FP48_select(&r, g3, 2 * w3[i] + s3[i]);
        FP48_mul(p, &r);
        FP48_select(&r, g4, 2 * w4[i] + s4[i]);
        FP48_mul(p, &r);
    }

// apply correction
    FP48_conj(&r, &q[0]);
    FP48_mul(&r, p);
    FP48_cmove(p, &r, pb1);
    FP48_conj(&r, &q[4]);
    FP48_mul(&r, p);
    FP48_cmove(p, &r, pb2);

    FP48_conj(&r, &q[8]);
    FP48_mul(&r, p);
    FP48_cmove(p, &r, pb3);
    FP48_conj(&r, &q[12]);
    FP48_mul(&r, p);
    FP48_cmove(p, &r, pb4);

    FP48_reduce(p);
}

/* Set w=w^p using Frobenius */
/* SU= 160 */
void YYY::FP48_frob(FP48 *w, FP2 *f, int n)
{
    int i;
    FP8 X2, X4;
    FP4 F;
    FP2 f3, f2;             // f=(1+i)^(p-19)/24
    FP2_sqr(&f2, f);    //
    FP2_mul(&f3, &f2, f); // f3=f^3=(1+i)^(p-19)/8

    FP2_mul_ip(&f3);
    FP2_norm(&f3);
    FP2_mul_ip(&f3);    // f3 = (1+i)^16/8.(1+i)^(p-19)/8 = (1+i)^(p-3)/8
    FP2_norm(&f3);

    for (i = 0; i < n; i++)
    {
        FP16_frob(&(w->a), &f3);  // a=a^p
        FP16_frob(&(w->b), &f3);  // b=b^p
        FP16_frob(&(w->c), &f3);  // c=c^p

        FP16_qmul(&(w->b), &(w->b), f); FP16_times_i4(&(w->b)); FP16_times_i2(&(w->b));
        FP16_qmul(&(w->c), &(w->c), &f2); FP16_times_i4(&(w->c)); FP16_times_i4(&(w->c)); FP16_times_i4(&(w->c));
    }
    w->type = FP_DENSE;
}

/* SU= 8 */
/* normalise all components of w */
void YYY::FP48_norm(FP48 *w)
{
    FP16_norm(&(w->a));
    FP16_norm(&(w->b));
    FP16_norm(&(w->c));
}

/* SU= 8 */
/* reduce all components of w */
void YYY::FP48_reduce(FP48 *w)
{
    FP16_reduce(&(w->a));
    FP16_reduce(&(w->b));
    FP16_reduce(&(w->c));
}

/* trace function w=trace(x) */
/* SU= 8 */
void YYY::FP48_trace(FP16 *w, FP48 *x)
{
    FP16_imul(w, &(x->a), 3);
    FP16_reduce(w);
}

/* SU= 8 */
/* Output w in hex */
void YYY::FP48_output(FP48 *w)
{
    printf("[");
    FP16_output(&(w->a));
    printf(",");
    FP16_output(&(w->b));
    printf(",");
    FP16_output(&(w->c));
    printf("]");
}

/* Convert g to octet string w */
void YYY::FP48_toOctet(octet *W, FP48 *g)
{
    W->len = 48 * MODBYTES_XXX;
    FP16_toBytes(&(W->val[0]),&(g->c));
    FP16_toBytes(&(W->val[16 * MODBYTES_XXX]),&(g->b));
    FP16_toBytes(&(W->val[32 * MODBYTES_XXX]),&(g->a));
}

/* Restore g from octet string w */
void YYY::FP48_fromOctet(FP48 *g, octet *W)
{
    FP16_fromBytes(&(g->c),&(W->val[0]));
    FP16_fromBytes(&(g->b),&(W->val[16 * MODBYTES_XXX]));
    FP16_fromBytes(&(g->a),&(W->val[32 * MODBYTES_XXX]));
}

/* Move b to a if d=1 */
void YYY::FP48_cmove(FP48 *f, FP48 *g, int d)
{
    FP16_cmove(&(f->a), &(g->a), d);
    FP16_cmove(&(f->b), &(g->b), d);
    FP16_cmove(&(f->c), &(g->c), d);
    d = ~(d - 1);
    f->type ^= (f->type ^ g->type)&d;
}

