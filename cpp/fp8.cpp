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

/* CORE Fp^8 functions */

/* FP8 elements are of the form a+ib, where i is sqrt(sqrt(-1+sqrt(-1))) */

#include "fp8_YYY.h"

using namespace XXX;

/* test x==0 ? */
int YYY::FP8_iszilch(FP8 *x)
{
    return (FP4_iszilch(&(x->a)) & FP4_iszilch(&(x->b)));
}

/* test x==1 ? */
int YYY::FP8_isunity(FP8 *x)
{
    return (FP4_isunity(&(x->a)) & FP4_iszilch(&(x->b)));
}

/* test is w real? That is in a+ib test b is zero */
int YYY::FP8_isreal(FP8 *w)
{
    return FP4_iszilch(&(w->b));
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int YYY::FP8_islarger(FP8 *x)
{
    int cmp;
    if (FP8_iszilch(x)) return 0;
    cmp=FP4_islarger(&(x->b));
    if (cmp!=0) return cmp;
    return FP4_islarger(&(x->a));
}

void YYY::FP8_toBytes(char *b,FP8 *x)
{
    FP4_toBytes(b,&(x->b));
    FP4_toBytes(&b[4*MODBYTES_XXX],&(x->a));
}

void YYY::FP8_fromBytes(FP8 *x,char *b)
{
    FP4_fromBytes(&(x->b),b);
    FP4_fromBytes(&(x->a),&b[4*MODBYTES_XXX]);
}

/* return 1 if x==y, else 0 */
int YYY::FP8_equals(FP8 *x, FP8 *y)
{
    return (FP4_equals(&(x->a), &(y->a)) & FP4_equals(&(x->b), &(y->b)));
}

/* set FP8 from two FP4s */
void YYY::FP8_from_FP4s(FP8 *w, FP4 * x, FP4* y)
{
    FP4_copy(&(w->a), x);
    FP4_copy(&(w->b), y);
}

/* set FP8 from FP4 */
void YYY::FP8_from_FP4(FP8 *w, FP4 *x)
{
    FP4_copy(&(w->a), x);
    FP4_zero(&(w->b));
}

/* set high part of FP8 from FP4 */
void YYY::FP8_from_FP4H(FP8 *w, FP4 *x)
{
    FP4_copy(&(w->b), x);
    FP4_zero(&(w->a));
}

/* set FP8 from FP */
void YYY::FP8_from_FP(FP8 *w, FP *x)
{
    FP2 t;
    FP4 t4;
    FP2_from_FP(&t, x);
    FP4_from_FP2(&t4, &t);
    FP8_from_FP4(w, &t4);
}

/* FP8 copy w=x */
void YYY::FP8_copy(FP8 *w, FP8 *x)
{
    if (w == x) return;
    FP4_copy(&(w->a), &(x->a));
    FP4_copy(&(w->b), &(x->b));
}

/* FP8 w=0 */
void YYY::FP8_zero(FP8 *w)
{
    FP4_zero(&(w->a));
    FP4_zero(&(w->b));
}

/* FP8 w=1 */
void YYY::FP8_one(FP8 *w)
{
    FP4_one(&(w->a));
    FP4_zero(&(w->b));
}

int YYY::FP8_sign(FP8 *w)
{
    int p1,p2;
    p1=FP4_sign(&(w->a));
    p2=FP4_sign(&(w->b));
#ifdef BIG_ENDIAN_SIGN_YYY
    p2 ^= (p1 ^ p2)&FP4_iszilch(&(w->b));
    return p2;
#else
    p1 ^= (p1 ^ p2)&FP4_iszilch(&(w->a));
    return p1;
#endif

}

/* Set w=-x */
void YYY::FP8_neg(FP8 *w, FP8 *x)
{
    /* Just one field neg */
    FP4 m, t;
    FP8_norm(x);
    FP4_add(&m, &(x->a), &(x->b));
    FP4_norm(&m);
    FP4_neg(&m, &m);
    FP4_add(&t, &m, &(x->b));
    FP4_add(&(w->b), &m, &(x->a));
    FP4_copy(&(w->a), &t);
    FP8_norm(w);
}

/* Set w=conj(x) */
void YYY::FP8_conj(FP8 *w, FP8 *x)
{
    FP4_copy(&(w->a), &(x->a));
    FP4_neg(&(w->b), &(x->b));
    FP8_norm(w);
}

/* Set w=-conj(x) */
void YYY::FP8_nconj(FP8 *w, FP8 *x)
{
    FP4_copy(&(w->b), &(x->b));
    FP4_neg(&(w->a), &(x->a));
    FP8_norm(w);
}

/* Set w=x+y */
void YYY::FP8_add(FP8 *w, FP8 *x, FP8 *y)
{
    FP4_add(&(w->a), &(x->a), &(y->a));
    FP4_add(&(w->b), &(x->b), &(y->b));
}

/* Set w=x-y */
/* Input y MUST be normed */
void YYY::FP8_sub(FP8 *w, FP8 *x, FP8 *y)
{
    FP8 my;

    FP8_neg(&my, y);
    FP8_add(w, x, &my);

}

/* reduce all components of w mod Modulus */
void YYY::FP8_reduce(FP8 *w)
{
    FP4_reduce(&(w->a));
    FP4_reduce(&(w->b));
}

/* normalise all elements of w */
void YYY::FP8_norm(FP8 *w)
{
    FP4_norm(&(w->a));
    FP4_norm(&(w->b));
}

/* Set w=s*x, where s is FP4 */
void YYY::FP8_pmul(FP8 *w, FP8 *x, FP4 *s)
{
    FP4_mul(&(w->a), &(x->a), s);
    FP4_mul(&(w->b), &(x->b), s);
}

/* Set w=s*x, where s is FP2 */
void YYY::FP8_qmul(FP8 *w, FP8 *x, FP2 *s)
{
    FP4_pmul(&(w->a), &(x->a), s);
    FP4_pmul(&(w->b), &(x->b), s);
}

/* Set w=s*x, where s is FP */
void YYY::FP8_tmul(FP8 *w, FP8 *x, FP *s)
{
    FP4_qmul(&(w->a), &(x->a), s);
    FP4_qmul(&(w->b), &(x->b), s);
}

/* Set w=s*x, where s is int */
void YYY::FP8_imul(FP8 *w, FP8 *x, int s)
{
    FP4_imul(&(w->a), &(x->a), s);
    FP4_imul(&(w->b), &(x->b), s);
}

/* Set w=x^2 */
/* Input MUST be normed  */
void YYY::FP8_sqr(FP8 *w, FP8 *x)
{
    FP4 t1, t2, t3;

    FP4_mul(&t3, &(x->a), &(x->b)); /* norms x */
    FP4_copy(&t2, &(x->b));
    FP4_add(&t1, &(x->a), &(x->b));
    FP4_times_i(&t2);

    FP4_add(&t2, &(x->a), &t2);

    FP4_norm(&t1);  // 2
    FP4_norm(&t2);  // 2

    FP4_mul(&(w->a), &t1, &t2);

    FP4_copy(&t2, &t3);
    FP4_times_i(&t2);

    FP4_add(&t2, &t2, &t3);

    FP4_norm(&t2);  // 2
    FP4_neg(&t2, &t2);
    FP4_add(&(w->a), &(w->a), &t2); /* a=(a+b)(a+i^2.b)-i^2.ab-ab = a*a+ib*ib */
    FP4_add(&(w->b), &t3, &t3); /* b=2ab */

    FP8_norm(w);
}

/* Set w=x*y */
/* Inputs MUST be normed  */
void YYY::FP8_mul(FP8 *w, FP8 *x, FP8 *y)
{

    FP4 t1, t2, t3, t4;

    FP4_mul(&t1, &(x->a), &(y->a));
    FP4_mul(&t2, &(x->b), &(y->b));
    FP4_add(&t3, &(y->b), &(y->a));
    FP4_add(&t4, &(x->b), &(x->a));

    FP4_norm(&t4); // 2
    FP4_norm(&t3); // 2

    FP4_mul(&t4, &t4, &t3); /* (xa+xb)(ya+yb) */
    FP4_neg(&t3, &t1); // 1
    FP4_add(&t4, &t4, &t3); //t4E=3
    FP4_norm(&t4);

    FP4_neg(&t3, &t2); // 1
    FP4_add(&(w->b), &t4, &t3); //wbE=3

    FP4_times_i(&t2);
    FP4_add(&(w->a), &t2, &t1);

    FP8_norm(w);
}

/* output FP8 in format [a,b] */
void YYY::FP8_output(FP8 *w)
{
    printf("[");
    FP4_output(&(w->a));
    printf(",");
    FP4_output(&(w->b));
    printf("]");
}

void YYY::FP8_rawoutput(FP8 *w)
{
    printf("[");
    FP4_rawoutput(&(w->a));
    printf(",");
    FP4_rawoutput(&(w->b));
    printf("]");
}

/* Set w=1/x */
void YYY::FP8_inv(FP8 *w, FP8 *x, FP *h)
{
    FP4 t1, t2;
    FP4_sqr(&t1, &(x->a));
    FP4_sqr(&t2, &(x->b));
    FP4_times_i(&t2);
    FP4_norm(&t2);

    FP4_sub(&t1, &t1, &t2);
    FP4_norm(&t1);
    FP4_inv(&t1, &t1, h);

    FP4_mul(&(w->a), &t1, &(x->a));
    FP4_neg(&t1, &t1);
    FP4_norm(&t1);
    FP4_mul(&(w->b), &t1, &(x->b));
}

/* w*=i where i = sqrt(sqrt(-1+sqrt(-1))) */
void YYY::FP8_times_i(FP8 *w)
{
    FP4 s, t;
    FP4_copy(&s, &(w->b));
    FP4_copy(&t, &(w->a));
    FP4_times_i(&s);
    FP4_copy(&(w->a), &s);
    FP4_copy(&(w->b), &t);
    FP8_norm(w);
#if TOWER_YYY == POSITOWER
    FP8_neg(w, w);  // ***
    FP8_norm(w);
#endif
}

void YYY::FP8_times_i2(FP8 *w)
{
    FP4_times_i(&(w->a));
    FP4_times_i(&(w->b));
}

/* Set w=w^p using Frobenius */
void YYY::FP8_frob(FP8 *w, FP2 *f)
{   // f=(i+1)^(p-3)/4
    FP2 ff;
    FP2_sqr(&ff, f); // (i+1)^(p-3)/2
    FP2_mul_ip(&ff); // (i+1)^(p-1)/2
    FP2_norm(&ff);
    FP4_frob(&(w->a), &ff);
    FP4_frob(&(w->b), &ff);
    FP4_pmul(&(w->b), &(w->b), f); // times (1+i)^(p-3)/4
    FP4_times_i(&(w->b));       // (i+1)^(p-1)/4
}

/* Set r=a^b mod m */
/*
void YYY::FP8_pow(FP8 *r, FP8* a, BIG b)
{
    FP8 w;
    BIG z, zilch;
    int bt;

    BIG_zero(zilch);

    BIG_copy(z, b);
    FP8_copy(&w, a);
    FP8_one(r);
    BIG_norm(z);

    while (1)
    {
        bt = BIG_parity(z);
        BIG_shr(z, 1);
        if (bt) FP8_mul(r, r, &w);
        if (BIG_comp(z, zilch) == 0) break;
        FP8_sqr(&w, &w);
    }
    FP8_reduce(r);
}*/
/*
#if CURVE_SECURITY_ZZZ == 192
*/
/* XTR xtr_a function */
/*
void YYY::FP8_xtr_A(FP8 *r, FP8 *w, FP8 *x, FP8 *y, FP8 *z)
{
    FP8 t1, t2;

    FP8_copy(r, x);
    FP8_sub(&t1, w, y);
    FP8_norm(&t1);
    FP8_pmul(&t1, &t1, &(r->a));
    FP8_add(&t2, w, y);
    FP8_norm(&t2);
    FP8_pmul(&t2, &t2, &(r->b));
    FP8_times_i(&t2);

    FP8_add(r, &t1, &t2);
    FP8_add(r, r, z);

    FP8_reduce(r);
}
*/
/* XTR xtr_d function */
/*
void YYY::FP8_xtr_D(FP8 *r, FP8 *x)
{
    FP8 w;
    FP8_copy(r, x);
    FP8_conj(&w, r);
    FP8_add(&w, &w, &w);
    FP8_sqr(r, r);
    FP8_norm(&w);
    FP8_sub(r, r, &w);
    FP8_reduce(r);    // reduce here as multiple calls trigger automatic reductions
}
*/
/* r=x^n using XTR method on traces of FP24s */
/*
void YYY::FP8_xtr_pow(FP8 *r, FP8 *x, BIG n)
{
    int i, par, nb;
    BIG v;
    FP2 w2;
    FP4 w4;
    FP8 t, a, b, c, sf;

    BIG_zero(v);
    BIG_inc(v, 3);
    BIG_norm(v);
    FP2_from_BIG(&w2, v);
    FP4_from_FP2(&w4, &w2);
    FP8_from_FP4(&a, &w4);

    FP8_copy(&sf, x);
    FP8_norm(&sf);
    FP8_copy(&b, &sf);
    FP8_xtr_D(&c, &sf);

    par = BIG_parity(n);
    BIG_copy(v, n);
    BIG_norm(v);
    BIG_shr(v, 1);
    if (par == 0)
    {
        BIG_dec(v, 1);
        BIG_norm(v);
    }

    nb = BIG_nbits(v);
    for (i = nb - 1; i >= 0; i--)
    {
        if (!BIG_bit(v, i))
        {
            FP8_copy(&t, &b);
            FP8_conj(&sf, &sf);
            FP8_conj(&c, &c);
            FP8_xtr_A(&b, &a, &b, &sf, &c);
            FP8_conj(&sf, &sf);
            FP8_xtr_D(&c, &t);
            FP8_xtr_D(&a, &a);
        }
        else
        {
            FP8_conj(&t, &a);
            FP8_xtr_D(&a, &b);
            FP8_xtr_A(&b, &c, &b, &sf, &t);
            FP8_xtr_D(&c, &c);
        }
    }

    if (par == 0) FP8_copy(r, &c);
    else FP8_copy(r, &b);
    FP8_reduce(r);
}
*/
/* r=ck^a.cl^n using XTR double exponentiation method on traces of FP24s. See Stam thesis. */
/*
void YYY::FP8_xtr_pow2(FP8 *r, FP8 *ck, FP8 *cl, FP8 *ckml, FP8 *ckm2l, BIG a, BIG b)
{
    int i, f2;
    BIG d, e, w;
    FP8 t, cu, cv, cumv, cum2v;


    BIG_copy(e, a);
    BIG_copy(d, b);
    BIG_norm(e);
    BIG_norm(d);

    FP8_copy(&cu, ck);
    FP8_copy(&cv, cl);
    FP8_copy(&cumv, ckml);
    FP8_copy(&cum2v, ckm2l);

    f2 = 0;
    while (BIG_parity(d) == 0 && BIG_parity(e) == 0)
    {
        BIG_shr(d, 1);
        BIG_shr(e, 1);
        f2++;
    }
    while (BIG_comp(d, e) != 0)
    {
        if (BIG_comp(d, e) > 0)
        {
            BIG_imul(w, e, 4);
            BIG_norm(w);
            if (BIG_comp(d, w) <= 0)
            {
                BIG_copy(w, d);
                BIG_copy(d, e);
                BIG_sub(e, w, e);
                BIG_norm(e);
                FP8_xtr_A(&t, &cu, &cv, &cumv, &cum2v);
                FP8_conj(&cum2v, &cumv);
                FP8_copy(&cumv, &cv);
                FP8_copy(&cv, &cu);
                FP8_copy(&cu, &t);
            }
            else if (BIG_parity(d) == 0)
            {
                BIG_shr(d, 1);
                FP8_conj(r, &cum2v);
                FP8_xtr_A(&t, &cu, &cumv, &cv, r);
                FP8_xtr_D(&cum2v, &cumv);
                FP8_copy(&cumv, &t);
                FP8_xtr_D(&cu, &cu);
            }
            else if (BIG_parity(e) == 1)
            {
                BIG_sub(d, d, e);
                BIG_norm(d);
                BIG_shr(d, 1);
                FP8_xtr_A(&t, &cu, &cv, &cumv, &cum2v);
                FP8_xtr_D(&cu, &cu);
                FP8_xtr_D(&cum2v, &cv);
                FP8_conj(&cum2v, &cum2v);
                FP8_copy(&cv, &t);
            }
            else
            {
                BIG_copy(w, d);
                BIG_copy(d, e);
                BIG_shr(d, 1);
                BIG_copy(e, w);
                FP8_xtr_D(&t, &cumv);
                FP8_conj(&cumv, &cum2v);
                FP8_conj(&cum2v, &t);
                FP8_xtr_D(&t, &cv);
                FP8_copy(&cv, &cu);
                FP8_copy(&cu, &t);
            }
        }
        if (BIG_comp(d, e) < 0)
        {
            BIG_imul(w, d, 4);
            BIG_norm(w);
            if (BIG_comp(e, w) <= 0)
            {
                BIG_sub(e, e, d);
                BIG_norm(e);
                FP8_xtr_A(&t, &cu, &cv, &cumv, &cum2v);
                FP8_copy(&cum2v, &cumv);
                FP8_copy(&cumv, &cu);
                FP8_copy(&cu, &t);
            }
            else if (BIG_parity(e) == 0)
            {
                BIG_copy(w, d);
                BIG_copy(d, e);
                BIG_shr(d, 1);
                BIG_copy(e, w);
                FP8_xtr_D(&t, &cumv);
                FP8_conj(&cumv, &cum2v);
                FP8_conj(&cum2v, &t);
                FP8_xtr_D(&t, &cv);
                FP8_copy(&cv, &cu);
                FP8_copy(&cu, &t);
            }
            else if (BIG_parity(d) == 1)
            {
                BIG_copy(w, e);
                BIG_copy(e, d);
                BIG_sub(w, w, d);
                BIG_norm(w);
                BIG_copy(d, w);
                BIG_shr(d, 1);
                FP8_xtr_A(&t, &cu, &cv, &cumv, &cum2v);
                FP8_conj(&cumv, &cumv);
                FP8_xtr_D(&cum2v, &cu);
                FP8_conj(&cum2v, &cum2v);
                FP8_xtr_D(&cu, &cv);
                FP8_copy(&cv, &t);
            }
            else
            {
                BIG_shr(d, 1);
                FP8_conj(r, &cum2v);
                FP8_xtr_A(&t, &cu, &cumv, &cv, r);
                FP8_xtr_D(&cum2v, &cumv);
                FP8_copy(&cumv, &t);
                FP8_xtr_D(&cu, &cu);
            }
        }
    }
    FP8_xtr_A(r, &cu, &cv, &cumv, &cum2v);
    for (i = 0; i < f2; i++)    FP8_xtr_D(r, r);
    FP8_xtr_pow(r, r, d);
}

#endif
*/

/* New stuff for ECp8 support */

/* Set w=x/2 */
void YYY::FP8_div2(FP8 *w, FP8 *x)
{
    FP4_div2(&(w->a), &(x->a));
    FP4_div2(&(w->b), &(x->b));
}

/* Move b to a if d=1 */
void YYY::FP8_cmove(FP8 *f, FP8 *g, int d)
{
    FP4_cmove(&(f->a), &(g->a), d);
    FP4_cmove(&(f->b), &(g->b), d);
}

void YYY::FP8_rand(FP8 *x,csprng *rng)
{
    FP4_rand(&(x->a),rng);
    FP4_rand(&(x->b),rng);
}

#if PAIRING_FRIENDLY_ZZZ == BLS48_CURVE

/* test for x a QR */
int YYY::FP8_qr(FP8 *x,FP *h)
{ /* test x^(p^4-1)/2 = 1 */
    FP8 c;
    FP8_conj(&c,x);
    FP8_mul(&c,&c,x);

    return FP4_qr(&(c.a),h);
}

/* sqrt(a+xb) = sqrt((a+sqrt(a*a-n*b*b))/2)+x.b/(2*sqrt((a+sqrt(a*a-n*b*b))/2)) */

void YYY::FP8_sqrt(FP8 *r, FP8* x, FP *h)
{
    FP4 a, b, s, t;
    FP8 nr;
    FP hint,twk;
    int sgn,qr;
    FP8_copy(r, x);
    if (FP8_iszilch(x)) return;

    FP4_copy(&a, &(x->a));
    FP4_copy(&s, &(x->b));

    FP4_sqr(&s, &s); // s*=s
    FP4_sqr(&a, &a); // a*=a
    FP4_times_i(&s);
    FP4_norm(&s);
    FP4_sub(&a, &a, &s); // a-=txx(s)
    FP4_norm(&a);  // **
    FP4_sqrt(&s, &a, h);

    FP4_copy(&t, &(x->a));
    FP4_add(&a, &t, &s);
    FP4_norm(&a);
    FP4_div2(&a, &a);

    FP4_div2(&b,&(r->b));                   // w1=b/2
    qr=FP4_qr(&a,&hint);                    // only exp! Cost=+1

// tweak hint - multiply old hint by Norm(1/Beta)^e where Beta is irreducible polynomial
    FP4_copy(&s,&a);
    FP_rcopy(&twk,TWK);
    FP_mul(&twk,&twk,&hint);
    FP4_div_i(&s); FP4_norm(&s); // switch to other candidate

    FP4_cmove(&a,&s,1-qr);
    FP_cmove(&hint,&twk,1-qr);

    FP4_sqrt(&(r->a),&a,&hint);             // a=sqrt(w2)  Cost=+1
    FP4_inv(&s,&a,&hint);                  // w3=1/w2
    FP4_mul(&s,&s,&(r->a));                // w3=1/sqrt(w2)
    FP4_mul(&(r->b),&s,&b);                // b=(b/2)*1/sqrt(w2)
    FP4_copy(&t,&(r->a));

    FP4_cmove(&(r->a),&(r->b),1-qr);
    FP4_cmove(&(r->b),&t,1-qr);

    sgn=FP8_sign(r);
    FP8_neg(&nr,r); FP8_norm(&nr);
    FP8_cmove(r,&nr,sgn);
}

void YYY::FP8_div_i(FP8 *f)
{
    FP4 u, v;
    FP4_copy(&u, &(f->a));
    FP4_copy(&v, &(f->b));

    FP4_div_i(&u);

    FP4_copy(&(f->a), &v);
    FP4_copy(&(f->b), &u);
#if TOWER_YYY == POSITOWER
    FP8_neg(f, f);  // ***
    FP8_norm(f);
#endif
}



#endif
