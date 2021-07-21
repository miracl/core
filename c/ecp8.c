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

/* CORE Weierstrass elliptic curve functions over FP2 */

//#include <iostream>
#include "ecp8_ZZZ.h"


int ECP8_ZZZ_isinf(ECP8_ZZZ *P)
{
    return (FP8_YYY_iszilch(&(P->x)) & FP8_YYY_iszilch(&(P->z)));
}

/* Set P=Q */
void ECP8_ZZZ_copy(ECP8_ZZZ *P, ECP8_ZZZ *Q)
{
    FP8_YYY_copy(&(P->x), &(Q->x));
    FP8_YYY_copy(&(P->y), &(Q->y));
    FP8_YYY_copy(&(P->z), &(Q->z));
}

/* set P to Infinity */
void ECP8_ZZZ_inf(ECP8_ZZZ *P)
{
    FP8_YYY_zero(&(P->x));
    FP8_YYY_one(&(P->y));
    FP8_YYY_zero(&(P->z));
}

/* Conditional move Q to P dependant on d */
static void ECP8_ZZZ_cmove(ECP8_ZZZ *P, ECP8_ZZZ *Q, int d)
{
    FP8_YYY_cmove(&(P->x), &(Q->x), d);
    FP8_YYY_cmove(&(P->y), &(Q->y), d);
    FP8_YYY_cmove(&(P->z), &(Q->z), d);
}

/* return 1 if b==c, no branching */
static int teq(sign32 b, sign32 c)
{
    sign32 x = b ^ c;
    x -= 1; // if x=0, x now -1
    return (int)((x >> 31) & 1);
}

/* Constant time select from pre-computed table */
static void ECP8_ZZZ_select(ECP8_ZZZ *P, ECP8_ZZZ W[], sign32 b)
{
    ECP8_ZZZ MP;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    ECP8_ZZZ_cmove(P, &W[0], teq(babs, 0)); // conditional move
    ECP8_ZZZ_cmove(P, &W[1], teq(babs, 1));
    ECP8_ZZZ_cmove(P, &W[2], teq(babs, 2));
    ECP8_ZZZ_cmove(P, &W[3], teq(babs, 3));
    ECP8_ZZZ_cmove(P, &W[4], teq(babs, 4));
    ECP8_ZZZ_cmove(P, &W[5], teq(babs, 5));
    ECP8_ZZZ_cmove(P, &W[6], teq(babs, 6));
    ECP8_ZZZ_cmove(P, &W[7], teq(babs, 7));

    ECP8_ZZZ_copy(&MP, P);
    ECP8_ZZZ_neg(&MP);  // minus P
    ECP8_ZZZ_cmove(P, &MP, (int)(m & 1));
}

/* Make P affine (so z=1) */
void ECP8_ZZZ_affine(ECP8_ZZZ *P)
{
    FP8_YYY one, iz;
    if (ECP8_ZZZ_isinf(P)) return;

    FP8_YYY_one(&one);
    if (FP8_YYY_isunity(&(P->z)))
    {
        FP8_YYY_reduce(&(P->x));
        FP8_YYY_reduce(&(P->y));
        return;
    }

    FP8_YYY_inv(&iz, &(P->z),NULL);
    FP8_YYY_mul(&(P->x), &(P->x), &iz);
    FP8_YYY_mul(&(P->y), &(P->y), &iz);

    FP8_YYY_reduce(&(P->x));
    FP8_YYY_reduce(&(P->y));
    FP8_YYY_copy(&(P->z), &one);
}

/* return 1 if P==Q, else 0 */
/* SU= 312 */
int ECP8_ZZZ_equals(ECP8_ZZZ *P, ECP8_ZZZ *Q)
{
    FP8_YYY a, b;

    FP8_YYY_mul(&a, &(P->x), &(Q->z));
    FP8_YYY_mul(&b, &(Q->x), &(P->z));
    if (!FP8_YYY_equals(&a, &b)) return 0;

    FP8_YYY_mul(&a, &(P->y), &(Q->z));
    FP8_YYY_mul(&b, &(Q->y), &(P->z));
    if (!FP8_YYY_equals(&a, &b)) return 0;
    return 1;
}

/* extract x, y from point P */
int ECP8_ZZZ_get(FP8_YYY *x, FP8_YYY *y, ECP8_ZZZ *P)
{
    ECP8_ZZZ W;
    ECP8_ZZZ_copy(&W, P);
    ECP8_ZZZ_affine(&W);
    if (ECP8_ZZZ_isinf(&W)) return -1;
    FP8_YYY_copy(y, &(W.y));
    FP8_YYY_copy(x, &(W.x));
    return 0;
}

/* Output point P */
void ECP8_ZZZ_output(ECP8_ZZZ *P)
{
    FP8_YYY x, y;
    if (ECP8_ZZZ_isinf(P))
    {
        printf("Infinity\n");
        return;
    }
    ECP8_ZZZ_get(&x, &y, P);
    printf("(");
    FP8_YYY_output(&x);
    printf(",");
    FP8_YYY_output(&y);
    printf(")\n");
}

/* Convert Q to octet string */
void ECP8_ZZZ_toOctet(octet *W, ECP8_ZZZ *Q,bool compress)
{
    FP8_YYY qx, qy;
    bool alt=false;
    ECP8_ZZZ_get(&qx, &qy, Q);
  
#if (MBITS-1)%8 <= 4
#ifdef ALLOW_ALT_COMPRESS_ZZZ
    alt=true;
#endif
#endif

    if (alt)
    {
        FP8_YYY_toBytes(&(W->val[0]),&qx);
        if (!compress)
        {
            W->len=16*MODBYTES_XXX;
            FP8_YYY_toBytes(&(W->val[8*MODBYTES_XXX]), &qy);
        } else {
            W->val[0]|=0x80;
            if (FP8_YYY_islarger(&qy)==1) W->val[0]|=0x20;
            W->len=8*MODBYTES_XXX;
        }
    } else {
        FP8_YYY_toBytes(&(W->val[1]),&qx);
        if (!compress)
        {
            W->val[0] = 0x04;

            FP8_YYY_toBytes(&(W->val[8 * MODBYTES_XXX+1]), &qy);
            W->len = 16 * MODBYTES_XXX+1;
        } else {
            W->val[0]=0x02;
            if (FP8_YYY_sign(&qy)==1) W->val[0] = 0x03;
            W->len = 8 * MODBYTES_XXX+1;
        }
    }
}

/* restore Q from octet string */
int ECP8_ZZZ_fromOctet(ECP8_ZZZ *Q, octet *W)
{
    FP8_YYY qx, qy;
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
        FP8_YYY_fromBytes(&qx,&(W->val[0]));
        W->val[0]=typ;
        if ((typ&0x80)==0)
        {
            FP8_YYY_fromBytes(&qy,&(W->val[8*MODBYTES_XXX]));
            if (ECP8_ZZZ_set(Q, &qx, &qy)) return 1;
            return 0;
        } else {
            if (!ECP8_ZZZ_setx(Q,&qx,0)) return 0;
            sgn=(typ&0x20)>>5;
            cmp=FP8_YYY_islarger(&(Q->y));
            if ((sgn==1 && cmp!=1) || (sgn==0 && cmp==1)) ECP8_ZZZ_neg(Q);
            return 1;
        }
    } else {
        FP8_YYY_fromBytes(&qx,&(W->val[1]));

        if (typ == 0x04)
        {
            FP8_YYY_fromBytes(&qy,&(W->val[8 * MODBYTES_XXX+1]));
            if (ECP8_ZZZ_set(Q, &qx, &qy)) return 1;
        } else {
            if (ECP8_ZZZ_setx(Q, &qx, typ&1)) return 1;
        }
    }
    return 0;
}

/* Calculate RHS of twisted curve equation x^3+B/i or x^3+Bi*/
void ECP8_ZZZ_rhs(FP8_YYY *rhs, FP8_YYY *x)
{
    /* calculate RHS of elliptic curve equation */
    FP8_YYY t;
    FP4_YYY t4;
    FP2_YYY t2;
    BIG_XXX b;
    FP8_YYY_sqr(&t, x);

    FP8_YYY_mul(rhs, &t, x);

    /* Assuming CURVE_A=0 */

    BIG_XXX_rcopy(b, CURVE_B_ZZZ);

    FP2_YYY_from_BIG(&t2, b);
    FP4_YYY_from_FP2(&t4, &t2);
    FP8_YYY_from_FP4(&t, &t4);

#if SEXTIC_TWIST_ZZZ == D_TYPE
    FP8_YYY_div_i(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

#if SEXTIC_TWIST_ZZZ == M_TYPE
    FP8_YYY_times_i(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

    FP8_YYY_add(rhs, &t, rhs);
    FP8_YYY_reduce(rhs);
}

/* Set P=(x,y). Return 1 if (x,y) is on the curve, else return 0*/
/* SU= 232 */
int ECP8_ZZZ_set(ECP8_ZZZ *P, FP8_YYY *x, FP8_YYY *y)
{
    FP8_YYY rhs, y2;

    FP8_YYY_sqr(&y2, y);
    ECP8_ZZZ_rhs(&rhs, x);

    if (!FP8_YYY_equals(&y2, &rhs))
    {
        ECP8_ZZZ_inf(P);
        return 0;
    }

    FP8_YYY_copy(&(P->x), x);
    FP8_YYY_copy(&(P->y), y);

    FP8_YYY_one(&(P->z));
    return 1;
}

/* Set P=(x,y). Return 1 if (x,.) is on the curve, else return 0 */
/* SU= 232 */
int ECP8_ZZZ_setx(ECP8_ZZZ *P, FP8_YYY *x,int s)
{
    FP8_YYY y;
    FP_YYY h;
    ECP8_ZZZ_rhs(&y, x);

    if (!FP8_YYY_qr(&y,&h))
    {
        ECP8_ZZZ_inf(P);
        return 0;
    }
    FP8_YYY_sqrt(&y, &y, &h);
    FP8_YYY_copy(&(P->x), x);
    FP8_YYY_copy(&(P->y), &y);
    FP8_YYY_one(&(P->z));
    if (FP8_YYY_sign(&(P->y)) != s)
        FP8_YYY_neg(&(P->y),&(P->y));
    FP8_YYY_reduce(&(P->y));
    return 1;
}

/* Set P=-P */
/* SU= 8 */
void ECP8_ZZZ_neg(ECP8_ZZZ *P)
{
    FP8_YYY_norm(&(P->y));
    FP8_YYY_neg(&(P->y), &(P->y));
    FP8_YYY_norm(&(P->y));
}



/* R+=R */
/* return -1 for Infinity, 0 for addition, 1 for doubling */
int ECP8_ZZZ_dbl(ECP8_ZZZ *P)
{
    FP8_YYY t0, t1, t2, t3, iy, x3, y3;

    FP8_YYY_copy(&iy, &(P->y));     //FP8_YYY iy=new FP8_YYY(y);
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_YYY_times_i(&iy);           //iy.mul_ip();
#endif

    FP8_YYY_sqr(&t0, &(P->y));          //t0.sqr();
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_YYY_times_i(&t0);           //t0.mul_ip();
#endif

    FP8_YYY_mul(&t1, &iy, &(P->z)); //t1.mul(z);
    FP8_YYY_sqr(&t2, &(P->z));              //t2.sqr();

    FP8_YYY_add(&(P->z), &t0, &t0); //z.add(t0);
    FP8_YYY_norm(&(P->z));              //z.norm();
    FP8_YYY_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP8_YYY_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP8_YYY_norm(&(P->z));          //z.norm();

    FP8_YYY_imul(&t2, &t2, 3 * CURVE_B_I_ZZZ); //t2.imul(3*ROM.CURVE_B_I);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP8_YYY_times_i(&t2);
#endif

    FP8_YYY_mul(&x3, &t2, &(P->z)); //x3.mul(z);

    FP8_YYY_add(&y3, &t0, &t2);     //y3.add(t2);
    FP8_YYY_norm(&y3);              //y3.norm();
    FP8_YYY_mul(&(P->z), &(P->z), &t1); //z.mul(t1);

    FP8_YYY_add(&t1, &t2, &t2);     //t1.add(t2);
    FP8_YYY_add(&t2, &t2, &t1);     //t2.add(t1);
    FP8_YYY_norm(&t2);              //t2.norm();
    FP8_YYY_sub(&t0, &t0, &t2);     //t0.sub(t2);
    FP8_YYY_norm(&t0);              //t0.norm();                           //y^2-9bz^2
    FP8_YYY_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP8_YYY_add(&(P->y), &y3, &x3);     //y3.add(x3);                          //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2

    FP8_YYY_mul(&t1, &(P->x), &iy);     //t1.mul(iy);                       //

    FP8_YYY_norm(&t0);          //x.norm();
    FP8_YYY_mul(&(P->x), &t0, &t1); //x.mul(t1);
    FP8_YYY_add(&(P->x), &(P->x), &(P->x)); //x.add(x);       //(y^2-9bz^2)xy2

    FP8_YYY_norm(&(P->x));          //x.norm();

    FP8_YYY_norm(&(P->y));          //y.norm();

    return 1;
}

/* Set P+=Q */

int ECP8_ZZZ_add(ECP8_ZZZ *P, ECP8_ZZZ *Q)
{
    FP8_YYY t0, t1, t2, t3, t4, x3, y3, z3;
    int b3 = 3 * CURVE_B_I_ZZZ;

    FP8_YYY_mul(&t0, &(P->x), &(Q->x)); //t0.mul(Q.x);         // x.Q.x
    FP8_YYY_mul(&t1, &(P->y), &(Q->y)); //t1.mul(Q.y);       // y.Q.y

    FP8_YYY_mul(&t2, &(P->z), &(Q->z)); //t2.mul(Q.z);
    FP8_YYY_add(&t3, &(P->x), &(P->y)); //t3.add(y);
    FP8_YYY_norm(&t3);              //t3.norm();          //t3=X1+Y1
    FP8_YYY_add(&t4, &(Q->x), &(Q->y)); //t4.add(Q.y);
    FP8_YYY_norm(&t4);              //t4.norm();            //t4=X2+Y2
    FP8_YYY_mul(&t3, &t3, &t4);     //t3.mul(t4);                       //t3=(X1+Y1)(X2+Y2)
    FP8_YYY_add(&t4, &t0, &t1);     //t4.add(t1);       //t4=X1.X2+Y1.Y2

    FP8_YYY_sub(&t3, &t3, &t4);     //t3.sub(t4);
    FP8_YYY_norm(&t3);              //t3.norm();
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_YYY_times_i(&t3);           //t3.mul_ip();  //t3=(X1+Y1)(X2+Y2)-(X1.X2+Y1.Y2) = X1.Y2+X2.Y1
#endif

    FP8_YYY_add(&t4, &(P->y), &(P->z)); //t4.add(z);
    FP8_YYY_norm(&t4);              //t4.norm();            //t4=Y1+Z1

    FP8_YYY_add(&x3, &(Q->y), &(Q->z)); //x3.add(Q.z);
    FP8_YYY_norm(&x3);              //x3.norm();            //x3=Y2+Z2

    FP8_YYY_mul(&t4, &t4, &x3);     //t4.mul(x3);                       //t4=(Y1+Z1)(Y2+Z2)

    FP8_YYY_add(&x3, &t1, &t2);     //x3.add(t2);                       //X3=Y1.Y2+Z1.Z2

    FP8_YYY_sub(&t4, &t4, &x3);     //t4.sub(x3);
    FP8_YYY_norm(&t4);              //t4.norm();
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_YYY_times_i(&t4);           //t4.mul_ip(); //t4=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1
#endif

    FP8_YYY_add(&x3, &(P->x), &(P->z)); //x3.add(z);
    FP8_YYY_norm(&x3);              //x3.norm();    // x3=X1+Z1

    FP8_YYY_add(&y3, &(Q->x), &(Q->z)); //y3.add(Q.z);
    FP8_YYY_norm(&y3);              //y3.norm();                // y3=X2+Z2
    FP8_YYY_mul(&x3, &x3, &y3);     //x3.mul(y3);                           // x3=(X1+Z1)(X2+Z2)

    FP8_YYY_add(&y3, &t0, &t2);     //y3.add(t2);                           // y3=X1.X2+Z1+Z2
    FP8_YYY_sub(&y3, &x3, &y3);     //y3.rsub(x3);
    FP8_YYY_norm(&y3);              //y3.norm();                // y3=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP8_YYY_times_i(&t0);           //t0.mul_ip();
    FP8_YYY_times_i(&t1);           //t1.mul_ip();
#endif

    FP8_YYY_add(&x3, &t0, &t0);     //x3.add(t0);
    FP8_YYY_add(&t0, &t0, &x3);     //t0.add(x3);
    FP8_YYY_norm(&t0);              //t0.norm();
    FP8_YYY_imul(&t2, &t2, b3);     //t2.imul(b);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP8_YYY_times_i(&t2);
#endif

    FP8_YYY_add(&z3, &t1, &t2);     //z3.add(t2);
    FP8_YYY_norm(&z3);              //z3.norm();
    FP8_YYY_sub(&t1, &t1, &t2);     //t1.sub(t2);
    FP8_YYY_norm(&t1);              //t1.norm();
    FP8_YYY_imul(&y3, &y3, b3);     //y3.imul(b);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP8_YYY_times_i(&y3);
#endif

    FP8_YYY_mul(&x3, &y3, &t4);     //x3.mul(t4);

    FP8_YYY_mul(&t2, &t3, &t1);     //t2.mul(t1);
    FP8_YYY_sub(&(P->x), &t2, &x3);     //x3.rsub(t2);
    FP8_YYY_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP8_YYY_mul(&t1, &t1, &z3);     //t1.mul(z3);
    FP8_YYY_add(&(P->y), &y3, &t1);     //y3.add(t1);
    FP8_YYY_mul(&t0, &t0, &t3);     //t0.mul(t3);
    FP8_YYY_mul(&z3, &z3, &t4);     //z3.mul(t4);
    FP8_YYY_add(&(P->z), &z3, &t0);     //z3.add(t0);


    FP8_YYY_norm(&(P->x));          //x.norm();
    FP8_YYY_norm(&(P->y));          //y.norm();
    FP8_YYY_norm(&(P->z));          //z.norm();

    return 0;
}

/* Set P-=Q */
/* SU= 16 */
void ECP8_ZZZ_sub(ECP8_ZZZ *P, ECP8_ZZZ *Q)
{
    ECP8_ZZZ NQ;
    ECP8_ZZZ_copy(&NQ, Q);
    ECP8_ZZZ_neg(&NQ);
    ECP8_ZZZ_add(P, &NQ);
}


void ECP8_ZZZ_reduce(ECP8_ZZZ *P)
{
    FP8_YYY_reduce(&(P->x));
    FP8_YYY_reduce(&(P->y));
    FP8_YYY_reduce(&(P->z));
}

/* P*=e */
/* SU= 280 */
void ECP8_ZZZ_mul(ECP8_ZZZ *P, BIG_XXX e)
{
    /* fixed size windows */
    int i, nb, s, ns;
    BIG_XXX mt, t;
    ECP8_ZZZ Q, W[8], C;
    sign8 w[1 + (NLEN_XXX * BASEBITS_XXX + 3) / 4];

    if (ECP8_ZZZ_isinf(P)) return;

    /* precompute table */

    ECP8_ZZZ_copy(&Q, P);
    ECP8_ZZZ_dbl(&Q);
    ECP8_ZZZ_copy(&W[0], P);

    for (i = 1; i < 8; i++)
    {
        ECP8_ZZZ_copy(&W[i], &W[i - 1]);
        ECP8_ZZZ_add(&W[i], &Q);
    }

    /* make exponent odd - add 2P if even, P if odd */
    BIG_XXX_copy(t, e);
    s = BIG_XXX_parity(t);
    BIG_XXX_inc(t, 1);
    BIG_XXX_norm(t);
    ns = BIG_XXX_parity(t);
    BIG_XXX_copy(mt, t);
    BIG_XXX_inc(mt, 1);
    BIG_XXX_norm(mt);
    BIG_XXX_cmove(t, mt, s);
    ECP8_ZZZ_cmove(&Q, P, ns);
    ECP8_ZZZ_copy(&C, &Q);

    nb = 1 + (BIG_XXX_nbits(t) + 3) / 4;

    /* convert exponent to signed 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_XXX_lastbits(t, 5) - 16;
        BIG_XXX_dec(t, w[i]);
        BIG_XXX_norm(t);
        BIG_XXX_fshr(t, 4);
    }
    w[nb] = BIG_XXX_lastbits(t, 5);

    //ECP8_ZZZ_copy(P, &W[(w[nb] - 1) / 2]);
    ECP8_ZZZ_select(P, W, w[i]);
    for (i = nb - 1; i >= 0; i--)
    {
        ECP8_ZZZ_select(&Q, W, w[i]);
        ECP8_ZZZ_dbl(P);
        ECP8_ZZZ_dbl(P);
        ECP8_ZZZ_dbl(P);
        ECP8_ZZZ_dbl(P);
        ECP8_ZZZ_add(P, &Q);
    }
    ECP8_ZZZ_sub(P, &C); /* apply correction */
    ECP8_ZZZ_affine(P);
}

void ECP8_ZZZ_frob_constants(FP2_YYY F[3])
{
    FP_YYY fx, fy;
    FP2_YYY X;

    FP_YYY_rcopy(&fx, Fra_YYY);
    FP_YYY_rcopy(&fy, Frb_YYY);
    FP2_YYY_from_FPs(&X, &fx, &fy); // (p-19)/24


    FP2_YYY_sqr(&F[0], &X);         // FF=F^2=(1+i)^(p-19)/12
    FP2_YYY_copy(&F[2], &F[0]);
    FP2_YYY_mul_ip(&F[2]);          // W=(1+i)^12/12.(1+i)^(p-19)/12 = (1+i)^(p-7)/12
    FP2_YYY_norm(&F[2]);
    FP2_YYY_sqr(&F[1], &F[2]);
    FP2_YYY_mul(&F[2], &F[2], &F[1]); // W=(1+i)^(p-7)/4

    FP2_YYY_mul_ip(&F[2]);          // W=(1+i)^4/4.W=(1+i)^(p-7)/4 = (1+i)^(p-3)/4
    FP2_YYY_norm(&F[2]);

    FP2_YYY_copy(&F[1], &X);


#if SEXTIC_TWIST_ZZZ == M_TYPE
    FP2_YYY_mul_ip(&F[1]);      // (1+i)^24/24.(1+i)^(p-19)/24 = (1+i)^(p+5)/24
    FP2_YYY_norm(&F[1]);
    FP2_YYY_inv(&F[1], &F[1], NULL);      // (1+i)^-(p+5)/24
    FP2_YYY_sqr(&F[0], &F[1]);      // (1+i)^-(p+5)/12
    FP2_YYY_mul(&F[1], &F[1], &F[0]); // (1+i)^-(p+5)/8  - ***
#endif

#if SEXTIC_TWIST_ZZZ == D_TYPE
    FP2_YYY_sqr(&F[0], &F[1]);  // (p-19)/12
    FP2_YYY_mul(&F[1], &F[1], &F[0]); // (p-19)/8
    FP2_YYY_mul_ip(&F[0]);      // (p-7)/12  - ***
    FP2_YYY_norm(&F[0]);
    FP2_YYY_mul_ip(&F[1]);      // (p-11)/8
    FP2_YYY_norm(&F[1]);
    FP2_YYY_mul_ip(&F[1]);      // (p-3)/8   - ***
    FP2_YYY_norm(&F[1]);
#endif
}

/* Calculates q^n.P using Frobenius constant X */
void ECP8_ZZZ_frob(ECP8_ZZZ *P, FP2_YYY F[3], int n)
{
    int i;
    FP8_YYY X, Y, Z;

    FP8_YYY_copy(&X, &(P->x));
    FP8_YYY_copy(&Y, &(P->y));
    FP8_YYY_copy(&Z, &(P->z));

    for (i = 0; i < n; i++)
    {
        FP8_YYY_frob(&X, &F[2]);    // X^p

#if SEXTIC_TWIST_ZZZ == M_TYPE
        FP8_YYY_qmul(&X, &X, &F[0]); // X^p.u^-(p+5)/12
        FP8_YYY_times_i2(&X);       // X^p.u^{1/2}.u^-(p+5)/12 = X^p.u^-(p-1)/12
#endif
#if SEXTIC_TWIST_ZZZ == D_TYPE
        FP8_YYY_qmul(&X, &X, &F[0]); // X^p.u^(p-7)/12
        FP8_YYY_times_i2(&X);       // X^p.u^{1/2}.u^(p-7)/12 = X^p.u^(p-1)/12
#endif

        FP8_YYY_frob(&Y, &F[2]);    // Y^p
#if SEXTIC_TWIST_ZZZ == M_TYPE
        FP8_YYY_qmul(&Y, &Y, &F[1]); // Y^p.u^-(p+5)/8
        FP8_YYY_times_i2(&Y);
        FP8_YYY_times_i(&Y);        // Y^p.u^(3/4}.u^-(p+5)/8 = Y.p.u^(p-1)/8

#endif
#if SEXTIC_TWIST_ZZZ == D_TYPE
        FP8_YYY_qmul(&Y, &Y, &F[1]); // Y^p.u^(p-3)/8
        FP8_YYY_times_i(&Y);        // Y^p.u^(1/4}.u^(p-3)/8 = Y.p.u^(p-1)/8
#endif
        FP8_YYY_frob(&Z, &F[2]);
    }

    FP8_YYY_copy(&(P->x), &X);
    FP8_YYY_copy(&(P->y), &Y);
    FP8_YYY_copy(&(P->z), &Z);
}

/* Side channel attack secure */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf

void ECP8_ZZZ_mul16(ECP8_ZZZ *P, ECP8_ZZZ Q[16], BIG_XXX u[16])
{
    int i, j, k, nb, pb1, pb2, pb3, pb4, bt;
    ECP8_ZZZ T1[8], T2[8], T3[8], T4[8], W;
    BIG_XXX mt, t[16];
    sign8 w1[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s1[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 w2[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s2[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 w3[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s3[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 w4[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s4[NLEN_XXX * BASEBITS_XXX + 1];

    for (i = 0; i < 16; i++)
    {
        BIG_XXX_copy(t[i], u[i]);
    }
// Precomputed table
    ECP8_ZZZ_copy(&T1[0], &Q[0]); // Q[0]
    ECP8_ZZZ_copy(&T1[1], &T1[0]);
    ECP8_ZZZ_add(&T1[1], &Q[1]); // Q[0]+Q[1]
    ECP8_ZZZ_copy(&T1[2], &T1[0]);
    ECP8_ZZZ_add(&T1[2], &Q[2]); // Q[0]+Q[2]
    ECP8_ZZZ_copy(&T1[3], &T1[1]);
    ECP8_ZZZ_add(&T1[3], &Q[2]); // Q[0]+Q[1]+Q[2]
    ECP8_ZZZ_copy(&T1[4], &T1[0]);
    ECP8_ZZZ_add(&T1[4], &Q[3]); // Q[0]+Q[3]
    ECP8_ZZZ_copy(&T1[5], &T1[1]);
    ECP8_ZZZ_add(&T1[5], &Q[3]); // Q[0]+Q[1]+Q[3]
    ECP8_ZZZ_copy(&T1[6], &T1[2]);
    ECP8_ZZZ_add(&T1[6], &Q[3]); // Q[0]+Q[2]+Q[3]
    ECP8_ZZZ_copy(&T1[7], &T1[3]);
    ECP8_ZZZ_add(&T1[7], &Q[3]); // Q[0]+Q[1]+Q[2]+Q[3]

    ECP8_ZZZ_copy(&T2[0], &Q[4]); // Q[0]
    ECP8_ZZZ_copy(&T2[1], &T2[0]);
    ECP8_ZZZ_add(&T2[1], &Q[5]); // Q[0]+Q[1]
    ECP8_ZZZ_copy(&T2[2], &T2[0]);
    ECP8_ZZZ_add(&T2[2], &Q[6]); // Q[0]+Q[2]
    ECP8_ZZZ_copy(&T2[3], &T2[1]);
    ECP8_ZZZ_add(&T2[3], &Q[6]); // Q[0]+Q[1]+Q[2]
    ECP8_ZZZ_copy(&T2[4], &T2[0]);
    ECP8_ZZZ_add(&T2[4], &Q[7]); // Q[0]+Q[3]
    ECP8_ZZZ_copy(&T2[5], &T2[1]);
    ECP8_ZZZ_add(&T2[5], &Q[7]); // Q[0]+Q[1]+Q[3]
    ECP8_ZZZ_copy(&T2[6], &T2[2]);
    ECP8_ZZZ_add(&T2[6], &Q[7]); // Q[0]+Q[2]+Q[3]
    ECP8_ZZZ_copy(&T2[7], &T2[3]);
    ECP8_ZZZ_add(&T2[7], &Q[7]); // Q[0]+Q[1]+Q[2]+Q[3]

    ECP8_ZZZ_copy(&T3[0], &Q[8]); // Q[0]
    ECP8_ZZZ_copy(&T3[1], &T3[0]);
    ECP8_ZZZ_add(&T3[1], &Q[9]); // Q[0]+Q[1]
    ECP8_ZZZ_copy(&T3[2], &T3[0]);
    ECP8_ZZZ_add(&T3[2], &Q[10]);   // Q[0]+Q[2]
    ECP8_ZZZ_copy(&T3[3], &T3[1]);
    ECP8_ZZZ_add(&T3[3], &Q[10]);   // Q[0]+Q[1]+Q[2]
    ECP8_ZZZ_copy(&T3[4], &T3[0]);
    ECP8_ZZZ_add(&T3[4], &Q[11]); // Q[0]+Q[3]
    ECP8_ZZZ_copy(&T3[5], &T3[1]);
    ECP8_ZZZ_add(&T3[5], &Q[11]);   // Q[0]+Q[1]+Q[3]
    ECP8_ZZZ_copy(&T3[6], &T3[2]);
    ECP8_ZZZ_add(&T3[6], &Q[11]);   // Q[0]+Q[2]+Q[3]
    ECP8_ZZZ_copy(&T3[7], &T3[3]);
    ECP8_ZZZ_add(&T3[7], &Q[11]);   // Q[0]+Q[1]+Q[2]+Q[3]

    ECP8_ZZZ_copy(&T4[0], &Q[12]); // Q[0]
    ECP8_ZZZ_copy(&T4[1], &T4[0]);
    ECP8_ZZZ_add(&T4[1], &Q[13]);   // Q[0]+Q[1]
    ECP8_ZZZ_copy(&T4[2], &T4[0]);
    ECP8_ZZZ_add(&T4[2], &Q[14]);   // Q[0]+Q[2]
    ECP8_ZZZ_copy(&T4[3], &T4[1]);
    ECP8_ZZZ_add(&T4[3], &Q[14]);   // Q[0]+Q[1]+Q[2]
    ECP8_ZZZ_copy(&T4[4], &T4[0]);
    ECP8_ZZZ_add(&T4[4], &Q[15]); // Q[0]+Q[3]
    ECP8_ZZZ_copy(&T4[5], &T4[1]);
    ECP8_ZZZ_add(&T4[5], &Q[15]);   // Q[0]+Q[1]+Q[3]
    ECP8_ZZZ_copy(&T4[6], &T4[2]);
    ECP8_ZZZ_add(&T4[6], &Q[15]);   // Q[0]+Q[2]+Q[3]
    ECP8_ZZZ_copy(&T4[7], &T4[3]);
    ECP8_ZZZ_add(&T4[7], &Q[15]);   // Q[0]+Q[1]+Q[2]+Q[3]

// Make them odd
    pb1 = 1 - BIG_XXX_parity(t[0]);
    BIG_XXX_inc(t[0], pb1);
    BIG_XXX_norm(t[0]);

    pb2 = 1 - BIG_XXX_parity(t[4]);
    BIG_XXX_inc(t[4], pb2);
    BIG_XXX_norm(t[4]);

    pb3 = 1 - BIG_XXX_parity(t[8]);
    BIG_XXX_inc(t[8], pb3);
    BIG_XXX_norm(t[8]);

    pb4 = 1 - BIG_XXX_parity(t[12]);
    BIG_XXX_inc(t[12], pb4);
    BIG_XXX_norm(t[12]);

// Number of bits
    BIG_XXX_zero(mt);
    for (i = 0; i < 16; i++)
    {
        BIG_XXX_or(mt, mt, t[i]);
    }
    nb = 1 + BIG_XXX_nbits(mt);

// Sign pivot
    s1[nb - 1] = 1;
    s2[nb - 1] = 1;
    s3[nb - 1] = 1;
    s4[nb - 1] = 1;
    for (i = 0; i < nb - 1; i++)
    {
        BIG_XXX_fshr(t[0], 1);
        s1[i] = 2 * BIG_XXX_parity(t[0]) - 1;
        BIG_XXX_fshr(t[4], 1);
        s2[i] = 2 * BIG_XXX_parity(t[4]) - 1;
        BIG_XXX_fshr(t[8], 1);
        s3[i] = 2 * BIG_XXX_parity(t[8]) - 1;
        BIG_XXX_fshr(t[12], 1);
        s4[i] = 2 * BIG_XXX_parity(t[12]) - 1;
    }


// Recoded exponents
    for (i = 0; i < nb; i++)
    {
        w1[i] = 0;
        k = 1;
        for (j = 1; j < 4; j++)
        {
            bt = s1[i] * BIG_XXX_parity(t[j]);
            BIG_XXX_fshr(t[j], 1);

            BIG_XXX_dec(t[j], (bt >> 1));
            BIG_XXX_norm(t[j]);
            w1[i] += bt * k;
            k *= 2;
        }

        w2[i] = 0;
        k = 1;
        for (j = 5; j < 8; j++)
        {
            bt = s2[i] * BIG_XXX_parity(t[j]);
            BIG_XXX_fshr(t[j], 1);

            BIG_XXX_dec(t[j], (bt >> 1));
            BIG_XXX_norm(t[j]);
            w2[i] += bt * k;
            k *= 2;
        }

        w3[i] = 0;
        k = 1;
        for (j = 9; j < 12; j++)
        {
            bt = s3[i] * BIG_XXX_parity(t[j]);
            BIG_XXX_fshr(t[j], 1);

            BIG_XXX_dec(t[j], (bt >> 1));
            BIG_XXX_norm(t[j]);
            w3[i] += bt * k;
            k *= 2;
        }

        w4[i] = 0;
        k = 1;
        for (j = 13; j < 16; j++)
        {
            bt = s4[i] * BIG_XXX_parity(t[j]);
            BIG_XXX_fshr(t[j], 1);

            BIG_XXX_dec(t[j], (bt >> 1));
            BIG_XXX_norm(t[j]);
            w4[i] += bt * k;
            k *= 2;
        }
    }

// Main loop
    ECP8_ZZZ_select(P, T1, 2 * w1[nb - 1] + 1);
    ECP8_ZZZ_select(&W, T2, 2 * w2[nb - 1] + 1);
    ECP8_ZZZ_add(P, &W);
    ECP8_ZZZ_select(&W, T3, 2 * w3[nb - 1] + 1);
    ECP8_ZZZ_add(P, &W);
    ECP8_ZZZ_select(&W, T4, 2 * w4[nb - 1] + 1);
    ECP8_ZZZ_add(P, &W);

    for (i = nb - 2; i >= 0; i--)
    {
        ECP8_ZZZ_dbl(P);
        ECP8_ZZZ_select(&W, T1, 2 * w1[i] + s1[i]);
        ECP8_ZZZ_add(P, &W);
        ECP8_ZZZ_select(&W, T2, 2 * w2[i] + s2[i]);
        ECP8_ZZZ_add(P, &W);
        ECP8_ZZZ_select(&W, T3, 2 * w3[i] + s3[i]);
        ECP8_ZZZ_add(P, &W);
        ECP8_ZZZ_select(&W, T4, 2 * w4[i] + s4[i]);
        ECP8_ZZZ_add(P, &W);
    }

// apply corrections
    ECP8_ZZZ_copy(&W, P);
    ECP8_ZZZ_sub(&W, &Q[0]);
    ECP8_ZZZ_cmove(P, &W, pb1);
    ECP8_ZZZ_copy(&W, P);
    ECP8_ZZZ_sub(&W, &Q[4]);
    ECP8_ZZZ_cmove(P, &W, pb2);

    ECP8_ZZZ_copy(&W, P);
    ECP8_ZZZ_sub(&W, &Q[8]);
    ECP8_ZZZ_cmove(P, &W, pb3);
    ECP8_ZZZ_copy(&W, P);
    ECP8_ZZZ_sub(&W, &Q[12]);
    ECP8_ZZZ_cmove(P, &W, pb4);
    ECP8_ZZZ_affine(P);
}

/* Hunt and Peck a BIG to a curve point */
/*
void ECP8_ZZZ_hap2point(ECP8_ZZZ *Q,BIG_XXX h)
{
    BIG_XXX one,hv;
    FP2_YYY X2;
    FP4_YYY X4;
    FP8_YYY X8;
    BIG_XXX_one(one);
    BIG_XXX_copy(hv,h);

    for (;;)
    {
        FP2_YYY_from_BIGs(&X2,one,hv); 
		FP4_YYY_from_FP2(&X4,&X2);
		FP8_YYY_from_FP4(&X8,&X4);
        if (ECP8_ZZZ_setx(Q,&X8,0)) break;
        BIG_XXX_inc(hv,1);
        BIG_XXX_norm(hv);
    }
}
*/
/* Constant time Map to Point in G2 */
void ECP8_ZZZ_map2point(ECP8_ZZZ *Q,FP8_YYY *H)
{
    int sgn,ne;
    FP8_YYY X1,X2,X3,W,Y,T,A,NY;
    FP_YYY Z,s;

    FP8_YYY_one(&NY);
    FP8_YYY_copy(&T,H);
    sgn=FP8_YYY_sign(&T);

    FP_YYY_from_int(&Z,RIADZG2A_YYY);
    FP8_YYY_from_FP(&A,&Z);
    ECP8_ZZZ_rhs(&A,&A);  // A=g(Z)

    FP8_YYY_sqrt(&W,&A,NULL);
    FP_YYY_rcopy(&s,SQRTm3_YYY);

    FP_YYY_mul(&Z,&Z,&s);

    FP8_YYY_sqr(&T,&T);   
    FP8_YYY_mul(&Y,&A,&T);   // tv1=u^2*g(Z)
    FP8_YYY_add(&T,&NY,&Y); FP8_YYY_norm(&T); // tv2=1+tv1
    FP8_YYY_sub(&Y,&NY,&Y); FP8_YYY_norm(&Y); // tv1=1-tv1 
    FP8_YYY_mul(&NY,&T,&Y);

    FP8_YYY_tmul(&NY,&NY,&Z);

    FP8_YYY_inv(&NY,&NY,NULL);     // tv3=inv0(tv1*tv2*Z*sqrt(-3))
    FP8_YYY_tmul(&W,&W,&Z); // tv4=Z*sqrt(-3).sqrt(g(Z))
    if (FP8_YYY_sign(&W)==1)
    {
        FP8_YYY_neg(&W,&W);
        FP8_YYY_norm(&W);
    }
    FP8_YYY_tmul(&W,&W,&Z); // tv4=Z*sqrt(-3g(Z))
    FP8_YYY_mul(&W,&W,H);
    FP8_YYY_mul(&W,&W,&Y);
    FP8_YYY_mul(&W,&W,&NY);     // tv5=u*tv1*tv3*tv4

    FP_YYY_from_int(&s,RIADZG2A_YYY);
    FP8_YYY_from_FP(&X1,&s);

    FP8_YYY_copy(&X3,&X1);
    FP8_YYY_neg(&X1,&X1); FP8_YYY_norm(&X1); FP8_YYY_div2(&X1,&X1); // -Z/2
    FP8_YYY_copy(&X2,&X1);
    FP8_YYY_sub(&X1,&X1,&W); FP8_YYY_norm(&X1);
    FP8_YYY_add(&X2,&X2,&W); FP8_YYY_norm(&X2);
    FP8_YYY_add(&A,&A,&A);
    FP8_YYY_add(&A,&A,&A); 
    FP8_YYY_norm(&A);      // 4*g(Z)
    FP8_YYY_sqr(&T,&T);
    FP8_YYY_mul(&T,&T,&NY);
    FP8_YYY_sqr(&T,&T);    // (tv2^2*tv3)^2
    FP8_YYY_mul(&A,&A,&T); // 4*g(Z)*(tv2^2*tv3)^2

    FP8_YYY_add(&X3,&X3,&A); FP8_YYY_norm(&X3);   

    ECP8_ZZZ_rhs(&W,&X2);
    FP8_YYY_cmove(&X3,&X2,FP8_YYY_qr(&W,NULL));
    ECP8_ZZZ_rhs(&W,&X1);
    FP8_YYY_cmove(&X3,&X1,FP8_YYY_qr(&W,NULL));
    ECP8_ZZZ_rhs(&W,&X3);
    FP8_YYY_sqrt(&Y,&W,NULL);
    
    ne=FP8_YYY_sign(&Y)^sgn;
    FP8_YYY_neg(&W,&Y); FP8_YYY_norm(&W);
    FP8_YYY_cmove(&Y,&W,ne);
 
    ECP8_ZZZ_set(Q,&X3,&Y);
}

/* Map octet to point */
/*
void ECP8_ZZZ_mapit(ECP8_ZZZ *Q, octet *W)
{
    BIG_XXX q, x;
    DBIG_XXX dx;
    BIG_XXX_rcopy(q, Modulus_YYY);

    BIG_XXX_dfromBytesLen(dx,W->val,W->len);
    BIG_XXX_dmod(x,dx,q);

    ECP8_ZZZ_hap2point(Q,x);   
    ECP8_ZZZ_cfp(Q);
}
*/
/* cofactor product */
void ECP8_ZZZ_cfp(ECP8_ZZZ *Q)
{
    FP2_YYY X[3];
    ECP8_ZZZ xQ, x2Q, x3Q, x4Q , x5Q, x6Q, x7Q, x8Q;
    BIG_XXX x;

    ECP8_ZZZ_frob_constants(X);

    BIG_XXX_rcopy(x, CURVE_Bnx_ZZZ);

    // Efficient hash maps to G2 on BLS48 curves - Budroni, Pintore
    // Q -> x8Q -x7Q -Q +  F(x7Q-x6Q) + F(F(x6Q-x5Q)) +F(F(F(x5Q-x4Q))) +F(F(F(F(x4Q-x3Q)))) + F(F(F(F(F(x3Q-x2Q))))) + F(F(F(F(F(F(x2Q-xQ)))))) + F(F(F(F(F(F(F(xQ-Q))))))) +F(F(F(F(F(F(F(F(2Q))))))))

    ECP8_ZZZ_copy(&xQ, Q);
    ECP8_ZZZ_mul(&xQ, x);
    ECP8_ZZZ_copy(&x2Q, &xQ);
    ECP8_ZZZ_mul(&x2Q, x);
    ECP8_ZZZ_copy(&x3Q, &x2Q);
    ECP8_ZZZ_mul(&x3Q, x);
    ECP8_ZZZ_copy(&x4Q, &x3Q);

    ECP8_ZZZ_mul(&x4Q, x);
    ECP8_ZZZ_copy(&x5Q, &x4Q);
    ECP8_ZZZ_mul(&x5Q, x);
    ECP8_ZZZ_copy(&x6Q, &x5Q);
    ECP8_ZZZ_mul(&x6Q, x);
    ECP8_ZZZ_copy(&x7Q, &x6Q);
    ECP8_ZZZ_mul(&x7Q, x);
    ECP8_ZZZ_copy(&x8Q, &x7Q);
    ECP8_ZZZ_mul(&x8Q, x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP8_ZZZ_neg(&xQ);
    ECP8_ZZZ_neg(&x3Q);
    ECP8_ZZZ_neg(&x5Q);
    ECP8_ZZZ_neg(&x7Q);
#endif

    ECP8_ZZZ_sub(&x8Q, &x7Q);
    ECP8_ZZZ_sub(&x8Q, Q);

    ECP8_ZZZ_sub(&x7Q, &x6Q);
    ECP8_ZZZ_frob(&x7Q, X, 1);

    ECP8_ZZZ_sub(&x6Q, &x5Q);
    ECP8_ZZZ_frob(&x6Q, X, 2);

    ECP8_ZZZ_sub(&x5Q, &x4Q);
    ECP8_ZZZ_frob(&x5Q, X, 3);

    ECP8_ZZZ_sub(&x4Q, &x3Q);
    ECP8_ZZZ_frob(&x4Q, X, 4);

    ECP8_ZZZ_sub(&x3Q, &x2Q);
    ECP8_ZZZ_frob(&x3Q, X, 5);

    ECP8_ZZZ_sub(&x2Q, &xQ);
    ECP8_ZZZ_frob(&x2Q, X, 6);

    ECP8_ZZZ_sub(&xQ, Q);
    ECP8_ZZZ_frob(&xQ, X, 7);

    ECP8_ZZZ_dbl(Q);
    ECP8_ZZZ_frob(Q, X, 8);


    ECP8_ZZZ_add(Q, &x8Q);
    ECP8_ZZZ_add(Q, &x7Q);
    ECP8_ZZZ_add(Q, &x6Q);
    ECP8_ZZZ_add(Q, &x5Q);

    ECP8_ZZZ_add(Q, &x4Q);
    ECP8_ZZZ_add(Q, &x3Q);
    ECP8_ZZZ_add(Q, &x2Q);
    ECP8_ZZZ_add(Q, &xQ);

    ECP8_ZZZ_affine(Q);

}

// ECP$ Get Group Generator

int ECP8_ZZZ_generator(ECP8_ZZZ *G)
{
    BIG_XXX a, b;
    FP2_YYY Aa, Bb;
    FP4_YYY A, B;
    FP8_YYY X, Y;

    BIG_XXX_rcopy(a, CURVE_Pxaaa_ZZZ);
    BIG_XXX_rcopy(b, CURVE_Pxaab_ZZZ);
    FP2_YYY_from_BIGs(&Aa, a, b);

    BIG_XXX_rcopy(a, CURVE_Pxaba_ZZZ);
    BIG_XXX_rcopy(b, CURVE_Pxabb_ZZZ);
    FP2_YYY_from_BIGs(&Bb, a, b);

    FP4_YYY_from_FP2s(&A, &Aa, &Bb);

    BIG_XXX_rcopy(a, CURVE_Pxbaa_ZZZ);
    BIG_XXX_rcopy(b, CURVE_Pxbab_ZZZ);
    FP2_YYY_from_BIGs(&Aa, a, b);

    BIG_XXX_rcopy(a, CURVE_Pxbba_ZZZ);
    BIG_XXX_rcopy(b, CURVE_Pxbbb_ZZZ);
    FP2_YYY_from_BIGs(&Bb, a, b);

    FP4_YYY_from_FP2s(&B, &Aa, &Bb);

    FP8_YYY_from_FP4s(&X, &A, &B);

    BIG_XXX_rcopy(a, CURVE_Pyaaa_ZZZ);
    BIG_XXX_rcopy(b, CURVE_Pyaab_ZZZ);
    FP2_YYY_from_BIGs(&Aa, a, b);

    BIG_XXX_rcopy(a, CURVE_Pyaba_ZZZ);
    BIG_XXX_rcopy(b, CURVE_Pyabb_ZZZ);
    FP2_YYY_from_BIGs(&Bb, a, b);

    FP4_YYY_from_FP2s(&A, &Aa, &Bb);

    BIG_XXX_rcopy(a, CURVE_Pybaa_ZZZ);
    BIG_XXX_rcopy(b, CURVE_Pybab_ZZZ);
    FP2_YYY_from_BIGs(&Aa, a, b);

    BIG_XXX_rcopy(a, CURVE_Pybba_ZZZ);
    BIG_XXX_rcopy(b, CURVE_Pybbb_ZZZ);
    FP2_YYY_from_BIGs(&Bb, a, b);

    FP4_YYY_from_FP2s(&B, &Aa, &Bb);

    FP8_YYY_from_FP4s(&Y, &A, &B);

    return ECP8_ZZZ_set(G, &X, &Y);
}
