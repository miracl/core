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

/* RSA Functions - see main program below */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "rsa_WWW.h"

using namespace XXX;

#define ROUNDUP(a,b) ((a)-1)/(b)+1

/* Mask Generation Function */

static void MGF1(int sha, octet *z, int olen, octet *mask)
{
    char h[64];
    octet H = {0, sizeof(h), h};
    int hlen = sha;
    int counter, cthreshold;

    OCT_empty(mask);

    cthreshold = ROUNDUP(olen, hlen);
    for (counter = 0; counter < cthreshold; counter++)
    {
        GPhash(MC_SHA2,sha,&H,0,z,counter,NULL);
        //hashit(sha, z, counter, &H);
        if (mask->len + hlen > olen) OCT_jbytes(mask, H.val, olen % hlen);
        else                     OCT_joctet(mask, &H);
    }
    OCT_clear(&H);
}

/* SHAXXX identifier strings */
const unsigned char SHA256ID[] = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20};
const unsigned char SHA384ID[] = {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30};
const unsigned char SHA512ID[] = {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40};

/* PKCS 1.5 padding of a message to be signed */

int WWW::PKCS15(int sha, octet *m, octet *w)
{
    int olen = w->max;
    int hlen = sha;
    int idlen = 19;
    char h[64];
    octet H = {0, sizeof(h), h};

    if (olen < idlen + hlen + 10) return 0;
    GPhash(MC_SHA2,sha,&H,0,m,-1,NULL);
    //hashit(sha, m, -1, &H);

    OCT_empty(w);
    OCT_jbyte(w, 0x00, 1);
    OCT_jbyte(w, 0x01, 1);
    OCT_jbyte(w, 0xff, olen - idlen - hlen - 3);
    OCT_jbyte(w, 0x00, 1);

    if (hlen == 32) OCT_jbytes(w, (char *)SHA256ID, idlen);
    if (hlen == 48) OCT_jbytes(w, (char *)SHA384ID, idlen);
    if (hlen == 64) OCT_jbytes(w, (char *)SHA512ID, idlen);

    OCT_joctet(w, &H);

    return 1;
}

/* OAEP Message Encoding for Encryption */

int WWW::OAEP_ENCODE(int sha, octet *m, csprng *RNG, octet *p, octet *f)
{
    int slen, olen = f->max - 1;
    int mlen = m->len;
    int hlen, seedlen;
    char dbmask[MAX_RSA_BYTES], seed[64];
    octet DBMASK = {0, sizeof(dbmask), dbmask};
    octet SEED = {0, sizeof(seed), seed};

    hlen = seedlen = sha;
    if (mlen > olen - hlen - seedlen - 1) return 0;
    if (m == f) return 0; /* must be distinct octets */

    GPhash(MC_SHA2,sha,f,0,p,-1,NULL);
    //hashit(sha, p, -1, f);

    slen = olen - mlen - hlen - seedlen - 1;

    OCT_jbyte(f, 0, slen);
    OCT_jbyte(f, 0x1, 1);
    OCT_joctet(f, m);

    OCT_rand(&SEED, RNG, seedlen);

    MGF1(sha, &SEED, olen - seedlen, &DBMASK);

    OCT_xor(&DBMASK, f);
    MGF1(sha, &DBMASK, seedlen, f);

    OCT_xor(f, &SEED);

    OCT_joctet(f, &DBMASK);

    OCT_pad(f, f->max);
    OCT_clear(&SEED);
    OCT_clear(&DBMASK);

    return 1;
}

/* OAEP Message Decoding for Decryption */

int WWW::OAEP_DECODE(int sha, octet *p, octet *f)
{
    int comp, x, t;
    int i, k, olen = f->max - 1;
    int hlen, seedlen;
    char dbmask[MAX_RSA_BYTES], seed[64], chash[64];
    octet DBMASK = {0, sizeof(dbmask), dbmask};
    octet SEED = {0, sizeof(seed), seed};
    octet CHASH = {0, sizeof(chash), chash};

    seedlen = hlen = sha;
    if (olen < seedlen + hlen + 1) return 0;
    if (!OCT_pad(f, olen + 1)) return 0;

    GPhash(MC_SHA2,sha,&CHASH,0,p,-1,NULL);
    //hashit(sha, p, -1, &CHASH);

    x = f->val[0];
    for (i = seedlen; i < olen; i++)
        DBMASK.val[i - seedlen] = f->val[i + 1];
    DBMASK.len = olen - seedlen;

    MGF1(sha, &DBMASK, seedlen, &SEED);
    for (i = 0; i < seedlen; i++) SEED.val[i] ^= f->val[i + 1];
    MGF1(sha, &SEED, olen - seedlen, f);
    OCT_xor(&DBMASK, f);

    comp = OCT_ncomp(&CHASH, &DBMASK, hlen);

    OCT_shl(&DBMASK, hlen);

    OCT_clear(&SEED);
    OCT_clear(&CHASH);

    for (k = 0;; k++)
    {
        if (k >= DBMASK.len)
        {
            OCT_clear(&DBMASK);
            return 0;
        }
        if (DBMASK.val[k] != 0) break;
    }

    t = DBMASK.val[k];
    if (!comp || x != 0 || t != 0x01)
    {
        OCT_clear(&DBMASK);
        return 0;
    }

    OCT_shl(&DBMASK, k + 1);
    OCT_copy(f, &DBMASK);
    OCT_clear(&DBMASK);

    return 1;
}

/* generate an RSA key pair */
void WWW::RSA_KEY_PAIR(csprng *RNG, sign32 e, rsa_private_key *PRIV, rsa_public_key *PUB, octet *P, octet* Q)
{
    /* IEEE1363 A16.11/A16.12 more or less */
    BIG t[HFLEN_WWW], p1[HFLEN_WWW], q1[HFLEN_WWW];

    if (RNG != NULL)
    {

        for (;;)
        {

            FF_random(PRIV->p, RNG, HFLEN_WWW);
            while (FF_lastbits(PRIV->p, 2) != 3) FF_inc(PRIV->p, 1, HFLEN_WWW);
            while (!FF_prime(PRIV->p, RNG, HFLEN_WWW))
                FF_inc(PRIV->p, 4, HFLEN_WWW);

            FF_copy(p1, PRIV->p, HFLEN_WWW);
            FF_dec(p1, 1, HFLEN_WWW);

            if (FF_cfactor(p1, e, HFLEN_WWW)) continue;
            break;
        }

        for (;;)
        {
            FF_random(PRIV->q, RNG, HFLEN_WWW);
            while (FF_lastbits(PRIV->q, 2) != 3) FF_inc(PRIV->q, 1, HFLEN_WWW);
            while (!FF_prime(PRIV->q, RNG, HFLEN_WWW))
                FF_inc(PRIV->q, 4, HFLEN_WWW);

            FF_copy(q1, PRIV->q, HFLEN_WWW);
            FF_dec(q1, 1, HFLEN_WWW);
            if (FF_cfactor(q1, e, HFLEN_WWW)) continue;

            break;
        }

    }
    else
    {
        FF_fromOctet(PRIV->p, P, HFLEN_WWW);
        FF_fromOctet(PRIV->q, Q, HFLEN_WWW);

        FF_copy(p1, PRIV->p, HFLEN_WWW);
        FF_dec(p1, 1, HFLEN_WWW);

        FF_copy(q1, PRIV->q, HFLEN_WWW);
        FF_dec(q1, 1, HFLEN_WWW);
    }

    FF_mul(PUB->n, PRIV->p, PRIV->q, HFLEN_WWW);
    PUB->e = e;

    FF_copy(t, p1, HFLEN_WWW);
    FF_shr(t, HFLEN_WWW);
    FF_init(PRIV->dp, e, HFLEN_WWW);
    FF_invmodp(PRIV->dp, PRIV->dp, t, HFLEN_WWW);
    if (FF_parity(PRIV->dp) == 0) FF_add(PRIV->dp, PRIV->dp, t, HFLEN_WWW);
    FF_norm(PRIV->dp, HFLEN_WWW);

    FF_copy(t, q1, HFLEN_WWW);
    FF_shr(t, HFLEN_WWW);
    FF_init(PRIV->dq, e, HFLEN_WWW);
    FF_invmodp(PRIV->dq, PRIV->dq, t, HFLEN_WWW);
    if (FF_parity(PRIV->dq) == 0) FF_add(PRIV->dq, PRIV->dq, t, HFLEN_WWW);
    FF_norm(PRIV->dq, HFLEN_WWW);

    FF_invmodp(PRIV->c, PRIV->p, PRIV->q, HFLEN_WWW);

    return;
}

/* destroy the Private Key structure */
void WWW::RSA_PRIVATE_KEY_KILL(rsa_private_key *PRIV)
{
    FF_zero(PRIV->p, HFLEN_WWW);
    FF_zero(PRIV->q, HFLEN_WWW);
    FF_zero(PRIV->dp, HFLEN_WWW);
    FF_zero(PRIV->dq, HFLEN_WWW);
    FF_zero(PRIV->c, HFLEN_WWW);
}

void WWW::RSA_fromOctet(BIG x[], octet *w)
{
    FF_fromOctet(x, w, FFLEN_WWW);
}

/* RSA encryption with the public key */
void WWW::RSA_ENCRYPT(rsa_public_key *PUB, octet *F, octet *G)
{
    BIG f[FFLEN_WWW];
    FF_fromOctet(f, F, FFLEN_WWW);

    FF_power(f, f, PUB->e, PUB->n, FFLEN_WWW);

    FF_toOctet(G, f, FFLEN_WWW);
}

/* RSA decryption with the private key */
void WWW::RSA_DECRYPT(rsa_private_key *PRIV, octet *G, octet *F)
{
    BIG g[FFLEN_WWW], t[FFLEN_WWW], jp[HFLEN_WWW], jq[HFLEN_WWW];

    FF_fromOctet(g, G, FFLEN_WWW);

    FF_dmod(jp, g, PRIV->p, HFLEN_WWW);
    FF_dmod(jq, g, PRIV->q, HFLEN_WWW);

    FF_skpow(jp, jp, PRIV->dp, PRIV->p, HFLEN_WWW);
    FF_skpow(jq, jq, PRIV->dq, PRIV->q, HFLEN_WWW);


    FF_zero(g, FFLEN_WWW);
    FF_copy(g, jp, HFLEN_WWW);
    FF_mod(jp, PRIV->q, HFLEN_WWW);
    if (FF_comp(jp, jq, HFLEN_WWW) > 0)
        FF_add(jq, jq, PRIV->q, HFLEN_WWW);
    FF_sub(jq, jq, jp, HFLEN_WWW);
    FF_norm(jq, HFLEN_WWW);

    FF_mul(t, PRIV->c, jq, HFLEN_WWW);
    FF_dmod(jq, t, PRIV->q, HFLEN_WWW);

    FF_mul(t, jq, PRIV->p, HFLEN_WWW);
    FF_add(g, t, g, FFLEN_WWW);
    FF_norm(g, FFLEN_WWW);

    FF_toOctet(F, g, FFLEN_WWW);

    return;
}

