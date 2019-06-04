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

using namespace XXX;

/* generate an RSA key pair */
void WWW::RSA_KEY_PAIR(csprng *RNG,sign32 e,rsa_private_key *PRIV,rsa_public_key *PUB,octet *P, octet* Q)
{
    /* IEEE1363 A16.11/A16.12 more or less */
    BIG t[HFLEN_WWW],p1[HFLEN_WWW],q1[HFLEN_WWW];

    if (RNG!=NULL)
    {

        for (;;)
        {

            FF_random(PRIV->p,RNG,HFLEN_WWW);
            while (FF_lastbits(PRIV->p,2)!=3) FF_inc(PRIV->p,1,HFLEN_WWW);
            while (!FF_prime(PRIV->p,RNG,HFLEN_WWW))
                FF_inc(PRIV->p,4,HFLEN_WWW);

            FF_copy(p1,PRIV->p,HFLEN_WWW);
            FF_dec(p1,1,HFLEN_WWW);

            if (FF_cfactor(p1,e,HFLEN_WWW)) continue;
            break;
        }

        for (;;)
        {
            FF_random(PRIV->q,RNG,HFLEN_WWW);
            while (FF_lastbits(PRIV->q,2)!=3) FF_inc(PRIV->q,1,HFLEN_WWW);
            while (!FF_prime(PRIV->q,RNG,HFLEN_WWW))
                FF_inc(PRIV->q,4,HFLEN_WWW);

            FF_copy(q1,PRIV->q,HFLEN_WWW);
            FF_dec(q1,1,HFLEN_WWW);
            if (FF_cfactor(q1,e,HFLEN_WWW)) continue;

            break;
        }

    }
    else
    {
        FF_fromOctet(PRIV->p,P,HFLEN_WWW);
        FF_fromOctet(PRIV->q,Q,HFLEN_WWW);

        FF_copy(p1,PRIV->p,HFLEN_WWW);
        FF_dec(p1,1,HFLEN_WWW);

        FF_copy(q1,PRIV->q,HFLEN_WWW);
        FF_dec(q1,1,HFLEN_WWW);
    }

    FF_mul(PUB->n,PRIV->p,PRIV->q,HFLEN_WWW);
    PUB->e=e;

    FF_copy(t,p1,HFLEN_WWW);
    FF_shr(t,HFLEN_WWW);
    FF_init(PRIV->dp,e,HFLEN_WWW);
    FF_invmodp(PRIV->dp,PRIV->dp,t,HFLEN_WWW);
    if (FF_parity(PRIV->dp)==0) FF_add(PRIV->dp,PRIV->dp,t,HFLEN_WWW);
    FF_norm(PRIV->dp,HFLEN_WWW);

    FF_copy(t,q1,HFLEN_WWW);
    FF_shr(t,HFLEN_WWW);
    FF_init(PRIV->dq,e,HFLEN_WWW);
    FF_invmodp(PRIV->dq,PRIV->dq,t,HFLEN_WWW);
    if (FF_parity(PRIV->dq)==0) FF_add(PRIV->dq,PRIV->dq,t,HFLEN_WWW);
    FF_norm(PRIV->dq,HFLEN_WWW);

    FF_invmodp(PRIV->c,PRIV->p,PRIV->q,HFLEN_WWW);

    return;
}

/* destroy the Private Key structure */
void WWW::RSA_PRIVATE_KEY_KILL(rsa_private_key *PRIV)
{
    FF_zero(PRIV->p,HFLEN_WWW);
    FF_zero(PRIV->q,HFLEN_WWW);
    FF_zero(PRIV->dp,HFLEN_WWW);
    FF_zero(PRIV->dq,HFLEN_WWW);
    FF_zero(PRIV->c,HFLEN_WWW);
}

void WWW::RSA_fromOctet(BIG x[],octet *w)
{
	FF_fromOctet(x,w,FFLEN_WWW);
}

/* RSA encryption with the public key */
void WWW::RSA_ENCRYPT(rsa_public_key *PUB,octet *F,octet *G)
{
    BIG f[FFLEN_WWW];
    FF_fromOctet(f,F,FFLEN_WWW);

    FF_power(f,f,PUB->e,PUB->n,FFLEN_WWW);

    FF_toOctet(G,f,FFLEN_WWW);
}

/* RSA decryption with the private key */
void WWW::RSA_DECRYPT(rsa_private_key *PRIV,octet *G,octet *F)
{
    BIG g[FFLEN_WWW],t[FFLEN_WWW],jp[HFLEN_WWW],jq[HFLEN_WWW];

    FF_fromOctet(g,G,FFLEN_WWW);

    FF_dmod(jp,g,PRIV->p,HFLEN_WWW);
    FF_dmod(jq,g,PRIV->q,HFLEN_WWW);

    FF_skpow(jp,jp,PRIV->dp,PRIV->p,HFLEN_WWW);
    FF_skpow(jq,jq,PRIV->dq,PRIV->q,HFLEN_WWW);


    FF_zero(g,FFLEN_WWW);
    FF_copy(g,jp,HFLEN_WWW);
    FF_mod(jp,PRIV->q,HFLEN_WWW);
    if (FF_comp(jp,jq,HFLEN_WWW)>0)
        FF_add(jq,jq,PRIV->q,HFLEN_WWW);
    FF_sub(jq,jq,jp,HFLEN_WWW);
    FF_norm(jq,HFLEN_WWW);

    FF_mul(t,PRIV->c,jq,HFLEN_WWW);
    FF_dmod(jq,t,PRIV->q,HFLEN_WWW);

    FF_mul(t,jq,PRIV->p,HFLEN_WWW);
    FF_add(g,t,g,FFLEN_WWW);
    FF_norm(g,FFLEN_WWW);

    FF_toOctet(F,g,FFLEN_WWW);

    return;
}

