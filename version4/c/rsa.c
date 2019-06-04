/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/* RSA Functions - see main program below */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "rsa_WWW.h"
#include "rsa_support.h"

/* generate an RSA key pair */
void RSA_WWW_KEY_PAIR(csprng *RNG,sign32 e,rsa_private_key_WWW *PRIV,rsa_public_key_WWW *PUB,octet *P, octet* Q)
{
    /* IEEE1363 A16.11/A16.12 more or less */
    BIG_XXX t[HFLEN_WWW],p1[HFLEN_WWW],q1[HFLEN_WWW];

    if (RNG!=NULL)
    {

        for (;;)
        {

            FF_WWW_random(PRIV->p,RNG,HFLEN_WWW);
            while (FF_WWW_lastbits(PRIV->p,2)!=3) FF_WWW_inc(PRIV->p,1,HFLEN_WWW);
            while (!FF_WWW_prime(PRIV->p,RNG,HFLEN_WWW))
                FF_WWW_inc(PRIV->p,4,HFLEN_WWW);

            FF_WWW_copy(p1,PRIV->p,HFLEN_WWW);
            FF_WWW_dec(p1,1,HFLEN_WWW);

            if (FF_WWW_cfactor(p1,e,HFLEN_WWW)) continue;
            break;
        }

        for (;;)
        {
            FF_WWW_random(PRIV->q,RNG,HFLEN_WWW);
            while (FF_WWW_lastbits(PRIV->q,2)!=3) FF_WWW_inc(PRIV->q,1,HFLEN_WWW);
            while (!FF_WWW_prime(PRIV->q,RNG,HFLEN_WWW))
                FF_WWW_inc(PRIV->q,4,HFLEN_WWW);

            FF_WWW_copy(q1,PRIV->q,HFLEN_WWW);
            FF_WWW_dec(q1,1,HFLEN_WWW);
            if (FF_WWW_cfactor(q1,e,HFLEN_WWW)) continue;

            break;
        }

    }
    else
    {
        FF_WWW_fromOctet(PRIV->p,P,HFLEN_WWW);
        FF_WWW_fromOctet(PRIV->q,Q,HFLEN_WWW);

        FF_WWW_copy(p1,PRIV->p,HFLEN_WWW);
        FF_WWW_dec(p1,1,HFLEN_WWW);

        FF_WWW_copy(q1,PRIV->q,HFLEN_WWW);
        FF_WWW_dec(q1,1,HFLEN_WWW);
    }

    FF_WWW_mul(PUB->n,PRIV->p,PRIV->q,HFLEN_WWW);
    PUB->e=e;

    FF_WWW_copy(t,p1,HFLEN_WWW);
    FF_WWW_shr(t,HFLEN_WWW);
    FF_WWW_init(PRIV->dp,e,HFLEN_WWW);
    FF_WWW_invmodp(PRIV->dp,PRIV->dp,t,HFLEN_WWW);
    if (FF_WWW_parity(PRIV->dp)==0) FF_WWW_add(PRIV->dp,PRIV->dp,t,HFLEN_WWW);
    FF_WWW_norm(PRIV->dp,HFLEN_WWW);

    FF_WWW_copy(t,q1,HFLEN_WWW);
    FF_WWW_shr(t,HFLEN_WWW);
    FF_WWW_init(PRIV->dq,e,HFLEN_WWW);
    FF_WWW_invmodp(PRIV->dq,PRIV->dq,t,HFLEN_WWW);
    if (FF_WWW_parity(PRIV->dq)==0) FF_WWW_add(PRIV->dq,PRIV->dq,t,HFLEN_WWW);
    FF_WWW_norm(PRIV->dq,HFLEN_WWW);

    FF_WWW_invmodp(PRIV->c,PRIV->p,PRIV->q,HFLEN_WWW);

    return;
}

/* destroy the Private Key structure */
void RSA_WWW_PRIVATE_KEY_KILL(rsa_private_key_WWW *PRIV)
{
    FF_WWW_zero(PRIV->p,HFLEN_WWW);
    FF_WWW_zero(PRIV->q,HFLEN_WWW);
    FF_WWW_zero(PRIV->dp,HFLEN_WWW);
    FF_WWW_zero(PRIV->dq,HFLEN_WWW);
    FF_WWW_zero(PRIV->c,HFLEN_WWW);
}

void RSA_WWW_fromOctet(BIG_XXX x[],octet *w)
{
    FF_WWW_fromOctet(x,w,FFLEN_WWW);
}

/* RSA encryption with the public key */
void RSA_WWW_ENCRYPT(rsa_public_key_WWW *PUB,octet *F,octet *G)
{
    BIG_XXX f[FFLEN_WWW];
    FF_WWW_fromOctet(f,F,FFLEN_WWW);

    FF_WWW_power(f,f,PUB->e,PUB->n,FFLEN_WWW);

    FF_WWW_toOctet(G,f,FFLEN_WWW);
}

/* RSA decryption with the private key */
void RSA_WWW_DECRYPT(rsa_private_key_WWW *PRIV,octet *G,octet *F)
{
    BIG_XXX g[FFLEN_WWW],t[FFLEN_WWW],jp[HFLEN_WWW],jq[HFLEN_WWW];

    FF_WWW_fromOctet(g,G,FFLEN_WWW);

    FF_WWW_dmod(jp,g,PRIV->p,HFLEN_WWW);
    FF_WWW_dmod(jq,g,PRIV->q,HFLEN_WWW);

    FF_WWW_skpow(jp,jp,PRIV->dp,PRIV->p,HFLEN_WWW);
    FF_WWW_skpow(jq,jq,PRIV->dq,PRIV->q,HFLEN_WWW);


    FF_WWW_zero(g,FFLEN_WWW);
    FF_WWW_copy(g,jp,HFLEN_WWW);
    FF_WWW_mod(jp,PRIV->q,HFLEN_WWW);
    if (FF_WWW_comp(jp,jq,HFLEN_WWW)>0)
        FF_WWW_add(jq,jq,PRIV->q,HFLEN_WWW);
    FF_WWW_sub(jq,jq,jp,HFLEN_WWW);
    FF_WWW_norm(jq,HFLEN_WWW);

    FF_WWW_mul(t,PRIV->c,jq,HFLEN_WWW);
    FF_WWW_dmod(jq,t,PRIV->q,HFLEN_WWW);

    FF_WWW_mul(t,jq,PRIV->p,HFLEN_WWW);
    FF_WWW_add(g,t,g,FFLEN_WWW);
    FF_WWW_norm(g,FFLEN_WWW);

    FF_WWW_toOctet(F,g,FFLEN_WWW);

    return;
}

