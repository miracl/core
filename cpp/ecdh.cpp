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

/* ECDH/ECIES/ECDSA API Functions */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "ecdh_ZZZ.h"

using namespace XXX;
using namespace YYY;


/* return 1 if S is in ranger 0 < S < order , else return 0 */
int ZZZ::ECP_IN_RANGE(octet* S)
{
    BIG r,s;
    BIG_rcopy(r, CURVE_Order);
    BIG_fromBytes(s,S->val);
    if (BIG_iszilch(s)) return 0;
    if (BIG_comp(s,r)>=0) return 0;
    return 1;
}
 
/* Calculate a public/private EC GF(p) key pair. W=S.G mod EC(p),
 * where S is the secret key and W is the public key
 * and G is fixed generator.
 * If RNG is NULL then the private key is provided externally in S
 * otherwise it is generated randomly internally */
int ZZZ::ECP_KEY_PAIR_GENERATE(csprng *RNG, octet* S, octet *W)
{
    BIG r, gx, gy, s;
    ECP G;
    int res = 0;

    ECP_generator(&G);
    BIG_rcopy(r, CURVE_Order);

    if (RNG != NULL)
    {
        BIG_randtrunc(s, r, 2 * CURVE_SECURITY_ZZZ, RNG);
    }
    else
    {
        BIG_fromBytes(s, S->val);
        BIG_mod(s, r);
    }

    S->len = EGS_ZZZ;
    BIG_toBytes(S->val, s);

    ECP_mul(&G, s);
    ECP_toOctet(W, &G, false);  // To use point compression on public keys, change to true

    return res;
}

/* Validate public key */
int ZZZ::ECP_PUBLIC_KEY_VALIDATE(octet *W)
{
    BIG q, r, wx, k;
    ECP WP;
    int valid, nb;
    int res = 0;

    BIG_rcopy(q, Modulus);
    BIG_rcopy(r, CURVE_Order);

    valid = ECP_fromOctet(&WP, W);
    if (!valid) res = ECDH_INVALID_PUBLIC_KEY;

    if (res == 0)
    {

        nb = BIG_nbits(q);
        BIG_one(k);
        BIG_shl(k, (nb + 4) / 2);
        BIG_add(k, q, k);
        BIG_sdiv(k, r); /* get co-factor */
        while (BIG_parity(k) == 0)
        {
            ECP_dbl(&WP);
            BIG_fshr(k, 1);
        }

        if (!BIG_isunity(k)) ECP_mul(&WP, k);
        if (ECP_isinf(&WP)) res = ECDH_INVALID_PUBLIC_KEY;
    }

    return res;
}

/* IEEE-1363 Diffie-Hellman online calculation Z=S.WD */
// type = 0 is just x coordinate output
// type = 1 for standard compressed output
// type = 2 for standard uncompress output 04|x|y
int ZZZ::ECP_SVDP_DH(octet *S, octet *WD, octet *Z,int type)
{
    BIG r, s, wx;
    int valid;
    ECP W;
    int res = 0;

    BIG_fromBytes(s, S->val);

    valid = ECP_fromOctet(&W, WD);

    if (!valid) res = ECDH_ERROR;
    if (res == 0)
    {
        BIG_rcopy(r, CURVE_Order);
        BIG_mod(s, r);

        ECP_mul(&W, s);
        if (ECP_isinf(&W)) res = ECDH_ERROR;
        else
        {
#if CURVETYPE_ZZZ!=MONTGOMERY
            if (type>0)
            {
                if (type==1) ECP_toOctet(Z,&W,true);
                else ECP_toOctet(Z,&W,false);
                return res;
            }
            else
                ECP_get(wx, wx, &W);
#else
            ECP_get(wx, &W);
#endif
        }
        Z->len = MODBYTES_XXX;
        BIG_toBytes(Z->val, wx);
    }
    return res;
}

#if CURVETYPE_ZZZ!=MONTGOMERY

/* IEEE ECDSA Signature, C and D are signature on F using private key S */
int ZZZ::ECP_SP_DSA(int hlen, csprng *RNG, octet *K, octet *S, octet *F, octet *C, octet *D)
{
    char h[128];
    octet H = {0, sizeof(h), h};

    BIG r, s, f, c, d, u, vx, w;
    ECP G, V;

    SPhash(MC_SHA2, hlen, &H, F);

    ECP_generator(&G);

    BIG_rcopy(r, CURVE_Order);

    BIG_fromBytes(s, S->val);

    int blen = H.len;
    if (H.len > MODBYTES_XXX) blen = MODBYTES_XXX;
    BIG_fromBytesLen(f, H.val, blen);


    if (RNG != NULL)
    {
        do
        {
            BIG_randomnum(u, r, RNG);
            BIG_randomnum(w, r, RNG); /* side channel masking */

#ifdef AES_S
            BIG_mod2m(u, 2 * AES_S);
#endif
            ECP_copy(&V, &G);
            ECP_mul(&V, u);

            ECP_get(vx, vx, &V);

            BIG_copy(c, vx);
            BIG_mod(c, r);
            if (BIG_iszilch(c)) continue;

            BIG_modmul(u, u, w, r);

            BIG_invmodp(u, u, r);
            BIG_modmul(d, s, c, r);

            BIG_add(d, f, d);

            BIG_modmul(d, d, w, r);

            BIG_modmul(d, u, d, r);

        }
        while (BIG_iszilch(d));
    }
    else
    {
        BIG_fromBytes(u, K->val);
        BIG_mod(u, r);

#ifdef AES_S
        BIG_mod2m(u, 2 * AES_S);
#endif
        ECP_copy(&V, &G);
        ECP_mul(&V, u);

        ECP_get(vx, vx, &V);

        BIG_copy(c, vx);
        BIG_mod(c, r);
        if (BIG_iszilch(c)) return ECDH_ERROR;

        BIG_invmodp(u, u, r);
        BIG_modmul(d, s, c, r);

        BIG_add(d, f, d);

        BIG_modmul(d, u, d, r);
        if (BIG_iszilch(d)) return ECDH_ERROR;

    }

    C->len = D->len = EGS_ZZZ;

    BIG_toBytes(C->val, c);
    BIG_toBytes(D->val, d);

    return 0;
}

/* IEEE1363 ECDSA Signature Verification. Signature C and D on F is verified using public key W */
int ZZZ::ECP_VP_DSA(int hlen, octet *W, octet *F, octet *C, octet *D)
{
    char h[128];
    octet H = {0, sizeof(h), h};

    BIG r, wx, wy, f, c, d, h2;
    int res = 0;
    ECP G, WP;
    int valid;

    SPhash(MC_SHA2, hlen, &H, F);

    ECP_generator(&G);
    BIG_rcopy(r, CURVE_Order);

    OCT_shl(C, C->len - MODBYTES_XXX);
    OCT_shl(D, D->len - MODBYTES_XXX);

    BIG_fromBytes(c, C->val);
    BIG_fromBytes(d, D->val);

    int blen = H.len;
    if (blen > MODBYTES_XXX) blen = MODBYTES_XXX;

    BIG_fromBytesLen(f, H.val, blen);

    if (BIG_iszilch(c) || BIG_comp(c, r) >= 0 || BIG_iszilch(d) || BIG_comp(d, r) >= 0)
        res = ECDH_ERROR;

    if (res == 0)
    {
        BIG_invmodp(d, d, r);

        BIG_modmul(f, f, d, r);
        BIG_modmul(h2, c, d, r);

        valid = ECP_fromOctet(&WP, W);
        if (!valid) res = ECDH_ERROR;
        else
        {
/*
            ECP VT[2];
            BIG e[2];
            ECP_copy(&VT[0],&WP);
            ECP_copy(&VT[1],&G);
            BIG_copy(e[0],h2);
            BIG_copy(e[1],f);

            ECP_muln(&WP,2,VT,e);
*/

            ECP_mul2(&WP, &G, h2, f);
            if (ECP_isinf(&WP)) res = ECDH_ERROR;
            else
            {
                ECP_get(d, d, &WP);
                BIG_mod(d, r);
                if (BIG_comp(d, c) != 0) res = ECDH_ERROR;
            }
        }
    }

    return res;
}

/* IEEE1363 ECIES encryption. Encryption of plaintext M uses public key W and produces ciphertext V,C,T */
void ZZZ::ECP_ECIES_ENCRYPT(int hlen, octet *P1, octet *P2, csprng *RNG, octet *W, octet *M, int tlen, octet *V, octet *C, octet *T)
{

    int i, len;
    char z[EFS_ZZZ], vz[3 * EFS_ZZZ + 1], k[2 * AESKEY_ZZZ], k1[AESKEY_ZZZ], k2[AESKEY_ZZZ], l2[8], u[EFS_ZZZ];
    octet Z = {0, sizeof(z), z};
    octet VZ = {0, sizeof(vz), vz};
    octet K = {0, sizeof(k), k};
    octet K1 = {0, sizeof(k1), k1};
    octet K2 = {0, sizeof(k2), k2};
    octet L2 = {0, sizeof(l2), l2};
    octet U = {0, sizeof(u), u};

    if (ECP_KEY_PAIR_GENERATE(RNG, &U, V) != 0) return;
    if (ECP_SVDP_DH(&U, W, &Z,0) != 0) return;

    OCT_copy(&VZ, V);
    OCT_joctet(&VZ, &Z);

    KDF2(MC_SHA2, hlen, &K, 2 * AESKEY_ZZZ, &VZ, P1);

    K1.len = K2.len = AESKEY_ZZZ;
    for (i = 0; i < AESKEY_ZZZ; i++)
    {
        K1.val[i] = K.val[i];
        K2.val[i] = K.val[AESKEY_ZZZ + i];
    }

    AES_CBC_IV0_ENCRYPT(&K1, M, C);

    OCT_jint(&L2, P2->len, 8);

    len = C->len;
    OCT_joctet(C, P2);
    OCT_joctet(C, &L2);
    HMAC(MC_SHA2, hlen, T, tlen, C, &K2);
    C->len = len;
}

/* IEEE1363 ECIES decryption. Decryption of ciphertext V,C,T using private key U outputs plaintext M */
int ZZZ::ECP_ECIES_DECRYPT(int hlen, octet *P1, octet *P2, octet *V, octet *C, octet *T, octet *U, octet *M)
{

    int i, len;
    char z[EFS_ZZZ], vz[3 * EFS_ZZZ + 1], k[2 * AESKEY_ZZZ], k1[AESKEY_ZZZ], k2[AESKEY_ZZZ], l2[8], tag[32];
    octet Z = {0, sizeof(z), z};
    octet VZ = {0, sizeof(vz), vz};
    octet K = {0, sizeof(k), k};
    octet K1 = {0, sizeof(k1), k1};
    octet K2 = {0, sizeof(k2), k2};
    octet L2 = {0, sizeof(l2), l2};
    octet TAG = {0, sizeof(tag), tag};

    if (ECP_SVDP_DH(U, V, &Z,0) != 0) return 0;

    OCT_copy(&VZ, V);
    OCT_joctet(&VZ, &Z);

    KDF2(MC_SHA2, hlen, &K, 2 * AESKEY_ZZZ, &VZ, P1);

    K1.len = K2.len = AESKEY_ZZZ;
    for (i = 0; i < AESKEY_ZZZ; i++)
    {
        K1.val[i] = K.val[i];
        K2.val[i] = K.val[AESKEY_ZZZ + i];
    }

    if (!AES_CBC_IV0_DECRYPT(&K1, C, M)) return 0;

    OCT_jint(&L2, P2->len, 8);

    len = C->len;
    OCT_joctet(C, P2);
    OCT_joctet(C, &L2);
    HMAC(MC_SHA2, hlen, &TAG, T->len, C, &K2);
    C->len = len;

    if (!OCT_ncomp(T, &TAG, T->len)) return 0;

    return 1;

}

#endif
