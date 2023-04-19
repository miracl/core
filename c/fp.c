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

/* Fast Modular Reduction Methods */

/* r=d mod m */
/* d MUST be normalised */
/* Products must be less than pR in all cases !!! */
/* So when multiplying two numbers, their product *must* be less than MODBITS+BASEBITS*NLEN */
/* Results *may* be one bit bigger than MODBITS */

#if MODTYPE_YYY == PSEUDO_MERSENNE
/* r=d mod m */

/* Converts from BIG integer to residue form mod Modulus */
void FP_YYY_nres(FP_YYY *y, BIG_XXX x)
{
    BIG_XXX mdls;
    BIG_XXX_rcopy(mdls, Modulus_YYY);
    BIG_XXX_copy(y->g, x);
    BIG_XXX_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_YYY_redc(BIG_XXX x, FP_YYY *y)
{
    BIG_XXX_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_YYY_mod(BIG_XXX r, DBIG_XXX d)
{
    BIG_XXX t, b;
    chunk v, tw;
    BIG_XXX_split(t, b, d, MODBITS_YYY);

    /* Note that all of the excess gets pushed into t. So if squaring a value with a 4-bit excess, this results in
       t getting all 8 bits of the excess product! So products must be less than pR which is Montgomery compatible */

    if (MConst_YYY < NEXCESS_XXX)
    {
        BIG_XXX_imul(t, t, MConst_YYY);
        BIG_XXX_norm(t);
        BIG_XXX_add(r, t, b);
        BIG_XXX_norm(r);
        tw = r[NLEN_XXX - 1];
        r[NLEN_XXX - 1] &= TMASK_YYY;
        r[0] += MConst_YYY * ((tw >> TBITS_YYY));
    }
    else
    {
        v = BIG_XXX_pmul(t, t, MConst_YYY);
        BIG_XXX_add(r, t, b);
        BIG_XXX_norm(r);
        tw = r[NLEN_XXX - 1];
        r[NLEN_XXX - 1] &= TMASK_YYY;
#if CHUNK == 16
        r[1] += muladd_XXX(MConst_YYY, ((tw >> TBITS_YYY) + (v << (BASEBITS_XXX - TBITS_YYY))), 0, &r[0]);
#else
        r[0] += MConst_YYY * ((tw >> TBITS_YYY) + (v << (BASEBITS_XXX - TBITS_YYY)));
#endif
    }
    BIG_XXX_norm(r);
}
#endif

/* This only applies to Curve C448, so specialised (for now) */
#if MODTYPE_YYY == GENERALISED_MERSENNE

void FP_YYY_nres(FP_YYY *y, BIG_XXX x)
{
    BIG_XXX mdls;
    BIG_XXX_rcopy(mdls, Modulus_YYY);
    BIG_XXX_copy(y->g, x);
    BIG_XXX_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_YYY_redc(BIG_XXX x, FP_YYY *y)
{
    BIG_XXX_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_YYY_mod(BIG_XXX r, DBIG_XXX d)
{
    BIG_XXX t, b;
    chunk carry;
    BIG_XXX_split(t, b, d, MBITS_YYY);

    BIG_XXX_add(r, t, b);

    BIG_XXX_dscopy(d, t);
    BIG_XXX_dshl(d, MBITS_YYY / 2);

    BIG_XXX_split(t, b, d, MBITS_YYY);

    BIG_XXX_add(r, r, t);
    BIG_XXX_add(r, r, b);
    BIG_XXX_norm(r);
    BIG_XXX_shl(t, MBITS_YYY / 2);

    BIG_XXX_add(r, r, t);

    carry = r[NLEN_XXX - 1] >> TBITS_YYY;

    r[NLEN_XXX - 1] &= TMASK_YYY;
    r[0] += carry;

    r[224 / BASEBITS_XXX] += carry << (224 % BASEBITS_XXX); /* need to check that this falls mid-word */
    BIG_XXX_norm(r);
}

#endif

#if MODTYPE_YYY == MONTGOMERY_FRIENDLY

/* convert to Montgomery n-residue form */
void FP_YYY_nres(FP_YYY *y, BIG_XXX x)
{
    DBIG_XXX d;
    BIG_XXX r;
    BIG_XXX_rcopy(r, R2modp_YYY);
    BIG_XXX_mul(d, x, r);
    FP_YYY_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_YYY_redc(BIG_XXX x, FP_YYY *y)
{
    DBIG_XXX d;
    BIG_XXX_dzero(d);
    BIG_XXX_dscopy(d, y->g);
    FP_YYY_mod(x, d);
}

/* fast modular reduction from DBIG to BIG exploiting special form of the modulus */
void FP_YYY_mod(BIG_XXX a, DBIG_XXX d)
{
    int i;

    for (i = 0; i < NLEN_XXX; i++)
        d[NLEN_XXX + i] += muladd_XXX(d[i], MConst_YYY - 1, d[i], &d[NLEN_XXX + i - 1]);

    BIG_XXX_sducopy(a, d);
    BIG_XXX_norm(a);
}

#endif

#if MODTYPE_YYY == NOT_SPECIAL

/* convert to Montgomery n-residue form */
void FP_YYY_nres(FP_YYY *y, BIG_XXX x)
{
    DBIG_XXX d;
    BIG_XXX r;
    BIG_XXX_rcopy(r, R2modp_YYY);
    BIG_XXX_mul(d, x, r);
    FP_YYY_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_YYY_redc(BIG_XXX x, FP_YYY *y)
{
    DBIG_XXX d;
    BIG_XXX_dzero(d);
    BIG_XXX_dscopy(d, y->g);
    FP_YYY_mod(x, d);
}


/* reduce a DBIG to a BIG using Montgomery's no trial division method */
/* d is expected to be dnormed before entry */
/* SU= 112 */
void FP_YYY_mod(BIG_XXX a, DBIG_XXX d)
{
    BIG_XXX mdls;
    BIG_XXX_rcopy(mdls, Modulus_YYY);
    BIG_XXX_monty(a, mdls, MConst_YYY, d);
}

#endif

void FP_YYY_from_int(FP_YYY *x,int a)
{
    BIG_XXX w;
    if (a<0) BIG_XXX_rcopy(w, Modulus_YYY);
    else BIG_XXX_zero(w); 
    BIG_XXX_inc(w,a); BIG_XXX_norm(w); 
    FP_YYY_nres(x,w);
}

/* test x==0 ? */
/* SU= 48 */
int FP_YYY_iszilch(FP_YYY *x)
{
    BIG_XXX m;
    FP_YYY y;
    FP_YYY_copy(&y,x);
    FP_YYY_reduce(&y);
    FP_YYY_redc(m,&y);
    return BIG_XXX_iszilch(m);
}

int FP_YYY_isunity(FP_YYY *x)
{
    BIG_XXX m;
    FP_YYY y;
    FP_YYY_copy(&y,x);
    FP_YYY_reduce(&y);
    FP_YYY_redc(m,&y);
    return BIG_XXX_isunity(m);
}


void FP_YYY_copy(FP_YYY *y, FP_YYY *x)
{
    BIG_XXX_copy(y->g, x->g);
    y->XES = x->XES;
}

void FP_YYY_rcopy(FP_YYY *y, const BIG_XXX c)
{
    BIG_XXX b;
    BIG_XXX_rcopy(b, c);
    FP_YYY_nres(y, b);
}

/* Swap a and b if d=1 */
void FP_YYY_cswap(FP_YYY *a, FP_YYY *b, int d)
{
    sign32 t, c = d;
    BIG_XXX_cswap(a->g, b->g, d);

    c = ~(c - 1);
    t = c & ((a->XES) ^ (b->XES));
    a->XES ^= t;
    b->XES ^= t;

}

/* Move b to a if d=1 */
void FP_YYY_cmove(FP_YYY *a, FP_YYY *b, int d)
{
    sign32 c = -d;

    BIG_XXX_cmove(a->g, b->g, d);
    a->XES ^= (a->XES ^ b->XES)&c;
}

void FP_YYY_zero(FP_YYY *x)
{
    BIG_XXX_zero(x->g);
    x->XES = 1;
}

int FP_YYY_equals(FP_YYY *x, FP_YYY *y)
{
    FP_YYY xg, yg;
    FP_YYY_copy(&xg, x);
    FP_YYY_copy(&yg, y);
    FP_YYY_reduce(&xg);
    FP_YYY_reduce(&yg);
    if (BIG_XXX_comp(xg.g, yg.g) == 0) return 1;
    return 0;
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP_YYY_islarger(FP_YYY *x)
{
    BIG_XXX p,fx,sx;
    if (FP_YYY_iszilch(x)) return 0;
    BIG_XXX_rcopy(p,Modulus_YYY);
    FP_YYY_redc(fx,x);
    BIG_XXX_sub(sx,p,fx);  BIG_XXX_norm(sx); 
    return BIG_XXX_comp(fx,sx);
}

void FP_YYY_toBytes(char *b,FP_YYY *x)
{
    BIG_XXX t;
    FP_YYY_redc(t, x);
    BIG_XXX_toBytes(b, t);
}

void FP_YYY_fromBytes(FP_YYY *x,char *b)
{
    BIG_XXX t;
    BIG_XXX_fromBytes(t, b);
    FP_YYY_nres(x, t);
}

/* output FP */
/* SU= 48 */
void FP_YYY_output(FP_YYY *r)
{
    BIG_XXX c;
    FP_YYY_reduce(r);
    FP_YYY_redc(c, r);
    BIG_XXX_output(c);
}

void FP_YYY_rawoutput(FP_YYY *r)
{
    BIG_XXX_rawoutput(r->g);
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
void FP_YYY_mul(FP_YYY *r, FP_YYY *a, FP_YYY *b)
{
    DBIG_XXX d;

    if ((sign64)a->XES * b->XES > (sign64)FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_YYY_reduce(a);  /* it is sufficient to fully reduce just one of them < p */
    }

#ifdef FUSED_MODMUL
    FP_YYY_modmul(r->g, a->g, b->g);
#else
    BIG_XXX_mul(d, a->g, b->g);
    FP_YYY_mod(r->g, d);
#endif
    r->XES = 2;
}


/* multiplication by an integer, r=a*c */
/* SU= 136 */
void FP_YYY_imul(FP_YYY *r, FP_YYY *a, int c)
{
    int s = 0;

    if (c < 0)
    {
        c = -c;
        s = 1;
    }

#if MODTYPE_YYY==PSEUDO_MERSENNE || MODTYPE_YYY==GENERALISED_MERSENNE
    DBIG_XXX d;
    BIG_XXX_pxmul(d, a->g, c);
    FP_YYY_mod(r->g, d);
    r->XES = 2;

#else
    //Montgomery
    BIG_XXX k;
    FP_YYY f;
    if (a->XES * c <= FEXCESS_YYY)
    {
        BIG_XXX_pmul(r->g, a->g, c);
        r->XES = a->XES * c; // careful here - XES jumps!
    }
    else
    {
        // don't want to do this - only a problem for Montgomery modulus and larger constants
        BIG_XXX_zero(k);
        BIG_XXX_inc(k, c);
        BIG_XXX_norm(k);
        FP_YYY_nres(&f, k);
        FP_YYY_mul(r, a, &f);
    }
#endif

    if (s)
    {
        FP_YYY_neg(r, r);
        FP_YYY_norm(r);
    }
}

/* Set r=a^2 mod m */
/* SU= 88 */
void FP_YYY_sqr(FP_YYY *r, FP_YYY *a)
{
    DBIG_XXX d;

    if ((sign64)a->XES * a->XES > (sign64)FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_YYY_reduce(a);
    }

    BIG_XXX_sqr(d, a->g);
    FP_YYY_mod(r->g, d);
    r->XES = 2;
}

/* SU= 16 */
/* Set r=a+b */
void FP_YYY_add(FP_YYY *r, FP_YYY *a, FP_YYY *b)
{
    BIG_XXX_add(r->g, a->g, b->g);
    r->XES = a->XES + b->XES;
    if (r->XES > FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("Sum too large - reducing it \n");
#endif
        FP_YYY_reduce(r);
    }
}

/* Set r=a-b mod m */
/* SU= 56 */
void FP_YYY_sub(FP_YYY *r, FP_YYY *a, FP_YYY *b)
{
    FP_YYY n;
    FP_YYY_neg(&n, b);
    FP_YYY_add(r, a, &n);
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
static int quo(BIG_XXX n, BIG_XXX m)
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
void FP_YYY_reduce(FP_YYY *a)
{
    BIG_XXX m, r;
    int sr, sb, q;
    chunk carry;

    BIG_XXX_rcopy(m, Modulus_YYY);

    BIG_XXX_norm(a->g);

    if (a->XES > 16)
    {
        q = quo(a->g, m);
        carry = BIG_XXX_pmul(r, m, q);
        r[NLEN_XXX - 1] += (carry << BASEBITS_XXX); // correction - put any carry out back in again
        BIG_XXX_sub(a->g, a->g, r);
        BIG_XXX_norm(a->g);
        sb = 2;
    }
    else sb = logb2(a->XES - 1); // sb does not depend on the actual data

    BIG_XXX_fshl(m, sb);

    while (sb > 0)
    {
// constant time...
        sr = BIG_XXX_ssn(r, a->g, m); // optimized combined shift, subtract and norm
        BIG_XXX_cmove(a->g, r, 1 - sr);
        sb--;
    }

    //BIG_XXX_mod(a->g,m);
    a->XES = 1;
}

void FP_YYY_norm(FP_YYY *x)
{
    BIG_XXX_norm(x->g);
}

/* Set r=-a mod Modulus */
/* SU= 64 */
void FP_YYY_neg(FP_YYY *r, FP_YYY *a)
{
    int sb;
    BIG_XXX m;

    BIG_XXX_rcopy(m, Modulus_YYY);

    sb = logb2(a->XES - 1);
    BIG_XXX_fshl(m, sb);
    BIG_XXX_sub(r->g, m, a->g);
    r->XES = ((sign32)1 << sb) + 1;

    if (r->XES > FEXCESS_YYY)
    {
#ifdef DEBUG_REDUCE
        printf("Negation too large -  reducing it \n");
#endif
        FP_YYY_reduce(r);
    }

}

/* Set r=a/2. */
/* SU= 56 */
void FP_YYY_div2(FP_YYY *r, FP_YYY *a)
{
    BIG_XXX m;
    BIG_XXX w;
    BIG_XXX_rcopy(m, Modulus_YYY);
    int pr=BIG_XXX_parity(a->g);

    FP_YYY_copy(r, a);
    BIG_XXX_copy(w,r->g);
    BIG_XXX_fshr(r->g,1);
    BIG_XXX_add(w, w, m);
    BIG_XXX_norm(w);
    BIG_XXX_fshr(w, 1);   
    
    BIG_XXX_cmove(r->g,w,pr);

}

// Could leak size of b
// but not used here with secret exponent b
void FP_YYY_pow(FP_YYY *r, FP_YYY *a, BIG_XXX b)
{
    sign8 w[1 + (NLEN_XXX * BASEBITS_XXX + 3) / 4];
    FP_YYY tb[16];
    BIG_XXX t;
    int i, nb;

    FP_YYY_copy(r,a);
    FP_YYY_norm(r);
    BIG_XXX_copy(t, b);
    BIG_XXX_norm(t);
    nb = 1 + (BIG_XXX_nbits(t) + 3) / 4;
    /* convert exponent to 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_XXX_lastbits(t, 4);
        BIG_XXX_dec(t, w[i]);
        BIG_XXX_norm(t);
        BIG_XXX_fshr(t, 4);
    }

    FP_YYY_one(&tb[0]);
    FP_YYY_copy(&tb[1], r);
    for (i = 2; i < 16; i++)
        FP_YYY_mul(&tb[i], &tb[i - 1], r);

    FP_YYY_copy(r, &tb[w[nb - 1]]);
    for (i = nb - 2; i >= 0; i--)
    {
        FP_YYY_sqr(r, r);
        FP_YYY_sqr(r, r);
        FP_YYY_sqr(r, r);
        FP_YYY_sqr(r, r);
        FP_YYY_mul(r, r, &tb[w[i]]);
    }
    FP_YYY_reduce(r);
}


#if MODTYPE_YYY == PSEUDO_MERSENNE || MODTYPE_YYY==GENERALISED_MERSENNE

// See eprint paper https://eprint.iacr.org/2018/1038
// If p=3 mod 4 r= x^{(p-3)/4}, if p=5 mod 8 r=x^{(p-5)/8}

static void FP_YYY_fpow(FP_YYY *r, FP_YYY *x)
{
    int i, j, k, bw, w, c, nw, lo, m, n, nd, e=PM1D2_YYY;
    FP_YYY xp[11], t, key;
    const int ac[] = {1, 2, 3, 6, 12, 15, 30, 60, 120, 240, 255};
// phase 1
    FP_YYY_copy(&xp[0], x); // 1
    FP_YYY_sqr(&xp[1], x); // 2
    FP_YYY_mul(&xp[2], &xp[1], x); //3
    FP_YYY_sqr(&xp[3], &xp[2]); // 6
    FP_YYY_sqr(&xp[4], &xp[3]); // 12
    FP_YYY_mul(&xp[5], &xp[4], &xp[2]); // 15
    FP_YYY_sqr(&xp[6], &xp[5]); // 30
    FP_YYY_sqr(&xp[7], &xp[6]); // 60
    FP_YYY_sqr(&xp[8], &xp[7]); // 120
    FP_YYY_sqr(&xp[9], &xp[8]); // 240
    FP_YYY_mul(&xp[10], &xp[9], &xp[5]); // 255

#if MODTYPE_YYY==PSEUDO_MERSENNE
    n = MODBITS_YYY;
#endif
#if MODTYPE_YYY==GENERALISED_MERSENNE  // Ed448 ONLY
    n = MODBITS_YYY / 2;
#endif

    n-=(e+1);
    c=(MConst_YYY+(1<<e)+1)/(1<<(e+1));

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
        FP_YYY_copy(&key, &xp[i]);
        k -= ac[i];
    }
    while (k != 0)
    {
        i--;
        if (ac[i] > k) continue;
        FP_YYY_mul(&key, &key, &xp[i]);
        k -= ac[i];
    }

// phase 2
    FP_YYY_copy(&xp[1], &xp[2]);
    FP_YYY_copy(&xp[2], &xp[5]);
    FP_YYY_copy(&xp[3], &xp[10]);

    j = 3; m = 8;
    nw = n - bw;
    while (2 * m < nw)
    {
        FP_YYY_copy(&t, &xp[j++]);
        for (i = 0; i < m; i++)
            FP_YYY_sqr(&t, &t);
        FP_YYY_mul(&xp[j], &xp[j - 1], &t);
        m *= 2;
    }

    lo = nw - m;
    FP_YYY_copy(r, &xp[j]);

    while (lo != 0)
    {
        m /= 2; j--;
        if (lo < m) continue;
        lo -= m;
        FP_YYY_copy(&t, r);
        for (i = 0; i < m; i++)
            FP_YYY_sqr(&t, &t);
        FP_YYY_mul(r, &t, &xp[j]);
    }
// phase 3

    if (bw != 0)
    {
        for (i = 0; i < bw; i++ )
            FP_YYY_sqr(r, r);
        FP_YYY_mul(r, r, &key);
    }
#if MODTYPE_YYY==GENERALISED_MERSENNE  // Ed448 ONLY
    FP_YYY_copy(&key, r);
    FP_YYY_sqr(&t, &key);
    FP_YYY_mul(r, &t, &xp[0]);
    for (i = 0; i < n + 1; i++)
        FP_YYY_sqr(r, r);
    FP_YYY_mul(r, r, &key);
#endif

    for (i=0;i<nd;i++)
        FP_YYY_sqr(r,r);
}

#endif


// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
void FP_YYY_progen(FP_YYY *r,FP_YYY *x)
{
#if MODTYPE_YYY==PSEUDO_MERSENNE  || MODTYPE_YYY==GENERALISED_MERSENNE
    FP_YYY_fpow(r, x);  
#else
    int e=PM1D2_YYY;
    BIG_XXX m;
    BIG_XXX_rcopy(m, Modulus_YYY);
    BIG_XXX_dec(m,1);
    BIG_XXX_shr(m,e);
    BIG_XXX_dec(m,1);
    BIG_XXX_fshr(m,1);
    FP_YYY_pow(r,x,m);
#endif
}

/* Is x a QR? return optional hint for fast follow-up square root */
int FP_YYY_qr(FP_YYY *x,FP_YYY *h)
{
    FP_YYY r;
    int i,e=PM1D2_YYY;
    FP_YYY_progen(&r,x);
    if (h!=NULL)
        FP_YYY_copy(h,&r);

    FP_YYY_sqr(&r,&r);
    FP_YYY_mul(&r,x,&r);
    for (i=0;i<e-1;i++ )
        FP_YYY_sqr(&r,&r);


//    for (i=0;i<e;i++)
//        FP_YYY_sqr(&r,&r);
//    FP_YYY_copy(&s,x);
//    for (i=0;i<e-1;i++ )
//        FP_YYY_sqr(&s,&s);
//    FP_YYY_mul(&r,&r,&s);
    
    return FP_YYY_isunity(&r);
}

/* Modular inversion */
void FP_YYY_inv(FP_YYY *r,FP_YYY *x,FP_YYY *h)
{
    int i,e=PM1D2_YYY;
    FP_YYY s,t;
    FP_YYY_norm(x);
    FP_YYY_copy(&s,x);

    if (h==NULL)
        FP_YYY_progen(&t,x);
    else
        FP_YYY_copy(&t,h);

    for (i=0;i<e-1;i++)
    {  
        FP_YYY_sqr(&s,&s);
        FP_YYY_mul(&s,&s,x);
    }
  
    for (i=0;i<=e;i++)
        FP_YYY_sqr(&t,&t);
    
    FP_YYY_mul(r,&t,&s);
    FP_YYY_reduce(r);
}

// Tonelli-Shanks in constant time
void FP_YYY_sqrt(FP_YYY *r, FP_YYY *a, FP_YYY *h)
{
    int i,j,k,u,e=PM1D2_YYY;
    FP_YYY v,g,t,b;
    BIG_XXX m;

    if (h==NULL)
        FP_YYY_progen(&g,a);
    else
        FP_YYY_copy(&g,h);

    BIG_XXX_rcopy(m,ROI_YYY);
    FP_YYY_nres(&v,m);

    FP_YYY_sqr(&t,&g);
    FP_YYY_mul(&t,&t,a);
   
    FP_YYY_mul(r,&g,a);
    FP_YYY_copy(&b,&t);
    for (k=e;k>1;k--)
    {
        for (j=1;j<k-1;j++)
            FP_YYY_sqr(&b,&b);
        u=1-FP_YYY_isunity(&b);
        FP_YYY_mul(&g,r,&v);
        FP_YYY_cmove(r,&g,u);
        FP_YYY_sqr(&v,&v);
        FP_YYY_mul(&g,&t,&v);
        FP_YYY_cmove(&t,&g,u);
        FP_YYY_copy(&b,&t);
    }
// always return +ve square root
    k=FP_YYY_sign(r);
    FP_YYY_neg(&v,r); FP_YYY_norm(&v);
    FP_YYY_cmove(r,&v,k);
}

// Calculate both inverse and square root of x, return QR
int FP_YYY_invsqrt(FP_YYY *i, FP_YYY *s, FP_YYY *x)
{
    FP_YYY h;
    int qr=FP_YYY_qr(x,&h);
    FP_YYY_sqrt(s,x,&h);
    FP_YYY_inv(i,x,&h);
    return qr;
}

// Two for Price of One - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
int FP_YYY_tpo(FP_YYY *i, FP_YYY *s)
{
    int qr;
    FP_YYY w,t;
    FP_YYY_mul(&w,s,i);
    FP_YYY_mul(&t,&w,i);
    qr=FP_YYY_invsqrt(i,s,&t);
    FP_YYY_mul(i,i,&w);
    FP_YYY_mul(s,s,i);
    return qr;
}

/* SU=8 */
/* set n=1 */
void FP_YYY_one(FP_YYY *n)
{
    BIG_XXX b;
    BIG_XXX_one(b);
    FP_YYY_nres(n, b);
}

int FP_YYY_sign(FP_YYY *x)
{
#ifdef BIG_ENDIAN_SIGN_YYY
    int cp;
    BIG_XXX m,pm1d2;
    FP_YYY y;
    BIG_XXX_rcopy(pm1d2, Modulus_YYY);
    BIG_XXX_dec(pm1d2,1);
    BIG_XXX_fshr(pm1d2,1); //(p-1)/2
     
    FP_YYY_copy(&y,x);
    FP_YYY_reduce(&y);
    FP_YYY_redc(m,&y);
    cp=BIG_XXX_comp(m,pm1d2);
    return ((cp+1)&2)>>1;

#else
    BIG_XXX m;
    FP_YYY y;
    FP_YYY_copy(&y,x);
    FP_YYY_reduce(&y);
    FP_YYY_redc(m,&y);
    return BIG_XXX_parity(m);
#endif
}

void FP_YYY_rand(FP_YYY *x,csprng *rng)
{
    BIG_XXX w,m;
    BIG_XXX_rcopy(m,Modulus_YYY);
    BIG_XXX_randomnum(w,m,rng);
    FP_YYY_nres(x,w);
}


