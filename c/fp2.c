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

/* test x==0 ? */
/* SU= 8 */
int FP2_YYY_iszilch(FP2_YYY *x)
{
    return (FP_YYY_iszilch(&(x->a)) & FP_YYY_iszilch(&(x->b)));
}

/* Move b to a if d=1 */
void FP2_YYY_cmove(FP2_YYY *f, FP2_YYY *g, int d)
{
    FP_YYY_cmove(&(f->a), &(g->a), d);
    FP_YYY_cmove(&(f->b), &(g->b), d);
}

/* test x==1 ? */
/* SU= 48 */
int FP2_YYY_isunity(FP2_YYY *x)
{
    FP_YYY one;
    FP_YYY_one(&one);
    return (FP_YYY_equals(&(x->a), &one) & FP_YYY_iszilch(&(x->b)));
}

/* SU= 8 */
/* Fully reduce a and b mod Modulus */
void FP2_YYY_reduce(FP2_YYY *w)
{
    FP_YYY_reduce(&(w->a));
    FP_YYY_reduce(&(w->b));
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int FP2_YYY_equals(FP2_YYY *x, FP2_YYY *y)
{
    return (FP_YYY_equals(&(x->a), &(y->a)) & FP_YYY_equals(&(x->b), &(y->b)));
}


// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP2_YYY_islarger(FP2_YYY *x)
{
    int cmp;
    if (FP2_YYY_iszilch(x)) return 0;
    cmp=FP_YYY_islarger(&(x->b));
    if (cmp!=0) return cmp;
    return FP_YYY_islarger(&(x->a));
}

void FP2_YYY_toBytes(char *b,FP2_YYY *x)
{
    FP_YYY_toBytes(b,&(x->b));
    FP_YYY_toBytes(&b[MODBYTES_XXX],&(x->a));
}

void FP2_YYY_fromBytes(FP2_YYY *x,char *b)
{
    FP_YYY_fromBytes(&(x->b),b);
    FP_YYY_fromBytes(&(x->a),&b[MODBYTES_XXX]);
}


/* Create FP2 from two FPs */
/* SU= 16 */
void FP2_YYY_from_FPs(FP2_YYY *w, FP_YYY *x, FP_YYY *y)
{
    FP_YYY_copy(&(w->a), x);
    FP_YYY_copy(&(w->b), y);
}

/* Create FP2 from two BIGS */
/* SU= 16 */
void FP2_YYY_from_BIGs(FP2_YYY *w, BIG_XXX x, BIG_XXX y)
{
    FP_YYY_nres(&(w->a), x);
    FP_YYY_nres(&(w->b), y);
}

/* Create FP2 from two ints */
void FP2_YYY_from_ints(FP2_YYY *w, int xa, int xb)
{
    FP_YYY a,b;
    FP_YYY_from_int(&a,xa);
    FP_YYY_from_int(&b,xb);
    FP2_YYY_from_FPs(w,&a,&b);
//    BIG_XXX a, b;
//    BIG_XXX_zero(a); BIG_XXX_inc(a, xa); BIG_XXX_norm(a);
//    BIG_XXX_zero(b); BIG_XXX_inc(b, xb); BIG_XXX_norm(b);
//    FP2_YYY_from_BIGs(w, a, b);
}

/* Create FP2 from FP */
/* SU= 8 */
void FP2_YYY_from_FP(FP2_YYY *w, FP_YYY *x)
{
    FP_YYY_copy(&(w->a), x);
    FP_YYY_zero(&(w->b));
}

/* Create FP2 from BIG */
/* SU= 8 */
void FP2_YYY_from_BIG(FP2_YYY *w, BIG_XXX x)
{
    FP_YYY_nres(&(w->a), x);
    FP_YYY_zero(&(w->b));
}

/* FP2 copy w=x */
/* SU= 16 */
void FP2_YYY_copy(FP2_YYY *w, FP2_YYY *x)
{
    if (w == x) return;
    FP_YYY_copy(&(w->a), &(x->a));
    FP_YYY_copy(&(w->b), &(x->b));
}

/* FP2 set w=0 */
/* SU= 8 */
void FP2_YYY_zero(FP2_YYY *w)
{
    FP_YYY_zero(&(w->a));
    FP_YYY_zero(&(w->b));
}

/* FP2 set w=1 */
/* SU= 48 */
void FP2_YYY_one(FP2_YYY *w)
{
    FP_YYY one;
    FP_YYY_one(&one);
    FP2_YYY_from_FP(w, &one);
}

void FP2_YYY_rcopy(FP2_YYY *w,const BIG_XXX a,const BIG_XXX b)
{
    FP_YYY_rcopy(&(w->a),a);
    FP_YYY_rcopy(&(w->b),b);
}

int FP2_YYY_sign(FP2_YYY *w)
{
    int p1,p2;
    p1=FP_YYY_sign(&(w->a));
    p2=FP_YYY_sign(&(w->b));
#ifdef BIG_ENDIAN_SIGN_YYY
    p2 ^= (p1 ^ p2)&FP_YYY_iszilch(&(w->b));
    return p2;
#else
    p1 ^= (p1 ^ p2)&FP_YYY_iszilch(&(w->a));
    return p1;
#endif
}

/* Set w=-x */
/* SU= 88 */
void FP2_YYY_neg(FP2_YYY *w, FP2_YYY *x)
{
    /* Just one neg! */
    FP_YYY m, t;
    FP_YYY_add(&m, &(x->a), &(x->b));
    FP_YYY_neg(&m, &m);
    FP_YYY_add(&t, &m, &(x->b));
    FP_YYY_add(&(w->b), &m, &(x->a));
    FP_YYY_copy(&(w->a), &t);

}

/* Set w=conj(x) */
/* SU= 16 */
void FP2_YYY_conj(FP2_YYY *w, FP2_YYY *x)
{
    FP_YYY_copy(&(w->a), &(x->a));
    FP_YYY_neg(&(w->b), &(x->b));
    FP_YYY_norm(&(w->b));
}

/* Set w=x+y */
/* SU= 16 */
void FP2_YYY_add(FP2_YYY *w, FP2_YYY *x, FP2_YYY *y)
{
    FP_YYY_add(&(w->a), &(x->a), &(y->a));
    FP_YYY_add(&(w->b), &(x->b), &(y->b));
}

/* Set w=x-y */
/* Input y MUST be normed */
void FP2_YYY_sub(FP2_YYY *w, FP2_YYY *x, FP2_YYY *y)
{
    FP2_YYY m;
    FP2_YYY_neg(&m, y);
    FP2_YYY_add(w, x, &m);
}

/* Set w=s*x, where s is FP */
/* SU= 16 */
void FP2_YYY_pmul(FP2_YYY *w, FP2_YYY *x, FP_YYY *s)
{
    FP_YYY_mul(&(w->a), &(x->a), s);
    FP_YYY_mul(&(w->b), &(x->b), s);
}

/* SU= 16 */
/* Set w=s*x, where s is int */
void FP2_YYY_imul(FP2_YYY *w, FP2_YYY *x, int s)
{
    FP_YYY_imul(&(w->a), &(x->a), s);
    FP_YYY_imul(&(w->b), &(x->b), s);
}

/* Set w=x^2 */
/* SU= 128 */
void FP2_YYY_sqr(FP2_YYY *w, FP2_YYY *x)
{
    FP_YYY w1, w3, mb;

    FP_YYY_add(&w1, &(x->a), &(x->b));
    FP_YYY_neg(&mb, &(x->b));

    FP_YYY_add(&w3, &(x->a), &(x->a));
    FP_YYY_norm(&w3);
    FP_YYY_mul(&(w->b), &w3, &(x->b));

    FP_YYY_add(&(w->a), &(x->a), &mb);

    FP_YYY_norm(&w1);
    FP_YYY_norm(&(w->a));

    FP_YYY_mul(&(w->a), &w1, &(w->a));   /* w->a#2 w->a=1 w1&w2=6 w1*w2=2 */
}

/* Set w=x*y */
/* Inputs MUST be normed  */
/* Now uses Lazy reduction */
void FP2_YYY_mul(FP2_YYY *w, FP2_YYY *x, FP2_YYY *y)
{
    DBIG_XXX A, B, E, F, pR;
    BIG_XXX C, D, p;

    BIG_XXX_rcopy(p, Modulus_YYY);
    BIG_XXX_dsucopy(pR, p);

// reduce excesses of a and b as required (so product < pR)

    if ((sign64)(x->a.XES + x->b.XES) * (y->a.XES + y->b.XES) > (sign64)FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("FP2 Product too large - reducing it\n");
#endif
        if (x->a.XES > 1) FP_YYY_reduce(&(x->a));
        if (x->b.XES > 1) FP_YYY_reduce(&(x->b));
    }

    BIG_XXX_mul(A, x->a.g, y->a.g);
    BIG_XXX_mul(B, x->b.g, y->b.g);

    BIG_XXX_add(C, x->a.g, x->b.g);
    BIG_XXX_norm(C);
    BIG_XXX_add(D, y->a.g, y->b.g);
    BIG_XXX_norm(D);

    BIG_XXX_mul(E, C, D);
    BIG_XXX_dadd(F, A, B);
    BIG_XXX_dsub(B, pR, B); //

    BIG_XXX_dadd(A, A, B);  // A<pR? Not necessarily, but <2pR
    BIG_XXX_dsub(E, E, F);  // E<pR ? Yes

    BIG_XXX_dnorm(A);
    FP_YYY_mod(w->a.g, A);
    w->a.XES = 3; // may drift above 2p...
    BIG_XXX_dnorm(E);
    FP_YYY_mod(w->b.g, E);
    w->b.XES = 2;

}

/* output FP2 in hex format [a,b] */
/* SU= 16 */
void FP2_YYY_output(FP2_YYY *w)
{
    BIG_XXX bx, by;
    FP2_YYY_reduce(w);
    FP_YYY_redc(bx, &(w->a));
    FP_YYY_redc(by, &(w->b));
    printf("[");
    BIG_XXX_output(bx);
    printf(",");
    BIG_XXX_output(by);
    printf("]");
    FP_YYY_nres(&(w->a), bx);
    FP_YYY_nres(&(w->b), by);
}

/* SU= 8 */
void FP2_YYY_rawoutput(FP2_YYY *w)
{
    printf("[");
    BIG_XXX_rawoutput(w->a.g);
    printf(",");
    BIG_XXX_rawoutput(w->b.g);
    printf("]");
}


/* Set w=1/x */
/* SU= 128 */
void FP2_YYY_inv(FP2_YYY *w, FP2_YYY *x, FP_YYY *h)
{
    BIG_XXX m, b;
    FP_YYY w1, w2;

    FP2_YYY_norm(x);
    FP_YYY_sqr(&w1, &(x->a));
    FP_YYY_sqr(&w2, &(x->b));
    FP_YYY_add(&w1, &w1, &w2);

    FP_YYY_inv(&w1, &w1, h);

    FP_YYY_mul(&(w->a), &(x->a), &w1);
    FP_YYY_neg(&w1, &w1);
    FP_YYY_norm(&w1);
    FP_YYY_mul(&(w->b), &(x->b), &w1);
}


/* Set w=x/2 */
/* SU= 16 */
void FP2_YYY_div2(FP2_YYY *w, FP2_YYY *x)
{
    FP_YYY_div2(&(w->a), &(x->a));
    FP_YYY_div2(&(w->b), &(x->b));
}

/* Set w*=(1+sqrt(-1)) */
/* where X^2-(1+sqrt(-1)) is irreducible for FP4, assumes p=3 mod 8 */

/* Input MUST be normed */
void FP2_YYY_mul_ip(FP2_YYY *w)
{
    FP2_YYY t;

    int i = QNRI_YYY;

    FP2_YYY_copy(&t, w);
    FP2_YYY_times_i(w);

// add 2^i.t
    while (i > 0)
    {
        FP2_YYY_add(&t, &t, &t);
        FP2_YYY_norm(&t);
        i--;
    }
    FP2_YYY_add(w, &t, w);

#if TOWER_YYY == POSITOWER
    FP2_YYY_norm(w);
    FP2_YYY_neg(w, w);  // ***
#endif
//    Output NOT normed, so use with care
}

/* Set w/=(1+sqrt(-1)) */
/* SU= 88 */
void FP2_YYY_div_ip(FP2_YYY *w)
{
    FP2_YYY z;
    FP2_YYY_norm(w);
    FP2_YYY_from_ints(&z, (1 << QNRI_YYY), 1);
    FP2_YYY_inv(&z, &z, NULL);
    FP2_YYY_mul(w, &z, w);
#if TOWER_YYY == POSITOWER
    FP2_YYY_neg(w, w);  // ***
#endif
}

/* SU= 8 */
/* normalise a and b components of w */
void FP2_YYY_norm(FP2_YYY *w)
{
    FP_YYY_norm(&(w->a));
    FP_YYY_norm(&(w->b));
}

/* Set w=a^b mod m */
/* SU= 208 */
/*
void FP2_YYY_pow(FP2_YYY *r, FP2_YYY* a, BIG_XXX b)
{
    FP2_YYY w;
    FP_YYY one;
    BIG_XXX z, zilch;
    int bt;

    BIG_XXX_norm(b);
    BIG_XXX_copy(z, b);
    FP2_YYY_copy(&w, a);
    FP_YYY_one(&one);
    BIG_XXX_zero(zilch);
    FP2_YYY_from_FP(r, &one);
    while (1)
    {
        bt = BIG_XXX_parity(z);
        BIG_XXX_shr(z, 1);
        if (bt) FP2_YYY_mul(r, r, &w);
        if (BIG_XXX_comp(z, zilch) == 0) break;
        FP2_YYY_sqr(&w, &w);
    }
    FP2_YYY_reduce(r);
}
*/
/* test for x a QR */

int FP2_YYY_qr(FP2_YYY *x, FP_YYY *h)
{ /* test x^(p^2-1)/2 = 1 */
    FP2_YYY c;
    FP2_YYY_conj(&c,x);
    FP2_YYY_mul(&c,&c,x);

    return FP_YYY_qr(&(c.a),h);
}

/* sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b)/2)+ib/(2*sqrt(a+sqrt(a*a-n*b*b)/2)) */

void FP2_YYY_sqrt(FP2_YYY *w, FP2_YYY *u, FP_YYY *h)
{
    FP_YYY w1, w2, w3, w4, hint;
    FP2_YYY nw;
    int sgn,qr;

    FP2_YYY_copy(w, u);
    if (FP2_YYY_iszilch(w)) return;

    FP_YYY_sqr(&w1, &(w->b));
    FP_YYY_sqr(&w2, &(w->a));
    FP_YYY_add(&w1, &w1, &w2);
    FP_YYY_norm(&w1);
    FP_YYY_sqrt(&w1, &w1,h);

    FP_YYY_add(&w2, &(w->a), &w1);
    FP_YYY_norm(&w2);
    FP_YYY_div2(&w2, &w2);

    FP_YYY_div2(&w1,&(w->b));                   // w1=b/2
    qr=FP_YYY_qr(&w2,&hint);                    // only exp!

// tweak hint
    FP_YYY_neg(&w3,&hint); FP_YYY_norm(&w3);    // QNR = -1
    FP_YYY_neg(&w4,&w2); FP_YYY_norm(&w4);

    FP_YYY_cmove(&w2,&w4,1-qr);
    FP_YYY_cmove(&hint,&w3,1-qr);

    FP_YYY_sqrt(&(w->a),&w2,&hint);             // a=sqrt(w2)
    FP_YYY_inv(&w3,&w2,&hint);                  // w3=1/w2
    FP_YYY_mul(&w3,&w3,&(w->a));                // w3=1/sqrt(w2)
    FP_YYY_mul(&(w->b),&w3,&w1);                // b=(b/2)*1/sqrt(w2)
    FP_YYY_copy(&w4,&(w->a));

    FP_YYY_cmove(&(w->a),&(w->b),1-qr);
    FP_YYY_cmove(&(w->b),&w4,1-qr);


/*

    FP_YYY_sqrt(&(w->a),&w2,&hint);             // a=sqrt(w2)
    FP_YYY_inv(&w3,&w2,&hint);                  // w3=1/w2
    FP_YYY_mul(&w3,&w3,&(w->a));                // w3=1/sqrt(w2)
    FP_YYY_mul(&(w->b),&w3,&w1);                // b=(b/2)*1/sqrt(w2)

// tweak hint
    FP_YYY_neg(&hint,&hint); FP_YYY_norm(&hint);    // QNR = -1
    FP_YYY_neg(&w2,&w2); FP_YYY_norm(&w2);

    FP_YYY_sqrt(&w4,&w2,&hint);                 // w4=sqrt(w2)
    FP_YYY_inv(&w3,&w2,&hint);                  // w3=1/w2    
    FP_YYY_mul(&w3,&w3,&w4);                    // w3=1/sqrt(w2)
    FP_YYY_mul(&w3,&w3,&w1);                    // w3=(b/2)*1/sqrt(w2)

    FP_YYY_cmove(&(w->a),&w3,1-qr);
    FP_YYY_cmove(&(w->b),&w4,1-qr);
*/
    sgn=FP2_YYY_sign(w);
    FP2_YYY_neg(&nw,w); FP2_YYY_norm(&nw);
    FP2_YYY_cmove(w,&nw,sgn);

/*
    FP_YYY_sub(&w3, &(w->a), &w1);
    FP_YYY_norm(&w3);
    FP_YYY_div2(&w3, &w3);

    FP_YYY_cmove(&w2,&w3,FP_YYY_qr(&w3,NULL)); // one or the other will be a QR

    FP_YYY_invsqrt(&w3,&(w->a),&w2);
    FP_YYY_mul(&w3,&w3,&(w->a));
    FP_YYY_div2(&w2,&w3);

    FP_YYY_mul(&(w->b), &(w->b), &w2);

    sgn=FP2_YYY_sign(w);
    FP2_YYY_neg(&nw,w); FP2_YYY_norm(&nw);
    FP2_YYY_cmove(w,&nw,sgn); */
}

/* New stuff for ECp4 support */

/* Input MUST be normed */
void FP2_YYY_times_i(FP2_YYY *w)
{
    FP_YYY z;
    FP_YYY_copy(&z, &(w->a));
    FP_YYY_neg(&(w->a), &(w->b));
    FP_YYY_copy(&(w->b), &z);

//    Output NOT normed, so use with care
}

void FP2_YYY_rand(FP2_YYY *x,csprng *rng)
{
    FP_YYY_rand(&(x->a),rng);
    FP_YYY_rand(&(x->b),rng);
}

