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

/* CORE Fp^2 functions */
/* SU=m, m is Stack Usage (no lazy )*/

/* FP2 elements are of the form a+ib, where i is sqrt(-1) */

#include "fp2_YYY.h"

//namespace YYY {
//extern int fp2muls;
//extern int fp2sqrs;
//}

using namespace XXX;

/* test x==0 ? */
/* SU= 8 */
int YYY::FP2_iszilch(FP2 *x)
{
    return (FP_iszilch(&(x->a)) & FP_iszilch(&(x->b)));
}

/* Move g to f if d=1 */
void YYY::FP2_cmove(FP2 *f, FP2 *g, int d)
{
    FP_cmove(&(f->a), &(g->a), d);
    FP_cmove(&(f->b), &(g->b), d);
}

/* test x==1 ? */
/* SU= 48 */
int YYY::FP2_isunity(FP2 *x)
{
    FP one;
    FP_one(&one);
    return (FP_equals(&(x->a), &one) & FP_iszilch(&(x->b)));
}

/* SU= 8 */
/* Fully reduce a and b mod Modulus */
void YYY::FP2_reduce(FP2 *w)
{
    FP_reduce(&(w->a));
    FP_reduce(&(w->b));
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int YYY::FP2_equals(FP2 *x, FP2 *y)
{
    return (FP_equals(&(x->a), &(y->a)) & FP_equals(&(x->b), &(y->b)));
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int YYY::FP2_islarger(FP2 *x)
{
    int cmp;
    if (FP2_iszilch(x)) return 0;
    cmp=FP_islarger(&(x->b));
    if (cmp!=0) return cmp;
    return FP_islarger(&(x->a));
}

void YYY::FP2_toBytes(char *b,FP2 *x)
{
    FP_toBytes(b,&(x->b));
    FP_toBytes(&b[MODBYTES_XXX],&(x->a));
}

void YYY::FP2_fromBytes(FP2 *x,char *b)
{
    FP_fromBytes(&(x->b),b);
    FP_fromBytes(&(x->a),&b[MODBYTES_XXX]);
}

/* Create FP2 from two FPs */
/* SU= 16 */
void YYY::FP2_from_FPs(FP2 *w, FP *x, FP *y)
{
    FP_copy(&(w->a), x);
    FP_copy(&(w->b), y);
}

/* Create FP2 from two BIGS */
/* SU= 16 */
void YYY::FP2_from_BIGs(FP2 *w, BIG x, BIG y)
{
    FP_nres(&(w->a), x);
    FP_nres(&(w->b), y);
}

/* Create FP2 from two ints */
void YYY::FP2_from_ints(FP2 *w, int xa, int xb)
{
    FP a,b;
    FP_from_int(&a,xa);
    FP_from_int(&b,xb);
    FP2_from_FPs(w,&a,&b);
}

/* Create FP2 from FP */
/* SU= 8 */
void YYY::FP2_from_FP(FP2 *w, FP *x)
{
    FP_copy(&(w->a), x);
    FP_zero(&(w->b));
}

/* Create FP2 from BIG */
/* SU= 8 */
void YYY::FP2_from_BIG(FP2 *w, BIG x)
{
    FP_nres(&(w->a), x);
    FP_zero(&(w->b));
}

/* FP2 copy w=x */
/* SU= 16 */
void YYY::FP2_copy(FP2 *w, FP2 *x)
{
    if (w == x) return;
    FP_copy(&(w->a), &(x->a));
    FP_copy(&(w->b), &(x->b));
}

/* FP2 set w=0 */
/* SU= 8 */
void YYY::FP2_zero(FP2 *w)
{
    FP_zero(&(w->a));
    FP_zero(&(w->b));
}

/* FP2 set w=1 */
/* SU= 48 */
void YYY::FP2_one(FP2 *w)
{
    FP one;
    FP_one(&one);
    FP2_from_FP(w, &one);
}

void YYY::FP2_rcopy(FP2 *w,const BIG a,const BIG b)
{
    FP_rcopy(&(w->a),a);
    FP_rcopy(&(w->b),b);
}

int YYY::FP2_sign(FP2 *w)
{
    int p1,p2;
    p1=FP_sign(&(w->a));
    p2=FP_sign(&(w->b));
#ifdef BIG_ENDIAN_SIGN_YYY
    p2 ^= (p1 ^ p2)&FP_iszilch(&(w->b));
    return p2;
#else
    p1 ^= (p1 ^ p2)&FP_iszilch(&(w->a));
    return p1;
#endif

}

/* Set w=-x */
/* SU= 88 */
void YYY::FP2_neg(FP2 *w, FP2 *x)
{
    /* Just one neg! */
    FP m, t;
    FP_add(&m, &(x->a), &(x->b));
    FP_neg(&m, &m);
    FP_add(&t, &m, &(x->b));
    FP_add(&(w->b), &m, &(x->a));
    FP_copy(&(w->a), &t);

}

/* Set w=conj(x) */
/* SU= 16 */
void YYY::FP2_conj(FP2 *w, FP2 *x)
{
    FP_copy(&(w->a), &(x->a));
    FP_neg(&(w->b), &(x->b));
    FP_norm(&(w->b));
}

/* Set w=x+y */
/* SU= 16 */
void YYY::FP2_add(FP2 *w, FP2 *x, FP2 *y)
{
    FP_add(&(w->a), &(x->a), &(y->a));
    FP_add(&(w->b), &(x->b), &(y->b));
}

/* Set w=x-y */
/* Input y MUST be normed */
void YYY::FP2_sub(FP2 *w, FP2 *x, FP2 *y)
{
    FP2 m;
    FP2_neg(&m, y);
    FP2_add(w, x, &m);
}

/* Set w=s*x, where s is FP */
/* SU= 16 */
void YYY::FP2_pmul(FP2 *w, FP2 *x, FP *s)
{
    FP_mul(&(w->a), &(x->a), s);
    FP_mul(&(w->b), &(x->b), s);
}

/* SU= 16 */
/* Set w=s*x, where s is int */
void YYY::FP2_imul(FP2 *w, FP2 *x, int s)
{
    FP_imul(&(w->a), &(x->a), s);
    FP_imul(&(w->b), &(x->b), s);
}

/* Set w=x^2 */
/* SU= 128 */
void YYY::FP2_sqr(FP2 *w, FP2 *x)
{
    FP w1, w3, mb;

    FP_add(&w1, &(x->a), &(x->b));
    FP_neg(&mb, &(x->b));

    FP_add(&w3, &(x->a), &(x->a));
    FP_norm(&w3);
    FP_mul(&(w->b), &w3, &(x->b));

    FP_add(&(w->a), &(x->a), &mb);

    FP_norm(&w1);
    FP_norm(&(w->a));

    FP_mul(&(w->a), &w1, &(w->a));   /* w->a#2 w->a=1 w1&w2=6 w1*w2=2 */

//  YYY::fp2sqrs++;
}


/* Set w=x*y */
/* Inputs MUST be normed  */
/* Now uses Lazy reduction */
void YYY::FP2_mul(FP2 *w, FP2 *x, FP2 *y)
{
    DBIG A, B, E, F, pR;
    BIG C, D, p;

    BIG_rcopy(p, Modulus);
    BIG_dsucopy(pR, p);

// reduce excesses of a and b as required (so product < pR)

    if ((sign64)(x->a.XES + x->b.XES) * (y->a.XES + y->b.XES) > (sign64)FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("FP2 Product too large - reducing it\n");
#endif
        if (x->a.XES > 1) FP_reduce(&(x->a));
        if (x->b.XES > 1) FP_reduce(&(x->b));
    }

    BIG_mul(A, x->a.g, y->a.g);
    BIG_mul(B, x->b.g, y->b.g);

    BIG_add(C, x->a.g, x->b.g); BIG_norm(C);
    BIG_add(D, y->a.g, y->b.g); BIG_norm(D);

    BIG_mul(E, C, D);
    BIG_dadd(F, A, B);
    BIG_dsub(B, pR, B); //

    BIG_dadd(A, A, B);  // A<pR? Not necessarily, but <2pR
    BIG_dsub(E, E, F);  // E<pR ? Yes

    BIG_dnorm(A); FP_mod(w->a.g, A);  w->a.XES = 3; // may drift above 2p...
    BIG_dnorm(E); FP_mod(w->b.g, E);  w->b.XES = 2;

//  YYY::fp2muls++;
}

/* output FP2 in hex format [a,b] */
/* SU= 16 */
void YYY::FP2_output(FP2 *w)
{
    BIG bx, by;
    FP2_reduce(w);
    FP_redc(bx, &(w->a));
    FP_redc(by, &(w->b));
    printf("[");
    BIG_output(bx);
    printf(",");
    BIG_output(by);
    printf("]");
    FP_nres(&(w->a), bx);
    FP_nres(&(w->b), by);
}

/* SU= 8 */
void YYY::FP2_rawoutput(FP2 *w)
{
    printf("[");
    BIG_rawoutput(w->a.g);
    printf(",");
    BIG_rawoutput(w->b.g);
    printf("]");
}


/* Set w=1/x */
/* SU= 128 */
void YYY::FP2_inv(FP2 *w, FP2 *x, FP *h)
{
    FP w1, w2;

    FP2_norm(x);
    FP_sqr(&w1, &(x->a));
    FP_sqr(&w2, &(x->b));
    FP_add(&w1, &w1, &w2);
    FP_inv(&w1, &w1, h);
    FP_mul(&(w->a), &(x->a), &w1);
    FP_neg(&w1, &w1);
    FP_norm(&w1);
    FP_mul(&(w->b), &(x->b), &w1);
}


/* Set w=x/2 */
/* SU= 16 */
void YYY::FP2_div2(FP2 *w, FP2 *x)
{
    FP_div2(&(w->a), &(x->a));
    FP_div2(&(w->b), &(x->b));
}

/* Input MUST be normed */
void YYY::FP2_times_i(FP2 *w)
{
    FP z;
    FP_copy(&z, &(w->a));
    FP_neg(&(w->a), &(w->b));
    FP_copy(&(w->b), &z);

//    Output NOT normed, so use with care
}

/* Set w*=(2^i+sqrt(-1)) */
/* where X^2-(2^i+sqrt(-1)) is irreducible for FP4 */

/* Input MUST be normed */
void YYY::FP2_mul_ip(FP2 *w)
{
    FP2 t;
    int i = QNRI_YYY;

    FP2_copy(&t, w);
    FP2_times_i(w);

// add 2^i.t
    while (i > 0)
    {
        FP2_add(&t, &t, &t);
        FP2_norm(&t);
        i--;
    }
    FP2_add(w, &t, w);

#if TOWER_YYY == POSITOWER
    FP2_norm(w);
    FP2_neg(w, w);  // ***
#endif

//    Output NOT normed, so use with care
}

/* Set w/=(2^i+sqrt(-1)) */
/* Slow */
void YYY::FP2_div_ip(FP2 *w)
{
    FP2 z;
    FP2_norm(w);
    FP2_from_ints(&z, (1 << QNRI_YYY), 1);
    FP2_inv(&z, &z, NULL);
    FP2_mul(w, &z, w);
#if TOWER_YYY == POSITOWER
    FP2_neg(w, w);  // ***
#endif
}

/* SU= 8 */
/* normalise a and b components of w */
void YYY::FP2_norm(FP2 *w)
{
    FP_norm(&(w->a));
    FP_norm(&(w->b));
}

/* Set w=a^b mod m */
/* SU= 208 */
/*
void YYY::FP2_pow(FP2 *r, FP2* a, BIG b)
{
    FP2 w;
    FP one;
    BIG z, zilch;
    int bt;

    BIG_norm(b);
    BIG_copy(z, b);
    FP2_copy(&w, a);
    FP_one(&one);
    BIG_zero(zilch);
    FP2_from_FP(r, &one);
    while (1)
    {
        bt = BIG_parity(z);
        BIG_shr(z, 1);
        if (bt) FP2_mul(r, r, &w);
        if (BIG_comp(z, zilch) == 0) break;
        FP2_sqr(&w, &w);
    }
    FP2_reduce(r);
} */

/* test for x a QR */
int YYY::FP2_qr(FP2 *x,FP *h)
{ /* test x^(p^2-1)/2 = 1 */

    FP2 c;
    FP2_conj(&c,x);
    FP2_mul(&c,&c,x);

    return FP_qr(&(c.a),h);
}

/* sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b))/2+ib/(2*sqrt(a+sqrt(a*a-n*b*b))/2) */

void YYY::FP2_sqrt(FP2 *w, FP2 *u, FP *h)
{
    FP w1, w2, w3, w4, hint;
    FP2 nw;
    int sgn,qr;
    FP2_copy(w, u);
    if (FP2_iszilch(w)) return;

    FP_sqr(&w1, &(w->b));  // b^2
    FP_sqr(&w2, &(w->a));  // a^2
    FP_add(&w1, &w1, &w2); FP_norm(&w1);  // a^2+b^2

    FP_sqrt(&w1, &w1, h);              // sqrt(a^2+b^2)  - could use an input hint to avoid exp!

    FP_add(&w2, &(w->a), &w1);            // a+sqrt(a^2+b^2)
    FP_norm(&w2);
    FP_div2(&w2, &w2);                    // w2=(a+sqrt(a^2+b^2))/2
// **
    FP_div2(&w1,&(w->b));                   // w1=b/2
    qr=FP_qr(&w2,&hint);                    // only exp!

// tweak hint
    FP_neg(&w3,&hint); FP_norm(&w3);    // QNR = -1
    FP_neg(&w4,&w2); FP_norm(&w4);

    FP_cmove(&w2,&w4,1-qr);
    FP_cmove(&hint,&w3,1-qr);

    FP_sqrt(&(w->a),&w2,&hint);             // a=sqrt(w2)
    FP_inv(&w3,&w2,&hint);                  // w3=1/w2
    FP_mul(&w3,&w3,&(w->a));                // w3=1/sqrt(w2)
    FP_mul(&(w->b),&w3,&w1);                // b=(b/2)*1/sqrt(w2)
    FP_copy(&w4,&(w->a));

    FP_cmove(&(w->a),&(w->b),1-qr);
    FP_cmove(&(w->b),&w4,1-qr);


/*

    FP_sqrt(&(w->a),&w2,&hint);             // a=sqrt(w2)
    FP_inv(&w3,&w2,&hint);                  // w3=1/w2
    FP_mul(&w3,&w3,&(w->a));                // w3=1/sqrt(w2)
    FP_mul(&(w->b),&w3,&w1);                // b=(b/2)*1/sqrt(w2)

// tweak hint
    FP_neg(&hint,&hint); FP_norm(&hint);    // QNR = -1
    FP_neg(&w2,&w2); FP_norm(&w2);

    FP_sqrt(&w4,&w2,&hint);                 // w4=sqrt(w2)
    FP_inv(&w3,&w2,&hint);                  // w3=1/w2    
    FP_mul(&w3,&w3,&w4);                    // w3=1/sqrt(w2)
    FP_mul(&w3,&w3,&w1);                    // w3=(b/2)*1/sqrt(w2)

    FP_cmove(&(w->a),&w3,1-qr);
    FP_cmove(&(w->b),&w4,1-qr);
*/
// return +ve root
    sgn=FP2_sign(w);
    FP2_neg(&nw,w); FP2_norm(&nw);
    FP2_cmove(w,&nw,sgn);
}

void YYY::FP2_rand(FP2 *x,csprng *rng)
{
    FP_rand(&(x->a),rng);
    FP_rand(&(x->b),rng);
}
