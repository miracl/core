/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
*/

/* CORE Weierstrass elliptic curve functions over FP2 */

//#include <iostream>
#include "ecp4_ZZZ.h"

using namespace std;
using namespace XXX;
using namespace YYY;

int ZZZ::ECP4_isinf(ECP4 *P)
{
    return (FP4_iszilch(&(P->x)) & FP4_iszilch(&(P->z)));
}

/* Set P=Q */
void ZZZ::ECP4_copy(ECP4 *P, ECP4 *Q)
{
    FP4_copy(&(P->x), &(Q->x));
    FP4_copy(&(P->y), &(Q->y));
    FP4_copy(&(P->z), &(Q->z));
}

/* set P to Infinity */
void ZZZ::ECP4_inf(ECP4 *P)
{
    FP4_zero(&(P->x));
    FP4_one(&(P->y));
    FP4_zero(&(P->z));
}

/* Conditional move Q to P dependant on d */
static void ECP4_cmove(ZZZ::ECP4 *P, ZZZ::ECP4 *Q, int d)
{
    FP4_cmove(&(P->x), &(Q->x), d);
    FP4_cmove(&(P->y), &(Q->y), d);
    FP4_cmove(&(P->z), &(Q->z), d);
}

/* return 1 if b==c, no branching */
static int teq(sign32 b, sign32 c)
{
    sign32 x = b ^ c;
    x -= 1; // if x=0, x now -1
    return (int)((x >> 31) & 1);
}

/* Constant time select from pre-computed table */
static void ECP4_select(ZZZ::ECP4 *P, ZZZ::ECP4 W[], sign32 b)
{
    ZZZ::ECP4 MP;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    ECP4_cmove(P, &W[0], teq(babs, 0)); // conditional move
    ECP4_cmove(P, &W[1], teq(babs, 1));
    ECP4_cmove(P, &W[2], teq(babs, 2));
    ECP4_cmove(P, &W[3], teq(babs, 3));
    ECP4_cmove(P, &W[4], teq(babs, 4));
    ECP4_cmove(P, &W[5], teq(babs, 5));
    ECP4_cmove(P, &W[6], teq(babs, 6));
    ECP4_cmove(P, &W[7], teq(babs, 7));

    ECP4_copy(&MP, P);
    ECP4_neg(&MP);  // minus P
    ECP4_cmove(P, &MP, (int)(m & 1));
}

/* Make P affine (so z=1) */
void ZZZ::ECP4_affine(ECP4 *P)
{
    FP4 one, iz;
    if (ECP4_isinf(P)) return;

    FP4_one(&one);
    if (FP4_isunity(&(P->z)))
    {
        FP4_reduce(&(P->x));
        FP4_reduce(&(P->y));
        return;
    }

    FP4_inv(&iz, &(P->z));
    FP4_mul(&(P->x), &(P->x), &iz);
    FP4_mul(&(P->y), &(P->y), &iz);

    FP4_reduce(&(P->x));
    FP4_reduce(&(P->y));
    FP4_copy(&(P->z), &one);
}

/* return 1 if P==Q, else 0 */
/* SU= 312 */
int ZZZ::ECP4_equals(ECP4 *P, ECP4 *Q)
{
    FP4 a, b;

    FP4_mul(&a, &(P->x), &(Q->z));
    FP4_mul(&b, &(Q->x), &(P->z));
    if (!FP4_equals(&a, &b)) return 0;

    FP4_mul(&a, &(P->y), &(Q->z));
    FP4_mul(&b, &(Q->y), &(P->z));
    if (!FP4_equals(&a, &b)) return 0;
    return 1;

}

/* extract x, y from point P */
int ZZZ::ECP4_get(FP4 *x, FP4 *y, ECP4 *P)
{
    ECP4 W;
    ECP4_copy(&W, P);
    ECP4_affine(&W);
    if (ECP4_isinf(&W)) return -1;
    FP4_copy(y, &(W.y));
    FP4_copy(x, &(W.x));
    return 0;
}

/* Output point P */
void ZZZ::ECP4_output(ECP4 *P)
{
    FP4 x, y;
    if (ECP4_isinf(P))
    {
        printf("Infinity\n");
        return;
    }
    ECP4_get(&x, &y, P);
    printf("(");
    FP4_output(&x);
    printf(",");
    FP4_output(&y);
    printf(")\n");
}

/* Convert Q to octet string */
void ZZZ::ECP4_toOctet(octet *W, ECP4 *Q,bool compress)
{
    BIG b;
    FP4 qx, qy;
    FP2 pa, pb;

    W->val[0]=0x06;
    ECP4_get(&qx, &qy, Q);

    FP2_copy(&pa, &(qx.a));
    FP2_copy(&pb, &(qx.b));

    FP_redc(b, &(pa.a));
    BIG_toBytes(&(W->val[1]), b);
    FP_redc(b, &(pa.b));
    BIG_toBytes(&(W->val[MODBYTES_XXX+1]), b);
    FP_redc(b, &(pb.a));
    BIG_toBytes(&(W->val[2 * MODBYTES_XXX+1]), b);
    FP_redc(b, &(pb.b));
    BIG_toBytes(&(W->val[3 * MODBYTES_XXX+1]), b);

    if (!compress)
    {
        W->val[0] = 0x04;
        FP2_copy(&pa, &(qy.a));
        FP2_copy(&pb, &(qy.b));

        FP_redc(b, &(pa.a));
        BIG_toBytes(&(W->val[4 * MODBYTES_XXX+1]), b);
        FP_redc(b, &(pa.b));
        BIG_toBytes(&(W->val[5 * MODBYTES_XXX+1]), b);
        FP_redc(b, &(pb.a));
        BIG_toBytes(&(W->val[6 * MODBYTES_XXX+1]), b);
        FP_redc(b, &(pb.b));
        BIG_toBytes(&(W->val[7 * MODBYTES_XXX+1]), b);

        W->len = 8 * MODBYTES_XXX+1;
    } else {
        W->val[0]=0x02;
        if (FP4_sign(&qy)==1) W->val[0] = 0x03;
        W->len = 4 * MODBYTES_XXX+1;
    }
}

/* restore Q from octet string */
int ZZZ::ECP4_fromOctet(ECP4 *Q, octet *W)
{
    BIG b;
    FP4 qx, qy;
    FP2 pa, pb;
    int typ = W->val[0];

    BIG_fromBytes(b, &(W->val[1]));
    FP_nres(&(pa.a), b);
    BIG_fromBytes(b, &(W->val[MODBYTES_XXX+1]));
    FP_nres(&(pa.b), b);
    BIG_fromBytes(b, &(W->val[2 * MODBYTES_XXX+1]));
    FP_nres(&(pb.a), b);
    BIG_fromBytes(b, &(W->val[3 * MODBYTES_XXX+1]));
    FP_nres(&(pb.b), b);

    FP2_copy(&(qx.a), &pa);
    FP2_copy(&(qx.b), &pb);

    if (typ == 0x04)
    {

        BIG_fromBytes(b, &(W->val[4 * MODBYTES_XXX+1]));
        FP_nres(&(pa.a), b);
        BIG_fromBytes(b, &(W->val[5 * MODBYTES_XXX+1]));
        FP_nres(&(pa.b), b);
        BIG_fromBytes(b, &(W->val[6 * MODBYTES_XXX+1]));
        FP_nres(&(pb.a), b);
        BIG_fromBytes(b, &(W->val[7 * MODBYTES_XXX+1]));
        FP_nres(&(pb.b), b);

        FP2_copy(&(qy.a), &pa);
        FP2_copy(&(qy.b), &pb);
        if (ECP4_set(Q, &qx, &qy)) return 1;
    } else {
        if (ECP4_setx(Q, &qx, typ&1)) return 1;
    }
    return 0;
}

/* Calculate RHS of twisted curve equation x^3+B/i or x^3+Bi*/
void ZZZ::ECP4_rhs(FP4 *rhs, FP4 *x)
{
    /* calculate RHS of elliptic curve equation */
    FP4 t;
    FP2 t2;
    BIG b;
    FP4_sqr(&t, x);

    FP4_mul(rhs, &t, x);

    /* Assuming CURVE_A=0 */

    BIG_rcopy(b, CURVE_B);

    FP2_from_BIG(&t2, b);
    FP4_from_FP2(&t, &t2);

#if SEXTIC_TWIST_ZZZ == D_TYPE
    FP4_div_i(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

#if SEXTIC_TWIST_ZZZ == M_TYPE
    FP4_times_i(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

    FP4_add(rhs, &t, rhs);
    FP4_reduce(rhs);
}

/* Set P=(x,y). Return 1 if (x,y) is on the curve, else return 0*/
/* SU= 232 */
int ZZZ::ECP4_set(ECP4 *P, FP4 *x, FP4 *y)
{
    FP4 rhs, y2;

    FP4_sqr(&y2, y);
    ECP4_rhs(&rhs, x);

    if (!FP4_equals(&y2, &rhs))
    {
        ECP4_inf(P);
        return 0;
    }

    FP4_copy(&(P->x), x);
    FP4_copy(&(P->y), y);

    FP4_one(&(P->z));
    return 1;
}

/* Set P=(x,y). Return 1 if (x,.) is on the curve, else return 0 */
/* SU= 232 */
int ZZZ::ECP4_setx(ECP4 *P, FP4 *x, int s)
{
    FP4 y;
    ECP4_rhs(&y, x);

    if (!FP4_qr(&y))
    {
        ECP4_inf(P);
        return 0;
    }
    FP4_sqrt(&y, &y);
    
    FP4_copy(&(P->x), x);
    FP4_copy(&(P->y), &y);
    FP4_one(&(P->z));

    if (FP4_sign(&(P->y)) != s)
        FP4_neg(&(P->y),&(P->y));
    FP4_reduce(&(P->y));

    return 1;
}

/* Set P=-P */
/* SU= 8 */
void ZZZ::ECP4_neg(ECP4 *P)
{
    FP4_norm(&(P->y));
    FP4_neg(&(P->y), &(P->y));
    FP4_norm(&(P->y));
}

/* R+=R */
/* return -1 for Infinity, 0 for addition, 1 for doubling */
int ZZZ::ECP4_dbl(ECP4 *P)
{
    FP4 t0, t1, t2, t3, iy, x3, y3;

    FP4_copy(&iy, &(P->y));     //FP4 iy=new FP4(y);
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_times_i(&iy);           //iy.mul_ip();
#endif

    FP4_sqr(&t0, &(P->y));          //t0.sqr();
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_times_i(&t0);           //t0.mul_ip();
#endif

    FP4_mul(&t1, &iy, &(P->z)); //t1.mul(z);
    FP4_sqr(&t2, &(P->z));              //t2.sqr();

    FP4_add(&(P->z), &t0, &t0); //z.add(t0);
    FP4_norm(&(P->z));              //z.norm();
    FP4_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP4_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP4_norm(&(P->z));          //z.norm();

    FP4_imul(&t2, &t2, 3 * CURVE_B_I); //t2.imul(3*ROM.CURVE_B_I);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_times_i(&t2);
#endif

    FP4_mul(&x3, &t2, &(P->z)); //x3.mul(z);

    FP4_add(&y3, &t0, &t2);     //y3.add(t2);
    FP4_norm(&y3);              //y3.norm();
    FP4_mul(&(P->z), &(P->z), &t1); //z.mul(t1);

    FP4_add(&t1, &t2, &t2);     //t1.add(t2);
    FP4_add(&t2, &t2, &t1);     //t2.add(t1);
    FP4_norm(&t2);              //t2.norm();
    FP4_sub(&t0, &t0, &t2);     //t0.sub(t2);
    FP4_norm(&t0);              //t0.norm();                           //y^2-9bz^2
    FP4_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP4_add(&(P->y), &y3, &x3);     //y3.add(x3);                          //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2

    FP4_mul(&t1, &(P->x), &iy);     //t1.mul(iy);                       //

    FP4_norm(&t0);          //x.norm();
    FP4_mul(&(P->x), &t0, &t1); //x.mul(t1);
    FP4_add(&(P->x), &(P->x), &(P->x)); //x.add(x);       //(y^2-9bz^2)xy2

    FP4_norm(&(P->x));          //x.norm();

    FP4_norm(&(P->y));          //y.norm();

    return 1;
}

/* Set P+=Q */

int ZZZ::ECP4_add(ECP4 *P, ECP4 *Q)
{
    FP4 t0, t1, t2, t3, t4, x3, y3, z3;
    int b3 = 3 * CURVE_B_I;

    FP4_mul(&t0, &(P->x), &(Q->x)); //t0.mul(Q.x);         // x.Q.x
    FP4_mul(&t1, &(P->y), &(Q->y)); //t1.mul(Q.y);       // y.Q.y

    FP4_mul(&t2, &(P->z), &(Q->z)); //t2.mul(Q.z);
    FP4_add(&t3, &(P->x), &(P->y)); //t3.add(y);
    FP4_norm(&t3);              //t3.norm();          //t3=X1+Y1
    FP4_add(&t4, &(Q->x), &(Q->y)); //t4.add(Q.y);
    FP4_norm(&t4);              //t4.norm();            //t4=X2+Y2
    FP4_mul(&t3, &t3, &t4);     //t3.mul(t4);                       //t3=(X1+Y1)(X2+Y2)
    FP4_add(&t4, &t0, &t1);     //t4.add(t1);       //t4=X1.X2+Y1.Y2

    FP4_sub(&t3, &t3, &t4);     //t3.sub(t4);
    FP4_norm(&t3);              //t3.norm();
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_times_i(&t3);           //t3.mul_ip();
#endif

    FP4_add(&t4, &(P->y), &(P->z)); //t4.add(z);
    FP4_norm(&t4);              //t4.norm();            //t4=Y1+Z1

    FP4_add(&x3, &(Q->y), &(Q->z)); //x3.add(Q.z);
    FP4_norm(&x3);              //x3.norm();            //x3=Y2+Z2

    FP4_mul(&t4, &t4, &x3);     //t4.mul(x3);                       //t4=(Y1+Z1)(Y2+Z2)

    FP4_add(&x3, &t1, &t2);     //x3.add(t2);                       //X3=Y1.Y2+Z1.Z2

    FP4_sub(&t4, &t4, &x3);     //t4.sub(x3);
    FP4_norm(&t4);              //t4.norm();
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_times_i(&t4);           //t4.mul_ip();
#endif

    FP4_add(&x3, &(P->x), &(P->z)); //x3.add(z);
    FP4_norm(&x3);              //x3.norm();    // x3=X1+Z1

    FP4_add(&y3, &(Q->x), &(Q->z)); //y3.add(Q.z);
    FP4_norm(&y3);              //y3.norm();                // y3=X2+Z2
    FP4_mul(&x3, &x3, &y3);     //x3.mul(y3);                           // x3=(X1+Z1)(X2+Z2)

    FP4_add(&y3, &t0, &t2);     //y3.add(t2);                           // y3=X1.X2+Z1+Z2
    FP4_sub(&y3, &x3, &y3);     //y3.rsub(x3);
    FP4_norm(&y3);              //y3.norm();                // y3=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1
#if SEXTIC_TWIST_ZZZ==D_TYPE
    FP4_times_i(&t0);           //t0.mul_ip();
    FP4_times_i(&t1);           //t1.mul_ip();
#endif

    FP4_add(&x3, &t0, &t0);     //x3.add(t0);
    FP4_add(&t0, &t0, &x3);     //t0.add(x3);
    FP4_norm(&t0);              //t0.norm();
    FP4_imul(&t2, &t2, b3);     //t2.imul(b);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_times_i(&t2);
#endif

    FP4_add(&z3, &t1, &t2);     //z3.add(t2);
    FP4_norm(&z3);              //z3.norm();
    FP4_sub(&t1, &t1, &t2);     //t1.sub(t2);
    FP4_norm(&t1);              //t1.norm();
    FP4_imul(&y3, &y3, b3);     //y3.imul(b);
#if SEXTIC_TWIST_ZZZ==M_TYPE
    FP4_times_i(&y3);
#endif

    FP4_mul(&x3, &y3, &t4);     //x3.mul(t4);

    FP4_mul(&t2, &t3, &t1);     //t2.mul(t1);
    FP4_sub(&(P->x), &t2, &x3);     //x3.rsub(t2);
    FP4_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP4_mul(&t1, &t1, &z3);     //t1.mul(z3);
    FP4_add(&(P->y), &y3, &t1);     //y3.add(t1);
    FP4_mul(&t0, &t0, &t3);     //t0.mul(t3);
    FP4_mul(&z3, &z3, &t4);     //z3.mul(t4);
    FP4_add(&(P->z), &z3, &t0);     //z3.add(t0);

    FP4_norm(&(P->x));          //x.norm();
    FP4_norm(&(P->y));          //y.norm();
    FP4_norm(&(P->z));          //z.norm();

    return 0;
}

/* Set P-=Q */
/* SU= 16 */
void ZZZ::ECP4_sub(ECP4 *P, ECP4 *Q)
{
    ECP4 NQ;
    ECP4_copy(&NQ, Q);
    ECP4_neg(&NQ);
    ECP4_add(P, &NQ);
}


void ZZZ::ECP4_reduce(ECP4 *P)
{
    FP4_reduce(&(P->x));
    FP4_reduce(&(P->y));
    FP4_reduce(&(P->z));
}

/* P*=e */
/* SU= 280 */
void ZZZ::ECP4_mul(ECP4 *P, BIG e)
{
    /* fixed size windows */
    int i, nb, s, ns;
    BIG mt, t;
    ECP4 Q, W[8], C;
    sign8 w[1 + (NLEN_XXX * BASEBITS_XXX + 3) / 4];

    if (ECP4_isinf(P)) return;

    /* precompute table */

    ECP4_copy(&Q, P);
    ECP4_dbl(&Q);
    ECP4_copy(&W[0], P);

    for (i = 1; i < 8; i++)
    {
        ECP4_copy(&W[i], &W[i - 1]);
        ECP4_add(&W[i], &Q);
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
    ECP4_cmove(&Q, P, ns);
    ECP4_copy(&C, &Q);

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

    ECP4_copy(P, &W[(w[nb] - 1) / 2]);
    for (i = nb - 1; i >= 0; i--)
    {
        ECP4_select(&Q, W, w[i]);
        ECP4_dbl(P);
        ECP4_dbl(P);
        ECP4_dbl(P);
        ECP4_dbl(P);
        ECP4_add(P, &Q);
    }
    ECP4_sub(P, &C); /* apply correction */
    ECP4_affine(P);
}

// calculate frobenius constants
void ZZZ::ECP4_frob_constants(FP2 F[3])
{
    FP fx, fy;
    FP2 X;

    FP_rcopy(&fx, Fra);
    FP_rcopy(&fy, Frb);
    FP2_from_FPs(&X, &fx, &fy);

    FP2_sqr(&F[0], &X);     // FF=F^2=(1+i)^(p-7)/6
    FP2_copy(&F[2], &F[0]);
    FP2_mul_ip(&F[2]);      // W=(1+i)^6/6.(1+i)^(p-7)/6 = (1+i)^(p-1)/6
    FP2_norm(&F[2]);
    FP2_sqr(&F[1], &F[2]);
    FP2_mul(&F[2], &F[2], &F[1]); // W=(1+i)^(p-1)/2

    FP2_copy(&F[1], &X);

#if SEXTIC_TWIST_ZZZ == M_TYPE
    FP2_mul_ip(&F[1]);      // (1+i)^12/12.(1+i)^(p-7)/12 = (1+i)^(p+5)/12
    FP2_inv(&F[1], &F[1]);      // (1+i)^-(p+5)/12
    FP2_sqr(&F[0], &F[1]);      // (1+i)^-(p+5)/6
#endif

    FP2_mul_ip(&F[0]);      // FF=(1+i)^(p-7)/6.(1+i) = (1+i)^(p-1)/6                   // (1+i)^6/6.(1+i)^-(p+5)/6 = (1+i)^-(p-1)/6
    FP2_norm(&F[0]);
    FP2_mul(&F[1], &F[1], &F[0]); // FFF = (1+i)^(p-7)/12 . (1+i)^(p-1)/6 = (1+i)^(p-3)/4    // (1+i)^-(p+5)/12 . (1+i)^-(p-1)/6 = (1+i)^-(p+1)/4

}

/* Calculates q^n.P using Frobenius constants */
void ZZZ::ECP4_frob(ECP4 *P, FP2 F[3], int n)
{
    int i;
    FP4 X, Y, Z;

    FP4_copy(&X, &(P->x));
    FP4_copy(&Y, &(P->y));
    FP4_copy(&Z, &(P->z));

    for (i = 0; i < n; i++)
    {
        FP4_frob(&X, &F[2]);    // X^p
        FP4_pmul(&X, &X, &F[0]); // X^p.(1+i)^(p-1)/6                                    // X^p.(1+i)^-(p-1)/6

        FP4_frob(&Y, &F[2]);    // Y^p
        FP4_pmul(&Y, &Y, &F[1]);
        FP4_times_i(&Y);        // Y.p.(1+i)^(p-3)/4.(1+i)^(2/4) = Y^p.(1+i)^(p-1)/4    // (1+i)^-(p+1)/4 .(1+i)^2/4 = Y^p.(1+i)^-(p-1)/4

        FP4_frob(&Z, &F[2]);
    }

    FP4_copy(&(P->x), &X);
    FP4_copy(&(P->y), &Y);
    FP4_copy(&(P->z), &Z);


    //ECP4_set(P,&X,&Y);
}

/* Side channel attack secure */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf

void ZZZ::ECP4_mul8(ECP4 *P, ECP4 Q[8], BIG u[8])
{
    int i, j, k, nb, pb1, pb2, bt;
    ECP4 T1[8], T2[8], W;
    BIG mt, t[8];
    sign8 w1[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s1[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 w2[NLEN_XXX * BASEBITS_XXX + 1];
    sign8 s2[NLEN_XXX * BASEBITS_XXX + 1];
//  FP2 X[3];
    /*
        ECP4_frob_constants(X);
    */
    for (i = 0; i < 8; i++)
    {
        BIG_copy(t[i], u[i]);
    }
// Precomputed tables
    ECP4_copy(&T1[0], &Q[0]); // Q[0]
    ECP4_copy(&T1[1], &T1[0]);
    ECP4_add(&T1[1], &Q[1]); // Q[0]+Q[1]
    ECP4_copy(&T1[2], &T1[0]);
    ECP4_add(&T1[2], &Q[2]); // Q[0]+Q[2]
    ECP4_copy(&T1[3], &T1[1]);
    ECP4_add(&T1[3], &Q[2]); // Q[0]+Q[1]+Q[2]
    ECP4_copy(&T1[4], &T1[0]);
    ECP4_add(&T1[4], &Q[3]); // Q[0]+Q[3]
    ECP4_copy(&T1[5], &T1[1]);
    ECP4_add(&T1[5], &Q[3]); // Q[0]+Q[1]+Q[3]
    ECP4_copy(&T1[6], &T1[2]);
    ECP4_add(&T1[6], &Q[3]); // Q[0]+Q[2]+Q[3]
    ECP4_copy(&T1[7], &T1[3]);
    ECP4_add(&T1[7], &Q[3]); // Q[0]+Q[1]+Q[2]+Q[3]

    ECP4_copy(&T2[0], &Q[4]); // Q[0]
    ECP4_copy(&T2[1], &T2[0]);
    ECP4_add(&T2[1], &Q[5]); // Q[0]+Q[1]
    ECP4_copy(&T2[2], &T2[0]);
    ECP4_add(&T2[2], &Q[6]); // Q[0]+Q[2]
    ECP4_copy(&T2[3], &T2[1]);
    ECP4_add(&T2[3], &Q[6]); // Q[0]+Q[1]+Q[2]
    ECP4_copy(&T2[4], &T2[0]);
    ECP4_add(&T2[4], &Q[7]); // Q[0]+Q[3]
    ECP4_copy(&T2[5], &T2[1]);
    ECP4_add(&T2[5], &Q[7]); // Q[0]+Q[1]+Q[3]
    ECP4_copy(&T2[6], &T2[2]);
    ECP4_add(&T2[6], &Q[7]); // Q[0]+Q[2]+Q[3]
    ECP4_copy(&T2[7], &T2[3]);
    ECP4_add(&T2[7], &Q[7]); // Q[0]+Q[1]+Q[2]+Q[3]

//  Use Frobenius
    /*
        for (i=0;i<8;i++)
        {
            ECP4_copy(&T2[i],&T1[i]);
            ECP4_frob(&T2[i],X,4);
        }
    */
// Make them odd
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
    ECP4_select(P, T1, 2 * w1[nb - 1] + 1);
    ECP4_select(&W, T2, 2 * w2[nb - 1] + 1);
    ECP4_add(P, &W);
    for (i = nb - 2; i >= 0; i--)
    {
        ECP4_dbl(P);
        ECP4_select(&W, T1, 2 * w1[i] + s1[i]);
        ECP4_add(P, &W);
        ECP4_select(&W, T2, 2 * w2[i] + s2[i]);
        ECP4_add(P, &W);
    }

// apply corrections
    ECP4_copy(&W, P);
    ECP4_sub(&W, &Q[0]);
    ECP4_cmove(P, &W, pb1);
    ECP4_copy(&W, P);
    ECP4_sub(&W, &Q[4]);
    ECP4_cmove(P, &W, pb2);

    ECP4_affine(P);
}

/* Hunt and Peck a BIG to G2 curve point */
void ZZZ::ECP4_hap2point(ECP4 *Q,BIG h)
{
    BIG one,hv;
    FP2 X2;
    FP4 X4;
    BIG_one(one);
    BIG_copy(hv,h);

    for (;;)
    {
        FP2_from_BIGs(&X2,one,hv);  /*******/
		FP4_from_FP2(&X4,&X2);
        if (ECP4_setx(Q,&X4,0)) break;
        BIG_inc(hv,1);
        BIG_norm(hv);
    }
}

/* Constant time Map BIG to Point in G2 */
void ZZZ::ECP4_map2point(ECP4 *Q,FP4 *H)
{
    int sgn,ne;
    FP4 X1,X2,X3,W,B,Y,S,T;
    FP b,j,s,one;

    FP_rcopy(&b,CURVE_B);
    FP4_from_FP(&B, &b);
#if SEXTIC_TWIST_ZZZ == D_TYPE
    FP4_div_i(&B);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

#if SEXTIC_TWIST_ZZZ == M_TYPE
    FP4_times_i(&B);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

    FP4_one(&W);
    FP_one(&one);
    FP4_copy(&T,H);
    sgn=FP4_sign(&T);
        
    FP_from_int(&s,-3);
    FP_sqrt(&s,&s,NULL);         // s=sqrt(-3)
    FP_sub(&j,&s,&one);     FP_norm(&j);
    FP_div2(&j,&j);         // j=(s-1)/2

    FP4_qmul(&S,&T,&s);       // s=s.t
    FP4_sqr(&Y,&T);          // t^2
    FP4_add(&Y,&Y,&W);     // t^2+1
    FP4_add(&B,&B,&Y);      // t^2+B+1
    FP4_norm(&B);
    FP4_inv(&B,&B);
    FP4_mul(&B,&B,&S);      // w=s.t/(1+B+t*2)

    FP4_mul(&X1,&B,&T);       
    FP4_from_FP(&Y,&j);     
    FP4_sub(&X2,&X1,&Y);    FP4_norm(&X2);// X2=t.w-j 
    FP4_neg(&X1,&X2);       FP4_norm(&X1);// X1=j-t.w
    FP4_sub(&X2,&X2,&W);    FP4_norm(&X2);

    FP4_sqr(&B,&B);
    FP4_inv(&B,&B);
    FP4_add(&X3,&B,&W);     FP4_norm(&X3);
    
    ECP4_rhs(&W,&X2);
    FP4_cmove(&X1,&X2,FP4_qr(&W));
    ECP4_rhs(&W,&X3);
    FP4_cmove(&X1,&X3,FP4_qr(&W));
    ECP4_rhs(&W,&X1);
    FP4_sqrt(&Y,&W);
    
    ne=FP4_sign(&Y)^sgn;
    FP4_neg(&W,&Y); FP4_norm(&W);
    FP4_cmove(&Y,&W,ne);
 
    ECP4_set(Q,&X1,&Y);
}

/* Map octet to point on G2 */
void ZZZ::ECP4_mapit(ECP4 *Q, octet *W)
{
    BIG q, x;
    DBIG dx;
    BIG_rcopy(q, Modulus);

    BIG_dfromBytesLen(dx,W->val,W->len);
    BIG_dmod(x,dx,q);

    ECP4_hap2point(Q,x);   
    ECP4_cfp(Q);
}

/* cofactor product */
void ZZZ::ECP4_cfp(ECP4 *Q)
{
    FP2 X[3];
    ECP4 xQ, x2Q, x3Q, x4Q;
    BIG x;

    ECP4_frob_constants(X);

    BIG_rcopy(x, CURVE_Bnx);

    // Efficient hash maps to G2 on BLS24 curves - Budroni, Pintore
    // Q -> x4Q -x3Q -Q + F(x3Q-x2Q) + F(F(x2Q-xQ)) + F(F(F(xQ-Q))) +F(F(F(F(2Q))))

    ECP4_copy(&xQ, Q);
    ECP4_mul(&xQ, x);
    ECP4_copy(&x2Q, &xQ);
    ECP4_mul(&x2Q, x);
    ECP4_copy(&x3Q, &x2Q);
    ECP4_mul(&x3Q, x);
    ECP4_copy(&x4Q, &x3Q);
    ECP4_mul(&x4Q, x);

#if SIGN_OF_X_ZZZ==NEGATIVEX
    ECP4_neg(&xQ);
    ECP4_neg(&x3Q);
#endif

    ECP4_sub(&x4Q, &x3Q);
    ECP4_sub(&x4Q, Q);

    ECP4_sub(&x3Q, &x2Q);
    ECP4_frob(&x3Q, X, 1);

    ECP4_sub(&x2Q, &xQ);
    ECP4_frob(&x2Q, X, 2);

    ECP4_sub(&xQ, Q);
    ECP4_frob(&xQ, X, 3);

    ECP4_dbl(Q);
    ECP4_frob(Q, X, 4);

    ECP4_add(Q, &x4Q);
    ECP4_add(Q, &x3Q);
    ECP4_add(Q, &x2Q);
    ECP4_add(Q, &xQ);

    ECP4_affine(Q);

}

// ECP$ Get Group Generator

int ZZZ::ECP4_generator(ECP4 *G)
{
    BIG a, b;
    FP2 Aa, Bb;
    FP4 X, Y;

    BIG_rcopy(a, CURVE_Pxaa);
    BIG_rcopy(b, CURVE_Pxab);
    FP2_from_BIGs(&Aa, a, b);

    BIG_rcopy(a, CURVE_Pxba);
    BIG_rcopy(b, CURVE_Pxbb);
    FP2_from_BIGs(&Bb, a, b);

    FP4_from_FP2s(&X, &Aa, &Bb);

    BIG_rcopy(a, CURVE_Pyaa);
    BIG_rcopy(b, CURVE_Pyab);
    FP2_from_BIGs(&Aa, a, b);

    BIG_rcopy(a, CURVE_Pyba);
    BIG_rcopy(b, CURVE_Pybb);
    FP2_from_BIGs(&Bb, a, b);

    FP4_from_FP2s(&Y, &Aa, &Bb);

    return ECP4_set(G, &X, &Y);
}

