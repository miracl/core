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

/* CORE Fp^24 functions */
/* SU=m, m is Stack Usage (no lazy )*/
/* FP24 elements are of the form a+i.b+i^2.c */

#include "fp24_YYY.h"
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
static void FP24_select(YYY::FP24 *f, YYY::FP24 g[], sign32 b)
{
    YYY::FP24 invf;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    FP24_cmove(f, &g[0], teq(babs, 0)); // conditional move
    FP24_cmove(f, &g[1], teq(babs, 1));
    FP24_cmove(f, &g[2], teq(babs, 2));
    FP24_cmove(f, &g[3], teq(babs, 3));
    FP24_cmove(f, &g[4], teq(babs, 4));
    FP24_cmove(f, &g[5], teq(babs, 5));
    FP24_cmove(f, &g[6], teq(babs, 6));
    FP24_cmove(f, &g[7], teq(babs, 7));

    FP24_copy(&invf, f);
    FP24_conj(&invf, &invf); // 1/f
    FP24_cmove(f, &invf, (int)(m & 1));
}

/* test x==0 ? */
/* SU= 8 */
int YYY::FP24_iszilch(FP24 *x)
{
    if (FP8_iszilch(&(x->a)) && FP8_iszilch(&(x->b)) && FP8_iszilch(&(x->c))) return 1;
    return 0;
}

/* test x==1 ? */
/* SU= 8 */
int YYY::FP24_isunity(FP24 *x)
{
    if (FP8_isunity(&(x->a)) && FP8_iszilch(&(x->b)) && FP8_iszilch(&(x->c))) return 1;
    return 0;
}

/* FP24 copy w=x */
/* SU= 16 */
void YYY::FP24_copy(FP24 *w, FP24 *x)
{
    if (x == w) return;
    FP8_copy(&(w->a), &(x->a));
    FP8_copy(&(w->b), &(x->b));
    FP8_copy(&(w->c), &(x->c));
    w->type = x->type;
}

/* FP24 w=1 */
/* SU= 8 */
void YYY::FP24_one(FP24 *w)
{
    FP8_one(&(w->a));
    FP8_zero(&(w->b));
    FP8_zero(&(w->c));
    w->type = FP_UNITY;

}

void YYY::FP24_zero(FP24 *w)
{
    FP8_zero(&(w->a));
    FP8_zero(&(w->b));
    FP8_zero(&(w->c));
    w->type = FP_ZILCH;

}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int YYY::FP24_equals(FP24 *x, FP24 *y)
{
    if (FP8_equals(&(x->a), &(y->a)) && FP8_equals(&(x->b), &(y->b)) && FP8_equals(&(x->c), &(y->c)))
        return 1;
    return 0;
}

/* Set w=conj(x) */
/* SU= 8 */
void YYY::FP24_conj(FP24 *w, FP24 *x)
{
    FP24_copy(w, x);
    FP8_conj(&(w->a), &(w->a));
    FP8_nconj(&(w->b), &(w->b));
    FP8_conj(&(w->c), &(w->c));
}

/* Create FP24 from FP8 */
/* SU= 8 */
void YYY::FP24_from_FP8(FP24 *w, FP8 *a)
{
    FP8_copy(&(w->a), a);
    FP8_zero(&(w->b));
    FP8_zero(&(w->c));
    w->type = FP_SPARSEST;
}

/* Create FP24 from 3 FP8's */
/* SU= 16 */
void YYY::FP24_from_FP8s(FP24 *w, FP8 *a, FP8 *b, FP8 *c)
{
    FP8_copy(&(w->a), a);
    FP8_copy(&(w->b), b);
    FP8_copy(&(w->c), c);
    w->type = FP_DENSE;
}

/* Granger-Scott Unitary Squaring. This does not benefit from lazy reduction */
/* SU= 600 */
void YYY::FP24_usqr(FP24 *w, FP24 *x)
{
    FP8 A, B, C, D;

    FP8_copy(&A, &(x->a));

    FP8_sqr(&(w->a), &(x->a));
    FP8_add(&D, &(w->a), &(w->a));
    FP8_add(&(w->a), &D, &(w->a));

    FP8_norm(&(w->a));
    FP8_nconj(&A, &A);

    FP8_add(&A, &A, &A);
    FP8_add(&(w->a), &(w->a), &A);
    FP8_sqr(&B, &(x->c));
    FP8_times_i(&B);

    FP8_add(&D, &B, &B);
    FP8_add(&B, &B, &D);
    FP8_norm(&B);

    FP8_sqr(&C, &(x->b));

    FP8_add(&D, &C, &C);
    FP8_add(&C, &C, &D);

    FP8_norm(&C);
    FP8_conj(&(w->b), &(x->b));
    FP8_add(&(w->b), &(w->b), &(w->b));
    FP8_nconj(&(w->c), &(x->c));

    FP8_add(&(w->c), &(w->c), &(w->c));
    FP8_add(&(w->b), &B, &(w->b));
    FP8_add(&(w->c), &C, &(w->c));
    w->type = FP_DENSE;

    FP24_reduce(w);     /* reduce here as in pow function repeated squarings would trigger multiple reductions */
}

/* FP24 squaring w=x^2 */
/* SU= 600 */
void YYY::FP24_sqr(FP24 *w, FP24 *x)
{
    /* Use Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */

    FP8 A, B, C, D;

    if (x->type <= FP_UNITY)
    {
        FP24_copy(w, x);
        return;
    }

    FP8_sqr(&A, &(x->a));
    FP8_mul(&B, &(x->b), &(x->c));
    FP8_add(&B, &B, &B);
    FP8_norm(&B);
    FP8_sqr(&C, &(x->c));

    FP8_mul(&D, &(x->a), &(x->b));
    FP8_add(&D, &D, &D);

    FP8_add(&(w->c), &(x->a), &(x->c));
    FP8_add(&(w->c), &(x->b), &(w->c));
    FP8_norm(&(w->c));

    FP8_sqr(&(w->c), &(w->c));

    FP8_copy(&(w->a), &A);
    FP8_add(&A, &A, &B);

    FP8_norm(&A);

    FP8_add(&A, &A, &C);
    FP8_add(&A, &A, &D);

    FP8_norm(&A);

    FP8_neg(&A, &A);
    FP8_times_i(&B);
    FP8_times_i(&C);

    FP8_add(&(w->a), &(w->a), &B);
    FP8_add(&(w->b), &C, &D);
    FP8_add(&(w->c), &(w->c), &A);

    if (x->type == FP_SPARSER || x->type == FP_SPARSEST)
        w->type = FP_SPARSE;
    else
        w->type = FP_DENSE;

    FP24_norm(w);
}

// Use FP24_mul when both multiplicands are dense
// Use FP24smul when it is known that both multiplicands are line functions
// Use FP24ssmul when it is suspected that one or both multiplicands could have some sparsity


/* FP24 full multiplication w=w*y */
void YYY::FP24_mul(FP24 *w, FP24 *y)
{
    FP8 z0, z1, z2, z3, t0, t1;

    FP8_mul(&z0, &(w->a), &(y->a));
    FP8_mul(&z2, &(w->b), &(y->b)); //

    FP8_add(&t0, &(w->a), &(w->b));
    FP8_add(&t1, &(y->a), &(y->b)); //

    FP8_norm(&t0);
    FP8_norm(&t1);

    FP8_mul(&z1, &t0, &t1);
    FP8_add(&t0, &(w->b), &(w->c));
    FP8_add(&t1, &(y->b), &(y->c)); //

    FP8_norm(&t0);
    FP8_norm(&t1);

    FP8_mul(&z3, &t0, &t1);

    FP8_neg(&t0, &z0);
    FP8_neg(&t1, &z2);

    FP8_add(&z1, &z1, &t0); // z1=z1-z0
    FP8_add(&(w->b), &z1, &t1); // z1=z1-z2
    FP8_add(&z3, &z3, &t1);      // z3=z3-z2
    FP8_add(&z2, &z2, &t0);      // z2=z2-z0

    FP8_add(&t0, &(w->a), &(w->c));
    FP8_add(&t1, &(y->a), &(y->c));

    FP8_norm(&t0);
    FP8_norm(&t1);

    FP8_mul(&t0, &t1, &t0);
    FP8_add(&z2, &z2, &t0);

    FP8_mul(&t0, &(w->c), &(y->c));
    FP8_neg(&t1, &t0);

    FP8_add(&(w->c), &z2, &t1);
    FP8_add(&z3, &z3, &t1);
    FP8_times_i(&t0);
    FP8_add(&(w->b), &(w->b), &t0);
    FP8_norm(&z3);
    FP8_times_i(&z3);
    FP8_add(&(w->a), &z0, &z3);

    FP24_norm(w);
    w->type = FP_DENSE;
}

/* FP24 full multiplication w=w*y */
/* Supports sparse multiplicands */
/* Usually w is denser than y */
void YYY::FP24_ssmul(FP24 *w, FP24 *y)
{
    FP8 z0, z1, z2, z3, t0, t1;
    if (w->type == FP_UNITY)
    {
        FP24_copy(w, y);
        return;
    }
    if (y->type == FP_UNITY)
        return;

    if (y->type >= FP_SPARSE)
    {
        FP8_mul(&z0, &(w->a), &(y->a)); // xa.ya   always 11x11

#if SEXTIC_TWIST_ZZZ == M_TYPE
        if (y->type == FP_SPARSE || w->type == FP_SPARSE)
        {
            FP4_mul(&z2.b, &(w->b).b, &(y->b).b);
            FP4_zero(&z2.a);
            if (y->type != FP_SPARSE)
                FP4_mul(&z2.a, &(w->b).b, &(y->b).a);
            if (w->type != FP_SPARSE)
                FP4_mul(&z2.a, &(w->b).a, &(y->b).b);
            FP8_times_i(&z2);
        }
        else
#endif
            FP8_mul(&z2, &(w->b), &(y->b)); // xb.yb  could be 00x00 or 01x01 or or 10x10 or 11x00 or 11x10 or 11x01 or 11x11

        FP8_add(&t0, &(w->a), &(w->b)); // (xa+xb)
        FP8_add(&t1, &(y->a), &(y->b)); // (ya+yb)

        FP8_norm(&t0);
        FP8_norm(&t1);

        FP8_mul(&z1, &t0, &t1); // (xa+xb)(ya+yb)  always 11x11
        FP8_add(&t0, &(w->b), &(w->c)); // (xb+xc)
        FP8_add(&t1, &(y->b), &(y->c)); // (yb+yc)

        FP8_norm(&t0);
        FP8_norm(&t1);

        FP8_mul(&z3, &t0, &t1); // (xb+xc)(yb+yc)   could be anything...
        FP8_neg(&t0, &z0);      // -(xa.ya)
        FP8_neg(&t1, &z2);      // -(xb.yb)

        FP8_add(&z1, &z1, &t0);
        FP8_add(&(w->b), &z1, &t1); // /wb = (xa+xb)(ya+yb) -(xa.ya) -(xb.yb)                     = xa.yb + xb.ya

        FP8_add(&z3, &z3, &t1);      // (xb+xc)(yb+yc) -(xb.yb)
        FP8_add(&z2, &z2, &t0);      // (xb.yb) - (xa.ya)

        FP8_add(&t0, &(w->a), &(w->c)); // (xa+xc)
        FP8_add(&t1, &(y->a), &(y->c)); // (ya+yc)

        FP8_norm(&t0);
        FP8_norm(&t1);

        FP8_mul(&t0, &t1, &t0); // (xa+xc)(ya+yc)    always 11x11
        FP8_add(&z2, &z2, &t0); // (xb.yb) - (xa.ya) + (xa+xc)(ya+yc)

#if SEXTIC_TWIST_ZZZ == D_TYPE
        if (y->type == FP_SPARSE || w->type == FP_SPARSE)
        {
            FP4_mul(&t0.a, &(w->c).a, &(y->c).a);
            FP4_zero(&t0.b);
            if (y->type != FP_SPARSE)
                FP4_mul(&t0.b, &(w->c).a, &(y->c).b);
            if (w->type != FP_SPARSE)
                FP4_mul(&t0.b, &(w->c).b, &(y->c).a);
        }
        else
#endif
            FP8_mul(&t0, &(w->c), &(y->c)); // (xc.yc)  could be anything

        FP8_neg(&t1, &t0);            // -(xc.yc)

        FP8_add(&(w->c), &z2, &t1);     // wc = (xb.yb) - (xa.ya) + (xa+xc)(ya+yc) - (xc.yc)    =  xb.yb + xc.ya + xa.yc
        FP8_add(&z3, &z3, &t1);         // (xb+xc)(yb+yc) -(xb.yb) - (xc.yc)                    =  xb.yc + xc.yb
        FP8_times_i(&t0);               // i.(xc.yc)
        FP8_add(&(w->b), &(w->b), &t0); // wb = (xa+xb)(ya+yb) -(xa.ya) -(xb.yb) +i(xc.yc)
        FP8_norm(&z3);
        FP8_times_i(&z3);               // i[(xb+xc)(yb+yc) -(xb.yb) - (xc.yc)]                 = i(xb.yc + xc.yb)
        FP8_add(&(w->a), &z0, &z3);     // wa = xa.ya + i(xb.yc + xc.yb)
    } else {
        if (w->type == FP_SPARSER || w->type == FP_SPARSEST)
        {
            FP24_smul(w, y);
            return;
        }
// dense by sparser - 13m
#if SEXTIC_TWIST_ZZZ == D_TYPE
        FP8_copy(&z3, &(w->b));
        FP8_mul(&z0, &(w->a), &(y->a));

        if (y->type == FP_SPARSEST)
            FP8_tmul(&z2, &(w->b), &(y->b).a.a.a);
        else
            FP8_pmul(&z2, &(w->b), &(y->b).a);


        FP8_add(&(w->b), &(w->a), &(w->b));
        FP8_copy(&t1, &(y->a));
        FP4_add(&t1.a, &t1.a, &(y->b).a);

        FP8_norm(&t1);
        FP8_norm(&(w->b));

        FP8_mul(&(w->b), &(w->b), &t1);
        FP8_add(&z3, &z3, &(w->c));
        FP8_norm(&z3);

        if (y->type == FP_SPARSEST)
            FP8_tmul(&z3, &z3, &(y->b).a.a.a);
        else
            FP8_pmul(&z3, &z3, &(y->b).a);

        FP8_neg(&t0, &z0);
        FP8_neg(&t1, &z2);

        FP8_add(&(w->b), &(w->b), &t0); // z1=z1-z0
        FP8_add(&(w->b), &(w->b), &t1); // z1=z1-z2

        FP8_add(&z3, &z3, &t1);      // z3=z3-z2
        FP8_add(&z2, &z2, &t0);      // z2=z2-z0

        FP8_add(&t0, &(w->a), &(w->c));
        FP8_norm(&t0);
        FP8_norm(&z3);

        FP8_mul(&t0, &(y->a), &t0);
        FP8_add(&(w->c), &z2, &t0);

        FP8_times_i(&z3);
        FP8_add(&(w->a), &z0, &z3);
#endif
#if SEXTIC_TWIST_ZZZ == M_TYPE
        FP8_mul(&z0, &(w->a), &(y->a));
        FP8_add(&t0, &(w->a), &(w->b));
        FP8_norm(&t0);

        FP8_mul(&z1, &t0, &(y->a));
        FP8_add(&t0, &(w->b), &(w->c));
        FP8_norm(&t0);


        if (y->type == FP_SPARSEST)
            FP8_tmul(&z3, &t0, &(y->c).b.a.a);
        else
            FP8_pmul(&z3, &t0, &(y->c).b);


        FP8_times_i(&z3);

        FP8_neg(&t0, &z0);
        FP8_add(&z1, &z1, &t0); // z1=z1-z0

        FP8_copy(&(w->b), &z1);
        FP8_copy(&z2, &t0);

        FP8_add(&t0, &(w->a), &(w->c));
        FP8_add(&t1, &(y->a), &(y->c));

        FP8_norm(&t0);
        FP8_norm(&t1);

        FP8_mul(&t0, &t1, &t0);
        FP8_add(&z2, &z2, &t0);

        if (y->type == FP_SPARSEST)
            FP8_tmul(&t0, &(w->c), &(y->c).b.a.a);
        else
            FP8_pmul(&t0, &(w->c), &(y->c).b);


        FP8_times_i(&t0);
        FP8_neg(&t1, &t0);
        FP8_times_i(&t0);

        FP8_add(&(w->c), &z2, &t1);
        FP8_add(&z3, &z3, &t1);

        FP8_add(&(w->b), &(w->b), &t0);
        FP8_norm(&z3);
        FP8_times_i(&z3);
        FP8_add(&(w->a), &z0, &z3);
#endif
    }
    w->type = FP_DENSE;
    FP24_norm(w);
}

/* FP24 multiplication w=w*y */
/* catering for special case that arises from special form of ATE pairing line function */
/* w and y are both sparser line functions - cost = 6m */
void YYY::FP24_smul(FP24 *w, FP24 *y)
{
    FP4 w1, w2, w3, ta, tb, tc, td, te, t;


#if SEXTIC_TWIST_ZZZ == D_TYPE
    FP4_mul(&w1, &(w->a).a, &(y->a).a); // A1.A2
    FP4_mul(&w2, &(w->a).b, &(y->a).b); // B1.B2

    if (y->type == FP_SPARSEST || w->type == FP_SPARSEST)
    {
        if (y->type == FP_SPARSEST && w->type == FP_SPARSEST)
        {
            FP4_zero(&w3);
            FP_mul(&w3.a.a, &(w->b).a.a.a, &(y->b).a.a.a);
        } else {
            if (y->type != FP_SPARSEST)
                FP4_qmul(&w3, &(y->b).a, &(w->b).a.a.a);
            if (w->type != FP_SPARSEST)
                FP4_qmul(&w3, &(w->b).a, &(y->b).a.a.a);
        }
    }
    else
        FP4_mul(&w3, &(w->b).a, &(y->b).a); // C1.C2

    FP4_add(&ta, &(w->a).a, &(w->a).b); // A1+B1
    FP4_add(&tb, &(y->a).a, &(y->a).b); // A2+B2
    FP4_norm(&ta);
    FP4_norm(&tb);
    FP4_mul(&tc, &ta, &tb);         // (A1+B1)(A2+B2)
    FP4_add(&t, &w1, &w2);
    FP4_neg(&t, &t);
    FP4_add(&tc, &tc, &t);          // (A1+B1)(A2+B2)-A1.A2-B1*B2 =  (A1.B2+A2.B1)

    FP4_add(&ta, &(w->a).a, &(w->b).a); // A1+C1
    FP4_add(&tb, &(y->a).a, &(y->b).a); // A2+C2
    FP4_norm(&ta);
    FP4_norm(&tb);
    FP4_mul(&td, &ta, &tb);         // (A1+C1)(A2+C2)
    FP4_add(&t, &w1, &w3);
    FP4_neg(&t, &t);
    FP4_add(&td, &td, &t);          // (A1+C1)(A2+C2)-A1.A2-C1*C2 =  (A1.C2+A2.C1)

    FP4_add(&ta, &(w->a).b, &(w->b).a); // B1+C1
    FP4_add(&tb, &(y->a).b, &(y->b).a); // B2+C2
    FP4_norm(&ta);
    FP4_norm(&tb);
    FP4_mul(&te, &ta, &tb);         // (B1+C1)(B2+C2)
    FP4_add(&t, &w2, &w3);
    FP4_neg(&t, &t);
    FP4_add(&te, &te, &t);          // (B1+C1)(B2+C2)-B1.B2-C1*C2 =  (B1.C2+B2.C1)

    FP4_times_i(&w2);
    FP4_add(&w1, &w1, &w2);
    FP8_from_FP4s(&(w->a), &w1, &tc);
    FP8_from_FP4s(&(w->b), &td, &te); // only norm these 2
    FP8_from_FP4(&(w->c), &w3);

    FP8_norm(&(w->a));
    FP8_norm(&(w->b));
#endif
#if SEXTIC_TWIST_ZZZ == M_TYPE
    FP4_mul(&w1, &(w->a).a, &(y->a).a); // A1.A2
    FP4_mul(&w2, &(w->a).b, &(y->a).b); // B1.B2

    if (y->type == FP_SPARSEST || w->type == FP_SPARSEST)
    {
        if (y->type == FP_SPARSEST && w->type == FP_SPARSEST)
        {
            FP4_zero(&w3);
            FP_mul(&w3.a.a, &(w->c).b.a.a, &(y->c).b.a.a);
        } else {
            if (y->type != FP_SPARSEST)
                FP4_qmul(&w3, &(y->c).b, &(w->c).b.a.a);
            if (w->type != FP_SPARSEST)
                FP4_qmul(&w3, &(w->c).b, &(y->c).b.a.a);
        }
    }
    else
        FP4_mul(&w3, &(w->c).b, &(y->c).b); // F1.F2

    FP4_add(&ta, &(w->a).a, &(w->a).b); // A1+B1
    FP4_add(&tb, &(y->a).a, &(y->a).b); // A2+B2
    FP4_norm(&ta);
    FP4_norm(&tb);
    FP4_mul(&tc, &ta, &tb);         // (A1+B1)(A2+B2)
    FP4_add(&t, &w1, &w2);
    FP4_neg(&t, &t);
    FP4_add(&tc, &tc, &t);          // (A1+B1)(A2+B2)-A1.A2-B1*B2 =  (A1.B2+A2.B1)

    FP4_add(&ta, &(w->a).a, &(w->c).b); // A1+F1
    FP4_add(&tb, &(y->a).a, &(y->c).b); // A2+F2
    FP4_norm(&ta);
    FP4_norm(&tb);
    FP4_mul(&td, &ta, &tb);         // (A1+F1)(A2+F2)
    FP4_add(&t, &w1, &w3);
    FP4_neg(&t, &t);
    FP4_add(&td, &td, &t);          // (A1+F1)(A2+F2)-A1.A2-F1*F2 =  (A1.F2+A2.F1)

    FP4_add(&ta, &(w->a).b, &(w->c).b); // B1+F1
    FP4_add(&tb, &(y->a).b, &(y->c).b); // B2+F2
    FP4_norm(&ta);
    FP4_norm(&tb);
    FP4_mul(&te, &ta, &tb);         // (B1+F1)(B2+F2)
    FP4_add(&t, &w2, &w3);
    FP4_neg(&t, &t);
    FP4_add(&te, &te, &t);          // (B1+F1)(B2+F2)-B1.B2-F1*F2 =  (B1.F2+B2.F1)

    FP4_times_i(&w2);
    FP4_add(&w1, &w1, &w2);
    FP8_from_FP4s(&(w->a), &w1, &tc);

    FP4_times_i(&w3);
    FP4_norm(&w3);
    FP8_from_FP4H(&(w->b), &w3);

    FP4_norm(&te);
    FP4_times_i(&te);
    FP8_from_FP4s(&(w->c), &te, &td);

    FP8_norm(&(w->a));
    FP8_norm(&(w->c));
#endif

    w->type = FP_SPARSE;
}

/* Set w=1/x */
/* SU= 600 */
void YYY::FP24_inv(FP24 *w, FP24 *x)
{
    FP8 f0, f1, f2, f3;

    FP8_sqr(&f0, &(x->a));
    FP8_mul(&f1, &(x->b), &(x->c));
    FP8_times_i(&f1);
    FP8_sub(&f0, &f0, &f1); /* y.a */
    FP8_norm(&f0);

    FP8_sqr(&f1, &(x->c));
    FP8_times_i(&f1);
    FP8_mul(&f2, &(x->a), &(x->b));
    FP8_sub(&f1, &f1, &f2); /* y.b */
    FP8_norm(&f1);

    FP8_sqr(&f2, &(x->b));
    FP8_mul(&f3, &(x->a), &(x->c));
    FP8_sub(&f2, &f2, &f3); /* y.c */
    FP8_norm(&f2);

    FP8_mul(&f3, &(x->b), &f2);
    FP8_times_i(&f3);
    FP8_mul(&(w->a), &f0, &(x->a));
    FP8_add(&f3, &(w->a), &f3);
    FP8_mul(&(w->c), &f1, &(x->c));
    FP8_times_i(&(w->c));

    FP8_add(&f3, &(w->c), &f3);
    FP8_norm(&f3);

    FP8_inv(&f3, &f3, NULL);
    FP8_mul(&(w->a), &f0, &f3);
    FP8_mul(&(w->b), &f1, &f3);
    FP8_mul(&(w->c), &f2, &f3);
    w->type = FP_DENSE;
}

/* constant time powering by small integer of max length bts */

void YYY::FP24_pinpow(FP24 *r, int e, int bts)
{
    int i, b;
    FP24 R[2];

    FP24_one(&R[0]);
    FP24_copy(&R[1], r);

    for (i = bts - 1; i >= 0; i--)
    {
        b = (e >> i) & 1;
        FP24_mul(&R[1 - b], &R[b]);
        FP24_usqr(&R[b], &R[b]);
    }
    FP24_copy(r, &R[0]);
}

/* Compressed powering of unitary elements y=x^(e mod r) */

/*
void YYY::FP24_compow(FP8 *c, FP24 *x, BIG e, BIG r)
{
    FP24 g1, g2;
    FP8 cp, cpm1, cpm2;
    FP2 f;
    BIG q, a, b, m;

    BIG_rcopy(a, Fra);
    BIG_rcopy(b, Frb);
    FP2_from_BIGs(&f, a, b);

    BIG_rcopy(q, Modulus);

    FP24_copy(&g1, x);
    FP24_copy(&g2, x);

    BIG_copy(m, q);
    BIG_mod(m, r);

    BIG_copy(a, e);
    BIG_mod(a, m);

    BIG_copy(b, e);
    BIG_sdiv(b, m);

    FP24_trace(c, &g1);

    if (BIG_iszilch(b))
    {
        FP8_xtr_pow(c, c, e);
        return;
    }

    FP24_frob(&g2, &f, 1);
    FP24_trace(&cp, &g2);
    FP24_conj(&g1, &g1);
    FP24_mul(&g2, &g1);
    FP24_trace(&cpm1, &g2);
    FP24_mul(&g2, &g1);

    FP24_trace(&cpm2, &g2);

    FP8_xtr_pow2(c, &cp, c, &cpm1, &cpm2, a, b);

}
*/
/* Note this is simple square and multiply, so not side-channel safe */

void YYY::FP24_pow(FP24 *r, FP24 *a, BIG b)
{
    FP24 w, sf;
    BIG b1, b3;
    int i, nb, bt;
    BIG_copy(b1, b);
    BIG_norm(b1);
    BIG_pmul(b3, b1, 3);
    BIG_norm(b3);

    FP24_copy(&sf, a);
    FP24_norm(&sf);
    FP24_copy(&w, &sf);

    if (BIG_iszilch(b3))
    {
        FP24_one(r);
        return;
    }

    nb = BIG_nbits(b3);
    for (i = nb - 2; i >= 1; i--)
    {
        FP24_usqr(&w, &w);
        bt = BIG_bit(b3, i) - BIG_bit(b1, i);
        if (bt == 1)
            FP24_mul(&w, &sf);
        if (bt == -1)
        {
            FP24_conj(&sf, &sf);
            FP24_mul(&w, &sf);
            FP24_conj(&sf, &sf);
        }
    }

    FP24_copy(r, &w);
    FP24_reduce(r);
}

/* p=q0^u0.q1^u1.q2^u2.q3^u3... */
/* Side channel attack secure */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf

void YYY::FP24_pow8(FP24 *p, FP24 *q, BIG u[8])
{
    int i, j, k, nb, pb1, pb2, bt;
    FP24 g1[8], g2[8], r;
    BIG t[8], mt;
    sign8 w1[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s1[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 w2[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s2[NLEN_XXX * BASEBITS_XXX + 1];
//    FP fx,fy;
//  FP2 X;
    /*
        FP_rcopy(&fx,Fra);
        FP_rcopy(&fy,Frb);
        FP2_from_FPs(&X,&fx,&fy);
    */
    for (i = 0; i < 8; i++)
        BIG_copy(t[i], u[i]);

// Precomputed table
    FP24_copy(&g1[0], &q[0]); // q[0]
    FP24_copy(&g1[1], &g1[0]);
    FP24_mul(&g1[1], &q[1]); // q[0].q[1]
    FP24_copy(&g1[2], &g1[0]);
    FP24_mul(&g1[2], &q[2]); // q[0].q[2]
    FP24_copy(&g1[3], &g1[1]);
    FP24_mul(&g1[3], &q[2]); // q[0].q[1].q[2]
    FP24_copy(&g1[4], &g1[0]);
    FP24_mul(&g1[4], &q[3]); // q[0].q[3]
    FP24_copy(&g1[5], &g1[1]);
    FP24_mul(&g1[5], &q[3]); // q[0].q[1].q[3]
    FP24_copy(&g1[6], &g1[2]);
    FP24_mul(&g1[6], &q[3]); // q[0].q[2].q[3]
    FP24_copy(&g1[7], &g1[3]);
    FP24_mul(&g1[7], &q[3]); // q[0].q[1].q[2].q[3]

    FP24_copy(&g2[0], &q[4]); // q[0]
    FP24_copy(&g2[1], &g2[0]);
    FP24_mul(&g2[1], &q[5]); // q[0].q[1]
    FP24_copy(&g2[2], &g2[0]);
    FP24_mul(&g2[2], &q[6]); // q[0].q[2]
    FP24_copy(&g2[3], &g2[1]);
    FP24_mul(&g2[3], &q[6]); // q[0].q[1].q[2]
    FP24_copy(&g2[4], &g2[0]);
    FP24_mul(&g2[4], &q[7]); // q[0].q[3]
    FP24_copy(&g2[5], &g2[1]);
    FP24_mul(&g2[5], &q[7]); // q[0].q[1].q[3]
    FP24_copy(&g2[6], &g2[2]);
    FP24_mul(&g2[6], &q[7]); // q[0].q[2].q[3]
    FP24_copy(&g2[7], &g2[3]);
    FP24_mul(&g2[7], &q[7]); // q[0].q[1].q[2].q[3]

// Use Frobenius
    /*
        for (i=0;i<8;i++)
        {
            FP24_copy(&g2[i],&g1[i]);
            FP24_frob(&g2[i],&X,4);
        }
    */
// Make it odd
    pb1 = 1 - BIG_parity(t[0]);
    BIG_inc(t[0], pb1);
    BIG_norm(t[0]);

    pb2 = 1 - BIG_parity(t[4]);
    BIG_inc(t[4], pb2);
    BIG_norm(t[4]);

// Number of bits
    BIG_zero(mt);
    for (i = 0; i < 8; i++)
    {
        BIG_or(mt, mt, t[i]);
    }
    nb = 1 + BIG_nbits(mt);

// Sign pivot
    s1[nb - 1] = 1;
    s2[nb - 1] = 1;
    for (i = 0; i < nb - 1; i++)
    {
        BIG_fshr(t[0], 1);
        s1[i] = 2 * BIG_parity(t[0]) - 1;
        BIG_fshr(t[4], 1);
        s2[i] = 2 * BIG_parity(t[4]) - 1;
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
    }

// Main loop
    FP24_select(p, g1, 2 * w1[nb - 1] + 1);
    FP24_select(&r, g2, 2 * w2[nb - 1] + 1);
    FP24_mul(p, &r);
    for (i = nb - 2; i >= 0; i--)
    {
        FP24_usqr(p, p);
        FP24_select(&r, g1, 2 * w1[i] + s1[i]);
        FP24_mul(p, &r);
        FP24_select(&r, g2, 2 * w2[i] + s2[i]);
        FP24_mul(p, &r);
    }

// apply correction
    FP24_conj(&r, &q[0]);
    FP24_mul(&r, p);
    FP24_cmove(p, &r, pb1);
    FP24_conj(&r, &q[4]);
    FP24_mul(&r, p);
    FP24_cmove(p, &r, pb2);

    FP24_reduce(p);
}

/* Set w=w^p using Frobenius */
/* SU= 160 */
void YYY::FP24_frob(FP24 *w, FP2 *f, int n)
{
    int i;
    FP4 X2, X4;
    FP2 f3, f2;             // f=(1+i)^(p-7)/12
    FP2_sqr(&f2, f);    //
    FP2_mul(&f3, &f2, f); // f3=f^3=(1+i)^(p-7)/4

    FP2_mul_ip(&f3);    // f3 = (1+i).f3 = (1+i)^(p-3)/4
    FP2_norm(&f3);

    for (i = 0; i < n; i++)
    {
        FP8_frob(&(w->a), &f3);  // a=a^p
        FP8_frob(&(w->b), &f3);  // b=b^p
        FP8_frob(&(w->c), &f3);  // c=c^p

        FP8_qmul(&(w->b), &(w->b), f); FP8_times_i2(&(w->b));
        FP8_qmul(&(w->c), &(w->c), &f2); FP8_times_i2(&(w->c)); FP8_times_i2(&(w->c));
    }
    w->type = FP_DENSE;
}


/* SU= 8 */
/* normalise all components of w */
void YYY::FP24_norm(FP24 *w)
{
    FP8_norm(&(w->a));
    FP8_norm(&(w->b));
    FP8_norm(&(w->c));
}

/* SU= 8 */
/* reduce all components of w */
void YYY::FP24_reduce(FP24 *w)
{
    FP8_reduce(&(w->a));
    FP8_reduce(&(w->b));
    FP8_reduce(&(w->c));
}

/* trace function w=trace(x) */
/* SU= 8 */
void YYY::FP24_trace(FP8 *w, FP24 *x)
{
    FP8_imul(w, &(x->a), 3);
    FP8_reduce(w);
}

/* SU= 8 */
/* Output w in hex */
void YYY::FP24_output(FP24 *w)
{
    printf("[");
    FP8_output(&(w->a));
    printf(",");
    FP8_output(&(w->b));
    printf(",");
    FP8_output(&(w->c));
    printf("]");
}

/* SU= 64 */
/* Convert g to octet string w */
void YYY::FP24_toOctet(octet *W, FP24 *g)
{
    W->len = 24 * MODBYTES_XXX;
    FP8_toBytes(&(W->val[0]),&(g->c));
    FP8_toBytes(&(W->val[8 * MODBYTES_XXX]),&(g->b));
    FP8_toBytes(&(W->val[16 * MODBYTES_XXX]),&(g->a));
}

/* SU= 24 */
/* Restore g from octet string w */
void YYY::FP24_fromOctet(FP24 *g, octet *W)
{
    FP8_fromBytes(&(g->c),&(W->val[0]));
    FP8_fromBytes(&(g->b),&(W->val[8 * MODBYTES_XXX]));
    FP8_fromBytes(&(g->a),&(W->val[16 * MODBYTES_XXX]));
}

/* Move b to a if d=1 */
void YYY::FP24_cmove(FP24 *f, FP24 *g, int d)
{
    FP8_cmove(&(f->a), &(g->a), d);
    FP8_cmove(&(f->b), &(g->b), d);
    FP8_cmove(&(f->c), &(g->c), d);
    d = ~(d - 1);
    f->type ^= (f->type ^ g->type)&d;
}
