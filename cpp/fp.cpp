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

/* CORE mod p functions */
/* Small Finite Field arithmetic */
/* SU=m, SU is Stack Usage (NOT_SPECIAL Modulus) */

#include "fp_YYY.h"

using namespace XXX;

/* Fast Modular Reduction Methods */

/* r=d mod m */
/* d MUST be normalised */
/* Products must be less than pR in all cases !!! */
/* So when multiplying two numbers, their product *must* be less than MODBITS_YYY+BASEBITS_XXX*NLEN_XXX */
/* Results *may* be one bit bigger than MODBITS_YYY */

#if MODTYPE_YYY == PSEUDO_MERSENNE
/* r=d mod m */

/* Converts from BIG integer to residue form mod Modulus */
void YYY::FP_nres(FP *y, BIG x)
{
    BIG mdls;
    BIG_rcopy(mdls, Modulus);
    BIG_copy(y->g, x);
    BIG_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void YYY::FP_redc(BIG x, FP *y)
{
    BIG_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void YYY::FP_mod(BIG r, DBIG d)
{
    BIG t, b;
    chunk v, tw;
    BIG_split(t, b, d, MODBITS_YYY);

    /* Note that all of the excess gets pushed into t. So if squaring a value with a 4-bit excess, this results in
       t getting all 8 bits of the excess product! So products must be less than pR which is Montgomery compatible */

    if (MConst < NEXCESS_XXX)
    {
        BIG_imul(t, t, MConst);
        BIG_norm(t);
        BIG_add(r, t, b);
        BIG_norm(r);
        tw = r[NLEN_XXX - 1];
        r[NLEN_XXX - 1] &= TMASK_YYY;
        r[0] += MConst * ((tw >> TBITS_YYY));
    }
    else
    {
        v = BIG_pmul(t, t, MConst);
        BIG_add(r, t, b);
        BIG_norm(r);
        tw = r[NLEN_XXX - 1];
        r[NLEN_XXX - 1] &= TMASK_YYY;
#if CHUNK == 16
        r[1] += muladd(MConst, ((tw >> TBITS_YYY) + (v << (BASEBITS_XXX - TBITS_YYY))), 0, &r[0]);
#else
        r[0] += MConst * ((tw >> TBITS_YYY) + (v << (BASEBITS_XXX - TBITS_YYY)));
#endif
    }
    BIG_norm(r);
}
#endif

/* This only applies to Curve C448, so specialised (for now) */
#if MODTYPE_YYY == GENERALISED_MERSENNE

void YYY::FP_nres(FP *y, BIG x)
{
    BIG mdls;
    BIG_rcopy(mdls, Modulus);
    BIG_copy(y->g, x);
    BIG_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void YYY::FP_redc(BIG x, FP *y)
{
    BIG_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of a modulus 2^m - 2^n -c */
void YYY::FP_mod(BIG r, DBIG d)
{

    BIG t, b;
    chunk carry;
    BIG_split(t, b, d, MBITS_YYY);

    BIG_add(r, t, b);

    BIG_dscopy(d, t);
    BIG_dshl(d, MBITS_YYY / 2);

    BIG_split(t, b, d, MBITS_YYY);

    BIG_add(r, r, t);
    BIG_add(r, r, b);
    BIG_norm(r);
    BIG_shl(t, MBITS_YYY / 2);

    BIG_add(r, r, t);

    carry = r[NLEN_XXX - 1] >> TBITS_YYY;

    r[NLEN_XXX - 1] &= TMASK_YYY;
    r[0] += carry;

    r[224 / BASEBITS_XXX] += carry << (224 % BASEBITS_XXX); /* need to check that this falls mid-word */
    BIG_norm(r);

/*

    BIG t, b, t2, b2;
    int BTset = MBITS_YYY / 2;
    chunk carry;
    BIG_split(t, b, d, MBITS_YYY);

    BIG_dscopy(d, t);
    BIG_dshl(d, BTset);

    BIG_split(t2, b2, d, MBITS_YYY);

    BIG_add(b, b, b2); // 2
    BIG_add(t, t, t2); // 2

    BIG_shl(t2, BTset);

    BIG_add(b, b, t2); // 3
    BIG_norm(t);

//  carry=0;
// Now multiply t by MConst..(?) and extract carry
//  if (MConst!=1)
//      carry=BIG_pmul(t,t,MConst);

    BIG_add(r, t, b);
    BIG_norm(r);

    carry = r[NLEN_XXX - 1] >> TBITS_YYY; // + (carry<<(BASEBITS_XXX-TBITS_YYY));
    r[NLEN_XXX - 1] &= TMASK_YYY;

    r[BTset / BASEBITS_XXX] += carry << (BTset % BASEBITS_XXX); // need to check that this falls mid-word 
//  if (MConst!=1) carry*=MConst;
    r[0] += carry;

    BIG_norm(r);
*/
}

#endif

#if MODTYPE_YYY == MONTGOMERY_FRIENDLY

/* convert to Montgomery n-residue form */
void YYY::FP_nres(FP *y, BIG x)
{
    DBIG d;
    BIG r;
    BIG_rcopy(r, R2modp);
    BIG_mul(d, x, r);
    FP_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void YYY::FP_redc(BIG x, FP *y)
{
    DBIG d;
    BIG_dzero(d);
    BIG_dscopy(d, y->g);
    FP_mod(x, d);
}

/* fast modular reduction from DBIG to BIG exploiting special form of the modulus */
void YYY::FP_mod(BIG a, DBIG d)
{
    int i;

    for (i = 0; i < NLEN_XXX; i++)
        d[NLEN_XXX + i] += muladd(d[i], MConst - 1, d[i], &d[NLEN_XXX + i - 1]);

    BIG_sducopy(a, d);
    BIG_norm(a);
}

#endif

#if MODTYPE_YYY == NOT_SPECIAL

/* convert to Montgomery n-residue form */
void YYY::FP_nres(FP *y, BIG x)
{
    DBIG d;
    BIG r;
    BIG_rcopy(r, R2modp);
    BIG_mul(d, x, r);
    FP_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void YYY::FP_redc(BIG x, FP *y)
{
    DBIG d;
    BIG_dzero(d);
    BIG_dscopy(d, y->g);
    FP_mod(x, d);
}


/* reduce a DBIG to a BIG using Montgomery's no trial division method */
/* d is expected to be dnormed before entry */
/* SU= 112 */
void YYY::FP_mod(BIG a, DBIG d)
{
    BIG mdls;
    BIG_rcopy(mdls, Modulus);
    BIG_monty(a, mdls, MConst, d);
}

#endif

void YYY::FP_from_int(FP *x,int a)
{
    BIG w;
    if (a<0) BIG_rcopy(w, Modulus);
    else BIG_zero(w); 
    BIG_inc(w,a); BIG_norm(w); 
    FP_nres(x,w);
}

/* test x==0 ? */
/* SU= 48 */
int YYY::FP_iszilch(FP *x)
{
    BIG m;
    FP y;
    FP_copy(&y,x);
    FP_reduce(&y);
    FP_redc(m,&y);
    return BIG_iszilch(m);
}

/* input must be reduced */
int YYY::FP_isunity(FP *x)
{
    BIG m;
    FP y;
    FP_copy(&y,x);
    FP_reduce(&y);
    FP_redc(m,&y);
    return BIG_isunity(m);
}

void YYY::FP_copy(FP *y, FP *x)
{
    BIG_copy(y->g, x->g);
    y->XES = x->XES;
}

void YYY::FP_rcopy(FP *y, const BIG c)
{
    BIG b;
    BIG_rcopy(b, c);
    FP_nres(y, b);
}

/* Swap a and b if d=1 */
void YYY::FP_cswap(FP *a, FP *b, int d)
{
    sign32 t, c = d;
    BIG_cswap(a->g, b->g, d);

    c = ~(c - 1);
    t = c & ((a->XES) ^ (b->XES));
    a->XES ^= t;
    b->XES ^= t;

}

/* Move b to a if d=1 */
void YYY::FP_cmove(FP *a, FP *b, int d)
{
    sign32 c = -d;

    BIG_cmove(a->g, b->g, d);
    a->XES ^= (a->XES ^ b->XES)&c;
}

void YYY::FP_zero(FP *x)
{
    BIG_zero(x->g);
    x->XES = 1;
}

int YYY::FP_equals(FP *x, FP *y)
{
    FP xg, yg;
    FP_copy(&xg, x);
    FP_copy(&yg, y);
    FP_reduce(&xg); FP_reduce(&yg);

    if (BIG_comp(xg.g, yg.g) == 0) return 1;
    return 0;
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int YYY::FP_islarger(FP *x)
{
    BIG p,fx,sx;
    if (FP_iszilch(x)) return 0;
    BIG_rcopy(p,Modulus);
    FP_redc(fx,x);
    BIG_sub(sx,p,fx);  BIG_norm(sx); 
    return BIG_comp(fx,sx);
}

void YYY::FP_toBytes(char *b,FP *x)
{
    BIG t;
    FP_redc(t, x);
    BIG_toBytes(b, t);
}

void YYY::FP_fromBytes(FP *x,char *b)
{
    BIG t;
    BIG_fromBytes(t, b);
    FP_nres(x, t);
}

/* output FP */
/* SU= 48 */
void YYY::FP_output(FP *r)
{
    BIG c;
    FP_reduce(r);
    FP_redc(c, r);
    BIG_output(c);
}

void YYY::FP_rawoutput(FP *r)
{
    BIG_rawoutput(r->g);
}

#ifdef GET_STATS
int tsqr = 0, rsqr = 0, tmul = 0, rmul = 0;
int tadd = 0, radd = 0, tneg = 0, rneg = 0;
int tdadd = 0, rdadd = 0, tdneg = 0, rdneg = 0;
#endif

#ifdef FUSED_MODMUL

/* Insert fastest code here */

#endif

/* r=a*b mod Modulus */
/* product must be less that p.R - and we need to know this in advance! */
/* SU= 88 */
void YYY::FP_mul(FP *r, FP *a, FP *b)
{
    DBIG d;

    if ((sign64)a->XES * b->XES > (sign64)FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_reduce(a);  /* it is sufficient to fully reduce just one of them < p */
    }

#ifdef FUSED_MODMUL
    FP_modmul(r->g, a->g, b->g);
#else
    BIG_mul(d, a->g, b->g);
    FP_mod(r->g, d);
#endif
    r->XES = 2;
}


/* multiplication by an integer, r=a*c */
/* SU= 136 */
void YYY::FP_imul(FP *r, FP *a, int c)
{
    DBIG d;
    BIG k;
    FP f;

    int s = 0;

    if (c < 0)
    {
        c = -c;
        s = 1;
    }

#if MODTYPE_YYY==PSEUDO_MERSENNE || MODTYPE_YYY==GENERALISED_MERSENNE

    BIG_pxmul(d, a->g, c);
    FP_mod(r->g, d);
    r->XES = 2;

#else
    //Montgomery
    if (a->XES * c <= FEXCESS_YYY)
    {
        BIG_pmul(r->g, a->g, c);
        r->XES = a->XES * c; // careful here - XES jumps!
    }
    else
    {   // don't want to do this - only a problem for Montgomery modulus and larger constants
        BIG_zero(k);
        BIG_inc(k, c);
        BIG_norm(k);
        FP_nres(&f, k);
        FP_mul(r, a, &f);
    }
#endif

    if (s)
    {
        FP_neg(r, r);
        FP_norm(r);
    }
}

/* Set r=a^2 mod m */
/* SU= 88 */
void YYY::FP_sqr(FP *r, FP *a)
{
    DBIG d;

    if ((sign64)a->XES * a->XES > (sign64)FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_reduce(a);
    }

    BIG_sqr(d, a->g);
    FP_mod(r->g, d);
    r->XES = 2;
}

/* SU= 16 */
/* Set r=a+b */
void YYY::FP_add(FP *r, FP *a, FP *b)
{
    BIG_add(r->g, a->g, b->g);
    r->XES = a->XES + b->XES;
    if (r->XES > FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("Sum too large - reducing it \n");
#endif
        FP_reduce(r);
    }
}

/* Set r=a-b mod m */
/* SU= 56 */
void YYY::FP_sub(FP *r, FP *a, FP *b)
{
    FP n;
    FP_neg(&n, b);
    FP_add(r, a, &n);
}

// https://graphics.stanford.edu/~seander/bithacks.html
// constant time log to base 2 (or number of bits in)

static int logb2(unsign32 v)
{
    int r;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    r = (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
    return r;
}

// find appoximation to quotient of a/m
// Out by at most 2.
// Note that MAXXES is bounded to be 2-bits less than half a word
static int quo(BIG n, BIG m)
{
    int sh;
    chunk num, den;
    int hb = CHUNK / 2;
    if (TBITS_YYY < hb)
    {
        sh = hb - TBITS_YYY;
        num = (n[NLEN_XXX - 1] << sh) | (n[NLEN_XXX - 2] >> (BASEBITS_XXX - sh));
        den = (m[NLEN_XXX - 1] << sh) | (m[NLEN_XXX - 2] >> (BASEBITS_XXX - sh));
    }
    else
    {
        num = n[NLEN_XXX - 1];
        den = m[NLEN_XXX - 1];
    }
    return (int)(num / (den + 1));
}

/* SU= 48 */
/* Fully reduce a mod Modulus */
void YYY::FP_reduce(FP *a)
{
    BIG m, r;
    int sr, sb, q;
    chunk carry;

    BIG_rcopy(m, Modulus);
    BIG_norm(a->g);

    if (a->XES > 16)
    {
        q = quo(a->g, m);
        carry = BIG_pmul(r, m, q);
        r[NLEN_XXX - 1] += (carry << BASEBITS_XXX); // correction - put any carry out back in again
        BIG_sub(a->g, a->g, r);
        BIG_norm(a->g);
        sb = 2;
    }
    else sb = logb2(a->XES - 1); // sb does not depend on the actual data

    BIG_fshl(m, sb);
    while (sb > 0)
    {
// constant time...
        sr = BIG_ssn(r, a->g, m); // optimized combined shift, subtract and norm
        BIG_cmove(a->g, r, 1 - sr);
        sb--;
    }

    a->XES = 1;
}

void YYY::FP_norm(FP *x)
{
    BIG_norm(x->g);
}

/* Set r=-a mod Modulus */
/* SU= 64 */
void YYY::FP_neg(FP *r, FP *a)
{
    int sb;
    BIG m;

    BIG_rcopy(m, Modulus);

    sb = logb2(a->XES - 1);
    BIG_fshl(m, sb);
    BIG_sub(r->g, m, a->g);
    r->XES = ((sign32)1 << sb) + 1; // +1 to cover case where a is zero ?

    if (r->XES > FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("Negation too large -  reducing it \n");
#endif
        FP_reduce(r);
    }

}

/* Set r=a/2. */
/* SU= 56 */
void YYY::FP_div2(FP *r, FP *a)
{
    BIG m;
    BIG w;
    BIG_rcopy(m, Modulus);
    int pr=BIG_parity(a->g);

    FP_copy(r, a);
    BIG_copy(w,r->g);
    BIG_fshr(r->g,1);
    BIG_add(w, w, m);
    BIG_norm(w);
    BIG_fshr(w, 1);   
    
    BIG_cmove(r->g,w,pr);
}

// Could leak size of b
// but not used here with secret exponent b
void YYY::FP_pow(FP *r, FP *a, BIG b)
{
    sign8 w[1 + (NLEN_XXX * BASEBITS_XXX + 3) / 4];
    FP tb[16];
    BIG t;
    int i, nb;

    FP_copy(r,a);
    FP_norm(r);
    BIG_copy(t, b);
    BIG_norm(t);
    nb = 1 + (BIG_nbits(t) + 3) / 4;
    // convert exponent to 4-bit window
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_lastbits(t, 4);
        BIG_dec(t, w[i]);
        BIG_norm(t);
        BIG_fshr(t, 4);
    }

    FP_one(&tb[0]);
    FP_copy(&tb[1], r);
    for (i = 2; i < 16; i++)
        FP_mul(&tb[i], &tb[i - 1], r);

    FP_copy(r, &tb[w[nb - 1]]);
    for (i = nb - 2; i >= 0; i--)
    {
        FP_sqr(r, r);
        FP_sqr(r, r);
        FP_sqr(r, r);
        FP_sqr(r, r);
        FP_mul(r, r, &tb[w[i]]);
    }
    FP_reduce(r);
}

#if MODTYPE_YYY==PSEUDO_MERSENNE  || MODTYPE_YYY==GENERALISED_MERSENNE

// See eprint paper https://eprint.iacr.org/2018/1038
// e.g. If p=3 mod 4 r= x^{(p-3)/4}, if p=5 mod 8 r=x^{(p-5)/8}

void YYY::FP_fpow(FP *r, FP *x)
{
    int i, j, k, bw, w, nw, lo, m, n, c, nd, e=PM1D2_YYY;
    FP xp[11], t, key;
    const int ac[] = {1, 2, 3, 6, 12, 15, 30, 60, 120, 240, 255};
// phase 1
    FP_copy(&xp[0], x); // 1
    FP_sqr(&xp[1], x); // 2
    FP_mul(&xp[2], &xp[1], x); //3
    FP_sqr(&xp[3], &xp[2]); // 6
    FP_sqr(&xp[4], &xp[3]); // 12
    FP_mul(&xp[5], &xp[4], &xp[2]); // 15
    FP_sqr(&xp[6], &xp[5]); // 30
    FP_sqr(&xp[7], &xp[6]); // 60
    FP_sqr(&xp[8], &xp[7]); // 120
    FP_sqr(&xp[9], &xp[8]); // 240
    FP_mul(&xp[10], &xp[9], &xp[5]); // 255

#if MODTYPE_YYY==PSEUDO_MERSENNE
    n = MODBITS_YYY;
#endif
#if MODTYPE_YYY==GENERALISED_MERSENNE  // Ed448 ONLY
    n = MODBITS_YYY / 2;
#endif

    n-=(e+1);
    c=(MConst+(1<<e)+1)/(1<<(e+1));

// need c to be odd
    nd=0;
    while (c%2==0)
    {
        c/=2;
        n-=1;
        nd++;
    }

    bw = 0; w = 1; while (w < c) {w *= 2; bw += 1;}
    k = w - c;

    if (k != 0)
    {
        i = 10; while (ac[i] > k) i--;
        FP_copy(&key, &xp[i]);
        k -= ac[i];
    }
    while (k != 0)
    {
        i--;
        if (ac[i] > k) continue;
        FP_mul(&key, &key, &xp[i]);
        k -= ac[i];
    }

// phase 2
    FP_copy(&xp[1], &xp[2]);
    FP_copy(&xp[2], &xp[5]);
    FP_copy(&xp[3], &xp[10]);

    j = 3; m = 8;
    nw = n - bw;
    while (2 * m < nw)
    {
        FP_copy(&t, &xp[j++]);
        for (i = 0; i < m; i++)
            FP_sqr(&t, &t);
        FP_mul(&xp[j], &xp[j - 1], &t);
        m *= 2;
    }

    lo = nw - m;
    FP_copy(r, &xp[j]);

    while (lo != 0)
    {
        m /= 2; j--;
        if (lo < m) continue;
        lo -= m;
        FP_copy(&t, r);
        for (i = 0; i < m; i++)
            FP_sqr(&t, &t);
        FP_mul(r, &t, &xp[j]);
    }
// phase 3

    if (bw != 0)
    {
        for (i = 0; i < bw; i++ )
            FP_sqr(r, r);
        FP_mul(r, r, &key);
    }

#if MODTYPE_YYY==GENERALISED_MERSENNE  // Ed448 ONLY
    FP_copy(&key, r);
    FP_sqr(&t, &key);
    FP_mul(r, &t, &xp[0]);
    for (i = 0; i < n + 1; i++)
        FP_sqr(r, r);
    FP_mul(r, r, &key);
#endif

    for (i=0;i<nd;i++)
        FP_sqr(r,r);

}

#endif

// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
void YYY::FP_progen(FP *r,FP *x)
{
#if MODTYPE_YYY==PSEUDO_MERSENNE  || MODTYPE_YYY==GENERALISED_MERSENNE
    FP_fpow(r, x);  
#else
    int e=PM1D2_YYY;
    BIG m;
    BIG_rcopy(m, Modulus);
    BIG_dec(m,1);
    BIG_shr(m,e);
    BIG_dec(m,1);
    BIG_fshr(m,1);
    FP_pow(r,x,m);
#endif
}

/* Is x a QR? return optional hint for fast follow-up square root */
int YYY::FP_qr(FP *x,FP *h)
{
    FP r;
    int i,e=PM1D2_YYY;
    FP_progen(&r,x);
    if (h!=NULL)
        FP_copy(h,&r);

    FP_sqr(&r,&r);
    FP_mul(&r,x,&r);
    for (i=0;i<e-1;i++ )
        FP_sqr(&r,&r);

    return FP_isunity(&r);
}

/* Modular inversion */
void YYY::FP_inv(FP *r,FP *x,FP *h)
{
    int i,e=PM1D2_YYY;
    FP s,t;
    FP_norm(x);
    FP_copy(&s,x);

    if (h==NULL)
        FP_progen(&t,x);
    else
        FP_copy(&t,h);

    for (i=0;i<e-1;i++)
    {  
        FP_sqr(&s,&s);
        FP_mul(&s,&s,x);
    }
    for (i=0;i<=e;i++)
        FP_sqr(&t,&t);

    FP_mul(r,&t,&s);
    FP_reduce(r);
}

// Tonelli-Shanks constant time
void YYY::FP_sqrt(FP *r, FP *a, FP* h)
{
    int i,j,k,u,e=PM1D2_YYY;
    FP v,g,t,b;
    BIG m;

    if (h==NULL)
        FP_progen(&g,a);
    else
        FP_copy(&g,h);

    BIG_rcopy(m,ROI);
    FP_nres(&v,m);

    FP_sqr(&t,&g);
    FP_mul(&t,&t,a);
   
    FP_mul(r,&g,a);
    FP_copy(&b,&t);
    for (k=e;k>1;k--)
    {
        for (j=1;j<k-1;j++)
            FP_sqr(&b,&b);
        u=1-FP_isunity(&b);
        FP_mul(&g,r,&v);
        FP_cmove(r,&g,u);
        FP_sqr(&v,&v);
        FP_mul(&g,&t,&v);
        FP_cmove(&t,&g,u);
        FP_copy(&b,&t);
    }
// always return +ve square root
    k=FP_sign(r);
    FP_neg(&v,r); FP_norm(&v);
    FP_cmove(r,&v,k);
}

// Calculate both inverse and square root of x, return QR
int YYY::FP_invsqrt(FP *i, FP *s, FP *x)
{
    FP h;
    int qr=FP_qr(x,&h);
    FP_sqrt(s,x,&h);
    FP_inv(i,x,&h);
    return qr;
}

// Two for Price of One - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
int YYY::FP_tpo(FP* i, FP* s)
{
    int qr;
    FP w,t;
    FP_mul(&w,s,i);
    FP_mul(&t,&w,i);
    qr=FP_invsqrt(i,s,&t);
    FP_mul(i,i,&w);
    FP_mul(s,s,i);
    return qr;
}

/* SU=8 */
/* set n=1 */
void YYY::FP_one(FP *n)
{
    BIG b;
    BIG_one(b);
    FP_nres(n, b);
}

int YYY::FP_sign(FP *x)
{
#ifdef BIG_ENDIAN_SIGN_YYY
    int cp;
    BIG m,pm1d2;
    FP y;
    BIG_rcopy(pm1d2, Modulus);
    BIG_dec(pm1d2,1);
    BIG_fshr(pm1d2,1); //(p-1)/2
     
    FP_copy(&y,x);
    FP_reduce(&y);
    FP_redc(m,&y);
    cp=BIG_comp(m,pm1d2);
    return ((cp+1)&2)>>1;

#else
    BIG m;
    FP y;
    FP_copy(&y,x);
    FP_reduce(&y);
    FP_redc(m,&y);
    return BIG_parity(m);
#endif
}

void YYY::FP_rand(FP *x,csprng *rng)
{
    BIG w,m;
    BIG_rcopy(m,Modulus);
    BIG_randomnum(w,m,rng);
    FP_nres(x,w);
}
