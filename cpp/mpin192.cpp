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

/* MPIN Functions */

/* Version 3.0 - supports Time Permits */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpin192_ZZZ.h"

using namespace XXX;
using namespace YYY;

#define ROUNDUP(a,b) ((a)-1)/(b)+1

namespace ZZZ {
static void mpin_hash(int, FP8 *, ECP *, octet *);
static void map(ECP *, BIG , int );
static int unmap(BIG , int *, ECP *);
}

/* Special mpin hashing */
static void ZZZ::mpin_hash(int sha, FP8 *f, ECP *P, octet *w)
{
    int i;
    BIG x, y;
    char h[64];
    hash256 sha256;
    hash512 sha512;
    char t[10 * MODBYTES_XXX]; // to hold 10 BIGs
    octet T={10 * MODBYTES_XXX,sizeof(t),t};
    int hlen = sha;


    FP_redc(x, &(f->a.a.a));
    BIG_toBytes(&T.val[0], x);
    FP_redc(x, &(f->a.a.b));
    BIG_toBytes(&T.val[MODBYTES_XXX], x);
    FP_redc(x, &(f->a.b.a));
    BIG_toBytes(&T.val[2 * MODBYTES_XXX], x);
    FP_redc(x, &(f->a.b.b));
    BIG_toBytes(&T.val[3 * MODBYTES_XXX], x);

    FP_redc(x, &(f->b.a.a));
    BIG_toBytes(&T.val[4 * MODBYTES_XXX], x);
    FP_redc(x, &(f->b.a.b));
    BIG_toBytes(&T.val[5 * MODBYTES_XXX], x);
    FP_redc(x, &(f->b.b.a));
    BIG_toBytes(&T.val[6 * MODBYTES_XXX], x);
    FP_redc(x, &(f->b.b.b));
    BIG_toBytes(&T.val[7 * MODBYTES_XXX], x);

    ECP_get(x, y, P);
    BIG_toBytes(&T.val[8 * MODBYTES_XXX], x);
    BIG_toBytes(&T.val[9 * MODBYTES_XXX], y);

    GPhash(MC_SHA2,sha,w,AESKEY_ZZZ,0,&T,-1,NULL);
/*
    OCT_empty(w);
    switch (sha)
    {
    case SHA256:
        HASH256_init(&sha256);
        for (i = 0; i < 10 * MODBYTES_XXX; i++) HASH256_process(&sha256, t[i]);
        HASH256_hash(&sha256, h);
        break;
    case SHA384:
        HASH384_init(&sha512);
        for (i = 0; i < 10 * MODBYTES_XXX; i++) HASH384_process(&sha512, t[i]);
        HASH384_hash(&sha512, h);
        break;
    case SHA512:
        HASH512_init(&sha512);
        for (i = 0; i < 10 * MODBYTES_XXX; i++) HASH512_process(&sha512, t[i]);
        HASH512_hash(&sha512, h);
        break;
    }

    OCT_jbytes(w, h, AESKEY_ZZZ);
    for (i = 0; i < hlen; i++) h[i] = 0;
*/
}

/* these next two functions help to implement elligator squared - http://eprint.iacr.org/2014/043 */
/* maps a random u to a point on the curve */
static void ZZZ::map(ECP *P, BIG u, int cb)
{
    BIG x, q;

    BIG_rcopy(q, Modulus);
    BIG_copy(x, u);
    BIG_mod(x, q);

    while (!ECP_setx(P, x, cb))
    {
        BIG_inc(x, 1); BIG_norm(x);
    }
}

/* returns u derived from P. Random value in range 1 to return value should then be added to u */
static int ZZZ::unmap(BIG u, int *cb, ECP *P)
{
    int s, r = 0;
    BIG x;

    s = ECP_get(x, x, P);
    BIG_copy(u, x);
    do
    {
        BIG_dec(u, 1); BIG_norm(u);
        r++;
    }
    while (!ECP_setx(P, u, s));
    ECP_setx(P, x, s);

    *cb = s;

    return r;
}

/* these next two functions implement elligator squared - http://eprint.iacr.org/2014/043 */
/* Elliptic curve point E in format (0x04,x,y} is converted to form {0x0-,u,v} */
/* Note that u and v are indistinguisible from random strings */
int ZZZ::MPIN_ENCODING(csprng *RNG, octet *E)
{
    int rn, m, su, sv, res = 0;

    BIG q, u, v;
    ECP P, W;

    if (!ECP_fromOctet(&P, E)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        BIG_rcopy(q, Modulus);

        BIG_randomnum(u, q, RNG);

        su = RAND_byte(RNG);
        if (su < 0) su = -su;
        su %= 2;
        map(&W, u, su);
        ECP_sub(&P, &W);

        rn = unmap(v, &sv, &P);
        m = RAND_byte(RNG);
        if (m < 0) m = -m;
        m %= rn;
        BIG_inc(v, m + 1);
        E->val[0] = su + 2 * sv;
        BIG_toBytes(&(E->val[1]), u);
        BIG_toBytes(&(E->val[PFS_ZZZ + 1]), v);
    }
    return res;
}

int ZZZ::MPIN_DECODING(octet *D)
{
    int su, sv;
    BIG u, v;
    ECP P, W;
    int res = 0;

    if ((D->val[0] & 0x04) != 0) res = MPIN_INVALID_POINT;
    if (res == 0)
    {

        BIG_fromBytes(u, &(D->val[1]));
        BIG_fromBytes(v, &(D->val[PFS_ZZZ + 1]));

        su = D->val[0] & 1;
        sv = (D->val[0] >> 1) & 1;
        map(&W, u, su);
        map(&P, v, sv);
        ECP_add(&P, &W);
        ECP_toOctet(D, &P, false);
    }

    return res;
}

/* R=R1+R2 in group G1 */
int ZZZ::MPIN_RECOMBINE_G1(octet *R1, octet *R2, octet *R)
{
    ECP P, T;
    int res = 0;
    if (res == 0)
    {
        if (!ECP_fromOctet(&P, R1)) res = MPIN_INVALID_POINT;
        if (!ECP_fromOctet(&T, R2)) res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        ECP_add(&P, &T);
        ECP_toOctet(R, &P, false);
    }
    return res;
}

/* W=W1+W2 in group G2 */
int ZZZ::MPIN_RECOMBINE_G2(octet *W1, octet *W2, octet *W)
{
    ECP4 Q, T;
    int res = 0;
    if (!ECP4_fromOctet(&Q, W1)) res = MPIN_INVALID_POINT;
    if (!ECP4_fromOctet(&T, W2)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        ECP4_add(&Q, &T);
        ECP4_toOctet(W, &Q, false);
    }
    return res;
}

/* create random secret S */
int ZZZ::MPIN_RANDOM_GENERATE(csprng *RNG, octet* S)
{
    BIG r, s;

    BIG_rcopy(r, CURVE_Order);
    BIG_randtrunc(s, r, 2 * CURVE_SECURITY_ZZZ, RNG);
    BIG_toBytes(S->val, s);
    S->len = MODBYTES_XXX;
    return 0;
}

/* Extract PIN from TOKEN for identity CID */
int ZZZ::MPIN_EXTRACT_PIN(int sha, octet *CID, int pin, octet *TOKEN)
{
    pin %= MAXPIN;
    return MPIN_EXTRACT_FACTOR(sha, CID, pin, PBLEN, TOKEN);
}

/* Extract a factor < 32 bits for identity CID */
int ZZZ::MPIN_EXTRACT_FACTOR(int sha, octet *CID, int factor, int facbits, octet *TOKEN)
{
    ECP P, R;
    int res = 0;
    char h[MODBYTES_XXX];
    octet H = {0, sizeof(h), h};

    if (!ECP_fromOctet(&P, TOKEN))  res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        GPhash(MC_SHA2,sha,&H,H.max,0,NULL,-1,CID);
        //mhashit(sha, -1, CID, &H);
        ECP_mapit(&R, &H);

        ECP_pinmul(&R, factor, facbits);
        ECP_sub(&P, &R);

        ECP_toOctet(TOKEN, &P, false);
    }
    return res;
}

/* Extract a factor < 32 bits for identity CID */
int ZZZ::MPIN_RESTORE_FACTOR(int sha, octet *CID, int factor, int facbits, octet *TOKEN)
{
    ECP P, R;
    int res = 0;
    char h[MODBYTES_XXX];
    octet H = {0, sizeof(h), h};

    if (!ECP_fromOctet(&P, TOKEN))  res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        GPhash(MC_SHA2,sha,&H,H.max,0,NULL,-1,CID);
        //mhashit(sha, -1, CID, &H);
        ECP_mapit(&R, &H);

        ECP_pinmul(&R, factor, facbits);
        ECP_add(&P, &R);

        ECP_toOctet(TOKEN, &P, false);
    }
    return res;
}

/* Implement step 2 on client side of MPin protocol - SEC=-(x+y)*SEC */
int ZZZ::MPIN_CLIENT_2(octet *X, octet *Y, octet *SEC)
{
    BIG px, py, r;
    ECP P;
    int res = 0;
    BIG_rcopy(r, CURVE_Order);
    if (!ECP_fromOctet(&P, SEC)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        BIG_fromBytes(px, X->val);
        BIG_fromBytes(py, Y->val);
        BIG_add(px, px, py);
        BIG_mod(px, r);
        PAIR_G1mul(&P, px);
        ECP_neg(&P);
        ECP_toOctet(SEC, &P, false);
    }
    return res;
}

/*
 W=x*H(G);
 if RNG == NULL then X is passed in
 if RNG != NULL the X is passed out
 if type=0 W=x*G where G is point on the curve, else W=x*M(G), where M(G) is mapping of octet G to point on the curve
*/

int ZZZ::MPIN_GET_G1_MULTIPLE(csprng *RNG, int type, octet *X, octet *G, octet *W)
{
    ECP P;
    BIG r, x;
    int res = 0;
    if (RNG != NULL)
    {
        BIG_rcopy(r, CURVE_Order);
        BIG_randtrunc(x, r, 2 * CURVE_SECURITY_ZZZ, RNG);
        X->len = MODBYTES_XXX;
        BIG_toBytes(X->val, x);
    }
    else
        BIG_fromBytes(x, X->val);

    if (type == 0)
    {
        if (!ECP_fromOctet(&P, G)) res = MPIN_INVALID_POINT;
    }
    else
    {
        ECP_mapit(&P, G);
    }

    if (res == 0)
    {
        PAIR_G1mul(&P, x);
        ECP_toOctet(W, &P, false);
    }
    return res;
}

/*
 if RNG == NULL then X is passed in
 if RNG != NULL the X is passed out
 W=x*G where G is point on the curve
 if type==1 W=(x^-1)G
*/

int ZZZ::MPIN_GET_G2_MULTIPLE(csprng *RNG, int type, octet *X, octet *G, octet *W)
{
    ECP4 P;
    BIG r, x;
    int res = 0;
    BIG_rcopy(r, CURVE_Order);
    if (RNG != NULL)
    {
        BIG_randtrunc(x, r, 2 * CURVE_SECURITY_ZZZ, RNG);
        X->len = MODBYTES_XXX;
        BIG_toBytes(X->val, x);
    }
    else
    {
        BIG_fromBytes(x, X->val);
        if (type == 1) BIG_invmodp(x, x, r);
    }

    if (!ECP4_fromOctet(&P, G)) res = MPIN_INVALID_POINT;

    if (res == 0)
    {
        PAIR_G2mul(&P, x);
        ECP4_toOctet(W, &P, false);
    }
    return res;
}



/* Client secret CST=s*H(CID) where CID is client ID and s is master secret */
/* CID is hashed externally */
int ZZZ::MPIN_GET_CLIENT_SECRET(octet *S, octet *CID, octet *CST)
{
    return MPIN_GET_G1_MULTIPLE(NULL, 1, S, CID, CST);
}

/* Implement step 1 on client side of MPin protocol */
int ZZZ::MPIN_CLIENT_1(int sha, int date, octet *CLIENT_ID, csprng *RNG, octet *X, int pin, octet *TOKEN, octet *SEC, octet *xID, octet *xCID, octet *PERMIT)
{
    BIG r, x;
    ECP P, T, W;
    int res = 0;
    char h[MODBYTES_XXX];
    octet H = {0, sizeof(h), h};

    BIG_rcopy(r, CURVE_Order);
    if (RNG != NULL)
    {
        BIG_randtrunc(x, r, 2 * CURVE_SECURITY_ZZZ, RNG);
        X->len = MODBYTES_XXX;
        BIG_toBytes(X->val, x);
    }
    else
        BIG_fromBytes(x, X->val);

    GPhash(MC_SHA2,sha,&H,H.max,0,NULL,-1,CLIENT_ID);
    //mhashit(sha, -1, CLIENT_ID, &H);

    ECP_mapit(&P, &H);

    if (!ECP_fromOctet(&T, TOKEN)) res = MPIN_INVALID_POINT;

    if (res == 0)
    {
        pin %= MAXPIN;

        ECP_copy(&W, &P);               // W=H(ID)
        ECP_pinmul(&W, pin, PBLEN);         // W=alpha.H(ID)
        ECP_add(&T, &W);                // T=Token+alpha.H(ID) = s.H(ID)

        if (date)
        {
            if (PERMIT != NULL)
            {
                if (!ECP_fromOctet(&W, PERMIT)) res = MPIN_INVALID_POINT;
                ECP_add(&T, &W);                // SEC=s.H(ID)+s.H(T|ID)
            }
            GPhash(MC_SHA2,sha,&H,H.max,0,NULL,date,&H);
            //mhashit(sha, date, &H, &H);

            ECP_mapit(&W, &H);

            if (xID != NULL)
            {
                PAIR_G1mul(&P, x);              // P=x.H(ID)
                ECP_toOctet(xID, &P, false); // xID
                PAIR_G1mul(&W, x);              // W=x.H(T|ID)
                ECP_add(&P, &W);
            }
            else
            {
                ECP_add(&P, &W); //ECP_affine(&P);
                PAIR_G1mul(&P, x);
            }
            if (xCID != NULL) ECP_toOctet(xCID, &P, false); // U
        }
        else
        {
            if (xID != NULL)
            {
                PAIR_G1mul(&P, x);              // P=x.H(ID)
                ECP_toOctet(xID, &P, false); // xID
            }
        }
    }

    if (res == 0)
    {
        ECP_toOctet(SEC, &T, false); // V
    }
    return res;
}

/* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
int ZZZ::MPIN_GET_SERVER_SECRET(octet *S, octet *SST)
{
    BIG r, s;
    ECP4 Q;
    int res = 0;

    BIG_rcopy(r, CURVE_Order);

    ECP4_generator(&Q);

    if (res == 0)
    {
        BIG_fromBytes(s, S->val);
        PAIR_G2mul(&Q, s);
        ECP4_toOctet(SST, &Q, false);
    }

    return res;
}


/* Time Permit CTT=s*H(date|H(CID)) where s is master secret */
int ZZZ::MPIN_GET_CLIENT_PERMIT(int sha, int date, octet *S, octet *CID, octet *CTT)
{
    BIG s;
    ECP P;
    char h[MODBYTES_XXX];
    octet H = {0, sizeof(h), h};

    GPhash(MC_SHA2,sha,&H,H.max,0,NULL,date,CID);
    //mhashit(sha, date, CID, &H);

    ECP_mapit(&P, &H);

    BIG_fromBytes(s, S->val);

    PAIR_G1mul(&P, s);
    ECP_toOctet(CTT, &P, false);
    return 0;
}

// if date=0 only use HID, set HCID=NULL
// if date and PE, use HID and HCID

/* Outputs H(CID) and H(CID)+H(T|H(CID)) for time permits. If no time permits set HTID=NULL */
void ZZZ::MPIN_SERVER_1(int sha, int date, octet *CID, octet *HID, octet *HTID)
{
    char h[MODBYTES_XXX];
    octet H = {0, sizeof(h), h};
    ECP P, R;
    BIG x;

#ifdef USE_ANONYMOUS
    ECP_mapit(&P, CID);
#else
    GPhash(MC_SHA2,sha,&H,H.max,0,NULL,-1,CID);
    //mhashit(sha, -1, CID, &H);
    ECP_mapit(&P, &H);
#endif

    ECP_toOctet(HID, &P, false); // new

    if (date)
    {
#ifdef USE_ANONYMOUS
        GPhash(MC_SHA2,sha,&H,H.max,0,NULL,date,CID);
        //mhashit(sha, date, CID, &H);
#else
        GPhash(MC_SHA2,sha,&H,H.max,0,NULL,date,&H);
        //mhashit(sha, date, &H, &H);
#endif
        ECP_mapit(&R, &H);
        ECP_add(&P, &R);
        ECP_toOctet(HTID, &P, false);
    }
}

/* Implement M-Pin on server side */
int ZZZ::MPIN_SERVER_2(int date, octet *HID, octet *HTID, octet *Y, octet *SST, octet *xID, octet *xCID, octet *mSEC, octet *E, octet *F, octet *Pa)
{
    BIG y;
    FP24 g;
    ECP4 Q, sQ;
    ECP P, R;
    int res = 0;

    ECP4_generator(&Q);

    // key-escrow less scheme: use Pa instead of Q in pairing computation
    // Q left for backward compatiblity
    if (Pa != NULL)
    {
        if (!ECP4_fromOctet(&Q, Pa)) res = MPIN_INVALID_POINT;
    }


    if (res == 0)
    {
        if (!ECP4_fromOctet(&sQ, SST)) res = MPIN_INVALID_POINT;
    }

    if (res == 0)
    {
        if (date)
        {
            if (!ECP_fromOctet(&R, xCID))  res = MPIN_INVALID_POINT;
        }
        else
        {
            if (!ECP_fromOctet(&R, xID))  res = MPIN_INVALID_POINT;
        }
    }
    if (res == 0)
    {
        BIG_fromBytes(y, Y->val);
        if (date)
        {
            if (!ECP_fromOctet(&P, HTID))  res = MPIN_INVALID_POINT;
        }
        else
        {
            if (!ECP_fromOctet(&P, HID))  res = MPIN_INVALID_POINT;
        }
    }
    if (res == 0)
    {
        PAIR_G1mul(&P, y); // y(A+AT)
        ECP_add(&P, &R); // x(A+AT)+y(A+T)
        if (!ECP_fromOctet(&R, mSEC))  res = MPIN_INVALID_POINT; // V
    }
    if (res == 0)
    {

        PAIR_double_ate(&g, &Q, &R, &sQ, &P);
        PAIR_fexp(&g);

        if (!FP24_isunity(&g))
        {
            if (HID != NULL && xID != NULL && E != NULL && F != NULL)
            {
                /* xID is set to NULL if there is no way to calculate PIN error */
                FP24_toOctet(E, &g);

                /* Note error is in the PIN, not in the time permit! Hence the need to exclude Time Permit from this check */

                if (date)
                {
                    if (!ECP_fromOctet(&P, HID)) res = MPIN_INVALID_POINT;
                    if (!ECP_fromOctet(&R, xID)) res = MPIN_INVALID_POINT; // U

                    if (res == 0)
                    {
                        PAIR_G1mul(&P, y); // yA
                        ECP_add(&P, &R); // yA+xA
                    }
                }
                if (res == 0)
                {
                    PAIR_ate(&g, &Q, &P);
                    PAIR_fexp(&g);
                    FP24_toOctet(F, &g);
                }
            }
            res = MPIN_BAD_PIN;
        }
    }

    return res;
}

#if MAXPIN==10000
#define MR_TS 10  /* 2^10/10 approx = sqrt(MAXPIN) */
#define TRAP 200  /* 2*sqrt(MAXPIN) */
#endif

#if MAXPIN==1000000
#define MR_TS 14
#define TRAP 2000
#endif

/* Pollards kangaroos used to return PIN error */
int ZZZ::MPIN_KANGAROO(octet *E, octet *F)
{
    int i, j, m, s, dn, dm, steps;
    int distance[MR_TS];
    FP24 ge, gf, t, table[MR_TS];
    int res = 0;

    FP24_fromOctet(&ge, E);
    FP24_fromOctet(&gf, F);

    FP24_copy(&t, &gf);

    for (s = 1, m = 0; m < MR_TS; m++)
    {
        distance[m] = s;
        FP24_copy(&table[m], &t);
        s *= 2;
        FP24_usqr(&t, &t);
        FP24_reduce(&t);
    }

    FP24_one(&t);

    for (dn = 0, j = 0; j < TRAP; j++)
    {
        i = t.a.a.a.a.g[0] % MR_TS;
        FP24_mul(&t, &table[i]);
        FP24_reduce(&t);
        dn += distance[i];
    }

    FP24_conj(&gf, &t);
    steps = 0;
    dm = 0;
    while (dm - dn < MAXPIN)
    {
        steps++;
        if (steps > 4 * TRAP) break;

        i = ge.a.a.a.a.g[0] % MR_TS;

        FP24_mul(&ge, &table[i]);
        FP24_reduce(&ge);
        dm += distance[i];
        if (FP24_equals(&ge, &t))
        {
            res = dm - dn;
            break;
        }
        if (FP24_equals(&ge, &gf))
        {
            res = dn - dm;
            break;
        }
    }
    if (steps > 4 * TRAP || dm - dn >= MAXPIN)
    {
        res = 0;  /* Trap Failed  - probable invalid token */
    }

    return res;
}

/* Functions to support M-Pin Full */

int ZZZ::MPIN_PRECOMPUTE(octet *TOKEN, octet *CID, octet *CP, octet *G1, octet *G2)
{
    ECP P, T;
    ECP4 Q;
    FP24 g;
    BIG x;
    int res = 0;

    if (!ECP_fromOctet(&T, TOKEN)) res = MPIN_INVALID_POINT;

    if (res == 0)
    {
        ECP_mapit(&P, CID);

        if (CP != NULL)
        {
            if (!ECP4_fromOctet(&Q, CP)) res = MPIN_INVALID_POINT;
        }
        else
        {
            ECP4_generator(&Q);
        }
    }
    if (res == 0)
    {
        PAIR_ate(&g, &Q, &T);
        PAIR_fexp(&g);

        FP24_toOctet(G1, &g);
        if (G2 != NULL)
        {
            PAIR_ate(&g, &Q, &P);
            PAIR_fexp(&g);
            FP24_toOctet(G2, &g);
        }
    }
    return res;
}

/* calculate common key on client side */
/* wCID = w.(A+AT) */
int ZZZ::MPIN_CLIENT_KEY(int sha, octet *G1, octet *G2, int pin, octet *R, octet *X, octet *H, octet *wCID, octet *CK)
{
    FP24 g1, g2;
    FP8 c;

    ECP W;
    int res = 0;
    BIG r, z, x, h;

    FP24_fromOctet(&g1, G1);
    FP24_fromOctet(&g2, G2);
    BIG_fromBytes(z, R->val);
    BIG_fromBytes(x, X->val);
    BIG_fromBytes(h, H->val);

    if (!ECP_fromOctet(&W, wCID)) res = MPIN_INVALID_POINT;

    if (res == 0)
    {
        BIG_rcopy(r, CURVE_Order);
        BIG_add(z, z, h);  // new
        BIG_mod(z, r);

        FP24_pinpow(&g2, pin, PBLEN);
        FP24_mul(&g1, &g2);

        PAIR_G1mul(&W, x);

        PAIR_GTpow(&g1,z);
        FP24_trace(&c,&g1);

        //FP24_compow(&c, &g1, z, r);
        mpin_hash(sha, &c, &W, CK);

    }
    return res;
}

/* calculate common key on server side */
/* Z=r.A - no time permits involved */

int ZZZ::MPIN_SERVER_KEY(int sha, octet *Z, octet *SST, octet *W, octet *H, octet *HID, octet *xID, octet *xCID, octet *SK)
{
    int res = 0;
    FP24 g;
    FP8 c;
    ECP R, U, A;
    ECP4 sQ;
    BIG w, h;

    if (!ECP4_fromOctet(&sQ, SST)) res = MPIN_INVALID_POINT;
    if (!ECP_fromOctet(&R, Z)) res = MPIN_INVALID_POINT;


    if (!ECP_fromOctet(&A, HID)) res = MPIN_INVALID_POINT;

    // new
    if (xCID != NULL)
    {
        if (!ECP_fromOctet(&U, xCID)) res = MPIN_INVALID_POINT;
    }
    else
    {
        if (!ECP_fromOctet(&U, xID)) res = MPIN_INVALID_POINT;
    }
    BIG_fromBytes(w, W->val);
    BIG_fromBytes(h, H->val);


    PAIR_ate(&g, &sQ, &A);
    PAIR_fexp(&g);

    if (res == 0)
    {
        PAIR_G1mul(&A, h);
        ECP_add(&R, &A); // new
        PAIR_ate(&g, &sQ, &R);
        PAIR_fexp(&g);
        PAIR_G1mul(&U, w);
        FP24_trace(&c, &g);
        mpin_hash(sha, &c, &U, SK);
    }
    return res;
}

/* Generate Y = H(TimeValue, xCID/xID) */
void ZZZ::MPIN_GET_Y(int sha, int TimeValue, octet *xCID, octet *Y)
{
    BIG q, y;
    char h[MODBYTES_XXX];
    octet H = {0, sizeof(h), h};

    GPhash(MC_SHA2,sha,&H,H.max,0,NULL,TimeValue,xCID);
    //mhashit(sha, TimeValue, xCID, &H);
    BIG_fromBytes(y, H.val);
    BIG_rcopy(q, CURVE_Order);
    BIG_mod(y, q);
    BIG_toBytes(Y->val, y);
    Y->len = PGS_ZZZ;
}

/* One pass MPIN Client */
int ZZZ::MPIN_CLIENT(int sha, int date, octet *ID, csprng *RNG, octet *X, int pin, octet *TOKEN, octet *V, octet *U, octet *UT, octet *TP, octet *MESSAGE, int TimeValue, octet *Y)
{
    int rtn = 0;
    char m[2 * PFS_ZZZ + 1];
    octet M = {0, sizeof(m), m};

    octet *pID;
    if (date == 0)
        pID = U;
    else
        pID = UT;

    rtn = MPIN_CLIENT_1(sha, date, ID, RNG, X, pin, TOKEN, V, U, UT, TP);
    if (rtn != 0)
        return rtn;

    OCT_joctet(&M, pID);
    if (MESSAGE != NULL)
    {
        OCT_joctet(&M, MESSAGE);
    }

    MPIN_GET_Y(sha, TimeValue, &M, Y);

    rtn = MPIN_CLIENT_2(X, Y, V);
    if (rtn != 0)
        return rtn;

    return 0;
}

/* One pass MPIN Server */
int ZZZ::MPIN_SERVER(int sha, int date, octet *HID, octet *HTID, octet *Y, octet *sQ, octet *U, octet *UT, octet *V, octet *E, octet *F, octet *ID, octet *MESSAGE, int TimeValue, octet *Pa)
{
    int rtn = 0;
    char m[2 * PFS_ZZZ + 1];
    octet M = {0, sizeof(m), m};

    octet *pU;
    if (date == 0)
        pU = U;
    else
        pU = UT;

    MPIN_SERVER_1(sha, date, ID, HID, HTID);

    OCT_joctet(&M, pU);
    if (MESSAGE != NULL)
    {
        OCT_joctet(&M, MESSAGE);
    }

    MPIN_GET_Y(sha, TimeValue, &M, Y);

    rtn = MPIN_SERVER_2(date, HID, HTID, Y, sQ, U, UT, V, E, F, Pa);
    if (rtn != 0)
        return rtn;

    return 0;
}

int ZZZ::MPIN_GET_DVS_KEYPAIR(csprng *R, octet *Z, octet *Pa)
{
    BIG z, r;
    ECP4 Q;
    int res = 0;

    BIG_rcopy(r, CURVE_Order);

    if (R != NULL)
    {
        BIG_randtrunc(z, r, 2 * CURVE_SECURITY_ZZZ, R);
        Z->len = MODBYTES_XXX;
        BIG_toBytes(Z->val, z);
    }
    else
        BIG_fromBytes(z, Z->val);

    BIG_invmodp(z, z, r);

    ECP4_generator(&Q);

    if (res == 0)
    {
        PAIR_G2mul(&Q, z);
        ECP4_toOctet(Pa, &Q, false);
    }

    return res;
}
